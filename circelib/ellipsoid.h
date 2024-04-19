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
* \file ellipsoid.h
* \brief Definition of the Ellipsoid class.
*/

#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <cmath>
#include "circeobject.h"

namespace circe
{

/**
* \brief This class manages ellipsoid data.
*/
class Ellipsoid: public CirceObject
{
private:
    double a, b, e2, f;
public:
    Ellipsoid();
    Ellipsoid(const Ellipsoid &B) ; // copy constructor
    void reset();
    void set_a(double _a) {a=_a;}
    void set_b(double _b) {b=_b;}
    void set_f(double _f) {f=_f;}
    void set_e2(double _e2) {e2=_e2;}
    double get_a() const {return a;}
    double get_b() const {return b;}
    double get_e2() const {return e2;}
    double get_f() const {return f;}
	/**\brief First parameter (a) given, calculates second parameter (e2) from b.*/
	void b__e2(){ e2 = 1. - pow(b / a, 2); }
	/**\brief First parameter (a) given, calculates second parameter (e2) from f.*/
	void f__e2(){ e2 = 1. - pow(1. - f, 2); }
};

} //namespace circe

#endif // ELLIPSOID_H
