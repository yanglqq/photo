import QtQuick 2.2
import "../themes"

Rectangle {
    id: tooltip

    property alias text: textContainer.text
    property int verticalPadding: Style.margin.smaller
    property int horizontalPadding: Style.margin.normal
    width: textContainer.width + horizontalPadding * 2
    height: textContainer.height + verticalPadding * 2
    color: Style.normal.baseColor
    Text {
        id: textContainer
        anchors.centerIn: parent
        color: Style.normal.text.color
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
    }
    NumberAnimation {
        id: fadein
        target: tooltip
        property: "opacity"
        easing.type: Easing.InOutQuad
        duration: 300
        from: 0
        to: 1
    }
    NumberAnimation {
        id: fadeout
        target: tooltip
        property: "opacity"
        easing.type: Easing.InOutQuad
        from: 1
        to: 0
        onStopped: visible = false;
    }
    visible: false
    onVisibleChanged: if(visible)fadein.start();
    function show(){
        visible = true;
        fadein.start();
    }
    function hide(){
        fadeout.start();
    }
}
