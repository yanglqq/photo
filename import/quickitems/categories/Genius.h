#ifndef GENIUS_H
#define GENIUS_H

#include <QQuickItem>
#include "../../image.h"
#include "../ImageProccessor.h"

class Genius : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    explicit Genius(QQuickItem *parent = 0);

    Q_INVOKABLE void detectObjects(const QString &uri);
};

#endif // GENIUS_H
