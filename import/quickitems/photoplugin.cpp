#include <qqml.h>
#include <QQmlContext>
#include <QApplication>
#include "photoplugin.h"
#include "ImageManager.h"
#include "ImageProvider.h"
#include "ExceptionHandler.h"

#include "controls/FileSaveDialog.h"
#include "controls/Histogram.h"
#include "controls/ImageMask.h"
#include "controls/Curve.h"
#include "controls/Slipper.h"
#include "controls/StructureElements.h"

#include "categories/ImageTool.h"
#include "categories/Morphology.h"
#include "categories/Segmentation.h"
#include "categories/Description.h"
#include "categories/Filters.h"
#include "categories/Multiresolution.h"
#include "categories/enhancement/Enhancement.h"
#include "categories/enhancement/LogTransformation.h"
#include "categories/enhancement/PowerLawTransformation.h"
#include "categories/enhancement/ContrastStretching.h"
#include "categories/enhancement/HistogramMatching.h"
#include "categories/color/Color.h"
#include "categories/color/IntensityColouring.h"
#include "categories/color/TonalCorrection.h"
#include "categories/color/SaturationCorrection.h"
#include "categories/Genius.h"

void Photo_Plugin::registerTypes(const char *uri) {
    Q_UNUSED(uri);
    qmlRegisterType<ExceptionHandler>(uri, 1, 0, "ExceptionHandler");
    qmlRegisterType<FileSaveDialog>(uri, 1, 0, "FileSaveDialog");
    qmlRegisterType<Histogram>(uri, 1, 0, "Histogram");
    qmlRegisterType<ImageMask>(uri, 1, 0, "ImageMask");
    qmlRegisterType<ImageManager>(uri, 1, 0, "ImageManager");
    qmlRegisterType<Curve>(uri, 1, 0, "Curve");
    qmlRegisterType<Slipper>(uri, 1, 0, "Slipper");

    qmlRegisterType<ImageTool>(uri, 1, 0, "ImageTool");

    qmlRegisterType<Enhancement>(uri, 1, 0, "Enhancement");
    qmlRegisterType<LogTransformation>(uri, 1, 0, "LogTransformation");
    qmlRegisterType<PowerLawTransformation>(uri, 1, 0, "PowerLawTransformation");
    qmlRegisterType<ContrastStretching>(uri, 1, 0, "ContrastStretching");
    qmlRegisterType<HistogramMatching>(uri, 1, 0, "HistogramMatching");

    qmlRegisterType<Filters>(uri, 1, 0, "Filters");
    qmlRegisterType<Multiresolution>(uri, 1, 0, "Multiresolution");

    qmlRegisterType<Color>(uri, 1, 0, "Color");
    qmlRegisterType<IntensityColouring>(uri, 1, 0, "IntensityColouring");
    qmlRegisterType<TonalCorrection>(uri, 1, 0, "TonalCorrection");
    qmlRegisterType<SaturationCorrection>(uri, 1, 0, "SaturationCorrection");

    qmlRegisterType<StructureElements>(uri, 1, 0, "StructureElements");
    qmlRegisterType<Morphology>(uri, 1, 0, "Morphology");
    qmlRegisterType<Segmentation>(uri, 1, 0, "Segmentation");
    qmlRegisterType<Description>(uri, 1, 0, "Description");
    qmlRegisterType<Genius>(uri, 1, 0, "Genius");
}

void Photo_Plugin::initializeEngine(QQmlEngine *engine, const char *uri) {
    Q_UNUSED(uri);
    ImageProvider *pImageProvider = new ImageProvider;
    engine->addImageProvider("polka", pImageProvider);
}
