import QtQuick 2.2
import "../themes"

Row {
    id: container
    property alias caption: caption.text
    property alias text: input.text
    property alias validator: input.validator
    signal editingFinished

    height: Style.text.size.normal + 6
    spacing: Style.margin.smaller
    Rectangle {
        id: rectangle
        width: 38
        height: parent.height
        color: Style.normal.baseColor
        radius: 3
        border.color: Style.normal.border.color
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#333333" }
            GradientStop { position: 1.0; color: Style.normal.tabButton.highlightedGradientColor1 }
        }
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
               parent.border.color = Style.hover.border.color;
            }
            onExited: {
               parent.border.color = Style.normal.border.color;
            }
        }
        TextInput {
            id: input
            width: 34
            height: 18
            y: 2
            wrapMode: TextInput.Wrap
            selectByMouse: true
            maximumLength: 7
            horizontalAlignment: TextInput.AlignHCenter
            color: Style.normal.text.color
            onEditingFinished: container.editingFinished()
        }
    }
    Text {
        id: caption
        width: 60
        color: Style.normal.text.color
        font.pixelSize: Style.text.size.larger;
        anchors.verticalCenter: parent.verticalCenter
        renderType: Text.NativeRendering
    }

    states: [
        State {
            when: enabled
            PropertyChanges {
                target: rectangle
                color: Style.normal.baseColor
            }
            PropertyChanges {
                target: input
                color: Style.normal.text.color
            }
            PropertyChanges {
                target: caption
                color: Style.normal.text.color
            }
        },
        State {
            when: !enabled
            PropertyChanges {
                target: rectangle
                color: Style.disabled.input.color
            }
            PropertyChanges {
                target: input
                color: Style.disabled.input.fontColor
            }
            PropertyChanges {
                target: caption
                color: Style.disabled.input.fontColor
            }
        }
    ]
}
