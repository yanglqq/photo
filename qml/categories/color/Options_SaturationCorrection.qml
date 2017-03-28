import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Saturation Correction"
    canApply: true
    optionModel: ObjectModel {
        SaturationCorrection {
            id: curve
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
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
