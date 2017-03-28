import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Subband"
    canApply: true

    optionModel: ObjectModel {
        Item {
            width: parent.width
            height: Style.category.optionItemHeight
            Combox {
                id: combox
                width: 160
                index: 0
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                model: ListModel {
                    ListElement { name: "Daubechies" }
                    ListElement { name: "Symlet" }
                    ListElement { name: "Cohen Daubechies Feauveau" }
                }
            }
            Checker {
                id: checker
                text: "Inverse"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                checked: false
            }
        }
    }
    onApply: {
        if (checker.checked) {
            var selected = manager.getSelectedImages();
            if (selected.length < 4) {
                manager.showMessage("you must choose four images!");
                return;
            }
            manager.execute(function() {
                var uri = multiresolution.subbandDecoding(selected[0], selected[1], selected[2], selected[3], combox.index, borderType);
                manager.addImage(uri, null, selected[3]);
                return true;
            });
        } else {
            manager.execute(function(){
                var uris = multiresolution.subbandCoding(manager.currentImage, combox.index, borderType);
                manager.addImage(uris[0], manager.currentImage, manager.currentImage, "approximation");
                manager.addImage(uris[1], manager.currentImage, uris[0], "vertical detail");
                manager.addImage(uris[2], manager.currentImage, uris[1], "horizontal detail");
                manager.addImage(uris[3], manager.currentImage, uris[2], "diagonal detail");
                return true;
            });
        }
    }
}
