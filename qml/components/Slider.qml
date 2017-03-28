import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../themes"

Rectangle {
    id: container

    property alias label: labelText.text
    property real value: 0;
    property real minValue: 0
    property real maxValue: 50
    property int  bins: 10
    property bool smallIncrement: true

    height: 22
    color: "transparent"
    radius: Style.border.radius
    border.color: Style.normal.border.color
    border.width: Style.border.width

    Rectangle {
        id: slider
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radius: Style.border.radius
        Image {
            id: sliderImage
            anchors.fill: parent
            fillMode: Image.TileHorizontally
        }
    }

    function adjustValue(value) {
        var binWidth = (maxValue - minValue) / bins;
        var temp = minValue;
        var i = 0;
        for (; i < bins; i++) {
            if (value <= temp) {
                value = temp;
                break;
            }
            temp += binWidth;
        }
        if (i == bins) {
            value = maxValue;
        }
        return value;
    }

    function trim(val) {
        var s = val + "";
        var i = s.indexOf('.');
        if (i != -1) {
            if (s.length - i > 4) {
                s = s.substring(0, i + 4);
            }
        }
        return Number(s);
    }

    function updateValue(val) {
        val = trim(val);
        container.value = val;
        slider.width = (val - minValue) * width / (maxValue - minValue);
        valueText.text = val;
        valueChanged(val);
    }

    Component.onCompleted: updateValue(adjustValue(value))

    Text {
        id: labelText
        anchors.left: parent.left
        anchors.leftMargin: Style.margin.normal
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
    }
    Text {
        id: valueText
        anchors.right: parent.right
        anchors.rightMargin: Style.margin.normal
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
    }
    states: [
        State {
            when: enabled
            PropertyChanges {
                target: slider
                color: Style.normal.slider.color
            }
            PropertyChanges {
                target: sliderImage
                source: "../../image/patterns/slider-stripe-grey.png"
            }
            PropertyChanges {
                target: labelText
                color: Style.normal.slider.fontColor
            }
            PropertyChanges {
                target: valueText
                color: Style.normal.slider.fontColor
            }
        },
        State {
            when: !enabled
            PropertyChanges {
                target: slider
                color: Style.disabled.slider.color
            }
            PropertyChanges {
                target: sliderImage
                source: "../../image/patterns/slider-stripe-darkgrey.png"
            }
            PropertyChanges {
                target: labelText
                color: Style.disabled.slider.fontColor
            }
            PropertyChanges {
                target: valueText
                color: Style.disabled.slider.fontColor
            }
        }
    ]
    MouseArea {
        anchors.fill: parent;
        hoverEnabled: enabled
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button == Qt.LeftButton) {
                if (smallIncrement) {
                    var binWidth = (maxValue - minValue) / bins;
                    var pos = (value  - minValue ) * width / (maxValue - minValue);
                    var sign = pos > mouse.x ? -1 : 1;
                    value += sign * binWidth;
                    if (value > maxValue)
                        value = maxValue;
                    else if (value < minValue)
                        value = minValue;
                } else {
                    value = adjustValue(minValue + (maxValue - minValue) * mouse.x / width)
                }
            } else if (mouse.button == Qt.RightButton) {
                menu.popup();
            }
            updateValue(value);
        }
        onEntered: parent.border.color = Style.hover.border.color
        onExited: parent.border.color = Style.normal.border.color
    }

    Menu {
        id: menu
        ExclusiveGroup { id: group }
        MenuItem {
            text: "Small Increment"
            checkable: true
            checked: smallIncrement
            exclusiveGroup: group
            onTriggered: smallIncrement = true
        }
        MenuItem {
            text: "Large Increment"
            checkable: true
            checked: !smallIncrement
            exclusiveGroup: group
            onTriggered: smallIncrement = false
        }
        style: MenuStyle {
            frame: Rectangle {
                color: Style.normal.contextMenu.backgroundColor
            }
            itemDelegate.label: Label {
                text: styleData.text
                color: Style.normal.text.color
                font.pixelSize: Style.text.size.normal;
            }
            itemDelegate.background: Rectangle {
                color: Style.normal.contextMenu.menuItemColor
            }
        }
    }
}
