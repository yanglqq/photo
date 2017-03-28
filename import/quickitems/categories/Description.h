#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QQuickItem>
#include <QJsonObject>
#include "../../image.h"
#include "../ImageProccessor.h"

class Description : public QQuickItem, ImageProccessor
{
    Q_OBJECT
    Q_ENUMS(BoudaryApproximation ChainCodeType ConvexHullAlgorithm)
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    enum BoudaryApproximation {
        Sub_Sampling,
        Minimum_Perimeter_Polygon,
        Merging,
        Splitting
    };
    enum ChainCodeType {
        Four_Connectivity,
        Eight_Connectivity
    };
    enum ConvexHullAlgorithm {
        GRAHAM_SCAN = polka::GRAHAM_SCAN,
        JARVIS_MARCH = polka::JARVIS_MARCH
    };

    explicit Description(QQuickItem *parent = 0);

    Q_INVOKABLE QJsonObject boundary(const QString &uri, int pixelValue, BoudaryApproximation approximation, ChainCodeType codeType, int cellSize, double threshold, bool pcTransform, bool drawLine);
    Q_INVOKABLE QString convexhull(const QString &uri, int pixelValue, int algorithm, bool drawLine);
    Q_INVOKABLE QString skeleton(const QString &uri);
    Q_INVOKABLE QString fourierDescriptors(const QString &uri, int pixelValue, double descriptors);
};

#endif // DESCRIPTION_H
