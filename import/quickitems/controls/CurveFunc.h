#ifndef CURVEFUNC_H
#define CURVEFUNC_H

#include <vector>
#include <QPointF>
#include "../ImageProccessor.h"

class CurveFunc: public ImageProccessor
{
public:
    bool m_adjustable;

    CurveFunc();

    static void calcCosineCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax);
    static void calcLinearCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax);
    static void calcSplineCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax);

    virtual void calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax) = 0;
    virtual QString processImage(const QString &uri);
    virtual void process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve);
protected:
	std::vector<double>   m_curve;
	std::vector<QPointF>  m_anchors;

    inline void normalizeCurve(std::vector<double> &curve, double yMax) {
        size_t xMax = curve.size() - 1;
        double max = curve[xMax];
        if (max != 0) {
            double ratio = yMax / max;
            for (int i = 0; i <= xMax; i++) {
                curve[i] *= ratio;
            }
        }
    }
};

#endif // CURVEFUNC_H
