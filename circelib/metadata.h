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
* \file metadata.h
* \brief Definition of Circe geodetic metadata writing and loading.
*/

#include <vector>
#include "tinyxml2/tinyxml2.h"
#include "units.h"
#include "geodeticset.h"

namespace circe
{

/**\brief Structure designed to store Ellipsoid data from Circe Legacy Metadata.*/
struct ELG {std::string num; double a; double b; double rf; double es; std::string name;};

/**\brief Structure designed to store Prime meridian data from Circe Legacy Metadata.*/
struct LGO { std::string num; std::string unit; double value; std::string name; };

/**\brief Structure designed to store Geodetic Frame data from Circe Legacy Metadata. 
Each REG instance is used to define at least 4 objects in the Circe XML metadata file: 
4 related to Greenwich and other related to the native prime meridian if different 
from Geenwich (Greenwich-related frames are mandatory in geodetic transformations).
- 1 or 2 geodeticReferenceSystem (id from num for internal management)
- 1 or 2 geodeticReferenceFrame (id from num for internal management)
- 1 GeocentricCRS for 3-D or 4-D systems (id = short_name, <b>for use by user</b>)
- 1 or 2 GeographicCRS (id = short_name+'G' and id = short_name+'I'+'G' where 'I' stands for the LGO (prime meridian) name initial, <b>for use by user</b>)
*
* \brief See the resulting Circe XML metadata file for a better comprehension.
*/
struct REG {std::string num; std::string ELG; std::string LGO; std::string dimension;
            std::string short_name; std::string name; double from_greenwich;};

/**\brief Structure designed to store Vertical Frame data from Circe Legacy Metadata.
Each REA instance is used to define 3 objects in the Circe XML metadata file:
- 1 verticalReferenceSystem (id from num for internal management)
- 1 verticalReferenceFrame (id from num for internal management)
- 1 VerticalCRS (id = short_name, <b>for use by user</b>)
*
* \brief See the resulting Circe XML metadata file for a better comprehension.
*/
struct REA { std::string num; std::string short_name; std::string name; };

/**\brief Structure designed to store approximate (mean) grid transformation parameters from Circe Legacy Metadata.*/
struct TSG0 { std::string REG1; std::string REG2; double tx0; double ty0; double tz0; };

/**\brief Structure designed to store constant (not grid) geodetic transformation parameters from Circe Legacy Metadata.*/
struct TSG {std::string num; std::string REG1; std::string REG2; int prec; 
			double tx; double ty; double tz; double ech; double rx; double ry; double rz;
			double dtx; double dty; double dtz; double dech; double drx; double dry; double drz;
            double epoch;};

/**\brief Structure designed to store constant (not grid) shift&tilt vertical transformation parameters from Circe Legacy Metadata.*/
struct TSV { std::string num; std::string REG; std::string REA1; std::string REA2; int prec;
             double a1; double a2; double a3; double lon0; double lat0;};

/**\brief Structure designed to store projection method data from Circe Legacy Metadata.*/
struct PRC {std::string num; int mode; double x0; double y0; std::string LGO; std::string unit; 
			double l0; double j0; double delta; double j1; double j2; double c; int type; std::string name;};

/**\brief Structure designed to store Projected CRS data from Circe Legacy Metadata.*/
struct PROJ {std::string REG; std::string PRC; std::string unit; double lonMin; double latMin;
            double lonMax; double latMax; std::string short_name; std::string name;};

/**\brief Structure designed to store vertical transformation grid data from Circe Legacy Metadata.*/
struct GR1D { std::string filename; std::string REG; std::string REA1; std::string REA2; };

/**\brief Structure designed to store geodetic transformation grid data from Circe Legacy Metadata.*/
struct GR3D { std::string filename; std::string REG1; std::string REG2; };

/**\brief Structure designed to store rectangular geographic definition domain (for frames and transformations) 
from Circe Legacy Metadata. Some objects get their definition domain from somewhere else (grid metadata give definition
domain to corresponding transformation and vertical frame ; etc.).*/
struct ZNE { std::string REG; std::string unit; double lonMin; double latMin; double lonMax; double latMax; };

/**\brief Structure designed to store coordinate type data from Circe Legacy Metadata (deprecated).*/
struct TYPCOOR { std::string REG; std::string COOR; };

/**\brief Structure designed to store unit data from Circe Legacy Metadata.*/
struct UNIT { std::string REG; std::string _UNIT; };

/**\brief Structure designed to store Plate Motion Model data from Circe Legacy Metadata.*/
struct PMM { std::string num; std::string REG; double dtx; double dty; double dtz; double drx; double dry; double drz; std::string name; };

/**\brief Structure designed to store Grid Motion Model data from Circe Legacy Metadata.*/
struct GRMM { std::string filename; std::string REG; };

/**\brief Structure designed to store Circe Legacy Metadata.*/
struct LEGACY_METADATA_SET
{
	std::vector<ELG> vELG;
	std::vector<LGO> vLG0;
	std::vector<REG> vREG;
	std::vector<REA> vREA;
	std::vector<TSG0> vTSG0;
	std::vector<TSG> vTSG;
	std::vector<TSV> vTSV;
	std::vector<PRC> vPRC;
	std::vector<PROJ> vPROJ;
	std::vector<GR1D> vGR1D;
	std::vector<GR3D> vGR3D;
    std::vector<ZNE> vZNE;
    std::vector<PMM> vPMM;
    std::vector<GRMM> vGRMM;
    std::vector<TYPCOOR> vTYPCOOR;
	std::vector<UNIT> vUNIT;
	std::string key_REG;
};

/**\brief Structure designed to store operation parameter data from IGNF only.*/
struct operationParameter {std::string id; std::string name; std::string valueFile; std::string unit; double value;};

std::string createID(std::string str);
std::string decodeID(const std::string& str, double *epoch);
int initValuesNumber(const TSG& tsg);
std::string initValuesString(const TSG& tsg);
void LoadEllipsoid(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadPrimeMeridian(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadgeodeticReferenceSystem(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadgeodeticReferenceFrame(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadGeocentricCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadGeographicCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadverticalReferenceSystem(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadverticalReferenceFrame(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadVerticalCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadFrameTransformation(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode, std::vector<operationParameter> *vOpams);
void LoadConversion(circe::GeodeticSet *GS, tinyxml2::XMLElement *_CirceNode, std::vector<operationParameter> *vOpams);
void LoadOperationParameterNames(std::vector<operationParameter> *vOpams, tinyxml2::XMLElement *_CirceNode);
double selectOpam(std::string paramname, std::vector<operationParameter> *vOpams, int *nParam);
void LoadOperationParameters(std::vector<operationParameter> *vOpams, tinyxml2::XMLElement *_EntityNode);
void LoadProjectedCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode);
void LoadCirceXMLmetadata(const std::string& infile, circe::GeodeticSet *GS);
void LoadIGNFxmlMetadata(const std::string& infile, circe::GeodeticSet *GS);
void LoadCirceLegacyMetadataFile(const std::string filedata, LEGACY_METADATA_SET *lms);
void WriteXMlfromCirceLegacyMetadata(const std::string outfile, const LEGACY_METADATA_SET& lms);
void readBounds(tinyxml2::XMLElement *_EntityNode, CirceBoundedObject *cbo);
}
