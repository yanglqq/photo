import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    property QtObject enhancement: Enhancement { resultNew: manager.resultNew }

    Options_LogTransformation {
        width: parent.width
    }
    Options_PowerLawTransformation {
        width: parent.width
    }
    Options_CustomCurve {
        width: parent.width
    }
    Options_ContrastStretching {
        width: parent.width
    }
    Options_HistogramMatching {
        width: parent.width
    }
    Options_ImageAveraging {
        width: parent.width
    }
}
