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
* \file meridian.cpp
* \brief Implementation of the Meridian class.
*/

#include "meridian.h"

namespace circe
{

/**
* \brief Constructor.
*/
Meridian::Meridian()
{
}

/**
* \brief Copy constructor.
*/
Meridian::Meridian(const Meridian &B) : CirceObject(B)
{
    fromGreenwich = B.fromGreenwich;
}

/**
* \brief Empties values of the members of a Meridian instance.
*/
void Meridian::reset()
{
    CirceObject::reset();
    fromGreenwich = 0.;
}

} //namespace circe
