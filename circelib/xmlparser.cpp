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
* \file xmlparser.cpp
* \brief Implementation of Circe / Tinyxml2 interface.
*/

#include "xmlparser.h"
#include "units.h"
#include "stringtools.h"
namespace circe
{
//TinyXML
std::string DetectAccents(std::string FichierIn)
{
    char c;
    FILE *fifi;
    if ((fifi=fopen(FichierIn.data(),"r")) == NULL)
        return "Fichier "+FichierIn+" introuvable";
    while (!feof(fifi))
    {
        if ((c=getc(fifi)))
            if (c=='e'||c=='e'||c=='e'||c=='a'||c=='u'||c=='u'||c=='i'||c=='i'||c=='c'||c=='o')
                return "There are accents.\n";
    }
    fclose(fifi);
    return "";
}

/**
* \brief Retrieves a code in a xlink:href value, from IGNF.xml.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \return A std::string containing the code.
*/
std::string ReadHref(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag)
{
    return afterChar(ReadString(_nodeFolder->FirstChildElement(_tag.data()),"xlink:href"),'#');
}

/**
* \brief Retrieves a string from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \return A std::string.
*/
std::string ReadString(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag)
{
    std::string _value="";
    if (!_nodeFolder)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    const char *str = _nodeFolder->Attribute(_tag.data());
    if (str)
        _value.assign(str);
    //Tester _nodeName->GetText() car elle renvoie 0 (et non "") s'il n'y a rien.
    else if ((_nodeName = _nodeFolder->FirstChildElement(_tag.data())) && _nodeName->GetText())
        _value.assign(_nodeName->GetText());
    return _value;
}

/**
* \brief Retrieves an unsigned int from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \return An unsigned int.
*/
unsigned int ReadUnsignedInt(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag)
{
    unsigned int _value=0;
    if (!_nodeFolder)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    if ((_nodeFolder->QueryUnsignedAttribute(_tag.data(),&_value))==tinyxml2::XML_NO_ATTRIBUTE)
    if ((_nodeName = _nodeFolder->FirstChildElement(_tag.data())))
        _nodeName->QueryUnsignedText(&_value);
    return _value;
}

/**
* \brief Retrieves an int from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \return An int.
*/
int ReadInt(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag)
{
    int _value=0;
    if (!_nodeFolder)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    if ((_nodeFolder->QueryIntAttribute(_tag.data(),&_value))==tinyxml2::XML_NO_ATTRIBUTE)
    if ((_nodeName = _nodeFolder->FirstChildElement(_tag.data())))
        _nodeName->QueryIntText(&_value);
    return _value;
}

/**
* \brief Retrieves a bool from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \return A bool.
*/
bool ReadBool(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag)
{
    bool _value=false;
    if (!_nodeFolder)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    if ((_nodeFolder->QueryBoolAttribute(_tag.data(),&_value))==tinyxml2::XML_NO_ATTRIBUTE)
    if ((_nodeName = _nodeFolder->FirstChildElement(_tag.data())))
        _nodeName->QueryBoolText(&_value);
    return _value;
}

/**
* \brief Retrieves a double value and its associated unit of measure from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \param[out] unit: The value unit of measure (uom).
* \return A double.
*/
double ReadDouble(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag, std::string *unit)
{
    double _value=0.;
    if (!_nodeFolder)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    if ((_nodeFolder->QueryDoubleAttribute(_tag.data(),&_value))==tinyxml2::XML_NO_ATTRIBUTE)
    if ((_nodeName = _nodeFolder->FirstChildElement(_tag.data())))
    {
        _nodeName->QueryDoubleText(&_value);
        *unit = DecodeUnit(ReadString(_nodeName,"uom"));
    }
    return _value;
}

/**
* \brief Retrieves and converts a double value from a XML node.
* \param[in] _nodeFolder: A tinyxml2::XMLElement*.
* \param[in] _tag: A XML tag as a std::string.
* \param[in] to_unit: The target value unit of measure.
* \param[out] unit: The value unit of measure (uom) read before conversion.
* \return A double in to_unit.
*/
double ReadConvertDouble(tinyxml2::XMLElement *_nodeFolder, const std::string& _tag,
                         std::string *unit, MEASURE_UNIT to_unit)
{
    double _value=0;
    if (!_nodeFolder)
        return _value;
    MEASURE_UNIT _unit;
    //IGNF
    if (_nodeFolder->FirstChildElement(_tag.data())->FirstChildElement("gco:Decimal"))
    {
        *unit = "DEGREES";
        _value = ReadDouble(_nodeFolder->FirstChildElement(_tag.data()), "gco:Decimal", unit);
        return UnitConvert(_value, MU_DEGREE, to_unit);
    }

    _value = ReadDouble(_nodeFolder, _tag, unit);
    if (*unit == "") return _value;
    _unit=(MEASURE_UNIT)getIndex(*unit,MeasureUnitAttributeText,MU_COUNT);
    return UnitConvert(_value, _unit, to_unit);
}

/**
* \brief Retrieves a double value and the name of a parameter from a XML node.
* \param[in] _SubNode: A tinyxml2::XMLElement*.
* \param[out] parameter_name: A parameter name as a std::string.
* \param[out] unit: The value unit of measure (uom).
* \return A double.
* \remark XML input example:
<gml:ParameterValue uom=\"METERS\" gml:name=\"X0\">%s</gml:ParameterValue>
*/
double ReadParameterValue(tinyxml2::XMLElement *_SubNode,
                            std::string *unit, std::string *parameter_name)
{
    double _value=0.;
    if (!_SubNode)
        return _value;
    *unit = DecodeUnit(ReadString(_SubNode,"uom"));
    *parameter_name = ReadString(_SubNode,"gml:name");
    _SubNode->QueryDoubleText(&_value);
    //double _value = ReadDouble(_SubNode, _tag, unit);
    return _value;
}

/**
* \brief Retrieves a sexagesimal value from a XML node (IGNF.xml).
* \param[in] _SubNode: A tinyxml2::XMLElement*.
* \param[out] unit: The unit of measure = "DMS".
* \return A sexagesimal value in decimal format (DD.MMSSSSSSSS).
* \remark XML input example: \n
      <gml:usesValue> \n
        <gml:ParameterValue> \n
          <gml:dmsAngleValue>     //***************READ FROM THIS NODE************** \n
            <gml:degrees direction="E">166</gml:degrees> \n
            <gml:minutes>26</gml:minutes> \n
            <gml:secondes>33</gml:secondes> \n
          </gml:dmsAngleValue> \n
          <gml:valueOfParameter xlink:href="http://registre.ign.fr/ign/IGNF/parameter/IGNF/PRCP300#PRCP300"/> \n
        </gml:ParameterValue> \n
      </gml:usesValue>
 */
double ReadDMSangleValueValue(tinyxml2::XMLElement *_SubNode, std::string *unit)
{
    double _value=0.;
    if (!_SubNode)
        return _value;
    tinyxml2::XMLElement *_nodeName;
    DMS dms = {0,0,0,0};
    *unit = "DMS";
    const char *str;
    if (_nodeName = _SubNode->FirstChildElement("gml:degrees"))
    {
        _nodeName->QueryIntText(&(dms.deg));
        str = _nodeName->Attribute("direction");
        if (strcmp(str,"S")==0 || strcmp(str,"W")==0)
            dms.neg = true;
        else
            dms.neg = false;
    }
    if (_nodeName = _SubNode->FirstChildElement("gml:minutes"))
        _nodeName->QueryIntText(&(dms.min));
    if (_nodeName = _SubNode->FirstChildElement("gml:secondes"))
        _nodeName->QueryDoubleText(&(dms.sec));
    return dms_to_double(&dms);;
}

}
