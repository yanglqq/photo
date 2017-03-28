import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Order-Statistics"
    canApply: true

    function restrict() {
        var temp = m.value * n.value - 1;
        d.bins = d.maxValue = temp;
        if (d.value > temp) {
            d.updateValue(temp);
        } else {
            d.updateValue(d.value);
        }
    }

    optionModel: ObjectModel {
        Combox {
            id: combox
            width: 150
            index: 1
            model: ListModel {
                ListElement { name: "Max Filter" }
                ListElement { name: "Median Filter" }
                ListElement { name: "Midpoint Filter" }
                ListElement { name: "Min Filter" }
                ListElement { name: "Alpha-trimmed Mean Filter" }
                ListElement { name: "Majority Filter" }
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
            onValueChanged: restrict()
            enabled: combox.index != 5
        }
        Slider {
            id: n
            width: contentWidth
            label: "N"
            minValue: 1
            maxValue: 33
            value: 3
            bins: 32
            onValueChanged: restrict()
            enabled: combox.index != 5
        }
        Slider {
            id: d
            width: contentWidth
            label: "D"
            minValue: 0
            maxValue: 0
            value: 0
            bins: 1
            enabled: combox.index == 4
            smallIncrement: false
        }
    }
    onApply: {
        manager.execute(function(){
            var currentImage = manager.currentImage;
            var uri = currentImage;
            switch (combox.index) {
            case 0:
                uri = filters.maxFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 1:
                uri = filters.medianFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 2:
                uri = filters.midpointFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 3:
                uri = filters.minFilter(currentImage, m.value, n.value, borderType, basedOnHSI);
                break;
            case 4:
                uri = filters.alphaTrimmedMeanFilter(currentImage, m.value, n.value, d.value, borderType, basedOnHSI);
                break;
            case 5:
                uri = filters.majorityFilter(currentImage, borderType, basedOnHSI);
                break;
            }
            manager.addImage(uri, currentImage);
            return true;
        });
    }
}
