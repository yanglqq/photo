#include <numeric>
#include <functional>
#include "../ImageManager.h"
#include "Multiresolution.h"

class OrthornormalFilterCoefficients : public FilterCoefficients {
public:
    OrthornormalFilterCoefficients(std::vector<float> aG0) {
        g0 = aG0;
        size_t n = g0.size();
        g1.resize(n);
        h0.resize(n);
        h1.resize(n);
        for (size_t i = 0; i < n; i++) {
            g1[i] = std::pow(-1.0f, static_cast<float>(i)) * g0[n - 1 - i];
        }
        for (size_t i = 0; i < n; i++) {
            h0[i] = g0[n - 1 - i];
            h1[i] = g1[n - 1 - i];
        }
    }
};

class BiorthogonalFilterCoefficients : public FilterCoefficients {
public:
    BiorthogonalFilterCoefficients(std::vector<float> aH0, std::vector<float> aH1) {
        h0 = aH0;
        h1 = aH1;
        size_t n = h0.size();
        g0.resize(n);
        g1.resize(n);
        for (size_t i = 0; i < n; i++) {
            g0[i] = std::pow(-1.0f, static_cast<float>(i + 1)) * h1[i];
            g1[i] = std::pow(-1.0f, static_cast<float>(i)) * h0[i];
        }
    }
};

class DaubechiesCoefficients : public OrthornormalFilterCoefficients {
public:
    DaubechiesCoefficients() : OrthornormalFilterCoefficients(
        {
          0.23037781f, 0.71484657f, 0.63088076f, -0.02798376f,
          -0.18703481f, 0.03084138f, 0.03288301f, -0.01059740f
        }
    ) {}
};

class SymletCoefficients: public OrthornormalFilterCoefficients {
public:
    SymletCoefficients() : OrthornormalFilterCoefficients(
        {
          0.0322f, -0.0126f, -0.0992f, 0.2979f,
          0.8037f, 0.4976f, -0.0296f, -0.0758f
        }
    ) {}
};

class Cohen_Daubechies_FeauveauCoefficients : public BiorthogonalFilterCoefficients {
public:
    Cohen_Daubechies_FeauveauCoefficients() : BiorthogonalFilterCoefficients(
        {
          0.0f, 0.0019f, -0.0019f, -0.017f,
          0.0119f, 0.0497f, -0.0773f, -0.0941f,
          0.4208f, 0.8259f, 0.4208f, -0.0941f,
          -0.0773f, 0.0497f, 0.0119f, -0.017f,
          -0.0019f, 0.0010f
        },
        {
          0.0f, 0.0f, 0.0f, 0.0144f,
          -0.0145f, -0.0787f, 0.0404f, 0.4178f,
          -0.7589f, 0.4178f, 0.0404f, -0.0787f,
          -0.0145f, 0.0144f, 0.0f, 0.0f,
          0.0f, 0.0f
        }
    ) {}
};

