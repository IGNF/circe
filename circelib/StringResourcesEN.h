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
* \file StringResourcesEN.h
* \brief Definition of string resources in english.
*/

#ifndef STRINGRESOURCESEN_H
#define STRINGRESOURCESEN_H
#include "StringResources.h"
namespace circe
{

#define OPTION_FILE_PREFIX "circe_options"
#define OPTION_FILE_PREFIX_W L"circe_options"

#define HELP_HEADER L"Circe Options\n\
*************\n\
\n\
A Circe command line is composed of one or more options.\n\
An option may be input by:\n\
	- a long option name;\n\
	- a long option name with an argument separated by a '=' without spaces;\n\
	- a short option name;\n\
	- a short option name with an argument with or without a separating space.\n\
RECOMMENDATION: Use long option names for long-term stability of your command line.\n\
\n\
short name long name          arg?  description\n\
---------- ---------          ----  -----------";

#define HELP_SYNTAX L"\
Syntax\n\
------\n\
Display help\n\
circe --help\n\
\n\
Get the CRS list\n\
circe --metadataFile=metadataFilePathName --getCRSlist\n\
\n\
Get the definition domain of a CRS\n\
circe --metadataFile=metadataFilePathName --getValidityArea --sourceCRS=SourceGeodeticCRSid.SourceVerticalCRSid\n\
\n\
Run test\n\
circe --runTest --sourcePathname=testFilePathName\n\
\n\
Operate a point\n\
circe --metadataFile=metadataFilePathName --sourceCRS=SourceGeodeticCRSid.SourceVerticalCRSid --sourceFormat=sourceFormatId.sourceLinearUnit.sourceAngularUnit --targetCRS=TargetGeodeticCRSid.TargetVerticalCRSid --targetFormat=targetFormatId.targetLinearUnit.targetAngularUnit coord1 coord2 [coord3]\n\
\n\
Operate a file\n\
circe --metadataFile = metadataFilePathName --sourceCRS = SourceGeodeticCRSid.SourceVerticalCRSid --sourceFormat = sourceFormatId.sourceLinearUnit.sourceAngularUnit --targetCRS = TargetGeodeticCRSid.TargetVerticalCRSid --targetFormat = targetFormatId.targetLinearUnit.targetAngularUnit --sourcePathname = sourcePathname --targetPathname = targetPathname\n\
\n\
Example\n\
-------\n\
The source file datafile.txt contains points expressed in the projected CRS whose code is NTFLAMB3 (NTF Lambert 3). Their format is IEN (id easting northing). \
They will be transform in the geographic CRS whose code is WGS84G in the format ILP (id longitude latitude) and written in the file outfile.txt. \
The codes and other metadata are defined in the file IGNF.xml. \
The command line is:\n\
circe --sourceCRS=NTFLAMB3. --sourceFormat=IEN.METERS. --targetCRS=WGS84G. --targetFormat=ILP.METERS.DEGREES --metadataFile=data/IGNF.xml --sourcePathname=./datafile.txt --targetPathname=./outfile.txt\n\
"

#define PROCESS_TEXT "Calculate"
#define RESTART_TEXT "Restart Circe for the modification to come into effect."

static const std::string ErrorMessageText[EM_COUNT] =
{
    "",
    "uninitialized",
    "Cannot open ",
    " unforeseen",
    " not found",
    " invalid",
    " inconsistent",
    "bad version number",
    "No layer found",
    "Target file extension doesn't match any in the driver list",
    "Creation of output file failed",
    "Layer creation failed",
    "Creating Name field failed",
    "Failed to create feature in layer",
    "Install a PDF viewer",
    "Install a CHM viewer"
};

static const std::string OperationMessageText[OM_COUNT] =
{
    "",
    "The source geodetic CRS required doesn't match any in the Geodetic Metadata Set",
    "The source vertical CRS required doesn't match any in the Geodetic Metadata Set",
    "The target geodetic CRS required doesn't match any in the Geodetic Metadata Set",
    "The target vertical CRS required doesn't match any in the Geodetic Metadata Set",
    "Point is outside source geodetic CRS",
    "Point is outside target geodetic CRS in source geodetic frame",
    "Point is outside target geodetic CRS in target geodetic frame",
    "Transformation required but none found in the Geodetic Metadata Set",
    "Ambiguous vertical transformations with both source and target geodetic frames (maybe remove the vertical CRS information)",
    "The source frames are not initialized",
    "The target frames are not initialized",
    "The geodetic transformation required doesn't match any in the Geodetic Metadata Set",
    "No geodetic transformation found for this point",
    "The vertical transformation required doesn't match any in the Geodetic Metadata Set",
    "No vertical transformation found for this point",
    "Interpolation failed",
    "Source CRS type and measure unit are incompatible.",
    "Point is outside CRS type definition domain.",
	"Source epoch is not valid or source frame is not 4D",
    "Target epoch is not valid or target frame is not 4D",
    "Points processed with success",
    "Source epoch has changed to its predefined value",
    "Target epoch has changed to its predefined value"
};

static const std::string GridFormatInfoText[GF_COUNT] =
{
    "UNDEFINED",
    "Native Circe Ascii grid format. Used for geodetic and vertical transformation grids.",
    "Native Circe binary grid format. Used for geodetic and vertical transformation grids.",
    "Gravsoft 1D ascii grid format whose header is: min latitude, max latitude, min longitude, max longitude, latitude inc, longitude inc.",
    "Surfer 1D ascii grid format.",
    "NTV2 little endian binary grid format.",
    "NTV2 big endian binary grid format.",
    "NTV2 ascii grid format.",
    "ESRI grid format whose header is: ncols, nrows, xllcorner, yllcorner, cellsize, NODATA_value.",
    "Format of the EGM2008 (and others) grid of geoid published by USNGA with sub-grid extraction software (gridget_1min) etc.",
    "BDAlti DIS grid format.",
    "PROJ.4 vertical transformation ascii grid format.",
    "PROJ.4 vertical transformation big endian binary grid format."
};

static const std::string ArgumentValidationText[AV_COUNT] =
{
    "",
    "Unknown error",
	"Bad Metadata filename.",
	"Bad Boundary filename.",
	"Invalid argument",
    "Invalid option or argument",
    "Invalid long option name",
    "Invalid option without argument",
    "Invalid short option name",
    "The vertical CRS id should be defined",
    "The geodetic CRS id should be defined",
    "Invalid geodetic format (must contain XYZ/EN/NE/LP/PL)",
    "Bad metadata file extension (must be txt or xml).",
    "Bad coordinate input.",
    "Bad DMS value",
    "Bad DM value"
};

static const std::wstring OptionsHelpText[OH_COUNT] =
{
   L"",
   L"Displays this help",
   L"Give a short usage message",
   L"Argument: Metadata file pathname",
   L"Argument: Display precision of coordinates in meters",
   L"Argument: Source frame ids in format GeodeticFrameid.VerticalFrameid",
   L"Argument: Target frame ids in format GeodeticFrameid.VerticalFrameid",
   L"Argument: Source CRS ids in format GeodeticCRSid.VerticalCRSid",
   L"Argument: Target CRS ids in format GeodeticCRSid.VerticalCRSid",
   L"Argument: Required transfo ids in format idGeodeticTransfo.idVerticalTransfo",
   L"Follow up transformation values",
   L"Argument: Source format and units in format FormatId.LinearUnit.AngularUnit",
   L"Argument: Target format and units in format FormatId.LinearUnit.AngularUnit",
   L"Argument: Source epoch in decimal years",
   L"Argument: Target epoch in decimal years",
   L"Argument: Source data file pathname",
   L"Argument: Target data file pathname",
   L"Process the tests",
   L"Process multiple CRS",
   L"Silent",
   L"Verbose",
   L"process as vector (for test purpose)",
   L"load as vector",
   L"Get the CRS list",
   L"Get the zone list",
   L"Get the definition domain of a CRS in a specified uom (default: degrees)",
   L"Get the authority of a CRS",
   L"Argument: gridLoading code",
   L"Argument: Log file pathname",
   L"Argument: Output format (default or JSON)",
   L"Argument: Boundary file name without path",
   L"Get a consistent list of UOMs in format id:text,id:text,",
   L"Get a consistent list of data file formats in format id:text,id:text,",
   L"Get a consistent list of point file formats in format id:text,id:text,",
   L"Get a consistent list of geodetic transformations in format id:text,id:text,",
   L"Get a consistent list of vertical transformations in format id:text,id:text,",
   L"Get a consistent list of source geodetic frames in format id:text,id:text,",
   L"Get a consistent list of target geodetic frames in format id:text,id:text,",
   L"Get a consistent list of source geodetic CRS in format id:text,id:text,",
   L"Get a consistent list of target geodetic CRS in format id:text,id:text,",
   L"Get a consistent list of source vertical CRS in format id:text,id:text,",
   L"Get a consistent list of target vertical CRS in format id:text,id:text,",
   L"Print plain DMS, not in decimal form",
   L"Don't print scale factor in PPM",
   L"Field separator"
};

// for user-friendly HMI purposes, all and only angular units between MU_UNDEF and MU_METER
static const std::string MeasureUnitDisplayText[MU_COUNT] =
{
    "",
    "radians",
    "seconds",
    "degrees",
    "DMS",
    "DM",
    "gons",
    "meters",
    "inches",
    "ppm",
    "°/My",
	"mas/y"
};

static const std::string CRStypeDisplayText[CT_COUNT] =
{
    "",
    "geocentric",
    "geographic",
    "projected",
    "vertical"
};

static const std::string DimensionText[SD_COUNT] =
{
    "UNDEFINED",
    "1D",
    "2D",
    "3D",
    "4D"
};

static const std::string DataValidationText[DV_COUNT] =
{
    "",
    "bad format",
    "bad coordinates"
    "Cannot open output.",
    "end of file",
    "comment",
    "loading error",
    "tag not found"
};

//for user-friendly HMI only (unused in API or command-line mode)
static const std::string uiFormatText[UF_COUNT] =
{
    "",
    "XYZ",
    "id XYZ",
    "",
    "longitude latitude",
    "longitude latitude vertical-height",
    "id longitude latitude",
    "id longitude latitude vertical-height",
    "",
    "latitude longitude",
    "latitude longitude vertical-height",
    "id latitude longitude",
    "id latitude longitude vertical-height",
    "",
    "longitude latitude ell.height",
    "id longitude latitude ell.height",
    "",
    "latitude longitude ell.height",
    "id latitude longitude ell.height",
    "",
    "easting northing",
    "easting northing vertical-height",
    "id easting northing",
    "id easting northing vertical-height",
    "",
    "northing easting",
    "northing easting vertical-height",
    "id northing easting",
    "id northing easting vertical-height",
    "",
    "easting northing ell.height",
    "id easting northing ell.height",
    "",
    "northing easting ell.height",
    "id northing easting ell.height",
    "",
#ifdef WITH_GDAL
     "shapefile",
#endif
    "NMEA points",
    "KML points",
    "GPX points"
};

static const std::string InterfaceElementText[IE_END] =
{
    "",
    "Geodetic Reference System",
    "Vertical Reference System",
    "Data",
    "Format",
    "Geodetic Transformation",
    "Vertical Transformation",
    "File",
    "Point",
    "Display",
    "Help",
    "Documentation",
    "About",
    "Clear Coordinates",
    "Horizontal / Vertical Layout",
    "Show Geographic Bounding Boxes",
    "Coordinate epoch",
    "Print more results",
    "Authoritative vertical frames",
    "Data File",
    "Select",
    "Meridian Convergence",
    "Scale error (ppm)",
    "Scale Factor",
    "Vert. Deflection",
    "Convergence unit",
    "Unit",
    "Coordinate type",
    "Input",
    "Output",
    "Plain DMS",
    "Scale Factor full value",
    "Scroll area",
    "Copy command line options",
    "Zone",
    "Tools",
    "Swap source/target",
    "Use Authoritative vertical frames",
    "User manual",
    "Copy Target Point",
    "File Field Separator"
};

static const std::string FieldSeparatorText[FS_END] =
{
    "Space",
    "Comma",
    "Semicolon",
    "Tabulation"
};

static const std::string DataModeText[DM_COUNT] =
{
    "",
    "file",
    "point",
    "list"
};

static const std::string MultipleCRSprocessingText[MP_COUNT] =
{
	"",
	"test",
	"calculation"
};

static const std::string CRSdescriptionElements[CD_COUNT] =
{
	"",
	"CODE",
	"SOURCE",
	"TARGET",
	"GEODETIC FRAME",
	"VERTICAL FRAME",
	"COORDINATES",
	"ELLIPSOID",
	"PROJECTION",
    "UNIT",
    "VERTICAL GRID",
    "EPOCH"
};

}
#endif // STRINGRESOURCESEN_H
