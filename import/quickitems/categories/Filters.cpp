#include <numeric>
#include <functional>
#include "../ImageManager.h"
#include "Filters.h"

Filters::Filters(QQuickItem *parent) :
    QQuickItem(parent), ImageProccessor() {}

QString Filters::process(const QString &uri, std::function<void(const cv::Mat &src, cv::Mat &dst)> callback, bool split, bool hsi) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat src;
        cv::Mat dst;
        image->convertTo(src, CV_32F);
        int cn = src.channels();
        if (hsi && cn > 2)
            polka::adjustLuminace(src, dst, callback);
        else if (split && cn > 2) {
            cv::Mat in[] = {
                cv::Mat(src.size(), CV_MAKE_TYPE(src.depth(), 1)),
                cv::Mat(src.size(), CV_MAKE_TYPE(src.depth(), 1)),
                cv::Mat(src.size(), CV_MAKE_TYPE(src.depth(), 1))
            };
			cv::Mat out[3];
            int from_to[] = { 0,0, 1,1, 2,2 };
            cv::mixChannels(&src, 1, &in[0], 3, from_to, 3);
            for (int i = 0; i < 3; i++) {
                callback(in[i], out[i]);
            }
            dst.create(src.size(), src.type());
            cv::mixChannels(&out[0], 3, &dst, 1, from_to, 3);
        }
        else
            callback(src, dst);
        return putImage(dst, uri);
    }
    return QString();
}

QString Filters::maxFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m, n));
        cv::dilate(src, dst, kernel, cv::Point(-1, -1), 1, borderType);
    }, false, hsi);
}

QString Filters::medianFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        int aperture = (m + n) / 2;
        if (aperture % 2 != 1)
            aperture++;
        cv::medianBlur(src, dst, aperture);
    }, false, hsi);
}

QString Filters::midpointFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::Mat max;
        cv::Mat min;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m, n));
        cv::dilate(src, max, kernel, cv::Point(-1, -1), 1, borderType);
        cv::erode(src, min, kernel, cv::Point(-1, -1), 1, borderType);
        dst = 0.5 * (max + min);
    }, false, hsi);
}

QString Filters::minFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m, n));
        cv::erode(src, dst, kernel, cv::Point(-1, -1), 1, borderType);
    }, false, hsi);
}

QString Filters::alphaTrimmedMeanFilter(const QString &uri, int m, int n, int d, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m, n));
        cv::filter2D(src, dst, src.depth(), kernel, cv::Point(-1, -1), 0, borderType);
        float alpha = 1.0f / static_cast<float>(m * n - d);
        dst *= alpha;
    }, false, hsi);
}

QString Filters::custom(const QString &uri, const QVariantList &kernelVariant, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        int size = floor(sqrt(kernelVariant.size()));
        if (size > 0) {
            cv::Mat kernel(size, size, src.depth());
            float *ptr = (float *)kernel.data;
            for (int i = 0, k = 0; i < size; i++) {
                for (int j = 0; j < size; j++, k++) {
                    ptr[k] = kernelVariant[k].toFloat();
                }
            }
            cv::filter2D(src, dst, src.depth(), kernel, cv::Point(-1, -1), 0, borderType);
        }
    }, false, hsi);
}

QString Filters::laplacian(const QString &uri, int apertureSize, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::Laplacian(src, dst, src.depth(), apertureSize, 1, 0, borderType);
    }, false, hsi);
}

QString Filters::arithmeticMeanFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        cv::boxFilter(src, dst, src.depth(), cv::Size(m, n), cv::Point(-1, -1), true, borderType);
    }, false, hsi);
}

