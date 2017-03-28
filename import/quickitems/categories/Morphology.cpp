#include <memory>
#include "Morphology.h"
#include "../../image.h"
#include "../imageManager.h"

Morphology::Morphology(QQuickItem *parent) :
    QQuickItem(parent), ImageProccessor() {}

void toKernel(const QVariantList &elements, int offset, int m, int n, cv::Mat &kernel) {
    kernel.create(n, m, CV_8U);
    uchar *ptr = (uchar *)kernel.data;
    for (int i = 0, k = offset, l = 0; i < n; i++) {
        for (int j = 0; j < m; j++, k++, l++) {
            ptr[l] = (uchar)elements[k].toInt();
        }
    }
}

QVariantList Morphology::createStructureElements(int m, int n, ElementsShape shape) {
    cv::Mat mat = cv::getStructuringElement(shape, cv::Size(m, n));
    QVariantList elements;
    uchar *ptr = (uchar *)mat.data;
    for (int i = 0, k = 0; i < n; i++) {
        for (int j = 0; j < m; j++, k++) {
            elements.push_back((int)ptr[k]);
        }
    }
    return elements;
}

inline cv::Point originCoordinate(int m, int n, int origin) {
    if (m == 1) return cv::Point(0, origin);
    if (n == 1) return cv::Point(origin, 0);
    return cv::Point(origin % m, origin / n);
}