class FilterBank {
protected:
    void flip(const std::vector<float> &vec, std::vector<float> &result) {
        size_t n = vec.size();
        result.resize(n);
        for (size_t i = 0; i < n; i++) {
            result[i] = vec[n - i - 1];
        }
    }
    void downsampleByRow(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.rows / 2, src.cols, src.type());
        const float *psrc = (const float*)src.data;
        float *pdst = (float*)dst.data;
        int cn = src.channels();
        for (int i = 0, k1 = 0; i < dst.rows; i++) {
            int k2 = i * 2 * src.cols * cn;
            for (int j = 0; j < dst.cols; j++, k1 += cn, k2 += cn) {
                for (int l = 0; l < cn; l++) {
                    pdst[k1 + l] = psrc[k2 + l];
                }
            }
        }
    }
    void downsampleByColumn(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.rows, src.cols / 2, src.type());
        const float *psrc = (const float*)src.data;
        float *pdst = (float*)dst.data;
        int cn = src.channels();
        for (int i = 0, k1 = 0; i < dst.rows; i++) {
            int k2 = i * src.cols * cn;
            for (int j = 0; j < dst.cols; j++, k1 += cn, k2 += 2 * cn) {
                for (int l = 0; l < cn; l++) {
                    pdst[k1 + l] = psrc[k2 + l];
                }
            }
        }
    }
    void upsampleByRow(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.rows * 2, src.cols, src.type());
        const float *psrc = (const float*)src.data;
        float *pdst = (float*)dst.data;
        int cn = src.channels();
        int offset = src.cols * cn;
        for (int i = 0, k1 = 0, k2 = 0; i < src.rows; i++) {
            for (int j = 0; j < src.cols; j++, k1 += cn, k2 += cn) {
                for (int l = 0; l < cn; l++) {
                    pdst[k1 + l] = psrc[k2 + l];
                    pdst[offset + k1 + l] = psrc[k2 + l];
                }
            }
            k1 += offset;
        }
    }
    void upsampleByColumn(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.rows, src.cols * 2, src.type());
        const float *psrc = (const float*)src.data;
        float *pdst = (float*)dst.data;
        int cn = src.channels();
        for (int i = 0, k1 = 0, k2 = 0; i < src.rows; i++) {
            for (int j = 0; j < src.cols; j++, k1 += 2 * cn, k2 += cn) {
                for (int l = 0; l < cn; l++) {
                    pdst[k1 + l] = psrc[k2 + l];
                    pdst[k1 + cn + l] = psrc[k2 + l];
                }
            }
        }
    }
};

class FourBandAnalysisFilterBank : public FilterBank {
public:
    virtual void run(const cv::Mat &src, cv::Mat &a, cv::Mat &dv, cv::Mat &dh, cv::Mat &dd) = 0;
};

class FourBandSynthesisFilterBank : public FilterBank {
public:
    virtual void run(const cv::Mat &a, const cv::Mat &dv, const cv::Mat &dh, const cv::Mat &dd, cv::Mat &dst) = 0;
};

class SubbandCoding : public FourBandAnalysisFilterBank {
public:
    SubbandCoding(const FilterCoefficients &coefficients, int border)
        : borderType(border) {
        std::vector<float> h0;
        std::vector<float> h1;
        flip(coefficients.h0, h0);
        flip(coefficients.h1, h1);
        cv::Mat(h0.size(), 1, CV_32F, (void *)h0.data()).copyTo(h0r);
        cv::Mat(h1.size(), 1, CV_32F, (void *)h1.data()).copyTo(h1r);
        cv::Mat(1, h0.size(), CV_32F, (void *)h0.data()).copyTo(h0c);
        cv::Mat(1, h1.size(), CV_32F, (void *)h1.data()).copyTo(h1c);
    }

    void run(const cv::Mat &src, cv::Mat &a, cv::Mat &dv, cv::Mat &dh, cv::Mat &dd) {
        cv::Mat temp1, temp2;

        cv::filter2D(src, temp1, -1, h0r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, temp2);
        cv::filter2D(temp2, temp1, -1, h0c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, a);
        cv::filter2D(temp2, temp1, -1, h1c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, dv);

        cv::filter2D(src, temp1, -1, h1r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, temp2);
        cv::filter2D(temp2, temp1, -1, h0c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, dh);
        cv::filter2D(temp2, temp1, -1, h1c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, dd);
    }

protected:
    int borderType;
    cv::Mat h0r;
    cv::Mat h1r;
    cv::Mat h0c;
    cv::Mat h1c;
};

class SubbandDecoding : public FourBandSynthesisFilterBank {
public:
    SubbandDecoding(const FilterCoefficients &coefficients, int border)
        : borderType(border) {
        std::vector<float> g0;
        std::vector<float> g1;
        flip(coefficients.g0, g0);
        flip(coefficients.g1, g1);
        cv::Mat(g0.size(), 1, CV_32F, (void *)g0.data()).copyTo(g0r);
        cv::Mat(g1.size(), 1, CV_32F, (void *)g1.data()).copyTo(g1r);
        cv::Mat(1, g0.size(), CV_32F, (void *)g0.data()).copyTo(g0c);
        cv::Mat(1, g1.size(), CV_32F, (void *)g1.data()).copyTo(g1c);
    }

