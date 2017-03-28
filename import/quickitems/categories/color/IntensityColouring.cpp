#include <QQuickWindow>
#include "../../../image.h"
#include "../../ImageManager.h"
#include "IntensityColouring.h"

IntensityColouring::IntensityColouring(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_slipperSize(QSizeF(6, 6)),
      m_slipperPen(QPen(QColor(255, 255, 255), 1)),
      m_slipperBrush(QBrush(QColor(220, 220, 220))) {
	// set the drawing render target to a paint FBO
	setRenderTarget(QQuickPaintedItem::FramebufferObject);
	// enable mouse events
	setAcceptedMouseButtons(Qt::AllButtons);

	m_slippers.push_back(0);
	m_slippers.push_back(0.5);
	m_slippers.push_back(1);
	m_colors.push_back(QColor(0, 0, 0));
	m_colors.push_back(QColor(255, 255, 255));
}

void IntensityColouring::setColor(int index, const QColor &color) {
	m_colors[index] = color;
	update();
}

void IntensityColouring::mousePressEvent(QMouseEvent *event) {
    QPointF clickPos = event->pos();
	qreal   w        = width();
    qreal   h        = height();
	qreal   clickx   = clickPos.x();
    qreal   clicky   = clickPos.y();
    qreal   dy       = m_slipperSize.height() * 2;
    if (clicky < h - dy) {
        if (event->button() == Qt::RightButton) {
			for (int i = 1; i < m_slippers.size(); ++i) {
				if (clickx <= m_slippers[i] * w) {
					emit colorDialog(i - 1, m_colors[i - 1]);
					break;
				}
			}			
        }
        return;
    }    
    qreal dx     = m_slipperSize.width() + 2;    
    int   index  = -1;
    for (int i = 0; i < m_slippers.size(); ++i) {
        qreal x = m_slippers[i] * w;
        if (x - dx < clickx && clickx < x + dx) {
            index = i;
            break;
        }
    }
    switch (event->button()) {
    case Qt::LeftButton:
        if (index > -1)
            m_currentIndex = index;
        break;
    case Qt::RightButton:
		if (index > -1 && m_slippers.size() > 3) {
			m_slippers.erase(m_slippers.begin() + index);
			m_colors.erase(m_colors.begin() + index - 1);
		}
        else {
			index = 1;
			for (int i = 0; i < m_slippers.size(); ++i) {
				if (clickx <= m_slippers[i] * w) {
					index = i;
					break;
				}
			}
            m_slippers.insert(m_slippers.begin() + index, clickx / w);
			m_colors.insert(m_colors.begin() + index - 1, m_colors[index - 1]);
            m_currentIndex = index;
        }
        update();
    }
}

void IntensityColouring::mouseReleaseEvent(QMouseEvent *event) {
    m_currentIndex = -1;
}

void IntensityColouring::mouseMoveEvent(QMouseEvent *event) {
    if (m_currentIndex == -1)
        return;
    m_slippers[m_currentIndex] = event->pos().x() / width();
    update();
}

void IntensityColouring::paint(QPainter *painter) {
    qreal w = width();
    qreal h = height();
    if (!(w > 0 && h > 0))
        return;

    qreal gh = h - m_slipperSize.height() - 1;
    QLinearGradient gradient(0, 0, w, gh);
    size_t slippersCount = m_slippers.size();
	for (size_t i = 1; i < slippersCount; i++) {
		gradient.setColorAt((m_slippers[i] - m_slippers[i - 1]) / 2.0 + m_slippers[i - 1], m_colors[i - 1]);
    }
    m_colorMap = QImage(QSize(w, gh), QImage::Format_RGB32);
    QPainter p(&m_colorMap);
    p.fillRect(0, 0, w, gh, gradient);
    m_table = ImageManager::toCvMat(m_colorMap).row(0);
    painter->drawImage(0, 0, m_colorMap, 0, 0, w, gh);

    painter->setPen(m_slipperPen);
	qreal pw = m_slipperSize.width();
	qreal ph = m_slipperSize.height();
	qreal y  = h - 1;
	qreal hf = pw / 2.0;
    for (int i = 1; i < slippersCount - 1; ++i) {
        qreal x1 = m_slippers[i] * w;
		qreal x2 = x1 + hf;
		qreal x3 = x1 + pw;
		QPainterPath path;
		path.moveTo(x1, y);
		path.lineTo(x2, y - ph);
		path.lineTo(x3, y);
		path.lineTo(x1, y);
		painter->fillPath(path, m_slipperBrush);
    }
}

QString IntensityColouring::colour(const QString &uri) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat src;
        if (image->channels() == 1)
            image->convertTo(src, CV_8U);
        else
            cv::cvtColor(*image, src, cv::COLOR_BGR2GRAY);
        cv::Mat dst(src.rows, src.cols, CV_8UC3);
        double  ratio = static_cast<double>(m_table.cols - 1) / static_cast<double>(255);
        uchar*  psrc  = src.data;
        uchar*  pdst  = dst.data;
        uchar*  pt    = m_table.data;
        size_t  step1 = src.step1();
		size_t  step2 = dst.step1();
        int height = src.rows;
        int width  = src.cols;
        for (; height--; psrc += step1, pdst += step2) {
            for (size_t i = 0, k = 0; i < width; i++, k += 3) {
                size_t index = static_cast<size_t>(static_cast<double>(psrc[i]) * ratio) * 4;
                for (size_t j = 0; j < 3; j++) {
                    pdst[k + j] = pt[index + j];
                }
            }
        }
        return putImage(dst, uri);
    }
    return QString();
}
