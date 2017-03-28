#include <polka_exception.h>
#include "Curve.h"

Curve::Curve(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      CurveFunc(),
      m_slipperShape(RectangleShape),
      m_maskType(None),
      m_curveType(Spline),
      m_slipperSize(QSizeF(6, 6)),
      m_gridPen(QPen(QColor(100, 100, 100, 100), 1)),
      m_slipperPen(QPen(QColor(255, 255, 255, 127), 1)),
      m_curvePen(QPen(QColor(255, 255, 255, 127), 2)),
      m_backgroundBrush(QBrush(Qt::black)),
      m_slipperBrush(QBrush(QColor(255, 255, 255))),
      m_topLeftBrush(QBrush(QColor(200,50,100))),
      m_bottomRightBrush(QBrush(QColor(50,150,50))),
      m_currentIndex(-1),
      m_anchorsInited(false) {
    // set the drawing render target to a paint FBO
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    // enable mouse events
    setAcceptedMouseButtons(Qt::AllButtons);
}

void Curve::calcCurve(const cv::Mat *image, std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax) {
    switch (m_curveType) {
    case Linear:
        calcLinearCurve(curve, anchors, xMax, yMax);
        break;
    case Cosine:
        calcCosineCurve(curve, anchors, xMax, yMax);
        break;
    case Spline:
    default:
        try {
            calcSplineCurve(curve, anchors, xMax, yMax);
        } catch(const polka_exception &ex) {
            calcLinearCurve(curve, anchors, xMax, yMax);
        }
    }
}

QQmlListProperty<Slipper> Curve::slippers() {
    return QQmlListProperty<Slipper>(this, 0, &Curve::appendSlipper, 0, 0, 0);
}

void Curve::appendSlipper(QQmlListProperty<Slipper> *list, Slipper *slipper) {
    Curve *curve = qobject_cast<Curve *>(list->object);
    curve->m_anchors.push_back(QPointF(slipper->x(), slipper->y()));
}

void Curve::mousePressEvent(QMouseEvent *event) {
    if (!m_adjustable)
        return;
    QPointF clickPos = event->pos();
    int     pos      = -1;
	qreal   w        = width();
	qreal   h        = height();
    qreal   dx       = m_slipperSize.width() + 2;
    qreal   dy       = m_slipperSize.height() * 2;
    qreal   clickx   = clickPos.x();
    qreal   clicky   = h - clickPos.y();
	size_t  xMax     = m_curve.size() - 1;
	for (int i = 0; i < xMax; ++i) {
		qreal x = static_cast<qreal>(i);
		qreal y = static_cast<qreal>(m_curve[i]);
        if (x - dx < clickx && clickx < x + dx && y - dy < clicky && clicky < y + dy) {
			pos = clickx;
			clicky = m_curve[static_cast<size_t>(clickx)];
            break;
        }
    }
	if (pos > -1) {
		int index = -1;
		for (int i = 0; i < m_anchors.size(); ++i) {
			qreal x = m_anchors[i].x() * w;
			if (x - dx < pos && pos < x + dx) {
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
			if (index > -1 && m_anchors.size() > 2) {
                m_anchors.erase(m_anchors.begin() + index);
				update();
			}
			else {
				for (int i = 0; i < m_anchors.size(); ++i) {
					if (pos <= m_anchors[i].x() * w) {
						index = i;
						break;
					}
				}
				if (index > 0) {
                    m_anchors.insert(m_anchors.begin() + index, QPointF(clickx / w, clicky / h));
					m_currentIndex = index;
					update();
				}
			}
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
		emit contextMenu(clickPos.x(), clickPos.y());
}

void Curve::mouseReleaseEvent(QMouseEvent *event) {
    m_currentIndex = -1;
}

void Curve::mouseMoveEvent(QMouseEvent *event) {
    if (m_currentIndex == -1)
        return;
    qreal left  = m_currentIndex == 0 ? 0.0f : m_anchors[m_currentIndex - 1].x();
    qreal right = m_currentIndex == m_anchors.size() - 1 ? 1.0f : m_anchors[m_currentIndex + 1].x();
    qreal x = event->pos().x() / width();
    qreal y = 1.0f - event->pos().y() / height();
    if (left <= x && x <= right && 0.0f <= y && y <= 1.0f) {
        m_anchors[m_currentIndex].setX(x);
        m_anchors[m_currentIndex].setY(y);
        update();
    }
}

void Curve::paint(QPainter *painter) {
	qreal w = width();
	qreal h = height();
	if (!(w > 0 && h > 0))
		return;

	qreal cellw = w / 10;
	qreal cellh = h / 10;
	painter->fillRect(0, 0, w, h, m_backgroundBrush);
    switch (m_maskType) {
    case LuminanceMask:
        {
            QLinearGradient gradient(0, 0, w, cellh / 2);
            gradient.setColorAt(0, Qt::black);
            gradient.setColorAt(1, Qt::white);
            painter->fillRect(0, h - cellh / 2, w, h, gradient);
        }
		break;
    case ColorMask:
        {
            QLinearGradient gradient(0, 0, w, cellh / 2);
            gradient.setColorAt(0, Qt::red);
            gradient.setColorAt(0.35, Qt::green);
            gradient.setColorAt(0.65, Qt::blue);
            gradient.setColorAt(1, Qt::red);
            painter->fillRect(0, h - cellh / 2, w, h, gradient);
        }
		break;
    case ColorBlocks:
        {
            qreal blockw = 3 * cellw;
            qreal blockh = 2 * cellh;
            painter->fillRect(0, 0, blockw, blockh, m_topLeftBrush);
            painter->fillRect(w - blockw, h - blockh, blockw, blockh, m_bottomRightBrush);
        }
        break;
	}

	qreal offsetx = cellw;
	qreal offsety = cellh;
	painter->setPen(m_gridPen);
	for (int i = 1; i < 10; ++i) {
		painter->drawLine(0, offsety, w, offsety);
		painter->drawLine(offsetx, 0, offsetx, h);
		offsetx += cellw;
		offsety += cellh;
	}

    if (!m_anchorsInited) {
        if (m_adjustable) {
			for (int i = 0; i < m_anchors.size(); ++i) {
				QPointF *p = &m_anchors[i];
				if (p->x() < 0) p->setX(0.0f);
				else if (p->x() > 1) p->setX(1.0f);
				if (p->y() < 0) p->setY(0.0f);
				else if (p->y() > 1) p->setY(1.0f);
			}
            if (m_anchors.size() < 2) {
                m_anchors.push_back(QPointF(0.0f, 0.0f));
                m_anchors.push_back(QPointF(0.5f, 0.5f));
                m_anchors.push_back(QPointF(1.0f, 1.0f));
            }
			std::sort(m_anchors.begin(), m_anchors.end(),
				[](const QPointF &p1, const QPointF &p2) {
				return p1.x() < p2.x();
			});
		}
        m_anchorsInited = true;
	}
	
	calcCurve(getImage(), m_curve, m_anchors, w - 1, h - 1);
    if (m_curve.size() == 0)
        return;

    painter->save();
    painter->setWindow(0, h, w, -h);

	QPainterPath path;
	path.moveTo(0, m_curve[0]);
	for (int i = 1; i < m_curve.size(); i++) {
		path.lineTo(i, m_curve[i]);
    }
	painter->setPen(m_curvePen);
	painter->drawPath(path);

    painter->setPen(m_slipperPen);
    painter->setBrush(m_slipperBrush);
	qreal pw = m_slipperSize.width();
	qreal ph = m_slipperSize.height();
    for (int i = 0; i < m_anchors.size(); ++i) {
		const QPointF *p = &m_anchors[i];
        qreal x = p->x() * w - pw / 2.0f;
        qreal y = p->y() * h - ph / 2.0f;
        switch (m_slipperShape) {
		case RectangleShape:
            painter->drawRect(x, y, pw, ph);
			break;
        case CircleShape:
            painter->drawEllipse(x, y, pw, ph);
            break;
		}
    }

    painter->restore();
}
