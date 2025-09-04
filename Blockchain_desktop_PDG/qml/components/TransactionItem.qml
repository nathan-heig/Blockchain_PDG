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
    property int status: 1  // 0=pending, 1=confirmed

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

            RowLayout {
                spacing: 8
                Label {
                    text: root.isReceive ? "Receive" : "Send"
                    font.pixelSize: 11
                    color: Theme.secondaryTextColor
                }
                Rectangle {
                    radius: 4
                    color: status === 1 ? Qt.rgba(0, 0.6, 0.4, 0.25) : Qt.rgba(1, 0.7, 0, 0.25)
                    border.color: status === 1 ? "#10b981" : "#f59e0b"
                    border.width: 1
                    anchors.verticalCenter: parent.verticalCenter
                    height: 18
                    width: implicitWidth
                    Text {
                        anchors.centerIn: parent
                        text: status === 1 ? "Confirmed" : "Pending"
                        font.pixelSize: 10
                        color: status === 1 ? "#10b981" : "#f59e0b"
                    }
                }
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
