pragma Singleton
import QtQuick 2.15

QtObject {
    // Couleurs principales
    readonly property color backgroundColor: "#1a1f2e"
    readonly property color panelColor: "#242938"
    readonly property color accentColor: "#00d4aa"
    readonly property color primaryButtonColor: "#8b5cf6"
    readonly property color secondaryButtonColor: "#374151"

    // Couleurs de texte
    readonly property color textColor: "#ffffff"
    readonly property color secondaryTextColor: "#9ca3af"

    // Couleurs d'état
    readonly property color positiveColor: "#10b981"
    readonly property color negativeColor: "#ef4444"
    readonly property color warningColor: "#f59e0b"

    // Dimensions
    readonly property int radius: 12
    readonly property int smallRadius: 8

    // Animations
    readonly property int animationDuration: 200
}
