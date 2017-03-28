import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Basic Global Thresholding"
    canApply: true

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Combox {
                id: typeCombox
                width: 100
                index: 0
                anchors.verticalCenter: parent.verticalCenter
                model: ListModel {
                    ListElement {
                        name: "Binary"
                        value: Segmentation.Thresholding_Binary
                    }
                    ListElement {
                        name: "Binary Inverse"
                        value: Segmentation.Thresholding_Binary_Inverse
                    }
                    ListElement {
                        name: "Truncate"
                        value: Segmentation.Thresholding_Truncate
                    }
                    ListElement {
                        name: "Zero"
                        value: Segmentation.Thresholding_Zero
                    }
                    ListElement {
                        name: "Zero Inverse"
                        value: Segmentation.Thresholding_Zero_Inverse
                    }
                }
            }
            Input {
                id: maxValue
                width: 102
                caption: "max value"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: absoluteChecker
                text: "absolute value"
                anchors.verticalCenter: parent.verticalCenter
                checked: false
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: delta
                width: 102
                caption: "delta"
                text: "0"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: evaluateChecker.checked
            }
            Checker {
                id: evaluateChecker
                anchors.verticalCenter: parent.verticalCenter
                text: "evaluate threshold"
                checked: true
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: threshold
                width: 102
                caption: "threshold"
                text: "5"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: !evaluateChecker.checked
            }
            Checker {
                id: thresholdPChecker
                enabled: !evaluateChecker.checked
                anchors.verticalCenter: parent.verticalCenter
                text: "% of max pixel value"
                checked: true
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var maxValuef = parseFloat(maxValue.text);
            var thresholdf = -1;
            if (!evaluateChecker.checked) {
                thresholdf = parseFloat(threshold.text);
                if (thresholdPChecker.checked) {
                    if (thresholdf < 0 || thresholdf > 100) {
                        manager.showMessage("0% <= threshold <= 100%");
                        return false;
                    }
                    thresholdf *= imageTool.maxPixelValue(manager.currentImage) / 100;
                }
            } else
                thresholdf = parseFloat(delta.text);
            var type = typeCombox.model.get(typeCombox.index).value;
            var uri = segmentation.basicGlobalThresholding(manager.currentImage, maxValuef, thresholdf, type, absoluteChecker.checked, evaluateChecker.checked);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
