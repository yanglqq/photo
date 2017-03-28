import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Histogram Matching"
    canApply: true
    optionModel: ObjectModel {
        HistogramMatching {
            id: histogramMatching
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
            slippers : [
                Slipper {
                    xp: 0
                    yp: 0.3
                },
                Slipper {
                    xp: 0.5
                    yp: 0.3
                },
                Slipper {
                    xp: 1
                    yp: 0.3
                }
            ]
            CurveMenu {
                id: menu
                curveControl: histogramMatching
                showMaskMenuItem: false
            }
            onContextMenu: menu.popup();
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = histogramMatching.processImage(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
