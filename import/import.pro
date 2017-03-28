TEMPLATE = lib
TARGET = polkaplugin
QT += qml quick widgets core-private gui-private
CONFIG += qt plugin

DESTDIR = ../Polka
TARGET = $$qtLibraryTarget($$TARGET)

INCLUDEPATH += . ../../include

target.path=$$DESTPATH
qmldir.files=$$PWD/qmldir
qmldir.path=$$DESTPATH
INSTALLS += target qmldir

OTHER_FILES += qmldir

# Copy the qmldir file to the same folder as the plugin binary
QMAKE_POST_LINK += $$QMAKE_COPY $$replace($$list($$quote($$PWD/qmldir) $$DESTDIR), /, $$QMAKE_DIR_SEP)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_core300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_core300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_highgui300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_highgui300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_imgcodecs300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_imgcodecs300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_imgproc300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_imgproc300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_ml300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_ml300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_objdetect300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12/lib/ -lopencv_objdetect300d


macx: LIBS += -stdlib=libc++
macx: LIBS += -L/usr/local/lib/ -lopencv_core
macx: LIBS += -L/usr/local/lib/ -lopencv_imgcodecs
macx: LIBS += -L/usr/local/lib/ -lopencv_imgproc
macx: LIBS += -L/usr/local/lib/ -lopencv_ml
macx: LIBS += -L/usr/local/lib/ -lopencv_objdetect

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7

INCLUDEPATH += $$PWD/../../thirdparty/opencv-3.0.0/build/include
DEPENDPATH += $$PWD/../../thirdparty/opencv-3.0.0/build/x64/vc12

INCLUDEPATH += $$PWD/../../polka

HEADERS += \
    quickitems/categories/color/Color.h \
    quickitems/categories/color/IntensityColouring.h \
    quickitems/categories/color/SaturationCorrection.h \
    quickitems/categories/color/TonalCorrection.h \
    quickitems/categories/enhancement/ContrastStretching.h \
    quickitems/categories/enhancement/Enhancement.h \
    quickitems/categories/enhancement/HistogramMatching.h \
    quickitems/categories/enhancement/LogTransformation.h \
    quickitems/categories/enhancement/PowerLawTransformation.h \
    quickitems/categories/Description.h \
    quickitems/categories/Filters.h \
    quickitems/categories/ImageTool.h \
    quickitems/categories/Morphology.h \
    quickitems/categories/Multiresolution.h \
    quickitems/categories/Segmentation.h \
    quickitems/categories/Genius.h \
    quickitems/controls/Curve.h \
    quickitems/controls/CurveFunc.h \
    quickitems/controls/FileSaveDialog.h \
    quickitems/controls/Histogram.h \
    quickitems/controls/ImageCurve.h \
    quickitems/controls/ImageMask.h \
    quickitems/controls/Slipper.h \
    quickitems/controls/StructureElements.h \
    quickitems/ExceptionHandler.h \
    quickitems/ImageManager.h \
    quickitems/ImageProvider.h \
    quickitems/photoplugin.h \
    quickitems/ImageProccessor.h \    
    image.h

SOURCES += \
    quickitems/categories/color/Color.cpp \
    quickitems/categories/color/IntensityColouring.cpp \
    quickitems/categories/enhancement/Enhancement.cpp \
    quickitems/categories/Description.cpp \
    quickitems/categories/Filters.cpp \
    quickitems/categories/ImageTool.cpp \
    quickitems/categories/Morphology.cpp \
    quickitems/categories/Multiresolution.cpp \
    quickitems/categories/Segmentation.cpp \
    quickitems/categories/Genius.cpp \
    quickitems/controls/Curve.cpp \
    quickitems/controls/CurveFunc.cpp \
    quickitems/controls/FileSaveDialog.cpp \
    quickitems/controls/Histogram.cpp \
    quickitems/controls/ImageMask.cpp \
    quickitems/controls/Slipper.cpp \
    quickitems/controls/StructureElements.cpp \
    quickitems/ExceptionHandler.cpp \
    quickitems/ImageManager.cpp \
    quickitems/ImageProvider.cpp \
    quickitems/photoplugin.cpp \
    quickitems/ImageProccessor.cpp \
    image.cpp

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
