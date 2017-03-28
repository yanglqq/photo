#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <memory>
#include <QQuickItem>
#include <QImage>
#include "../image.h"

const QString imageProtocol = "image://polka/";

struct QStringHasher
{
    std::size_t operator()(const QString& key) const {
        std::wstring str = key.toStdWString();
        std::hash<std::wstring> hash_fn;
        return hash_fn(str);
    }
};

class ImageManager : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool resultNew READ resultNew WRITE setResultNew NOTIFY resultNewChanged)
public:
    explicit ImageManager(QQuickItem *parent = 0);

    bool resultNew() { return m_resultNew; }
    void setResultNew(const bool resultNew) {
        if (m_resultNew != resultNew) {
            m_resultNew = resultNew;
            emit resultNewChanged();
        }
    }

    Q_INVOKABLE bool save(const QString &uri, const QString &path);
    Q_INVOKABLE QString load(const QString &uri);
    Q_INVOKABLE void remove(const QString &uri);

    static QString toPath(const QString uri);
    static QString open(const QString &path);
    static QString put(const cv::Mat &image, const QString &uri);
    static void replace(const cv::Mat &image, const QString &uri);
    static void erase(const QString &uri);
    static const cv::Mat *get(const QString &uri);

    static QImage  toQImage(const cv::Mat &inMat);
    static cv::Mat toCvMat(const QImage &inImage, bool inCloneImageData = true);

    static std::vector<std::shared_ptr<polka::ScaledRect>> *getMasks(const QString &uri);

signals:
    void resultNewChanged();

private:
    bool m_resultNew;

};

#endif // IMAGEMANAGER_H
