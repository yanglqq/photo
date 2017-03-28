pragma Singleton
import QtQuick 2.2
QtObject {
    property QtObject border: QtObject {
        property int width: 1;
        property int radius: 3;
    }

    property QtObject histogram: QtObject {
        property int height: 128;
    }

    property QtObject category: QtObject {
        property int width              : 289;
        property int barWidth           : 28;
        property int contentWidth       : 256;
        property int contentTitleHeight : 20;
        property int optionItemHeight   : 24;
        property int optionLeftMargin   : 16;
    }

    property QtObject text: QtObject {
        property QtObject size: QtObject {
            property int normal: 11;
            property int larger: 12;
        }
    }

    property QtObject margin: QtObject {
        property int normal: 5;
        property int smaller: 3
    }

    property QtObject tooltip: QtObject {
        property int offsetY: 15
    }

    property QtObject normal: QtObject {
        property color baseColor: "#333333";

        property QtObject border: QtObject {
            property color color: "#000000"
        }

        property QtObject tabButton: QtObject {
            property color normalGradientColor0: "#737373"
            property color normalGradientColor1: "#333333"
            property color highlightedGradientColor0: "#737373"
            property color highlightedGradientColor1: "#1E1E1E"
        }

        property QtObject text: QtObject {
            property color color: "#F0F0F0";
            property color strongColor: "#FFFFFF"
        }

        property QtObject histogram: QtObject {
            property color backgroudColor: "#1E1E1E";
        }

        property QtObject slider: QtObject {
            property color color: "#646464"
            property color fontColor: "#d4d4d4"
        }

        property QtObject combox: QtObject {
            property color color: "#333333"
            property color dropDownBorderColor: "#c9c9c9"
        }

        property QtObject category: QtObject {
            property color contentTitleColor: "#1E1E1E";
        }

        property QtObject contextMenu: QtObject {
            property color backgroundColor: "#1E1E1E";
            property color menuItemColor:   "#1E1E1E";
        }
    }

    property QtObject hover: QtObject {
        property color baseColor: "#1E1E1E";

        property QtObject text: QtObject {
            property color color: "#C8C8C8";
            property color strongColor: "#969696"
        }

        property QtObject border: QtObject {
            property color color: "#64FF64"
        }

        property QtObject combox: QtObject {
            property color color: "#c9c9c9"
            property color fontColor: "gray"
        }
    }

    property QtObject disabled: QtObject {
        property QtObject slider: QtObject {
            property color color: "#545454"
            property color fontColor: "#959595"
        }
        property QtObject input: QtObject {
            property color color: "#545454"
            property color fontColor: "#959595"
        }
        property QtObject checker: QtObject {
            property color fontColor: "#959595"
        }
        property QtObject combox: QtObject {
            property color fontColor: "#959595"
        }
    }
}
