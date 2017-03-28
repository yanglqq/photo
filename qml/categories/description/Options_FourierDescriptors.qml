import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Fourier Descriptors"
    canApply: true

    optionModel: ObjectModel {
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: boundaryPixelValue
                width: 102
                caption: "boundary pixel value"
                text: "255"
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            width: contentWidth
            height: Style.category.optionItemHeight
            Input {
                id: descriptors
                width: 102
                caption: "% of descriptors"
                text: "10"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    onApply: {
        manager.execute(function(){
            var descriptorsf = parseFloat(descriptors.text);
            if (descriptorsf < 0 || descriptorsf > 100) {
                manager.showMessage("0% <= descriptors <= 100%");
                return false;
            }
            descriptorsf /= 100;
            var boundaryPixelValuei = parseInt(boundaryPixelValue.text);
            var uri = description.fourierDescriptors(manager.currentImage, boundaryPixelValuei, descriptorsf);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
