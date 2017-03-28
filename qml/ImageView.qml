import QtQuick 2.2
import QtQml.Models 2.1
import "themes"
import "components"

Item {
    id: container

    property alias editable: imageGrid.interactive
    signal highlighted(var uri);
    signal highlightingCleared;
    signal imageSelected(var uri);
    signal imageUnselected(var uri);

    function zoomIn() {
        if (imageGrid.columns > 1) {
            imageGrid.columns--;
            imageGrid.cellWidth = imageGrid.width / imageGrid.columns;
            imageGrid.cellHeight = imageGrid.height / imageGrid.columns;
        }
    }
    function zoomOut() {
        if (imageGrid.columns < imageGrid.maxColumns) {
            imageGrid.columns++;
            imageGrid.cellWidth = imageGrid.width / imageGrid.columns;
            imageGrid.cellHeight = imageGrid.height / imageGrid.columns;
        }
    }

    function refreshImage(uri) {
        var model = imageGrid.model;
        for (var i = 0; i < model.count; i++) {
            var item = model.get(i);
            if (item.uri == uri) {
                imageGrid.currentIndex = i;
                imageGrid.currentItem.refresh();
                break;
            }
        }
    }

    GridView {
        property int maxColumns: 3
        property int columns: 0

        id: imageGrid
        model: manager.imageList
        anchors.fill: parent
        function fitColumns() {
            columns = count > maxColumns ? maxColumns : count;
            if (columns == 0) {
                columns = 1;
                return;
            }
            cellWidth = width / columns;
            cellHeight = height / columns;
        }
        highlight: Rectangle {
            color: "lightsteelblue";
            radius: 5
            opacity: 0.2
        }
        focus: true

        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
        }

        Keys.onReleased: {
            if (event.key == Qt.Key_Left ||
                event.key == Qt.Key_Up ||
                event.key == Qt.Key_Right ||
                event.key == Qt.Key_Down) {
                highlighted(manager.imageList.get(imageGrid.currentIndex).uri);
                event.accepted = false;
            }
        }

        delegate: MouseArea {
            property int itemIndex: DelegateModel.itemsIndex
            id: mouseArea
            width: imageGrid.cellWidth - 2 * Style.margin.normal
            height: imageGrid.cellHeight - 2 * Style.margin.normal
            enabled: imageGrid.interactive

            function refresh() {
               image.refresh();
            }

            GridView.onAdd: {
                imageGrid.fitColumns();
                if (mouseArea.itemIndex == 0) {
                    highlighted(manager.imageList.get(mouseArea.itemIndex).uri);
                }
            }
            GridView.onRemove: {
                imageGrid.fitColumns();
                if (manager.imageList.count > 0) {
                    var i = mouseArea.itemIndex;
                    if (i == imageGrid.currentIndex) {
                        if (i == manager.imageList.count)
                            i--;
                        highlighted(manager.imageList.get(i).uri);
                    }
                }
                else
                    highlightingCleared();
            }
            onClicked: {
                if (imageGrid.currentIndex != mouseArea.itemIndex) {
                    imageGrid.currentIndex = mouseArea.itemIndex;
                    highlighted(manager.imageList.get(mouseArea.itemIndex).uri);
                }
            }
            drag.target: image
            ImageItem {
                id: image
                source: uri
                title: caption
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter
                enabled: !imageGrid.interactive
                onRemove: {
                    manager.remove(manager.imageList.get(index).uri);
                    manager.imageList.remove(index);
                }
                onSave: manager.saveImage(image.source)
                onSelected: {
                    manager.imageList.get(index).selected = true;
                    imageSelected(image.source);
                }
                onUnselected: {
                    manager.imageList.get(index).selected = false;
                    imageUnselected(image.source);
                }
                Drag.active: mouseArea.drag.active
                Drag.source: mouseArea
                states: [
                    State {
                        when: image.Drag.active
                        ParentChange {
                            target: image
                            parent: imageGrid
                            width: mouseArea.width
                            height: mouseArea.width
                        }
                        AnchorChanges {
                            target: image;
                            anchors.horizontalCenter: undefined;
                            anchors.verticalCenter: undefined
                        }
                    }
                ]
            }
            DropArea {
                anchors.fill: parent
                onEntered: manager.imageList.move(drag.source.itemIndex, mouseArea.itemIndex, 1)
            }
        }
    }
}
