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
* \file geodeticreferenceframe.h
* \brief Definition of the GeodeticReferenceSystem and GeodeticReferenceFrame classes.
*/

#ifndef GEODETICREFERENCEFRAME_H
#define GEODETICREFERENCEFRAME_H

#include "circeboundedobject.h"
#include "referenceframe.h"
#include "meridian.h"

namespace circe
{

/**
* \brief This class manages Geodetic Reference System data.
*/
class GeodeticReferenceSystem: public ReferenceSystem
{
private:
    std::string primeMeridian;
    std::string ellipsoid;
    std::string usesForTransformation;
public:
    GeodeticReferenceSystem();
    GeodeticReferenceSystem(const GeodeticReferenceSystem &B); // copy constructor
    void reset();
    void setPrimeMeridianId(std::string _value) {primeMeridian=_value;}
    void setEllipsoidId(std::string _value) {ellipsoid=_value;}
    void setUsesForTransformation(std::string _value) {usesForTransformation=_value;}
    std::string getPrimeMeridianId() const {return primeMeridian;}
    std::string getEllipsoidId() const {return ellipsoid;}
    std::string getUsesForTransformation() const {return usesForTransformation;}

};

/**
* \brief This class manages Geodetic Reference Frame data.
*/
class GeodeticReferenceFrame : public ReferenceFrame
{
private:
    double a;
    double e2;
    double b;
    double f;
	std::string ellName;
    double fromGreenwich;
    SYSTEM_DIMENSION SystemDimension;
    GeodeticReferenceSystem* GRS;
    std::string geographicCRSid;
    bool hasTransformationWithKeyFrame;
public:
    GeodeticReferenceFrame();
    GeodeticReferenceFrame(const GeodeticReferenceFrame &B); // copy constructor
    void reset();

    double get_a() const {return a;}
    double get_b() const {return b;}
    double get_e2() const {return e2;}
	std::string getEllName() const { return ellName; }
    double getPrimeMeridianValue() const {return fromGreenwich;}
    SYSTEM_DIMENSION getSystemDimension() const {return SystemDimension;}
    GeodeticReferenceSystem* getGeodeticReferenceSystem() const {return GRS;}
    std::string getGeographicCRSid() const {return geographicCRSid;}
    bool getHasTransformationWithKeyFrame() const {return hasTransformationWithKeyFrame;}

    void set_a(double _value) {a=_value;}
    void set_b(double _value) {b=_value;}
    void set_e2(double _value) {e2=_value;}
	void set_f(double _value) { f = _value; }
	void setEllName(std::string _value) { ellName = _value; }
	void setFromGreenwich(double _value) { fromGreenwich = _value; }
    void setSystemDimension(SYSTEM_DIMENSION _value) {SystemDimension=_value;}
    void setGeodeticReferenceSystem(GeodeticReferenceSystem* _value) {GRS=_value;}
    void setGeographicCRSid(std::string _value) {geographicCRSid=_value;}
    void setHasTransformationWithKeyFrame(bool _value) {hasTransformationWithKeyFrame=_value;}
};

} //namespace circe

#endif // GEODETICREFERENCEFRAME_H
