#include "../../image.h"
#include "../ImageManager.h"
#include "ImageTool.h"

ImageTool::ImageTool(QQuickItem *parent)
    : QQuickItem(parent), ImageProccessor() {}

bool ImageTool::same(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        return image_1->type() == image_2->type() &&
            image_1->rows == image_2->rows &&
            image_1->cols == image_2->cols;
    }
    return false;
}

QString ImageTool::add128(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        return putImage(*image + 128, uri);
    }
    return QString();
}

QString ImageTool::to8u(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        image->convertTo(dst, CV_8U);
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::normalize(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        cv::normalize(*image, dst, 0.0, 255.0, cv::NORM_MINMAX);
        dst.convertTo(dst, CV_8U);
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::histogramEqualization(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        polka::adjustLuminace(*image, dst,
            [](const cv::Mat &src, cv::Mat &dst) {
            equalizeHist(src, dst);
        });
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::AND(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::bitwise_and(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QString ImageTool::XOR(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::bitwise_xor(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QString ImageTool::OR(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::bitwise_or(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QString ImageTool::NOT(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        cv::bitwise_not(*image, dst);
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::abs(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst = cv::abs(*image);
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::add(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2)
        return putImage(*image_1 + *image_2, uri_2);
    return QString();
}

QString ImageTool::subtract(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2)
        return putImage(*image_1 - *image_2, uri_2);
    return QString();
}

QString ImageTool::max(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::max(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QString ImageTool::min(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::min(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QString ImageTool::multiply(const QString &uri_1, const QString &uri_2) {
    const cv::Mat *image_1 = ImageManager::get(uri_1);
    const cv::Mat *image_2 = ImageManager::get(uri_2);
    if (image_1 && image_2) {
        cv::Mat dst;
        cv::multiply(*image_1, *image_2, dst);
        return putImage(dst, uri_2);
    }
    return QString();
}

QVariantList ImageTool::crop(const QString &uri) {
	QVariantList uris;
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return uris;
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(uri);
    bool backup = m_resultNew;
    m_resultNew = true;
    double x, y, w, h;
    double width  = static_cast<double>(image->cols);
    double height = static_cast<double>(image->rows);
    for (auto it = masks->begin(); it != masks->end(); ++it) {
        cv::Mat dst;
        (*it)->toRect(width, height, x, y, w, h);
        cv::Mat(*image, cv::Rect(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h))).copyTo(dst);
        uris.push_back(putImage(dst, uri));
    }
    m_resultNew = backup;
	return uris;
}

int ImageTool::countImageMasks(const QString &uri) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(uri);
    if (!masks) return 0;
    return (int)masks->size();
}

int ImageTool::width(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) return image->cols;
    return 0;
}

int ImageTool::height(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) return image->rows;
    return 0;
}

int ImageTool::maxPixelValue(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image){
        double max;
        cv::minMaxLoc(*image, 0, &max);
        return max;
    }
    return 0;
}

int ImageTool::channels(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image){
        return image->channels();
    }
    return 0;
}

QJsonArray ImageTool::statistics(const QString &uri) {
    QJsonArray infoArray;
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return infoArray;
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(uri);
    double x, y, w, h;
    double width  = static_cast<double>(image->cols);
    double height = static_cast<double>(image->rows);
    int channels = image->channels();
    for (auto it = masks->begin(); it != masks->end(); ++it) {
        std::vector<double> means;
        std::vector<double> stdDevs;
        (*it)->toRect(width, height, x, y, w, h);
        polka::meanStdDev(cv::Mat(*image, cv::Rect(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h))), means, stdDevs);
        QJsonArray meansArray;
        QJsonArray stdDevArray;
        for (int i = 0; i < channels; i++) {
            meansArray.append(means[i]);
            stdDevArray.append(stdDevs[i]);
        }
        QJsonObject info;
        info["width"] = w;
        info["height"] = h;
        info["x"] = x;
        info["y"] = y;
        info["mean"] = meansArray;
        info["stand deviation"] = stdDevArray;
        infoArray.append(info);
    }
    if (masks->size() == 0) {
        std::vector<double> means;
        std::vector<double> stdDevs;
        polka::meanStdDev(*image, means, stdDevs);
        QJsonArray meansArray;
        QJsonArray stdDevArray;
        for (int i = 0; i < channels; i++) {
            meansArray.append(means[i]);
            stdDevArray.append(stdDevs[i]);
        }
        QJsonObject info;
        info["width"] = image->cols;
        info["height"] = image->rows;
        info["mean"] = meansArray;
        info["stand deviation"] = stdDevArray;
        infoArray.append(info);
    }
    return infoArray;
}

QString ImageTool::rgb2gray(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
	if (image) {
		if (image->channels() == 1)
			return putImage(*image, uri);
		cv::Mat dst;
        cvtColor(*image, dst, cv::COLOR_BGR2GRAY);
		return putImage(dst, uri);
	}
	return QString();
}

QString ImageTool::gray2rgb(const QString &uri_r, const QString &uri_g, const QString &uri_b) {
    const cv::Mat *image_r = ImageManager::get(uri_r);
    const cv::Mat *image_g = ImageManager::get(uri_g);
    const cv::Mat *image_b = ImageManager::get(uri_b);
    if (image_r && image_g && image_b &&
        image_r->channels() == 1 && image_g->channels() == 1 && image_b->channels() == 1 &&
        image_r->depth() == image_g->depth() && image_g->depth() == image_b->depth() &&
        image_r->rows == image_g->rows && image_g->rows == image_b->rows &&
        image_r->cols == image_g->cols && image_g->cols == image_b->cols) {
        cv::Mat dst(image_r->size(), CV_MAKETYPE(image_r->depth(), 3));
        cv::Mat images[] = {*image_b, *image_g, *image_r};
        int from_to[] = { 0,0, 1,1, 2,2 };
        cv::mixChannels(&images[0], 3, &dst, 1, from_to, 3);
        return putImage(dst, uri_b);
    }
    return QString();
}

QString ImageTool::fourierSpectrum(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        polka::fourier(*image, dst, true, false,
            [](cv::Mat &frequency, cv::Mat &dst){
            cv::Mat temp;
            cv::Mat planes[2];
            split(frequency, planes);
            // compute the magnitude
            // => sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
            cv::magnitude(planes[0], planes[1], temp);
            // switch to logarithmic scale
            // => log(1 + dst)
            temp += cv::Scalar::all(1);
            cv::log(temp, temp);
            cv::normalize(temp, dst, 0.0, 255.0, cv::NORM_MINMAX);
        });
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::fourierPhaseAngle(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        polka::fourier(*image, dst, true, false,
            [](cv::Mat &complex, cv::Mat &dst){
            dst.create(complex.size(), CV_32FC1);
            float *pc  = (float*)complex.data;
            float *pdst  = (float*)dst.data;
            size_t count = complex.rows * complex.cols * 2;
            for (size_t i = 0, k = 0; i < count; i += 2, k++) {
                pdst[k] = std::atan(pc[i + 1] / pc[i]);
            }
        });
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::reconstructUsingSpectrum(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        polka::fourier(*image, dst, false, false,
            [](cv::Mat &complex, cv::Mat &dst){
            float *pc  = (float*)complex.data;
            size_t count = complex.rows * complex.cols * 2;
            for (size_t i = 0; i < count; i += 2) {
                pc[i]     = std::sqrt(pc[i + 1] * pc[i + 1] + pc[i] * pc[i]);
                pc[i + 1] = 0.0;
            }
            cv::dft(complex, dst, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        });
        return putImage(dst, uri);
    }
    return QString();
}

QString ImageTool::reconstructUsingPhaseAngle(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
        polka::fourier(*image, dst, false, false,
            [](cv::Mat &complex, cv::Mat &dst){
            float *pc  = (float*)complex.data;
            size_t count = complex.rows * complex.cols * 2;
            for (size_t i = 0; i < count; i += 2) {
                float angle = std::atan(pc[i + 1] / pc[i]);
                pc[i]   = angle;
                pc[i+1] = 0.0;
            }
            cv::dft(complex, dst, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        });
        return putImage(dst, uri);
    }
    return QString();
}
