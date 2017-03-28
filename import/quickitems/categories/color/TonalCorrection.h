#ifndef TONALCORRECTION_H
#define TONALCORRECTION_H

#include <QQuickItem>
#include "../../../image.h"
#include "../../controls/Curve.h"

class TonalCorrection : public Curve
{
    Q_OBJECT
    Q_ENUMS(SpaceComponent)
    Q_PROPERTY(SpaceComponent component READ component WRITE setComponent)
public:
    enum SpaceComponent {
        Black,
        Cyan,
        Magenta,
        Yellow
    };

    SpaceComponent component() { return m_component; }
    void setComponent(const SpaceComponent component) {
        m_component = component;
        switch (m_component) {
        case Black:
            m_topLeftBrush.setColor(Qt::black);
            m_bottomRightBrush.setColor(Qt::white);
            break;
        case Cyan:
            m_topLeftBrush.setColor(Qt::cyan);
            m_bottomRightBrush.setColor(Qt::red);
            break;
        case Magenta:
            m_topLeftBrush.setColor(Qt::magenta);
            m_bottomRightBrush.setColor(Qt::green);
            break;
        case Yellow:
            m_topLeftBrush.setColor(Qt::yellow);
            m_bottomRightBrush.setColor(Qt::blue);
            break;
        }
    }

    explicit TonalCorrection(QQuickItem *parent = 0)
        : Curve(parent) {
        m_maskType = ColorBlocks;
        setComponent(Black);
    }

    void process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
        if (src.channels() < 3)
            return;
        cv::Mat c, m, y, k;
        polka::bgr2cmyk(src, c, m, y, k);
        switch (m_component) {
        case Black:
            CurveFunc::process(k, k, curve);
            break;
        case Cyan:
            CurveFunc::process(c, c, curve);
            break;
        case Magenta:
            CurveFunc::process(m, m, curve);
            break;
        case Yellow:
            CurveFunc::process(y, y, curve);
        }
        polka::cmyk2bgr(c, m, y, k, dst);
    }
private:
    SpaceComponent m_component;
};

#endif // TONALCORRECTION_H
