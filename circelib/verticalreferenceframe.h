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
* \file verticalreferenceframe.h
* \brief Definition of the VerticalReferenceFrame and VerticalReferenceSystem classes.
*/

#ifndef VERTICALREFERENCEFRAME_H
#define VERTICALREFERENCEFRAME_H

#include "circeboundedobject.h"
#include "referenceframe.h"

namespace circe
{

/**
* \brief This class manages Vertical Reference System data.
*/
class VerticalReferenceSystem: public ReferenceSystem
{
private:
    static const SYSTEM_DIMENSION SystemDimension=SD_1D;
public:
    VerticalReferenceSystem();
    VerticalReferenceSystem(const VerticalReferenceSystem &B); 
    void reset();
    SYSTEM_DIMENSION getSystemDimension() const {return SD_1D;}
};

/**
* \brief This class manages Vertical Reference Frame data.
*/
class VerticalReferenceFrame : public ReferenceFrame
{
private:
    VerticalReferenceSystem* VRS;
public:
    VerticalReferenceFrame();
    VerticalReferenceFrame(const VerticalReferenceFrame &B);
    void reset();
    VerticalReferenceSystem* getVerticalReferenceSystem() const {return VRS;}
    void setVerticalReferenceSystem(VerticalReferenceSystem* _value) {VRS=_value;}
};

} //namespace circe

#endif // VERTICALREFERENCEFRAME_H
