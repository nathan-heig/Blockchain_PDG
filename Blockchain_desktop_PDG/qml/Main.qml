import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import "components"
import "styles"

ApplicationWindow {
    id: window
    visible: true
    width: 1200
    height: 700
    minimumWidth: 800
    minimumHeight: 600
    title: qsTr("Skibidi Coin")
    color: Theme.backgroundColor

    // Connexion au backend blockchain
    property var backend: blockchain

    // Properties liées au backend
    property real availableBalance: backend ? backend.balance : 0
    property string cryptoCurrency: "SKBC"
    property bool isMining: backend ? backend.isMining : false

    // Stats properties
    property int mineurs: backend ? backend.mineurs : 0
    property real hashrate: backend ? backend.hashrate : 0
    property int bloc: backend ? backend.blockCount : 0
    property real tps: backend ? backend.tps : 0

    // Transaction history model local (pourrait être amélioré avec un model C++)
    ListModel {
        id: transactionModel
    }

    // Connexions aux signaux du backend
    Connections {
        target: backend

        function onTransactionSent(success, message) {
            if (success) {
                // Ajouter à l'historique
                transactionModel.insert(0, {
                    "txId": "0x" + Date.now().toString(16).slice(-8) + "...",
                    "amount": parseFloat(message),
                    "currency": "SKBC",
                    "isReceive": false
                })
                console.log("Transaction sent successfully:", message, "SKBC")
            } else {
                console.error("Transaction failed:", message)
            }
        }

        function onBlockMined(reward) {
            // Ajouter la récompense de mining à l'historique
            transactionModel.insert(0, {
                "txId": "Mining reward",
                "amount": reward,
                "currency": "SKBC",
                "isReceive": true
            })
            console.log("Block mined! Reward:", reward, "SKBC")
        }

        function onAddressGenerated(address) {
            console.log("Your receive address:", address)
            // Pourrait afficher dans un dialog
        }

        function onBalanceChanged() {
            // La balance se met à jour automatiquement via le binding
            console.log("Balance updated:", backend.balance, "SKBC")
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Panel gauche - Stats
        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            color: Theme.panelColor
            radius: Theme.radius

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                Label {
                    text: "Stats"
                    font.pixelSize: 18
                    font.weight: Font.Medium
                    color: Theme.textColor
                    Layout.bottomMargin: 20
                }

                StatsItem {
                    iconSource: "qrc:/icons/miners.svg"
                    label: "Mineurs"
                    value: window.mineurs.toString()
                    Layout.fillWidth: true
                }

                StatsItem {
                    iconSource: "qrc:/icons/hashrate.svg"
                    label: "Hashrate"
                    value: window.hashrate.toFixed(2) + " MH/s"
                    Layout.fillWidth: true
                }

                StatsItem {
                    iconSource: "qrc:/icons/block.svg"
                    label: "Bloc"
                    value: "#" + window.bloc.toString()
                    Layout.fillWidth: true
                }

                StatsItem {
                    iconSource: "qrc:/icons/tps.svg"
                    label: "TPS"
                    value: window.tps.toFixed(2)
                    Layout.fillWidth: true
                }

                Item { Layout.fillHeight: true }
            }
        }

        // Panel central - Balance et actions
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // Carte de balance
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.panelColor
                radius: Theme.radius

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 30
                    width: Math.min(400, parent.width * 0.8)

                    // Logo
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 80
                        height: 80
                        radius: 40
                        color: Theme.accentColor

                        Label {
                            anchors.centerIn: parent
                            text: "₿"
                            font.pixelSize: 40
                            font.weight: Font.Bold
                            color: Theme.backgroundColor
                        }
                    }

                    // Balance
                    ColumnLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 10

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Available Balance"
                            font.pixelSize: 14
                            color: Theme.secondaryTextColor
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: window.availableBalance.toLocaleString(Qt.locale(), 'f', 2)
                            font.pixelSize: 48
                            font.weight: Font.Bold
                            color: Theme.textColor
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: window.cryptoCurrency
                            font.pixelSize: 16
                            color: Theme.secondaryTextColor
                        }
                    }

                    // Bouton Mining
                    MiningButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        isMining: window.isMining
                        onClicked: {
                            if (backend) {
                                backend.isMining = !backend.isMining
                            }
                        }
                    }

                    // Boutons Send/Receive
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Send"
                            isPrimary: true
                            onClicked: {
                                // Pour la démo, envoyer 1 SKBC à une adresse générée
                                // Dans une vraie app, il faudrait un dialog pour entrer l'adresse
                                if (backend && window.availableBalance >= 1.01) { // 1 SKBC + 0.01 fees
                                    backend.sendTransaction(1.0, "")
                                    console.log("Sending 1 SKBC...")
                                } else {
                                    console.log("Insufficient balance. Need at least 1.01 SKBC")
                                }
                            }
                        }

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Receive"
                            isPrimary: false
                            onClicked: {
                                if (backend) {
                                    backend.receiveCoins()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Panel droit - Historique
    Rectangle {
        Layout.preferredWidth: 350
        Layout.fillHeight: true
        color: Theme.panelColor
        radius: Theme.radius

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Label {
                text: "Transaction history"
                font.pixelSize: 18
                font.weight: Font.Medium
                color: Theme.textColor
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ListView {
                    id: transactionList
                    model: transactionModel
                    spacing: 10

                    delegate: TransactionItem {
                        width: transactionList.width
                        txId: model.txId
                        amount: (model.isReceive ? "+" : "-") + model.amount.toFixed(2)
                        currency: model.currency
                        isReceive: model.isReceive
                    }
                }
            }
        }
    }
}