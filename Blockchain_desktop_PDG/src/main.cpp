#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include <openssl/pem.h>
#include <openssl/evp.h>

#include "Blockchain.hpp"
#include "ui/BlockchainFacade.hpp"



// Fonction pour charger la clé privée depuis un fichier
EVP_PKEY* loadPrivateKey(const QString& path) {
    QByteArray pathBytes = path.toUtf8();
    FILE* fp = fopen(pathBytes.constData(), "r");
    if (!fp) {
        qWarning() << "Impossible d'ouvrir le fichier de clé privée pour lecture:" << path;
        return nullptr;
    }
    EVP_PKEY* pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!pkey) {
        qWarning() << "Impossible de lire la clé privée depuis le fichier:" << path;
    }
    return pkey;
}

// Fonction pour sauvegarder la clé privée dans un fichier
bool savePrivateKey(EVP_PKEY* pkey, const QString& path) {
    QDir dir = QFileInfo(path).dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Impossible de créer le répertoire de configuration:" << dir.path();
            return false;
        }
    }

    QByteArray pathBytes = path.toUtf8();
    FILE* fp = fopen(pathBytes.constData(), "w");
    if (!fp) {
        qWarning() << "Impossible d'ouvrir le fichier de clé privée pour écriture:" << path;
        return false;
    }
    bool success = PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
    fclose(fp);
    if (!success) {
        qWarning() << "Impossible d'écrire la clé privée dans le fichier:" << path;
    }
    return success;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Important pour QStandardPaths
    app.setOrganizationName("SkibidiCorp");
    app.setApplicationName("SkibidiCoin");

    // Déterminer le chemin du fichier de la clé privée
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString keyPath = configPath + "/wallet.pem";
    qDebug() << "Chemin de la clé du portefeuille:" << keyPath;

    EVP_PKEY* privKey = loadPrivateKey(keyPath);

    if (!privKey) {
        qInfo() << "Aucune clé de portefeuille existante trouvée. Génération d'une nouvelle clé...";
        privKey = crypto::createPrivateKey();
        if (privKey) {
            if (savePrivateKey(privKey, keyPath)) {
                qInfo() << "Nouvelle clé de portefeuille générée et sauvegardée avec succès.";
            } else {
                qCritical() << "ERREUR: Impossible de sauvegarder la nouvelle clé de portefeuille. L'application ne peut pas continuer en toute sécurité.";
                return -1; // Erreur critique
            }
        } else {
            qCritical() << "ERREUR: Impossible de générer la clé privée.";
            return -1;
        }
    } else {
        qInfo() << "Clé de portefeuille existante chargée avec succès.";
    }

    static Blockchain blockchain;

    blockchain.getNetwork().start(8186);
    blockchain.getNetwork().connect(PeerInfo("127.0.0.1", 8185));

    static BlockchainFacade blockchainFacade(blockchain, privKey);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("BlockchainAPI", &blockchainFacade);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && objUrl == url)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    if (engine.rootObjects().isEmpty())
        return -1;
    
    QObject::connect(&app, &QGuiApplication::aboutToQuit, [&]() {
        blockchain.stopMining();
        blockchain.getNetwork().stop();
        EVP_PKEY_free(privKey);
    });

    return app.exec();
}