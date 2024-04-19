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
* \file crs.cpp
* \brief Implementation of the CRS class.
*/

#include "crs.h"
#include "stringtools.h"

namespace circe
{

/**
* \brief Constructor.
*/
CRS::CRS()
{
}

/**
* \brief Copy constructor.
*/
CRS::CRS(const CRS &B) : CirceBoundedObject(B) 
{
    CRStype = B.CRStype;
    referenceFrameId = B.referenceFrameId;
    baseGeographicCRSid = B.baseGeographicCRSid;
    conversionId = B.conversionId;
    RF = B.RF;
    DefiningConversion = B.DefiningConversion;
}

/**
* \brief Empties values of the members of a CRS instance.
*/
void CRS::reset()
{
    CirceBoundedObject::reset();
    CRStype = CT_UNDEF;
    referenceFrameId = baseGeographicCRSid = conversionId = "";
    RF = NULL;
    DefiningConversion = NULL;
}

/**
* \brief Returns a pointer to a CRS corresponding to the input id.
* \param[in] id: An id as a std::string. The authority may be IGNF or EPSG. Without an authority, it is considered to be IGNF. 
* \return A pointer to a CRS corresponding to the input id.
*/
CRS* CRS::isCRSmatching(std::string id)
{
    if (getCirceId() == id)
        return this;
    else if (id.find_first_of("IGNF") != std::string::npos
             || id.find_first_of("ignf") != std::string::npos)
        //e.g. id=EPSG::310024142
        if (getCirceId() == afterChar(id,':'))
            return this;
    else if (id.find_first_of("EPSG") != std::string::npos
             || id.find_first_of("epsg") != std::string::npos)
        //e.g. id=EPSG::310024142
        if (getEPSGid() == afterChar(id,':'))
            return this;
    return NULL;
}

/**
* \brief Displays the description of the CRS.
* \return The description.
*/
std::string CRS::displayDescription()
{
	std::string str;
	if (getCRStype() == CT_VERTICAL)
		str += "*!" + CRSdescriptionElements[CD_VERTICAL_FRAME] + ": " + getReferenceFrame()->getName() + "\n";
	else
	{
		str += "*!" + CRSdescriptionElements[CD_GEODETIC_FRAME] + ": " + getReferenceFrame()->getName() + "\n";
		str += "*!" + CRSdescriptionElements[CD_COORDINATES] + ": " + CRStypeDisplayText[getCRStype()] + "\n";
		if (getCRStype() == CT_GEOGRAPHIC || getCRStype() == CT_PROJECTED)
			str += "*!" + CRSdescriptionElements[CD_ELLIPSOID] + ": " + ((GeodeticReferenceFrame*)getReferenceFrame())->getEllName() + "\n";
		if (getCRStype() == CT_PROJECTED)
			str += "*!" + CRSdescriptionElements[CD_PROJECTION] + ": " + getDefiningConversion()->getName() + "\n";
	}
	return str;
}

} //namespace circe