/**
QString Filters::spatial2DFilter(const QString &uri, cv::Ptr<cv::BaseFilter> &filter, int borderType, bool hsi) {
    return process(uri, [&](const cv::Mat &src, cv::Mat &dst){
        int type = src.type();
        cv::Ptr<cv::FilterEngine> engine = cv::Ptr<cv::FilterEngine>(
                    new cv::FilterEngine(filter, cv::Ptr<cv::BaseRowFilter>(), cv::Ptr<cv::BaseColumnFilter>(),
                                         type, type, type, borderType & ~BORDER_ISOLATED));
        dst.create(src.size(), type);
        engine->apply(src, dst, cv::Rect(0, 0, -1, -1), cv::Point(0, 0), (borderType & BORDER_ISOLATED) != 0);
    }, false, hsi);
}

QString Filters::majorityFilter(const QString &uri, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::MajorityFilter()), borderType, hsi);
}

QString Filters::geometricMeanFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::GeometricMeanFilter(m, n)), borderType, hsi);
}

QString Filters::harmonicMeanFilter(const QString &uri, int m, int n, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::HarmonicMeanFilter(m, n)), borderType, hsi);
}

QString Filters::contraharmonicMeanFilter(const QString &uri, int m, int n, int Q, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::ContraharmonicMeanFilter(m, n, Q)), borderType, hsi);
}

QString Filters::adaptiveLocalNoiseFilter(const QString &uri, int m, int n, int variance, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::AdaptiveLocalNoiseFilter(m, n, variance)), borderType, hsi);
}

QString Filters::adaptiveMeanFilter(const QString &uri, int m, int n, int mMax, int nMax, int borderType, bool hsi) {
    return spatial2DFilter(uri, cv::Ptr<cv::BaseFilter>(new polka::AdaptiveMeanFilter(m, n, mMax, nMax)), borderType, hsi);
}
**/

QString Filters::frequencyFilter(const QString &uri, const FrequencyFilterAlgorithm &algorithm, const int resultType, const bool hsi) {
    switch (resultType) {
    case Filter_Spatial_Result:
        return process(uri, [&](const cv::Mat &src, cv::Mat &dst) {
            algorithm.spatialResult(src, dst);
        }, true, hsi);
    case Filter_Frequency_Result:
        return process(uri, [&](const cv::Mat &src, cv::Mat &dst) {
            algorithm.frequencyResult(src, dst);
        }, true, true);
    case Filter_Spatial_Image:
        {
            cv::Mat dst;
            cv::Mat src = cv::Mat::ones(512, 512, CV_32FC1);
            algorithm.spatialImage(src, dst);
            return putImage(dst, uri);
        }
    case Filter_Frequency_Image:
        {
            cv::Mat dst;
            cv::Mat src = cv::Mat(512, 512, CV_32FC1, cv::Scalar(255));
            algorithm.frequencyImage(src, dst);
            return putImage(dst, uri);
        }
    }
    return QString();
}

