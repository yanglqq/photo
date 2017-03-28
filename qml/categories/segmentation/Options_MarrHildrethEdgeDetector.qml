import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Marr-Hildreth Edge Detector"
    canApply: true

    property var laplacians: [
        [0, 1, 0, 1, -4, 1, 0, 1, 0],
        [1, 1, 1, 1, -8, 1, 1, 1, 1],
        [-1, -1, -1, 2, 2, 2, -1, -1, -1],
        [-1, 2, -1, -1, 2, -1, -1, 2, -1],
        [2, -1, -1, -1, 2, -1, -1, -1, 2],
        [-1, -1, 2, -1, 2, -1, 2, -1, -1]
    ]

    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: sigma
                width: 112
                caption: "Guassian standard deviation"
                text: "1"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            spacing: Style.margin.smaller
            Input {
                id: threshold
                width: 112
                caption: "threshold % of max pixel value"
                text: "4"
                validator: DoubleValidator { decimals: 3 }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Combox {
            id: combox
            width: 130
            index: 0
            model: ListModel {
                ListElement { name: "Laplacian 4-Neighbors" }
                ListElement { name: "Laplacian 8-Neighbors" }
                ListElement { name: "Laplacian Horizontal" }
                ListElement { name: "Laplacian Vertical" }
                ListElement { name: "Laplacian +45 Degree" }
                ListElement { name: "Laplacian -45 Degree" }
            }
            onSelected: laplacianKernel.model = laplacians[index]
        }
        ImageKernel {
            id: laplacianKernel
            width: contentWidth
            onCellChanged: laplacians[combox.index][index] = value
        }
    }
    onApply: {
        manager.execute(function(){
            var sigmaf = parseFloat(sigma.text);
            var thresholdf = parseFloat(threshold.text);
            if (thresholdf < 0 || thresholdf > 100) {
                manager.showMessage("0% <= threshold <= 100%");
                return false;
            }
            thresholdf *= imageTool.maxPixelValue(manager.currentImage) / 100;
            var laplacian = laplacians[combox.index];
            var uri = segmentation.marrHildreth(manager.currentImage, sigmaf, laplacian, thresholdf);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
