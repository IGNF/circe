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
* \file StringResources.h
* \brief Definition of codes and string resources.
*/
#ifndef STRINGRESOURCES_H
#define STRINGRESOURCES_H
#include <string>
namespace circe
{

/**\brief Codes for error messages.*/
enum ERROR_MESSAGE
{
    EM_OK,
    EM_UNINITIALIZED,
    EM_CANNOT_OPEN,
    EM_UNFORESEEN,
    EM_NOT_FOUND,
    EM_NOT_VALID,
    EM_INCONSISTENT,
    EM_BAD_VERSION_NUMBER,
    EM_NO_LAYER_FOUND,
    EM_BAD_TARGET_FILE_EXTENSION,
    EM_TARGET_FILE_CREATION_FAILED,
    EM_LAYER_CREATION_FAILED,
    EM_NAME_FIELD_CREATION_FAILED,
    EM_FEATURE_CREATION_FAILED,
    EM_INSTALL_PDF_VIEWER,
    EM_INSTALL_CHM_VIEWER,
    EM_COUNT
};

/**\brief Codes for operation messages.*/
enum OPERATION_MESSAGE
{
    OM_OK,
    OM_SOURCE_GEODETIC_CRS_NOT_FOUND,
    OM_SOURCE_VERTICAL_CRS_NOT_FOUND,
    OM_TARGET_GEODETIC_CRS_NOT_FOUND,
    OM_TARGET_VERTICAL_CRS_NOT_FOUND,
    OM_POINT_OUTSIDE_SOURCE_GEODETIC_CRS,
    OM_POINT_OUTSIDE_TARGET_GEODETIC_CRS_IN_SOURCE_GEODETIC_FRAME,
    OM_POINT_OUTSIDE_TARGET_GEODETIC_CRS_IN_TARGET_GEODETIC_FRAME,
    OM_TRANSFORMATION_REQUIRED_NONE_FOUND,
    OM_AMBIGUOUS_VERTICAL_TRANSFORMATION,
    OM_SOURCE_FRAMES_NOT_INITIALIZED,
    OM_TARGET_FRAMES_NOT_INITIALIZED,
    OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS,
    OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THIS_POINT,
    OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THESE_CRS,
    OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THIS_POINT,
    OM_INTERPOLATION_FAILED,
    OM_CRS_TYPE_MEASURE_UNIT_INCOMPATIBLE,
    OM_POINT_OUTSIDE_CRS_TYPE_DOMAIN,
	OM_SOURCE_EPOCH_FRAME_NOT_VALID,
	OM_TARGET_EPOCH_FRAME_NOT_VALID,
    OM_NB_PROCESSED_POINTS,
    OM_SOURCE_EPOCH_CHANGED,
    OM_TARGET_EPOCH_CHANGED,
	OM_COUNT
};

/**\brief Codes for grid format info.*/
enum GRID_FORMAT_INFO
{
    GF_UNDEF,
    GF_SGN,
    GF_SGN_BIN,
    GF_GRAVSOFT,
    GF_SURFER,
    GF_NTV2_BIN_L,
    GF_NTV2_BIN_B,
    GF_NTV2_ASC,
    GF_ESRI,
    GF_EGM,
    GF_DIS,
    GF_GTX_ASC,
    GF_GTX_BIN,
    GF_COUNT
};

/**\brief Codes for loading type.
    The loading type points out the way a grid file is read to interpolate values.
    - LD_BINARY: The binary file is opened for further interpolation.
               This may be the preferred way in those cases:
               - For a single point or a little number of points, it is very fast
                    and leads to instant calculation.
               - So it should be used to process the tests (--runTest)
               - If the grid is too big to be loaded into memory.
    - LD_ARRAY : The text or binary file is loaded into memory for further interpolation.
               It is the preferred way if there are a lot of points to be interpolated.
    - LD_HEADER: read metadata only (header and xml).
    - LD_XML   : read metadata only (xml only).
 */
enum LOADING_TYPE
{
    LD_UNDEF,
    LD_BINARY,
    LD_ARRAY,
    LD_HEADER,
    LD_XML,
    LD_COUNT
};

/**\brief Codes for encoding type.*/
enum ENCODING_TYPE
{
    ET_UNDEF,
    ET_ASCII,
    ET_LITTLE_ENDIAN,
    ET_BIG_ENDIAN,
    ET_COUNT
};

/**\brief Codes for argument validation.*/
enum ARGUMENT_VALIDATION
{
    AV_OK,
    AV_ERR_UNKNOWN,
	AV_BAD_METADATAFILENAME,
	AV_BAD_BOUNDARYFILENAME,
	AV_INVALID_ARGUMENT,
    AV_INVALID_OPTION_ARGUMENT,
    AV_INVALID_LONG_OPTION_NAME,
    AV_INVALID_OPTION_WITHOUT_ARGUMENT,
    AV_INVALID_SHORT_OPTION_NAME,
    AV_VERTICAL_CRS_SHOULD_NOT_BE_UNDEFINED,
    AV_GEODETIC_CRS_SHOULD_NOT_BE_UNDEFINED,
    AV_INVALID_GEODETIC_FORMAT,
    AV_BAD_METADATAFILE_EXTENSION,
    AV_BAD_COORDINATE_INPUT,
    AV_BAD_DMS_VALUE,
    AV_BAD_DM_VALUE,
    AV_COUNT
};

/**\brief Codes for option help.*/
enum OPTIONS_HELP
{
    OH_UNDEF,
    OH_HELP,
    OH_USAGE,
    OH_METADATAFILE,
	OH_DISPLAYPRECISION,
    OH_SOURCE_FRAME,
    OH_TARGET_FRAME,
    OH_SOURCE_CRS,
    OH_TARGET_CRS,
    OH_TRANSFO_REQUIRED,
	OH_FOLLOW_UP_TRANSFOS,
    OH_SOURCE_FORMAT,
    OH_TARGET_FORMAT,
    OH_SOURCE_EPOCH,
    OH_TARGET_EPOCH,
    OH_SOURCE_PATHNAME,
    OH_TARGET_PATHNAME,
    OH_PROCESS_TESTS,
	OH_MULTIPLE_CRS,
    OH_SILENT,
    OH_VERBOSE,
    OH_PROCESS_VECTOR,
    OH_LOAD_VECTOR,
    OH_GET_CRS_LIST,
    OH_GET_ZONE_LIST,
    OH_GET_VALIDITY_AREA,
    OH_GET_AUTHORITY,
    OH_GRID_LOADING,
    OH_LOGPATHNAME,
    OH_OUTPUTFORMAT,
	OH_BOUNDARY_FILE,
    OH_GET_UOM,
    OH_GET_DATAFILEFORMAT,
    OH_GET_DATAPOINTFORMAT,
    OH_GET_GEODETICTRANSFO,
    OH_GET_VERTICALTRANSFO,
    OH_GET_SOURCEGEODETICFRAME,
    OH_GET_TARGETGEODETICFRAME,
    OH_GET_SOURCEGEODETICCRS,
    OH_GET_TARGETGEODETICCRS,
    OH_GET_SOURCEVERTICALCRS,
    OH_GET_TARGETVERTICALCRS,
    OH_PLAIN_DMS,
    OH_NOPRINTSPPMM,
    OH_SEPARATOR,
    OH_COUNT
};

/**\brief Codes for measure unit.*/
// for user-friendly HMI purposes, all and only angular units between MU_UNDEF and MU_METER
enum MEASURE_UNIT
{
    MU_UNDEF,
    MU_RADIAN,
    MU_SECOND,
    MU_DEGREE,
    MU_DMS,
    MU_DM,
    MU_GON,
    MU_METER,
    MU_INCH,
	MU_PPM,
	MU_DEGPERMY,
	MU_MASPERY,
    MU_COUNT
};

/**\brief  Codes for coordinate type.*/
enum CRS_TYPE
{
    CT_UNDEF,
    CT_GEOCENTRIC,
    CT_GEOGRAPHIC,
    CT_PROJECTED,
    CT_VERTICAL,
    CT_COUNT
};

/**\brief Codes for system dimension.*/
enum SYSTEM_DIMENSION
{
    SD_UNDEF,
    SD_1D,
    SD_2D,
    SD_3D,
    SD_4D,
    SD_COUNT
};


/**\brief Codes for data validation.*/
enum DATA_VALIDATION
{
    DV_OK,
    DV_BAD_FORMAT,
    DV_BAD_COORD,
    DV_CANNOT_OPEN_OUTPUT,
    DV_EOF,
    DV_COMMENT,
    DV_LOADING_ERROR,
    DV_TAG_NOT_FOUND,
    DV_COUNT
};

/**\brief Codes for file format.*/
enum FILE_FORMAT
{
    FF_UNDEF,
    FF_BASIC /*must remain before the other formats*/,
    FF_GDAL_OGR,
    FF_KML_POINT,
    FF_GPX_POINT,
    FF_NMEA_POINT,
    FF_COUNT
};

/**\brief Codes for coordinate format.*/
//for user-friendly HMI only (unused in API or command-line mode)
enum UI_FORMAT
{
    UF_UNDEF,
    UF_XYZ,
    UF_IXYZ,
    UF_GEOCENTRIC_END,
    UF_LP,
    UF_LPV,
    UF_ILP,
    UF_ILPV,
    UF_GEOGRAPHIC_POINT_END,
    UF_PL,
    UF_PLV,
    UF_IPL,
    UF_IPLV,
    UF_GEOGRAPHIC_3D_BEGIN,
    UF_LPH,
    UF_ILPH,
    UF_GEOGRAPHIC_3D_POINT_END,
    UF_PLH,
    UF_IPLH,
    UF_GEOGRAPHIC_END,
    UF_EN,
    UF_ENV,
    UF_IEN,
    UF_IENV,
    UF_PROJECTED_POINT_END,
    UF_NE,
    UF_NEV,
    UF_INE,
    UF_INEV,
    UF_PROJECTED_3D_BEGIN,
    UF_ENH,
    UF_IENH,
    UF_PROJECTED_3D_POINT_END,
    UF_NEH,
    UF_INEH,
    UF_PROJECTED_END,
#ifdef WITH_GDAL
        UF_GDAL_OGR,
#endif
    UF_NMEA_POINT,
    UF_KML_POINT,
    UF_GPX_POINT,
    UF_COUNT
};

/**\brief Codes for interface elements.*/
enum INTERFACE_ELEMENT
{
    IE_UNDEF,
    IE_GEODETIC_CRS,
    IE_VERTICAL_CRS,
    IE_DATA,
    IE_FORMAT,
    IE_GEODETIC_TRANSFO,
    IE_VERTICAL_TRANSFO,
    IE_FILE,
    IE_POINT,
    IE_DISPLAY,
    IE_HELP,
    IE_DOCUMENTATION,
    IE_ABOUT,
    IE_CLEAR,
    IE_LAYOUT,
    IE_BOUNDINGBOX,
    IE_EPOCH,
    IE_PRINT_CS,
    IE_AUTHORITATIVE_VERTICAL_FRAMES,
    IE_DATA_FILE,
    IE_SELECT,
    IE_CONVERGENCE,
    IE_SCALE_ERROR,
    IE_SCALE_FACTOR,
    IE_DEFLECTION,
    IE_CONV_UNIT,
    IE_UNIT,
    IE_COORD_TYPE,
    IE_INPUT,
    IE_OUTPUT,
    IE_PLAIN_DMS,
    IE_SPPM,
    IE_SCROLLAREA,
    IE_COPYCL,
    IE_ZONE,
    IE_TOOL,
    IE_SWAP,
    IE_AUTHORITATIVE,
    IE_USERMANUAL,
    IE_COPYRES,
    IE_SEPARATOR,
    IE_END
};

/**\brief Codes for file field separators.*/
enum SEPARATORS
{
    FS_SPACE,
    FS_COMMA,
    FS_SEMICOLON,
    FS_TAB,
    FS_END
};

/**\brief Field separator values.*/
static const std::string SeparatorValues[FS_END] =
{
    "_",
    ",",
    ";",
    "\t"
};

/**\brief Codes for boolean.*/
enum CIRCE_BOOL
{
    CB_FALSE,
    CB_TRUE
};


/**\brief Codes for operation application in two contexts:
- 1. Metadata of operations: an operation can be direct only (R_DIRECT) or reversible
with the same method but parameters of opposite signs (R_REVERSE_SAME_METHOD) or
reversible with the same parameters but a different method (R_REVERSE_SAME_PARAMETERS).
It concerns transformations.
- 2. In a specific operation context, the operation may be applied in the direct way
(R_DIRECT) or reverse way depending on its metada (R_REVERSE_SAME_METHOD
or R_REVERSE_SAME_PARAMETERS).
*/
enum OPERATION_APPLICATION
{
    OA_UNDEF,
    OA_DIRECT,
    OA_REVERSE_SAME_METHOD,
    OA_REVERSE_SAME_PARAMETERS,
    OA_COUNT
};

/**\brief Codes for operation status.*/
enum STATUS
{
    S_UNDEF,
    S_AUTHORITATIVE,
    S_DERIVED,
    S_ROUGH,
    S_COUNT
};

/**\brief Codes for transformation required.*/
enum TRANSFO_REQUIRED
{
    T_UNDEF = 0x0000,
    T_GEODETIC_REQUIRED = 0x0001,
    T_VERTICAL_REQUIRED = 0x0002,
    T_VERTICAL_VERTICAL_GEOD0_REQUIRED = 0x0004,
    T_VERTICAL_VERTICAL_GEOD1_REQUIRED = 0x0008,
    T_VERTICAL_VERTICAL_GEOD2_REQUIRED = 0x0010,
    T_GEODETIC1_VERTICAL_REQUIRED = 0x0020,
    T_GEODETIC2_VERTICAL_REQUIRED = 0x0040,
    T_VERTICAL_GEODETIC1_REQUIRED = 0x0080,
    T_VERTICAL_GEODETIC2_REQUIRED = 0x0100
};

/**\brief Codes for coordinate input mode.*/
enum DATA_MODE
{
    DM_UNDEF,
    DM_FILE,
    DM_POINT,
    DM_LIST,
    DM_COUNT
};

/**\brief Codes for multiple CRS definition file processing.*/
enum MULTIPLE_CRS_PROCESSING
{
	MP_UNDEF,
	MP_TEST,
	MP_CALC,
	MP_COUNT
};

/**\brief Codes for CRS description elements.*/
enum CRS_DESCRIPTION_ELEMENTS
{
	CD_UNDEF,
	CD_CODE,
	CD_SOURCE,
	CD_TARGET,
	CD_GEODETIC_FRAME,
	CD_VERTICAL_FRAME,
	CD_COORDINATES,
	CD_ELLIPSOID,
	CD_PROJECTION,
	CD_UNIT,
    CD_GRID,
    CD_EPOCH,
	CD_COUNT
};

//Here are string resources that must not be translated

static std::string GridTypeText[GF_COUNT] =
{
    "UNDEFINED","Circe ascii","Circe bin","Gravsoft ascii","Surfer ascii",
    "NTV2 bin little endian","NTV2 bin big endian","NTV2 ascii",
    "ESRI","EGM","DIS","GTX ascii","GTX bin (big endian)"
};

static const std::string LoadingTypeText[LD_COUNT] =
{
    "UNDEFINED","BINARY","ARRAY","HEADER","XML"
};

static std::string EncodingTypeText[ET_COUNT] =
{
    "UNDEFINED","ASCII","LITTLE_ENDIAN","BIG_ENDIAN"
};

/**\brief Array of std::string representing measure units.*/
static const std::string MeasureUnitAttributeText[MU_COUNT] =
{
    "UNDEFINED", "RADIANS", "SECONDS", "DEGREES", "DMS", "DM", "GONS", "METERS", "INCHES", "PPM", "DEGPERMY", "MASPERY"
};

/*
 "dec"="deg"
static const std::string MeasureUnitIGNFtext[MU_COUNT] =
{
    "UNITE","rad","sec","deg","","","gr","m","","",""
};
*/

static const std::string CRStypeAttributeText[CT_COUNT] =
{
    "UNDEFINED","GEOCENTRIC","GEOGRAPHIC","PROJECTED","VERTICAL"
};

static const std::string BoolText[2] =
{
    "NO","YES"
};

static const std::string OperationApplicationText[OA_COUNT] =
{
    "UNDEFINED","DIRECT","REVERSE_SAME_METHOD","REVERSE_SAME_PARAMETERS"
};

static const std::string StatusText[S_COUNT] =
{
    "UNDEFINED","AUTHORITATIVE","DERIVED","ROUGH"
};

static const std::string FileFormatText[FF_COUNT] =
{
    "UNDEFINED","BASIC","GDAL_OGR","KML","GPX","NMEA"
};

//for user-friendly HMI only (unused in API or command-line mode)
static const std::string uiFormat[UF_COUNT] =
{
        "",
        "XYZ",
        "IXYZ",
        "",
        "LP",
        "LPV",
        "ILP",
        "ILPV",
        "",
        "PL",
        "PLV",
        "IPL",
        "IPLV",
        "",
        "LPH",
        "ILPH",
        "",
        "PLH",
        "IPLH",
        "",
        "EN",
        "ENV",
        "IEN",
        "IENV",
        "",
        "NE",
        "NEV",
        "INE",
        "INEV",
        "",
        "ENH",
        "IENH",
        "",
        "NEH",
        "INEH",
        "",
#ifdef WITH_GDAL
     "GDAL_OGR",
#endif
    "NMEA",
    "KML",
    "GPX"
};

}

#endif // STRINGRESOURCES_H
