import QtQuick 2.2
import Polka 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../themes"

Rectangle {
    id: container
    height: message.contentHeight + 60
    border.width: 3
    radius: 8
    state: "hide"

    TextEdit {
        id: message
        anchors.fill: parent
        anchors.margins: 25
        wrapMode: TextEdit.Wrap
        font.pixelSize: 14
        font.bold: true
        renderType: Text.NativeRendering
        selectByMouse: true
    }

    IconButton {
        tip: "close"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Style.margin.smaller
        source: "../../image/dark/icons/image-remove.png"
        onClicked: container.state = "hide"
    }

    function error(str) {
        message.text = str;
        container.state = "error";
    }

    function hint(str) {
        message.text = str;
        container.state = "hint";
    }

    function help(str) {
        message.text = str;
        container.state = "help";
    }

    states: [
        State {
            name: "hide"
            PropertyChanges {
                target: container
                visible: false
            }
        },
        State {
            name: "hint"
            PropertyChanges {
                target: container
                color: "#78aa78"
                border.color: "#008200"
                visible: true
            }
            PropertyChanges {
                target: message
                color: "black"
            }
            PropertyChanges {
                target: timer
                interval: 3000
                running: true
            }
        },
        State {
            name: "help"
            PropertyChanges {
                target: container
                color: "#78aa78"
                border.color: "#008200"
                visible: true
            }
            PropertyChanges {
                target: message
                color: "black"
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: container
                color: "#ffbeae"
                border.color: "#ff4b4b"
                visible: true
            }
            PropertyChanges {
                target: message
                color: "black"
            }
            PropertyChanges {
                target: timer
                interval: 3000
                running: true
            }
        }
    ]
    Timer {
        id: timer
        running: false
        repeat: false
        onTriggered: {
            container.state = "hide";
            timer.running = false;
        }
    }
}
