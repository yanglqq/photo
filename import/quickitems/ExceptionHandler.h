#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <QQuickItem>

class ExceptionHandler : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool ignoreWarning READ ignoreWarning WRITE setIgnoreWarning)
public:
    explicit ExceptionHandler(QQuickItem *parent = 0);

    bool ignoreWarning() const { return m_ignoreWarning; }
    void setIgnoreWarning(const bool &ignoreWarning) { m_ignoreWarning = ignoreWarning; }
signals:
    void exception(QString message);    
private:
   bool m_ignoreWarning;
};

#endif // EXCEPTIONHANDLER_H
