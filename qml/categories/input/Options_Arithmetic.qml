import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    title: "Arithmetic Operations"

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
                tip: "image1 and image2"
                source: "../../../image/dark/icons/apply.png"
                text: "and"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.AND(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "image1 or image2"
                source: "../../../image/dark/icons/apply.png"
                text: "or"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.OR(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "image1 xor image2"
                source: "../../../image/dark/icons/apply.png"
                text: "xor"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.XOR(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "not image"
                source: "../../../image/dark/icons/apply.png"
                text: "not"
                onClicked: {
                    manager.execute(function() {
                        var uri = imageTool.NOT(manager.currentImage);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }            
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: 10
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "image1 + image2"
                source: "../../../image/dark/icons/apply.png"
                text: "add"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.add(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "image1 - image2"
                source: "../../../image/dark/icons/apply.png"
                text: "subtract"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.subtract(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "image1 * image2"
                source: "../../../image/dark/icons/apply.png"
                text: "multiply"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.multiply(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: 10
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "the point-wise maximum of image1 and image2"
                source: "../../../image/dark/icons/apply.png"
                text: "max"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.max(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "the point-wise minimum of image1 and image2"
                source: "../../../image/dark/icons/apply.png"
                text: "min"
                onClicked: {
                    manager.execute(function() {
                        var selected = manager.getSelectedImages();
                        if (!checkImages(selected))
                            return false;
                        var uri = imageTool.min(selected[0], selected[1]);
                        manager.addImage(uri, null, selected[1]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                enabled: manager.enabled
                tip: "get the absolute value of the image"
                source: "../../../image/dark/icons/apply.png"
                text: "abs"
                onClicked: {
                    manager.execute(function() {
                        var uri = imageTool.abs(manager.currentImage);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
        }
    }
}
