#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>
#include <iostream>

#include "Blockchain.hpp"
#include "ui/BlockchainFacade.hpp"

int main(int argc, char *argv[])
{
    // Parse command line arguments first to determine if GUI is needed
    QCoreApplication tempApp(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("Skibidi Coin Blockchain");
    parser.addHelpOption();
    
    QCommandLineOption noGuiOption(QStringList() << "no-gui" << "n",
                                   "Run without GUI interface");
    parser.addOption(noGuiOption);
    
    parser.process(tempApp);
    bool useGui = !parser.isSet(noGuiOption);
    
    // Create appropriate application type
    QCoreApplication* app;
    if (useGui) {
        app = new QGuiApplication(argc, argv);
    } else {
        app = new QCoreApplication(argc, argv);
    }

    static Blockchain blockchain;

    blockchain.getNetwork().start(8186);
    blockchain.getNetwork().connect(PeerInfo("127.0.0.1", 8185));

    EVP_PKEY* privKey = crypto::createPrivateKey();
    static BlockchainFacade blockchainFacade(blockchain, privKey);

    if (useGui) {
        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("BlockchainAPI", &blockchainFacade);

        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == url)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

        engine.load(url);
        if (engine.rootObjects().isEmpty()) {
            delete app;
            return -1;
        }
    } else {
        std::cout << "Skibidi Coin running in headless mode..." << std::endl;
        std::cout << "Network started on port 8186" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
        
        // Optionally start mining automatically in headless mode
        // blockchainFacade.startMining();
    }

    int result = app->exec();
    delete app;
    return result;
}