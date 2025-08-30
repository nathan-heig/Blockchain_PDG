import QtQuick 2.15
import QtQuick.Controls 2.15
import "../styles"

Button {
    id: root
    property bool isMining: false
    text: isMining ? "Mining en cours..." : "Mining en cours..." //a modifier

    background: Rectangle {
        radius: Theme.radius
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#065f46" }
            GradientStop { position: 1.0; color: "#064e3b" }
        }
        border.color: Theme.accentColor
        border.width: 1
        opacity: root.hovered ? 0.9 : 1.0

        Behavior on opacity {
            NumberAnimation { duration: Theme.animationDuration }
        }

        // Animation de pulsation quand mining actif
        SequentialAnimation on opacity {
            running: root.isMining
            loops: Animation.Infinite
            NumberAnimation { to: 0.7; duration: 1000 }
            NumberAnimation { to: 1.0; duration: 1000 }
        }
    }

    contentItem: Text {
        text: root.text
        color: Theme.accentColor
        font.pixelSize: 14
        font.weight: Font.Medium
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: mouse.accepted = false
    }
}
