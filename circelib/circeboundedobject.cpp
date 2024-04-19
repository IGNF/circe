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
* \file circeboundedobject.cpp
* \brief Implementation of CirceBoundedObject class.
*/

#include "circeboundedobject.h"
#include "stringtools.h"
#include "pt4d.h"
#include "conversioncore.h"

namespace circe
{

/**
* \brief Constructor.
*/
CirceBoundedObject::CirceBoundedObject()
{
    reset();
}

/**
* \brief Copy constructor.
*/
CirceBoundedObject::CirceBoundedObject(const CirceBoundedObject &B) : CirceObject(B)
{
    copyBounds(B);
}

/**
* \brief Sets BoundUnit codes from a string value.
*/
void CirceBoundedObject::setBoundUnit(std::string _value)
{
    for(int i = 0; i<MU_COUNT; i++) if (_value == MeasureUnitAttributeText[i])
    {
        BoundUnit = (MEASURE_UNIT)i;
        break;
    }
}

/**
* \brief Empties values of the members of a CirceBoundedObject instance.
*/
void CirceBoundedObject::reset()
{
    CirceObject::reset();
    WestBound = EastBound = NorthBound = SouthBound = Area = 0.;
    BoundUnit = MU_UNDEF;
	hasPolygonBound = false;
    polygonBound = NULL;
}

/**
* \brief Copy bounds from a CirceBoundedObject instance.
*/
void CirceBoundedObject::copyBounds(const CirceBoundedObject &B)
{
    WestBound = B.WestBound;
    EastBound = B.EastBound;
    NorthBound = B.NorthBound;
    SouthBound = B.SouthBound;
    Area = B.Area;
    BoundUnit = B.BoundUnit;
	hasPolygonBound = B.hasPolygonBound;
	polygonBound = B.polygonBound;
}

/**
* \brief Checks wether this geographic boundary intersects another CirceBoundedObject instance or not.
* \return true if the geographic intersection is not null.
*/
//WestBound < EastBound ; SouthBound < NorthBound
bool CirceBoundedObject::intersects(const CirceBoundedObject &B)
{
    if (((WestBound >= B.WestBound && WestBound <= B.EastBound)
      || (EastBound >= B.WestBound && EastBound <= B.EastBound)
      || (B.WestBound >= WestBound && B.WestBound <= EastBound)
      || (B.EastBound >= WestBound && B.EastBound <= EastBound))
     && ((SouthBound >= B.SouthBound && SouthBound <= B.NorthBound)
      || (NorthBound >= B.SouthBound && NorthBound <= B.NorthBound)
      || (B.SouthBound >= SouthBound && B.SouthBound <= NorthBound)
      || (B.NorthBound >= SouthBound && B.NorthBound <= NorthBound)))
        return true;
    else
        return false;
}

/**
* \brief Checks wether this CRS geographic boundary intersects another CRS instance or not.
* \return true if the geographic intersection is not null.
*/
//WestBound < EastBound ; SouthBound < NorthBound
bool CirceBoundedObject::CRSintersect(const CirceBoundedObject &B)
{
    if (((WestBound >= B.WestBound && WestBound <= B.EastBound)
      || (EastBound >= B.WestBound && EastBound <= B.EastBound)
      || (B.WestBound >= WestBound && B.WestBound <= EastBound)
      || (B.EastBound >= WestBound && B.EastBound <= EastBound))
     && ((SouthBound >= B.SouthBound && SouthBound <= B.NorthBound)
      || (NorthBound >= B.SouthBound && NorthBound <= B.NorthBound)
      || (B.SouthBound >= SouthBound && B.SouthBound <= NorthBound)
      || (B.NorthBound >= SouthBound && B.NorthBound <= NorthBound)))
        return true;
    else
        return false;
}

/**
* \brief Copies a polygon to this->polygonBound.
* \param[in] _value: a poly*.
*/
void CirceBoundedObject::setPolygonBound(poly* _value)
{
    polygonBound = new poly();
    polygonBound->id = _value->id;
    polygonBound->bound.reserve(_value->bound.size());
    copy(_value->bound.begin(),_value->bound.end(),back_inserter(polygonBound->bound));
}

/**
* \brief Writes the geographic rectangular boundary.
* \param[in] outUnit: a MEASURE_UNIT.
* \return A std::string containing the geographic rectangular boundary in outUnit.
*/
std::string CirceBoundedObject::getValidityArea(MEASURE_UNIT outUnit, std::string frmt)
{
    std::string validityArea="", kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    validityArea += kotes+std::string("ID")+kotes+":"+kotes+getCirceId()+kotes+",";
    validityArea += kotes+std::string("W")+kotes+":"+kotes+asString(UnitConvert(WestBound, MU_RADIAN, outUnit),8)+kotes+",";
    validityArea += kotes+std::string("S")+kotes+":"+kotes+asString(UnitConvert(SouthBound, MU_RADIAN, outUnit),8)+kotes+",";
    validityArea += kotes+std::string("E")+kotes+":"+kotes+asString(UnitConvert(EastBound, MU_RADIAN, outUnit),8)+kotes+",";
    validityArea += kotes+std::string("N")+kotes+":"+kotes+asString(UnitConvert(NorthBound, MU_RADIAN, outUnit),8)+kotes;
//    validityArea += asString(UnitConvert(WestBound, MU_RADIAN, outUnit),8)+",";
//    validityArea += asString(UnitConvert(SouthBound, MU_RADIAN, outUnit),8)+",";
//    validityArea += asString(UnitConvert(EastBound, MU_RADIAN, outUnit),8)+",";
//    validityArea += asString(UnitConvert(NorthBound, MU_RADIAN, outUnit),8)+"}";
    //validityArea += std::string(" (")+MeasureUnitDisplayText[outUnit]+")\n";
    if (frmt == "JSON")
        //validityArea = "\"output\":{"+validityArea+"}";
        validityArea = "{"+validityArea+"}";
    return validityArea;
}

/**
* \brief Checks wether a coordinate set is inside the boundaries or not.
* \return true if it is inside.
*/
bool CirceBoundedObject::isInside(double lon, double lat) const
{
    if (hasPolygonBound)
    {
        if (isInsidePoly(lon, lat, *polygonBound))
            return true;
        else return false;
    }
	if (lon >= WestBound&&lon <= EastBound&&lat <= NorthBound&&lat >= SouthBound) 
		return true;	
	else return false; 
}


//from Alain Harmel
/**
* \brief Checks wether a coordinate set is inside the polygon boundary or not.
* \return true if it is inside.
*/
bool CirceBoundedObject::isInsidePoly(double l, double p, const poly& poly)
{
	double daz = 0, az_p, dl, az, dda;
    double epsilon = 1e-10;
	for (unsigned iPoly = 0; iPoly < poly.bound.size() / 2; iPoly++)
	{
		dl = poly.bound[2 * iPoly] - l;
		//simplify
		az = atan2(sin(dl), -cos(dl)*sin(p) + cos(p)*sin(poly.bound[2 * iPoly + 1]) / cos(poly.bound[2 * iPoly + 1]))*180.0 / M_PI;
		if (iPoly > 0)
		{
			dda = remainder((az - az_p), 360.0);
			daz += dda;
		}
		az_p = az;
	}
    if (daz+epsilon < -350)
		return true;
	else
		return false;
}

/**
* \brief Loads polygons from a file in memory.
* \param[in] polygonFilePath: polygon file path.
* \param[in] pt: a pt4d used if mode == 1.
* \param[in] mode: If mode == 0, load all polygons, pt unused [use findPolygon to search polys in a further step].
* If mode == 1, load polygons (useless ?) until find polygon enclosing pt and return its id.
* \param[out] polys: vector of poly.
* \return a poly id if mode == 1.
*/
std::string CirceBoundedObject::loadPolygons(const std::string& polygonFilePath, std::vector<poly>& polys, 
											const circe::pt4d& pt, int mode)
{
	std::ifstream pfb;
    pfb.open(polygonFilePath.data());
	std::string line0;
	poly plt;
	int nptsall = 0;
	while (!pfb.eof()) 
	{
		if (pfb.eof())
		{
			std::cout << "EOF!" << std::endl;
			break;
		}
		getline(pfb, line0);
		std::string id = line0.substr(0, 2);
		plt.id = id;

		while (!pfb.eof())
		{
			std::string line;
			getline(pfb, line);
			std::string h = line.substr(0, 3);
			if (h == "***"){  //end of polygon
				polys.push_back(plt);
				nptsall += plt.bound.size();
				//std::cout << h << nPoly++ << " " << plt.id << " " << plt.bound.size() << " " << nptsall << std::endl;
				plt.id = "";
				plt.bound.clear();
				if (mode == 1 && isInsidePoly(pt.l, pt.p, plt))
					return plt.id;
				break;
			}
			else
			{
				plt.bound.push_back(atof(line.substr(1, 12).c_str())*M_PI / 180.0);
				plt.bound.push_back(atof(line.substr(14, 12).c_str())*M_PI / 180.0);
			}
		}
	}
	pfb.close();
	return "";
}

/**
* \brief Find polygon enclosing a pt4D and return its id. The polygons have been loaded by loadPolygons with mode=0.
* \param[in] pt: a pt4d.
* \param[int] polys: vector of poly.
* \return a poly id.
*/
std::string CirceBoundedObject::findPolygon(circe::pt4d pt, const std::vector<poly>& polys)
{
	for (int i = 0; i<polys.size(); i++)
		if (isInsidePoly(pt.l, pt.p, polys[i]))
			return polys[i].id;
	return "";
}

} //namespace circe
