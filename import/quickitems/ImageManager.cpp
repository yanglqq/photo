#include <unordered_map>
#include <regex>
#include "ImageManager.h"
#include "image.h"

static int imageSequence = 0;
static std::regex uriPattern = std::regex("image://polka/(.+)__\\d+");

std::unordered_map<QString, cv::Mat, QStringHasher> images;
std::unordered_map<QString, std::vector<std::shared_ptr<polka::ScaledRect>>, QStringHasher> masks;

ImageManager::ImageManager(QQuickItem *parent)
    : QQuickItem(parent), m_resultNew(true) {}

bool ImageManager::save(const QString &uri, const QString &path) {
    const cv::Mat *image = ImageManager::get(uri);
    if (!image)
        return false;
#ifdef Q_OS_WIN
    const QString filePath = path.startsWith("file://") ? path.right(path.length() - 8) : path;
#else
    const QString filePath = path.startsWith("file://") ? path.right(path.length() - 7) : path;
#endif
    return cv::imwrite(filePath.toStdString(), *image);
}

QString ImageManager::load(const QString &uri) {
    if (uri.startsWith("file://")) {
#ifdef Q_OS_WIN
        QString path = uri.right(uri.length() - 8);
#else
        QString path = uri.right(uri.length() - 7);
#endif
        return open(path);
    }
    return QString();
}

void ImageManager::remove(const QString &uri) {
    erase(uri);
}

QString ImageManager::toPath(const QString uri) {
    std::smatch result;
    std::string sURI = uri.toStdString();
    if (std::regex_match(sURI, result, uriPattern)) {
        std::string path = result[1];
        return QString(path.c_str());
    }
    return uri;
}

QString ImageManager::open(const QString &path) {
    QString uri;
    uri.append(imageProtocol);
    uri.append(path);
    uri.append("__");
    uri.append(std::to_string(++imageSequence).c_str());
    images[uri] = cv::imread(path.toStdString(), cv::IMREAD_UNCHANGED);
    masks[uri] = std::vector<std::shared_ptr<polka::ScaledRect>>();
    return uri;
}

QString ImageManager::put(const cv::Mat &image, const QString &uri) {
    QString newURI;
    newURI.append(imageProtocol);
    newURI.append(toPath(uri));
    newURI.append("__");
    newURI.append(std::to_string(++imageSequence).c_str());
    images[newURI] = image;
    masks[newURI] = std::vector<std::shared_ptr<polka::ScaledRect>>();
    return newURI;
}

const cv::Mat *ImageManager::get(const QString &uri) {
    std::unordered_map<QString, cv::Mat, QStringHasher>::const_iterator got = images.find(uri);
    if (got == images.end())
        return nullptr;
    return &(got->second);
}

std::vector<std::shared_ptr<polka::ScaledRect>>*ImageManager::getMasks(const QString &uri) {
    std::unordered_map<QString, std::vector<std::shared_ptr<polka::ScaledRect>>, QStringHasher>::const_iterator got = masks.find(uri);
    if (got == masks.end())
        return nullptr;
    return const_cast<std::vector<std::shared_ptr<polka::ScaledRect>> *>(&(got->second));
}

void ImageManager::replace(const cv::Mat &image, const QString &uri) {
    images[uri] = image;
}

void ImageManager::erase(const QString &uri) {
    images.erase(uri);
    masks.erase(uri);
}

QImage ImageManager::toQImage(const cv::Mat &inMat) {
   cv::Mat cvImage;
   bool temp = false;
   if (inMat.depth() == CV_32F) {
       inMat.convertTo(cvImage, CV_MAKE_TYPE(CV_8U, inMat.channels()));
       temp = true;
   }
   else
       cvImage = inMat;

   switch (cvImage.type()) {
      // 8-bit, 4 channel
      case CV_8UC4:
      {
         QImage image(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step1(), QImage::Format_RGB32);
         return temp ? image.copy() : image;
      }

      // 8-bit, 3 channel
      case CV_8UC3:
      {
         QImage image(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step1(), QImage::Format_RGB888);
         return image.rgbSwapped();
      }

      // 8-bit, 1 channel
      case CV_8UC1:
      {
         static QVector<QRgb> sColorTable;
         // only create our color table once
         if (sColorTable.isEmpty())
         {
            for (int i = 0; i < 256; ++i)
               sColorTable.push_back( qRgb( i, i, i ) );
         }
         QImage image(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step1(), QImage::Format_Indexed8);
         image.setColorTable(sColorTable);
         return temp ? image.copy() : image;
      }
      default:
         qWarning("Image::toQImage() - Mat image type not handled in switch: " + inMat.type());
   }
   return QImage();
}

cv::Mat ImageManager::toCvMat(const QImage &inImage, bool inCloneImageData)
{
   switch (inImage.format()) {
      // 8-bit, 4 channel
      case QImage::Format_RGB32:
      {
         cv::Mat mat(inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()));
         return (inCloneImageData ? mat.clone() : mat);
      }

      // 8-bit, 3 channel
      case QImage::Format_RGB888:
      {
         if (!inCloneImageData)
            qWarning("Image::toCvMat() - Conversion requires cloning since we use a temporary QImage");
         QImage swapped = inImage.rgbSwapped();
         return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits())).clone();
      }
      // 8-bit, 1 channel
      case QImage::Format_Indexed8:
      {
         cv::Mat mat(inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()));
         return (inCloneImageData ? mat.clone() : mat);
      }
      default:
         qWarning("Image::toCvMat() - QImage format not handled in switch: " + inImage.format());
         break;
   }
   return cv::Mat();
}
