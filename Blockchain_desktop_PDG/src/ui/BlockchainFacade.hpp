#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include "Blockchain.hpp"

class BlockchainFacade : public QObject {
    Q_OBJECT
    Q_PROPERTY(uint32_t blockCount READ blockCount NOTIFY blockCountChanged)
    Q_PROPERTY(bool mining READ isMining NOTIFY miningChanged)
    Q_PROPERTY(double balance READ walletBalance NOTIFY blockCountChanged)
    Q_PROPERTY(int transactionCount READ getTransactionCount NOTIFY blockCountChanged)
    Q_PROPERTY(QString publicKey READ getPublicKeyString CONSTANT)
    Q_PROPERTY(double lastHashrate READ getLastHashrateMHs NOTIFY periodicUpdate)
    Q_PROPERTY(double lastTPS READ getLastTPS NOTIFY periodicUpdate)

public:
    explicit BlockchainFacade(Blockchain& chain, EVP_PKEY* privKey, QObject* parent=nullptr)
    : QObject(parent), m_chain(chain), walletTransactions(), privKey(privKey), m_periodicTimer(new QTimer(this))
    {

        minerPubKey = crypto::getPubKey(privKey);
        // callback core -> signal Qt (thread-safe via QueuedConnection)
        m_chain.setOnNewBlock([this](const Block& block) {

            for (size_t i = 0; i < block.getBlockTransactions().size(); ++i) {
                if (block.getBlockTransactions()[i].isInTransaction(minerPubKey, m_chain)) {
                    walletTransactions.push_back(block.getBlockTransactions()[i]);
                }
            }
            //std::cout << "[C++] New block accepted. Total blocks: " << m_chain.size() << std::endl;
            QMetaObject::invokeMethod(this, [this]{
                emit blockCountChanged();
            }, Qt::QueuedConnection);
        });

    // Signal périodique toutes les 5 secondes
    m_periodicTimer->setInterval(5000);
    connect(m_periodicTimer, &QTimer::timeout, this, &BlockchainFacade::periodicUpdate);
    m_periodicTimer->start();
    }

    Q_INVOKABLE void startMining() {
        if (m_chain.isMining()) return;
        m_chain.doMine(minerPubKey);
        emit miningChanged();
    }
    Q_INVOKABLE void stopMining() {
        if (!m_chain.isMining()) return;
        m_chain.stopMining();
        emit miningChanged();
    }
    Q_INVOKABLE double walletBalance() const {
        return m_chain.getWalletBalance(minerPubKey);
    }
    Q_INVOKABLE QString getTransactionAt(int index) const {
        if (index >= 0 && index < walletTransactions.size()) {
            return QString::fromStdString(walletTransactions[index].getTransactionWalletStr(minerPubKey, m_chain));
        }
        return "";
    }
    Q_INVOKABLE void sendTransaction(const QString& toPubKey, double amount) {
        std::cout << "[C++] Sending " << amount << " SKBC to " << toPubKey.toStdString() << std::endl;
        try {
            Transaction tx = Transaction::createWithFromPub(privKey, minerPubKey, toPubKey.toStdString(), amount, 0.0, m_chain);
            m_chain.addAndBroadCastTransaction(tx);
        } catch (const std::exception& e) {
            std::cerr << "Error creating transaction: " << e.what() << std::endl;
        }
    }
    Q_INVOKABLE double getLastHashrateMHs() const {
        return m_chain.getLastHashrateMHs();
    }
    Q_INVOKABLE double getLastTPS() const {
        return m_chain.getLastTPS();
    }

    QString getPublicKeyString() const {
        return QString::fromStdString(minerPubKey);
    }



    bool isMining() const { return m_chain.isMining(); }
    uint32_t blockCount() const { return m_chain.size(); }
    int getTransactionCount() const { return walletTransactions.size(); }

signals:
    void blockCountChanged();
    void miningChanged();
    void periodicUpdate(); // émis toutes les 5 secondes

private:
    Blockchain& m_chain;
    EVP_PKEY* privKey; // clé privée du wallet
    PubKey minerPubKey;
    std::vector<Transaction> walletTransactions;
    QTimer* m_periodicTimer; // timer pour emission périodique
};