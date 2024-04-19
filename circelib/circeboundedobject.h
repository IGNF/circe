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
* \file circeboundedobject.h
* \brief Definition of CirceBoundedObject class.
*/

#ifndef CIRCEBOUNDEDOBJECT_H
#define CIRCEBOUNDEDOBJECT_H

#include <cstdlib> //atof
#include <iterator>     // std::back_inserter
#include "circeobject.h"
#include "units.h"
#include "pt4d.h"

namespace circe
{

/** \brief Container of polygon boundaries. 
In the vector of double bound, 2 back-to-back values represent the geographic coordinates (longitude latitude) of one point.
*/
struct poly
{
	std::string id;
	std::vector<double> bound;
};

/**
* \brief This class handles the geographic boundaries of most of Circe object (generally, its child classes).
* By default, a boundary is defined by four geographic coordinates (west, east, north and south limits).
* This class also handles polygon boundaries (hasPolygonBound==true) namely for tectonic plates.
*/
class CirceBoundedObject : public CirceObject
{
protected:
	/**\brief West bound of a rectangular boundary*/
	double WestBound;

	/**\brief East bound of a rectangular boundary*/
	double EastBound;

	/**\brief North bound of a rectangular boundary*/
	double NorthBound;

	/**\brief South bound of a rectangular boundary*/
	double SouthBound;

	/**\brief Simplistic area of a rectangular boundary. \see setArea.*/
	double Area;

	/**\brief MEASURE_UNIT code for boundary unit*/
	MEASURE_UNIT BoundUnit;

	/**\brief true if the object has polygon boundaries*/
	bool hasPolygonBound;

	/**\brief polygon boundary*/
	poly* polygonBound;

public:
    CirceBoundedObject();
    CirceBoundedObject(const CirceBoundedObject &B) ; // copy constructor
    void reset();

    void copyBounds(const CirceBoundedObject &B);

    void setWestBound(double _value) {WestBound=_value;}
    void setEastBound(double _value) {EastBound=_value;}
    void setNorthBound(double _value) {NorthBound=_value;}
    void setSouthBound(double _value) {SouthBound=_value;}
    //    void setWestBound(double _value){if(_value==0)return; WestBound=_value;}
    //    void setEastBound(double _value){if(_value==0)return; EastBound=_value;}
    //    void setSouthBound(double _value){if(_value==0)return; SouthBound=_value;}
    //    void setNorthBound(double _value){if(_value==0)return; NorthBound=_value;}
    void setWestBound(std::string _value){if(_value=="")return; WestBound=atof(_value.data());}
    void setEastBound(std::string _value){if(_value=="")return; EastBound=atof(_value.data());}
    void setSouthBound(std::string _value){if(_value=="")return; SouthBound=atof(_value.data());}
    void setNorthBound(std::string _value){if(_value=="")return; NorthBound=atof(_value.data());}
    void setBoundUnit(std::string _value);
    void setArea() {Area = fabs ((WestBound-EastBound)*(NorthBound-SouthBound));}
	void setHasPolygonBound(bool _value) {hasPolygonBound=_value;}
    void setPolygonBound(poly* _value);

    double getWestBound() const {return WestBound;}
    double getEastBound() const {return EastBound;}
    double getNorthBound() const {return NorthBound;}
    double getSouthBound() const {return SouthBound;}
    double getArea() const {return Area;}
	bool getHasPolygonBound() const {return hasPolygonBound;}
	poly* getPolygonBound() const {return polygonBound;}

	bool isInside(double lon, double lat) const;
    //bool operator<(const CirceBoundedObject& cbo); //inclusion
    bool intersects(const CirceBoundedObject &B);
    bool CRSintersect(const CirceBoundedObject &B);
    std::string getValidityArea(MEASURE_UNIT outUnit, std::string frmt);

	static bool isInsidePoly(double l, double p, const poly& poly);
	static std::string loadPolygons(const std::string& polygonFilePath, std::vector<poly>& polys, const circe::pt4d& pt, int mode);
	static std::string findPolygon(circe::pt4d pt, const std::vector<poly>& polys);

};

} //namespace circe

#endif // CIRCEBOUNDEDOBJECT_H
