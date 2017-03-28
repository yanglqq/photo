import QtQuick 2.2
import "../themes"

Rectangle {
    id: container

    property alias tip: tipArea.text
    property alias text: text.text
    property bool checked: false
    signal clicked;

    width: 20
    height: 20

    color: "transparent"
    radius: Style.border.radius

    Rectangle {
        id: indicator
        width: 12
        height: 12
        anchors.left: parent.left
        anchors.leftMargin: Style.margin.smaller
        anchors.verticalCenter: parent.verticalCenter
        radius: Style.border.radius
        border.color: Style.normal.border.color
        border.width: Style.border.width
    }

    Text {
        id: text
        anchors.left: indicator.right
        anchors.leftMargin: Style.margin.smaller
        anchors.verticalCenter: parent.verticalCenter
        color: Style.normal.text.color
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
    }

    states: [
        State {
            when: !checked || !enabled
            PropertyChanges {
                target: indicator
                color: "transparent"
            }
            PropertyChanges {
                target: text
                color: Style.disabled.checker.fontColor
            }
        },
        State {
            when: checked && enabled
            PropertyChanges {
                target: indicator
                color: "green"
            }
            PropertyChanges {
                target: text
                color: Style.normal.text.color
            }
        }
    ]

    ToolTipArea {
        id: tipArea
        anchors.fill: parent
        onClicked: {
            container.checked = !container.checked;
            container.clicked(container.checked);
        }
        onEntered: container.border.color = Style.hover.border.color
        onExited: container.border.color = "transparent"
    }

    Component.onCompleted: {
        if (text.paintedWidth > 0) {
            container.width += text.paintedWidth + text.anchors.leftMargin
        }
    }

}
