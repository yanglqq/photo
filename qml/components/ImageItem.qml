import QtQuick 2.2
import Polka 1.0
import "../themes"

Item {
    id: container

    property alias source: image.source
    property alias enabled: imageMask.enabled
    property alias title: imageTitle.text

    signal remove
    signal save
    signal selected
    signal unselected

    function refresh() {
        image.update();
        imageMask.update();
    }

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        cache: false
        ImageMask {
            id: imageMask
            uri: source
            width: parent.paintedWidth
            height: parent.paintedHeight
            x: (parent.width - parent.paintedWidth) / 2
            y: (parent.height - parent.paintedHeight) / 2
            enabled: false
        }
    }
    Text {
        id: imageTitle
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Style.margin.normal
        color: "lightBlue"
        font.pixelSize: 16
        font.bold: true
        style: Text.Raised
        styleColor: "black"
        renderType: Text.NativeRendering
    }
    Column {
        id: imageBar
        width: 20
        height: parent.height
        spacing: Style.margin.normal
        anchors.right: parent.right
        IconButton {
            tip: "remove this image"
            source: "../../image/dark/icons/image-remove.png"
            onClicked: remove()
        }
        SwitchButton {
            on: false
            tip: "select/unselect this image"
            onStateImage: "../../image/dark/icons/indicator-checked.png"
            offStateImage: "../../image/dark/icons/indicator-unchecked.png"
            onStateChanged:{
                if (on)
                    selected();
                else
                    unselected();
            }
        }
        IconButton {
            tip: "save this image"
            source: "../../image/dark/icons/save-image.png"
            onClicked: save()
        }
    }
}
