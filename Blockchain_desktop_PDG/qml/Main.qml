import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import "components"
import "styles"

ApplicationWindow {
    id: window
    visible: true
    width: 1200; height: 700
    minimumWidth: 800; minimumHeight: 600
    title: qsTr("Skibidi Coin")
    color: Theme.backgroundColor

    // NOTE: backend est fourni par C++ via contextProperty("backend", &backend)
    // => Pas de property var backend: ... ici

    // Petites aides UI
    function showInfo(msg) { console.log(msg) } // remplace un vrai toast/dialog si besoin

    Connections {
        target: backend

        function onTransactionSent(success, message) {
            if (success) {
                showInfo("Transaction sent: " + message + " SKBC")
            } else {
                showInfo("Transaction failed: " + message)
            }
        }

        function onBlockMined(reward) {
            showInfo("Block mined! +" + reward + " SKBC")
        }

        function onAddressGenerated(address) {
            showInfo("Receive address: " + address)
        }
        // function onBalanceChanged() { /* binding auto via Q_PROPERTY */ }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // --- Panel gauche (Stats) ---
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
                    font.pixelSize: 18; font.weight: Font.Medium
                    color: Theme.textColor
                    Layout.bottomMargin: 20
                }

                StatsItem {
                    iconSource: "qrc:/icons/miners.svg"
                    label: "Mineurs"
                    value: String(backend.mineurs)
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/hashrate.svg"
                    label: "Hashrate"
                    value: backend.hashrate.toFixed(2) + " MH/s"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/block.svg"
                    label: "Bloc"
                    value: "#" + String(backend.blockCount)
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/tps.svg"
                    label: "TPS"
                    value: backend.tps.toFixed(2)
                    Layout.fillWidth: true
                }

                Item { Layout.fillHeight: true }

                // Indicateur mining (optionnel)
                BusyIndicator {
                    running: backend.isMining
                    visible: running
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // --- Panel central (Balance + actions) ---
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.panelColor
                radius: Theme.radius

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 30
                    width: Math.min(420, parent.width * 0.8)

                    // Logo
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 80; height: 80; radius: 40
                        color: Theme.accentColor
                        Label {
                            anchors.centerIn: parent
                            text: "₿"
                            font.pixelSize: 40; font.weight: Font.Bold
                            color: Theme.backgroundColor
                        }
                    }

                    // Balance
                    ColumnLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 6
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Available Balance"
                            font.pixelSize: 14; color: Theme.secondaryTextColor
                        }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: backend.balance.toLocaleString(Qt.locale(), 'f', 2)
                            font.pixelSize: 48; font.weight: Font.Bold; color: Theme.textColor
                        }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "SKBC"
                            font.pixelSize: 16; color: Theme.secondaryTextColor
                        }
                    }

                    // Mining
                    MiningButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        isMining: backend.isMining
                        text: isMining ? "Arrêter le mining" : "Démarrer le mining"
                        onClicked: backend.setIsMining(!backend.isMining)
                    }

                    // Send / Receive
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Send 1.00 SKBC"
                            isPrimary: true
                            enabled: backend.balance >= 1.01
                            onClicked: backend.sendTransaction(1.0, "")
                        }

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Receive"
                            isPrimary: false
                            onClicked: backend.receiveCoins()
                        }
                    }
                }
            }
        }

        // --- Panel droit (Historique) ---
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
                    font.pixelSize: 18; font.weight: Font.Medium
                    color: Theme.textColor
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    // Si tu as un modèle C++: model: backend.txModel
                    // En attendant, tu peux laisser ton ListModel QML ou passer plus tard
                    ListView {
                        id: transactionList
                        model: backend.txModel   // <-- recommandé, côté C++
                        spacing: 10

                        delegate: TransactionItem {
                            width: transactionList.width
                            txId: model.txId
                            amount: (model.isReceive ? "+" : "-") + Number(model.amount).toFixed(2)
                            currency: model.currency
                            isReceive: model.isReceive
                        }
                    }
                }
            }
        }
    }
}
