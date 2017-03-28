import QtQuick 2.2
import QtQuick.Dialogs 1.1
import Polka 1.0
import "components"
import "categories/enhancement"
import "themes"
import "js/json.js" as JSON

Rectangle {
    id: root

    property var root: root
    property var manager: imageManager

    color: Style.normal.baseColor
    /**
    ExceptionHandler {
        ignoreWarning: true
        onException: hintMessage.error(message)
    }
    **/
    FileSaveDialog {
        id: fileSaveDialog

        property string uri: ""

        title: "Save image"
        filters: manager.filters
        onAccepted: {
            if (manager.save(fileSaveDialog.uri, fileSaveDialog.fileUrl))
                processingMessage.state = "done";
        }
    }
    ImageTool {
        id: imageTool
        resultNew: manager.resultNew
    }
    ImageManager {
        id: imageManager

        property bool     enabled: false
        property string   currentImage: ""
        property QtObject imageList: ListModel {}
        property var      uriPattern: /image:\/\/polka\/(.+)__\d+/
        property var      filters: ["Image files (*.jpg *.jpeg *.png *.gif *.bmp *.tif)"]

        Timer {
            id: delayTimer
            interval: 30
            running: false
            repeat: false
            property var callback: null
            function execute(func) {
                callback = func;
                running = true;
            }
            onTriggered:  {
                callback();
                running = false;
            }
        }

        function showHelp(text) {
            hintMessage.help(text);
        }

        function showMessage(message) {
            hintMessage.hint(message);
        }

        function loadImages(fileUrls) {
            processingMessage.state = "loading";
            delayTimer.execute(function(){
                for (var i = 0; i < fileUrls.length; i++) {
                    manager.addImage(fileUrls[i]);
                }
                processingMessage.state = "hide";
            });
        }

        function execute(callback) {
            processingMessage.state = "processing";
            delayTimer.execute(function() {
                processingMessage.state = callback() ? "done" : "error";
            });
        }

        function addImage(uri, owner, relative, title, generated, type) {
            for (var i = 0; i < imageList.count; i++) {
                var item = imageList.get(i);
                if (item.uri == uri) {
                    var newItem = {
                        uri: item.uri,
                        owner: item.owner,
                        caption: item.caption,
                        selected: item.selected,
                        type: item.type
                    };
                    imageList.set(i, {uri: "", caption: ""});
                    imageList.set(i, newItem);
                    histogram.draw(newItem.uri);
                    return;
                }
            }
            if (!relative) relative = owner;
            var newItem = {
                uri: uri,
                owner: owner,
                caption: title ? title : "",
                selected: false,
                type: type
            };
            if (relative) {
                for (var i = 0; i < imageList.count; i++) {
                    var item = imageList.get(i);
                    if (item.uri == relative) {
                        imageList.insert(i + 1, newItem);
                        return;
                    }
                }
            }
            if (generated)
                imageList.append(newItem);
            else {
                uri = manager.load(uri);
                if (uri.length > 0) {
                    newItem.uri = uri;
                    imageList.append(newItem);
                }
            }
        }

        function saveImage(uri) {
            var result = uriPattern.exec(uri);
            if (result) {
                fileSaveDialog.uri = uri;
                fileSaveDialog.filename = result[1];
                fileSaveDialog.open();
            }
        }

        function getSelectedImages() {
            var selected = [];
            for (var i = 0; i < imageList.count; i++) {
                var item = imageList.get(i);
                if (item.selected) {
                    selected.push(item.uri);
                }
            }
            return selected;
        }

        function getImageItem(uri) {
            for (var i = 0; i < imageList.count; i++) {
                var item = imageList.get(i);
                if (item.uri == uri) {
                    return item;
                }
            }
        }

        function imageExists(uri) {
            for (var i = 0; i < imageList.count; i++) {
                var item = imageList.get(i);
                if (item.uri == uri) {
                    return true;
                }
            }
            return false;
        }

        function refreshImage(uri) {
            imageView.refreshImage(uri);
        }
    }
    Column {
        id: left
        width: Style.category.width
        height: parent.height - 2 * Style.margin.normal
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: Style.margin.normal
        spacing: Style.margin.normal
        Rectangle {
            width: Style.category.width
            height: Style.histogram.height
            color: "transparent"
            border.color: Style.normal.border.color
            border.width: Style.border.width
            Histogram {
                id: histogram
                anchors.fill: parent
                anchors.margins: parent.border.width
                backgroundColor: Style.normal.histogram.backgroudColor
            }
        }
        Row {
            id: toolbarTop
            width: Style.category.width
            height: 20
            spacing: Style.margin.normal
            layoutDirection: Qt.RightToLeft
            SwitchButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "enable/disable edition"
                on: false
                onStateImage: "../../image/dark/icons/edit-enable.png"
                offStateImage: "../../image/dark/icons/edit-disable.png"
                onStateChanged: imageView.editable = !on
            }
            SwitchButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "add/don't add an image after processing"
                on: true
                onStateImage: "../../image/dark/icons/result-new.png"
                offStateImage: "../../image/dark/icons/result-old.png"
                onStateChanged: manager.resultNew = on
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "crop an image"
                source: "../image/dark/icons/crop.png"
                enabled: manager.enabled
                onClicked: {
                    var currentImage = manager.currentImage;
                    if (imageTool.countImageMasks(currentImage) == 0) {
                        manager.showMessage("Please draw some rectangles on the selected image!");
                        return;
                    }
                    manager.execute(function() {                        
                        var uris = imageTool.crop(currentImage);
                        for (var i = uris.length - 1; i >= 0; i--) {
                            manager.addImage(uris[i], null, currentImage);
                        }
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "convert a RGB image to a gray image"
                source: "../image/dark/icons/rgb2gray.png"
                enabled: manager.enabled
                onClicked: {
                    manager.execute(function(){
                        var uri = imageTool.rgb2gray(manager.currentImage);
                        manager.addImage(uri, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "combine three gray images to a RGB image"
                source: "../image/dark/icons/gray2rgb.png"
                enabled: manager.enabled
                onClicked: {
                    var selected = manager.getSelectedImages();
                    if (selected.length < 3) {
                        manager.showMessage("you must choose three gray images!");
                        return;
                    }
                    manager.execute(function() {
                        var uri = imageTool.gray2rgb(selected[0], selected[1], selected[2]);
                        manager.addImage(uri, null, selected[2]);
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "Fourier Spectrum"
                enabled: manager.enabled
                source: "../image/dark/icons/fourier-transform.png"                
                onClicked: {
                    manager.execute(function(){
                        var image = manager.currentImage;
                        var uri = imageTool.fourierSpectrum(image);
                        manager.addImage(uri, image, image, "", true, "fourierSpectrum");
                        return true;
                    });
                }
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "downsampling"
                source: "../image/dark/icons/downsampling.png"
                enabled: manager.enabled
                onClicked: {
                    manager.showHelp(JSON.stringify(imageTool.statistics(manager.currentImage), null, '\t'));
                }
            }
            IconButton {
                enabled: manager.enabled
                tip: "Histogram Equalization"
                source: "../image/dark/icons/histogram-equalization.png"
                onClicked: {
                    manager.execute(function(){
                        var uri = imageTool.histogramEqualization(manager.currentImage);
                        manager.addImage(uri, null, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                enabled: manager.enabled
                tip: "Normalize 0-255"
                source: "../image/dark/icons/255.png"
                onClicked: {
                    manager.execute(function(){
                        var uri = imageTool.normalize(manager.currentImage);
                        manager.addImage(uri, null, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                enabled: manager.enabled
                tip: "Add 128"
                source: "../image/dark/icons/128.png"
                onClicked: {
                    manager.execute(function(){
                        var uri = imageTool.add128(manager.currentImage);
                        manager.addImage(uri, null, manager.currentImage);
                        return true;
                    });
                }
            }
            IconButton {
                enabled: manager.enabled
                tip: "Convert to 8U"
                source: "../image/dark/icons/8u.png"
                onClicked: {
                    manager.execute(function(){
                        var uri = imageTool.to8u(manager.currentImage);
                        manager.addImage(uri, null, manager.currentImage);
                        return true;
                    });
                }
            }
        }
        CategoryView {
            id: categoryView
            width: Style.category.width
            height: parent.height - 3 * Style.margin.normal - toolbarTop.height - toolbarBottom.height - histogram.height
        }
        Row {
            id: toolbarBottom
            width: Style.category.width
            height: 20
            spacing: Style.margin.normal
            layoutDirection: Qt.RightToLeft
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "Zoom in"
                source: "../image/dark/icons/zoom-in.png"
                onClicked: imageView.zoomIn()
            }
            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                tip: "Zoom out"
                source: "../image/dark/icons/zoom-out.png"
                onClicked: imageView.zoomOut()
            }
        }
    }
    ImageView {
        id: imageView
        width: parent.width - Style.category.width - 4 * Style.margin.normal
        height: parent.height - 2 * Style.margin.normal
        anchors.left: left.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: Style.margin.normal
        onHighlighted: {
            manager.enabled = true;
            manager.currentImage = uri;
            histogram.visible = true;
            histogram.draw(uri);
        }
        onHighlightingCleared: {
            manager.enabled = false;
            manager.currentImage = "";
            histogram.visible = false;
            histogram.clear();
        }
    }
    ProcessingMessage {
        id: processingMessage
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
    }
    HintMessage {
        id: hintMessage;
        width: parent.width / 2
        anchors.left: imageView.left
        anchors.bottom: imageView.bottom
    }
}
