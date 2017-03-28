#include <List>
#include "image.h"

namespace polka {

template<typename T>
inline static void meanStdDev_(const T* src, size_t step, cv::Size size, int channels, std::vector<double> &mean, std::vector<double> &stdDev) {
    mean.resize(channels);
    stdDev.resize(channels);
    for (int i = 0; i < channels; i++) {
        mean[i] = 0.0;
        stdDev[i] = 0.0;
    }

    double   count = 0.0;
    const T  *p    = src;
    int      h     = size.height;
    int      w     = size.width * channels - channels;

    for (; h--; p += step) {
        for (int i = 0; i <= w; i += channels, count++) {
            for (int j = 0; j < channels; j++) {
                mean[j] += static_cast<double>(p[i + j]);
            }
        }
    }
    for (int i = 0; i < channels; i++) {
        mean[i] /= count;
    }

    p = src;
    h = size.height;
    for (; h--; p += step) {
        for (int i = 0; i <= w; i += channels) {
            for (int j = 0; j < channels; j++) {
                stdDev[j] += std::pow(static_cast<double>(p[i + j]) - mean[j], 2);
            }
        }
    }
    for (int i = 0; i < channels; i++) {
        stdDev[i] = std::sqrt(stdDev[i] / count);
    }
}

#define DEF_MEAN_STDDEV(suffix, type) \
    inline static void meanStdDev_##suffix(const type* src, size_t step, cv::Size size, int channels, std::vector<double> &mean, std::vector<double> &stdDev) \
{ \
    return meanStdDev_(src, step, size, channels, mean, stdDev); \
}

typedef void(*meanStdDev_func)(const uchar* src, size_t step, cv::Size size, int channels, std::vector<double> &mean, std::vector<double> &stdDev);

DEF_MEAN_STDDEV(8u,  uchar)
DEF_MEAN_STDDEV(8s,  schar)
DEF_MEAN_STDDEV(16u, ushort)
DEF_MEAN_STDDEV(16s, short)
DEF_MEAN_STDDEV(32s, int)
DEF_MEAN_STDDEV(32f, float)
DEF_MEAN_STDDEV(64f, double)

void meanStdDev(cv::InputArray src, std::vector<double> &mean, std::vector<double> &stdDev) {
    static meanStdDev_func meanStdDev_table[] =
    {
        (meanStdDev_func)meanStdDev_8u,
        (meanStdDev_func)meanStdDev_8s,
        (meanStdDev_func)meanStdDev_16u,
        (meanStdDev_func)meanStdDev_16s,
        (meanStdDev_func)meanStdDev_32s,
        (meanStdDev_func)meanStdDev_32f,
        (meanStdDev_func)meanStdDev_64f,
        0
    };
    cv::Mat input = src.getMat();
    meanStdDev_table[input.depth()](input.data, input.step1(), input.size(), input.channels(), mean, stdDev);
}

void extractLuminace(const cv::Mat &src, std::function<void(const cv::Mat &luminance)> callback) {
    if (src.channels() == 1) {
        callback(src);
        return;
    }
    cv::Mat hsi;
    cv::Mat dst;
    cv::cvtColor(src, hsi, cv::COLOR_BGR2HLS);
    dst.create(hsi.rows, hsi.cols, CV_MAKE_TYPE(hsi.depth(), 1));
    int from_to_1[] = { 1,0 };
    cv::mixChannels(&hsi, 1, &dst, 1, from_to_1, 1);
    callback(dst);
}

void adjustLuminace(const cv::Mat &src, cv::Mat &dst, std::function<void(const cv::Mat &graySrc, cv::Mat &grayDst)> callback) {
    if (src.channels() == 1) {
        callback(src, dst);
        return;
    }
    cv::Mat hsi;
    cv::cvtColor(src, hsi, cv::COLOR_BGR2HLS);
    cv::Mat lumination(hsi.rows, hsi.cols, CV_MAKE_TYPE(hsi.depth(), 1));
    int from_to_1[] = { 1,0 };
    cv::mixChannels(&hsi, 1, &lumination, 1, from_to_1, 1);
    callback(lumination, dst);
    int from_to_2[] = { 0,1 };
    cv::mixChannels(&dst, 1, &hsi, 1, from_to_2, 1);
    cv::cvtColor(hsi, dst, cv::COLOR_HLS2BGR);
}

void fourier(const cv::Mat &src, cv::Mat &dst, bool center, bool restore, std::function<void(cv::Mat &frequency, cv::Mat &dst)> callback) {
    extractLuminace(src, [&](const cv::Mat &luminance){
        cv::Mat src32f;
        luminance.convertTo(src32f, CV_32F);
        //expand input image to optimal size
        cv::Mat padded;
        int m = cv::getOptimalDFTSize(src32f.rows);
        int n = cv::getOptimalDFTSize(src32f.cols);
        // on the border add zero values
        cv::copyMakeBorder(src32f, padded, 0, m - src.rows, 0, n - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        // center the transform
        if (center) {            
            int x, y;
            size_t step = padded.step1();
            float *p = (float *)padded.data;
            for (y = 0; y < padded.rows; p += step, y++) {
                for (x = 0; x < padded.cols; x++) {
                    *(p + x) *= std::powf(-1.0, static_cast<float>(x + y));
                }
            }
        }
        // Add to the expanded another plane with zeros
        cv::Mat planes[] = {padded, cv::Mat::zeros(padded.size(), CV_32F)};

        cv::Mat complex, frequency;
        cv::merge(planes, 2, complex);
        cv::dft(complex, frequency);
        callback(frequency, dst);

        // restore the transform
        if (restore) {
            int x, y;
            size_t step = dst.step1();
            float *p = (float *)dst.data;
            for (y = 0; y < dst.rows; p += step, y++) {
                for (x = 0; x < dst.cols; x++) {
                    *(p + x) *= std::powf(-1.0, static_cast<float>(x + y));
                }
            }
        }

        dst = cv::Mat(dst, cv::Rect(0, 0, src.cols, src.rows));
    });
}

void bgr2cmyk(const cv::Mat &src, cv::Mat &c, cv::Mat &m, cv::Mat &y, cv::Mat &k) {
    CV_Assert(src.channels() > 2);
    c.create(src.size(), CV_8UC1);
    m.create(src.size(), CV_8UC1);
    y.create(src.size(), CV_8UC1);
    k.create(src.size(), CV_8UC1);
    cv::Mat dst[] = {c, m, y};
    int from_to[] = { 0,2, 1,1, 2,0 };
    cv::mixChannels(&src, 1, &dst[0], 3, from_to, 3);
    uchar *p0 = c.data;
    uchar *p1 = m.data;
    uchar *p2 = y.data;
    uchar *p3 = k.data;    
    double r, g, b, _k;
    int count = src.rows * src.cols;
    for (int i = 0; i < count; i++) {
        r  = static_cast<double>(p0[i]) / 255.0;
        g  = static_cast<double>(p1[i]) / 255.0;
        b  = static_cast<double>(p2[i]) / 255.0;
        _k = std::min(std::min(1.0 - r, 1.0 - g), 1.0 - b);
        p0[i] = cv::saturate_cast<uchar>((1.0 - r - _k) / (1.0 - _k) * 255.0);
        p1[i] = cv::saturate_cast<uchar>((1.0 - g - _k) / (1.0 - _k) * 255.0);
        p2[i] = cv::saturate_cast<uchar>((1.0 - b - _k) / (1.0 - _k) * 255.0);
        p3[i] = cv::saturate_cast<uchar>(_k * 255.0);
    }
}

void cmyk2bgr(const cv::Mat &c, const cv::Mat &m, const cv::Mat &y, const cv::Mat &k, cv::Mat &dst) {
    dst.create(c.size(), CV_8UC3);
    uchar *p  = dst.data;
    uchar *p0 = c.data;
    uchar *p1 = m.data;
    uchar *p2 = y.data;
    uchar *p3 = k.data;
    double _c, _m, _y, _k;
    int count = dst.rows * dst.cols;
    for (int i = 0, j = 0; i < count; i++, j += 3) {
        _c = static_cast<double>(p0[i]) / 255.0;
        _m = static_cast<double>(p1[i]) / 255.0;
        _y = static_cast<double>(p2[i]) / 255.0;
        _k = static_cast<double>(p3[i]) / 255.0;
        p[j]     = cv::saturate_cast<uchar>((1.0 - _y * (1.0 - _k) - _k) * 255.0);
        p[j + 1] = cv::saturate_cast<uchar>((1.0 - _m * (1.0 - _k) - _k) * 255.0);
        p[j + 2] = cv::saturate_cast<uchar>((1.0 - _c * (1.0 - _k) - _k) * 255.0);
    }
}

template<typename T>
inline static void curveTransform_(
        const T* src, T* dst,
        size_t step, cv::Size size,
        int channels,
        std::vector<double> &curve) {
    T max = std::numeric_limits<T>::max();
    T min = std::numeric_limits<T>::min();
    double ratio = static_cast<double>(curve.size() - 1) / static_cast<double>(max - min);
    int height = size.height;
    int width  = size.width * channels - channels;
    for (; height--; src += step, dst += step) {
        for (int i = 0; i <= width; i++) {
            for (int j = 0; j < channels; j++) {
                dst[i + j] = cv::saturate_cast<T>(curve[static_cast<int>(static_cast<double>(src[i + j] - min) * ratio)]);
            }
        }
    }
}

#define DEF_CURVE_TRANSFORM(suffix, type) \
    inline static void curveTransform_##suffix(const type* src, type* dst, size_t step, cv::Size size, int channels, std::vector<double> &curve) \
{ \
    curveTransform_(src, dst, step, size, channels, curve); \
}

typedef void(*curveTransform_func)(const uchar* src, uchar* dst, size_t step, cv::Size size, int channels, std::vector<double> &curve);

DEF_CURVE_TRANSFORM(8u,  uchar)
DEF_CURVE_TRANSFORM(8s,  schar)
DEF_CURVE_TRANSFORM(16u, ushort)
DEF_CURVE_TRANSFORM(16s, short)
DEF_CURVE_TRANSFORM(32s, int)
DEF_CURVE_TRANSFORM(32f, float)
DEF_CURVE_TRANSFORM(64f, double)

void curveTransform(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
    static curveTransform_func curveTransform_table[] =
    {
        (curveTransform_func)curveTransform_8u,
        (curveTransform_func)curveTransform_8s,
        (curveTransform_func)curveTransform_16u,
        (curveTransform_func)curveTransform_16s,
        (curveTransform_func)curveTransform_32s,
        (curveTransform_func)curveTransform_32f,
        (curveTransform_func)curveTransform_64f,
        0
    };
    dst.create(src.size(), src.type());
    curveTransform_table[src.depth()](src.data, dst.data, src.step1(), src.size(), src.channels(), curve);
}

void basicGlobalThreshold(const cv::Mat &image, double &threshold) {
    cv::Mat hist;
    double min, max;
    cv::minMaxLoc(image, &min, &max);
    int hisSize = static_cast<int>(max - min + 1);
    float range[] = { static_cast<float>(min), static_cast<float>(max) } ;
    const float* histRanges[] = { range };
    int channel = 0;
    calcHist(&image, 1, &channel, cv::Mat(), hist, 1, &hisSize, histRanges, true, false);

    std::vector<double> accumlation(hisSize);
    float  *p  = (float *)hist.data;
    double sum = 0.0;
    for (int i = 0; i < hisSize; i++) {
        sum += p[i];
        accumlation[i] = sum;
    }
    int k = (hisSize - 1) / 2, old = hisSize;
    double m1, m2, sumk1, sumk2;
    while (1) {
        m1 = 0, m2 = 0;
        sumk1 = accumlation[k];
        for (int i = 0; i < k; i++) {
            m1 += static_cast<float>(i) * p[i];
        }
        m1 /= sumk1 + 0.001;
        sumk2 = sum - sumk1;
        for (int i = k; i < hisSize; i++) {
            m2 += static_cast<float>(i) * p[i];
        }
        m2 /= sumk2 + 0.001;
        k = static_cast<int>((m1 + m2) * 0.5);
        if (std::abs(old - k) <= threshold) {
            threshold = k + min;
            break;
        }
        old = k;
    }
}

void optimumGlobalThreshold1(const cv::Mat &image, double &threshold) {
    cv::Mat hist;
    double min, max;
    cv::minMaxLoc(image, &min, &max);
    int hisSize = static_cast<int>(max - min + 1);
    float range[] = { static_cast<float>(min), static_cast<float>(max) } ;
    const float* histRanges[] = { range };
    int channel = 0;
    calcHist(&image, 1, &channel, cv::Mat(), hist, 1, &hisSize, histRanges, true, false);

    std::vector<double> accumulation(hisSize);
    float  *p  = (float *)hist.data;
    double sum = 0.0;
    for (int i = 0; i < hisSize; i++) {
        sum += p[i];
        accumulation[i] = sum;
    }
    std::vector<double> betweenVars(hisSize);
    double m1, m2, sumk1, sumk2;
    for (int k = 0; k < hisSize; k++) {
        m1 = 0, m2 = 0;
        sumk1 = accumulation[k];
        for (int i = 0; i < k; i++) {
            m1 += static_cast<float>(i) * p[i];
        }
        m1 /= sumk1 + 0.001;
        sumk2 = sum - sumk1;
        for (int i = k; i < hisSize; i++) {
            m2 += static_cast<float>(i) * p[i];
        }
        m2 /= sumk2 + 0.001;
        betweenVars[k] = sumk1 * sumk2 * std::pow((m1 - m2), 2);
    }
    double maxVar = 0.0;
    for (int k = 0; k < hisSize; k++) {
        if (maxVar < betweenVars[k]) {
            maxVar = betweenVars[k];
        }
    }
    threshold = 0.0;
    double count = 0.0;
    for (int k = 0; k < hisSize; k++) {
        if (maxVar == betweenVars[k]) {
            threshold += k;
            count++;
        }
    }
    threshold /= count;
    threshold += min;
}

void optimumGlobalThreshold2(const cv::Mat &image, double &threshold1, double &threshold2) {
    cv::Mat hist;
    double min, max;
    cv::minMaxLoc(image, &min, &max);
    int hisSize = static_cast<int>(max - min + 1);
    float range[] = { static_cast<float>(min), static_cast<float>(max) } ;
    const float* histRanges[] = { range };
    int channel = 0;
    calcHist(&image, 1, &channel, cv::Mat(), hist, 1, &hisSize, histRanges, true, false);

    std::vector<double> accumulation(hisSize);
    float  *p  = (float *)hist.data;
    double sum = 0.0;
    for (int i = 0; i < hisSize; i++) {
        sum += p[i];
        accumulation[i] = sum;
    }
    double mG = 0.0;
    for (int i = 0; i < hisSize; i++) {
        mG += static_cast<float>(i) * p[i];
    }
    mG /= sum + 0.001;
    std::vector<cv::Vec3f> betweenVars;
    double m1, m2, m3, sumk1, sumk2, sumk3, betweenVar;
    for (int k1 = 0; k1 < hisSize - 1; k1++) {
        for (int k2 = k1 + 1; k2 < hisSize; k2++) {
            m1 = 0, m2 = 0, m3 = 0;
            sumk1 = accumulation[k1];
            for (int i = 0; i <= k1; i++) {
                m1 += static_cast<float>(i) * p[i];
            }
            m1 /= sumk1 + 0.001;

            sumk3 = sum - accumulation[k2];
            for (int i = k2; i < hisSize; i++) {
                m3 += static_cast<float>(i)* p[i];
            }
            m3 /= sumk3 + 0.001;

            sumk2 = sum - sumk1 - sumk3;
            for (int i = k1 + 1; i < k2; i++) {
                m2 += static_cast<float>(i) * p[i];
            }
            m2 /= sumk2 + 0.001;

            betweenVar = sumk1 * std::pow((m1 - mG), 2) + sumk2 * std::pow((m2 - mG), 2) + sumk3 * std::pow((m3 - mG), 2);
            betweenVars.push_back(cv::Vec3f(static_cast<float>(k1), static_cast<float>(k2), static_cast<float>(betweenVar)));
        }
    }
    double maxVar = 0.0;
    for (int k = 0, n = static_cast<int>(betweenVars.size()); k < n; k++) {
        if (maxVar < betweenVars[k][2]) {
            maxVar = betweenVars[k][2];
        }
    }
    threshold1 = 0.0;
    threshold2 = 0.0;
    double count = 0.0;
    for (int k = 0, n = static_cast<int>(betweenVars.size()); k < n; k++) {
        if (maxVar == betweenVars[k][2]) {
            threshold1 += betweenVars[k][0];
            threshold2 += betweenVars[k][1];
            count++;
        }
    }
    threshold1 /= count;
    threshold2 /= count;
    threshold1 += min;
    threshold2 += min;
}

template<typename T>
inline static void threshold2_(
        const T* src, T* dst,
        size_t step, cv::Size size,
        double threshold1, double threshold2, double value1, double value2, double value3) {
    int height = size.height;
    int width  = size.width;
    for (; height--; src += step, dst += step) {
        for (int i = 0; i < width; i++) {
            T value = src[i];
            if (value <= threshold1) dst[i] = cv::saturate_cast<T>(value1);
            else if (threshold1 < value && value <= threshold2) dst[i] = cv::saturate_cast<T>(value2);
            else dst[i] = cv::saturate_cast<T>(value3);
        }
    }
}

#define DEF_THRESHOLD2_FUNC(suffix, type) \
    inline static void threshold2_##suffix(const type* src, type* dst, size_t step, cv::Size size, double threshold1, double threshold2, double value1, double value2, double value3) \
{ \
    threshold2_(src, dst, step, size, threshold1, threshold2, value1, value2, value3); \
}

typedef void(*threshold2_func)(const uchar* src, uchar* dst, size_t step, cv::Size size, double threshold1, double threshold2, double value1, double value2, double value3);

DEF_THRESHOLD2_FUNC(8u,  uchar)
DEF_THRESHOLD2_FUNC(8s,  schar)
DEF_THRESHOLD2_FUNC(16u, ushort)
DEF_THRESHOLD2_FUNC(16s, short)
DEF_THRESHOLD2_FUNC(32s, int)
DEF_THRESHOLD2_FUNC(32f, float)
DEF_THRESHOLD2_FUNC(64f, double)

void threshold2(const cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2, double value1, double value2, double value3) {
    CV_Assert(src.channels() == 1);

    static threshold2_func threshold2_table[] =
    {
        (threshold2_func)threshold2_8u,
        (threshold2_func)threshold2_8s,
        (threshold2_func)threshold2_16u,
        (threshold2_func)threshold2_16s,
        (threshold2_func)threshold2_32s,
        (threshold2_func)threshold2_32f,
        (threshold2_func)threshold2_64f,
        0
    };    

    dst.create(src.size(), src.type());
    threshold2_table[src.depth()](
                src.data, dst.data,
                src.step1(), src.size(),
                threshold1, threshold2, value1, value2, value3);
}

template<typename T>
inline static void zigzagOrder_(const T* src, T* dst, size_t step, cv::Size size, int channels, int *indices) {
    int w = size.width;
    int h = size.height;
    int row = 0;
    int col = 0;
    int k = 0;
    int ii = 0;
    int i = 0;
    int step1 = static_cast<int>(step);
    while (row < h && col < w) {
        while (row > -1 && col < w) {
            for (int j = 0; j < channels; j++) {
                dst[i + j] = src[k + j];
            }
            indices[ii++] = k;
            i += channels;
            k = k - step1 + 1;
            row--;
            col++;
        }
        if (col == w) {
            row += 2;
            col--;
            k = k + 2 * step1 - 1;
        } else if (row == -1) {
            row++;
            k += step1;
        }
        while (row < h && col > -1) {
            for (int j = 0; j < channels; j++) {
                dst[i + j] = src[k + j];
            }
            indices[ii++] = k;
            i += channels;
            k = k + step1 - 1;
            row++;
            col--;
        }
        if (row == h) {
            row--;
            col += 2;
            k = k - step1 + 2;
        } else if (col == -1) {
            col++;
            k++;
        }
    }
}

#define DEF_ZIGZAG_ORDER(suffix, type) \
    inline static void zigzagOrder_##suffix(const type* src, type* dst, size_t step, cv::Size size, int channels, int *indices) \
{ \
    zigzagOrder_(src, dst, step, size, channels, indices); \
}

typedef void(*zigzagOrder_func)(const uchar* src, uchar* dst, size_t step, cv::Size size, int channels, int *indices);

DEF_ZIGZAG_ORDER(8u,  uchar)
DEF_ZIGZAG_ORDER(8s,  schar)
DEF_ZIGZAG_ORDER(16u, ushort)
DEF_ZIGZAG_ORDER(16s, short)
DEF_ZIGZAG_ORDER(32s, int)
DEF_ZIGZAG_ORDER(32f, float)
DEF_ZIGZAG_ORDER(64f, double)

void zigzagOrder(const cv::Mat &src, cv::Mat &dst, int *indices) {
    static zigzagOrder_func zigzagOrder_table[] =
    {
        (zigzagOrder_func)zigzagOrder_8u,
        (zigzagOrder_func)zigzagOrder_8s,
        (zigzagOrder_func)zigzagOrder_16u,
        (zigzagOrder_func)zigzagOrder_16s,
        (zigzagOrder_func)zigzagOrder_32s,
        (zigzagOrder_func)zigzagOrder_32f,
        (zigzagOrder_func)zigzagOrder_64f,
        0
    };
    dst.create(src.size(), src.type());
    zigzagOrder_table[src.depth()](
                src.data, dst.data,
                src.step1(), src.size(), src.channels(),
                indices);
}

template<typename T>
inline static void movingAverage_(const T* src, uchar* dst, size_t step, cv::Size size, uchar maxValue, double weightOfMean, int n) {
    int count = size.width * size.height;
    std::vector<T>   zigzag(count + n);
    std::vector<int> indices(count);
    T   *pZigzag  = (T *)&zigzag[0];
    int *pIndices = (int *)&indices[0];
    for (int i = 0; i < n; i++) {
        pZigzag[i] = 0;
    }
    zigzagOrder_(src, pZigzag + n, step, size, 1, pIndices);
    double mk = 0.0;
    for (int i = n, j = 0; j < count; i++, j++) {
        mk = mk + static_cast<double>(pZigzag[i] - pZigzag[i - n]) / static_cast<double>(n);
        dst[indices[j]] = pZigzag[i] > weightOfMean * mk ? maxValue : 0;
    }
}

#define DEF_MOVING_AVERAGE(suffix, type) \
    inline static void movingAverage_##suffix(const type* src, uchar* dst, size_t step, cv::Size size, uchar maxValue, double weightOfMean, int n) \
{ \
    movingAverage_(src, dst, step, size, maxValue, weightOfMean, n); \
}

typedef void(*movingAverage_func)(const uchar* src, uchar* dst, size_t step, cv::Size size, uchar maxValue, double weightOfMean, int n);

DEF_MOVING_AVERAGE(8u,  uchar)
DEF_MOVING_AVERAGE(8s,  schar)
DEF_MOVING_AVERAGE(16u, ushort)
DEF_MOVING_AVERAGE(16s, short)
DEF_MOVING_AVERAGE(32s, int)
DEF_MOVING_AVERAGE(32f, float)
DEF_MOVING_AVERAGE(64f, double)

void movingAverage(const cv::Mat &src, cv::Mat &dst, double maxValue, double weightOfMean, int n) {
    CV_Assert(src.channels() == 1);

    static movingAverage_func movingAverage_table[] =
    {
        (movingAverage_func)movingAverage_8u,
        (movingAverage_func)movingAverage_8s,
        (movingAverage_func)movingAverage_16u,
        (movingAverage_func)movingAverage_16s,
        (movingAverage_func)movingAverage_32s,
        (movingAverage_func)movingAverage_32f,
        (movingAverage_func)movingAverage_64f,
        0
    };
    if (n < 1) n = 1;
    dst.create(src.size(), CV_8UC1);
    movingAverage_table[src.depth()](src.data, dst.data, src.step1(), src.size(), static_cast<uchar>(maxValue), weightOfMean, n);
}

void partitionImage(const cv::Mat &src, cv::Mat &dst, int m, int n, std::vector<std::pair<cv::Mat, cv::Mat>> &subImages) {
    int subCols = src.cols / m;
    int subRows = src.rows / n;
    dst.create(src.size(), src.type());
    int offsetx = 0;
    int offsety = 0;
    for (int i = 0; i < n - 1; i++) {
        offsetx = 0;
        for (int j = 0; j < m - 1; j++) {
            cv::Rect roi(offsetx, offsety, subCols, subRows);
            subImages.push_back(std::make_pair(cv::Mat(src, roi), cv::Mat(dst, roi)));
            offsetx += subCols;
        }
        offsety += subRows;
    }
    int deltax = src.cols % m;
    int deltay = src.rows % n;
    offsety = 0;
    for (int i = 0; i < n - 1; i++) {
        cv::Rect roi(offsetx, offsety, subCols + deltax, subRows);
        subImages.push_back(std::make_pair(cv::Mat(src, roi), cv::Mat(dst, roi)));
        offsety += subRows;
    }
    offsetx = 0;
    for (int j = 0; j < m - 1; j++) {
        cv::Rect roi(offsetx, offsety, subCols, subRows + deltay);
        subImages.push_back(std::make_pair(cv::Mat(src, roi), cv::Mat(dst, roi)));
        offsetx += subCols;
    }
    cv::Rect roi(offsetx, offsety, subCols + deltax, subRows + deltay);
    subImages.push_back(std::make_pair(cv::Mat(src, roi), cv::Mat(dst, roi)));
}

template<typename T>
inline static bool max_diff_(const T* matA, const T* matB, T* dst, const T *ref, size_t step, cv::Size size) {
    T v;
    int diff = 0;
    for (; size.height--; matA += step, matB += step, ref += step, dst += step) {
        for (int i = 0; i < size.width; i++) {
            v = std::max(matA[i], matB[i]);
            if (ref[i] != v)
                diff++;
            dst[i] = v;
        }
    }
    return diff > 0;
}

#define DEF_MAX_DIFF(suffix, type) \
inline static bool max_diff_##suffix(const type* matA, const type* matB, type* dst, const type *ref, size_t step, cv::Size size) \
{ \
    return max_diff_(matA, matB, dst, ref, step, size); \
}

typedef bool(*max_diff_func)(const uchar* matA, const uchar* matB, uchar* dst, const uchar* ref, size_t step, cv::Size size);

DEF_MAX_DIFF(8u,  uchar)
DEF_MAX_DIFF(8s,  schar)
DEF_MAX_DIFF(16u, ushort)
DEF_MAX_DIFF(16s, short)
DEF_MAX_DIFF(32s, int)
DEF_MAX_DIFF(32f, float)
DEF_MAX_DIFF(64f, double)

template<typename T>
inline static bool min_diff_(const T* matA, const T* matB, T* dst, const T* ref, size_t step, cv::Size size) {
    T v;
    int diff = 0;
    for (; size.height--; matA += step, matB += step, ref += step, dst += step) {
        for (int i = 0; i < size.width; i++) {
            v = std::min(matA[i], matB[i]);
			if (ref[i] != v)
                diff++;
            dst[i] = v;
        }
    }
    return diff > 0;
}

#define DEF_MIN_DIFF(suffix, type) \
inline static bool min_diff_##suffix(const type* matA, const type* matB, type* dst, const type* ref, size_t step, cv::Size size) \
{ \
    return min_diff_(matA, matB, dst, ref, step, size); \
}

typedef bool(*min_diff_func)(const uchar* matA, const uchar* matB, uchar* dst, const uchar* ref, size_t step, cv::Size size);

DEF_MIN_DIFF(8u,  uchar)
DEF_MIN_DIFF(8s,  schar)
DEF_MIN_DIFF(16u, ushort)
DEF_MIN_DIFF(16s, short)
DEF_MIN_DIFF(32s, int)
DEF_MIN_DIFF(32f, float)
DEF_MIN_DIFF(64f, double)

bool max_diff(const cv::Mat &matA, const cv::Mat &matB, cv::Mat &dst, const cv::Mat &ref) {
    static max_diff_func max_diff_table[] =
    {
        (max_diff_func)max_diff_8u,
        (max_diff_func)max_diff_8s,
        (max_diff_func)max_diff_16u,
        (max_diff_func)max_diff_16s,
        (max_diff_func)max_diff_32s,
        (max_diff_func)max_diff_32f,
        (max_diff_func)max_diff_64f,
        0
    };
    dst.create(matA.size(), matA.type());
    return max_diff_table[matA.depth()](matA.data, matB.data, dst.data, ref.data, matA.step1(), matA.size());
}

bool min_diff(const cv::Mat &matA, const cv::Mat &matB, cv::Mat &dst, const cv::Mat &ref) {
    static min_diff_func min_diff_table[] =
    {
        (min_diff_func)min_diff_8u,
        (min_diff_func)min_diff_8s,
        (min_diff_func)min_diff_16u,
        (min_diff_func)min_diff_16s,
        (min_diff_func)min_diff_32s,
        (min_diff_func)min_diff_32f,
        (min_diff_func)min_diff_64f,
        0
    };
    dst.create(matA.size(), matA.type());
    return min_diff_table[matA.depth()](matA.data, matB.data, dst.data, ref.data, matA.step1(), matA.size());
}

void fillHole(cv::Mat &image, cv::Point2i &pointInHole) {

}

void connectedComponents(const cv::Mat &image, std::vector<std::shared_ptr<std::vector<cv::Point2i>>> &components) {
    CV_Assert(image.channels() == 1);

    cv::Mat matA;
    image.copyTo(matA);
    size_t step = matA.step1();
    cv::Mat matB = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat matX(matA.size(), CV_8UC1, cv::Scalar(0));

    cv::Mat temp;
    int x, y, i, j;
    uchar *p1 = matA.data, *p2, *p3;
    for (y = 0; y < matA.rows; p1 += step, y++) {
        for (x = 0; x < matA.cols; x++) {
            if (*(p1 + x) > 0) {
                matX.at<uchar>(y, x) = 1;
                while (true) {
                    cv::dilate(matX, temp, matB, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
                    if (!min_diff(temp, matA, matX, matX))
                        break;
                }
                std::shared_ptr<std::vector<cv::Point2i>> connected(new std::vector<cv::Point2i>());
                p2 = matX.data;
                for (i = 0; i < matX.rows; p2 += step, i++) {
                    for (j = 0; j < matX.cols; j++) {
                        p3 = p2 + j;
                        if (*p3 > 0) {
                            matA.at<uchar>(i, j) = 0;
                            connected->push_back(cv::Point2i(j, i));
                            *p3 = 0;
                        }
                    }
                }
				components.push_back(connected);
            }
        }
    }
}

void geodesicErode(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const int iterations, const cv::Mat &elements, const cv::Point &origin) {
    cv::Mat temp;
    marker.copyTo(dst);
    for (int i = 0; i < iterations; i++) {
        cv::erode(dst, temp, elements, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        if (!max_diff(temp, mask, dst, dst))
            break;
    }
}

void geodesicDilate(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const int iterations, const cv::Mat &elements, const cv::Point &origin) {
    cv::Mat temp;
    marker.copyTo(dst);
    for (int i = 0; i < iterations; i++) {
        cv::dilate(dst, temp, elements, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        if (!min_diff(temp, mask, dst, dst))
            break;
    }
}

void reconstructByErosion(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const cv::Mat &elements, const cv::Point &origin) {
    cv::Mat temp;
    marker.copyTo(dst);
    while (true) {
        cv::erode(dst, temp, elements, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        if (!max_diff(temp, mask, dst, dst))
            break;
    }
}

void reconstructByDilation(const cv::Mat &mask, const cv::Mat &marker, cv::Mat &dst, const cv::Mat &elements, const cv::Point &origin) {
    cv::Mat temp;
    marker.copyTo(dst);
    while (true) {
        cv::dilate(dst, temp, elements, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        if (!min_diff(temp, mask, dst, dst))
            break;
    }
}

inline int crossProduct(const cv::Point2i &p1, const cv::Point2i &p2) {
    return p1.x * p2.y - p2.x * p1.y;
}

inline int crossProduct(const cv::Point2i &p1, const cv::Point2i &p2, const cv::Point2i &p0) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

inline int manhattanDistance(const cv::Point2i &p1, const cv::Point2i &p2) {
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
}

void convexhull(const std::vector<cv::Point2i> &points, std::vector<cv::Point2i> &vertices, const int algorithm) {
    switch (algorithm) {
    case GRAHAM_SCAN:
        {
            int n = static_cast<int>(points.size());
            if (n < 3) return;

            int i = 0;
            int j = 0;
            std::vector<int> indices(n);
            int ymin = std::numeric_limits<int>::max();
            for (i = 0; i < n; i++) {
                indices[i] = i;
                if (points[i].y < ymin) {
                    ymin = points[i].y;
                    indices[0] = i;
                }
            }
            indices[indices[0]] = 0;

            const cv::Point2i &p0 = points[indices[0]];
            std::sort(indices.begin() + 1,
                      indices.end(),
                      [&points, &indices, &p0](int i, int j) {
                return crossProduct(points[i], points[j], p0) > 0;
			});

            std::vector<bool> pass(n);
            int dmax  = -1;
            int dimax = -1;
            int d1 = -1, d2;
			i = 0;
			pass[0] = true;
            while (++i < n) {
                if (crossProduct(points[indices[i - 1]], points[indices[i]], p0) == 0) {
					if (d1 == -1) {
                        d1 = manhattanDistance(p0, points[indices[i - 1]]);
						if (dmax < d1) {
							dmax = d1;
							dimax = i - 1;
						}
					}
                    d2 = manhattanDistance(p0, points[indices[i]]);
                    if (dmax < d2) {
                        dmax = d2;
                        dimax = i;
                    }
					d1 = d2;
                } else if (dimax != -1) {
                    pass[dimax] = true;
                    dmax = -1;
                    dimax = -1;
					d1 = -1;
                } else 
                    pass[i] = true;
            }

            std::vector<int> stack;
            for (i = 0, j = 0; i < n && j < 3; i++) {
				if (!pass[i])
					continue;
                stack.push_back(i);
				j++;
            }
            if (stack.size() < 3) return;

            int top = 2, nextTop = 1;
            for (; i < n; i++) {
				if (!pass[i])
					continue;
                while (true) {
                    if (crossProduct(points[indices[i]], points[indices[stack[top]]], points[indices[stack[nextTop]]]) < 0)
                        break;
                    stack.pop_back();
                    top--;
                    nextTop--;
                }
				stack.push_back(i);
				top++;
				nextTop++;
            }
            vertices.resize(stack.size());
			i = 0;
			j = 0;
            for (std::vector<int>::const_reverse_iterator it = stack.crbegin(); it != stack.crend(); ++it, ++i) {
                j = indices[*it];
                vertices[i].x = points[j].x;
                vertices[i].y = points[j].y;
            }
        }
        break;
    case JARVIS_MARCH:
        {
            int n = static_cast<int>(points.size());
            if (n < 3) return;

            int i, j, k, l;
            int ymin = std::numeric_limits<int>::max();
            for (k = 0, i = 0; i < n; i++) {
                j = points[i].y;
                if (j < ymin) {
                    ymin = j;
                    k = i;
                }
            }

            i = k;
            int c, d1, d2;
            const cv::Point2i *p;
            do {
                p = &points[i];
                vertices.push_back(*p);
                ++i %= n;
				if (i == k)
					break;
                for (j = (i + 1) % n, l = 0; l < n - 1; ++l, ++j %= n) {
                    c = crossProduct(points[i], points[j], *p);
                    if (c == 0) {
                       d1 = manhattanDistance(*p, points[i]);
                       d2 = manhattanDistance(*p, points[j]);
                       if (d1 < d2)
                           i = j;
                    } else if (c < 0)
                        i = j;
                }
            } while (true);
        }
        break;
    }
}

void farthestPoints(const std::vector<cv::Point2i> &points, int &p1, int &p2) {
    std::vector<cv::Point2i> vertices;
    convexhull(points, vertices, GRAHAM_SCAN);
    int xmin = std::numeric_limits<int>::max();
    int xmax = std::numeric_limits<int>::min();
    int ymin = xmin;
    int ymax = xmax;
    int ximin, ximax, yimin, yimax;
    int n = static_cast<int>(points.size());
    const cv::Point2i *p;
    for (int i = 0; i < n; i++) {
        p = &points[i];
        if (p->x < xmin) {
            xmin = p->x;
            ximin = i;
        }
        if (p->x > xmax) {
            xmax = p->x;
            ximax = i;
        }
        if (p->y < ymin) {
            ymin = p->y;
            yimin = i;
        }
        if (p->y > ymax) {
            ymax = p->y;
            yimax = i;
        }
    }
    if (manhattanDistance(points[ximin], points[ximax]) > manhattanDistance(points[yimin], points[yimax])) {
        p1 = ximin;
        p2 = ximax;
    } else {
        p1 = yimin;
        p2 = yimax;
    }
}

void skeleton(const cv::Mat &src, cv::Mat &dst) {
    CV_Assert(src.type() == CV_8UC1);

    cv::copyMakeBorder(src, dst, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    int delta = static_cast<int>(dst.step1());

    int neighbors1[8] = {
        -delta - 1,
        -delta,
        -delta + 1,
        1,
        delta + 1,
        delta,
        delta - 1,
        -1
    };
    static uchar neighbors2[9] = { 1, 2, 3, 4, 5, 6, 7, 0, 1};
    static uchar neighbors3[2][4] = { {3, 5, 1, 7}, {1, 7, 3, 5} };

    auto condition = [&](const uchar *p2, int step) {
        int i;

        int nc = 0;
        int nt = 0;
        for (i = 0; i < 8; i++) {
            if (*(p2 + neighbors1[neighbors2[i]]) > 0)
                nc++;
            if (*(p2 + neighbors1[neighbors2[i]]) == 0 && *(p2 + neighbors1[neighbors2[i + 1]]) > 0)
                nt++;
        }
        if (nc < 2 || nc > 6)
            return false;
        if (nt != 1)
            return false;

        if ((*(p2 + neighbors1[neighbors3[step][0]]) == 0 || *(p2 + neighbors1[neighbors3[step][1]]) == 0) ||
            (*(p2 + neighbors1[neighbors3[step][2]]) == 0 && *(p2 + neighbors1[neighbors3[step][3]]) == 0))
            return true;
        return false;
    };

    int ppn = 0;
    int ppl, ppi;
    std::vector<uchar *> pp;
    auto step = [&](int stepi) {
        int x, y;
        uchar *ps = dst.data, *p2;
        for (y = 0; y < dst.rows; ps += delta, y++) {
            for (x = 0; x < dst.cols; x++) {
                p2 = ps + x;
                if (*p2 == 0)
                    continue;
                if (condition(p2, stepi)) {
                    if (ppl == 0) {
                        pp.push_back(p2);
                        ppn++;
                        ppi++;
                    } else {
                        pp[ppi] = p2;
                        ppl--;
                        ppi++;
                    }
                }
            }
        }
    };

	int i;
	do {
        ppi = 0;
        ppl = ppn;
        step(0);
        for (i = 0; i < ppi; i++)
            *(pp[i]) = 0;

        ppi = 0;
        ppl = ppn;
        step(1);
        for (i = 0; i < ppi; i++)
            *(pp[i]) = 0;
    } while (ppi != 0);
	dst = cv::Mat(dst, cv::Rect(1, 1, src.cols, src.rows));
}

void principalComponentsTransform(const std::vector<cv::Point2i> &points, std::vector<cv::Point2i> &transformed) {
    int sampleSize = static_cast<int>(points.size());
    if (sampleSize == 0) return;

    cv::Mat samples(2, sampleSize, CV_64FC1);
    for (int i = 0; i < sampleSize; i++) {
        cv::Mat col = samples.col(i);
        col.at<double>(0) = points[i].x;
        col.at<double>(1) = points[i].y;
    }
    cv::Mat covariances;
    cv::Mat eigenvectors;
    cv::Mat mean;
    cv::Mat eigenvalues;
    cv::calcCovarMatrix(samples, covariances, mean, cv::COVAR_NORMAL | cv::COVAR_COLS);
    cv::eigen(covariances, eigenvalues, eigenvectors);

    for (int i = 0; i < sampleSize; i++) {
        cv::Mat col = samples.col(i);
        col.at<double>(0) = col.at<double>(0) - mean.at<double>(0);
        col.at<double>(1) = col.at<double>(1) - mean.at<double>(1);
    }
    cv::Mat dst = eigenvectors * samples;
    cv::Mat min;
    cv::reduce(dst, min, 1, cv::REDUCE_MIN);

    for (int i = 0; i < sampleSize; i++) {
        cv::Mat col = dst.col(i);
        col.at<double>(0) = col.at<double>(0) - min.at<double>(0);
        col.at<double>(1) = col.at<double>(1) - min.at<double>(1);
    }

    transformed.resize(sampleSize);
    for (int i = 0; i < sampleSize; i++) {
        cv::Mat col = dst.col(i);
        transformed[i].x = static_cast<int>(col.at<double>(0));
        transformed[i].y = static_cast<int>(col.at<double>(1));
    }
}

void principalComponents(const std::vector<cv::Mat> &inputArray, std::vector<cv::Mat> &outputArray) {

}

void boundaryFollowing(const cv::Mat &src, std::vector<cv::Point2i> &boundary) {
    CV_Assert(src.type() == CV_8UC1);

    cv::Mat srcPadded;
    cv::copyMakeBorder(src, srcPadded, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Point2i first;
    int  delta = static_cast<int>(srcPadded.step1());
    const uchar *ps = srcPadded.data;

    auto find = [&](){
        int x, y;
        for (y = 0; y < srcPadded.rows; ps += delta, y++) {
            for (x = 0; x < srcPadded.cols; x++) {
                if (ps[x] > 0) {
                    first.x = x - 1;
                    first.y = y - 1;
                    return true;
                }
            }
        }
        return false;
    };
    if (!find()) return;

    int neighbors[8][3] = {
        {-delta-1,  -1, -1},
        {-delta,     0, -1},
        {-delta+1,   1, -1},
        {1,          1,  0},
        {delta+1,    1,  1},
        {delta,      0,  1},
        {delta-1,   -1,  1},
        {-1,        -1,  0}
    };

    boundary.push_back(first);

    int i, j = 0, n = 1;
    cv::Point2i vertex = first;
    do {
        i = 0;
        for (; i < 8; i++, j++, j %= 8) {
            if (*(ps + vertex.x + neighbors[j][0] + 1) > 0) {
                vertex.x = vertex.x + neighbors[j][1];
                vertex.y = vertex.y + neighbors[j][2];
                if (n > 1 && boundary[n - 2].x == vertex.x && boundary[n - 2].y == vertex.y)
                    i = 8;
                else {
                    ps += neighbors[j][2] * delta;
                    boundary.push_back(vertex);
                    n++;
                    if (--j == -1) j = 7;
                }
                break;
            }
        }
    } while(i != 8 && (vertex.x != first.x || vertex.y != first.y));

    if (i == 8)
        boundary.clear();
}

void boundarySubSampling(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &sampled, int width, int height, int cellSize) {
    int n = static_cast<int>(boundary.size());
    if (n == 0) return;

    int c1x = boundary[0].x / cellSize;
    int c1y = boundary[0].y / cellSize;
    int c2x = c1x * cellSize;
    int c2y = c1y * cellSize;
    int hf  = cellSize / 2;
    int d[4] = {0,1,2,3}; // topLeft, topRight, bottomRight, bottomLeft
    int occupied[4] = {0, 0, 0, 0};
	int k, x, y;
    for (int j = 0, i = 0; j <= n; j++, ++i %= n) {
        x = boundary[i].x;
        y = boundary[i].y;

        if (c2x + cellSize <= x) {
            if (c2y + cellSize <= y) {
                if (occupied[d[0]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y));
                    occupied[d[0]] = 0;
                }
                if (occupied[d[1]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y));
                    occupied[d[1]] = 0;
                }
                if (occupied[d[3]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y + cellSize));
                    occupied[d[3]] = 0;
                }
				std::swap(d[0], d[2]);
                c2y += cellSize;
            } else {
                if (occupied[d[0]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y));
                    occupied[d[0]] = 0;
                }
                if (occupied[d[3]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y + cellSize));
                    occupied[d[3]] = 0;
                }
				std::swap(d[0], d[1]);
				std::swap(d[2], d[3]);
            }
            c2x += cellSize;
        } else if (x < c2x) {
            if (y < c2y) {
                if (occupied[d[1]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y));
                    occupied[d[1]] = 0;
                }
                if (occupied[d[2]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y + cellSize));
                    occupied[d[2]] = 0;
                }
                if (occupied[d[3]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y + cellSize));
                    occupied[d[3]] = 0;
                }
				std::swap(d[0], d[2]);
                c2y -= cellSize;
            } else {
                if (occupied[d[1]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y));
                    occupied[d[1]] = 0;
                }
                if (occupied[d[2]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y + cellSize));
                    occupied[d[2]] = 0;
                }
				std::swap(d[0], d[1]);
				std::swap(d[2], d[3]);
            }
            c2x -= cellSize;
        } else if (c2y + cellSize <= y) {
			if (x < c2x) {
				if (occupied[d[0]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y));
					occupied[d[0]] = 0;
				}
				if (occupied[d[1]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y));
					occupied[d[1]] = 0;
				}
				if (occupied[d[2]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y + cellSize));
					occupied[d[2]] = 0;
				}
				std::swap(d[2], d[3]);
				c2x -= cellSize;
			} else {
				if (occupied[d[0]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y));
					occupied[d[0]] = 0;
				}
				if (occupied[d[1]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y));
					occupied[d[1]] = 0;
				}
				std::swap(d[0], d[3]);
				std::swap(d[1], d[2]);
			}
            c2y += cellSize;
        } else if (y < c2y) {
			if (c2x + cellSize <= x) {
				if (occupied[d[0]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y));
					occupied[d[0]] = 0;
				}
				if (occupied[d[2]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y + cellSize));
					occupied[d[2]] = 0;
				}
				if (occupied[d[3]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y + cellSize));
					occupied[d[3]] = 0;
				}
				std::swap(d[1], d[3]);
				c2x += cellSize;
            } else {
				if (occupied[d[3]]) {
                    sampled.push_back(cv::Point2i(c2x, c2y + cellSize));
					occupied[d[3]] = 0;
				}
				if (occupied[d[2]]) {
                    sampled.push_back(cv::Point2i(c2x + cellSize, c2y + cellSize));
					occupied[d[2]] = 0;
				}
				std::swap(d[0], d[3]);
				std::swap(d[1], d[2]);
			}
            c2y -= cellSize;
        }

        k = 0;
        if (x < c2x + hf)
            k = y < c2y + hf ? 0 : 3;
        else
            k = y < c2y + hf ? 1 : 2;
        if (!occupied[d[k]])
            occupied[d[k]] = 1;
    }
    for (int i = 0, n = static_cast<int>(sampled.size()); i < n; i++) {
		if (sampled[i].x >= width) sampled[i].x = width - 1;
		if (sampled[i].y >= height) sampled[i].y = height - 1;
	}
}

