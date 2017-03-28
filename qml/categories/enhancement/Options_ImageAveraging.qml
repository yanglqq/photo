import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Image Averaging"
    canApply: true

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Text {
                width: 65
                color: Style.normal.text.color
                font.pixelSize: Style.text.size.larger
                anchors.verticalCenter: parent.verticalCenter
                renderType: Text.NativeRendering
                text: "Gaussian"
            }
            Input {
                id: mean
                width: 80
                caption: "mean"
                text: "0"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
            Input {
                id: sigma
                width: 72
                caption: "sigma"
                text: "0"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Slider {
            id: count
            width: contentWidth
            label: "Images"
            minValue: 1
            maxValue: 256
            value: 1
            smallIncrement: false
            bins: 255
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = enhancement.average(manager.currentImage, mean.text, sigma.text, count.value);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
