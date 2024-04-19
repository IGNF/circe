/***********************************************************************

This file is part of the Circé project source files.

Circé is a computer program whose purpose is the transformation of
4D+1 point coordinates between Coordinate Reference Systems.

Homepage:

   https://github.com/IGNF/circe

Copyright:

    Institut National de l'Information Géographique et Forestière (2018)

Author:

    Samuel Branchu


    Circé is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Circé is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Circé.  If not, see
    <https://www.gnu.org/licenses/gpl-3.0.html#license-text>.

***********************************************************************/
/**
* \file parser.h
* \brief function arguments  =>  [parser]  =>  circe options.
*/

#ifndef PARSER_H
#define PARSER_H
#include "_circelib_version_.h"
#include "pt4d.h"
#include "units.h"
#include "StringResources.h"
#include <string>
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif

namespace circe
{

static const std::string circe_program_version = "circe v5";
static const std::string CIRCE_ERROR = "CIRCE ERROR";
static const std::string CIRCE_NAME = "CIRCE";
static const std::string CIRCE_ALT_NAME = "TT";

//static char doc[] = "Circe v5";
//static char args_doc[] = "";

/**
* \brief Option structure instance dedicated to the options available to the users following some of the POSIX recommendations.
*/
static const struct
{
  const char *name;
  int key;
  const char *arg;
  int flags;
  OPTIONS_HELP doc;
  int group;
} circeArgumentDefinitions[] =
{
    {"usage",	              -1,  0,        0,  OH_USAGE, 1},
    {"getValidityArea",       'a', 0,        0,  OH_GET_VALIDITY_AREA, 5},
    {"boundaryFile",          'b', "STRING", 0,  OH_BOUNDARY_FILE, 2},
    {"processAsVector",       'c', 0,        0,  OH_PROCESS_VECTOR, 2},
    {"loadAsVector",          'C', 0,        0,  OH_LOAD_VECTOR, 2},
    {"plainDMS",              'd', 0,        0,  OH_PLAIN_DMS, 2},
    {"noPrintSppm",           'D', 0,        0,  OH_NOPRINTSPPMM, 2},
    {"sourceEpoch",           'e', "FLOAT",  0,  OH_SOURCE_EPOCH, 2},
    {"targetEpoch",           'E', "FLOAT",  0,  OH_TARGET_EPOCH, 2},
    {"sourceFormat",          'f', "STRING", 0,  OH_SOURCE_FORMAT, 2},
    {"targetFormat",          'F', "STRING", 0,  OH_TARGET_FORMAT, 2},
    {"gridLoading",           'g', "STRING", 0,  OH_GRID_LOADING, 4},
    {"help",	              'h', 0,    	 0,  OH_HELP, 0},
    {"sourceFrame",           'i', "STRING", 0,  OH_SOURCE_FRAME, 2},
    {"targetFrame",           'I', "STRING", 0,  OH_TARGET_FRAME, 2},
    {"getCRSlist",            'l', 0,        0,  OH_GET_CRS_LIST, 5},
    {"getZoneList",           'L', 0,        0,  OH_GET_ZONE_LIST, 5},
    {"metadataFile",          'm', "STRING", 0,  OH_METADATAFILE, 2},
    {"displayPrecision",      'n', "FLOAT",  0,  OH_DISPLAYPRECISION, 2},
    {"separator",             'N', "STRING", 0,  OH_SEPARATOR, 4},
    {"logPathname",           'o', "STRING", 0,  OH_LOGPATHNAME, 2},
    {"outputFormat",          'O', "STRING", 0,  OH_OUTPUTFORMAT, 2},
    {"sourcePathname",        'p', "STRING", 0,  OH_SOURCE_PATHNAME, 2},
    {"targetPathname",        'P', "STRING", 0,  OH_TARGET_PATHNAME, 2},
    {"quiet",                 'q', 0,        0,  OH_SILENT, 1},
    {"verbose",               'Q', 0,        0,  OH_VERBOSE, 1},
    {"transfoRequired",       'r', "STRING", 0,  OH_TRANSFO_REQUIRED, 2 },
    {"followUpTransfos",      'R', 0       , 0,  OH_FOLLOW_UP_TRANSFOS, 2 },
    {"sourceCRS",             's', "STRING", 0,  OH_SOURCE_CRS, 2},
    {"targetCRS",             'S', "STRING", 0,  OH_TARGET_CRS, 2},
    {"runTest",               't', 0,        0,  OH_PROCESS_TESTS, 3},
    {"multipleCRS",           'T', 0,        0,  OH_MULTIPLE_CRS, 3 },
    {"getAuthority",          'u', 0,        0,  OH_GET_AUTHORITY, 5},
    {"getUom",                'U', 0,        0,  OH_GET_UOM, 5},
    {"getDataFileFormat",     'v', 0,        0,  OH_GET_DATAFILEFORMAT, 5},
    {"getDataPointFormat",    'V', 0,        0,  OH_GET_DATAPOINTFORMAT, 5},
    {"getGeodeticTransfo",    'w', 0,        0,  OH_GET_GEODETICTRANSFO, 5},
    {"getVerticalTransfo",    'W', 0,        0,  OH_GET_VERTICALTRANSFO, 5},
    {"getSourceGeodeticFrame",'x', 0,        0,  OH_GET_SOURCEGEODETICFRAME, 5},
    {"getTargetGeodeticFrame",'X', 0,        0,  OH_GET_TARGETGEODETICFRAME, 5},
    {"getSourceGeodeticCRS",  'y', 0,        0,  OH_GET_SOURCEGEODETICCRS, 5},
    {"getTargetGeodeticCRS",  'Y', 0,        0,  OH_GET_TARGETGEODETICCRS, 5},
    {"getSourceVerticalCRS",  'z', 0,        0,  OH_GET_SOURCEVERTICALCRS, 5},
    {"getTargetVerticalCRS",  'Z', 0,        0,  OH_GET_TARGETVERTICALCRS, 5},
};

/**
* \brief structure without any important name
*/
struct swain
{
    std::ifstream *fi;
    std::ofstream *fo;
};

/**
* \brief geodeticOptions structure used by circeOptions.
*/
struct geodeticOptions
{
    std::string geodeticFrameId, geodeticCRSid, verticalFrameId, verticalCRSid;
    std::string usesForTransformation, formatId, linearUnit, angularUnit;
    std::string pathname;
    double epoch;
    circe::FILE_FORMAT subformatId;
    bool useGeodeticCRS, useVerticalCRS, verticalFrameAuthoritative;
    circe::CRS_TYPE geodeticCRStype;
};

/**
* \brief Full option structure, some of them being available to the users.
*/
struct circeOptions
{
    std::string appTitle, appCopyright, appVersion, appDescription, appHelp, appStyle, appUserManual;
    geodeticOptions sourceOptions;
    geodeticOptions targetOptions;
    std::string idGeodeticTransfoRequired, idVerticalTransfoRequired;
    circe::DATA_MODE dataMode;
    std::string metadataFile, gridLoading, logPathname, optionFile, imageFilename, logoFilename, iconFilename, boundaryFile;
    int displayHelp, runTest, multipleCRS, quiet, verbose, extraOut, processAsVector, loadAsVector;
    int getCRSlist, getValidityArea, getAuthority, followUpTransfos;
    int getSourceGeodeticFrame, getSourceGeodeticCRS, getSourceVerticalCRS;
    int getTargetGeodeticFrame, getTargetGeodeticCRS, getTargetVerticalCRS;
    int getDataFileFormat, getDataPointFormat, getUom;
    int getGeodeticTransfo, getVerticalTransfo;
    int getAllGeodeticSubsetLists, getZoneList;
    circe::pt4d Pt4d;
    bool printCS, displayBoundingBox, noPrintSppm, noUseVerticalFrameAuthoritative, scrollArea, plainDMS;
	double displayPrecision;
    std::string separator, outputFormat, operationType;
    std::string menuTitle_zone, menuTitle_display, menuTitle_tool, menuTitle_help;
};


bool hasEllHeight(std::string frmt);
bool hasVertical(std::string frmt);
bool hasIdentifier(std::string frmt);
bool changeVerticalComponent(std::string *frmt, char char_in, const char *char_out);
bool hasConvergence(std::string frmt);
void initDefOptions(circeOptions *circopt);
void initTarget(circeOptions *circopt1, circeOptions circopt2);
void copyOptions(circeOptions *circopt2, const circeOptions &circopt1);
void swapGeodeticOptions(geodeticOptions *go1, geodeticOptions *go2);
void copyGeodeticOptions(const geodeticOptions &go1, geodeticOptions *go2);
std::string displayGeodeticOptions(const geodeticOptions& go);
std::string displayGeodeticOptionsJSON(const geodeticOptions& go);
std::string displayOptions(const circeOptions &circopt);
std::string displayOptionsJSON(const circeOptions &circopt);
std::string displayInvalidArgument(char *argv[], int iargv, ARGUMENT_VALIDATION av);
std::string displayCRS(circeOptions circopt);
std::string displayHelp();
void scrutinizeFormat(circeOptions *circopt, std::string what);
void parseArgArray(circe::circeOptions *circopt, int argc, char *argv[], int *iargv);
void parseArgString(std::string definitionLine, circeOptions *circopt);
void parseOptions(int key, char *arg, circeOptions *circopt);
void parsePt4dCoordinates(int argc, char **argv, circe::circeOptions *circopt, int *iargv, int *idt);
void scrutinizeOptions(circeOptions *circopt);
std::string rmExtraFormat(std::string frmt);
std::string readXMLOptionFile(circeOptions* circopt, std::string circeOptionFile) /*throw(std::string)*/;
std::string writeXMLOptionFile(const circeOptions& circopt) /*throw(std::string)*/;
std::string writeCommandLine(const circeOptions& circopt) /*throw(std::string)*/;
std::string displayOutput(const circeOptions& circopt, const std::string& str, const std::string &brckt1, const std::string &brckt2, bool bWriteMessage);
char getSeparator(std::string sep);

} //namespace circe

#endif // PARSER_H
