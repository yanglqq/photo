#ifndef FILTERS_H
#define FILTERS_H

#include <functional>
#include <QQuickItem>
#include "../../image.h"
#include "../ImageProccessor.h"

class FrequencyFilterAlgorithm {
public:
    FrequencyFilterAlgorithm(polka::FrequencyFilter *filter) { m_filter = filter; }

    virtual void spatialResult(const cv::Mat &src, cv::Mat &dst) const {
        polka::fourier(src, dst, true, true,
            [&](cv::Mat &frequency, cv::Mat &dst) {
            cv::Mat filtered;
            (*m_filter)(src, frequency, filtered);
            cv::dft(filtered, dst, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        });
    }

    virtual void frequencyResult(const cv::Mat &src, cv::Mat &dst) const {
        polka::fourier(src, dst, true, false,
            [&](cv::Mat &frequency, cv::Mat &dst) {
            cv::Mat temp;
            (*m_filter)(src, frequency, temp);
            cv::Mat planes[2];
            split(temp, planes);
            // compute the magnitude
            // => sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
            cv::magnitude(planes[0], planes[1], temp);
            // switch to logarithmic scale
            // => log(1 + dst)
            temp += cv::Scalar::all(1);
            cv::log(temp, temp);
            cv::normalize(temp, dst, 0.0, 255.0, cv::NORM_MINMAX);
        });
    }

    virtual void spatialImage(cv::Mat &frequency, cv::Mat &dst) const {
        cv::Mat filtered;
        (*m_filter)(frequency, frequency, filtered);
        cv::dft(filtered, dst, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

        // crop the spectrum, if it has an odd number of rows or columns
        dst = dst(cv::Rect(0, 0, dst.cols & -2, dst.rows & -2));
        // rearrange the quadrants of Fourier image so that the origin is at the image center
        int cx = dst.cols / 2;
        int cy = dst.rows / 2;

        cv::Mat q0(dst, cv::Rect(0, 0, cx, cy)); // Top-Left - Create a ROI per quadrant
        cv::Mat q1(dst, cv::Rect(cx, 0, cx, cy)); // Top-Right
        cv::Mat q2(dst, cv::Rect(0, cy, cx, cy)); // Bottom-Left
        cv::Mat q3(dst, cv::Rect(cx, cy, cx, cy)); // Bottom-Right
        cv::Mat tmp;
        // swap quadrants (Top-Left with Bottom-Right)
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        // swap quadrant (Top-Right with Bottom-Left)
        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);
    }

    virtual void frequencyImage(cv::Mat &frequency, cv::Mat &dst) const {
        (*m_filter)(frequency, frequency, dst);
    }

protected:
    polka::FrequencyFilter *m_filter;
};

class Filters : public QQuickItem, public ImageProccessor
{
    Q_OBJECT
    Q_ENUMS(BorderType FrequencyFilterResult)
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew)
public:
    enum BorderType {
        BORDER_REPLICATE   = cv::BORDER_REPLICATE,
        BORDER_CONSTANT    = cv::BORDER_CONSTANT,
        BORDER_REFLECT     = cv::BORDER_REFLECT,
        BORDER_WRAP        = cv::BORDER_WRAP,
        BORDER_REFLECT_101 = cv::BORDER_REFLECT_101,
        BORDER_TRANSPARENT = cv::BORDER_TRANSPARENT,
        BORDER_ISOLATED    = cv::BORDER_ISOLATED,
        BORDER_DEFAULT     = cv::BORDER_DEFAULT
    };

    enum FrequencyFilterResult {
        Filter_Spatial_Result,
        Filter_Frequency_Result,
        Filter_Spatial_Image,
        Filter_Frequency_Image
    };

    explicit Filters(QQuickItem *parent = 0);

    Q_INVOKABLE QString maxFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString medianFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString midpointFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString minFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString alphaTrimmedMeanFilter(const QString &uri, int m, int n, int d, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString arithmeticMeanFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);

    Q_INVOKABLE QString custom(const QString &uri, const QVariantList &kernelVariant, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString laplacian(const QString &uri, int apertureSize, int borderType = BORDER_DEFAULT, bool hsi = false);

    /**
    Q_INVOKABLE QString majorityFilter(const QString &uri, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString geometricMeanFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString harmonicMeanFilter(const QString &uri, int m, int n, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString contraharmonicMeanFilter(const QString &uri, int m, int n, int Q, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString adaptiveLocalNoiseFilter(const QString &uri, int m, int n, int variance, int borderType = BORDER_DEFAULT, bool hsi = false);
    Q_INVOKABLE QString adaptiveMeanFilter(const QString &uri, int m, int n, int mMax, int nMax, int borderType = BORDER_DEFAULT, bool hsi = false);
    **/

    Q_INVOKABLE QString idealLowpassFilter(const QString &uri, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString butterworthLowpassFilter(const QString &uri, double cutoff, int order, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString gaussianLowpassFilter(const QString &uri, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString idealHighpassFilter(const QString &uri, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString butterworthHighpassFilter(const QString &uri, double cutoff, int order, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString gaussianHighpassFilter(const QString &uri, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString highFrequencyEmphasis_Ideal(const QString &uri, double cutoff, double offset, double contribution, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString highFrequencyEmphasis_Butterworth(const QString &uri, double cutoff, int order, double offset, double contribution, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString highFrequencyEmphasis_Gaussian(const QString &uri, double cutoff, double offset, double contribution, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString homomorphic(const QString &uri, double cutoff, double low, double high, double sharpness, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString bandRejectFilter_Ideal(const QString &uri, double cutoff, double width, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString bandRejectFilter_Butterworth(const QString &uri, double cutoff, int order, double width, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString bandRejectFilter_Gaussian(const QString &uri, double cutoff, double width, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString bandPassFilter_Ideal(const QString &uri, double cutoff, double width, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString bandPassFilter_Butterworth(const QString &uri, double cutoff, int order, double width, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString bandPassFilter_Gaussian(const QString &uri, double cutoff, double width, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString notchRejectFilter_Block(const QString &uri, const QString &maskURI, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchRejectFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchRejectFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchRejectFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString notchPassFilter_Block(const QString &uri, const QString &maskURI, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchPassFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchPassFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString notchPassFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, int resultType = Filter_Spatial_Result, bool hsi = false);

    Q_INVOKABLE QString optimumNotchFilter_Block(const QString &uri, const QString &maskURI, double m, double n, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString optimumNotchFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, double m, double n, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString optimumNotchFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, double m, double n, int resultType = Filter_Spatial_Result, bool hsi = false);
    Q_INVOKABLE QString optimumNotchFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, double m, double n, int resultType = Filter_Spatial_Result, bool hsi = false);
private:
    QString process(const QString &uri, std::function<void(const cv::Mat &src, cv::Mat &dst)> callback, bool split, bool hsi = false);
    //QString spatial2DFilter(const QString &uri, cv::Ptr<cv::BaseFilter> &filter, int borderType = BORDER_DEFAULT, bool hsi = false);
    QString frequencyFilter(const QString &uri, const FrequencyFilterAlgorithm &algorithm, const int resultType = Filter_Spatial_Result, const bool hsi = false);
};

#endif // FILTERS_H
