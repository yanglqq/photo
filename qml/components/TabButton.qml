import QtQuick 2.2
import QtGraphicalEffects 1.0
import "../themes"

Item {
    id: container

    property alias label: labelText.text
    property bool hasSelected: false
    signal clicked;

    width: labelText.height + 10
    height: labelText.width + 20
    Rectangle {
        id: gradientRec
        anchors.centerIn: parent
        width: parent.height
        height: parent.width
        border.width: Style.border.width
        border.color: Style.normal.border.color
        radius: Style.border.radius
        gradient: normalGrad
        rotation: -90;
    }
    Gradient{
        id: normalGrad
        GradientStop { position: 0.0; color: Style.normal.tabButton.normalGradientColor0 }
        GradientStop { position: 1.0; color: Style.normal.tabButton.normalGradientColor1 }
    }
    Gradient{
        id: highlightGrad
        GradientStop { position: 0.0; color: Style.normal.tabButton.highlightedGradientColor0 }
        GradientStop { position: 1.0; color: Style.normal.tabButton.highlightedGradientColor1 }
    }
    Text {
        id: labelText;
        anchors.centerIn: parent
        color: Style.normal.text.color
        font.pixelSize: Style.text.size.normal
        renderType: Text.NativeRendering
        rotation: -90;
    }
    function setSelectedState() {
        container.hasSelected = true;
        gradientRec.gradient = highlightGrad;
        labelText.color = Style.normal.text.strongColor;
        gradientRec.border.color = Style.hover.border.color;
    }
    function clearSelectedState() {
        container.hasSelected = false;
        gradientRec.gradient = normalGrad;
        labelText.color = Style.normal.text.color;
        gradientRec.border.color = Style.normal.border.color;
    }
    ToolTipArea {
        text: label
        anchors.fill: parent
        onClicked: {
            container.clicked();
        }
        onEntered: {
            if (!container.hasSelected) {
                gradientRec.gradient = highlightGrad;
                labelText.color = Style.normal.text.strongColor;
            }
        }
        onExited: {
            if (!container.hasSelected) {
                gradientRec.gradient = normalGrad;
                labelText.color = Style.normal.text.color;
            }
        }
    }
}