inline void lineParameters(const cv::Point2i &point1, const cv::Point2i &point2, double &slope, double &intercect) {
    double x1 = static_cast<double>(point1.x);
    double y1 = static_cast<double>(point1.y);
    double x2 = static_cast<double>(point2.x);
    double y2 = static_cast<double>(point2.y);
    slope = (y1 - y2) / ((x1 - x2) + 0.0001);
    intercect = y1 - slope * x1;
}

void boundaryMerging(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &merged, double threshold) {
    int n = static_cast<int>(boundary.size());
    if (n == 0) return;

    const cv::Point2i *p = &boundary[0];
    merged.push_back(*p);

    double slope, intercect, error = 0.0;
    int i = 2;
    while (i < n) {
        lineParameters(*p, boundary[i], slope, intercect);
        error += std::abs(static_cast<double>(boundary[i - 1].y) - (slope * static_cast<double>(boundary[i - 1].x) + intercect));
        if (error > threshold) {
            p = &boundary[i];
            merged.push_back(*p);
            error = 0.0;
            i += 2;
        } else i++;
    }
}

void boundarySplitting(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &splitted, double threshold) {
    int n = static_cast<int>(boundary.size());
    if (n == 0) return;

    std::list<int> open, closed;
    int a, b;
    farthestPoints(boundary, a, b);
    open.push_back(b);
    open.push_back(a);
    closed.push_back(b);
    double slope, intercect, d, md, x, y;
    int k, i = 0;
    while (open.size() > 0) {
        b = closed.back();
        a = open.back();
        lineParameters(boundary[a], boundary[b], slope, intercect);
        md = 0.0;
        k = b;
        while (k != a) {
            x = static_cast<double>(boundary[k].x);
            y = static_cast<double>(boundary[k].y);
            d = std::abs(y - slope * x - intercect) / std::sqrt(slope * slope + 1.0);
            if (d >= md) {
                md = d;
                i = k;
            }
            if (--k == -1) k = n - 1;
        }
        if (md > threshold)
            open.push_back(i);
        else {
            closed.push_back(a);
            open.pop_back();
        }
    }
    k = 0;
    splitted.resize(closed.size());
    for (std::list<int>::const_reverse_iterator it = closed.crbegin(); it != closed.crend(); ++it, ++k) {
        splitted[k].x = boundary[*it].x;
        splitted[k].y = boundary[*it].y;
    }
}

