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
* \file referenceframe.h
* \brief Definition of the ReferenceFrame and ReferenceSystem classes.
*/

#ifndef REFERENCEFRAME_H
#define REFERENCEFRAME_H

#include "circeboundedobject.h"

namespace circe
{

/**
* \brief This class manages Reference Frame data (that Geodetic and Vertical Frames share).
*/
class ReferenceFrame: public CirceBoundedObject
{
protected:
    double EpochIni;
    std::string usesForTransformation;
    std::string referenceSystemId;
public:
    ReferenceFrame();
    ReferenceFrame(const ReferenceFrame &B);
    void reset();
    //ReferenceFrame getFromId(std::string id);
    double get_EpochIni(){return EpochIni;}
    std::string getUsesForTransformation() const {return usesForTransformation;}
    std::string getReferenceSystemId() const {return referenceSystemId;}
    void setUsesForTransformation(std::string _value) {usesForTransformation=_value;}
    void setReferenceSystem(std::string _value) {referenceSystemId=_value;}
};

/**
* \brief This class manages Reference System data (that Geodetic and Vertical Systems share).
*/
class ReferenceSystem : public CirceBoundedObject
{
private:
    SYSTEM_DIMENSION SystemDimension;
public:
    ReferenceSystem();
    ReferenceSystem(const ReferenceSystem &B);
    void reset();
    SYSTEM_DIMENSION getSystemDimension() const {return SystemDimension;}
    void setSystemDimension(std::string _value);
};

} //namespace circe

#endif // REFERENCEFRAME_H
