import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Local Thresholding"
    canHelp: true
    canApply: true

    Component.onCompleted: {
        var text = [
            "Choosing the global mean generally gives better results when the background is nearly constant and all the object intensities are above or below the background intensity."
        ];
        container.helpText = text.join('\n');
    }

    optionModel: ObjectModel {
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: maxValue
                width: 102
                caption: "max value"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Slider {
            id: m
            width: contentWidth
            label: "m"
            minValue: 1
            maxValue: 101
            bins: 50
            value: 3
        }
        Slider {
            id: n
            width: contentWidth
            label: "n"
            minValue: 1
            maxValue: 101
            bins: 50
            value: 3
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: weightOfVariance
                width: contentWidth
                caption: "nonnegative weight of local variance"
                text: "30"
                validator: DoubleValidator {
                    decimals: 3
                    bottom: 0.0
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: weightOfMean
                width: contentWidth
                caption: "nonnegative weight of mean"
                text: "1.5"
                validator: DoubleValidator {
                    decimals: 3
                    bottom: 0.0
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: usingGlobalMean
                anchors.verticalCenter: parent.verticalCenter
                text: "using global mean"
                checked: true
            }
        }
    }

    onApply: {
        manager.execute(function(){
            var maxValuef = parseFloat(maxValue.text);
            var weightOfVariancef = parseFloat(weightOfVariance.text);
            var weightOfMeanf = parseFloat(weightOfMean.text);
            var uri = segmentation.localThresholding(
                        manager.currentImage,
                        maxValuef,
                        weightOfVariancef, weightOfMeanf,
                        m.value, n.value,
                        usingGlobalMean.checked);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
