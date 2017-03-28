#ifndef INTENSITY_COLOURING_H
#define INTENSITY_COLOURING_H

#include <QQuickPaintedItem>
#include <QPainter>
#include "../../ImageProccessor.h"

class IntensityColouring : public QQuickPaintedItem, public ImageProccessor
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit IntensityColouring(QQuickItem *parent = 0);

    Q_INVOKABLE QString colour(const QString &uri);
	Q_INVOKABLE void setColor(int index, const QColor &color);
Q_SIGNALS:
	void colorDialog(int index, const QColor &color);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paint(QPainter *painter);
private:
    QImage  m_colorMap;
    cv::Mat m_table;
    int     m_currentIndex;
    QSizeF  m_slipperSize;
    QPen    m_slipperPen;
    QBrush  m_slipperBrush;

    std::vector<qreal>  m_slippers;
    std::vector<QColor> m_colors;
};

#endif // INTENSITY_COLOURING_H
