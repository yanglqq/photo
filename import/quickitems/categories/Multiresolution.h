#ifndef MULTIRESOLUTION_H
#define MULTIRESOLUTION_H

#include <QQuickItem>
#include <memory>
#include "../../image.h"
#include "../ImageProccessor.h"

class FilterCoefficients {
public:
    std::vector<float> g0;
    std::vector<float> g1;
    std::vector<float> h0;
    std::vector<float> h1;
};

class Multiresolution : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_ENUMS(FilterCoefficientsType)
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    enum FilterCoefficientsType {
        Daubechies_Coefficients = 0,
        Symlet_Coefficients = 1,
        Cohen_Daubechies_Feauveau_Coefficients = 2
    };
    explicit Multiresolution(QQuickItem *parent = 0);

    Q_INVOKABLE QVariantList subbandCoding(const QString &uri, int filterCoefficients, int borderType = cv::BORDER_DEFAULT);
    Q_INVOKABLE QString subbandDecoding(const QString &a, const QString &dv, const QString &dh, const QString &dd, int filterCoefficients, int borderType = cv::BORDER_DEFAULT);

    Q_INVOKABLE QVariantList fastWaveletTransform(const QString &uri, int filterCoefficients, int borderType = cv::BORDER_DEFAULT);
    Q_INVOKABLE QString inverseFastWaveletTransform(const QString &a, const QString &dv, const QString &dh, const QString &dd, int filterCoefficients, int borderType = cv::BORDER_DEFAULT);

protected:
    std::vector<std::shared_ptr<FilterCoefficients>> coefficients;
};

#endif // MULTIRESOLUTION_H
