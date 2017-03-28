import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Optimum Notch"
    canApply: true
    canHelp: true
    Component.onCompleted: {
        var text = [
            "1. Generate a Fourier spectrum image.",
            "2. Draw some rectangles on this Fourier spectrum image to indicate some notches.",
            "3. Choose the propriate highpass in the combox.",
            "4. Click the apply button."
        ];
        container.helpText = text.join('\n');
    }

    function check(imageItem) {
        if (imageItem.type != "fourierSpectrum") {
            manager.showMessage("The selected image is not a Fourier spectrum image!");
            return false;
        }
        if (imageTool.countImageMasks(imageItem.uri) == 0) {
            manager.showMessage("Please draw some rectangles on the Fourier spectrum image!");
            return false;
        }
        if (!manager.imageExists(imageItem.owner)) {
            manager.showMessage("the owner of the image has been removed!");
            return false;
        }
        return true;
    }

    function runOptimum(type) {
        var image = manager.currentImage;
        var imageItem = manager.getImageItem(image);
        if (!check(imageItem))
            return;
        manager.execute(function(){
            var uri = image;
            switch (combox.index) {
            case 0:
                uri = filters.optimumNotchFilter_Block(imageItem.owner, image, m.value, n.value, type, basedOnHSI);
                break;
            case 1:
                uri = filters.optimumNotchFilter_Ideal(imageItem.owner, image, cutoff.value, m.value, n.value, type, basedOnHSI);
                break;
            case 2:
                uri = filters.optimumNotchFilter_Butterworth(imageItem.owner, image, cutoff.value, order.value, m.value, n.value, type, basedOnHSI);
                break;
            case 3:
                uri = filters.optimumNotchFilter_Gaussian(imageItem.owner, image, cutoff.value, m.value, n.value, type, basedOnHSI);
                break;
            }
            manager.addImage(uri, image);
            return true;
        });
    }

    buttonModel: ObjectModel {
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the spacial image of the filter"
            enabled: manager.enabled
            source: "../../../image/dark/icons/spatial-image.png"
            onClicked: runOptimum(Filters.Filter_Spatial_Image)
        }
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the frequency result"
            enabled: manager.enabled
            source: "../../../image/dark/icons/fourier-transform.png"
            onClicked: runOptimum(Filters.Filter_Frequency_Result)
        }
    }

    optionModel: ObjectModel {
        Combox {
            id: combox
            width: 100
            index: 0
            model: ListModel {
                ListElement { name: "Block" }
                ListElement { name: "Ideal" }
                ListElement { name: "Butterworth" }
                ListElement { name: "Gaussian" }
            }
        }
        Slider {
            id: cutoff
            width: contentWidth
            label: "Cutoff Frequency"
            minValue: 1
            maxValue: 101
            value: 9
            bins: 100
            enabled: combox.index != 0
            smallIncrement: false
        }
        Slider {
            id: order
            width: contentWidth
            label: "Order"
            minValue: 1
            maxValue: 9
            value: 2
            bins: 8
            enabled: combox.index == 2
            smallIncrement: false
        }
        Slider {
            id: m
            width: contentWidth
            label: "m"
            minValue: 1
            maxValue: 33
            value: 1
            bins: 32
        }
        Slider {
            id: n
            width: contentWidth
            label: "n"
            minValue: 1
            maxValue: 33
            value: 1
            bins: 32
        }
    }
    onApply: runOptimum(Filters.Filter_Spatial_Result)
}
