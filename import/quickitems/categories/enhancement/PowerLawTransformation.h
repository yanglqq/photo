#ifndef POWERLAWTRANSFORMATION_H
#define POWERLAWTRANSFORMATION_H

#include <QQuickItem>
#include <QPointF>
#include "../../controls/Curve.h"

class PowerLawTransformation : public Curve
{
    Q_OBJECT
    Q_PROPERTY(qreal gamma READ gamma WRITE setGamma)
public:
    explicit PowerLawTransformation(QQuickItem *parent = 0)
		: Curve(parent), m_gamma(1) {
		m_adjustable = false;
	}

    qreal gamma() { return m_gamma; }
    void setGamma(const qreal gamma) { m_gamma = gamma; }

    void calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax) {
        curve.clear();
        size_t max = static_cast<size_t>(xMax);
        curve.resize(max + 1);
        for (size_t i = 0; i <= max; i++) {
			curve[i] = pow(static_cast<double>(i), m_gamma);
        }
        normalizeCurve(curve, yMax);
    }
private:
    qreal m_gamma;
};

#endif // POWERLAWTRANSFORMATION_H
