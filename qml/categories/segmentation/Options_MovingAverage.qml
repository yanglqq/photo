import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Moving Average"
    canApply: true

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
            id: n
            width: contentWidth
            label: "n"
            minValue: 1
            maxValue: 101
            bins: 50
            value: 21
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: weightOfMean
                width: contentWidth
                caption: "nonnegative weight of mean"
                text: "0.5"
                validator: DoubleValidator {
                    decimals: 3
                    bottom: 0.0
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    onApply: {
        manager.execute(function(){
            var maxValuef = parseFloat(maxValue.text);
            var weightOfMeanf = parseFloat(weightOfMean.text);
            var uri = segmentation.movingAverage(manager.currentImage, maxValuef, weightOfMeanf, n.value);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
