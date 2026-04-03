import QtQuick 2.15
import QtQuick.Window 2.15
import GodotQt 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: "Godot-Qt QML Application"

    GodotVKItem {
        id: godotVKItem
        anchors.fill: parent
    }

    Text {
        id: text
        anchors.centerIn: parent
        text: "Hello from QML!"
        font.pointSize: 24
        color: '#a34444'
    }
}
