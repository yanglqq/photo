import QtQuick 2.2
import QtQml.Models 2.1

import "themes"
import "components"
import "categories"
import "categories/input"
import "categories/enhancement"
import "categories/filters"
import "categories/color"
import "categories/multiresolution"
import "categories/morphology"
import "categories/segmentation"
import "categories/description"
import "categories/genius"

Item {
    id: container

    property var categoryNames: ["Input", "Enhancement", "Filters", "Color", "Multiresolution", "Morphology", "Segmentation", "Description", "Genius"]

    Column {
        id: tabButtonsColumn
        width: Style.category.barWidth
        height: parent.height
        spacing: Style.margin.smaller
        Repeater {
            id: categoryNamesRepeater
            model: categoryNames
            TabButton {
                label: modelData
                onClicked: {
                    for(var i = 0; i < categoryNamesRepeater.count; i++) {
                        var button = categoryNamesRepeater.itemAt(i);
                        if (button.hasSelected) {
                            button.clearSelectedState();
                        }
                        var content = categoryContentsColumn.children[i];
                        if (content.visible) {
                            content.visible = false;
                        }
                    }
                    setSelectedState();
                    categoryContentsColumn.children[index].visible = true;
                }
            }
        }
    }
    ObjectModel {
        id: categoryContents
        Input {
            id: input
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Enhancement {
            id: enhancement
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Filters {
            id: filters
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Color {
            id: color
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Multiresolution {
            id: multiresolution
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Morphology {
            id: morphology
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Segmentation {
            id: segmentation
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Description {
            id: description
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
        Genius {
            id: genius
            visible: false
            width: categoryContentsColumn.width
            height: categoryContentsColumn.height
        }
    }
    Column {
        id: categoryContentsColumn
        width: parent.width - tabButtonsColumn.width
        height: parent.height
        anchors.left: tabButtonsColumn.right
        Repeater {
            model: categoryContents
        }
    }
    Component.onCompleted: {
        categoryNamesRepeater.itemAt(0).setSelectedState();
    }
}
