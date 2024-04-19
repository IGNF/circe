QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = circeWidget
TEMPLATE = app

DEFINES += UI_LANGUAGE_FRENCH

unix:DEFINES += UNIX
win32:DEFINES += WIN32
#unix:DEFINES += WITH_COMPLEX_FORMATS
#win32:DEFINES += WITH_COMPLEX_FORMATS
#unix:CONFIG += WITH_COMPLEX_FORMATS
#win32:CONFIG += WITH_COMPLEX_FORMATS

WITH_COMPLEX_FORMATS {
unix:DEFINES += WITH_GDAL
Release:win32:DEFINES += WITH_GDAL
unix:CONFIG += QMAKE_WITH_GDAL
Release:win32:CONFIG += QMAKE_WITH_GDAL
}

#INCLUDEPATH += ../tinyxml2-master

QMAKE_WITH_GDAL {
unix:INCLUDEPATH += /usr/include/gdal
unix:LIBS += "/usr/lib/libgdal.so"
unix:SOURCES += ../../circelib/gdal/circe_ogr.cpp
unix:HEADERS += ../../circelib/gdal/circe_ogr.h
win32:INCLUDEPATH += ../../circelib/gdal/include
win32:LIBS += ../../circelib/gdal/gdal.lib
win32:SOURCES += ../../circelib/gdal/circe_ogr.cpp
win32:HEADERS += ../../circelib/gdal/circe_ogr.h
}

RESOURCES = Images.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../circelib/catoperation.cpp \
    ../../circelib/circe.cpp \
    ../../circelib/circeboundedobject.cpp \
    ../../circelib/circeobject.cpp \
    ../../circelib/conversion.cpp \
    ../../circelib/conversioncore.cpp \
    ../../circelib/crs.cpp \
    ../../circelib/ellipsoid.cpp \
    ../../circelib/files.cpp \
    ../../circelib/geodeticreferenceframe.cpp \
    ../../circelib/geodeticset.cpp \
    ../../circelib/meridian.cpp \
    ../../circelib/metadata.cpp \
    ../../circelib/operation.cpp \
    ../../circelib/parser.cpp \
    ../../circelib/pt4d.cpp \
    ../../circelib/referenceframe.cpp \
    ../../circelib/stringtools.cpp \
    ../../circelib/transformation.cpp \
    ../../circelib/units.cpp \
    ../../circelib/verticalreferenceframe.cpp \
    ../../circelib/xmlparser.cpp \
    ../../circelib/tinyxml2/tinyxml2.cpp \
    messageherald.cpp \
    ../../circelib/compoundoperation.cpp

HEADERS  += mainwindow.h \
    ../../circelib/_circelib_version_.h \
    ../../circelib/catoperation.h \
    ../../circelib/circe.h \
    ../../circelib/circeboundedobject.h \
    ../../circelib/circeobject.h \
    ../../circelib/conversion.h \
    ../../circelib/conversioncore.h \
    ../../circelib/crs.h \
    ../../circelib/ellipsoid.h \
    ../../circelib/files.h \
    ../../circelib/geodeticreferenceframe.h \
    ../../circelib/geodeticset.h \
    ../../circelib/meridian.h \
    ../../circelib/metadata.h \
    ../../circelib/operation.h \
    ../../circelib/parser.h \
    ../../circelib/pt4d.h \
    ../../circelib/referenceframe.h \
    ../../circelib/StringResources.h \
    ../../circelib/StringResourcesEN.h \
    ../../circelib/StringResourcesFR.h \
    ../../circelib/stringtools.h \
    ../../circelib/transformation.h \
    ../../circelib/units.h \
    ../../circelib/verticalreferenceframe.h \
    ../../circelib/xmlparser.h \
    ../../circelib/tinyxml2/tinyxml2.h \
    _circe_gui_version_.h \
    messageherald.h \
    ../../circelib/messagebearer.h \
    ../../circelib/compoundoperation.h

FORMS    += mainwindow.ui
