import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Canny Edge Detector"
    canApply: true

    optionModel: ObjectModel {  
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: lowerThreshold
                width: 127
                caption: "lower threshold"
                text: "100"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: higherThreshold
                width: 127
                caption: "higher threshold"
                text: "300"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var lowerThresholdf = parseFloat(lowerThreshold.text);
            var higherThresholdf = parseFloat(higherThreshold.text);
            var uri = segmentation.canny(manager.currentImage, lowerThresholdf, higherThresholdf);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
