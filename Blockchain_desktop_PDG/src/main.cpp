#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Blockchain.hpp"
#include "ui/BlockchainFacade.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    static Blockchain blockchain;

    blockchain.getNetwork().start(8186);
    blockchain.getNetwork().connect(PeerInfo("127.0.0.1", 8185));

    EVP_PKEY* privKey = crypto::createPrivateKey(); // clé privée du wallet
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