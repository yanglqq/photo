import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Band Reject"
    canApply: true

    function run(resultType) {
        manager.execute(function(){
            var generated = false;
            var currentImage = manager.currentImage;
            if (!currentImage) {
                generated = true;
                currentImage = "generated"
            }
            var uri = currentImage;
            switch (combox.index) {
            case 0:
                uri = filters.bandRejectFilter_Ideal(currentImage, cutoff.value, bandWidth.value, resultType, basedOnHSI);
                break;
            case 1:
                uri = filters.bandRejectFilter_Butterworth(currentImage, cutoff.value, order.value, bandWidth.value, resultType, basedOnHSI);
                break;
            case 2:
                uri = filters.bandRejectFilter_Gaussian(currentImage, cutoff.value, bandWidth.value, resultType, basedOnHSI);
                break;
            }
            if (generated)
                manager.addImage(uri, null, null, "", true, null);
            else
                manager.addImage(uri, currentImage);
            return true;
        });
    }

    buttonModel: ObjectModel {
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the spacial image of the filter"
            source: "../../../image/dark/icons/spatial-image.png"
            onClicked: run(Filters.Filter_Spatial_Image)
        }
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the frequency image of the filter"
            source: "../../../image/dark/icons/fourier-inverse.png"
            onClicked: run(Filters.Filter_Frequency_Image)
        }
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the frequency result"
            enabled: manager.enabled
            source: "../../../image/dark/icons/fourier-transform.png"
            onClicked: run(Filters.Filter_Frequency_Result)
        }
    }

    optionModel: ObjectModel {
        Combox {
            id: combox
            width: 100
            index: 0
            model: ListModel {
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
            enabled: combox.index == 1
            smallIncrement: false
        }
        Slider {
            id: bandWidth
            width: contentWidth
            label: "Band Width"
            minValue: 1
            maxValue: 101
            value: 3
            bins: 100
            smallIncrement: false
        }
    }
    onApply: run(Filters.Filter_Spatial_Result)
}
