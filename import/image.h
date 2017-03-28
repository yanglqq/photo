#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <functional>
#include <memory>
#include <list>
#include <opencv2/opencv.hpp>

namespace polka {

class ScaledRect {
public:
    inline void setXY(const double X1, const double Y1, const double X2, const double Y2, const double scaleX, const double scaleY) {
        x1 = X1 / scaleX;
        x2 = X2 / scaleX;
        y1 = Y1 / scaleY;
        y2 = Y2 / scaleY;
    }
    inline void resizeTopLeft(const double X, const double Y, const double scaleX, const double scaleY) {
        x1 = X / scaleX;
        y1 = Y / scaleY;
    }
    inline void resizeTopRight(const double X, const double Y, const double scaleX, const double scaleY) {
        x2 = X / scaleX;
        y1 = Y / scaleY;
    }
    inline void resizeBottomRight(const double X, const double Y, const double scaleX, const double scaleY) {
        x2 = X / scaleX;
        y2 = Y / scaleY;
    }
    inline void resizeBottomLeft(const double X, const double Y, const double scaleX, const double scaleY) {
        x1 = X / scaleX;
        y2 = Y / scaleY;
    }
    inline void resizeTop(const double Y, const double scaleY) {
        y1 = Y / scaleY;
    }
    inline void resizeRight(const double X, const double scaleX) {
        x2 = X / scaleX;
    }
    inline void resizeBottom(const double Y, const double scaleY) {
        y2 = Y / scaleY;
    }
    inline void resizeLeft(const double X, const double scaleX) {
        x1 = X / scaleX;
    }
    inline void move(const double dx, const double dy, const double scaleX, const double scaleY) {
        double scaledDX = dx / scaleX;
        double scaledDY = dy / scaleY;
        x1 += scaledDX;
        x2 += scaledDX;
        y1 += scaledDY;
        y2 += scaledDY;
    }
    inline void toRect(const double scaleX, const double scaleY, double &left, double &top, double &width, double &height) const {
        left = x1 * scaleX;
        top = y1 * scaleY;
        width = x2 * scaleX - left;
        height = y2 * scaleY - top;
    }
    inline bool operator ==(const ScaledRect &rect) const {
        return x1 == rect.x1 && x2 == rect.x2 && y1 == rect.y1 && y2 == rect.y2 && selected == rect.selected;
    }