QString Filters::idealLowpassFilter(const QString &uri, double cutoff, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff](double d) { return d > cutoff ? 0.0 : 1.0; });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::butterworthLowpassFilter(const QString &uri, double cutoff, int order, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &order](double d) { return 1.0 / (1.0 + std::pow(d / cutoff, 2.0 * order)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::gaussianLowpassFilter(const QString &uri, double cutoff, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff](double d) { return std::exp(-d * d / (2.0 * cutoff * cutoff)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::idealHighpassFilter(const QString &uri, double cutoff, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff](double d) { return d > cutoff ? 1.0 : 0.0; });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::butterworthHighpassFilter(const QString &uri, double cutoff, int order, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &order](double d) { return 1.0 / (1.0 + std::pow(cutoff / d, 2.0 * order)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::gaussianHighpassFilter(const QString &uri, double cutoff, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff](double d) { return 1.0 - std::exp(-d * d / (2.0 * cutoff * cutoff)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::highFrequencyEmphasis_Ideal(const QString &uri, double cutoff, double offset, double contribution, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &offset, &contribution](double d) { return offset + contribution * (d > cutoff ? 1.0 : 0.0); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::highFrequencyEmphasis_Butterworth(const QString &uri, double cutoff, int order, double offset, double contribution, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &order, &offset, &contribution](double d) { return offset + contribution / (1.0 + std::pow(cutoff / d, 2.0 * order)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::highFrequencyEmphasis_Gaussian(const QString &uri, double cutoff, double offset, double contribution, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &offset, &contribution](double d) { return offset + contribution * (1.0 - std::exp(-d * d / (2.0 * cutoff * cutoff))); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::homomorphic(const QString &uri, double cutoff, double low, double high, double sharpness, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &low, &high, &sharpness](double d) { return (high - low) * (1.0 - std::exp(-sharpness * d * d / (cutoff * cutoff))) + low; });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandRejectFilter_Ideal(const QString &uri, double cutoff, double width, int resultType, bool hsi) {
    float halfW = width / 2.0;
    polka::SimpleFrequencyFilter filter([&cutoff, &halfW](double d) { return cutoff - halfW <= d && d <= cutoff + halfW ? 0.0 : 1.0; });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandRejectFilter_Butterworth(const QString &uri, double cutoff, int order, double width, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &order, &width](double d) { return 1.0 / (1.0 + std::pow(d * width / (d * d - cutoff * cutoff), 2.0 * order)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandRejectFilter_Gaussian(const QString &uri, double cutoff, double width, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &width](double d) { return 1.0 - std::exp(-std::pow((d * d - cutoff * cutoff) / (d * width), 2.0)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandPassFilter_Ideal(const QString &uri, double cutoff, double width, int resultType, bool hsi) {
    float halfW = width / 2.0;
    polka::SimpleFrequencyFilter filter([&cutoff, &halfW](double d) { return cutoff - halfW <= d && d <= cutoff + halfW ? 1.0 : 0.0; });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandPassFilter_Butterworth(const QString &uri, double cutoff, int order, double width, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &order, &width](double d) { return 1.0 - 1.0 / (1.0 + std::pow(d * width / (d * d - cutoff * cutoff), 2.0 * order)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::bandPassFilter_Gaussian(const QString &uri, double cutoff, double width, int resultType, bool hsi) {
    polka::SimpleFrequencyFilter filter([&cutoff, &width](double d) { return std::exp(-std::pow((d * d - cutoff * cutoff) / (d * width), 2.0)); });
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchRejectFilter_Block(const QString &uri, const QString &maskURI, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::BlockNotchRejectFilter filter(masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchRejectFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(1.0, 0.0, [&cutoff](double d1, double d2){ return (d1 > cutoff ? 1.0 : 0.0) * (d2 > cutoff ? 1.0 : 0.0); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchRejectFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
	if (!masks)
		return QString();
    polka::NotchFilter filter(1.0, 0.0, [&cutoff, &order](double d1, double d2){ return (1.0 / (1.0 + std::pow(cutoff / d1, 2.0 * order))) * (1.0 / (1.0 + std::pow(cutoff / d2, 2.0 * order))); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchRejectFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
	if (!masks)
		return QString();
    polka::NotchFilter filter(1.0, 0.0, [&cutoff](double d1, double d2){ return (1.0 - std::exp(-d1 * d1 / (2.0 * cutoff * cutoff))) * (1.0 - std::exp(-d2 * d2 / (2.0 * cutoff * cutoff))); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchPassFilter_Block(const QString &uri, const QString &maskURI, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::BlockNotchPassFilter filter(masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchPassFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff](double d1, double d2){ return (d1 > cutoff ? 1.0 : 0.0) * (d2 > cutoff ? 1.0 : 0.0); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchPassFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff, &order](double d1, double d2){ return (1.0 / (1.0 + std::pow(cutoff / d1, 2.0 * order))) * (1.0 / (1.0 + std::pow(cutoff / d2, 2.0 * order))); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

QString Filters::notchPassFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff](double d1, double d2){ return (1.0 - std::exp(-d1 * d1 / (2.0 * cutoff * cutoff))) * (1.0 - std::exp(-d2 * d2 / (2.0 * cutoff * cutoff))); }, masks);
    return frequencyFilter(uri, FrequencyFilterAlgorithm(&filter), resultType, hsi);
}

class OptimumNotchAlgorithm : public FrequencyFilterAlgorithm {
public:
    OptimumNotchAlgorithm(double m, double n, polka::FrequencyFilter *filter)
        : m_m(m), m_n(n), FrequencyFilterAlgorithm(filter) {}

    void spatialResult(const cv::Mat &src, cv::Mat &dst) {
        cv::Mat noise;
        polka::fourier(src, noise, true, true,
            [&](cv::Mat &frequency, cv::Mat &dst) {
            cv::Mat filtered;
            (*m_filter)(src, frequency, filtered);
            cv::dft(frequency, dst, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        });
        int M        = 2 * m_m + 1;
        int N        = 2 * m_n + 1;
        int xResidue = src.cols % M;
        int yResidue = src.rows % N;
        int xpad     = xResidue == 0 ? 0 : M - xResidue;
        int ypad     = yResidue == 0 ? 0 : N - yResidue;
        float area   = M * N;
        cv::Mat srcPadded;
        cv::Mat noisePadded;
        if (xpad != 0 || ypad != 0) {
            cv::copyMakeBorder(src,   srcPadded,   0, ypad, 0, xpad, cv::BORDER_CONSTANT, cv::Scalar::all(0));
            cv::copyMakeBorder(noise, noisePadded, 0, ypad, 0, xpad, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        } else {
            srcPadded   = src;
            noisePadded = noise;
        }
        std::function<cv::Mat(const cv::Mat &src)> mean = [&M, &N, &area](const cv::Mat &src){
            cv::Mat dst(src.rows / N, src.cols / M, CV_32FC1);
            float *psrc = (float*)src.data;
            float *pdst = (float*)dst.data;
            float sum;
            int i, I, j, J, kd, ks, x, y;
            for (i = 0; i < dst.rows; i++) {
                kd = i * dst.cols;
                I  = i * N;
                for (j = 0; j < dst.cols; j++, kd++) {
                    sum = 0.0;
                    J  = j * M;
                    for (y = I; y < I + N; y++) {
                        ks = y * src.cols + J;
                        for (x = 0; x < M; x++, ks++) {
                            sum += psrc[ks];
                        }
                    }
                    pdst[kd] = sum / area;
                }
            }
            return dst;
        };

        int i, I, j, J, k, K, x, y;
        cv::Mat weight = (mean(srcPadded.mul(noisePadded)) - mean(srcPadded).mul(mean(noisePadded)) / (mean(noisePadded.mul(noisePadded)) - mean(noisePadded).mul(mean(noisePadded))));
        cv::Mat approximated(srcPadded.size(), srcPadded.type());
        float *pw  = (float*)weight.data;
        float *pws = (float*)approximated.data;
        for (i = 0; i < weight.rows; i++) {
            k = i * weight.cols;
            I = i * N;
            for (j = 0; j < weight.cols; j++, k++) {
                J  = j * M;
                for (y = I; y < I + N; y++) {
                    K = y * approximated.cols + J;
                    for (x = 0; x < M; x++, K++) {
                       pws[K] = pw[k];
                    }
                }
            }
        }

        dst = srcPadded - approximated.mul(noisePadded);

        if (xpad != 0 || ypad != 0)
            dst = cv::Mat(dst, cv::Rect(0, 0, src.cols, src.rows));
    }
protected:
    double m_m;
    double m_n;
};

QString Filters::optimumNotchFilter_Block(const QString &uri, const QString &maskURI, double m, double n, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::BlockNotchPassFilter filter(masks);
    return frequencyFilter(uri, OptimumNotchAlgorithm(m, n, &filter), resultType, hsi);
}

QString Filters::optimumNotchFilter_Ideal(const QString &uri, const QString &maskURI, double cutoff, double m, double n, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff](double d1, double d2){ return (d1 > cutoff ? 1.0 : 0.0) * (d2 > cutoff ? 1.0 : 0.0); }, masks);
    return frequencyFilter(uri, OptimumNotchAlgorithm(m, n, &filter), resultType, hsi);
}

QString Filters::optimumNotchFilter_Butterworth(const QString &uri, const QString &maskURI, double cutoff, int order, double m, double n, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff, &order](double d1, double d2){ return (1.0 / (1.0 + std::pow(cutoff / d1, 2.0 * order))) * (1.0 / (1.0 + std::pow(cutoff / d2, 2.0 * order))); }, masks);
    return frequencyFilter(uri, OptimumNotchAlgorithm(m, n, &filter), resultType, hsi);
}

QString Filters::optimumNotchFilter_Gaussian(const QString &uri, const QString &maskURI, double cutoff, double m, double n, int resultType, bool hsi) {
    const std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(maskURI);
    if (!masks)
        return QString();
    polka::NotchFilter filter(-1.0, 1.0, [&cutoff](double d1, double d2){ return (1.0 - std::exp(-d1 * d1 / (2.0 * cutoff * cutoff))) * (1.0 - std::exp(-d2 * d2 / (2.0 * cutoff * cutoff))); }, masks);
    return frequencyFilter(uri, OptimumNotchAlgorithm(m, n, &filter), resultType, hsi);
}
