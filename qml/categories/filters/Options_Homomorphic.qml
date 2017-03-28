import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Homomorphic"
    canApply: true
    canHelp: true
    Component.onCompleted: {
        var text = [
            "This filter affects the low- and high-frequency components of the Fourier transorm in different controllable ways.",
            "The C controls the sharpness of the slope of the filter function as it transitions between Low and High.",
            "If the parameters Low less than 1 and High greater than 1, the filter tends to attenuate the contribution made by the low frequencies(illumination) and amplify the contribution made by high frequencies(reflectance).The net result of the filter is simultaneous dynamic range compression and contrast enhancement."
        ];
        container.helpText = text.join('\n\n');
    }

    function run(type) {
        var generated = false;
        var currentImage = manager.currentImage;
        if (!currentImage) {
            generated = true;
            currentImage = "generated"
        }
        var uri = currentImage;
        manager.execute(function(){
            uri = filters.homomorphic(currentImage, cutoff.value, lowFrequency.value, highFrequency.value, c.value, type, basedOnHSI);
            manager.addImage(uri, currentImage);
            return true;
        });
    }

    buttonModel: ObjectModel {
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the spacial image of the filter"
            enabled: manager.enabled
            source: "../../../image/dark/icons/spatial-image.png"
            onClicked: run(Filters.Filter_Spatial_Image)
        }
        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            tip: "show the frequency image of the filter"
            enabled: manager.enabled
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
        Slider {
            id: cutoff
            width: contentWidth
            label: "Cutoff Frequency"
            minValue: 1
            maxValue: 101
            value: 50
            bins: 100
            smallIncrement: false
        }
        Slider {
            id: lowFrequency
            width: contentWidth
            label: "Low"
            minValue: 0
            maxValue: 5
            value: 0.25
            bins: 50
            smallIncrement: false
        }
        Slider {
            id: highFrequency
            width: contentWidth
            label: "High"
            minValue: 0
            maxValue: 5
            value: 2
            bins: 50
            smallIncrement: false
        }
        Slider {
            id: c
            width: contentWidth
            label: "C"
            minValue: 0
            maxValue: 5
            value: 0.5
            bins: 50
            smallIncrement: false
        }
    }
    onApply: run(Filters.Filter_Spatial_Result)
}
