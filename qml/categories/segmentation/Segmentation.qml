import QtQuick 2.2
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Category {
    Segmentation {
        id: segmentation
        resultNew: manager.resultNew
    }

    Options_BasicGlobalThresholding {
        width: parent.width
    }

    Options_OptimumGlobalThresholding {
        width: parent.width
    }

    Options_LocalThresholding {
        width: parent.width
    }

    Options_MovingAverage {
        width: parent.width
    }

    Options_RegionSplittingMerging {
        width: parent.width
    }

    Options_MarrHildrethEdgeDetector {
        width: parent.width
    }

    Options_CannyEdgeDetector {
        width: parent.width
    }

    Options_HoughLines {
        width: parent.width
    }

    Options_HoughCircles {
        width: parent.width
    }
}
