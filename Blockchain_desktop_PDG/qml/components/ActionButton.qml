import QtQuick 2.15
import QtQuick.Controls 2.15
import "../styles"

Button {
    id: root

    property bool isPrimary: true
    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radius
        color: isPrimary ? Theme.primaryButtonColor : Theme.secondaryButtonColor
        opacity: root.hovered ? 0.9 : 1.0

        Behavior on opacity {
            NumberAnimation { duration: Theme.animationDuration }
        }

        // Effet de clic
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "white"
            opacity: root.pressed ? 0.2 : 0

            Behavior on opacity {
                NumberAnimation { duration: 100 }
            }
        }
    }

    contentItem: Text {
        text: root.text
        color: Theme.textColor
        font.pixelSize: 16
        font.weight: Font.Medium
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
