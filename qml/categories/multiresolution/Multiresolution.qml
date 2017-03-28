import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    property int borderType: Filters.BORDER_DEFAULT

    Multiresolution {
        id: multiresolution
        resultNew: manager.resultNew
    }

    Item {
        width: parent.width
        height: Style.category.optionItemHeight
        Combox {
            id: borderTypeCombox
            index: 0
            width: 120
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            model: ListModel {
                ListElement { name: "Border Constant" }
                ListElement { name: "Border Replicate" }
                ListElement { name: "Border Reflect" }
                ListElement { name: "Border Reflect_101" }
                //ListElement { name: "Border Wrap" }
                //ListElement { name: "Border Transparent" }
                //ListElement { name: "Border Isolated" }
            }
            onSelected: {
                switch (borderTypeCombox.index) {
                   case 0:
                       borderType = Filters.BORDER_CONSTANT;
                       //borderTypeHint.text = "iiiiii|abcdefgh|iiiiiii";
                       break;
                   case 1:
                       borderType = Filters.BORDER_REPLICATE;
                       //borderTypeHint.text = "aaaaaa|abcdefgh|hhhhhhh";
                       break;
                   case 2:
                       borderType = Filters.BORDER_REFLECT;
                       //borderTypeHint.text = "fedcba|abcdefgh|hgfedcb";
                       break;
                   case 3:
                       borderType = Filters.BORDER_REFLECT_101;
                       //borderTypeHint.text = "gfedcb|abcdefgh|gfedcba";
                       break;
                   /**
                   case 4:
                       borderType = Filters.BORDER_WRAP;
                       borderTypeHint.text = "cdefgh|abcdefgh|abcdefg";
                       break;
                   case 5:
                       borderType = Filters.BORDER_TRANSPARENT;
                       borderTypeHint.text = "";
                       break;
                   case 6:
                       borderType = Filters.BORDER_ISOLATED;
                       borderTypeHint.text = "";
                       break;
                   **/
                   default:
                       borderType = Filters.BORDER_DEFAULT;
                       borderTypeHint.text = "";
                }
            }
        }
        /**
        Text {
            id: borderTypeHint
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            color: Style.normal.text.color
            renderType: Text.NativeRendering
        }
        **/
    }
    Options_Subband {
        width: parent.width
    }
    Options_FastWaveletTransform {
        width: parent.width
    }
}
