import QtQuick 2.2
import Polka 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../themes"

Menu {
    id: contextMenu

    property bool showMaskMenuItem: true
    property var curveControl: null

    Component.onCompleted: {
        switch (curveControl.curve) {
        case Curve.Linear:
            linear.checked = true;
            break;
        case Curve.Spline:
            spline.checked = true;
            break;
        case Curve.Cosine:
            cosine.checked = true;
            break;
        }
        switch (curveControl.mask) {
        case Curve.LuminanceMask:
            luminanceMask.checked = true;
            break;
        case Curve.ColorMask:
            colorMask.checked = true;
            break;
        }
    }

    ExclusiveGroup { id: curveGroup }
    ExclusiveGroup { id: maskGroup }
    MenuItem {
        id: linear
        text: "Linear"
        checkable: true
        exclusiveGroup: curveGroup
        onTriggered: {
            curveControl.curve = Curve.Linear;
            curveControl.update();
        }
    }
    MenuItem {
        id: spline
        text: "Spline"
        checkable: true
        exclusiveGroup: curveGroup
        onTriggered: {
            curveControl.curve = Curve.Spline;
            curveControl.update();
        }
    }
    MenuItem {
        id: cosine
        text: "Cosine"
        checkable: true
        exclusiveGroup: curveGroup
        onTriggered: {
            curveControl.curve = Curve.Cosine;
            curveControl.update();
        }
    }
    MenuSeparator {
        visible: showMaskMenuItem
    }
    MenuItem {
        id: luminanceMask
        text: "Luminance Mask"
        visible: showMaskMenuItem
        checkable: true
        exclusiveGroup: maskGroup
        onTriggered: {
            curveControl.mask = Curve.LuminanceMask;
            curveControl.update();
        }
    }
    MenuItem {
        id: colorMask
        text: "Color Mask"
        visible: showMaskMenuItem
        checkable: true
        exclusiveGroup: maskGroup
        onTriggered: {
            curveControl.mask = Curve.ColorMask;
            curveControl.update();
        }
    }
    style: MenuStyle {
        frame: Rectangle {
            color: Style.normal.contextMenu.backgroundColor
        }
        itemDelegate.label: Label {
            text: styleData.text
            color: Style.normal.text.color
            font.pixelSize: Style.text.size.normal;
        }
        itemDelegate.background: Rectangle {
            color: Style.normal.contextMenu.menuItemColor
        }
    }
}
