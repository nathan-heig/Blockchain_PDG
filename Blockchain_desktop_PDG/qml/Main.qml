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
    readonly property bool backendReady: (typeof backend !== "undefined" && backend !== null)


    // NOTE: backend est fourni par C++ via contextProperty("backend", &backend)

    property alias toastVisible: toast.visible
    function showInfo(msg) {
        toast.text = msg
        toast.open()
    }

    Popup {
        id: toast
        x: (parent.width - width)/2
        y: parent.height - height - 30
        modal: false
        focus: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        contentItem: Rectangle {
            color: Theme.panelColor
            radius: 8
            Text {
                id: toastText
                text: toast.text
                color: Theme.textColor
                font.pixelSize: 14
            }
        }
        property string text: ""
        onVisibleChanged: if (visible) Qt.createQmlObject('import QtQuick 2.15; Timer { interval: 2000; running: true; repeat: false; onTriggered: toast.close(); }', toast, "ToastTimer")
    }

    Connections {
        target: backendReady ? backend : null

        function onTransactionSent(success, message) {
            if (success) showInfo("Transaction envoyée: " + message + " SKBC")
            else showInfo("Échec de l’envoi: " + message)
        }

        function onBlockMined(reward) {
            showInfo("Bloc miné ! +" + reward + " SKBC")
        }

        function onAddressGenerated(address) {
            showInfo("Adresse: " + address)
        }

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
                    value: backendReady ? String(backend.mineurs) : "0"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/hashrate.svg"
                    label: "Hashrate"
                    value: backendReady ? backend.hashrate.toFixed(2) + " MH/s" : "0.00 MH/s"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/block.svg"
                    label: "Bloc"
                    value: "#" + (backendReady ? String(backend.blockCount) : "0")
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/tps.svg"
                    label: "TPS"
                    value: backendReady ? backend.tps.toFixed(2) : "0.00"
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

        // Panel central - Balance et actions
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
                            text: backendReady ? backend.balance.toLocaleString(Qt.locale(), 'f', 2) : "0.00"
                            font.pixelSize: 48; font.weight: Font.Bold; color: Theme.textColor
                        }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Spendable: " + (backendReady ? backend.spendableBalance.toLocaleString(Qt.locale(), 'f', 2) : "0.00") + " SKBC"
                            font.pixelSize: 12; color: Theme.secondaryTextColor
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
                        isMining: backendReady && backend.isMining
                        text: isMining ? "Arrêter le mining" : "Démarrer le mining"
                        onClicked: if (backendReady) backend.setIsMining(!backend.isMining)
                    }

                    // Mon adresse + copier
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        TextField {
                            id: myAddr
                            Layout.fillWidth: true
                            readOnly: true
                            text: backendReady ? backend.myAddress : ""
                            placeholderText: "Mon adresse"
                            selectByMouse: true
                        }
                        Button {
                            text: "Copier"
                            enabled: backendReady && myAddr.text.length > 0
                            onClicked: {
                                Qt.application.clipboard.setText(myAddr.text)
                                showInfo("Adresse copiée")
                            }
                        }
                    }

                    // Adresse destinataire (ID visible pour le bouton ci-dessous)
                    TextField {
                        id: dest
                        Layout.fillWidth: true
                        placeholderText: "Adresse (pubkey) du destinataire"
                        selectByMouse: true
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
                            enabled: backendReady
                                && backend.spendableBalance >= 1.01
                                && dest.text.length > 0
                                && backend.isAddressValid(dest.text)
                            onClicked: {
                                if (!backendReady) return
                                backend.sendTransaction(1.0, dest.text)
                            }
                        }

                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: "Receive"
                            isPrimary: false
                            onClicked: {
                                if (!backendReady) return
                                backend.receiveCoins()
                                showInfo("Votre adresse est affichée ci-dessus")
                            }
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

                    ListView {
                        id: transactionList
                        model: backendReady ? backend.txModel : null
                        spacing: 10

                        delegate: TransactionItem {
                            width: transactionList.width
                            txId: model.txId
                            amount: (model.isReceive ? "+" : "-") + Number(model.amount).toFixed(2)
                            currency: model.currency
                            isReceive: model.isReceive
                            status: model.status
                        }
                    }
                }
            }
        }
    }
}
