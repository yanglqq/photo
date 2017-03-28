import QtQuick 2.2
import "../themes"

Grid {
    property alias model: repeater.model

    signal cellChanged(int index, int value)

    columns: 3
    spacing: Style.margin.normal
    IntValidator { id: validator }
    Repeater {
        id: repeater
        Input {
            id: input
            width: 42
            caption: ""
            text: modelData
            validator: validator
            onEditingFinished: cellChanged(index, input.text)
        }
    }
}

