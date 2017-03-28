import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    Description {
        id: description
        resultNew: manager.resultNew
    }

    Options_Boundary {
        width: parent.width
    }

    Options_ConvexHull {
        width: parent.width
    }

    Options_FourierDescriptors {
        width: parent.width
    }

    Row {
        width: parent.width
        height: Style.category.optionItemHeight
        spacing: 10

        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            enabled: manager.enabled
            source: "../../../image/dark/icons/apply.png"
            text: "skeleton"
            onClicked: {
                manager.execute(function() {
                    var uri = description.skeleton(manager.currentImage);
                    manager.addImage(uri, manager.currentImage);
                    return true;
                });
            }
        }
    }

}
