#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/opencv.hpp>
#include <QtQuick/QQuickPaintedItem>
#include <QPainter>

class Histogram : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
public:
    explicit Histogram(QQuickItem *parent = 0);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    void paint(QPainter *painter);
    Q_INVOKABLE void draw(const QString &uri);
    Q_INVOKABLE void clear();

private:
    cv::Mat    m_histogramMat;
	cv::Scalar m_backgroundColor;
};

#endif // HISTOGRAM_H
