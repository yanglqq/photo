#include "ImageManager.h"
#include "ImageProccessor.h"

ImageProccessor::ImageProccessor()
    : m_resultNew(true) {}

QString ImageProccessor::putImage(const cv::Mat &image, const QString &uri) {
    if (!m_resultNew) {
        ImageManager::replace(image, uri);
        return uri;
    }
    return ImageManager::put(image, uri);
}

