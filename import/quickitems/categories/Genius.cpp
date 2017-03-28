#include <QDir>
#include "../ImageManager.h"
#include "Genius.h"

Genius::Genius(QQuickItem *parent)
    : QQuickItem(parent), ImageProccessor() {}

void Genius::detectObjects(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    std::vector<std::shared_ptr<polka::ScaledRect>> *masks = ImageManager::getMasks(uri);
    if (!image || !masks) return;

    QDir dir;
    QString path;
    QChar separator;
    path = dir.currentPath();
    separator = dir.separator();
    path.append(separator);
    path.append("data");
    path.append(separator);
    path.append("haarcascades");
    path.append(separator);
    path.append("haarcascade_frontalface_alt.xml");

    cv::CascadeClassifier classifier;
    if(!classifier.load(path.toStdString())) {
        qErrnoWarning(path.toStdString().c_str());
        return;
    }

    std::vector<cv::Rect> objects;
    polka::extractLuminace(*image, [&](const cv::Mat &luminance) {
        cv::Mat gray;
        cv::equalizeHist(luminance, gray);
        classifier.detectMultiScale(gray, objects, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    });
    masks->clear();
	masks->reserve(objects.size());
    for (auto it = objects.begin(); it != objects.end(); it++) {
        std::shared_ptr<polka::ScaledRect> p(new polka::ScaledRect);
        p->setXY(it->x, it->y, it->x + it->width, it->y + it->height, image->cols, image->rows);
        masks->push_back(p);
    }
}
