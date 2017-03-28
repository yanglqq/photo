#include "ExceptionHandler.h"

static ExceptionHandler *pExceptionHandler;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static char message[1024];
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        sprintf(message, "Debug: %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        if (pExceptionHandler->ignoreWarning())
            return;
        sprintf(message, "Warning: %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        sprintf(message, "Critical: %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        sprintf(message, "Fatal: %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
    }
    fprintf(stderr, "%s\n", message);
    emit pExceptionHandler->exception(QString(message));
}

ExceptionHandler::ExceptionHandler(QQuickItem *parent)
    : QQuickItem(parent) {
    pExceptionHandler = this;
    qInstallMessageHandler(messageHandler);
}
