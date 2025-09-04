import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

Item {
    id: root

    property string iconSource: ""
    property string label: ""
    property string value: ""

    implicitHeight: 60

    RowLayout {
        anchors.fill: parent
        spacing: 15

        // Icône
        Rectangle {
            width: 40
            height: 40
            radius: 8
            color: Qt.rgba(Theme.accentColor.r, Theme.accentColor.g, Theme.accentColor.b, 0.1)

            Image {
                anchors.centerIn: parent
                width: 24
                height: 24
                source: root.iconSource
                visible: root.iconSource !== ""

                // Fallback si pas d'icône
                Label {
                    anchors.centerIn: parent
                    text: root.label.charAt(0)
                    font.pixelSize: 16
                    font.weight: Font.Bold
                    color: Theme.accentColor
                    visible: root.iconSource === ""
                }
            }
        }

        // Texte
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                text: root.label
                font.pixelSize: 12
                color: Theme.secondaryTextColor
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
            }

            Label {
                text: root.value
                font.pixelSize: 16
                font.weight: Font.Medium
                color: Theme.textColor
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
            }
        }
    }
}