QString Morphology::erode(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::erode(*image, dst, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::dilate(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::dilate(*image, dst, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::open(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::morphologyEx(*image, dst, cv::MORPH_OPEN, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::close(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::morphologyEx(*image, dst, cv::MORPH_CLOSE, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::gradient(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::morphologyEx(*image, dst, cv::MORPH_GRADIENT, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::tophat(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::morphologyEx(*image, dst, cv::MORPH_TOPHAT, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::bottomhat(const QString &uri, const QVariantList &elements, int m, int n, int origin, int iterations) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::morphologyEx(*image, dst, cv::MORPH_BLACKHAT, kernel, originCoordinate(m, n, origin), iterations, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(dst, uri);
}

QString Morphology::boundary(const QString &uri, const QVariantList &elements, int m, int n, int origin) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst;
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::erode(*image, dst, kernel, originCoordinate(m, n, origin), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    return putImage(*image - dst, uri);
}

QString Morphology::skeleton(const QString &uri, const QVariantList &elements, int m, int n, int origin) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        int K = 0;
        cv::Mat temp1, temp2;
        cv::Point oringinPoint = originCoordinate(m, n, origin);
        luminance.copyTo(temp1);
        while (1) {
            cv::erode(temp1, temp2, kernel, oringinPoint, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
            if (cv::countNonZero(temp2) == 0) break;
            temp2.copyTo(temp1);
            K++;
        }
        for (int k = 0; k <= K; k++) {
            luminance.copyTo(temp1);
            for (int i = 0; i < k; i++) {
                cv::erode(temp1, temp2, kernel, oringinPoint, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
                temp2.copyTo(temp1);
            }
            cv::morphologyEx(temp1, temp2, cv::MORPH_OPEN, kernel, oringinPoint, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
            cv::max(dst, temp1 - temp2, dst);
        }
    });
    return putImage(dst, uri);
}

void hitmiss(const cv::Mat &src, cv::Mat &dst, const cv::Mat &b1, const cv::Mat &b2, cv::Point &origin) {
    cv::Mat e1, e2;
    cv::erode(src, e1, b1, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    cv::dilate(src, e2, -b2, origin, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    dst = e1 - e2;
}

void toKernels(const QVariantList &elements,
               const QVariantList &ms,
               const QVariantList &ns,
               const QVariantList &origins,
               QVector<cv::Mat> &b1Array,
               QVector<cv::Mat> &b2Array,
               QVector<cv::Point> &originArray) {
    int count = ms.size();
    b1Array.resize(count);
    b2Array.resize(count);
    originArray.resize(count);
    cv::Mat kernel;
    for (int i = 0, offset = 0; i < count; i++) {
        int m = ms[i].toInt();
        int n = ns[i].toInt();
        int origin = origins[i].toInt();
        toKernel(elements, offset, m, n, kernel);
        b1Array[i] = kernel == 1;
        b2Array[i] = kernel == 0;
        originArray[i] = originCoordinate(m, n, origin);
        offset += m * n;
    }
}

QString Morphology::thin(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat temp1;
    cv::Mat temp2;
    cv::Mat temp3;
    QVector<cv::Mat> b1Array;
    QVector<cv::Mat> b2Array;
    QVector<cv::Point> originArray;
    toKernels(elements, ms, ns, origins, b1Array, b2Array, originArray);    
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat diff;
        int count = ms.size();
        luminance.copyTo(temp1);
        luminance.copyTo(temp3);
        while (1) {
            for (int i = 0; i < count; i++) {
                hitmiss(temp1, temp2, b1Array[i], b2Array[i], originArray[i]);
                temp1 = temp1 - temp2;
            }
            cv::compare(temp1, temp3, diff, cv::CMP_NE);
            if (cv::countNonZero(diff) == 0) break;
            temp1.copyTo(temp3);
        }
    });

    uchar *pdst = temp1.data;
    for (int i = 0, k = 0; i < temp1.rows; i++) {
        for (int j = 0; j < temp1.cols; j++, k++) {
            if (pdst[k] == 0) continue;
            // left
            if (i > 0 && j > 0 && pdst[k - 1] !=0 && pdst[k - temp1.cols - 1] != 0)
                pdst[k - 1] = 0;
            // up
            if (i > 0 && j < temp1.cols - 1 && pdst[k  - temp1.cols] != 0 && pdst[k - temp1.cols + 1] != 0)
                pdst[k  - temp1.cols] = 0;
            // right
            if (i < temp1.rows - 1 && j < temp1.cols -1 && pdst[k + 1] != 0 && pdst[k + temp1.cols + 1] != 0)
                pdst[k + 1] = 0;
            // down
            if (i < temp1.rows - 1 && j > 0 && pdst[k + temp1.cols] != 0 && pdst[k + temp1.cols - 1] != 0)
                pdst[k + temp1.cols] = 0;
        }
    }
    return putImage(temp1, uri);
}

QString Morphology::thicken(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat temp1;
    cv::Mat temp2;
    cv::Mat temp3;
    QVector<cv::Mat> b1Array;
    QVector<cv::Mat> b2Array;
    QVector<cv::Point> originArray;
    toKernels(elements, ms, ns, origins, b1Array, b2Array, originArray);
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat diff;
        int count = ms.size();
        luminance.copyTo(temp1);
        luminance.copyTo(temp3);
        while (1) {
            for (int i = 0; i < count; i++) {
                hitmiss(temp1, temp2, b1Array[i], b2Array[i], originArray[i]);
                cv::max(temp1, temp2, temp1);
            }
            cv::compare(temp1, temp3, diff, cv::CMP_NE);
            if (cv::countNonZero(diff) == 0) break;
            temp1.copyTo(temp3);
        }
    });
    return putImage(temp1, uri);
}

QString Morphology::prune(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat temp1;
    cv::Mat temp2;
    cv::Mat temp3;
    QVector<cv::Mat> b1Array;
    QVector<cv::Mat> b2Array;
    QVector<cv::Point> originArray;
    toKernels(elements, ms, ns, origins, b1Array, b2Array, originArray);
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        int count = ms.size();
        luminance.copyTo(temp1);
        for (int k = 0; k < 3; k++) {
            for (int i = 0; i < count; i++) {
                hitmiss(temp1, temp2, b1Array[i], b2Array[i], originArray[i]);
                temp1 = temp1 - temp2;
            }
        }
        temp2 = cv::Mat::zeros(temp2.rows, temp2.cols, temp2.type());
        for (int i = 0; i < count; i++) {
            hitmiss(temp1, temp3, b1Array[i], b2Array[i], originArray[i]);
            cv::max(temp2, temp3, temp2);
        }
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        for (int k = 0; k < 3; k++) {
            cv::dilate(temp2, temp3, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
            cv::min(temp3, luminance, temp2);
        }
        cv::max(temp1, temp2, temp3);
    });
    return putImage(temp3, uri);
}

QString Morphology::convexhull(const QString &uri, const QVariantList &elements, const QVariantList &ms, const QVariantList &ns, const QVariantList &origins) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    QVector<cv::Mat> b1Array;
    QVector<cv::Mat> b2Array;
    QVector<cv::Point> originArray;
    toKernels(elements, ms, ns, origins, b1Array, b2Array, originArray);
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat diff;
        cv::Mat temp1, temp2;
        int count = ms.size();
        for (int i = 0; i < count; i++) {
            luminance.copyTo(temp1);
            while (1) {
                hitmiss(temp1, temp2, b1Array[i], b2Array[i], originArray[i]);
                cv::max(temp1, temp2, temp2);
                cv::compare(temp1, temp2, diff, cv::CMP_NE);
                if (cv::countNonZero(diff) == 0) break;
                temp2.copyTo(temp1);
            }
            cv::max(dst, temp2, dst);
        }
    });
    return putImage(dst, uri);
}

QString Morphology::connectedComponents(const QString &uri, int pixelValue, bool largestComponent) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image) return QString();

    std::vector<std::shared_ptr<std::vector<cv::Point2i>>> components;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        polka::connectedComponents(luminance, components);
    });

    size_t i = 0, n1 = components.size();
    if (largestComponent) {
        size_t k = 0, max = 0, size;
        for (auto it = components.cbegin(); it < components.cend(); ++it, ++k) {
            size = (*it)->size();
            if (size > max) {
                max = size;
                i = k;
            }
        }
        n1 = i + 1;
    }

    size_t j, n2;
    cv::Point2i *p;
    cv::Mat dst(image->size(), CV_8UC1, cv::Scalar(0));
    for (; i < n1; i++) {
        std::shared_ptr<std::vector<cv::Point2i>> connected = components[i];
        for (j = 0, n2 = connected->size(); j < n2; j++) {
            p = &(connected->at(j));
            dst.at<uchar>(p->y, p->x) = pixelValue;
        }
    }
    return putImage(dst, uri);
}

QString Morphology::geodesicErode(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI, int iterations) {
    const cv::Mat *marker = ImageManager::get(markerURI);
    const cv::Mat *mask   = ImageManager::get(maskURI);
    if (!marker || !mask) return QString();
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::Mat dst;
    polka::geodesicErode(*mask, *marker, dst, iterations, kernel, originCoordinate(m, n, origin));
    return putImage(dst, markerURI);
}

QString Morphology::geodesicDilate(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI, int iterations) {
    const cv::Mat *marker = ImageManager::get(markerURI);
    const cv::Mat *mask   = ImageManager::get(maskURI);
    if (!marker || !mask) return QString();
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::Mat dst;
    polka::geodesicDilate(*mask, *marker, dst, iterations, kernel, originCoordinate(m, n, origin));
    return putImage(dst, markerURI);
}

QString Morphology::reconstructByErosion(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI) {
    const cv::Mat *marker = ImageManager::get(markerURI);
    const cv::Mat *mask   = ImageManager::get(maskURI);
    if (!marker || !mask) return QString();
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::Mat dst;
    polka::reconstructByErosion(*mask, *marker, dst, kernel, originCoordinate(m, n, origin));
    return putImage(dst, markerURI);
}

QString Morphology::reconstructByDilation(const QVariantList &elements, int m, int n, int origin, const QString &markerURI, const QString &maskURI) {
    const cv::Mat *marker = ImageManager::get(markerURI);
    const cv::Mat *mask   = ImageManager::get(maskURI);
    if (!marker || !mask) return QString();
    cv::Mat kernel;
    toKernel(elements, 0, m, n, kernel);
    cv::Mat dst;
    polka::reconstructByDilation(*mask, *marker, dst, kernel, originCoordinate(m, n, origin));
    return putImage(dst, markerURI);
}
