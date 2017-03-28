#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <qquickimageprovider.h>
#include <QImage>

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // IMAGEPROVIDER_H
