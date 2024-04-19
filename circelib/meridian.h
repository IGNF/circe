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
* \file meridian.h
* \brief Definition of the Meridian class.
*/

#ifndef MERIDIAN_H
#define MERIDIAN_H

#include "circeobject.h"
#include "units.h"

namespace circe
{

/**
* \brief This class manages prime meridian data.
*/
class Meridian: public CirceObject
{
private:
	/**\brief Prime meridian longitude in radians*/
    double fromGreenwich;
public:
    Meridian();
    Meridian(const Meridian &B); // copy constructor
    void reset();
    void setFromGreenwich(double _value) {fromGreenwich=_value;}
    double getFromGreenwich () const {return fromGreenwich;}
};

} //namespace circe

#endif // MERIDIAN_H
