import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Tonal Correction Yellow"
    canApply: true
    optionModel: ObjectModel {
        TonalCorrection {
            id: curve
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
            component: TonalCorrection.Yellow
            CurveMenu {
                id: curveMenu
                curveControl: curve
                showMaskMenuItem: false
            }
            onContextMenu: curveMenu.popup()
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = curve.processImage(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
