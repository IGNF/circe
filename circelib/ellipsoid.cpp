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
* \file ellipsoid.cpp
* \brief Implementation of the Ellipsoid class.
*/

#include "ellipsoid.h"

namespace circe
{

/**
* \brief Constructor.
*/
Ellipsoid::Ellipsoid()
{
}

/**
* \brief Copy constructor.
*/
Ellipsoid::Ellipsoid(const Ellipsoid &B) : CirceObject(B)
{
    a = B.a;
    b = B.b;
    e2 = B.e2;
    f = B.f;
}

/**
* \brief Empties values of the members of an Ellipsoid instance.
*/
void Ellipsoid::reset()
{
    CirceObject::reset();
    a = b = e2 = f = 0.;
}

} //namespace circe
