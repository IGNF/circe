#ifndef CIRCE_OGR
#define CIRCE_OGR

#include "../../circelib/parser.h"
#include "../../circelib/catoperation.h"
#ifdef UI_LANGUAGE_FRENCH
#include "../../circelib/StringResourcesFR.h"
#else
#include "../../circelib/StringResourcesEN.h"
#endif

#include "ogrsf_frmts.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_core.h"

namespace circe
{
static const std::string Extensions[][2] =
{
    { "shp"    , "ESRI Shapefile" },
    { "dbf"    , "ESRI Shapefile" },
    { "sqlite" , "SQLite" },
    { "db"     , "SQLite" },
    { "mif"    , "MapInfo File" },
    { "tab"    , "MapInfo File" },
    { "s57"    , "S57" },
    { "bna"    , "BNA" },
    { "csv"    , "CSV" },
    { "gml"    , "GML" },
    { "kml"    , "LIBKML" },
    { "kmz"    , "LIBKML" },
    { "json"   , "GeoJSON" },
    { "geojson", "GeoJSON" },
    { "dxf"    , "DXF" },
    { "gdb"    , "FileGDB" },
    { "pix"    , "PCIDSK" },
    { "sql"    , "PGDump" },
    { "gtm"    , "GPSTrackMaker" },
    { "gmt"    , "GMT" },
    { "pdf"    , "PDF" },
    { "", "" }
};

void processPoint(const CatOperation& CO, OGRPoint* pointIn, OGRPoint* pointOut);
void processLineString(const circe::CatOperation &CO, OGRLineString* lineStringIn, OGRLineString* lineStringOut);
void processPolygon(const CatOperation& CO, OGRPolygon* polygonIn, OGRPolygon* polygonOut);
void processMultiPolygon(const circe::CatOperation &CO, OGRMultiPolygon* polygonMultiIn, OGRMultiPolygon* polygonMultiOut);
void processOGRdataSource(circeOptions circopt, GeodeticSet* GS);
} //namespace circe

#endif // CIRCE_OGR

