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
* \file crs.h
* \brief Definition of the CRS class.
*/

#ifndef CRS_H
#define CRS_H

#include "units.h"
#include "conversion.h"
#include "geodeticreferenceframe.h"
#include "verticalreferenceframe.h"

namespace circe
{

/**
* \brief This class manages the Coordinate Reference Systems (CRSs) of every type: 
* Geocentric, Geographic, Projected and Vertical.
*/
class CRS: public CirceBoundedObject
{
private:
    CRS_TYPE CRStype;
    std::string referenceFrameId;
    std::string baseGeographicCRSid;
    std::string conversionId;
    ReferenceFrame* RF;
    Conversion* DefiningConversion;
public:
    CRS();
    CRS(const CRS &B) ; // copy constructor
    void reset();

    CRS_TYPE getCRStype() const {return CRStype;}
    std::string getReferenceFrameId() const {return referenceFrameId;}
    std::string getBaseGeographicCRSid() const {return baseGeographicCRSid;}
    std::string getConversionId() const {return conversionId;}
    ReferenceFrame* getReferenceFrame() const {return RF;}
    Conversion* getDefiningConversion() const {return DefiningConversion;}

    void setCRStype (CRS_TYPE _value) {CRStype=_value;}
    void setReferenceFrameId(std::string _value) {referenceFrameId=_value;}
    void setBaseGeographicCRSid(std::string _value) {baseGeographicCRSid=_value;}
    void setConversionId(std::string _value) {conversionId=_value;}
    void setReferenceFrame(ReferenceFrame* _value) {RF=_value;}
    void setDefiningConversion(Conversion* _value) {DefiningConversion=_value;}

    CRS* isCRSmatching(std::string id);
	std::string displayDescription();
};

} //namespace circe

#endif // CRS_H
