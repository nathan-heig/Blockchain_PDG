import QtQuick
import QtQuick.Controls
import "../styles"

Button {
    id: root

    text: BlockchainAPI.mining ? "Stop Mining" : "Start Mining"
    hoverEnabled: true

    onClicked: {
        if (BlockchainAPI.mining)  // Changé ici
            BlockchainAPI.stopMining()
        else
            BlockchainAPI.startMining()
    }


    background: Rectangle {
        radius: Theme.radius
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: BlockchainAPI.mining ? "#92400e" : "#065f46" }  // Changé ici
            GradientStop { position: 1.0; color: BlockchainAPI.mining ? "#7c2d12" : "#064e3b" }  // Changé ici
        }
        border.color: Theme.accentColor
        border.width: 1
        opacity: root.hovered ? 0.9 : 1.0

        SequentialAnimation on opacity {
            running: BlockchainAPI.mining  // Changé ici
            loops: Animation.Infinite
            NumberAnimation { to: 0.7; duration: 900 }
            NumberAnimation { to: 1.0; duration: 900 }
        }
    }

    contentItem: Text {
        text: root.text
        color: Theme.accentColor
        font.pixelSize: 14
        font.weight: Font.Medium
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
    }
}
