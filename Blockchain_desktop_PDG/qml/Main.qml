import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects   // si tu utilises des effets

ApplicationWindow {
    visible: true
    width: 640; height: 360
    title: "Hello Qt Quick (CI)"
    Rectangle {
        anchors.fill: parent
        color: "#0e1220"
        Text {
            anchors.centerIn: parent
            text: "Hello!"
            color: "white"
            font.pixelSize: 28
        }
    }
}
