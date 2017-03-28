#ifndef ENHANCEMENT_H
#define ENHANCEMENT_H

#include <QQuickItem>
#include "../../ImageProccessor.h"

class Enhancement : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit Enhancement(QQuickItem *parent = 0);

    Q_INVOKABLE QString average(const QString &uri, double mean, double sigma, int count);
};
#endif // ENHANCEMENT_H
