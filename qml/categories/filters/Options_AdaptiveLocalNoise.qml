import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Adaptive Local Noise"
    canApply: true

    optionModel: ObjectModel {
        Slider {
            id: m
            width: contentWidth
            label: "M"
            minValue: 1
            maxValue: 33
            value: 3
            bins: 32
        }
        Slider {
            id: n
            width: contentWidth
            label: "N"
            minValue: 1
            maxValue: 33
            value: 3
            bins: 32
        }
        Slider {
            id: variance
            width: contentWidth
            label: "The variance of the overall noise"
            minValue: 0
            maxValue: 33
            value: 1
            bins: 33
        }
    }
    onApply: {
        manager.execute(function(){
            var currentImage = manager.currentImage;
            var uri = filters.adaptiveLocalNoiseFilter(currentImage, m.value, n.value, variance.value, borderType, basedOnHSI);
            manager.addImage(uri, currentImage);
            return true;
        });
    }
}
