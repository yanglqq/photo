import QtQuick 2.2
import "../themes"

Item {
    id: container

    property alias source: image.source
    property alias tip: tipArea.text
    property alias text: text.text
    signal clicked;

    width: 20
    height: 20

    Rectangle {
        id: imageBorder
        width: 20
        height: 20
        radius: Style.border.radius
        color: "transparent"
        border.color: "transparent"
        border.width: Style.border.width
        Image {
            id: image
            anchors.fill: parent
            anchors.margins: 2
        }
        ToolTipArea {
            id: tipArea
            anchors.fill: parent
            onClicked: {
                if (container.enabled) {
                    container.clicked();
                }
            }
            onEntered: {
                if (container.enabled) {
                    parent.border.color = Style.hover.border.color;
                }
            }
            onExited: {
                if (container.enabled) {
                    parent.border.color = "transparent";
                }
            }
        }
    }
    Text {
        id: text
        anchors.left: imageBorder.right
        anchors.leftMargin: Style.margin.smaller
        anchors.verticalCenter: parent.verticalCenter
        color: Style.normal.text.color
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
    }

    Component.onCompleted: {
        if (text.paintedWidth > 0) {
            container.width += text.paintedWidth + text.anchors.leftMargin
        }
    }

}
