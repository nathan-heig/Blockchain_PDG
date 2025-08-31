import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

Rectangle {
    id: root

    property string txId: ""
    property string amount: ""
    property string currency: ""
    property bool isReceive: true

    implicitHeight: 60
    color: Theme.backgroundColor
    radius: Theme.smallRadius

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // ID de transaction
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                text: root.txId
                font.pixelSize: 12
                color: Theme.secondaryTextColor
            }

            Label {
                text: root.amount + " " + root.currency
                font.pixelSize: 14
                font.weight: Font.Medium
                color: root.isReceive ? Theme.positiveColor : Theme.negativeColor
            }
        }
    }

    // Hover effect
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onEntered: parent.color = Qt.lighter(Theme.backgroundColor, 1.1)
        onExited: parent.color = Theme.backgroundColor
    }

    Behavior on color {
        ColorAnimation { duration: Theme.animationDuration }
    }
}
