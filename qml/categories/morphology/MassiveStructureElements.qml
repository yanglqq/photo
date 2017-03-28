import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Rectangle {
    id: container
    color: "#000000"
    property int shape: Morphology.RECT
    property int m: 3
    property int n: 3
    property int origin: 4
    property var elements: null

    function update() {
        elements = morphology.createStructureElements(m, n, shape);
        origin = elements.length / 2;
    }
    function select() {
        color = "#222222";
    }
    function unSelect() {
        color = "#000000";
    }

    Component.onCompleted: update();

    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: Style.normal.text.color
        font.pixelSize: 15
        font.bold: true
        renderType: Text.NativeRendering
    }
    states: [
        State {
            when: shape == Morphology.RECT
            PropertyChanges {
                target: text
                text: "Rect"
            }
        },
        State {
            when: shape == Morphology.ELLIPSE
            PropertyChanges {
                target: text
                text: "Ellipse"
            }
        },
        State {
            when: shape == Morphology.CROSS
            PropertyChanges {
                target: text
                text: "Cross"
            }
        }
    ]
}
