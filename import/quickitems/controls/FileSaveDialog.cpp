#include "FileSaveDialog.h"

#include <QQuickWindow>
#include <QFileInfo>

FileSaveDialog::FileSaveDialog(QQuickItem *parent) :
    QQuickItem(parent)
  , m_modality(Qt::WindowModal)
  , m_options(QSharedPointer<QFileDialogOptions>(new QFileDialogOptions())) {
    m_dlgHelper = init_helper();

    connect(m_dlgHelper, SIGNAL(accept()), this, SLOT(accept()));
    connect(m_dlgHelper, SIGNAL(reject()), this, SLOT(reject()));
}

FileSaveDialog::~FileSaveDialog() {
    if (m_dlgHelper)
        m_dlgHelper->hide();
    delete m_dlgHelper;
}

void FileSaveDialog::setFileUrl(QUrl fileUrl) {
    m_fileUrl = fileUrl;
    emit fileUrlChanged();
}

void FileSaveDialog::setFilename(QString filename) {
    QFileInfo fileInfo(filename);
    m_filename = fileInfo.fileName();
    emit filenameChanged();
}

void FileSaveDialog::setTitle(QString title) {
    m_title = title;
    emit titleChanged();
}

QPlatformFileDialogHelper* FileSaveDialog::init_helper() {
    m_dlgHelper = static_cast<QPlatformFileDialogHelper*>(QGuiApplicationPrivate::platformTheme()->createPlatformDialogHelper(QPlatformTheme::FileDialog));
    if (!m_dlgHelper)
        return NULL;
    return m_dlgHelper;
}

void FileSaveDialog::open() {
    QQuickItem *parent = this->parentItem();
    Q_ASSERT(parent);

    QQuickWindow *window = parent->window();
    Q_ASSERT(window);

    m_parentWindow = window;

    m_options->setFileMode(QFileDialogOptions::AnyFile);
    m_options->setAcceptMode(QFileDialogOptions::AcceptSave);
    m_options->setWindowTitle(m_title);
    m_options->setNameFilters(m_filters);
#ifdef Q_OS_OSX
    QString initialSelection = QFileInfo(QDir::homePath(), m_filename).absoluteFilePath();
    m_options->setInitiallySelectedFiles(QList<QUrl>() << QUrl::fromLocalFile(initialSelection));
#endif
#ifdef Q_OS_WIN
    m_options->setInitiallySelectedFiles(QList<QUrl>() << QUrl::fromLocalFile(m_filename));
#endif
#ifdef Q_OS_LINUX
    m_dlgHelper->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
#endif

    m_dlgHelper->setOptions(m_options);
    m_dlgHelper->setFilter(); // applyOptions();

    Qt::WindowFlags flags = Qt::Dialog;
    if (!m_title.isEmpty()) flags |= Qt::WindowTitleHint;

    m_visible = m_dlgHelper->show(flags, m_modality, m_parentWindow);
}

void FileSaveDialog::close() {
    m_dlgHelper->hide();
    m_visible = false;
}

void FileSaveDialog::accept() {
    m_dlgHelper->hide();
    QList<QUrl> selectedUrls = m_dlgHelper->selectedFiles();
    if (selectedUrls.count())
        setFileUrl(selectedUrls.at(0));
    emit accepted();
}

void FileSaveDialog::reject() {
    m_dlgHelper->hide();
    emit rejected();
}
