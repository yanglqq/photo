TARGET = photo
QT += qml quick

# Avoid going to debug/release subdirectory
# so that our application will see the
# import path for the photo module.
win32: DESTDIR = ./

SOURCES += \
    main.cpp


RESOURCES += \
    photo.qrc
    
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += . ../include

win32: LIBS += -L../build/bin

