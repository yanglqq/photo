#ifndef CONTRASTSTRETCHING_H
#define CONTRASTSTRETCHING_H

#include <QQuickItem>
#include <QPointF>
#include "../../../image.h"
#include "../../controls/ImageCurve.h"

class ContrastStretching : public ImageCurve
{
    Q_OBJECT
public:
    explicit ContrastStretching(QQuickItem *parent = 0)
        : ImageCurve(parent) {}

    void calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax) {
        static double minLevels[] = {
            std::numeric_limits<uchar>::min(),
            std::numeric_limits<schar>::min(),
            std::numeric_limits<ushort>::min(),
            std::numeric_limits<short>::min(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<double>::min(),
		};
        static double maxLevels[] = {
            std::numeric_limits<uchar>::max(),
            std::numeric_limits<schar>::max(),
            std::numeric_limits<ushort>::max(),
            std::numeric_limits<short>::max(),
            std::numeric_limits<int>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<double>::max(),
        };

        curve.clear();
        if (!image)
			return;
        size_t channels = image->channels();
        if (channels == 0)
            return;

        if (m_newURI) {
            m_newURI = false;
            double minValue;
            double maxValue;
            double minLevel = static_cast<double>(minLevels[image->depth()]);
            double maxLevel = static_cast<double>(maxLevels[image->depth()]);
            if (channels > 1) {
				cv::Mat hsi;
                cvtColor(*image, hsi, cv::COLOR_BGR2HLS);
				cv::Mat lumination(hsi.rows, hsi.cols, CV_MAKE_TYPE(hsi.depth(), 1));
				int from_to[] = { 1,0 };
				cv::mixChannels(&hsi, 1, &lumination, 1, from_to, 1);
                minMaxLoc(lumination, &minValue, &maxValue);
            } else
                minMaxLoc(*image, &minValue, &maxValue);
            anchors.clear();
            anchors.push_back(QPointF(0.0f, 0.0f));
            anchors.push_back(QPointF((minValue - minLevel) / (maxLevel - minLevel), 0.0f));
            anchors.push_back(QPointF((maxValue - minLevel) / (maxLevel - minLevel), 1.0f));
            anchors.push_back(QPointF(1.0f, 1.0f));
        }
        calcLinearCurve(curve, anchors, xMax, yMax);
    }

	void process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
		polka::adjustLuminace(src, dst,
			[&](const cv::Mat &src, cv::Mat &dst) {
			CurveFunc::process(src, dst, curve);
		});
	}
};

#endif // CONTRASTSTRETCHING_H