    void run(const cv::Mat &a, const cv::Mat &dv, const cv::Mat &dh, const cv::Mat &dd, cv::Mat &dst) {
        cv::Mat temp1, temp2, temp3, temp4, temp5;

        upsampleByColumn(a, temp1);
        cv::filter2D(temp1, temp2, -1, g0c, cv::Point(-1, -1), 0, borderType);
        upsampleByColumn(dv, temp1);
        cv::filter2D(temp1, temp3, -1, g1c, cv::Point(-1, -1), 0, borderType);
        temp1 = temp2 + temp3;
        upsampleByRow(temp1, temp2);
        cv::filter2D(temp2, temp4, -1, g0r, cv::Point(-1, -1), 0, borderType);

        upsampleByColumn(dh, temp1);
        cv::filter2D(temp1, temp2, -1, g0c, cv::Point(-1, -1), 0, borderType);
        upsampleByColumn(dd, temp1);
        cv::filter2D(temp1, temp3, -1, g1c, cv::Point(-1, -1), 0, borderType);
        temp1 = temp2 + temp3;
        upsampleByRow(temp1, temp2);
        cv::filter2D(temp2, temp5, -1, g1r, cv::Point(-1, -1), 0, borderType);

        dst = temp4 + temp5;
    }

protected:
    int borderType;
    cv::Mat g0r;
    cv::Mat g1r;
    cv::Mat g0c;
    cv::Mat g1c;
};

class FastWaveletTransform : public FourBandAnalysisFilterBank {
public:
    FastWaveletTransform(const FilterCoefficients &coefficients, int border)
        : borderType(border) {
        std::vector<float> h0;
        std::vector<float> h1;
        flip(coefficients.h0, h0);
        flip(coefficients.h1, h1);
        cv::Mat(h0.size(), 1, CV_32F, (void *)h0.data()).copyTo(h0r);
        cv::Mat(h1.size(), 1, CV_32F, (void *)h1.data()).copyTo(h1r);
        cv::Mat(1, h0.size(), CV_32F, (void *)h0.data()).copyTo(h0c);
        cv::Mat(1, h1.size(), CV_32F, (void *)h1.data()).copyTo(h1c);
    }

    void run(const cv::Mat &src, cv::Mat &a, cv::Mat &dv, cv::Mat &dh, cv::Mat &dd) {
        cv::Mat temp1, temp2;

        cv::filter2D(src, temp1, -1, h1c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, temp2);
        cv::filter2D(temp2, temp1, -1, h1r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, dd);
        cv::filter2D(temp2, temp1, -1, h0r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, dv);

        cv::filter2D(src, temp1, -1, h0c, cv::Point(-1, -1), 0, borderType);
        downsampleByColumn(temp1, temp2);
        cv::filter2D(temp2, temp1, -1, h1r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, dh);
        cv::filter2D(temp2, temp1, -1, h0r, cv::Point(-1, -1), 0, borderType);
        downsampleByRow(temp1, a);
    }

protected:
    int borderType;
    cv::Mat h0r;
    cv::Mat h1r;
    cv::Mat h0c;
    cv::Mat h1c;
};

class InverseWaveletTransform : public FourBandSynthesisFilterBank {
public:
    InverseWaveletTransform(const FilterCoefficients &coefficients, int border)
        : borderType(border) {
        std::vector<float> g0;
        std::vector<float> g1;
        flip(coefficients.g0, g0);
        flip(coefficients.g1, g1);
        cv::Mat(g0.size(), 1, CV_32F, (void *)g0.data()).copyTo(g0r);
        cv::Mat(g1.size(), 1, CV_32F, (void *)g1.data()).copyTo(g1r);
        cv::Mat(1, g0.size(), CV_32F, (void *)g0.data()).copyTo(g0c);
        cv::Mat(1, g1.size(), CV_32F, (void *)g1.data()).copyTo(g1c);
    }

