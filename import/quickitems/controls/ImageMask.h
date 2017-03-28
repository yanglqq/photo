#ifndef IMAGEMASK_H
#define IMAGEMASK_H
#include <list>
#include <QQuickPaintedItem>
#include <QPainter>
#include "../ImageManager.h"

class ImageMask : public QQuickPaintedItem
{
	Q_OBJECT
    Q_PROPERTY(QString uri READ uri WRITE setURI)
    Q_PROPERTY(qreal recAnchorSize READ recAnchorSize WRITE setRecAnchorSize)
public:
    explicit ImageMask(QQuickItem *parent = 0);

    QString uri() const { return m_uri; }
    qreal   recAnchorSize() const { return m_recAnchorSize; }
    void setURI(const QString &uri);
    void setRecAnchorSize(qreal recAnchorSize){ m_recAnchorSize = recAnchorSize; }
    void paint(QPainter *painter);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void hoverMoveEvent(QHoverEvent * event);
    void keyPressEvent(QKeyEvent * event);
private:
    enum State {
        NONE,
        RESIZE_TOPLEFT,
        RESIZE_TOP,
        RESIZE_TOPRIGHT,
        RESIZE_RIGHT,
        RESIZE_BOTTOMRIGHT,
        RESIZE_BOTTOM,
        RESIZE_BOTTOMLEFT,
        RESIZE_LEFT,
        SELECTED,
        SELECTED_MOVE,
        DRAW
    };

    QString     m_uri;
    qreal       m_recAnchorSize;
    QPointF     m_startPoint;
    State       m_state;
    bool        m_isValidDrawing;
    QPen        m_pen;

    std::shared_ptr<polka::ScaledRect> m_selectedMask;
    std::vector<std::shared_ptr<polka::ScaledRect>> *m_masks;
    std::vector<std::shared_ptr<polka::ScaledRect>> m_defaultMasks;

    bool makeSelection(const QPointF &position);
    void drawSizableRectange(QPainter *painter, const QRectF &rect);
};

#endif // IMAGEMASK_H
