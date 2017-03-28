import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Derivatives"
    canApply: true

    property var kernels: [
        [0, 1, 0, 1, -4, 1, 0, 1, 0],
        [1, 1, 1, 1, -8, 1, 1, 1, 1],
        [-1, -1, -1, 2, 2, 2, -1, -1, -1],
        [-1, 2, -1, -1, 2, -1, -1, 2, -1],
        [2, -1, -1, -1, 2, -1, -1, -1, 2],
        [-1, -1, 2, -1, 2, -1, 2, -1, -1],
        [0, -1, 0, -1, 5, -1, 0, -1, 0],
        [-1, -1, -1, -1, 9, -1, -1, -1, -1],
        [0, -1, 1, 0],
        [-1, 0, 0, 1],
        [-1, -1, -1, 0, 0, 0, 1, 1, 1],
        [-1, 0, 1, -1, 0, 1, -1, 0, 1],
        [0, 1, 1, -1, 0, 1, -1, -1, 0],
        [-1, -1, 0, -1, 0, 1, 0, 1, 1],
        [-1, -2, -1, 0, 0, 0, 1, 2, 1],
        [-1, 0, 1, -2, 0, 2, -1, 0, 1],
        [0, 1, 2, -1, 0, 1, -2, -1, 0],
        [-2, -1, 0, -1, 0, 1, 0, 1, 2],
        [0,0,1,0,0,0,1,2,1,0,1,2,-16,2,1,0,1,2,1,0,0,0,1,0,0]
    ]
    property var columns: [3,3,3,3,3,3,3,3,2,2,3,3,3,3,3,3,3,3,5]
    optionModel: ObjectModel {
        Combox {
            id: combox
            width: contentWidth
            index: 0
            model: ListModel {
                ListElement { name: "Laplacian 4-Neighbors" }
                ListElement { name: "Laplacian 8-Neighbors" }
                ListElement { name: "Laplacian Horizontal" }
                ListElement { name: "Laplacian Vertical" }
                ListElement { name: "Laplacian +45 Degree" }
                ListElement { name: "Laplacian -45 Degree" }
                ListElement { name: "Laplacian Sharpen 4-Neighbors" }
                ListElement { name: "Laplacian Sharpen 8-Neighbors" }
                ListElement { name: "Roberts cross-gradient +45 Degree" }
                ListElement { name: "Roberts cross-gradient -45 Degree" }
                ListElement { name: "Prewitt Horizontal" }
                ListElement { name: "Prewitt Vertical" }
                ListElement { name: "Prewitt +45 Degree" }
                ListElement { name: "Prewitt -45 Degree" }
                ListElement { name: "Sobel Horizontal" }
                ListElement { name: "Sobel Vertical" }
                ListElement { name: "Sobel +45 Degree" }
                ListElement { name: "Sobel -45 Degree" }
                ListElement { name: "Laplacian of a Gaussian" }
            }
            onSelected: {
                imageKernel.columns = columns[index];
                imageKernel.model = kernels[index];
            }
        }
        ImageKernel {
            id: imageKernel
            width: contentWidth
            onCellChanged: kernels[combox.index][index] = value
        }
    }
    onApply: {
        manager.execute(function() {
            var kernel = kernels[combox.index];
            var uri = filters.custom(manager.currentImage, kernel, borderType, basedOnHSI);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
