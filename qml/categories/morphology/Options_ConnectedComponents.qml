import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Connected Components"
    canApply: true

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: pixelValue
                width: 102
                caption: "pixel value"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
            Checker {
                id: showAll
                text: "show all"
                anchors.verticalCenter: parent.verticalCenter
                checked: false
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var pixelValuei = parseFloat(pixelValue.text);
            var uri = morphology.connectedComponents(manager.currentImage, pixelValuei, !showAll.checked);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
