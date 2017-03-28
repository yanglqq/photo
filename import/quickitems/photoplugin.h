#ifndef PHOTO_PLUGIN_H
#define PHOTO_PLUGIN_H

#include <QQmlExtensionPlugin>
#include "ImageProvider.h"
#include "ExceptionHandler.h"

class Photo_Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void initializeEngine(QQmlEngine *engine, const char *uri);
    void registerTypes(const char *uri);
private:
    ExceptionHandler exceptionHandler;
};

#endif // PHOTO_PLUGIN_H