    void run(const cv::Mat &a, const cv::Mat &dv, const cv::Mat &dh, const cv::Mat &dd, cv::Mat &dst) {
        cv::Mat temp1, temp2, temp3, temp4, temp5;

        upsampleByRow(dd, temp1);
        cv::filter2D(temp1, temp2, -1, g1r, cv::Point(-1, -1), 0, borderType);
        upsampleByRow(dv, temp1);
        cv::filter2D(temp1, temp3, -1, g0r, cv::Point(-1, -1), 0, borderType);
        temp1 = temp2 + temp3;
        upsampleByColumn(temp1, temp2);
        cv::filter2D(temp2, temp4, -1, g1c, cv::Point(-1, -1), 0, borderType);

        upsampleByRow(dh, temp1);
        cv::filter2D(temp1, temp2, -1, g1r, cv::Point(-1, -1), 0, borderType);
        upsampleByRow(a, temp1);
        cv::filter2D(temp1, temp3, -1, g0r, cv::Point(-1, -1), 0, borderType);
        temp1 = temp2 + temp3;
        upsampleByColumn(temp1, temp2);
        cv::filter2D(temp2, temp5, -1, g0c, cv::Point(-1, -1), 0, borderType);

        dst = temp4 + temp5;
    }

protected:
    int borderType;
    cv::Mat g0r;
    cv::Mat g1r;
    cv::Mat g0c;
    cv::Mat g1c;
};

Multiresolution::Multiresolution(QQuickItem *parent) :
    QQuickItem(parent), ImageProccessor() {
    coefficients = {
        std::shared_ptr<FilterCoefficients>(new DaubechiesCoefficients()),
        std::shared_ptr<FilterCoefficients>(new SymletCoefficients()),
        std::shared_ptr<FilterCoefficients>(new Cohen_Daubechies_FeauveauCoefficients())
    };
}

QVariantList Multiresolution::subbandCoding(const QString &uri, int filterCoefficients, int borderType) {
    QVariantList result;
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat src;
        image->convertTo(src, CV_32F);

        cv::Mat a, dv, dh, dd;

        SubbandCoding coding(*coefficients[filterCoefficients], borderType);
        coding.run(src, a, dv, dh, dd);

        bool backup = m_resultNew;
        m_resultNew = true;
        result.push_back(putImage(a, uri));
        result.push_back(putImage(dv, uri));
        result.push_back(putImage(dh, uri));
        result.push_back(putImage(dd, uri));
        m_resultNew = backup;
    }
    return result;
}

QVariantList Multiresolution::fastWaveletTransform(const QString &uri, int filterCoefficients, int borderType) {
    QVariantList result;
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat src;
        image->convertTo(src, CV_32F);

        cv::Mat a, dv, dh, dd;

        FastWaveletTransform transform(*coefficients[filterCoefficients], borderType);
        transform.run(src, a, dv, dh, dd);

        bool backup = m_resultNew;
        m_resultNew = true;
        result.push_back(putImage(a, uri));
        result.push_back(putImage(dv, uri));
        result.push_back(putImage(dh, uri));
        result.push_back(putImage(dd, uri));
        m_resultNew = backup;
    }
    return result;
}

QString Multiresolution::subbandDecoding(const QString &a, const QString &dv, const QString &dh, const QString &dd, int filterCoefficients, int borderType) {
    const cv::Mat *image_a  = ImageManager::get(a);
    const cv::Mat *image_dv = ImageManager::get(dv);
    const cv::Mat *image_dh = ImageManager::get(dh);
    const cv::Mat *image_dd = ImageManager::get(dd);
    if (image_a && image_dv && image_dh && image_dd) {
        cv::Mat dst;
        SubbandDecoding decoding(*coefficients[filterCoefficients], borderType);
        decoding.run(*image_a, *image_dv, *image_dh, *image_dd, dst);
        return putImage(dst, dd);
    }
    return QString();
}

QString Multiresolution::inverseFastWaveletTransform(const QString &a, const QString &dv, const QString &dh, const QString &dd, int filterCoefficients, int borderType) {
    const cv::Mat *image_a  = ImageManager::get(a);
    const cv::Mat *image_dv = ImageManager::get(dv);
    const cv::Mat *image_dh = ImageManager::get(dh);
    const cv::Mat *image_dd = ImageManager::get(dd);
    if (image_a && image_dv && image_dh && image_dd) {
        cv::Mat dst;
        InverseWaveletTransform inverseTransform(*coefficients[filterCoefficients], borderType);
        inverseTransform.run(*image_a, *image_dv, *image_dh, *image_dd, dst);
        return putImage(dst, dd);
    }
    return QString();
}
