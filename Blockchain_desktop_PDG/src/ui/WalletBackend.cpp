#include "ui/WalletBackend.hpp"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

#include "test/crypto.hpp"   // PubKey, getPubKey(...)

#include <openssl/pem.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#define emit

namespace {
    // Sauvegarde/chargement PEM
    bool savePrivateKeyPem(EVP_PKEY* pkey, const QString& file) {
        QFile f(file);
        if (!f.open(QIODevice::WriteOnly)) return false;
        BIO* bio = BIO_new(BIO_s_mem());
        bool ok = PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        if (!ok) { BIO_free(bio); return false; }
        BUF_MEM* mem = nullptr;
        BIO_get_mem_ptr(bio, &mem);
        f.write(mem->data, static_cast<qint64>(mem->length));
        BIO_free(bio);
        f.close();
        return true;
    }

    EVP_PKEY* loadPrivateKeyPem(const QString& file) {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly)) return nullptr;
        QByteArray pem = f.readAll();
        BIO* bio = BIO_new_mem_buf(pem.constData(), pem.size());
        EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        return pkey;
    }

    QString walletPath() {
        const auto base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(base);
        return base + "/wallet.key";
    }
} // namespace

WalletBackend::WalletBackend(QObject* parent)
: QObject(parent),
m_txModel(std::make_unique<TransactionListModel>())
{
    ensureKeyLoaded();
    startNetwork();   // démarrer réseau au lancement (tolère échec UPnP)
    startPolling();   // rafraîchit balance/bloc tant qu’on a pas de callbacks
}

WalletBackend::~WalletBackend() {
    stopPolling();
    stopNetwork();
    if (m_key) EVP_PKEY_free(m_key);
}

void WalletBackend::ensureKeyLoaded() {
    const auto path = walletPath();
    m_key = loadPrivateKeyPem(path);
    if (!m_key) {
        m_key = generateKeySecp256k1();
        if (!m_key) {
            qWarning() << "Failed to generate keypair";
            return;
        }
        if (!savePrivateKeyPem(m_key, path)) {
            qWarning() << "Failed to save wallet key to" << path;
        }
    }

    //Extrait la pubkey une seule fois et la mémorise
    try {
        PubKey pub = crypto::getPubKey(m_key);
        m_fromPubKey = pub; // cache
    } catch (const std::exception& e) {
        qWarning() << "getPubKey failed:" << e.what();
        m_fromPubKey.clear();
    } catch (...) {
        qWarning() << "getPubKey failed (unknown)";
        m_fromPubKey.clear();
    }

    m_chain.onBlockMined = [this](double reward){
        QMetaObject::invokeMethod(this, [this, reward]{
            TxRow row;
            row.txId = "Mining reward";
            row.amount = reward;
            row.currency = "SKBC";
            row.isReceive = true;
            row.timestamp = QDateTime::currentSecsSinceEpoch();
            m_txModel->append(row);
            // la balance sera rafraîchie par le polling
        }, Qt::QueuedConnection);
    };
}



EVP_PKEY* WalletBackend::generateKeySecp256k1() {
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!pctx) return nullptr;
    if (EVP_PKEY_keygen_init(pctx) <= 0) { EVP_PKEY_CTX_free(pctx); return nullptr; }
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp256k1) <= 0) { EVP_PKEY_CTX_free(pctx); return nullptr; }
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0) { EVP_PKEY_free(pkey); pkey = nullptr; }
    EVP_PKEY_CTX_free(pctx);
    return pkey;
}

QByteArray WalletBackend::evpPublicKeyDer(EVP_PKEY* pkey) {
    int len = i2d_PUBKEY(pkey, nullptr);
    QByteArray out;
    out.resize(len);
    unsigned char* p = reinterpret_cast<unsigned char*>(out.data());
    i2d_PUBKEY(pkey, &p);
    return out;
}

QString WalletBackend::publicAddressHex() const {
    return QString::fromStdString(m_fromPubKey);
}

void WalletBackend::startNetwork() {
    // Démarre le réseau; si UPnP indispo, l’impl renvoie false mais on laisse tourner
    try { m_chain.getNetwork().start(); } catch(...) {}
}

void WalletBackend::stopNetwork() {
    try { m_chain.getNetwork().stop(); } catch(...) {}
}

void WalletBackend::startPolling() {
    m_poll.setInterval(500); // 0,5 s : fluide sans être bruyant
    connect(&m_poll, &QTimer::timeout, this, &WalletBackend::refreshComputedProps);
    m_poll.start();
}

void WalletBackend::stopPolling() {
    m_poll.stop();
}

void WalletBackend::refreshComputedProps() {
    // Block count
    int bc = static_cast<int>(m_chain.size());
    if (bc != m_blockCount) {
        m_blockCount = bc;
        emit blockCountChanged();
    }

    // Balance : solde de NOTRE clé
    // Utilise la pubkey en cache
    if (!m_fromPubKey.empty()) {
        double b = m_chain.getWalletBalance(m_fromPubKey);
        if (b != m_balance) { m_balance = b; emit balanceChanged(); }
    }

    // Stubs (on branchera plus tard)
    // mineurs, hashrate, tps
    int miners = m_chain.getNetwork().peerCount();
    if (miners != m_mineurs) { m_mineurs = miners; emit mineursChanged(); }
    double hr = m_chain.getHashrate();
    if (hr != m_hashrate) { m_hashrate = hr; emit hashrateChanged(); }
    double tpsV = 0.0;
    if (tpsV != m_tps) { m_tps = tpsV; emit tpsChanged(); }
}

void WalletBackend::setIsMining(bool on) {
    if (m_isMining == on) return;
    m_isMining = on;
    emit isMiningChanged();

    if (m_fromPubKey.empty()) return;

    if (on) {
        // Mining en arrière-plan; on n’a pas encore de callback → on ajoutera une ligne
        // d’historique sur changement de balance (poll), c’est suffisant pour MVP.
        m_chain.doMine(m_fromPubKey);
    } else {
        m_chain.stopMining();
    }
}

void WalletBackend::sendTransaction(double amount, const QString& toAddress) {
    if (!m_key) { emit transactionSent(false, "Wallet not initialized"); return; }
    try {
        const PubKey fromPub = m_fromPubKey;
        const PubKey toPub = toAddress.isEmpty()
        ? crypto::getPubKey(m_key)               // self
        : toAddress.toStdString();               // MVP: on suppose l’adresse == PubKey string

        const double fee = 0.01;
        auto tx = Transaction::createWithFromPub(m_key, fromPub, toPub, amount, fee, m_chain);

        // Ajout + broadcast
        const bool ok = m_chain.addAndBroadCastTransaction(tx);
        if (!ok) {
            emit transactionSent(false, "Rejected by mempool");
            return;
        }

        // Historique UI immédiat (côté wallet)
        TxRow row;
        row.txId = QStringLiteral("0x%1").arg(QString::number(QDateTime::currentMSecsSinceEpoch(), 16));
        row.amount = amount;
        row.currency = "SKBC";
        row.isReceive = false;
        row.timestamp = QDateTime::currentSecsSinceEpoch();
        m_txModel->append(row);

        emit transactionSent(true, QString::number(amount));
    } catch (const std::exception& e) {
        emit transactionSent(false, e.what());
    } catch (...) {
        emit transactionSent(false, "Unknown error");
    }
}

void WalletBackend::receiveCoins() {
    // MVP: juste émettre l’adresse de réception (PubKey sous forme de string)
    const auto addr = publicAddressHex();
    emit addressGenerated(addr);
}
