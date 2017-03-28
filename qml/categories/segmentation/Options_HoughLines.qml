import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Hough Lines"
    canApply: true

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: rho
                width: 112
                caption: "rho"
                text: "1"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
            Input {
                id: theta
                width: 112
                caption: "theta"
                text: "1"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Threshold {
            id: threshold
            width: contentWidth
            percentage: true
            name: "threshold"
            initialValue: "30"
        }
        Threshold {
            id: minLineLength
            width: contentWidth
            percentage: true
            name: "min line length"
            initialValue: "10"
        }
        Threshold {
            id: maxLineGap
            width: contentWidth
            percentage: true
            name: "max line gap"
            initialValue: "10"
        }        
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: minAngle
                width: 112
                caption: "min angle"
                text: "-90"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
            Input {
                id: maxAngle
                width: 112
                caption: "max angle"
                text: "90"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    onApply: {
        manager.execute(function() {
            if (!threshold.checkValue()) {
                manager.showMessage("0% <= threshold <= 100%");
                return false;
            }
            if (!minLineLength.checkValue()) {
                manager.showMessage("0% <= minLineLength <= 100%");
                return false;
            }
            if (!maxLineGap.checkValue()) {
                manager.showMessage("0% <= maxLineGap <= 100%");
                return false;
            }
            var width = imageTool.width(manager.currentImage);
            var height = imageTool.height(manager.currentImage);
            var rhof = parseFloat(rho.text);
            var thetaf = parseFloat(theta.text);
            var thresholdf = threshold.getValue(width, height);
            var minLineLengthf = minLineLength.getValue(width, height);
            var maxLineGapf = maxLineGap.getValue(width, height);
            var minAnglef = parseFloat(minAngle.text);
            var maxAnglef = parseFloat(maxAngle.text);
            var uri = segmentation.houghLines(manager.currentImage, rhof, thetaf, thresholdf, minLineLengthf, maxLineGapf, minAnglef, maxAnglef);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
