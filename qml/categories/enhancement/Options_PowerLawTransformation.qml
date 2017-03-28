import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Power-Law Transformation"
    canApply: true
    DoubleValidator {
       id: validator
       bottom: 0
       decimals: 3
    }
    optionModel: ObjectModel {
        Text {
            width: contentWidth
            color: Style.normal.text.color
            font.pixelSize: Style.text.size.normal
            font.bold: true
            renderType: Text.NativeRendering
            text: "s = c r^gamma"
        }
        Slider {
            width: contentWidth
            label: "gamma"
            minValue: 0
            maxValue: 5
            bins: 20
            value: 1
            onValueChanged: {
                powerLaw.gamma = value;
                powerLaw.update();
            }
        }
        PowerLawTransformation {
            id: powerLaw
            width: contentWidth
            height: contentWidth
            resultNew: manager.resultNew
        }
    }
    onApply: {
        manager.execute(function(){
            var uri = powerLaw.processImage(manager.currentImage);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
