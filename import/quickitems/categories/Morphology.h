#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QQuickItem>
#include "../../image.h"
#include "../ImageProccessor.h"

class Morphology : public QQuickItem, ImageProccessor
{
    Q_OBJECT
    Q_ENUMS(ElementsShape)
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    enum ElementsShape {
        RECT    = cv::MORPH_RECT,
        ELLIPSE = cv::MORPH_ELLIPSE,
        CROSS   = cv::MORPH_CROSS
    };
    explicit Morphology(QQuickItem *parent = 0);

    Q_INVOKABLE QVariantList createStructureElements(int m, int n, ElementsShape shape);

    Q_INVOKABLE QString erode(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString dilate(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString open(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString close(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString gradient(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString tophat(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString bottomhat(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations);
    Q_INVOKABLE QString boundary(const QString &uri, const QVariantList &elements, int m, int n, int origin);
    Q_INVOKABLE QString skeleton(const QString &uri, const QVariantList &elements, int m, int n, int origin);
    Q_INVOKABLE QString thin(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins);
    Q_INVOKABLE QString thicken(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins);
    Q_INVOKABLE QString prune(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins);
    Q_INVOKABLE QString convexhull(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins);
    Q_INVOKABLE QString connectedComponents(const QString &uri, int pixelValue, bool largestComponent);

    Q_INVOKABLE QString geodesicErode(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI, int iterations);
    Q_INVOKABLE QString geodesicDilate(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI, int iterations);
    Q_INVOKABLE QString reconstructByErosion(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI);
    Q_INVOKABLE QString reconstructByDilation(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI);
};

#endif // MORPHOLOGY_H
