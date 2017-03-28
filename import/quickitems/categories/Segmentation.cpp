#include "Segmentation.h"
#include "../ImageManager.h"

Segmentation::Segmentation(QQuickItem *parent) :
    QQuickItem(parent), ImageProccessor() {}

QString Segmentation::basicGlobalThresholding(const QString &uri, double maxValue, double threshold, int type, bool absolute, bool evaluate) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat src = absolute ? abs(luminance) : luminance;
        if (evaluate) polka::basicGlobalThreshold(src, threshold);
        cv::threshold(src, dst, threshold, maxValue, type);
    });
    return putImage(dst, uri);
}

QString Segmentation::optimumGlobalThresholding1(const QString &uri, double maxValue, int type, bool useHistogramImage, const QString histogramImageURI, bool partition, int m, int n) {
    const cv::Mat *image1 = ImageManager::get(uri);
    if (!image1) return QString();
    const cv::Mat *histogramImage = image1;
    if (useHistogramImage) {
        const cv::Mat *image2 = ImageManager::get(histogramImageURI);
        if (!image2) return QString();
        histogramImage = image2;
    }
    cv::Mat dst;
    polka::extractLuminace(*image1, [&](const cv::Mat &luminance) {
        if (partition) {
            double threshold;
            std::vector<std::pair<cv::Mat, cv::Mat>> subImages;
            polka::partitionImage(luminance, dst, m, n, subImages);
            for (size_t i = 0, count = subImages.size(); i < count; i++) {
                std::pair<cv::Mat, cv::Mat> item = subImages[i];
                polka::optimumGlobalThreshold1(item.first, threshold);
                cv::threshold(item.first, item.second, threshold, maxValue, type);
            }
        } else {
            double threshold;
            polka::optimumGlobalThreshold1(*histogramImage, threshold);
            cv::threshold(luminance, dst, threshold, maxValue, type);
        }
    });
    return putImage(dst, uri);
}

QString Segmentation::optimumGlobalThresholding2(const QString &uri, double value1, double value2, double value3, bool useHistogramImage, const QString histogramImageURI, bool partition, int m, int n) {
    const cv::Mat *image1 = ImageManager::get(uri);
    if (!image1) return QString();
    const cv::Mat *histogramImage = image1;
    if (useHistogramImage) {
        const cv::Mat *image2 = ImageManager::get(histogramImageURI);
        if (!image2) return QString();
        histogramImage = image2;
    }
    cv::Mat dst;
    polka::extractLuminace(*image1, [&](const cv::Mat &luminance) {
        if (partition) {
            double t1;
            double t2;
            std::vector<std::pair<cv::Mat, cv::Mat>> subImages;
            polka::partitionImage(luminance, dst, m, n, subImages);
            for (size_t i = 0, count = subImages.size(); i < count; i++) {
                std::pair<cv::Mat, cv::Mat> item = subImages[i];
                polka::optimumGlobalThreshold2(item.first, t1, t2);
                polka::threshold2(item.first, item.second, t1, t2, value1, value2, value3);
            }
        } else {
            double t1;
            double t2;
            polka::optimumGlobalThreshold2(*histogramImage, t1, t2);
            polka::threshold2(luminance, dst, t1, t2, value1, value2, value3);
        }
    });
    return putImage(dst, uri);
}

/**
struct LocalThresholding : public cv::BaseFilter {
    int m;
    int n;
    int area;
    double maxValue;
    double weightOfLocalVariance;
    double weightOfMean;
    double globalMean;
    bool usingGlobalMean;
    std::vector<float*> pLocal;

    LocalThresholding(double _maxValue, double _weightOfLocalVariance, double _weightOfMean, int _m, int _n, bool _usingGlobalMean, double _globalMean) {
        maxValue = _maxValue;
        weightOfLocalVariance = _weightOfLocalVariance;
        weightOfMean = _weightOfMean;
        m = _m;
        n = _n;
        usingGlobalMean = _usingGlobalMean;
        globalMean = _globalMean;
        anchor = cv::Point();
        ksize = cv::Size(m, n);
        area = m * n;
        pLocal.resize(area);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;
        int center = area / 2;
        const float** pl = (const float**)&pLocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            uchar* d = (uchar *)dst;
            for (int i = 0, k = 0; i < n; i++ ) {
                for (int j = 0; j < m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                float sum = 0.0;
                for (int k = 0; k < area; k++)
                    sum += pl[k][i];
                float value = pl[center][i];
                float mean = sum / static_cast<float>(area);
                float variance = std::pow(value - mean, 2);
                d[i] = value > weightOfLocalVariance * variance && value > weightOfMean * (usingGlobalMean ? globalMean : mean) ? maxValue : 0;
            }
        }
    }
};

QString Segmentation::localThresholding(const QString &uri, double maxValue, double weightOfLocalVariance, double weightOfMean, int m, int n, bool usingGlobalMean) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat src;
        luminance.convertTo(src, CV_32F);
        double globalMean = 0.0;
        if (usingGlobalMean) {
            float *p = (float *)src.data;
            for (int i = 0, k = 0; i < src.rows; i++) {
                for (int j = 0; j < src.cols; j++, k++) {
                    globalMean += p[k];
                }
            }
            globalMean /= src.rows * src.cols;
        }
        cv::Ptr<cv::FilterEngine> engine = cv::Ptr<cv::FilterEngine>(
                    new cv::FilterEngine(
                        cv::Ptr<cv::BaseFilter>(new LocalThresholding(maxValue, weightOfLocalVariance, weightOfMean, m, n, usingGlobalMean, globalMean)),
                        cv::Ptr<cv::BaseRowFilter>(), cv::Ptr<cv::BaseColumnFilter>(),
                        src.type(), CV_8UC1, src.type(),
                        cv::BORDER_CONSTANT, cv::BORDER_CONSTANT, cv::Scalar(0)));
        dst.create(src.size(), CV_8UC1);
        engine->apply(src, dst);
    });
    return putImage(dst, uri);
}
**/

