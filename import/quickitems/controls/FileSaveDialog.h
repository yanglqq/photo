#ifndef FILESAVEDIALOG_H
#define FILESAVEDIALOG_H

#include <QQuickItem>
#include <QFileDialog>
#include <QUrl>
#include <private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformdialoghelper.h>
#include <QtGui/qpa/qplatformtheme.h>

class FileSaveDialog : public QQuickItem
{
    Q_OBJECT
public:
    explicit FileSaveDialog(QQuickItem *parent = 0);
    ~FileSaveDialog();

    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)
    QUrl fileUrl() const { return m_fileUrl; }

    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    QString filename() const { return m_filename; }
    void setFilename(QString filename);

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    QString title() const { return m_title; }
    void setTitle(QString title);

    Q_PROPERTY(QStringList filters READ filters WRITE setFilters)
    QStringList filters() const { return m_filters; }
    void setFilters(QStringList filters) { m_filters = filters; }

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();

signals:
    void fileUrlChanged();
    void filenameChanged();
    void titleChanged();
    void accepted();
    void rejected();

protected:
    QPlatformFileDialogHelper* init_helper();

protected:
    QPlatformFileDialogHelper *m_dlgHelper;
    QQuickWindow *m_parentWindow;
    Qt::WindowModality m_modality;
    bool m_visible;
    QSharedPointer<QFileDialogOptions> m_options;

protected Q_SLOTS:
    virtual void accept();
    virtual void reject();

private:
    void setFileUrl(QUrl fileUrl);

    QFileDialog *m_dialog;
    QUrl         m_fileUrl;
    QString      m_filename;
    QString      m_title;
    QStringList  m_filters;

    Q_DISABLE_COPY(FileSaveDialog)
};

#endif // FILESAVEDIALOG_H
