import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQml.Models 2.1
import "../../components"
import "../../themes"
import ".."

Options {
    title: "Split Components"
    optionModel: ObjectModel {
        IconButton {
            width: contentWidth
            source: "../../../image/dark/icons/apply.png"
            text: "RGB Components"
            onClicked: {
                manager.execute(function() {
                    var uris = colorTool.splitRGB(manager.currentImage);
                    if (uris.length == 0)
                        return false;
                    manager.addImage(uris[0], manager.currentImage, manager.currentImage, "Red");
                    manager.addImage(uris[1], manager.currentImage, uris[0], "Green");
                    manager.addImage(uris[2], manager.currentImage, uris[1], "Blue");
                    return true;
                });
            }
        }
        IconButton {
            width: contentWidth
            source: "../../../image/dark/icons/apply.png"
            text: "HSI Components"
            onClicked: {
                manager.execute(function() {
                    var uris = colorTool.splitHSI(manager.currentImage);
                    if (uris.length == 0)
                        return false;
                    manager.addImage(uris[0], manager.currentImage, manager.currentImage, "Hue");
                    manager.addImage(uris[1], manager.currentImage, uris[0], "Saturation");
                    manager.addImage(uris[2], manager.currentImage, uris[1], "Intensity");
                    return true;
                });
            }
        }
        IconButton {
            width: contentWidth
            source: "../../../image/dark/icons/apply.png"
            text: "CMYK Components"
            onClicked: {
                manager.execute(function() {
                    var uris = colorTool.splitCMYK(manager.currentImage);
                    if (uris.length == 0)
                        return false;
                    manager.addImage(uris[0], manager.currentImage, manager.currentImage, "Cyan");
                    manager.addImage(uris[1], manager.currentImage, uris[0], "Magenta");
                    manager.addImage(uris[2], manager.currentImage, uris[1], "Yellow");
                    manager.addImage(uris[3], manager.currentImage, uris[2], "Black");
                    return true;
                });
            }
        }
    }
}
