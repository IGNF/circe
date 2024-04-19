QT       += core
QT       -= gui

TARGET = circeFR
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += _UI_LANGUAGE_FRENCH

unix:DEFINES += UNIX
win32:DEFINES += WIN32
#win32:DEFINES += WITH_COMPLEX_FORMATS
#unix:DEFINES += WITH_COMPLEX_FORMATS
#win32:CONFIG += WITH_COMPLEX_FORMATS
#unix:CONFIG += WITH_COMPLEX_FORMATS

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

SOURCES += \
    main.cpp \
    ../../circelib/conversion.cpp \
    ../../circelib/transformation.cpp \
    ../../circelib/metadata.cpp \
    ../../circelib/circe.cpp \
    ../../circelib/operation.cpp \
    ../../circelib/circeboundedobject.cpp \
    ../../circelib/circeobject.cpp \
    ../../circelib/meridian.cpp \
    ../../circelib/ellipsoid.cpp \
    ../../circelib/geodeticreferenceframe.cpp \
    ../../circelib/verticalreferenceframe.cpp \
    ../../circelib/geodeticset.cpp \
    ../../circelib/crs.cpp \
    ../../circelib/referenceframe.cpp \
    ../../circelib/catoperation.cpp \
    ../../circelib/parser.cpp \
    ../../circelib/files.cpp \
    ../../circelib/pt4d.cpp \
    ../../circelib/xmlparser.cpp \
    ../../circelib/conversioncore.cpp \
    ../../circelib/stringtools.cpp \
    ../../circelib/units.cpp \
    ../../circelib/tinyxml2/tinyxml2.cpp \
    ../../circelib/compoundoperation.cpp

HEADERS += \
    ../../circelib/_circelib_version_.h \
    ../../circelib/conversion.h \
    ../../circelib/transformation.h \
    ../../circelib/metadata.h \
    ../../circelib/circe.h \
    ../../circelib/operation.h \
    ../../circelib/circeboundedobject.h \
    ../../circelib/circeobject.h \
    ../../circelib/meridian.h \
    ../../circelib/ellipsoid.h \
    ../../circelib/geodeticreferenceframe.h \
    ../../circelib/verticalreferenceframe.h \
    ../../circelib/geodeticset.h \
    ../../circelib/crs.h \
    ../../circelib/referenceframe.h \
    ../../circelib/catoperation.h \
    ../../circelib/parser.h \
    ../../circelib/StringResources.h \
    ../../circelib/StringResourcesEN.h \
    ../../circelib/StringResourcesFR.h \
    ../../circelib/files.h \
    ../../circelib/pt4d.h \
    ../../circelib/xmlparser.h \
    ../../circelib/conversioncore.h \
    ../../circelib/stringtools.h \
    ../../circelib/units.h \
    ../../circelib/tinyxml2/tinyxml2.h \
    ../../circelib/messagebearer.h \
    ../../circelib/compoundoperation.h
