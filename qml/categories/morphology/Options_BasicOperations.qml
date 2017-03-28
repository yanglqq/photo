import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Basic Operations"
    canHelp: true
    canApply: false

    Component.onCompleted: {
        var text = [
            "Opening suppresses bright details smaller than the specified SE and closing suppresses dark details. They are used often in combination as morphological filters for image smoothing and noise removal."
        ];
        container.helpText = text.join('\n');
    }

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
                        var se = ses;
                        var uri = morphology.erode(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
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
                        var se = ses;
                        var uri = morphology.dilate(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "open"
                onClicked: {
                    manager.execute(function() {
                        var se = ses;
                        var uri = morphology.open(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                source: "../../../image/dark/icons/apply.png"
                text: "close"
                onClicked: {
                    manager.execute(function() {
                        var se = ses;
                        var uri = morphology.close(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
        }
    }
}
