#pragma once
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QString>
#include <memory>

#include "transaction/Transaction.hpp"
#include "Blockchain.hpp"
#include "ui/TransactionListModel.hpp"

#include <openssl/evp.h>

class WalletBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(double balance READ balance NOTIFY balanceChanged)
    Q_PROPERTY(bool isMining READ isMining WRITE setIsMining NOTIFY isMiningChanged)
    Q_PROPERTY(int blockCount READ blockCount NOTIFY blockCountChanged)
    Q_PROPERTY(double hashrate READ hashrate NOTIFY hashrateChanged)
    Q_PROPERTY(double tps READ tps NOTIFY tpsChanged)
    Q_PROPERTY(int mineurs READ mineurs NOTIFY mineursChanged)
    Q_PROPERTY(QObject* txModel READ txModel CONSTANT)

public:
    explicit WalletBackend(QObject* parent = nullptr);
    ~WalletBackend();

    // Props
    double balance() const { return m_balance; }
    bool isMining() const { return m_isMining; }
    int blockCount() const { return m_blockCount; }
    double hashrate() const { return m_hashrate; }   // stub pour l’instant
    double tps() const { return m_tps; }             // stub pour l’instant
    int mineurs() const { return m_mineurs; }        // stub pour l’instant

    QObject* txModel() const { return m_txModel.get(); }

    // Slots/Invokables
    Q_INVOKABLE void setIsMining(bool on);
    Q_INVOKABLE void sendTransaction(double amount, const QString& toAddress);
    Q_INVOKABLE void receiveCoins(); // émet l’adresse

    // Réseau
    Q_INVOKABLE void startNetwork();
    Q_INVOKABLE void stopNetwork();

signals:
    void transactionSent(bool success, QString message);
    void blockMined(double reward);
    void addressGenerated(QString address);
    void balanceChanged();
    void isMiningChanged();
    void blockCountChanged();
    void hashrateChanged();
    void tpsChanged();
    void mineursChanged();

private:
    // Helpers wallet/keys
    void ensureKeyLoaded();
    QString publicAddressHex() const;
    static EVP_PKEY* generateKeySecp256k1();
    static QByteArray evpPublicKeyDer(EVP_PKEY* pkey);

    // Polling pour rafraîchir UI tant que les callbacks ne sont pas branchés
    void startPolling();
    void stopPolling();
    void refreshComputedProps();

    // Conversion utilitaire
    static std::string qToStd(const QString& s) { return s.toStdString(); }

private:
    Blockchain m_chain;
    std::unique_ptr<TransactionListModel> m_txModel;
    EVP_PKEY* m_key = nullptr; // propriété du backend

    std::string m_fromPubKey;
    QTimer m_poll;
    double m_balance = 0.0;
    bool m_isMining = false;
    int m_blockCount = 0;
    double m_hashrate = 0.0;
    double m_tps = 0.0;
    int m_mineurs = 0;
};
