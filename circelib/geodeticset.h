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
* \file geodeticset.h
* \brief Definition of the GeodeticSet class.
*/

#ifndef GEODETICSET_H
#define GEODETICSET_H

#include <vector>
#include "parser.h"
#include "circeboundedobject.h"
#include "ellipsoid.h"
#include "meridian.h"
#include "referenceframe.h"
#include "geodeticreferenceframe.h"
#include "verticalreferenceframe.h"
#include "conversion.h"
#include "transformation.h"
#include "crs.h"
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif


namespace circe
{


/**
* \brief geodeticSubset is used by operationSubset.
*/
struct geodeticSubset
{
    std::vector<std::string> geodeticFrameIdList, geodeticFrameDisplayList;
    std::vector<std::string> geodeticCRSidList, geodeticCRSdisplayList;
    std::vector<std::string> verticalFrameIdList, verticalFrameDisplayList;
    std::vector<std::string> dataFileFormatList, dataFileFormatDisplayList;
    std::vector<std::string> dataPointFormatList, dataPointFormatDisplayList;
    std::vector<std::string> dataUnitList, dataUnitDisplayList;
    int iGeodeticFrame, iGeodeticCRS, iVerticalFrame, iDataFileFormat, iDataPointFormat, iDataUnit;
    circe::SYSTEM_DIMENSION geodeticDimension;
};

/**
* \brief operationSubset is used in user-friendly HMIs. It is intended to provide lists of frames and CRS consistent
* with the user's choices: the target CRSs that may be reached from the source CRS chosen, using existing transformations, etc.
*/
struct operationSubset
{
    geodeticSubset sourceSubset, targetSubset;
    std::vector<std::string> geodeticTransfoIdList, verticalTransfoIdList;
    std::vector<std::string> geodeticTransfoDisplayList, verticalTransfoDisplayList;
    int iGeodeticTransfo, iVerticalTransfo;
};

/**
* \brief This class is directly used by Circe API, high-level and convenience functions to store data loaded from a metadata file.
* It is also used to manage data subsets according to the options selected by the user, in order to populate widgets in user-friendly HMIs.
* \see getGeodeticSubset
*/
class GeodeticSet : public CirceBoundedObject
{
private:
	std::string keyGeodeticFrame;
public:
	std::vector<Ellipsoid> vEllipsoid;
    std::vector<Meridian> vMeridian;
    std::vector<GeodeticReferenceFrame> vGeodeticFrame;
    std::vector<VerticalReferenceFrame> vVerticalFrame;
    std::vector<GeodeticReferenceSystem> vGeodeticSystem;
    std::vector<VerticalReferenceSystem> vVerticalSystem;
    std::vector<Transformation> vGeodeticTransfo;
    std::vector<Transformation> vVerticalTransfo;
    std::vector<Conversion> vConversion;
    std::vector<CRS> vCRS;
    std::string metadataPath;
public:
	GeodeticSet();
	void reset();
    GeodeticReferenceFrame* getGeodeticReferenceFrame(std::string id);
    CRS* getCRS(std::string id);
	std::string getVerticalReferenceFrameName(std::string id);
	std::string getCRSid(std::string frameId);
	std::string getAuthority(std::string CRSid, std::string *CRStype);
    std::string displayGeodeticSet();
    std::string displayAll(const operationSubset& os);
    void CompleteMetadata();
    std::string getCRSidList(std::string CRStype, const circe::circeOptions& circopt);
    std::string getValidityArea(std::string CRSid, MEASURE_UNIT outUnit, std::string frmt);
    std::string getValidityArea(MEASURE_UNIT outUnit, std::string frmt);
    bool isVerticalFrameMatching(std::string str, STATUS st, const circe::geodeticOptions& go);
    void getGeodeticFrameList(geodeticSubset* gss, const circe::geodeticOptions& go, std::string *l, std::string frmt);
    void getTargetGeodeticFrameList(geodeticSubset *targetSubset, const circe::circeOptions& CO, std::string* l, std::string frmt);
    void getGeodeticTransfoList(operationSubset *OS, const circe::circeOptions& CO, std::string *l, std::string frmt);
    void getGeodeticCRSlist(geodeticSubset *gss, const circe::geodeticOptions &go, std::string* l, std::string frmt);
    void getSourceVerticalFrameList(geodeticSubset* sourceSubset, const circe::geodeticOptions& sourceOptions, std::string *l, std::string frmt);
    void getTargetVerticalFrameList(geodeticSubset *targetSubset, const circe::circeOptions &CO, std::string* l, std::string frmt);
    void getVerticalTransfoList(operationSubset *OS, const circe::circeOptions& CO, std::string *l, std::string frmt);
    void getDataFormatList(int* iDataFormat, std::vector<std::string>* dataFormatList, std::vector<std::string>* dataFormatDisplayList, circe::SYSTEM_DIMENSION gd, const circe::geodeticOptions& go, DATA_MODE dm, std::string* l, std::string frmt);
    void getDataUnitList(std::string what, geodeticSubset* gss, const circe::geodeticOptions &go, std::string *l, std::string frmt);
	void initGeodeticIds(circe::geodeticOptions* go, geodeticSubset* gs);

	std::string	getKeyGeodeticFrame() const { return keyGeodeticFrame; }
	void setKeyGeodeticFrame(std::string _value){ if (_value == "")return; keyGeodeticFrame = _value; }

	static std::string displayGeodeticSubset(const geodeticSubset &gs);
	static std::string displayOperationSubset(const operationSubset &os);
};

} //namespace circe


#endif // GEODETICSET_H
