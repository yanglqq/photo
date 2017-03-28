import QtQuick 2.2
import QtQml.Models 2.1
import QtQuick.Dialogs 1.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Color Slicing"
    canApply: true
    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        onAccepted: colorPanel.color = colorDialog.currentColor
    }
    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: 15
            Rectangle {
                id: colorPanel
                width: 50
                height: parent.height - 2 * Style.margin.normal
                anchors.verticalCenter: parent.verticalCenter
                color: "yellow"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        colorDialog.setColor(colorPanel.color);
                        colorDialog.open();
                    }
                }
            }
            Checker {
                id: checker
                text: "Sphere"
                anchors.verticalCenter: parent.verticalCenter
                checked: false
                onClicked: slider.label = checker.checked ? "Radius" : "Width"
            }
        }
        Slider {
            id: slider
            width: contentWidth
            label: "Width"
            minValue: 0
            maxValue: 20
            value: 1
            bins: 400
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = colorTool.colorSlicing(manager.currentImage, colorPanel.color, slider.value, checker.checked);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
