import QtQuick 2.2
import Polka 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../themes"

Rectangle {
    id: container
    height: 75
    border.width: 10
    radius: 30
    state: "hide"
    states: [
        State {
            name: "hide"
            PropertyChanges {
                target: container
                visible: false
            }
        },
        State {
            name: "resize"
            PropertyChanges {
                target: container
                width: 130
                color: "#bedcff"
                border.color: "#4b96ff"
                visible: true
            }
            PropertyChanges {
                target: message
                text: "100%"
            }
        },
        State {
            name: "processing"
            PropertyChanges {
                target: container
                width: 160
                color: "#ffc878"
                border.color: "#ff8c00"
                visible: true
            }
            PropertyChanges {
                target: message
                text: "Processing"
            }
        },
        State {
            name: "loading"
            PropertyChanges {
                target: container
                width: 160
                color: "#ffc878"
                border.color: "#ff8c00"
                visible: true
            }
            PropertyChanges {
                target: message
                text: "Loading"
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: container
                width: 130
                color: "#ffbebe"
                border.color: "#ff4b4b"
                visible: true
            }
            PropertyChanges {
                target: message
                text: "Error"
            }
            PropertyChanges {
                target: timer
                interval: 2000
                running: true
            }
        },
        State {
            name: "done"
            PropertyChanges {
                target: container
                width: 130
                color: "#78aa78"
                border.color: "#008200"
                visible: true
            }
            PropertyChanges {
                target: message
                text: "Done"
            }
            PropertyChanges {
                target: timer
                interval: 1000
                running: true
            }
        }
    ]
    Text {
        id: message
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: parent.border.color
        font.pixelSize: 19
        font.bold: true
        renderType: Text.NativeRendering
    }
    Timer {
        id: timer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            container.state = "hide";
            timer.running = false;
        }
    }
}
