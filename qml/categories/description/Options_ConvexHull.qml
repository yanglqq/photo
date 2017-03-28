import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Convex Hull"
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
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Combox {
                id: algorithm
                width: 147
                index: 0
                anchors.verticalCenter: parent.verticalCenter
                model: ListModel {
                    ListElement {
                        name: "Graham's Scan"
                        value: Description.GRAHAM_SCAN
                    }
                    ListElement {
                        name: "Jarvis March"
                        value: Description.JARVIS_MARCH
                    }
                }
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: drawLine
                 anchors.verticalCenter: parent.verticalCenter
                text: "draw lines"
                checked: true
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var boundaryPixelValuei = parseInt(boundaryPixelValue.text);
            var uri = description.convexhull(
                        manager.currentImage,
                        boundaryPixelValuei,
                        algorithm.model.get(algorithm.index).value,
                        drawLine.checked);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
