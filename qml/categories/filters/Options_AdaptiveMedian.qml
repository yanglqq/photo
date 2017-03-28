import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Adaptive Mean"
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
            onValueChanged: {
                if (m.value > mMax.value)
                    mMax.updateValue(m.value);
            }
        }
        Slider {
            id: n
            width: contentWidth
            label: "N"
            minValue: 1
            maxValue: 33
            value: 3
            bins: 32
            onValueChanged: {
                if (n.value > nMax.value)
                    nMax.updateValue(n.value);
            }
        }
        Slider {
            id: mMax
            width: contentWidth
            label: "M Max"
            minValue: 3
            maxValue: 33
            value: 3
            bins: 30
            onValueChanged: {
                if (m.value > mMax.value)
                    m.updateValue(mMax.value);
            }
        }
        Slider {
            id: nMax
            width: contentWidth
            label: "N Max"
            minValue: 3
            maxValue: 33
            value: 3
            bins: 30
            onValueChanged: {
                if (n.value > nMax.value)
                    n.updateValue(nMax.value);
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var currentImage = manager.currentImage;
            var uri = filters.adaptiveMeanFilter(currentImage, m.value, n.value, mMax.value, nMax.value, borderType, basedOnHSI);
            manager.addImage(uri, currentImage);
            return true;
        });
    }
}
