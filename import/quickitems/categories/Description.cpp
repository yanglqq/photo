#include "Segmentation.h"
#include "../../image.h"
#include "../ImageManager.h"
#include "Description.h"

Description::Description(QQuickItem *parent) :
    QQuickItem(parent), ImageProccessor() {}
namespace impl {
    void drawBoundary(const std::vector<cv::Point2i> &boundary, cv::Mat &dst, const cv::Size &size, const int pixelValue, const bool drawLine) {
        dst.create(size, CV_8UC1);
        dst.setTo(cv::Scalar(0));
        size_t n = boundary.size();
        if (drawLine) {
            for (size_t i = 1; i < n; i++) {
                line(dst, boundary[i - 1], boundary[i], cv::Scalar(pixelValue));
            }
            if (n - 1 > 0)
                line(dst, boundary[n - 1], boundary[0], cv::Scalar(pixelValue));
        } else {
            uchar *pdst = dst.data;
            size_t step = dst.step1();
            for (size_t i = 0; i < n; i++) {
                pdst[boundary[i].y * step + boundary[i].x] = pixelValue;
            }
        }
    }
}

QJsonObject Description::boundary(const QString &uri, int pixelValue, BoudaryApproximation approximation, ChainCodeType codeType, int cellSize, double threshold, bool pcTransform, bool drawLine) {
    QJsonObject result;
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return result;

    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        std::vector<cv::Point2i> boundary;
        polka::boundaryFollowing(luminance, boundary);

        switch (approximation) {
        case Sub_Sampling:
        {
            std::vector<cv::Point2i> sampled;
            polka::boundarySubSampling(boundary, sampled, image->cols, image->rows, cellSize);
            if (pcTransform) polka::principalComponentsTransform(sampled, sampled);
            impl::drawBoundary(sampled, dst, image->size(), pixelValue, drawLine);
            break;
        }
        case Minimum_Perimeter_Polygon:
        {
            std::vector<cv::Point2i> polygon;
            polka::minimumPerimeterPolygon(boundary, polygon, cellSize);
            if (pcTransform) polka::principalComponentsTransform(polygon, polygon);
            impl::drawBoundary(polygon, dst, image->size(), pixelValue, drawLine);
            break;
        }
        case Merging:
        {
            std::vector<cv::Point2i> merged;
            polka::boundaryMerging(boundary, merged, threshold);
            if (pcTransform) polka::principalComponentsTransform(merged, merged);
            impl::drawBoundary(merged, dst, image->size(), pixelValue, drawLine);
            break;
        }
		case Splitting:
		{
			std::vector<cv::Point2i> splitted;
			polka::boundarySplitting(boundary, splitted, threshold);
            if (pcTransform) polka::principalComponentsTransform(splitted, splitted);
            impl::drawBoundary(splitted, dst, image->size(), pixelValue, drawLine);
			break;
		}
        default:
            if (pcTransform) polka::principalComponentsTransform(boundary, boundary);
            impl::drawBoundary(boundary, dst, image->size(), pixelValue, drawLine);
        }
    });
    result["uri"] = putImage(dst, uri);
    return result;
}

QString Description::convexhull(const QString &uri, int pixelValue, int algorithm, bool drawLine) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        std::vector<cv::Point2i> boundary;
        polka::boundaryFollowing(luminance, boundary);
        std::vector<cv::Point2i> polygon;
        polka::convexhull(boundary, polygon, algorithm);
        impl::drawBoundary(polygon, dst, image->size(), pixelValue, drawLine);
    });
    return putImage(dst, uri);
}

QString Description::skeleton(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        polka::skeleton(luminance, dst);
    });
    return putImage(dst, uri);
}

QString Description::fourierDescriptors(const QString &uri, int pixelValue, double descriptors) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        std::vector<cv::Point2i> boundary;
        polka::boundaryFollowing(luminance, boundary);

        int n = static_cast<int>(boundary.size());
        int m = cv::getOptimalDFTSize(n);
		int l = static_cast<int>(static_cast<double>(m) * descriptors);
        cv::Mat boundaryMat(m, 1, CV_32FC2, cv::Scalar(0));

        int i, j;
		n = n * 2;
		m = m * 2;
		l = l * 2;
		float *p = (float *)boundaryMat.data;
        for (i = 0, j = n / 2 - 1; i < n; i += 2, j--) {
            p[i]     = boundary[j].x;
            p[i + 1] = boundary[j].y;
        }
        cv::Mat frequency;
        cv::dft(boundaryMat, frequency);
        p = (float *)frequency.data;
        for (i = l; i < m; i += 2) {
            p[i] = 0.0;
            p[i + 1] = 0.0;
        }
        int x, y;
        cv::Mat inverse;
        cv::dft(frequency, inverse, cv::DFT_INVERSE | cv::DFT_SCALE);
        p = (float *)inverse.data;
        for (i = 0; i < n; i += 2) {
            x = static_cast<int>(p[i]);
            y = static_cast<int>(p[i + 1]);
            dst.at<uchar>(y, x) = pixelValue;
        }
    });
    return putImage(dst, uri);
}
