#include <QCursor>
#include "ImageMask.h"

ImageMask::ImageMask(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_recAnchorSize(4),
    m_state(NONE),
    m_pen(QPen(Qt::red, 1, Qt::SolidLine)) {
    m_masks = &m_defaultMasks;
    // set the drawing render target to a paint FBO
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    // enable mouse events
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
}

void ImageMask::setURI(const QString &uri) {
    m_masks = ImageManager::getMasks(uri);
    if (!m_masks)
        m_masks = &m_defaultMasks;
}

void ImageMask::drawSizableRectange(QPainter *painter, const QRectF &rect) {
	qreal width = (int)rect.width();
	qreal height = (int)rect.height();
    if (width < (int)m_recAnchorSize || height < (int)m_recAnchorSize) {
        m_isValidDrawing = false;
        return;
    }

    m_isValidDrawing = true;
    QPointF topLeft = rect.topLeft();
    int r = m_recAnchorSize / 2;
	int x = (int)topLeft.x();
	int y = (int)topLeft.y();
	int halfx = (int)width / 2;
	int halfy = (int)height / 2;

    QRect anchor(x - r, y - r, (int)m_recAnchorSize, (int)m_recAnchorSize);

    painter->translate(halfx, 0);
    painter->drawLine(x - halfx + r, y, x - r, y);
    painter->drawRect(anchor);

    painter->translate(halfx, 0);
    painter->drawLine(x - halfx + r, y, x - r, y);
    painter->drawRect(anchor);

    painter->translate(0, halfy);
    painter->drawLine(x, y - halfy + r, x, y - r);
    painter->drawRect(anchor);

    painter->translate(0, halfy);
    painter->drawLine(x, y - halfy + r, x, y - r);
    painter->drawRect(anchor);

    painter->translate(-halfx, 0);
    painter->drawLine(x + r, y, x + halfx - r, y);
    painter->drawRect(anchor);

    painter->translate(-halfx, 0);
    painter->drawLine(x + r, y, x + halfx - r, y);
    painter->drawRect(anchor);

    painter->translate(0, -halfy);
    painter->drawLine(x, y + r, x, y + halfy - r);
    painter->drawRect(anchor);

    painter->translate(0, -halfy);
    painter->drawLine(x, y + r, x, y + halfy - r);
    painter->drawRect(anchor);
}

void ImageMask::paint(QPainter *painter) {
    painter->setPen(m_pen);
    double left, top, w, h;
    for (auto it = m_masks->begin(); it != m_masks->end(); ++it) {
        (*it)->toRect(width(), height(), left, top, w, h);
        if ((*it)->selected)
            drawSizableRectange(painter, QRectF(left, top, w, h));
        else
            painter->drawRect(QRectF(left, top, w, h));
    }
}

bool ImageMask::makeSelection(const QPointF &position) {
    double left, top, w, h;
    double scalex = width();
    double scaley = height();
    for (auto it = m_masks->begin(); it != m_masks->end(); ++it) {
        (*it)->toRect(scalex, scaley, left, top, w, h);
        if (QRectF(left, top, w, h).contains(position)) {
            m_selectedMask = *it;
            m_selectedMask->selected = true;
			setCursor(QCursor(Qt::SizeAllCursor));
            m_state = SELECTED_MOVE;
            m_startPoint = position;
			update();
			return true;
		}
	}
	return false;
}

void ImageMask::mousePressEvent(QMouseEvent *event) {
	forceActiveFocus();
    switch (m_state) {
	case SELECTED:
	case SELECTED_MOVE:
        m_selectedMask->selected = false;
		if (!makeSelection(QPointF(event->pos()))) {
            m_state = NONE;
			update();
		}
		break;
	case NONE:
		makeSelection(QPointF(event->pos()));
	}
    if (m_state == NONE) {
        m_state = DRAW;
        m_startPoint = QPointF(event->pos());
        m_isValidDrawing = false;
        std::shared_ptr<polka::ScaledRect> mask(new polka::ScaledRect);
        m_masks->push_back(mask);
        m_selectedMask = mask;
        m_selectedMask->setXY(m_startPoint.x(), m_startPoint.y(), m_startPoint.x(), m_startPoint.y(), width(), height());
        m_selectedMask->selected = true;
	}
}

void ImageMask::mouseReleaseEvent(QMouseEvent *event) {
    switch (m_state) {
    case DRAW:
        if (!m_isValidDrawing) {
            m_masks->pop_back();
            m_state = NONE;
            break;
        }
    case RESIZE_TOPLEFT:
    case RESIZE_TOP:
    case RESIZE_TOPRIGHT:
    case RESIZE_RIGHT:
    case RESIZE_BOTTOMRIGHT:
    case RESIZE_BOTTOM:
    case RESIZE_BOTTOMLEFT:
    case RESIZE_LEFT:
    case SELECTED_MOVE:
        m_state = SELECTED;
        break;
    }
    unsetCursor();
}

inline bool hostpot(const QPointF &p, const qreal x, const qreal y) {
   return x - 4 < p.x() && p.x() < x + 4 && y - 4 < p.y() && p.y() < y + 4;
}

void ImageMask::hoverMoveEvent(QHoverEvent * event) {
    if (m_state == NONE || m_state == DRAW ) {
		return;
	}
    double left, top, right, bottom, w, h;
    QPointF position = event->pos();
    m_selectedMask->toRect(width(), height(), left, top, w, h);
    right = left + w;
    bottom = top + h;
	//topLeft
    if (hostpot(position, left, top)) {
		setCursor(QCursor(Qt::SizeFDiagCursor));
        m_state = RESIZE_TOPLEFT;
	}
	//topRight
    else if (hostpot(position, right, top)) {
		setCursor(QCursor(Qt::SizeBDiagCursor));
        m_state = RESIZE_TOPRIGHT;
	}
	//bottomRIGHT
    else if (hostpot(position, right, bottom)) {
		setCursor(QCursor(Qt::SizeFDiagCursor));
        m_state = RESIZE_BOTTOMRIGHT;
	}
	//bottomLEFT
    else if (hostpot(position, left, bottom)) {
		setCursor(QCursor(Qt::SizeBDiagCursor));
        m_state = RESIZE_BOTTOMLEFT;
	}
	//top
    else if (hostpot(position, (right - left) / 2 + left, top)) {
		setCursor(QCursor(Qt::SizeVerCursor));
        m_state = RESIZE_TOP;
	}
	//bottom
    else if (hostpot(position, (right - left) / 2 + left, bottom)) {
		setCursor(QCursor(Qt::SizeVerCursor));
        m_state = RESIZE_BOTTOM;
	}
	//left
    else if (hostpot(position, left, (bottom - top) / 2 + top)) {
		setCursor(QCursor(Qt::SizeHorCursor));
        m_state = RESIZE_LEFT;
	}
	//right
    else if (hostpot(position, right, (bottom - top) / 2 + top)) {
		setCursor(QCursor(Qt::SizeHorCursor));
        m_state = RESIZE_RIGHT;
	}
	//move
    else if (QRectF(left, top, w, h).contains(position)) {
		setCursor(QCursor(Qt::SizeAllCursor));
        m_state = SELECTED_MOVE;
        m_startPoint = position;
	}
	else {
		unsetCursor();
        m_state = SELECTED;
	}
}

void ImageMask::mouseMoveEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
	qreal x     = pos.x();
	qreal y     = pos.y();
	qreal w     = width();
	qreal h     = height();
    switch (m_state) {
	case RESIZE_TOPLEFT:
        if (x < m_recAnchorSize || y < m_recAnchorSize ||
            m_selectedMask->x2 * w - x < m_recAnchorSize ||
            m_selectedMask->y2 * h - y < m_recAnchorSize)
			return;
        m_selectedMask->resizeTopLeft(x, y, w, h);
		update();
		break;
	case RESIZE_TOP:
        if (y < m_recAnchorSize || m_selectedMask->y2 * h - y < m_recAnchorSize)
			return;
        m_selectedMask->resizeTop(y, h);
		update();
		break;
	case RESIZE_TOPRIGHT:
        if (x > w - m_recAnchorSize || y < m_recAnchorSize ||
            x - m_selectedMask->x1 * w < m_recAnchorSize ||
            m_selectedMask->y2 * h - y < m_recAnchorSize)
			return;
        m_selectedMask->resizeTopRight(x, y, w, h);
		update();
		break;
	case RESIZE_RIGHT:
        if (x > w - m_recAnchorSize || x - m_selectedMask->x1 * w < m_recAnchorSize)
			return;
        m_selectedMask->resizeRight(x, w);
		update();
		break;
	case DRAW:
	case RESIZE_BOTTOMRIGHT:
        if (x > w - m_recAnchorSize || y > h - m_recAnchorSize ||
            x - m_selectedMask->x1 * w < m_recAnchorSize ||
            y - m_selectedMask->y1 * h < m_recAnchorSize)
			return;
        m_selectedMask->resizeBottomRight(x, y, w, h);
		update();
		break;
	case RESIZE_BOTTOM:
        if ( y > h - m_recAnchorSize || y - m_selectedMask->y1 * h < m_recAnchorSize)
			return;
        m_selectedMask->resizeBottom(y, h);
		update();
		break;
	case RESIZE_BOTTOMLEFT:
        if (x < m_recAnchorSize || y > h - m_recAnchorSize ||
            m_selectedMask->x2 * w - x < m_recAnchorSize ||
            y - m_selectedMask->y1 * h < m_recAnchorSize)
			return;
        m_selectedMask->resizeBottomLeft(x, y, w, h);
		update();
		break;
	case RESIZE_LEFT:
        if (x < m_recAnchorSize || m_selectedMask->x2 * w - x < m_recAnchorSize)
			return;
        m_selectedMask->resizeLeft(x, w);
		update();
		break;
	case SELECTED_MOVE:
        qreal dx = x - m_startPoint.x();
        qreal dy = y - m_startPoint.y();
        qreal x1 = m_selectedMask->x1 * w + dx;
        qreal x2 = m_selectedMask->x2 * w + dx;
        qreal y1 = m_selectedMask->y1 * h + dy;
        qreal y2 = m_selectedMask->y2 * h + dy;
        if (x1 < m_recAnchorSize || y1 < m_recAnchorSize ||
            x2 > w - m_recAnchorSize || y2 > h - m_recAnchorSize)
			return;
        m_selectedMask->move(dx, dy, w, h);
        m_startPoint = pos;
		update();
		break;
    }
}

void ImageMask::keyPressEvent(QKeyEvent * event){
    if (m_state == SELECTED || m_state == SELECTED_MOVE) {
        if (event->key() == Qt::Key_Delete) {
            for (auto it = m_masks->begin(); it != m_masks->end(); ++it) {
                if (it->get() == m_selectedMask.get())
                    m_masks->erase(it);
            }
            m_state = NONE;
			unsetCursor();
            update();
        }
    }
}
