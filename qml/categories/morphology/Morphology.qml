import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    property var ses: null

    Morphology {
        id: morphology
        resultNew: manager.resultNew
    }    

    Slider {
        id: sliderM
        width: parent.width
        label: "m"
        minValue: 1
        maxValue: 101
        bins: 50
        value: 3
        smallIncrement: false
        onValueChanged: {
            if (ses.m == value) return;
            ses.m = value;
            ses.update();
        }
    }
    Slider {
        id: sliderN
        width: parent.width
        label: "n"
        minValue: 1
        maxValue: 101
        bins: 50
        value: 3
        smallIncrement: false
        onValueChanged: {
            if (ses.n == value) return;
            ses.n = value;
            ses.update();
        }
    }

    Grid {
        id: grid
        width: parent.width;
        height: 170
        columns: 3
        Repeater {
            id: repeater1
            model: [Morphology.RECT, Morphology.ELLIPSE, Morphology.CROSS]
            Item {
                width: grid.width / 3
                height: grid.width / 3
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (ses == massive) return;
                        ses.unSelect();
                        ses = massive;
                        ses.select();
                        sliderM.updateValue(ses.m);
                        sliderN.updateValue(ses.n);
                    }
                }
                MassiveStructureElements {
                    id: massive
                    shape: Morphology.RECT
                    width: parent.width - 4 * Style.margin.normal
                    height: parent.width - 4 * Style.margin.normal
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Component.onCompleted: {
                    massive.shape = repeater1.model[index];
                    massive.update();
                    if (index == 0) {
                        ses = massive;
                        ses.select();
                    }
                }
            }
        }
        Repeater {
            id: repeater2
            model: ListModel {
                id: selist
                ListElement {
                    elements: "1,1,1,1,1,1,1,1,1"
                    m: 3
                    n: 3
                    origin: 4
                }
                ListElement {
                    elements: "0,1,0,1,1,1,0,1,0"
                    m: 3
                    n: 3
                    origin: 4
                }
                ListElement {
                    elements: "0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0"
                    m: 7
                    n: 7
                    origin: 24
                }
            }
            Item {
                width: grid.width / 3
                height: grid.width / 3
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (ses == se) return;
                        ses.unSelect();
                        ses = se;
                        ses.select();
                        sliderM.updateValue(ses.m);
                        sliderN.updateValue(ses.n);
                    }
                }
                StructureElements {
                    id: se
                    width: parent.width - 4 * Style.margin.normal
                    height: parent.width - 4 * Style.margin.normal
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    function select() {
                        se.backgroundColor = "#222222";
                        se.update();
                    }
                    function unSelect() {
                        se.backgroundColor = "#000000";
                        se.update();
                    }
                }
                Component.onCompleted: {
                    var item = repeater2.model.get(index);
                    var elements = item.elements.split(",");
                    for (var i = 0; i < elements.length; i++) {
                        elements[i] = parseInt(elements[i]);
                    }
                    se.setElements(elements, item.m, item.n, item.origin);
                    se.update();
                }
            }
        }
    }

    Options_BasicOperations {
        width: parent.width
    }

    Options_Gradient {
        width: parent.width
    }

    Options_Hat {
        width: parent.width
    }

    Options_Thinning {
        width: parent.width
    }

    Options_Thickening {
        width: parent.width
    }

    Options_ConvexHull {
        width: parent.width
    }

    Options_Pruning {
        width: parent.width
    }

    Options_Geodesic {
        width: parent.width
    }

    Options_Reconstruction {
        width: parent.width
    }

    Options_ConnectedComponents {
        width: parent.width
    }

    Row {
        width: parent.width
        height: Style.category.optionItemHeight
        spacing: 10

        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            enabled: manager.enabled
            source: "../../../image/dark/icons/apply.png"
            text: "boundary extraction"
            onClicked: {
                manager.execute(function() {
                    var se = ses;
                    var uri = morphology.boundary(manager.currentImage, se.elements, se.m, se.n, se.origin);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }

        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            enabled: manager.enabled
            source: "../../../image/dark/icons/apply.png"
            text: "skeleton"
            onClicked: {
                manager.execute(function() {
                    var se = ses;
                    var uri = morphology.skeleton(manager.currentImage, se.elements, se.m, se.n, se.origin);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }
    }
}
