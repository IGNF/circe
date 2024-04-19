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
* \file geodeticreferenceframe.cpp
* \brief Implementation of the GeodeticReferenceSystem and GeodeticReferenceFrame classes.
*/

#include "geodeticreferenceframe.h"

namespace circe
{

/**
* \brief Constructor.
*/
GeodeticReferenceSystem::GeodeticReferenceSystem()
{
}

/**
* \brief Copy constructor.
*/
GeodeticReferenceSystem::GeodeticReferenceSystem(const GeodeticReferenceSystem &B) : ReferenceSystem(B)
{
    primeMeridian = B.primeMeridian;
    ellipsoid = B.ellipsoid;
    usesForTransformation = B.usesForTransformation;
}

/**
* \brief Empties values of the members of a GeodeticReferenceSystem instance.
*/
void GeodeticReferenceSystem::reset()
{
    ReferenceSystem::reset();
    primeMeridian = ellipsoid = usesForTransformation = "";
}

/**
* \brief Constructor.
*/
GeodeticReferenceFrame::GeodeticReferenceFrame()
{
}

/**
* \brief Copy constructor.
*/
GeodeticReferenceFrame::GeodeticReferenceFrame(const GeodeticReferenceFrame &B) : ReferenceFrame(B) 
{
    a = B.a;
    b = B.b;
    e2 = B.e2;
    f = B.f;
	ellName = B.ellName;
    fromGreenwich = B.fromGreenwich;
    SystemDimension = B.SystemDimension;
    GRS = B.GRS;
    hasTransformationWithKeyFrame = B.hasTransformationWithKeyFrame;
}

/**
* \brief Empties values of the members of a GeodeticReferenceFrame instance.
*/
void GeodeticReferenceFrame::reset()
{
    ReferenceFrame::reset();
    a = b = e2 = f = fromGreenwich = 0.;
	ellName = "";
    SystemDimension = SD_UNDEF;
    GRS = NULL;
    hasTransformationWithKeyFrame = false;
}


} //namespace circe
