import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Region Splitting Merging"
    canApply: true

    optionModel: ObjectModel {
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: maxValue
                width: 102
                caption: "max value"
                text: "255"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Slider {
            id: m
            width: contentWidth
            label: "minimum width"
            minValue: 1
            maxValue: 101
            bins: 50
            value: 3
        }
        Slider {
            id: n
            width: contentWidth
            label: "minimum height"
            minValue: 1
            maxValue: 101
            bins: 50
            value: 3
        }
        Text {
            width: contentWidth
            color: Style.normal.text.color
            font.pixelSize: Style.text.size.larger;
            renderType: Text.NativeRendering
            text: "javascript"
        }
        Rectangle {
            id: code
            width: contentWidth
            height: contentWidth / 2
            color: Style.normal.baseColor
            radius: 3
            border.color: Style.normal.border.color
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#333333" }
                GradientStop { position: 1.0; color: Style.normal.tabButton.highlightedGradientColor1 }
            }
            MouseArea {
                hoverEnabled: true
                anchors.fill: parent
                onEntered: {
                   parent.border.color = Style.hover.border.color;
                }
                onExited: {
                   parent.border.color = Style.normal.border.color;
                }
            }
            TextInput {
                id: input
                y: 2
                width: parent.width - 2 * Style.margin.smaller
                height: parent.height - 2 * Style.margin.smaller
                wrapMode: TextInput.Wrap
                selectByMouse: true
                color: Style.normal.text.color
            }
        }
    }

    onApply: {
        manager.execute(function(){
            var maxValuef = parseFloat(maxValue.text);
            var weightOfMeanf = parseFloat(weightOfMean.text);
            var uri = segmentation.movingAverage(manager.currentImage, maxValuef, weightOfMeanf, n.value);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
