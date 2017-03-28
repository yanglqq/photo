#ifndef CURVE_H
#define CURVE_H

#include <QQuickPaintedItem>
#include <QPainter>
#include "Slipper.h"
#include "CurveFunc.h"

class Curve : public QQuickPaintedItem, public CurveFunc
{
    Q_OBJECT
    Q_ENUMS(SlipperShape MaskType CurveType)
    Q_PROPERTY(SlipperShape slipperShape READ slipperShape WRITE setSlipperShape)
    Q_PROPERTY(MaskType mask READ maskType WRITE setMaskType)
    Q_PROPERTY(CurveType curve READ curveType WRITE setCurveType)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)
    Q_PROPERTY(QColor topLeftColor READ topLeftColor WRITE setTopLeftColor)
    Q_PROPERTY(QColor bottomRightColor READ bottomRightColor WRITE setBottomRightColor)
    Q_PROPERTY(QSizeF slipperSize READ anchorSize WRITE setAnchorSize)
    Q_PROPERTY(QQmlListProperty<Slipper> slippers READ slippers)

    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit Curve(QQuickItem *parent = 0);

    enum SlipperShape {
        CircleShape,
        RectangleShape
    };

    enum MaskType {
        LuminanceMask,
        ColorMask,
        ColorBlocks,
        None
    };

    enum CurveType {
        Linear,
        Spline,
        Cosine
    };

    SlipperShape slipperShape() const { return m_slipperShape; }
    MaskType     maskType() const { return m_maskType; }
    QSizeF       anchorSize() const { return m_slipperSize; }
    CurveType    curveType() const { return m_curveType; }
    QColor       backgroundColor() const { return m_backgroundBrush.color(); }
    QColor       gridColor() const { return m_gridPen.color(); }
    QColor       topLeftColor() const { return m_topLeftBrush.color(); }
    QColor       bottomRightColor() const { return m_bottomRightBrush.color(); }

    QQmlListProperty<Slipper> slippers();

    void setSlipperShape(const SlipperShape slipperShape) { m_slipperShape = slipperShape; }
    void setMaskType(MaskType mask) { m_maskType = mask; }
    void setCurveType(CurveType curveType) { m_curveType = curveType; }
    void setAnchorSize(const QSizeF &size) { m_slipperSize = size; }
    void setBackgroundColor(const QColor &color) { m_backgroundBrush.setColor(color); }
    void setGridColor(const QColor &color) { m_gridPen.setColor(color); }
    void setTopLeftColor(const QColor &color) { m_topLeftBrush.setColor(color); }
    void setBottomRightColor(const QColor &color) { m_bottomRightBrush.setColor(color); }

    virtual void calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax);
	Q_INVOKABLE virtual QString processImage(const QString &uri) { return CurveFunc::processImage(uri); }
Q_SIGNALS:
    void contextMenu(qreal x, qreal y);
protected:
    SlipperShape  m_slipperShape;
    MaskType      m_maskType;
    CurveType     m_curveType;
    QSizeF        m_slipperSize;
    QPen          m_gridPen;
    QPen          m_slipperPen;
    QPen          m_curvePen;
    QBrush        m_backgroundBrush;
    QBrush        m_slipperBrush;
    QBrush        m_topLeftBrush;
    QBrush        m_bottomRightBrush;
    int           m_currentIndex;
    bool          m_anchorsInited;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paint(QPainter *painter);
	virtual const cv::Mat *getImage() { return nullptr;	}

    static void appendSlipper(QQmlListProperty<Slipper> *list, Slipper *slipper);
};

#endif // CURVE_H
