#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDirIterator>
#include <QDebug>

// Inclure ici les classes backend
// #include "WalletBackend.h"
// #include "TransactionModel.h"
// #include "MiningController.h"
#include <Blockchain.hpp>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Configuration de l'application
    app.setOrganizationName("SkibidiCoin");
    app.setOrganizationDomain("SkibidiCoin.com");
    app.setApplicationName("SkibidiCoin");

    QQmlApplicationEngine engine;

    // Enregistrement des types C++ pour QML
    // qmlRegisterType<WalletBackend>("Backend", 1, 0, "WalletBackend");
    // qmlRegisterType<TransactionModel>("Backend", 1, 0, "TransactionModel");
    // qmlRegisterType<MiningController>("Backend", 1, 0, "MiningController");

    // Création des instances backend
    // WalletBackend walletBackend;
    // TransactionModel transactionModel;
    // MiningController miningController;

    // Exposition au contexte QML
    // engine.rootContext()->setContextProperty("walletBackend", &walletBackend);
    // engine.rootContext()->setContextProperty("transactionModel", &transactionModel);
    // engine.rootContext()->setContextProperty("miningController", &miningController);

    // Enregistrement du singleton Theme
    qmlRegisterSingletonType(QUrl("qrc:/styles/Theme.qml"), "Styles", 1, 0, "Theme");

    // Debug: Vérifier les ressources disponibles
    QDirIterator it(":", QDirIterator::Subdirectories);
    qDebug() << "Resources disponibles:";
    while (it.hasNext()) {
        qDebug() << it.next();
    }

    const QUrl url(u"qrc:/main.qml"_qs);
    qDebug() << "Tentative de chargement:" << url;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}