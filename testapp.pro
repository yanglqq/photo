TARGET = test
QT += qml quick

# Avoid going to debug/release subdirectory
# so that our application will see the
# import path for the photo module.
win32: DESTDIR = ./

SOURCES += \
    testmain.cpp


RESOURCES += \
    photo.qrc \
    test.qrc
    
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

OTHER_FILES =

INCLUDEPATH += . ../include

win32: LIBS += -L../build/bin
