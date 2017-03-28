import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Hough Circles"
    canHelp: true
    canApply: true

    Component.onCompleted: {
        var text = [
            "dp – Inverse ratio of the accumulator resolution to the image resolution. For example, if dp=1 , the accumulator has the same resolution as the input image. If dp=2 , the accumulator has half as big width and height.",
            "min dist – Minimum distance between the centers of the detected circles. If the parameter is too small, multiple neighbor circles may be falsely detected in addition to a true one. If it is too large, some circles may be missed.",
            "threshold1 - it is the higher threshold of the two passed to the Canny() edge detector (the lower one is twice smaller).",
            "threshold2 - it is the accumulator threshold for the circle centers at the detection stage. The smaller it is, the more false circles may be detected. Circles, corresponding to the larger accumulator values, will be returned first.",
            "min radius – Minimum circle radius.",
            "max radius – Maximum circle radius."
        ];
        container.helpText = text.join('\n\n');
    }

    optionModel: ObjectModel {
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: dp
                width: 104
                caption: "dp"
                text: "2"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Threshold {
            id: minDist
            width: contentWidth
            percentage: true
            name: "min dist"
            initialValue: "10"
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: threshold1
                width: 127
                caption: "threshold1"
                text: "300"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Threshold {
            id: threshold2
            width: contentWidth
            percentage: true
            name: "threshold2"
            initialValue: "10"
        }
        Threshold {
            id: minRadius
            width: contentWidth
            percentage: true
            name: "min radius"
            initialValue: "10"
        }
        Threshold {
            id: maxRadius
            width: contentWidth
            percentage: true
            name: "max radius"
            initialValue: "10"
        }
    }

    onApply: {
        manager.execute(function() {
            if (!minDist.checkValue()) {
                manager.showMessage("0% <= min distance <= 100%");
                return false;
            }
            if (!threshold2.checkValue()) {
                manager.showMessage("0% <= threshold2 <= 100%");
                return false;
            }
            if (!minRadius.checkValue()) {
                manager.showMessage("0% <= min radius <= 100%");
                return false;
            }
            if (!maxRadius.checkValue()) {
                manager.showMessage("0% <= max radius <= 100%");
                return false;
            }
            var width = imageTool.width(manager.currentImage);
            var height = imageTool.height(manager.currentImage);
            var dpf = parseFloat(dp.text);
            var minDistf = minDist.getValue(width, height);
            var threshold1f = parseFloat(threshold1.text);
            var threshold2f = threshold2.getValue(width, height);
            var minRadiusf = minRadius.getValue(width, height);
            var maxRadiusf = maxRadius.getValue(width, height);
            var uri = segmentation.houghCircles(manager.currentImage, dpf, minDistf, threshold1f, threshold2f, minRadiusf, maxRadiusf);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
