import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Reconstruction"
    canApply: false

    function checkImages(images) {
        if (images.length < 2) {
            manager.showMessage("please select two images first!");
            return false;
        }
        if (!imageTool.same(images[0], images[1])) {
            manager.showMessage("the two images must be the same configuration!");
            return false;
        }
        return true;
    }

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: 10
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "erosion"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var se = ses;
                        var uri = morphology.reconstructByErosion(se.elements, se.m, se.n, se.origin, selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "dilation"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var se = ses;
                        var uri = morphology.reconstructByDilation(se.elements, se.m, se.n, se.origin, selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
        }
    }
}
