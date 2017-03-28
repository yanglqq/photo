#include "ImageProvider.h"
#include "ImageManager.h"

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &srequestedSize){
	QString uri("");
    uri.append(imageProtocol);
	uri.append(id);
    const cv::Mat *image = ImageManager::get(uri);
    return image ? ImageManager::toQImage(*image) : QImage();
}
