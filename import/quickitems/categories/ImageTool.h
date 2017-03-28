#ifndef IMAGETOOL_H
#define IMAGETOOL_H

#include <QQuickItem>
#include <QJsonObject>
#include <QJsonArray>
#include "../ImageProccessor.h"

class ImageTool : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit ImageTool(QQuickItem *parent = 0);

    Q_INVOKABLE bool same(const QString &uri_1, const QString &uri_2);

    Q_INVOKABLE QString add128(const QString &uri);
    Q_INVOKABLE QString to8u(const QString &uri);
    Q_INVOKABLE QString normalize(const QString &uri);
    Q_INVOKABLE QString histogramEqualization(const QString &uri);

    Q_INVOKABLE QString AND(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString XOR(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString OR(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString NOT(const QString &uri);
    Q_INVOKABLE QString abs(const QString &uri);
    Q_INVOKABLE QString add(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString subtract(const QString &uri_1, const QString &uri_2);    
    Q_INVOKABLE QString max(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString min(const QString &uri_1, const QString &uri_2);
    Q_INVOKABLE QString multiply(const QString &uri_1, const QString &uri_2);

	Q_INVOKABLE QVariantList crop(const QString &uri);    
    Q_INVOKABLE int countImageMasks(const QString &uri);

    Q_INVOKABLE int width(const QString &uri);
    Q_INVOKABLE int height(const QString &uri);
    Q_INVOKABLE int maxPixelValue(const QString &uri);
    Q_INVOKABLE int channels(const QString &uri);
    Q_INVOKABLE QJsonArray statistics(const QString &uri);

	Q_INVOKABLE QString rgb2gray(const QString &uri);
    Q_INVOKABLE QString gray2rgb(const QString &uri_r, const QString &uri_g, const QString &uri_b);

    Q_INVOKABLE QString fourierSpectrum(const QString &uri);
    Q_INVOKABLE QString fourierPhaseAngle(const QString &uri);
    Q_INVOKABLE QString reconstructUsingSpectrum(const QString &uri);
    Q_INVOKABLE QString reconstructUsingPhaseAngle(const QString &uri);
};

#endif // IMAGETOOL_H
