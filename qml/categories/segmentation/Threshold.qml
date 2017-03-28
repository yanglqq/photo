import QtQuick 2.2
import "../../components"
import "../../themes"
import ".."

Row {
    property alias name: valueInput.caption
    property alias percentage: percentageChecker.checked
    property alias initialValue: valueInput.text

    function checkValue() {
        var valuef = parseFloat(valueInput.text);
        if (percentage) {
            if (valuef < 0 || valuef > 100) {
                return false;
            }
        }
        return true;
    }

    function getValue(width, height) {
        var valuef = parseFloat(valueInput.text);
        if (percentage) {
            valuef /= 100;
            switch (reference.index) {
            case 0:
                valuef *= width;
                break;
            case 1:
                valuef *= height;
                break;
            case 2:
                valuef *= Math.sqrt(width * width + height * height);
                break;
            }
        }
        return valuef;
    }

    height: Style.category.optionItemHeight
    spacing: Style.margin.smaller
    Input {
        id: valueInput
        width: 127
        caption: ""
        text: "10"
        validator: DoubleValidator { decimals: 3 }
        anchors.verticalCenter: parent.verticalCenter
    }
    Checker {
        id: percentageChecker
        anchors.verticalCenter: parent.verticalCenter
        text: "% of"
        checked: true
    }
    Combox {
        id: reference
        width: 60
        index: 0
        enabled: percentageChecker.checked
        anchors.verticalCenter: parent.verticalCenter
        model: ListModel {
            ListElement { name: "width" }
            ListElement { name: "height" }
            ListElement { name: "diagonal" }
        }
    }
}
