#ifndef SATURATIONCORRECTION_H
#define SATURATIONCORRECTION_H

#include <opencv2/opencv.hpp>
#include <QQuickItem>
#include "../../controls/Curve.h"

class SaturationCorrection : public Curve
{
    Q_OBJECT
public:
    explicit SaturationCorrection(QQuickItem *parent = 0)
        : Curve(parent) {}

    void process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
        if (src.channels() > 1) {
            cv::Mat hsi;
            cv::cvtColor(src, hsi, cv::COLOR_BGR2HLS);
            cv::Mat saturation(hsi.rows, hsi.cols, CV_MAKE_TYPE(hsi.depth(), 1));
            int from_to_1[] = { 2,0 };
            cv::mixChannels(&hsi, 1, &saturation, 1, from_to_1, 1);
            CurveFunc::process(saturation, saturation, curve);
            int from_to_2[] = { 0,2 };
            cv::mixChannels(&saturation, 1, &hsi, 1, from_to_2, 1);
            cv::cvtColor(hsi, dst, cv::COLOR_HLS2BGR);
        }
    }
};

#endif // SATURATIONCORRECTION_H
