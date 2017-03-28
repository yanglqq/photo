import QtQuick 2.2
import Polka 1.0
import ".."

Category {
    property QtObject colorTool: Color { resultNew: manager.resultNew }

    Options_ColorSlicing {
        width: parent.width
    }
    Options_IntensitySlicing {
        width: parent.width
    }
    Options_SplitComponents {
        width: parent.width
    }
    Options_TonalCorrection_Black {
        width: parent.width
    }
    Options_TonalCorrection_Cyan {
        width: parent.width
    }
    Options_TonalCorrection_Magenta {
        width: parent.width
    }
    Options_TonalCorrection_Yellow {
        width: parent.width
    }
    Options_SaturationCorrection {
        width: parent.width
    }
}
