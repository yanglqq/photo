import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Boundary"
    canApply: true

    optionModel: ObjectModel {
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: boundaryPixelValue
                width: 102
                caption: "boundary pixel value"
                text: "255"
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Text {
                color: Style.normal.text.color
                font.pixelSize: Style.text.size.larger;
                renderType: Text.NativeRendering
                anchors.verticalCenter: parent.verticalCenter
                text: "approximated by "
            }
            Combox {
                id: approximation
                width: 147
                index: 0
                anchors.verticalCenter: parent.verticalCenter
                model: ListModel {
                    ListElement {
                        name: "None"
                        value: -1
                    }
                    ListElement {
                        name: "Sub Sampling"
                        value: Description.Sub_Sampling
                    }
                    ListElement {
                        name: "Minimum Perimeter Polygon"
                        value: Description.Minimum_Perimeter_Polygon
                    }
                    ListElement {
                        name: "Merging"
                        value: Description.Merging
                    }
                    ListElement {
                        name: "Splitting "
                        value: Description.Splitting
                    }
                }
            }
        }
        Slider {
            id: cellSize
            width: contentWidth
            label: "square cell size "
            minValue: 2
            maxValue: 102
            bins: 100
            value: 2
            enabled: approximation.index == 1 || approximation.index == 2
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: threshold
                width: 102
                caption: "threshold"
                text: "5"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: approximation.index == 3 || approximation.index == 4
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Text {
                color: Style.normal.text.color
                font.pixelSize: Style.text.size.larger;
                renderType: Text.NativeRendering
                anchors.verticalCenter: parent.verticalCenter
                text: "coding by "
            }
            Combox {
                id: coding
                width: 100
                index: 0
                anchors.verticalCenter: parent.verticalCenter
                model: ListModel {
                    ListElement {
                        name: "4-connectivity"
                        value: Description.Four_Connectivity
                    }
                    ListElement {
                        name: "8-connectivity"
                        value: Description.Eight_Connectivity
                    }
                }
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: pcTransform
                 anchors.verticalCenter: parent.verticalCenter
                text: "principal components transform"
                checked: false
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: drawLine
                 anchors.verticalCenter: parent.verticalCenter
                text: "draw lines"
                checked: false
            }
        }

    }
    onApply: {
        manager.execute(function(){
            var boundaryPixelValuei = parseInt(boundaryPixelValue.text);
            var thresholdf = parseFloat(threshold.text);
            var result = description.boundary(
                        manager.currentImage,
                        boundaryPixelValuei,
                        approximation.model.get(approximation.index).value,
                        coding.model[coding.index],
                        cellSize.value,
                        thresholdf,
                        pcTransform.checked,
                        drawLine.checked);
            manager.addImage(result.uri, manager.currentImage);
            //var codes = ["chain code: " + result.chainCode, "first difference: " + result.firstDifference, "shape number: " + result.shapeNumber];
            //hintMessage.help(codes.join("\n\n"));
            return true;
        });
    }
}
