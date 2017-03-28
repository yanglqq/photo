#include "../../../image.h"
#include "../../imageManager.h"
#include "Color.h"

Color::Color(QQuickItem *parent)
    : QQuickItem(parent), ImageProccessor() {}

QVariantList Color::splitRGB(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        if (image->channels() > 2) {
            cv::Mat dst[] = {
                cv::Mat(image->size(), CV_8UC1),
                cv::Mat(image->size(), CV_8UC1),
                cv::Mat(image->size(), CV_8UC1)
            };
            int from_to[] = { 0,2, 1,1, 2,0 };
            cv::mixChannels(image, 1, &dst[0], 3, from_to, 3);
            bool backup = m_resultNew;
            m_resultNew = true;
            QVariantList result;
            result.push_back(putImage(dst[0], uri));
            result.push_back(putImage(dst[1], uri));
            result.push_back(putImage(dst[2], uri));
            m_resultNew = backup;
            return result;
        }
    }
    return QVariantList();
}

QVariantList Color::splitHSI(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        if (image->channels() > 2) {
            cv::Mat hls;
            cv::cvtColor(*image, hls, cv::COLOR_BGR2HLS);
            cv::Mat dst[] = {
                cv::Mat(image->size(), CV_8UC1),
                cv::Mat(image->size(), CV_8UC1),
                cv::Mat(image->size(), CV_8UC1)
            };
            int from_to[] = { 0,0, 1,2, 2,1 };
            cv::mixChannels(&hls, 1, &dst[0], 3, from_to, 3);
            normalize(dst[0], dst[0], 0, 255, cv::NORM_MINMAX, -1, cv::Mat());
            normalize(dst[1], dst[1], 0, 255, cv::NORM_MINMAX, -1, cv::Mat());
            bool backup = m_resultNew;
            m_resultNew = true;
            QVariantList result;
            result.push_back(putImage(dst[0], uri));
            result.push_back(putImage(dst[1], uri));
            result.push_back(putImage(dst[2], uri));
            m_resultNew = backup;
            return result;
        }
    }
    return QVariantList();
}

QVariantList Color::splitCMYK(const QString &uri) {
    QVariantList result;
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        if (image->channels() > 2) {
            cv::Mat c, m, y, k;
            polka::bgr2cmyk(*image, c, m, y, k);
            bool backup = m_resultNew;
            m_resultNew = true;            
            result.push_back(putImage(c, uri));
            result.push_back(putImage(m, uri));
            result.push_back(putImage(y, uri));
            result.push_back(putImage(k, uri));
            m_resultNew = backup;
        }
    }
    return result;
}

QString Color::colorSlicing(const QString &uri, const QColor &color, float width, bool sphere) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst(image->size(), image->type());
        uchar *psrc = image->data;
        uchar *pdst = dst.data;
        float blue  = static_cast<float>(color.blue());
        float green = static_cast<float>(color.green());
        float red   = static_cast<float>(color.red());
        if (image->channels() > 2) {
            size_t count = image->rows * image->cols;
            if (sphere) {
                float r2 = width * width;
                for (size_t i = 0; i < count; i++, psrc += 3, pdst += 3) {
                    if (std::powf(static_cast<float>(psrc[0] - green), 2.0) +
                        std::powf(static_cast<float>(psrc[1] - blue), 2.0) +
                        std::powf(static_cast<float>(psrc[2] - red), 2.0) > r2) {
                        pdst[0] = 128;
                        pdst[1] = 128;
                        pdst[2] = 128;
                    } else {
                        pdst[0] = psrc[0];
                        pdst[1] = psrc[1];
                        pdst[2] = psrc[2];
                    }
                }
            } else {
                float hf = width / 2;
                for (size_t i = 0; i < count; i++, psrc += 3, pdst += 3) {
                    if (std::abs(static_cast<float>(psrc[0]) - green) > hf ||
                        std::abs(static_cast<float>(psrc[1]) - blue) > hf ||
                        std::abs(static_cast<float>(psrc[2]) - red) > hf) {
                        pdst[0] = 128;
                        pdst[1] = 128;
                        pdst[2] = 128;
                    } else {
                        pdst[0] = psrc[0];
                        pdst[1] = psrc[1];
                        pdst[2] = psrc[2];
                    }
                }
            }
        }
        return putImage(dst, uri);
    }
    return QString();
}
