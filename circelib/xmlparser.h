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
* \file xmlparser.h
* \brief Definition of Circe / Tinyxml2 interface.
*/

#ifndef XMLPARSER
#define XMLPARSER
#include <string>
#include "tinyxml2/tinyxml2.h"
#include "StringResources.h"

namespace circe
{
std::string DetectAccents(std::string FichierIn);
std::string ReadString(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag);
unsigned int ReadUnsignedInt(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag);
int ReadInt(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag);
bool ReadBool(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag);
double ReadDouble(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag, std::string *unit);
double ReadConvertDouble(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag, std::string *unit, MEASURE_UNIT to_unit);
std::string ReadHref(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag);
double ReadParameterValue(tinyxml2::XMLElement *_SubNode, std::string *unit, std::string *parameter_name);
double ReadDMSangleValueValue(tinyxml2::XMLElement *_SubNode, std::string *unit);
}
#endif // XMLPARSER

