import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Mean"
    canApply: true

    optionModel: ObjectModel {
        Combox {
            id: combox
            width: 150
            index: 0
            model: ListModel {
                ListElement { name: "Arithmetic Mean Filter" }
                ListElement { name: "Geometric Mean Filter" }
                ListElement { name: "Harmonic Mean Filter" }
                ListElement { name: "Contraharmonic Mean Filter" }
            }
        }
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
            id: q
            width: contentWidth
            label: "Q"
            minValue: -33
            maxValue: 33
            value: 1
            bins: 66
            enabled: combox.index == 3
        }
    }
    onApply: {
        manager.execute(function(){
            var currentImage = manager.currentImage;
            var uri = currentImage;
            switch (combox.index) {
            case 0:
                uri = filters.arithmeticMeanFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 1:
                uri = filters.geometricMeanFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 2:
                uri = filters.harmonicMeanFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 3:
                uri = filters.contraharmonicMeanFilter(currentImage, m.value, n.value, q.value, borderType, basedOnHSI);
                break;
            }
            manager.addImage(uri, currentImage);
            return true;
        });
    }
}
