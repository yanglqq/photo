import QtQuick 2.2
import QtQml.Models 2.1
import QtQuick.Dialogs 1.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Intensity Slicing"
    canApply: true
    optionModel: ObjectModel {
        IntensityColouring {
            id: colouring
            property int colorIndex: 0
            width: contentWidth
            height: 25
            resultNew: manager.resultNew
            onColorDialog: {
                colouring.colorIndex = index;
                colorDialog.setColor(color);
                colorDialog.open();
            }
            ColorDialog {
                id: colorDialog
                title: "Please choose a color"
                onAccepted: colouring.setColor(colouring.colorIndex, colorDialog.currentColor)
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = colouring.colour(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
