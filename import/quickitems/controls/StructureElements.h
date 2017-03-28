#ifndef STRUCTUREELEMENTS_H
#define STRUCTUREELEMENTS_H

#include <QQuickPaintedItem>
#include <QPainter>

class StructureElements : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)
    Q_PROPERTY(QColor elementColor READ elementColor WRITE setElementColor)
    Q_PROPERTY(QColor originColor READ originColor WRITE setOriginColor)
    Q_PROPERTY(int m READ m WRITE setM)
    Q_PROPERTY(int n READ n WRITE setN)
    Q_PROPERTY(int origin READ origin)
    Q_PROPERTY(QVariantList elements READ elements)    
public:
    explicit StructureElements(QQuickItem *parent = 0);

    QColor backgroundColor() const { return m_backgroundBrush.color(); }
    QColor gridColor() const { return m_gridPen.color(); }
    QColor elementColor() const { return m_elementBrush.color(); }
    QColor originColor() const { return m_originBrush.color(); }
    QSizeF elementSize() const { return m_elementSize; }

    int m() const { return m_m; }
    int n() const { return m_n; }
    int origin() const { return m_origin; }
    QVariantList elements() const {
        QVariantList list;
        for (int i = 0; i < m_elements.size(); i++) {
            list.push_back(m_elements[i]);
        }
        return list;
    }

    void setBackgroundColor(const QColor &color) { m_backgroundBrush.setColor(color); }
    void setGridColor(const QColor &color) { m_gridPen.setColor(color); }
    void setElementColor(const QColor &color) { m_elementBrush.setColor(color); }
    void setOriginColor(const QColor &color) { m_originBrush.setColor(color); }
    void setElementSize(const QSizeF &size) { m_elementSize = size; }

    void setM(int m);
    void setN(int n);

    Q_INVOKABLE void setElements(const QVariantList &elements, int m, int n, int origin);

protected:
    QPen   m_gridPen;
    QBrush m_backgroundBrush;
    QBrush m_elementBrush;
    QBrush m_originBrush;
    QSizeF m_elementSize;

    int    m_m;
    int    m_n;
    int    m_origin;

    qreal  m_gridLeft;
    qreal  m_gridTop;
    qreal  m_gridRight;
    qreal  m_gridBottom;

    QVector<int> m_elements;

    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void paint(QPainter *painter);
};

#endif // STRUCTUREELEMENTS_H
