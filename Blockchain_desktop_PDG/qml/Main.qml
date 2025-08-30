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
    title: qsTr("SkibidiCoin")
    color: Theme.backgroundColor


    // Backend properties (a supprimer)
    property real availableBalance: 1335.42
    property string cryptoCurrency: "SKBC"
    property bool isMining: false

    // Stats properties (a supprimer)
    property int mineurs: 42
    property real hashrate: 12.34
    property int bloc: 123456
    property real tps: 3.20


    // Transaction history model (a supprimer)
    ListModel {
        id: transactionModel
        ListElement {
            txId: "0x12ab...89"
            amount: "+12.50"
            currency: "SKBC"
            type: "receive"
        }
        ListElement {
            txId: "0x98ff...aa"
            amount: "-2.00"
            currency: "SKBC"
            type: "send"
        }
        ListElement {
            txId: "0x77cd...4e"
            amount: "+0.80"
            currency: "SKBC"
            type: "receive"
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
                            window.isMining = !window.isMining
                            // Backend call here
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
                                // Backend call here
                                console.log("Send clicked")
                            }
                        }

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Receive"
                            isPrimary: false
                            onClicked: {
                                // Backend call here
                                console.log("Receive clicked")
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
                            amount: model.amount
                            currency: model.currency
                            isReceive: model.type === "receive"
                        }
                    }
                }
            }
        }
    }
}
