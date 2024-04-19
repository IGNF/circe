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
* \file referenceframe.cpp
* \brief Implementation of the ReferenceFrame and ReferenceSystem classes.
*/

#include "referenceframe.h"

namespace circe
{

/**
* \brief Constructor.
*/
ReferenceFrame::ReferenceFrame()
{
}

/**
* \brief Copy constructor.
*/
ReferenceFrame::ReferenceFrame(const ReferenceFrame &B) : CirceBoundedObject(B) 
{
    EpochIni = B.EpochIni;
    usesForTransformation = B.usesForTransformation;
    referenceSystemId = B.referenceSystemId;
}

/**
* \brief Empties values of the members of a ReferenceFrame instance.
*/
void ReferenceFrame::reset()
{
    CirceBoundedObject::reset();
    EpochIni = 0.;
    usesForTransformation = referenceSystemId = "";
}

/**
* \brief Constructor.
*/
ReferenceSystem::ReferenceSystem()
{
}

/**
* \brief Copy constructor.
*/
ReferenceSystem::ReferenceSystem(const ReferenceSystem &B) : CirceBoundedObject(B) 
{
    SystemDimension = B.SystemDimension;
}

/**
* \brief Initializes SystemDimension.
* \param[in] _value: A std::string that should fit with a DimensionText value.
*/
void ReferenceSystem::setSystemDimension(std::string _value)
{
    for(int i = 0; i<SD_COUNT; i++) if (_value == DimensionText[i])
    {
        SystemDimension = (SYSTEM_DIMENSION)i;
        break;
    }
}

/**
* \brief Empties values of the members of a ReferenceSystem instance.
*/
void ReferenceSystem::reset()
{
    CirceBoundedObject::reset();
    SystemDimension = SD_UNDEF;
}

} //namespace circe
