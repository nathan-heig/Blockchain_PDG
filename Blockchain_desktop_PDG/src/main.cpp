#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFileInfo>

#include "Blockchain.hpp"
#include "ui/BlockchainFacade.hpp"
#include "cryptography/crypto.hpp"

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

    EVP_PKEY* privKey = nullptr;
    
    // Essayer de charger la clé si le fichier existe
    if (QFileInfo::exists(keyPath)) {
        try {
            privKey = crypto::getPrivateKey(keyPath.toStdString());
        } catch (const std::exception& e) {
            qWarning() << "Impossible de charger la clé privée existante:" << e.what() << ". Une nouvelle clé sera générée.";
            privKey = nullptr; // S'assurer que la clé est nulle si le chargement échoue
        }
    }

    if (!privKey) {
        qInfo() << "Aucune clé de portefeuille valide trouvée. Génération d'une nouvelle clé...";
        try {
            privKey = crypto::createPrivateKey();
            if (privKey) {
                // Créer le répertoire si nécessaire
                QDir dir = QFileInfo(keyPath).dir();
                if (!dir.exists()) {
                    if (!dir.mkpath(".")) {
                        qCritical() << "ERREUR: Impossible de créer le répertoire de configuration:" << dir.path();
                        if(privKey) EVP_PKEY_free(privKey);
                        return -1;
                    }
                }
                crypto::savePrivateKey(privKey, keyPath.toStdString());
                qInfo() << "Nouvelle clé de portefeuille générée et sauvegardée avec succès.";
            } else {
                qCritical() << "ERREUR: Impossible de générer la clé privée.";
                return -1;
            }
        } catch (const std::exception& e) {
            qCritical() << "ERREUR lors de la création/sauvegarde de la clé:" << e.what();
            if(privKey) EVP_PKEY_free(privKey);
            return -1; // Erreur critique
        }
    } else {
        qInfo() << "Clé de portefeuille existante chargée avec succès.";
    }

    static Blockchain blockchain;

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
        if (privKey) {
            EVP_PKEY_free(privKey);
        }
    });

    return app.exec();
}