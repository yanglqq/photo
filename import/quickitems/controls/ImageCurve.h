#ifndef IMAGECURVE_H
#define IMAGECURVE_H

#include <QQuickItem>
#include "../ImageManager.h"
#include "Curve.h"

class ImageCurve : public Curve
{
    Q_OBJECT
    Q_PROPERTY(QString uri READ uri WRITE setURI)
public:
    explicit ImageCurve(QQuickItem *parent = 0)
        : Curve(parent) {}

    QString uri() const { return m_uri; }
    void setURI(const QString &uri) {
        m_uri = uri;
        m_newURI = true;
        if (this->isVisible())
            update();
    }

protected:
    QString m_uri;
    bool    m_newURI;

    const cv::Mat *getImage() {
        if (m_uri.length() == 0)
            return nullptr;
        return ImageManager::get(m_uri);
    }

};

#endif // IMAGECURVE_H
