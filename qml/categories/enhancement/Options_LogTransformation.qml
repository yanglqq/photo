import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Log Transformation"
    canApply: true
    optionModel: ObjectModel {
        Text {
            color: Style.normal.text.color
            font.pixelSize: Style.text.size.normal
            font.bold: true
            renderType: Text.NativeRendering
            text: "s = c log(1 + r)"
        }
        LogTransformation {
            id: logTransformation
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = logTransformation.processImage(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