    double x1;
    double x2;
    double y1;
    double y2;
    bool selected;
};

/**

class MajorityFilter : public cv::BaseFilter {
public:
    MajorityFilter() {
        m_m = 3;
        m_n = 3;
        m_plocal.resize(9);
        anchor = cv::Point();
        ksize = cv::Size(3, 3);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;

        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_n; i++ ) {
                for (int j = 0; j < m_m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                int ones = 0;
                for (int k = 0; k < 9; k++) {
                    if (pl[k][i] > 0) {
                        ones++;
                    }
                }
                d[i] = ones < 5 ? 0 : pl[4][i];
            }
        }
    }
protected:
    int m_m;
    int m_n;
    std::vector<float*> m_plocal;
};

class GeometricMeanFilter : public cv::BaseFilter {
public:
    GeometricMeanFilter(int m, int n) {
        m_m = m;
        m_n = n;
        m_area = m * n;
        m_reciprocalArea = 1.0 / static_cast<float>(m_area);
        m_plocal.resize(m_area);
        anchor = cv::Point();
        ksize = cv::Size(m, n);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;

        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_n; i++ ) {
                for (int j = 0; j < m_m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                float s0 = 1.0;
                for (int k = 0; k < m_area; k++)
                    s0 *= pl[k][i];
                d[i] = static_cast<float>(std::pow(s0, m_reciprocalArea));
            }
        }
    }
protected:
    int m_m;
    int m_n;
    int m_area;
    double m_reciprocalArea;
    std::vector<float*> m_plocal;
};

class HarmonicMeanFilter : public cv::BaseFilter {
public:
    HarmonicMeanFilter(int m, int n) {
        m_m = m;
        m_n = n;
        m_area = m * n;
        m_plocal.resize(m_area);
        anchor = cv::Point();
        ksize = cv::Size(m, n);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;

        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_n; i++ ) {
                for (int j = 0; j < m_m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                double s0 = 0.0;
                for (int k = 0; k < m_area; k++)
                    s0 += 1.0 / pl[k][i];
                d[i] = static_cast<float>(m_area / s0);
            }
        }
    }
protected:
    int m_m;
    int m_n;
    int m_area;
    std::vector<float*> m_plocal;
};

class ContraharmonicMeanFilter : public cv::BaseFilter {
public:
    ContraharmonicMeanFilter(int m, int n, int q) {
        m_m = m;
        m_n = n;
        m_q = q;
        m_area = m * n;
        m_plocal.resize(m_area);
        anchor = cv::Point();
        ksize = cv::Size(m, n);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;

        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_n; i++ ) {
                for (int j = 0; j < m_m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                float s0 = 0.0;
                float s1 = 0.0;
                for (int k = 0; k < m_area; k++) {
                    s0 += pow(pl[k][i], m_q + 1);
                    s1 += pow(pl[k][i], m_q);
                }
                d[i] = s0 / s1;
            }
        }
    }
protected:
    int m_m;
    int m_n;
    int m_q;
    int m_area;
    std::vector<float*> m_plocal;
};

class AdaptiveLocalNoiseFilter : public cv::BaseFilter {
public:
    AdaptiveLocalNoiseFilter(int m, int n, int variance) {
        m_m = m;
        m_n = n;
        m_noiseVariance = variance;
        m_area = m * n;
        m_plocal.resize(m_area);
        m_vlocal.resize(m_area);
        anchor = cv::Point();
        ksize = cv::Size(m, n);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;
        int center = m_area / 2;
        std::vector<double> meanVec, stdVec;
        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_n; i++ ) {
                for (int j = 0; j < m_m; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                for (int k = 0; k < m_area; k++) {
                    m_vlocal[k] = pl[k][i];
                }
                cv::meanStdDev(m_vlocal, meanVec, stdVec);
                d[i] = static_cast<float>(pl[center][i] - m_noiseVariance * (pl[center][i] - meanVec[0]) / std::pow(stdVec[0], 2));
            }
        }
    }
protected:
    int m_m;
    int m_n;
    int m_noiseVariance;
    int m_area;
    std::vector<float*> m_plocal;
    std::vector<double> m_vlocal;
};

class AdaptiveMeanFilter : public cv::BaseFilter {
public:
    AdaptiveMeanFilter(int m, int n, int mMax, int nMax) {
        m_m = m;
        m_n = n;
        m_mMax = mMax;
        m_nMax = nMax;
        m_area = mMax * nMax;
        m_plocal.resize(m_area);
        anchor = cv::Point();
        ksize = cv::Size(mMax, nMax);
    }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width, int cn) {
        width *= cn;
        int center = m_area / 2;
        double maxl;
        double minl;
        double median;

        const float** pl = (const float**)&m_plocal[0];
        for (; count > 0; count--, dst += dststep, src++) {
            float* d = (float*)dst;
            for (int i = 0, k = 0; i < m_nMax; i++ ) {
                for (int j = 0; j < m_mMax; j++, k++) {
                    pl[k] = (const float *)src[i] + j * cn;
                }
            }
            for (int i = 0; i < width; i++) {
                int mtemp = m_m;
                int ntemp = m_n;
                while (true) {
                    int mn     = mtemp * ntemp;
                    int offset = (m_area - mn) / 2;
                    m_vlocal.resize(mn);
                    for (int k = offset; k < mn + offset; k++) {
                        m_vlocal[k - offset] = pl[k][i];
                    }

                    std::sort(m_vlocal.begin(), m_vlocal.end());
                    median = m_vlocal[static_cast<int>(static_cast<float>(m_vlocal.size()) * 0.5)];

                    cv::minMaxLoc(m_vlocal, &minl, &maxl);

                    if (median - minl > 0 && median - maxl < 0) {
                        d[i] = static_cast<float>(pl[center][i] - minl > 0 && pl[center][i] - maxl < 0 ? pl[center][i] : median);
                        break;
                    } else {
                        int mNew = mtemp;
                        int nNew = ntemp;
                        if (mtemp + 1 <= m_mMax)
                            mNew++;
                        if (ntemp + 1 <= m_nMax)
                            nNew++;
                        if (mNew == mtemp && nNew == ntemp) {
                            d[i] = pl[center][i];
                            break;
                        } else {
                            mtemp = mNew;
                            ntemp = nNew;
                        }
                    }
                }
            }
        }
    }
protected:
    int m_m;
    int m_n;
    int m_mMax;
    int m_nMax;
    int m_area;
    std::vector<float*> m_plocal;
    std::vector<double> m_vlocal;
};
**/

class FrequencyFilter {
public:
    virtual void operator()(const cv::Mat &src, const cv::Mat &frequency, cv::Mat &dst) = 0;
};

class SimpleFrequencyFilter : public FrequencyFilter {
public:
    SimpleFrequencyFilter(std::function<double(double d)> expression)
        : m_expression(expression){ }
    void operator()(const cv::Mat &src, const cv::Mat &frequency, cv::Mat &dst) {
        dst.create(frequency.size(), frequency.type());
        double n, d, value;
        int i, j, k, l;
        int   cn     = frequency.channels();
        float *pf    = (float *)frequency.data;
        float *pd    = (float *)dst.data;
        double halfP = static_cast<double>(frequency.cols) / 2.0;
        double halfQ = static_cast<double>(frequency.rows) / 2.0;
        for (i = 0, k = 0; i < frequency.rows; i++) {
            n = std::pow(static_cast<double>(i) - halfQ, 2.0);
            for (j = 0; j < frequency.cols; j++, k += cn) {
                d = std::sqrt(n + std::pow(static_cast<double>(j) - halfP, 2.0));
                value = m_expression(d);
                for (l = 0; l < cn; l++) {
                    pd[k + l] = pf[k + l] * static_cast<float>(value);
                }
            }
        }
    }
protected:
    std::function<double(double d)> m_expression;
};

class BlockNotchRejectFilter : public FrequencyFilter {
public:
    BlockNotchRejectFilter(const std::vector<std::shared_ptr<polka::ScaledRect>> *masks)
        : m_masks(masks){ }
    void operator()(const cv::Mat &src, const cv::Mat &frequency, cv::Mat &dst) {
        frequency.copyTo(dst);
        double x, y, w, h;
        int i, j, k, l;
        int    cn = frequency.channels();
        float *pd = (float *)dst.data;
        for (auto it = m_masks->cbegin(); it != m_masks->cend(); ++it) {
            (*it)->toRect(static_cast<double>(src.cols), static_cast<double>(src.rows), x, y, w, h);
            for (i = static_cast<int>(y); i < static_cast<int>(y + h); i++) {
                k = cn * (i * frequency.cols + static_cast<int>(x));
                for (j = static_cast<int>(x); j < static_cast<int>(x + w); j++, k += cn) {
                    for (l = 0; l < cn; l++) {
                        pd[k + l] = 0.0;
                    }
                }
            }
        }
    }
protected:
    const std::vector<std::shared_ptr<polka::ScaledRect>> *m_masks;
};

class BlockNotchPassFilter : public FrequencyFilter {
public:
    BlockNotchPassFilter(const std::vector<std::shared_ptr<polka::ScaledRect>> *masks)
        : m_masks(masks){ }
    void operator()(const cv::Mat &src, const cv::Mat &frequency, cv::Mat &dst) {
        dst.create(frequency.size(), frequency.type());
        dst.setTo(cv::Scalar(0.0));
        double x, y, w, h;
        int i, j, k, l;
        int    cn = frequency.channels();
        float *pf = (float *)frequency.data;
        float *pd = (float *)dst.data;
        for (auto it = m_masks->cbegin(); it != m_masks->cend(); ++it) {
            (*it)->toRect(static_cast<double>(src.cols), static_cast<double>(src.rows), x, y, w, h);
            for (i = static_cast<int>(y); i < static_cast<int>(y + h); i++) {
                k = cn * (i * frequency.cols + static_cast<int>(x));
                for (j = static_cast<int>(x); j < static_cast<int>(x + w); j++, k += cn) {
                    for (l = 0; l < cn; l++) {
                        pd[k + l] = pf[k + l];
                    }
                }
            }
        }
    }
protected:
    const std::vector<std::shared_ptr<polka::ScaledRect>> *m_masks;
};

class NotchFilter : public FrequencyFilter {
public:
    NotchFilter(double beta,
                double alpha,
                std::function<double(double d1, double d2)> expression,
                const std::vector<std::shared_ptr<polka::ScaledRect>> *masks)
        : m_beta(beta),
          m_alpha(alpha),
          m_expression(expression),
          m_masks(masks){ }

