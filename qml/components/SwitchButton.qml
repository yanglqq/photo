import QtQuick 2.2

IconButton {
    id: container

    property string onStateImage: ""
    property string offStateImage: ""
    property bool on: true
    signal stateChanged;

    onClicked: {
        on = !on;
        stateChanged(on);
    }
    states: [
        State {
            when: on
            PropertyChanges {
                target: container
                source: onStateImage
            }
        },
        State {
            when: !on
            PropertyChanges {
                target: container
                source: offStateImage
            }
        }
    ]
}
