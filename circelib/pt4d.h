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
* \file pt4d.h
* \brief Definition of pt4d management.
*/

#ifndef PT4D
#define PT4D
#include <string>
#include <vector>
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif
namespace circe
{

/**
\brief pt4d accomodates data used in single point operations: id, all type of coordinates 
(considering that some operations require more than one coordinate type together) and collateral projection output.
The structure refers to points that have 2d or 3d geodetic coordinates and a vertical coordinate.
In this version, it doesn't include velocities.
*
\see The function writeBasicLine outputs a pt4d content according to a data format.
*/

struct displayFormat {
	unsigned int n_m;   //precision (number of decimal digits) of values in meters 
	unsigned int n_ang;  //precision (number of decimal digits) of angular values according to specified unit
    std::string angularUnit;
    bool plainDMS;
} ;

// info is firstly intended to contain target vertical CRS id or name when an authoritative vertical 
// transformation is required and so the target vertical CRS can vary from point to point.
struct pt4d {
	std::string id; double x; double y; double z; double l; double p; double h; double e; double n; double w;
    double conv; double scfact; double xi; double eta; CRS_TYPE CRStypeInit; int Gprec; int Vprec; displayFormat frmt;
    std::string info; std::string info2; int transfoIndex;
};

void initFormats(double prec, std::string angularUnit, displayFormat *frmt);
std::string precisionCode2Text(int CodePrecision);
std::string ComparePoint(pt4d *p, double c1, double c2, double c3, double c4);
bool ComparePoint2(double a, double e2, pt4d *p, double c1, double c2, double c3, double c4, MEASURE_UNIT linearUnitId, MEASURE_UNIT angularUnitId, std::string *retMess, double *maxShift);
void InitDefPoint(pt4d *p);
void InitPoint(pt4d *p, double c1, double c2, double c3, double c4);
void copyPt4d(const pt4d& p, double *c1, double *c2, double *c3, double *c4);
void copyPt4d(pt4d *p1, const pt4d& p2);
void copyPt4dVertical(pt4d *p1, const pt4d& p2);

std::string displayElement(const pt4d& p, char c);
std::string displayEpoch(double e);
}
#endif // PT4D

