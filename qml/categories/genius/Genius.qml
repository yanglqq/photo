import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    Genius {
        id: genius
        resultNew: manager.resultNew
    }

    Options_DetectObjects {
        width: parent.width
    }
}