#define sgn(v1, v2, v3) (v1.x * v2.y + v1.y * v3.x + v2.x * v3.y - v3.x * v2.y - v3.y * v1.x - v2.x * v1.y)

void minimumPerimeterPolygon(const std::vector<cv::Point2i> &boundary, std::vector<cv::Point2i> &polygon, int cellSize) {
    int n = static_cast<int>(boundary.size());
    if (n == 0) return;

    int c1x = boundary[0].x / cellSize;
    int c1y = boundary[0].y / cellSize;
    int c2x = c1x * cellSize;
    int c2y = c1y * cellSize;

    cv::Point2i vertex;
    cv::Point2i last(c2x, c2y + cellSize);
    cv::Point2i wc = last, bc = last;

    polygon.push_back(last);

    auto assertMPP = [&polygon, &vertex, &last, &bc, &wc](int vertexType) {
        while (true) {
            int s1 = sgn(last, wc, vertex);
            int s2 = sgn(last, bc, vertex);
            if (s1 > 0) {
                last = wc;
                bc = last;
                polygon.push_back(last);
            } else if (s2 < 0) {
                last = bc;
                wc = last;
                polygon.push_back(last);
            } else {
                // convex
                if (vertexType == 0)
                    wc = vertex;
                // concave
                else
                   bc = vertex;
                break;
            }
        }
    };

    int direction = 0;
    int x, y;

    for (int j = 1, i = 1; j <= n; j++, ++i %= n) {
        x = boundary[i].x;
        y = boundary[i].y;

        if (c2x + cellSize <= x) {
            if (direction == 1) {
                direction = 0;
                vertex.x = c2x + cellSize;
                vertex.y = c2y;
                assertMPP(1);
            } else if (direction == 3) {
                direction = 0;
                vertex.x = c2x + cellSize;
                vertex.y = c2y + cellSize;
                assertMPP(0);
            }
            c2x += cellSize;
        } else if (x < c2x) {
            if (direction == 1) {
                direction = 2;
                vertex.x = c2x;
                vertex.y = c2y;
                assertMPP(0);
            } else if (direction == 3) {
                direction = 2;
                vertex.x = c2x;
                vertex.y = c2y + cellSize;
                assertMPP(1);
            }
            c2x -= cellSize;
        } else if (c2y + cellSize <= y) {
            if (direction == 0) {
                direction = 1;
                vertex.x = c2x;
                vertex.y = c2y + cellSize;
                assertMPP(0);
            } else if (direction == 2) {
                direction = 1;
                vertex.x = c2x + cellSize;
                vertex.y = c2y + cellSize;
                assertMPP(1);
            }
            c2y += cellSize;
        } else if (y < c2y) {
            if (direction == 2) {
                direction = 3;
                vertex.x = c2x + cellSize;
                vertex.y = c2y;
                assertMPP(0);
            } else if (direction == 0) {
                direction = 3;
                vertex.x = c2x;
                vertex.y = c2y;
                assertMPP(1);
            }
            c2y -= cellSize;
        }
    }
}

}
