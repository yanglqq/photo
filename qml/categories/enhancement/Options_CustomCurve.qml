import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Custom Curve Transformation"
    canApply: true
    optionModel: ObjectModel {
        Curve {
            id: curve
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
            slippers : [
                Slipper {
                    xp: 0.0
                    yp: 0.0
                },
                Slipper {
                    xp: 0.2
                    yp: 0.1
                },
                Slipper {
                    xp: 0.5
                    yp: 0.5
                },
                Slipper {
                    xp: 0.8
                    yp: 0.9
                },
                Slipper {
                    xp: 1.0
                    yp: 1.0
                }
            ]
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
