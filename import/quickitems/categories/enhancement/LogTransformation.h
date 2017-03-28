#ifndef LOGTRANSFORMATION_H
#define LOGTRANSFORMATION_H

#include <QQuickItem>
#include <QPointF>
#include "../../controls/Curve.h"

class LogTransformation : public Curve
{
    Q_OBJECT
public:
    explicit LogTransformation(QQuickItem *parent = 0)
        : Curve(parent) {
		m_adjustable = false;
	}

    void calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax) {
        curve.clear();
        size_t max = static_cast<size_t>(xMax);
        curve.resize(max + 1);
        for (size_t i = 0; i <= max; i++) {
            curve[i] = log(1.0f + static_cast<double>(i));
        }
        normalizeCurve(curve, yMax);
    }
};

#endif // LOGTRANSFORMATION_H
