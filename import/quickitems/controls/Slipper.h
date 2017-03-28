#ifndef POINT_H
#define POINT_H

#include <QObject>

class Slipper : public QObject
{
    Q_OBJECT
	Q_ENUMS(LockType)
    Q_PROPERTY(LockType lockType READ lockType WRITE setLockType)
    Q_PROPERTY(qreal xp READ x WRITE setX)
    Q_PROPERTY(qreal yp READ y WRITE setY)
public:
	enum LockType {
        LockToLeft,
        LockToRight,
        LockToTop,
        LockToBottom,
        Free
	};
    LockType lockType() const { return m_lockType; }
    qreal    x() const { return m_x; }
    qreal    y() const { return m_y; }
	void setLockType(const LockType lockType) { m_lockType = lockType; }
    void setX(qreal x) { m_x = x; }
    void setY(qreal y) { m_y = y; }

    explicit Slipper(QObject *parent = 0);
signals:

public slots:

private:
	LockType m_lockType;
    qreal    m_x;
    qreal    m_y;
};

#endif // POINT_H
