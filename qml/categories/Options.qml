import QtQuick 2.2
import "../components"
import "../themes"

Item {
    id: container

    property alias buttonModel: buttonRepeater.model
    property alias optionModel: optionRepeater.model
    property alias title: labelTitle.text
    property bool expanded: false
    property bool canHelp: false
    property bool canApply: false
    property int contentWidth: options.width
    property string helpText: ""

    signal apply()
    signal expand()
    signal fold()

    Rectangle {
        id: titleBar
        height: Style.category.contentTitleHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        Image {
            id: indicator
            width: 16
            height: 16
            anchors.left: parent.left
            anchors.leftMargin: Style.margin.normal
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    expanded = !expanded;
                    if (expanded)
                        expand();
                    else
                        fold();
                }
            }
        }
        Text {
            id: labelTitle
            anchors.left: indicator.right
            anchors.right: buttons.left
            anchors.leftMargin: Style.margin.normal
            anchors.rightMargin: Style.margin.normal
            anchors.verticalCenter: parent.verticalCenter;
            color: Style.normal.text.color
            font.bold: true
            font.pixelSize: Style.text.size.normal
            renderType: Text.NativeRendering
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    expanded = !expanded;
                    if (expanded)
                        expand();
                    else
                        fold();
                }
            }
        }
        Row {
            id: buttons
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: Style.margin.normal
            visible: expanded
            Repeater { id: buttonRepeater }
            IconButton {
                id: helpButton
                anchors.verticalCenter: parent.verticalCenter
                source: "../../image/dark/icons/info-pane.png"
                visible: canHelp
                onClicked: manager.showHelp(helpText)
            }
            IconButton {
                id: applyButton
                enabled: manager.enabled
                anchors.verticalCenter: parent.verticalCenter
                source: "../../image/dark/icons/apply.png"
                visible: canApply
                onClicked: apply()
            }
        }
        states: [
            State {
                name: "expanded"
                when: expanded
                PropertyChanges {
                    target: indicator
                    source: "../../image/dark/icons/indicator-expanded-normal.png"
                }
                PropertyChanges {
                    target: titleBar
                    color: Style.normal.category.contentTitleColor
                }
                PropertyChanges {
                    target: container
                    height: titleBar.height + space.height + options.height
                }
            },
            State {
                name: "folded"
                when: !expanded
                PropertyChanges {
                    target: indicator
                    source: "../../image/dark/icons/indicator-folded-normal.png"
                }
                PropertyChanges {
                    target: titleBar
                    color: "transparent"
                }
                PropertyChanges {
                    target: container
                    height: titleBar.height
                }
            }
        ]
    }
    Item {
        id: space
        width: parent.width
        height: Style.margin.normal
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
    Flow {
        id: options
        width: parent.width - Style.category.optionLeftMargin
        anchors.top: space.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.category.optionLeftMargin
        visible: expanded
        spacing: Style.margin.smaller
        Repeater { id: optionRepeater }
    }
}