    void operator()(const cv::Mat &src, const cv::Mat &frequency, cv::Mat &dst) {
        dst.create(frequency.size(), frequency.type());
        int   cn     = frequency.channels();
        float *pf    = (float *)frequency.data;
        float *pd    = (float *)dst.data;
        double halfP = static_cast<double>(frequency.cols) / 2.0;
        double halfQ = static_cast<double>(frequency.rows) / 2.0;
        double x, y, w, h, value, mx, my, d1, d2;
        int i, j, k, l;
        for (i = 0, k = 0; i < frequency.rows; i++) {
            for (j = 0; j < frequency.cols; j++, k += cn) {
                value = 1.0;
                for (auto it = m_masks->cbegin(); it != m_masks->cend(); ++it) {
                    (*it)->toRect(static_cast<double>(src.cols), static_cast<double>(src.rows), x, y, w, h);
                    mx = x + w / 2.0 - halfP;
                    my = y + h / 2.0 - halfQ;
                    d1 = std::sqrt(std::pow(i - halfQ - my, 2.0) + std::pow(j - halfP - mx, 2.0));
                    d2 = std::sqrt(std::pow(i - halfQ + my, 2.0) + std::pow(j - halfP + mx, 2.0));
                    value *= m_expression(d1, d2);
                }
                value = value * m_beta + m_alpha;
                for (l = 0; l < cn; l++) {
                    pd[k + l] = pf[k + l] * static_cast<float>(value);
                }
            }
        }
    }
protected:
    double m_alpha;
    double m_beta;
    const std::vector<std::shared_ptr<polka::ScaledRect>> *m_masks;
    std::function<double(double d1, double d2)> m_expression;
};

void meanStdDev(cv::InputArray src, std::vector<double> &mean, std::vector<double> &stdDev);

void extractLuminace(const cv::Mat &src, std::function<void(const cv::Mat &luminance)> callback);
void adjustLuminace(const cv::Mat &src, cv::Mat &dst, std::function<void(const cv::Mat &src, cv::Mat &dst)> callback);
void fourier(const cv::Mat &src, cv::Mat &dst, bool center, bool restore, std::function<void(cv::Mat &frequency, cv::Mat &dst)> callback);
void curveTransform(const cv::Mat &src, cv::Mat &dest, std::vector<double> &curve);

void bgr2cmyk(const cv::Mat &src, cv::Mat &c, cv::Mat &m, cv::Mat &y, cv::Mat &k);
void cmyk2bgr(const cv::Mat &c, const cv::Mat &m, const cv::Mat &y, const cv::Mat &k, cv::Mat &dst);

void basicGlobalThreshold(const cv::Mat &image, double &threshold);
void optimumGlobalThreshold1(const cv::Mat &image, double &threshold);
void optimumGlobalThreshold2(const cv::Mat &image, double &threshold1, double &threshold2);
void threshold2(const cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2, double value1, double value2, double value3);
void zigzagOrder(const cv::Mat &src, cv::Mat &dst, int *indices);
void movingAverage(const cv::Mat &src, cv::Mat &dst, double maxValue, double weightOfMean, int n);
void partitionImage(const cv::Mat &src, cv::Mat &dst, int m, int n, std::vector<std::pair<cv::Mat, cv::Mat>> &subImages);

void fillHole(cv::Mat &image, cv::Point2i &pointInHole);
void connectedComponents(const cv::Mat &image, std::vector<std::shared_ptr<std::vector<cv::Point2i>>> &components);
void geodesicErode(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const int iterations, const cv::Mat &elements, const cv::Point &origin);
void geodesicDilate(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const int iterations, const cv::Mat &elements, const cv::Point &origin);
void reconstructByErosion(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const cv::Mat &elements, const cv::Point &origin);
void reconstructByDilation(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const cv::Mat &elements, const cv::Point &origin);

enum {
    GRAHAM_SCAN,
    JARVIS_MARCH
};

void convexhull(const std::vector<cv::Point2i> &points, std::vector<cv::Point2i> &vertices, const int algorithm);
void farthestPoints(const std::vector<cv::Point2i> &points, int &p1, int &p2);
void skeleton(const cv::Mat &src, cv::Mat &dst);

void principalComponentsTransform(const std::vector<cv::Point2i> &points, std::vector<cv::Point2i> &transformed);
void principalComponents(const std::vector<cv::Mat> &inputArray, std::vector<cv::Mat> &outputArray);

void boundaryFollowing(const cv::Mat &src, std::vector<cv::Point2i> &boundary);
void boundarySubSampling(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &sampled, int width, int height, int cellSize);
void boundaryMerging(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &merged, double threshold);
void boundarySplitting(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &splitted, double threshold);
void minimumPerimeterPolygon(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &polygon, int cellSize);

}

#endif // IMAGE_H
