import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Contrast Stretching"
    canApply: true
    optionModel: ObjectModel {
        ContrastStretching {
            id: contrastStretching
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
            uri: manager.currentImage
        }
    }
    onExpand: contrastStretching.update()
    onApply: {
        manager.execute(function(){
            var uri = contrastStretching.processImage(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
