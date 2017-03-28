import QtQuick 2.2
import QtQuick.Dialogs 1.1
import Polka 1.0
import QtQuick.Window 2.2
import "qml/components"
import "qml/themes"

Window {
    width: 800
    height: 800
    color: "#333333"
    visible: true
    Morphology {
        id: morphology
    }

    Item {
        id: root
        property var root: root

        Grid {
            id: grid
            width: 420
            height: 420
            columns: 2
            spacing: 5

            Column {
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                spacing: 5
                IntensityColouring {
                    id: colouring
                    property int colorIndex: 0
                    width: parent.width
                    height: 20
                    onColorDialog: {
                        colouring.colorIndex = index;
                        colorDialog.setColor(color);
                        colorDialog.open();
                    }
                    ColorDialog {
                        id: colorDialog
                        title: "Please choose a color"
                        onAccepted: colouring.setColor(colouring.colorIndex, colorDialog.currentColor)
                    }
                }
                Row {
                    width: parent.width / 2 - 2 * parent.spacing
                    height: 50
                    spacing: 5
                    Combox {
                        width: 70
                        model: ListModel {
                            ListElement {
                                name: "Apple"
                            }
                            ListElement {
                                name: "Orange"
                            }
                            ListElement {
                                name: "Banana"
                            }
                        }
                    }
                    Checker {
                        text: "check1"
                        onClicked: {
                            checked ? hintMessage.hint("checked") : hintMessage.hint("unchecked");
                            input.enabled = !checked;
                        }
                    }

                }
                Row {
                    width: parent.width / 2 - 2 * parent.spacing
                    height: 50
                    spacing: 5
                    IconButton {
                        tip: "show an error message"
                        source: "image/dark/icons/apply.png"
                        onClicked: hintMessage.error("this is an error message!")
                        text: "error"
                    }
                    IconButton {
                        tip: "show a hint message"
                        source: "image/dark/icons/apply.png"
                        onClicked: hintMessage.hint("this is a hint message!")
                        text: "hint"
                    }
                }
                Input {
                    id: input
                    width: 100
                    caption: "input1"
                    text: "10"
                    validator: DoubleValidator {
                        bottom: 1
                        top: 100
                        decimals: 2
                    }
                }
            }


            Column {
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                spacing: 5
                Slider {
                    width: parent.width
                    label: "slider 0 - 20"
                    minValue: 0
                    maxValue: 20
                    value: 0
                }
                Slider {
                    width: parent.width
                    label: "slider 0 - 0.1"
                    minValue: 0
                    maxValue: 0.1
                    value: 0
                }
                Slider {
                    width: parent.width
                    label: "slider 1 - 20"
                    minValue: 1
                    maxValue: 20
                    value: 10
                }
                Slider {
                    width: parent.width
                    label: "large increment -100 - 800"
                    minValue: -100
                    maxValue: 800
                    value: 500
                    smallIncrement: false;
                    onValueChanged: hintMessage.hint(value)
                }
                Slider {
                    width: parent.width
                    enabled: false
                    label: "slider 0-200"
                    minValue: 0
                    maxValue: 200
                    value: 10
                }
            }

            Column {
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                spacing: 5
                Slider {
                    id: sliderM
                    width: parent.width
                    label: "m"
                    minValue: 3
                    maxValue: 23
                    bins: 10
                    value: 3
                    onValueChanged: {
                        structureElements.m = value;
                        structureElements.update();
                    }
                }
                Slider {
                    id: sliderN
                    width: parent.width
                    label: "n"
                    minValue: 3
                    maxValue: 23
                    bins: 10
                    value: 3
                    onValueChanged: {
                        structureElements.n = value;
                        structureElements.update();
                    }
                }
                Row {
                    width: parent.width / 2 - 2 * parent.spacing
                    height: 20
                    spacing: 5
                    IconButton {
                        tip: "generate structure element code"
                        source: "image/dark/icons/apply.png"
                        text: "rect"
                        onClicked: {
                            var elements = morphology.createStructureElements(sliderM.value, sliderN.value, Morphology.RECT);
                            hintMessage.help(elements.join(","));
                        }
                    }
                    IconButton {
                        tip: "generate structure element code"
                        source: "image/dark/icons/apply.png"
                        text: "ellipse"
                        onClicked: {
                            var elements = morphology.createStructureElements(sliderM.value, sliderN.value, Morphology.ELLIPSE);
                            hintMessage.help(elements.join(","));
                        }
                    }
                    IconButton {
                        tip: "generate structure element code"
                        source: "image/dark/icons/apply.png"
                        text: "cross"
                        onClicked: {
                            var elements = morphology.createStructureElements(sliderM.value, sliderN.value, Morphology.CROSS);
                            hintMessage.help(elements.join(","));
                        }
                    }
                }
                Row {
                    width: parent.width / 2 - 2 * parent.spacing
                    height: 20
                    spacing: 5
                    IconButton {
                        tip: "generate structure element code"
                        source: "image/dark/icons/apply.png"
                        text: "generate"
                        onClicked: {
                            var elements = structureElements.elements;
                            hintMessage.help(elements.join(","));
                        }
                    }
                }
                StructureElements {
                    id: structureElements
                    width: parent.width
                    height: parent.height - 3 * sliderM.height
                }
            }

            Curve {
                id: curve1
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                mask: Curve.LuminanceMask
                curve: Curve.Spline
                slippers : [
                    Slipper {
                        xp: 0.3
                        yp: 0.3
                    }
                ]
                CurveMenu {
                    id: curveMenu1
                    curveControl: curve1
                }
                onContextMenu: curveMenu1.popup();
            }
            Curve {
                id: curve2
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                mask: Curve.ColorMask
                curve: Curve.Cosine
                slippers : [
                    Slipper {
                        xp: 0.6
                        yp: 0.6
                    }
                ]
                CurveMenu {
                    id: curveMenu2
                    curveControl: curve2
                }
                onContextMenu: curveMenu2.popup();
            }
            Curve {
                id: curve3
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                mask: Curve.ColorBlocks
                curve: Curve.Linear
                topLeftColor: "#c83264"
                bottomRightColor: "#329632"
                CurveMenu {
                    id: curveMenu3
                    curveControl: curve3
                }
                onContextMenu: curveMenu3.popup();
            }
            Curve {
                id: curve4
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                mask: Curve.ColorBlocks
                slipperShape: Curve.CircleShape
                topLeftColor: "#ffff4b"
                bottomRightColor: "#3264c8"
                CurveMenu {
                    id: curveMenu4
                    curveControl: curve4
                }
                onContextMenu: curveMenu4.popup();
            }
            Curve {
                id: curve5
                width: parent.width / 2 - 2 * parent.spacing
                height: parent.height / 2 - 2 * parent.spacing
                curve: Curve.Linear
                slippers : [
                    Slipper {
                        xp: 0.3
                        yp: 0.3
                    },
                    Slipper {
                        xp: 0.3
                        yp: 0.5
                    },
                    Slipper {
                        xp: 0.4
                        yp: 0.5
                    },
                    Slipper {
                        xp: 0.4
                        yp: 0.4
                    }
                ]
                CurveMenu {
                    id: curveMenu5
                    curveControl: curve5
                    showMaskMenuItem: false
                }
                onContextMenu: curveMenu5.popup();
            }
        }
        Rectangle {
            anchors.left: grid.right
            width: parent.width - grid.width
            height: parent.height
            color: "black"
            ImageMask {
                anchors.fill: parent
            }
        }
    }
    HintMessage {
        id: hintMessage;
        width: parent.width / 2
        height: 100
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}