QString Segmentation::movingAverage(const QString &uri, double maxValue, double weightOfMean, int n) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();    
    cv::Mat dst;
    polka::movingAverage(*image, dst, maxValue, weightOfMean, n);
    return putImage(dst, uri);
}

#define sign(a) ((a) < 0 ? -1 : ((a) > 0 ? 1 : 0))

QString Segmentation::marrHildreth(const QString &uri, double sigma, const QVariantList &laplacian, double threshold) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();

    int n = std::ceil(6.0 * sigma);
    if (n % 2 == 0) n++;
    cv::Mat gussianKernel(n, n, CV_32FC1);
    int offset = n / 2;
    float *pGussianKernel = (float *)gussianKernel.data;
    for (int i = -offset, k = 0; i <= offset; i++) {
        for (int j = -offset; j <= offset; j++, k++) {
            pGussianKernel[k] = std::exp(-0.5 * static_cast<float>(i * i + j * j) / static_cast<float>(sigma * sigma));
        }
    }
    n = std::sqrt(laplacian.size());
    cv::Mat laplacianKernel(n, n, CV_32FC1);
    float *pLaplacian = (float *)laplacianKernel.data;
    for (int i = 0, k = 0; i < n; i++) {
        for (int j = 0; j < n; j++, k++) {
            pLaplacian[k] = laplacian[k].toFloat();
        }
    }
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat smooth;
        cv::Mat deviative;
        cv::filter2D(luminance, smooth, CV_32F, gussianKernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
        cv::filter2D(smooth, deviative, CV_32F, laplacianKernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
        uchar *pdst = dst.data;
        float *pDeviative = (float *)deviative.data;
        for (int i = 0, k = 0; i < dst.rows; i++) {
            for (int j = 0; j < dst.cols; j++, k++) {
                int condition = 0;
                // left/right
                if (j > 0 && j < dst.cols - 1) {
                    condition++;
                    float left  = pDeviative[k - 1];
                    float right = pDeviative[k + 1];
                    if (sign(left) != sign(right) && std::abs(std::abs(left) - std::abs(right)) > threshold) {
                        pdst[k] = 255;
                        continue;
                    }
                }
                // up/down
                if (i > 0 && i < dst.rows - 1) {
                    condition++;
                    float up   = pDeviative[k - dst.cols];
                    float down = pDeviative[k + dst.cols];
                    if (sign(up) != sign(down) && std::abs(std::abs(up) - std::abs(down)) > threshold) {
                        pdst[k] = 255;
                        continue;
                    }
                }

                if (condition == 2) {
                    // +45 diagonal
                    float v1 = pDeviative[k - dst.cols + 1];
                    float v2 = pDeviative[k + dst.cols - 1];
                    if (sign(v1) != sign(v2) && std::abs(std::abs(v1) - std::abs(v2)) > threshold) {
                        pdst[k] = 255;
                        continue;
                    }
                    // -45 diagonal
                    v1 = pDeviative[k - dst.cols - 1];
                    v2 = pDeviative[k + dst.cols + 1];
                    if (sign(v1) != sign(v2) && std::abs(std::abs(v1) - std::abs(v2)) > threshold) {
                        pdst[k] = 255;
                        continue;
                    }
                }
            }
        }
    });

    return putImage(dst, uri);
}

QString Segmentation::canny(const QString &uri, double lowerThreshold, double higherThreshold) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Canny(luminance, dst, lowerThreshold, higherThreshold);
    });
    return putImage(dst, uri);
}

QString Segmentation::houghLines(const QString &uri, double rho, double theta, int threshold, double minLineLength, double maxLineGap, double minAngle, double maxAngle) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        std::vector<cv::Vec4i> lines;
        HoughLinesP(luminance, lines, rho, theta, threshold, minLineLength, maxLineGap);
        for(size_t i = 0; i < lines.size(); i++) {
            cv::Vec4i l = lines[i];
            cv::line(dst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255), 1, cv::LINE_AA);
        }
    });
    return putImage(dst, uri);
}

QString Segmentation::houghCircles(const QString &uri, double dp, double minDist, double threshold1, double threshold2, int minRadiusf, int maxRadius) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(luminance, circles, cv::HOUGH_GRADIENT, dp, minDist, threshold1, threshold2, minRadiusf, maxRadius);
        for(size_t i = 0; i < circles.size(); i++) {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // draw the circle center
            cv::circle(dst, center, 3, cv::Scalar(255), -1, 8, 0);
            // draw the circle outline
            cv::circle(dst, center, radius, cv::Scalar(255), 1, 8, 0);
        }
    });
    return putImage(dst, uri);
}
