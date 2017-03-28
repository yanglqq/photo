import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQml.Models 2.1
import "../../components"
import "../../themes"
import ".."

Options {
    title: "Open File"
    optionModel: ObjectModel {
        Row {
            width: contentWidth
            height: Style.category.optionItemHeight
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "open image files"
                source: "../../../image/dark/icons/load-image.png"
                text: "Open Images"
                onClicked: fileDialog.open()
            }
        }
    }
    FileDialog {
        id: fileDialog
        title: "Please choose image files"
        nameFilters: manager.filters
        selectMultiple: true
        onAccepted: manager.loadImages(fileDialog.fileUrls)
        Component.onCompleted: visible = false
    }
}
