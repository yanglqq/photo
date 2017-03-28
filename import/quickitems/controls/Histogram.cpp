#include "../ImageManager.h"
#include "Histogram.h"

Histogram::Histogram(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_backgroundColor(cv::Scalar(0, 0, 0)) {}

QColor Histogram::backgroundColor() const {
    return QColor(m_backgroundColor[2], m_backgroundColor[1], m_backgroundColor[0]);
}

void Histogram::setBackgroundColor(const QColor &color) {
    m_backgroundColor[0] = color.blue();
    m_backgroundColor[1] = color.green();
    m_backgroundColor[2] = color.red();
}

void Histogram::paint(QPainter *painter) {
    QRectF rect(0.0, 0.0, width(), height());
    QImage image = ImageManager::toQImage(m_histogramMat);
    painter->drawImage(rect, image, rect);
}

void Histogram::draw(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) {
        clear();
        return;
    }

	std::vector<cv::Mat> planes;

    /// Separate the image in 3 places ( B, G, R and alpha )
    split(*image, planes);
	int chanelsCount =  planes.size();

    int histw   = width();
    int histh   = height();
    int hisSize = 256;
    float binw  = static_cast<float>(histw) / static_cast<float>(hisSize);

    // Draw the histograms for B, G and R
    m_histogramMat.create(histh, histw, CV_8UC3);
    m_histogramMat.setTo(m_backgroundColor);

    cv::Mat hists[4];

    /// Set the ranges ( for B,G,R,alpha) )
    float range[] = { 0, 255 } ;
    const float* histRange = { range };

	for (int i = 0; i < chanelsCount; i++) {
        /// Compute the histograms:
        calcHist(&planes[i], 1, 0, cv::Mat(), hists[i], 1, &hisSize, &histRange, true, false);
        /// Normalize the result to [ 0, m_histogramMat.rows ]
		normalize(hists[i], hists[i], 0, m_histogramMat.rows, cv::NORM_MINMAX, -1, cv::Mat());
    }

    /// Draw for each channel
    static cv::Scalar bgra[] = {
		cv::Scalar(255, 0, 0),
		cv::Scalar(0, 255, 0),
		cv::Scalar(0, 0, 255),
		cv::Scalar(255, 128, 128)
	};
	static cv::Scalar gray[] = { cv::Scalar(133, 133, 133) };
	cv::Scalar *colors = chanelsCount == 1 ? gray : bgra;
    for (int i = 1; i < hisSize; i++) {
		for (int j = 0; j < chanelsCount; j++) {
            line(m_histogramMat,
                 cv::Point2f(binw * static_cast<float>(i - 1), static_cast<float>(histh) - hists[j].at<float>(i - 1)),
                 cv::Point2f(binw * static_cast<float>(i), static_cast<float>(histh) - hists[j].at<float>(i)),
                 colors[j], 2, cv::LINE_AA, 0);
        }
    }

    update();
}

void Histogram::clear() {
    m_histogramMat.create(height(), width(), CV_8UC3);
    m_histogramMat.setTo(m_backgroundColor);
    update();
}
