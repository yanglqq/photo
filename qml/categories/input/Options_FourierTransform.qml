import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    title: "Fourier Transform"

    optionModel: ObjectModel {
        IconButton {
            width: contentWidth
            enabled: manager.enabled
            source: "../../../image/dark/icons/apply.png"
            text: "Fourier Phase Angle"
            onClicked: {
                manager.execute(function(){
                    var uri = imageTool.fourierPhaseAngle(manager.currentImage);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }
        IconButton {
            width: contentWidth
            enabled: manager.enabled
            tip: "reconstruct an image using its spectrum"
            source: "../../../image/dark/icons/apply.png"
            text: "Reconstruct from spectrum"
            onClicked: {
                manager.execute(function(){
                    var uri = imageTool.reconstructUsingSpectrum(manager.currentImage);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }
        IconButton {
            width: contentWidth
            enabled: manager.enabled
            tip: "reconstruct an image using its phase angle"
            source: "../../../image/dark/icons/apply.png"
            text: "Reconstruct from phase angle"
            onClicked: {
                manager.execute(function(){
                    var uri = imageTool.reconstructUsingPhaseAngle(manager.currentImage);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }
    }
}
