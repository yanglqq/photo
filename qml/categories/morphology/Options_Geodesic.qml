import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Geodesic"
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
        Slider {
            id: iterations
            width: contentWidth
            label: "Number of Iterations"
            smallIncrement: false
            minValue: 1
            maxValue: 100
            value: 1
            bins: 99
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: 10
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "erode"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var se = ses;
                        var uri = morphology.geodesicErode(se.elements, se.m, se.n, se.origin, selected[0], selected[1], iterations.value);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "dilate"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var se = ses;
                        var uri = morphology.geodesicDilate(se.elements, se.m, se.n, se.origin, selected[0], selected[1], iterations.value);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
        }
    }
}
