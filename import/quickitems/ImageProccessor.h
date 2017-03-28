#ifndef IMAGEPROCCESSOR_H
#define IMAGEPROCCESSOR_H

#include <QString>
#include "../image.h"

class ImageProccessor
{
public:
    ImageProccessor();

    bool resultNew() { return m_resultNew; }
    void setResultNew(const bool resultNew) { m_resultNew = resultNew;}
protected:
    bool m_resultNew;

	QString putImage(const cv::Mat &image, const QString &uri);
};

#endif // IMAGEPROCCESSOR_H
