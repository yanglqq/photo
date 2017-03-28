#ifndef COLOR_H
#define COLOR_H

#include <QQuickItem>
#include "../../ImageProccessor.h"

class Color : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit Color(QQuickItem *parent = 0);

    Q_INVOKABLE QVariantList splitRGB(const QString &uri);
    Q_INVOKABLE QVariantList splitHSI(const QString &uri);
    Q_INVOKABLE QVariantList splitCMYK(const QString &uri);
    Q_INVOKABLE QString colorSlicing(const QString &uri, const QColor &color, float width, bool sphere = false);

};

#endif // COLOR_H
