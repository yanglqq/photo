import QtQuick 2.2
import "../themes"

MouseArea {
    id: mouseArea

    property alias tip: tip
    property alias text: tip.text
    property alias hideDelay: hideTimer.interval
    property alias showDelay: showTimer.interval

    hoverEnabled: true
    Timer {
        id: showTimer
        interval: 1000
        running: tip.text != "" && mouseArea.containsMouse && !tip.visible
        onTriggered: {
            var pos = root.mapFromItem(mouseArea, mouseArea.mouseX, mouseArea.mouseY + Style.tooltip.offsetY);
            tip.x = pos.x;
            tip.y = pos.y;
            tip.show();
        }
    }
    Timer {
        id: hideTimer
        interval: 100
        running: tip.text != "" && !mouseArea.containsMouse && tip.visible
        onTriggered: tip.hide();
    }

    ToolTip{
        id: tip
        Component.onCompleted: tip.parent = root
    }
}

