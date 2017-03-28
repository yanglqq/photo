import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Optimum Global Thresholding"
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
                enabled: !twoThresholds.checked
            }
            Input {
                id: maxValue
                width: 102
                caption: "max value"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: !twoThresholds.checked
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: partition
                anchors.verticalCenter: parent.verticalCenter
                text: "partition image"
                checked: false
            }
        }
        Slider {
            id: m
            width: parent.width
            label: "m"
            minValue: 1
            maxValue: 101
            bins: 100
            value: 1
            enabled: partition.checked
        }
        Slider {
            id: n
            width: parent.width
            label: "n"
            minValue: 1
            maxValue: 101
            bins: 100
            value: 1
            enabled: partition.checked
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: histogram
                anchors.verticalCenter: parent.verticalCenter
                text: "choose a histogram image"
                checked: false
                enabled: !partition.checked
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Checker {
                id: twoThresholds
                anchors.verticalCenter: parent.verticalCenter
                text: "two thresholds"
                checked: false
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: value1
                width: 78
                caption: "value1"
                text: "0"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: twoThresholds.checked
            }
            Input {
                id: value2
                width: 78
                caption: "value2"
                text: "128"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: twoThresholds.checked
            }
            Input {
                id: value3
                width: 78
                caption: "value3"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
                enabled: twoThresholds.checked
            }
        }
    }

    function checkImages(images) {
        if (images.length < 1) {
            manager.showMessage("please select an image as a histogram evaluation first!");
            return false;
        }
        if (imageTool.channels(images[0]) != 1) {
            manager.showMessage("the checked image must be a gray image");
            return false;
        }
        return true;
    }

    onApply: {
        manager.execute(function(){
            var histogramURI = "";
            if (histogram.checked) {
                var selected = manager.getSelectedImages();
                if (!checkImages(selected))
                    return false;
                histogramURI = selected[0];
            }
            var uri = "";
            if (twoThresholds.checked) {
                var value1f = parseFloat(value1.text);
                var value2f = parseFloat(value2.text);
                var value3f = parseFloat(value3.text);
                uri = segmentation.optimumGlobalThresholding2(
                            manager.currentImage,
                            value1f, value2f, value3f,
                            histogram.checked && !partition.checked, histogramURI,
                            partition.checked, m.value, n.value);
            }
            else {
                var maxValuef = parseFloat(maxValue.text);
                var type = typeCombox.model.get(typeCombox.index).value;
                uri = segmentation.optimumGlobalThresholding1(
                            manager.currentImage,
                            maxValuef, type,
                            histogram.checked && !partition.checked, histogramURI,
                            partition.checked, m.value, n.value);
            }
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
