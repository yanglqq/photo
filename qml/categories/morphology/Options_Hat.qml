import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Hat"
    canApply: false

    optionModel: ObjectModel {
        Slider {
            id: iterations
            width: contentWidth
            label: "Number of Iterations"
            minValue: 1
            maxValue: 10
            value: 1
            bins: 9
        }

        Row {
            width: parent.width
            height: Style.category.optionItemHeight
            spacing: 10
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                tip: "image - opening"
                text: "tophat"
                onClicked: {
                    manager.execute(function() {
                        var se = ses;
                        var uri = morphology.tophat(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                tip: "closing - image"
                text: "bottomhat"
                onClicked: {
                    manager.execute(function() {
                        var se = ses;
                        var uri = morphology.bottomhat(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
        }
    }
}
