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
* \file circeobject.cpp
* \brief Implementation of the CirceObject class.
*/

#include "circeobject.h"
#include "stringtools.h"

namespace circe
{

/**
* \brief Constructor.
*/
CirceObject::CirceObject()
{
    mb = NULL;
}

/**
* \brief  Copy constructor.
*/
CirceObject::CirceObject(const CirceObject &B) 
{
    Name = B.Name;
    CirceId = B.CirceId;
    EPSGid = B.EPSGid;
    IGNid = B.IGNid;
    mb = B.mb;
}

CirceObject::~CirceObject()
{
}

/**
* \brief Empties values of the members of a CirceObject instance.
*/
void CirceObject::reset()
{
    Name = CirceId = EPSGid = IGNid = "";
    mb = NULL;
}

/* without codeSpace, authority is IGNF (IGNF.xml or legacy metadata file)
   e.g.
        <gml:name>RGF93 CC42 zone 1</gml:name>
        <gml:name codeSpace="OGP">urn:ogc:def:crs:EPSG::3942</gml:name>
        <gml:name codeSpace="OGP">urn:ogc:def:crs:EPSG::310024142</gml:name>
   !!! The last <gml:name codeSpace="OGP"> would erase the previous one !!!!
 */
/*
        <gml:name>ED50 UTM fuseau 30</gml:name>
        <gml:name>http://registre.ign.fr/ign/IGNF/crs/IGNF/UTM30</gml:name>
        <gml:name>http://registre.ign.fr/ign/IGNF/crs/IGNF/UTM30ED50</gml:name>
   !!! The last <gml:name> would erase the previous ones !!!!
 */
/*
    So always keep the first value ...
 */
/**
* \brief Initializes the Name and EPSGid members of a CirceObject instance.
* \param[in] elem: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
*/
void CirceObject::parseNames(tinyxml2::XMLElement *elem)
{
    tinyxml2::XMLElement *elem2;
    std::string name, cdspc;
    for (elem2 = elem->FirstChildElement("gml:name");
         elem2 ;
         elem2 = elem2->NextSiblingElement("gml:name") )
    {
        if (elem2->GetText())
            name.assign(elem2->GetText());
        else name = "";
        cdspc = ReadString(elem2,"codeSpace");
        if (cdspc == "")
        {
            if (getName() == "") //keep the first value
                setName(name);
        }
        else if (cdspc == "OGP")
        {
            if (getEPSGid() == "") //keep the first value
                setEPSGid(afterChar(name,':'));
        }
    }
}

} //namespace circe
