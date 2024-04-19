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
* \file circeobject.h
* \brief Definition of the CirceObject class.
*/

#ifndef CIRCEOBJECT_H
#define CIRCEOBJECT_H
#include <string>
#include "xmlparser.h"
#include "messagebearer.h"

namespace circe
{
/**\brief The CirceObject class is the base class of all Circe objects.*/
class CirceObject
{
private:
	/**\brief Object name*/
	std::string Name;

	/**\brief Main Object id*/
	std::string CirceId;

	/**\brief EPSG id*/
	std::string EPSGid;

	/**\brief Synonym of CirceId*/
    std::string IGNid;

public:
    /**\brief Holds intermediate execution message*/
    MessageBearer *mb;

public:
    CirceObject();
    CirceObject(const CirceObject &B) ; // copy constructor
    ~CirceObject();
    void setCirceId(std::string _value) {CirceId=_value;}
    void setIGNid(std::string _value) {CirceId=IGNid=_value;}
    void setName(std::string _value) {Name=_value;}
    void setEPSGid(std::string _value) {EPSGid=_value;}
    //std::string getCirceId() const {return CirceId;}
    std::string getCirceId() const {return IGNid;}
    std::string getIGNid() const {return IGNid;}
    std::string getName() const {return Name;}
    std::string getEPSGid() const {return EPSGid;}
    void parseNames(tinyxml2::XMLElement *elem);
    virtual void reset();

};

} //namespace circe

#endif // CIRCEOBJECT_H
