import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import "components"
import "styles"

// Import pour le clipboard
import QtQuick.Window 2.15

ApplicationWindow {
    id: window
    visible: true
    width: 1200; height: 700
    minimumWidth: 800; minimumHeight: 600
    title: qsTr("Skibidi Coin")
    color: Theme.backgroundColor

    // NOTE: BlockchainAPI est fourni par C++ via contextProperty("BlockchainAPI", &facade)

    // TextEdit invisible pour le clipboard
    TextEdit {
        id: clipboardHelper
        visible: false
    }

    property alias toastVisible: toast.visible
    function showInfo(msg) {
        toast.text = msg
        toast.open()
    }
    
    function copyToClipboard(text) {
        clipboardHelper.text = text
        clipboardHelper.selectAll()
        clipboardHelper.copy()
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
        target: BlockchainAPI

        function onBlockCountChanged() {
            showInfo("Nouveau bloc ! Total: " + BlockchainAPI.blockCount)
        }

        function onMiningChanged() {
            showInfo(BlockchainAPI.mining ? "Mining démarré" : "Mining arrêté")
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
                    label: "Mining"
                    value: BlockchainAPI.mining ? "Actif" : "Inactif"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/block.svg"
                    label: "Blocs"
                    value: "#" + String(BlockchainAPI.blockCount)
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/tps.svg"
                    label: "Balance"
                    value: BlockchainAPI.balance.toFixed(2) + " SKBC"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/hashrate.svg" // ajouter l'icône si disponible
                    label: "Hashrate"
                    value: (BlockchainAPI.lastHashrate).toFixed(2) + " MH/s"
                    Layout.fillWidth: true
                }
                StatsItem {
                    iconSource: "qrc:/icons/speed.svg" // ajouter l'icône si disponible
                    label: "TPS"
                    value: (BlockchainAPI.lastTPS).toFixed(2)
                    Layout.fillWidth: true
                }

                Item { Layout.fillHeight: true }

                // Indicateur mining
                BusyIndicator {
                    running: BlockchainAPI.mining
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
                            text: BlockchainAPI.balance.toLocaleString(Qt.locale(), 'f', 2)
                            font.pixelSize: 48; font.weight: Font.Bold; color: Theme.textColor
                        }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "SKBC"
                            font.pixelSize: 16; color: Theme.secondaryTextColor
                        }
                    }

                    // Mining Button
                    MiningButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                    }

                    // Info mining
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: BlockchainAPI.mining ? "Mining en cours..." : "Cliquez pour démarrer le mining"
                        font.pixelSize: 12
                        color: Theme.secondaryTextColor
                    }

                    // Public Key avec bouton copier
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Ma clé publique"
                            font.pixelSize: 12
                            color: Theme.secondaryTextColor
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Rectangle {
                                Layout.fillWidth: true
                                height: 40
                                color: Theme.backgroundColor
                                radius: 8
                                border.color: Theme.accentColor
                                border.width: 1

                                ScrollView {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    clip: true

                                    Text {
                                        text: BlockchainAPI.publicKey
                                        color: Theme.textColor
                                        font.pixelSize: 10
                                        font.family: "monospace"
                                        wrapMode: Text.WrapAnywhere
                                        width: parent.width
                                    }
                                }
                            }

                            Button {
                                text: "📋"
                                font.pixelSize: 16
                                width: 40
                                height: 40
                                onClicked: {
                                    copyToClipboard(BlockchainAPI.publicKey)
                                    showInfo("Clé publique copiée")
                                }
                                
                                background: Rectangle {
                                    radius: 8
                                    color: parent.hovered ? Theme.accentColor : Theme.backgroundColor
                                    border.color: Theme.accentColor
                                    border.width: 1
                                }
                                
                                contentItem: Text {
                                    text: parent.text
                                    color: parent.hovered ? Theme.backgroundColor : Theme.textColor
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    // Section Envoyer Transaction
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Envoyer Transaction"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            color: Theme.textColor
                        }

                        // Champ destinataire
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: "Destinataire (clé publique)"
                                font.pixelSize: 11
                                color: Theme.secondaryTextColor
                            }

                            TextField {
                                id: destinataireField
                                Layout.fillWidth: true
                                placeholderText: "Entrez la clé publique du destinataire..."
                                font.pixelSize: 10
                                font.family: "monospace"
                                color: Theme.textColor
                                selectByMouse: true
                                
                                background: Rectangle {
                                    radius: 8
                                    color: Theme.backgroundColor
                                    border.color: Theme.accentColor
                                    border.width: 1
                                }
                            }
                        }

                        // Champ montant
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: "Montant (SKBC)"
                                font.pixelSize: 11
                                color: Theme.secondaryTextColor
                            }

                            TextField {
                                id: montantField
                                Layout.fillWidth: true
                                placeholderText: "0.00"
                                validator: DoubleValidator {
                                    bottom: 0.01
                                    decimals: 2
                                    notation: DoubleValidator.StandardNotation
                                }
                                font.pixelSize: 12
                                color: Theme.textColor
                                selectByMouse: true
                                
                                background: Rectangle {
                                    radius: 8
                                    color: Theme.backgroundColor
                                    border.color: Theme.accentColor
                                    border.width: 1
                                }
                            }
                        }

                        // Bouton Send
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            text: "Send Transaction"
                            enabled: destinataireField.text.length > 0 && 
                                    montantField.text.length > 0 && 
                                    parseFloat(montantField.text) > 0 &&
                                    parseFloat(montantField.text) <= BlockchainAPI.balance
                            
                            onClicked: {
                                var amount = parseFloat(montantField.text)
                                var recipient = destinataireField.text.trim()
                                
                                // Ici vous ajouterez votre logique d'envoi
                                BlockchainAPI.sendTransaction(recipient, amount)
                                console.log("Sending", amount, "SKBC to", recipient)
                                showInfo("Transaction créée: " + amount + " SKBC vers " + recipient.substring(0, 8) + "...")
                                
                                // Reset des champs après envoi
                                destinataireField.text = ""
                                montantField.text = ""
                            }
                            
                            background: Rectangle {
                                radius: 8
                                color: parent.enabled ? (parent.hovered ? "#065f46" : "#059669") : "#6b7280"
                                border.color: parent.enabled ? "#10b981" : "#9ca3af"
                                border.width: 1
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: parent.enabled ? "white" : "#d1d5db"
                                font.pixelSize: 14
                                font.weight: Font.Medium
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
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
                    text: "Transaction History"
                    font.pixelSize: 18; font.weight: Font.Medium
                    color: Theme.textColor
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: transactionList
                        model: BlockchainAPI.transactionCount
                        spacing: 10

                        // Ajout de ce gestionnaire pour défiler vers le bas
                        onCountChanged: {
                            if (count > 0) {
                                positionViewAtEnd()
                            }
                        }

                        delegate: Rectangle {
                            width: transactionList.width
                            height: 80
                            color: Theme.backgroundColor
                            radius: 8
                            border.color: Theme.accentColor
                            border.width: 1

                            Column {
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 4

                                Text {
                                    text: BlockchainAPI.getTransactionAt(index)
                                    color: Theme.textColor
                                    font.pixelSize: 11
                                    font.weight: Font.Medium
                                    wrapMode: Text.WordWrap
                                    width: parent.parent.width - 30
                                }

                                Text {
                                    text: "Confirmée"
                                    color: Theme.secondaryTextColor
                                    font.pixelSize: 9
                                }
                            }
                        }

                        // Message si aucune transaction - maintenant à l'intérieur de ListView
                        Label {
                            anchors.centerIn: parent
                            text: "Aucune transaction"
                            color: Theme.secondaryTextColor
                            font.pixelSize: 14
                            visible: transactionList.count === 0
                        }
                    }
                }
            }
        }
    }
}