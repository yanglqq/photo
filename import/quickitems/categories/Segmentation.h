#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <QQuickItem>
#include "../../image.h"
#include "../ImageProccessor.h"

class Segmentation : public QQuickItem, ImageProccessor
{
    Q_OBJECT
    Q_ENUMS(ThresholdingType)
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    enum ThresholdingType {
        Thresholding_Binary         = cv::THRESH_BINARY,
        Thresholding_Binary_Inverse = cv::THRESH_BINARY_INV,
        Thresholding_Truncate       = cv::THRESH_TRUNC,
        Thresholding_Zero           = cv::THRESH_TOZERO,
        Thresholding_Zero_Inverse   = cv::THRESH_TOZERO_INV
    };

    explicit Segmentation(QQuickItem *parent = 0);

    Q_INVOKABLE QString basicGlobalThresholding(const QString &uri, double maxValue, double threshold, int type, bool absolute, bool evaluate);
    Q_INVOKABLE QString optimumGlobalThresholding1(const QString &uri, double maxValue, int type, bool useHistogramImage, const QString histogramImageURI, bool partition, int m, int n);
    Q_INVOKABLE QString optimumGlobalThresholding2(const QString &uri, double value1, double value2, double value3, bool useHistogramImage, const QString histogramImageURI, bool partition, int m, int n);
    //Q_INVOKABLE QString localThresholding(const QString &uri, double maxValue, double weightOfLocalVariance, double weightOfMean, int m, int n, bool usingGlobalMean);
    Q_INVOKABLE QString movingAverage(const QString &uri, double maxValue, double weightOfMean, int n);
    Q_INVOKABLE QString marrHildreth(const QString &uri, double sigma, const QVariantList &laplacian, double threshold);
    Q_INVOKABLE QString canny(const QString &uri, double lowerThreshold, double higherThreshold);
    Q_INVOKABLE QString houghLines(const QString &uri, double rho, double theta, int threshold, double minLineLength, double maxLineGap, double minAngle, double maxAngle);
    Q_INVOKABLE QString houghCircles(const QString &uri, double dp, double minDist, double threshold1, double threshold2, int minRadiusf, int maxRadius);
};

#endif // SEGMENTATION_H
