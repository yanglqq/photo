#include "StructureElements.h"

StructureElements::StructureElements(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_gridPen(QPen(QColor(100, 100, 100, 100), 2)),
      m_backgroundBrush(QBrush(Qt::black)),
      m_elementBrush(QBrush(QColor(255, 255, 255))),
      m_originBrush(QBrush(QColor(200,50,100))),
      m_elementSize(QSizeF(8, 8)),
      m_m(3),
      m_n(3),
      m_origin(4)
{
    m_elements.resize(m_m * m_n);
    m_elements.fill(1);
    // set the drawing render target to a paint FBO
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    // enable mouse events
    setAcceptedMouseButtons(Qt::AllButtons);
}

void StructureElements::mousePressEvent(QMouseEvent *event) {
    QPointF clickPos = event->pos();
    qreal clickx = clickPos.x();
    qreal clicky = clickPos.y();
    if (clickx < m_gridLeft ||
        clickx > m_gridRight ||
        clicky < m_gridTop ||
        clicky > m_gridBottom)
        return;
    qreal ew  = m_elementSize.width();
    qreal eh  = m_elementSize.height();
    int x = (clickx - m_gridLeft) / ew;
    int y = (clicky - m_gridTop) / eh;
    int index = y * m_m + x;
    if (m_elements[index] != 0)
        m_elements[index] = 0;
    else if (event->button() == Qt::LeftButton)
        m_elements[index] = 1;
    else if (event->button() == Qt::RightButton)
        m_elements[index] = -1;
    update();
}

void StructureElements::mouseDoubleClickEvent(QMouseEvent *event) {
    QPointF clickPos = event->pos();
    qreal clickx = clickPos.x();
    qreal clicky = clickPos.y();
    if (clickx < m_gridLeft ||
        clickx > m_gridRight ||
        clicky < m_gridTop ||
        clicky > m_gridBottom)
        return;
    qreal ew  = m_elementSize.width();
    qreal eh  = m_elementSize.height();
    int x = (clickx - m_gridLeft) / ew;
    int y = (clicky - m_gridTop) / eh;
    m_origin = y * m_m + x;
    update();
}

void StructureElements::setM(int m) {
    if (m <= 0 || m == m_m) return;
    qreal w   = width();
    qreal ew  = m_elementSize.width();
    qreal esw = m * ew;
    if (esw > w) {
        esw = w;
        m = esw / ew;
    }
    if (m == m_m) return;

    QVector<int> elements(m * m_n);
    elements.fill(0);
    if (m > m_m) {
        int inc = (m - m_m) / 2;
        for (int i = 0; i < m_m; i++) {
            for (int j = 0; j < m_n; j++) {
                elements[j * m + i + inc] = m_elements[j * m_m + i];
            }
        }
    } else {
        int dec = (m_m - m) / 2;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < m_n; j++) {
                elements[j * m + i] = m_elements[j * m_m + i + dec];
            }
        }
    }
    m_origin = elements.size() / 2;
    m_elements = elements;
    m_m = m;
}

void StructureElements::setN(int n) {
    if (n <= 0 || n == m_n) return;
    qreal h   = height();
    qreal eh  = m_elementSize.height();
    qreal esh = n * eh;
    if (esh > h) {
        esh = h;
        n = esh / eh;
    }
    if (n == m_n) return;

    QVector<int> elements(m_m * n);
    elements.fill(0);
    if (n > m_n) {
        int inc = (n - m_n) / 2;
        for (int i = 0; i < m_m; i++) {
            for (int j = 0; j < m_n; j++) {
                elements[(j + inc) * m_m + i] = m_elements[j * m_m + i];
            }
        }
    } else {
        int dec = (m_n - n) / 2;
        for (int i = 0; i < m_m; i++) {
            for (int j = 0; j < n; j++) {
                elements[j * m_m + i] = m_elements[(j + dec) * m_m + i];
            }
        }
    }
    m_origin = elements.size() / 2;
    m_elements = elements;
    m_n = n;
}

void StructureElements::setElements(const QVariantList &elements, int m, int n, int origin) {
    m_elements.resize(elements.size());
    for (int i = 0; i < elements.size(); i++) {
        m_elements[i] = elements[i].toInt();
    }
    m_m = m;
    m_n = n;
    m_origin = origin;
}

void StructureElements::paint(QPainter *painter) {
    qreal w = width();
    qreal h = height();
    if (!(w > 0 && h > 0))
        return;

    painter->fillRect(0, 0, w, h, m_backgroundBrush);

    qreal ew  = m_elementSize.width();
    qreal eh  = m_elementSize.height();
    qreal esw = m_m * ew;
    qreal esh = m_n * eh;
    if (esw > w) {
        esw = w;
        m_m = esw / ew;
    }
    if (esh > h) {
        esh = h;
        m_n = esh / eh;
    }

    m_gridLeft   = (w - esw) / 2;
    m_gridTop    = (h - esh) / 2;
    m_gridRight  = m_gridLeft + esw;
    m_gridBottom = m_gridTop + esh;

    for (int i = 0; i < m_elements.size(); i++) {
        qreal x = i % m_m;
        qreal y = i / m_m;
        if (m_elements[i] == 1)
            painter->fillRect(m_gridLeft + x * ew, m_gridTop + y * eh, ew, eh, m_elementBrush);
        else if (m_elements[i] == -1) {
            qreal left = m_gridLeft + x * ew;
            qreal top = m_gridTop + y * eh;
            painter->setPen(m_gridPen);
            painter->drawLine(left, top, left + ew, top + eh);
            painter->drawLine(left + ew, top, left, top + eh);
        }
    }
    qreal x = m_origin % m_m;
    qreal y = m_origin / m_m;
    painter->setBrush(m_originBrush);
    painter->drawEllipse(m_gridLeft + x * ew + 1, m_gridTop + y * eh + 1, ew - 2, eh - 2);

    qreal offsetx = m_gridLeft;
    qreal offsety = m_gridTop;
    painter->setPen(m_gridPen);
    for (int i = 0; i <= m_m; ++i) {
        painter->drawLine(offsetx, m_gridTop, offsetx, m_gridBottom);
        offsetx += ew;
    }
    for (int i = 0; i <= m_n; ++i) {
        painter->drawLine(m_gridLeft, offsety, m_gridRight, offsety);
        offsety += eh;
    }
}
