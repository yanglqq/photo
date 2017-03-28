import QtQuick 2.2
import "../themes"

Rectangle {
    id: container

    property alias model: repeater.model
    property int index: 0

    signal selected(int index)

    height: 18
    color: Style.normal.combox.color
    radius: Style.border.radius
    border.color: Style.normal.border.color
    border.width: Style.border.width
    Text {
        id: optionText
        anchors.left: parent.left
        anchors.leftMargin: Style.margin.normal
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: Style.text.size.normal
        color: Style.normal.text.color
        renderType: Text.NativeRendering
    }
    Text {
        id: line
        width: 15
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        renderType: Text.NativeRendering
        color: "#959595"
        text: "|"
    }
    states: [
        State {
            when: container.enabled
            PropertyChanges {
                target: optionText
                color: Style.normal.text.color
            }
            PropertyChanges {
                target: line
                color: Style.normal.text.color
            }
        },
        State {
            when: !container.enabled
            PropertyChanges {
                target: optionText
                color: Style.disabled.combox.fontColor
            }
            PropertyChanges {
                target: line
                color: Style.disabled.combox.fontColor
            }
        }
    ]
    function select(index) {
        var listModel = repeater.model;
        if (listModel.count > 0) {
            for (var i = 0; i < listModel.count; i++) {
                listModel.get(i).selected = false;
            }
            var item = listModel.get(index);
            optionText.text = item.name;
            item.selected = true;
        }
        container.index = index;
        dropDown.visible = false;
        selected(index);
    }

    Component.onCompleted: {
        select(index);
        dropDown.parent = root;
    }

    Rectangle {
        id: dropDown
        width: container.width
        visible: false;
        radius: Style.border.radius
        color: Style.normal.combox.color
        border.color: Style.normal.combox.dropDownBorderColor
        border.width: Style.border.width
        Flow {
            id: itemList
            anchors.fill: parent
            anchors.leftMargin: Style.margin.smaller
            spacing: Style.margin.smaller
            Repeater {
                id: repeater
                Rectangle {
                    id: item
                    width: dropDown.width - 2 * Style.margin.smaller
                    height: text.height
                    anchors.margins: Style.margin.smaller
                    state: "normal"
                    Text {
                        id: text
                        text: name
                        font.pixelSize: Style.text.size.normal
                        renderType: Text.NativeRendering
                    }
                    states: [
                        State {
                            name: "normal"
                            PropertyChanges {
                                target: item
                                color: Style.normal.combox.color
                            }
                            PropertyChanges {
                                target: text
                                color: Style.normal.text.color
                            }
                        },
                        State {
                            name: "highlighted"
                            PropertyChanges {
                                target: item
                                color: Style.hover.combox.color
                            }
                            PropertyChanges {
                                target: text
                                color: Style.hover.combox.fontColor
                            }
                        }
                    ]
                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: enabled
                        onClicked: select(index);
                        onEntered: item.state = "highlighted"
                        onExited: item.state = "normal"
                    }
                }
                Component.onCompleted: {
                    var h = 0;
                    var children = itemList.children;
                    for (var i = 0; i < children.length; i++) {
                        h += children[i].height;
                    }
                    dropDown.height = h + children.length * Style.margin.smaller;
                }
            }
        }
    }
    MouseArea {
        anchors.fill: parent;
        hoverEnabled: enabled
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            dropDown.visible = !dropDown.visible;
            if (dropDown.visible) {                
                var pos = root.mapFromItem(container, container.x, container.y + container.height + 1);
                dropDown.x = pos.x;
                dropDown.y = pos.y;
                var children = itemList.children;
                for (var i = 0; i < children.length; i++) {
                    children[i].state = "normal";
                }
                if (children.length > 0) {
                    children[index].state = "highlighted";
                }
            }
        }
        onEntered: parent.border.color = Style.hover.border.color
        onExited: parent.border.color = Style.normal.border.color
    }
}
