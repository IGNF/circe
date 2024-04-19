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
* \file metadata.cpp
* \brief Implementation of Circe geodetic metadata writing and loading. 
* Three metadata file formats are considered.
* - The Circe Legacy Metadata format (files usually named DataXXX.txt where XXX refers to a geographic area).
*   It is intended to be easilly written "by hand". It can be loaded by Circe, not written.
* - The Circe v5 native XML format. It can be written from a legacy file, possibly edited "by hand", and loaded by Circe.
* - IGN.xml.
*
* \remark IGNF and EPSG codes to be checked (not fully automated).
*/

#include <fstream>
#include <sys/stat.h>
#include <string>
#include <algorithm> //std::transform
#include "metadata.h"
#include "xmlparser.h"

namespace circe
{

/**
* \brief Writes transformation parameters as a string.
* \param[in] tsg: A TSG instance.
* \return A std::string containing the comma-separated transformation parameters.
*/
std::string initValuesString(const TSG& tsg)
{
    std::string values;
    values = asString(tsg.tx)+","+asString(tsg.ty)+","+asString(tsg.tz)+","+asString(tsg.ech);
    values += ","+asString(tsg.rx)+","+asString(tsg.ry)+","+asString(tsg.rz);
    values += ","+asString(tsg.dtx)+","+asString(tsg.dty)+","+asString(tsg.dtz)+","+asString(tsg.dech);
    values += ","+asString(tsg.drx)+","+asString(tsg.dry)+","+asString(tsg.drz);
    return values;
}

/**
* \brief Calculates the number of transformation parameters of a TSG (questionable reliability).
* \param[in] tsg: A TSG instance.
* \return The number of transformation parameters.
*/
int initValuesNumber(const TSG& tsg)
{
    int values_number = 0;
    if (tsg.tx!=0 || tsg.ty!=0 || tsg.tz!=0) values_number += 3;
    if (tsg.ech!=0)
        values_number += 1;
    if (tsg.rx!=0 || tsg.ry!=0 || tsg.rz!=0)
        values_number += 3;
    if ( tsg.dtx!=0 || tsg.dty!=0 || tsg.dtz!=0 || tsg.dech!=0
      || tsg.drx!=0 || tsg.dry!=0 || tsg.drz!=0)
            values_number = 14;
    if (values_number == 0) //e.g. ITRF2008 to ITRF2008 transformation
        values_number = 14;
    return values_number;
}

/**
* \brief Reads the rectangular geographic boundaries from a tinyxml2 element and writes them in the CirceBoundedObject members.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] cbo: A CirceBoundedObject* (practically, one of its child classes) whose boundaries are set and converted into radians.
*/
void readBounds(tinyxml2::XMLElement *_EntityNode, CirceBoundedObject *cbo)
{
    tinyxml2::XMLElement *_EntityNode2;
    std::string unit="";
    //sub-tags for IGNF
    if (_EntityNode->FirstChildElement("gml:domainOfValidity"))
    {
            _EntityNode2 = _EntityNode->FirstChildElement("gml:domainOfValidity");
        if (_EntityNode2->FirstChildElement("gmd:EX_Extent"))
            _EntityNode2 = _EntityNode2->FirstChildElement("gmd:EX_Extent");
        if (_EntityNode2->FirstChildElement("gmd:geographicElement"))
            _EntityNode2 = _EntityNode2->FirstChildElement("gmd:geographicElement");
        if (_EntityNode2->FirstChildElement("gmd:EX_GeographicBoundingBox"))
            _EntityNode2 = _EntityNode2->FirstChildElement("gmd:EX_GeographicBoundingBox");
    }
    else
        _EntityNode2 = _EntityNode;
    if (!(_EntityNode2->FirstChildElement("gmd:westBoundLongitude")))
        return;
    cbo->setWestBound(ReadConvertDouble(_EntityNode2,"gmd:westBoundLongitude",&unit,MU_RADIAN));
    cbo->setEastBound(ReadConvertDouble(_EntityNode2,"gmd:eastBoundLongitude",&unit,MU_RADIAN));
    cbo->setSouthBound(ReadConvertDouble(_EntityNode2,"gmd:southBoundLatitude",&unit,MU_RADIAN));
    cbo->setNorthBound(ReadConvertDouble(_EntityNode2,"gmd:northBoundLatitude",&unit,MU_RADIAN));
    cbo->setBoundUnit(MeasureUnitAttributeText[MU_RADIAN]);
    cbo->setArea();
}

/**
* \brief Reads ellipsoid parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vEllipsoid contains a new element.
* \throw str A std:string containing the error message if any.
*/
void LoadEllipsoid(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::Ellipsoid ell;
    std::string unit;
    ell.reset();
    ell.setIGNid(ReadString(_EntityNode,"gml:id"));
    ell.parseNames(_EntityNode);
    ell.set_a(ReadDouble(_EntityNode,"gml:semiMajorAxis",&unit)); //unit without dimension
    //Everything following is a "secondDefiningParameter" but circe XML does not include
    //this tag. IGNF does.
    if (_EntityNode->FirstChildElement("gml:secondDefiningParameter"))
        _EntityNode = _EntityNode->FirstChildElement("gml:secondDefiningParameter");
    //Yes, twice (with uppercase S)
    if (_EntityNode->FirstChildElement("gml:SecondDefiningParameter"))
        _EntityNode = _EntityNode->FirstChildElement("gml:SecondDefiningParameter");
    //circe XML and IGNF.
    ell.set_b(ReadDouble(_EntityNode,"gml:semiMinorAxis",&unit)); //unit without dimension
    ell.set_e2(ReadDouble(_EntityNode,"circe:squareEccentricity",&unit)); //unit without dimension
    ell.set_f(ReadDouble(_EntityNode,"gml:inverseFlattening",&unit)); //unit without dimension
    //Circe uses a and e2
    if (ell.get_e2() == 0.)
    {
        if (ell.get_b() != 0)
            ell.b__e2();
        else if (ell.get_f() != 0)
        {
            ell.set_f(1/ell.get_f());
            ell.f__e2();
        }
        else if (_EntityNode->FirstChildElement("gml:isSphere"))
            ell.set_e2(0.);
        else throw ell.getName()+" ellipsoid without second defining parameter.";
    }
    GS->vEllipsoid.push_back(ell);
}

/**
* \brief Reads prime meridian parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vMeridian contains a new element.
*/
void LoadPrimeMeridian(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::Meridian mer;
    std::string unit;
    mer.reset();
    mer.setIGNid(ReadString(_EntityNode,"gml:id"));
    mer.parseNames(_EntityNode);
    mer.setFromGreenwich(ReadConvertDouble(_EntityNode,"gml:greenwichLongitude",&unit,MU_RADIAN));
    GS->vMeridian.push_back(mer);
}

/**
* \brief Reads geodetic reference system parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vGeodeticSystem contains a new element.
*/
void LoadgeodeticReferenceSystem(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::GeodeticReferenceSystem grs;
    grs.reset();
    grs.setIGNid(ReadString(_EntityNode,"gml:id"));
    grs.parseNames(_EntityNode);
    grs.setSystemDimension("3D"); //default value for IGNF.xml
    grs.setSystemDimension(ReadString(_EntityNode,"circe:dimension"));
    //Bounds only for circe XML (IGNF sets bounds in CRSs and transformations).
    readBounds(_EntityNode, &grs);
    grs.setPrimeMeridianId(ReadHref(_EntityNode,"gml:usesPrimeMeridian"));
    grs.setEllipsoidId(ReadHref(_EntityNode,"gml:usesEllipsoid"));
    grs.setUsesForTransformation(ReadHref(_EntityNode,"circe:usesForTransformation"));
    GS->vGeodeticSystem.push_back(grs);
}

/**
* \brief Reads geodetic reference frame parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vGeodeticFrame contains a new element.
*/
void LoadgeodeticReferenceFrame(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::GeodeticReferenceFrame grf;
    grf.reset();
    grf.setIGNid(ReadString(_EntityNode,"gml:id"));
    grf.parseNames(_EntityNode);
    if (_EntityNode->FirstChildElement("gml:usesReferenceSystem"))
        grf.setReferenceSystem(ReadHref(_EntityNode,"gml:usesReferenceSystem"));
    else //IGNF
        grf.setReferenceSystem(ReadString(_EntityNode,"gml:id"));
    GS->vGeodeticFrame.push_back(grf);

	if (ReadInt(_EntityNode, "circe:isKeyGeodeticFrame") == 1)
		GS->setKeyGeodeticFrame(grf.getCirceId());
}

/**
* \brief Reads geocentric CRS parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vCRS contains a new element.
*/
void LoadGeocentricCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::CRS crs;
    crs.reset();
    crs.setIGNid(ReadString(_EntityNode,"gml:id"));
    crs.parseNames(_EntityNode);
    //crs.setStatus(ReadString(_EntityNode,"circe:status"));
    //Bounds only for IGNF (circe XML sets bounds in systems).
    if (_EntityNode->FirstChildElement("gml:domainOfValidity"))
        readBounds(_EntityNode, &crs);
    if (_EntityNode->FirstChildElement("circe:usesReferenceFrame"))
        crs.setReferenceFrameId(ReadHref(_EntityNode,"circe:usesReferenceFrame"));
    else if (_EntityNode->FirstChildElement("gml:usesGeodeticDatum")) //IGNF
        crs.setReferenceFrameId(ReadHref(_EntityNode,"gml:usesGeodeticDatum"));
    crs.setCRStype(CT_GEOCENTRIC);
    GS->vCRS.push_back(crs);
}

/**
* \brief Reads geographic CRS parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vCRS contains a new element.
*/
void LoadGeographicCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::CRS crs;
    crs.reset();
    crs.setIGNid(ReadString(_EntityNode,"gml:id"));
    crs.parseNames(_EntityNode);
    //crs.setStatus(ReadString(_EntityNode,"circe:status"));
    //Bounds only for IGNF (circe XML sets bounds in systems).
    if (_EntityNode->FirstChildElement("gml:domainOfValidity"))
        readBounds(_EntityNode, &crs);
    if (_EntityNode->FirstChildElement("circe:usesReferenceFrame"))
        crs.setReferenceFrameId(ReadHref(_EntityNode,"circe:usesReferenceFrame"));
    else if (_EntityNode->FirstChildElement("gml:usesGeodeticDatum")) //IGNF
        crs.setReferenceFrameId(ReadHref(_EntityNode,"gml:usesGeodeticDatum"));
    crs.setCRStype(CT_GEOGRAPHIC);
    GS->vCRS.push_back(crs);
}

/**
* \brief Reads vertical reference system parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vVerticalSystem contains a new element.
*/
void LoadverticalReferenceSystem(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::VerticalReferenceSystem vrs;
    //Emprise = celle de la grille de transfo verticale
    vrs.reset();
    vrs.setIGNid(ReadString(_EntityNode,"gml:id"));
	//Bounds only for circe XML (IGNF sets bounds in CRSs and transformations).
	readBounds(_EntityNode, &vrs);
	vrs.parseNames(_EntityNode);
    GS->vVerticalSystem.push_back(vrs);
}

/**
* \brief Reads vertical reference frame parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vVerticalFrame contains a new element.
*/
void LoadverticalReferenceFrame(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::VerticalReferenceFrame vrf;
    vrf.reset();
    vrf.setIGNid(ReadString(_EntityNode,"gml:id"));
    vrf.parseNames(_EntityNode);
    if (_EntityNode->FirstChildElement("gml:usesVerticalSystem"))
        vrf.setReferenceSystem(ReadHref(_EntityNode,"gml:usesVerticalSystem"));
    else //IGNF
        vrf.setReferenceSystem(ReadString(_EntityNode,"gml:id"));
    GS->vVerticalFrame.push_back(vrf);
}

/**
* \brief Reads vertical CRS parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vCRS contains a new element.
*/
void LoadVerticalCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::CRS crs;
    crs.reset();
    crs.setIGNid(ReadString(_EntityNode,"gml:id"));
    crs.parseNames(_EntityNode);
    //crs.setStatus(ReadString(_EntityNode,"circe:status"));
    //Bounds only for IGNF (circe XML sets bounds in systems).
    if (_EntityNode->FirstChildElement("gml:domainOfValidity"))
        readBounds(_EntityNode, &crs);
    if (_EntityNode->FirstChildElement("circe:usesReferenceFrame"))
        crs.setReferenceFrameId(ReadHref(_EntityNode,"circe:usesReferenceFrame"));
    else if (_EntityNode->FirstChildElement("gml:usesVerticalDatum")) //IGNF
        crs.setReferenceFrameId(ReadHref(_EntityNode,"gml:usesVerticalDatum"));
    crs.setCRStype(CT_VERTICAL);
    GS->vCRS.push_back(crs);
}

/**
* \brief Initializes a vector of operation parameters (IGNF only).
* \param[in] _CirceNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] vOpams: id/name pairs initialized.
* \remark parameters names
- projection: LAMBDA0 (*2), PHI0 (*2), PHI1, PHI2, X0 (*2), Y0 (*2), K0 (*2)
- transformation similarity: TX, TY, TZ, ECH_D6, EXS, EYS, EZS
- transformation grid: GRILLE
*/
void LoadOperationParameterNames(std::vector<operationParameter> *vOpams, tinyxml2::XMLElement *_CirceNode)
{
    tinyxml2::XMLElement *_EntityNode, *_EntityNode2;
    operationParameter op;
    for (_EntityNode = _CirceNode->FirstChildElement("parameters");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("parameters") )
    {
        if (_EntityNode->FirstChildElement("gml:OperationParameter"))
            _EntityNode2 = _EntityNode->FirstChildElement("gml:OperationParameter");
        else
            return;
        op.id = op.name = "";
        op.id = ReadString(_EntityNode2,"gml:id");
        op.name = ReadString(_EntityNode2,"gml:name");
        vOpams->push_back(op);
    }
}

/**
* \brief Sets operation parameter values (IGNF only).
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] vOpams: values set.
*/
void LoadOperationParameters(std::vector<operationParameter> *vOpams, tinyxml2::XMLElement *_EntityNode)
{
    tinyxml2::XMLElement *_SubNode, *_SubNode2;
    std::string unit, parameter_name, vop;
    std::vector<circe::operationParameter>::iterator itOpam;
    for(itOpam=vOpams->begin();itOpam<vOpams->end();itOpam++)
    {
        itOpam->value = 0.;
        itOpam->unit = itOpam->valueFile = "";
    }
    if (!(_EntityNode->FirstChildElement("gml:usesValue")))
        return;
    for (_SubNode = _EntityNode->FirstChildElement("gml:usesValue"); _SubNode ;
        _SubNode = _SubNode->NextSiblingElement("gml:usesValue") )
    {
        _SubNode2 = _SubNode->FirstChildElement("gml:ParameterValue");
        vop = ReadHref(_SubNode2,"gml:valueOfParameter");
        for(itOpam=vOpams->begin();itOpam<vOpams->end();itOpam++)
            if (itOpam->id == vop)
                break;
        if (itOpam == vOpams->end())
            throw std::string("valueOfParameter doesn't match (should not occur):"+vop+":\n");
        if (_SubNode2->FirstChildElement("gml:value"))
        {
            itOpam->value = ReadParameterValue(_SubNode2->FirstChildElement("gml:value"),&unit,&parameter_name);
            itOpam->unit = unit;
        }
        else if (_SubNode2->FirstChildElement("gml:dmsAngleValue"))
        {
            itOpam->value = ReadDMSangleValueValue(_SubNode2->FirstChildElement("gml:dmsAngleValue"),&unit);
            itOpam->unit = unit;
        }
        if (_SubNode2->FirstChildElement("gml:valueFile"))
        itOpam->valueFile = ReadString(_SubNode2,"gml:valueFile");
    }
}

/**
* \brief Returns the parameter value from its input name (IGNF only).
* \param[in] paramname: name of the parameter.
* \param[in] vOpams: vector of operationParameter.
* \param[in] nParam: number of operation parameters being read (also output).
* \return Parameter value.
*/
double selectOpam(std::string paramname, std::vector<operationParameter> *vOpams, int *nParam)
{
    std::vector<circe::operationParameter>::iterator itOpam;
    for(itOpam=vOpams->begin();itOpam<vOpams->end();itOpam++)
        if (itOpam->name == paramname)
        {
            (*nParam)++;
            return itOpam->value;
        }
    return 0.;
}

/**
* \brief Reads frame transformation parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[in] vOpams: vector of operationParameter.
* \param[out] GS: vector GS->vGeodeticTransfo or GS->vVerticalTransfo contains a new element.
*/
void LoadFrameTransformation(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode,
                             std::vector<operationParameter> *vOpams)
{
    circe::Transformation rft;
    std::vector<circe::operationParameter>::iterator itOpam;
    std::string values, methodId;
//    std::string paramNames[7]={"TX", "TY", "TZ", "ECH_D6", "EXS", "EYS", "EZS"};
    TSG tsg;
    int nParam;
    struct stat buffer;
    rft.reset();
    rft.setIGNid(ReadString(_EntityNode,"gml:id"));
    rft.parseNames(_EntityNode);
    readBounds(_EntityNode, &rft);

    if (_EntityNode->FirstChildElement("gml:usesValue"))
    //IGNF only (or another test)
    //supposing all transformations are authoritative and all grids have tac (IGN) format
    {
        rft.setStatus("AUTHORITATIVE");
        rft.setReversibility("REVERSE_SAME_PARAMETERS"); //or DIRECT
        rft.setEpoch("0");

        rft.setSourceFrameId(ReadHref(_EntityNode,"gml:sourceCRS")); //IGNF, will be replaced by frame id
        rft.setTargetFrameId(ReadHref(_EntityNode,"gml:targetCRS")); //IGNF, will be replaceb by frame id
        //Precision code
        rft.setbPrecisionCode("NO");
        rft.setPrecisionCodeInit("0");
        //Transformation Parameters
        LoadOperationParameters(vOpams, _EntityNode);
        nParam = 0;
        values = "";
        for(itOpam=vOpams->begin();itOpam<vOpams->end();itOpam++)
            if (itOpam->name.compare(0,6,"GRILLE")==0 && itOpam->valueFile != "")
                break;
        if (itOpam<vOpams->end())
        {
            //Grid file with old extension "mnt" or "txt" (same circe grid format):
            //suppression of full path and new extension "tac".
            rft.setGridFile(getFilename(itOpam->valueFile)+".tac");
            rft.setMetadadaFileName(replaceExtension(rft.getGridFile(),std::string("xml")));
            rft.setGridType("Circe ascii");
            rft.setNodeFrameInfo("TARGET");
            rft.setEncoding("ASCII");
        }
        else
        {
//            for (int iParam=0;iParam<7;iParam++)
//                values += asString(selectOpam(paramNames[iParam], vOpams, &nParam))+",";
            tsg.tx = tsg.ty = tsg.tz = tsg.ech = tsg.rx = tsg.ry = tsg.rz = 0.;
            tsg.dtx = tsg.dty = tsg.dtz = tsg.dech = tsg.drx = tsg.dry = tsg.drz = 0.;
            tsg.tx = selectOpam("TX", vOpams, &nParam);
            tsg.ty = selectOpam("TY", vOpams, &nParam);
            tsg.tz = selectOpam("TZ", vOpams, &nParam);
            tsg.ech = selectOpam("ECH_D6", vOpams, &nParam);
            tsg.rx = selectOpam("EXS", vOpams, &nParam);
            tsg.ry = selectOpam("EYS", vOpams, &nParam);
            tsg.rz = selectOpam("EZS", vOpams, &nParam);
            values = initValuesString(tsg);
            nParam = initValuesNumber(tsg);
            rft.setLayout("CONSTANT");
        }
        rft.setValuesNumber(asString(nParam));
        rft.setValueUnit("SECONDS"); //MODIF
        rft.setInitValues(values);

        methodId = ReadHref(_EntityNode,"gml:usesMethod");
        if (methodId == "Geographic3DtoGravityRelatedHeight_IGN")
        {
            rft.setCrsType("VERTICAL");
            if (rft.getGridFile() != "" )
            {
                rft.setValuesNumber(asString(1));
                rft.setInitValues("0");//no init values in IGNF.xml
            }
        }
        else
        {
            rft.setCrsType("GEOCENTRIC");
            if (rft.getGridFile() != "" && rft.getGridFile() != "UNDEFINED")
            {
                rft.setValuesNumber(asString(3));
                rft.setInitValues("0,0,0");//no init values in IGNF.xml
            }
        }
    }
    else
    {
		rft.setDescription(ReadString(_EntityNode, "circe:description"));
		rft.setStatus(ReadString(_EntityNode, "circe:status"));
		rft.setReversibility(ReadString(_EntityNode, "circe:reversibility"));
        rft.setEpoch(ReadString(_EntityNode,"circe:epoch"));
        rft.setSourceFrameId(ReadString(_EntityNode,"circe:source_frame"));
        rft.setEpochF(ReadString(_EntityNode,"circe:source_epoch"));
        rft.setTargetFrameId(ReadString(_EntityNode,"circe:target_frame"));
        rft.setEpochT(ReadString(_EntityNode,"circe:target_epoch"));
        rft.setLayout(ReadString(_EntityNode,"circe:layout"));
        rft.setEncoding(ReadString(_EntityNode,"circe:ENDIANNESS"));
        //Grid
        rft.setGridFile(ReadString(_EntityNode,"circe:data_file_name"));
        rft.setGridType(ReadString(_EntityNode,"circe:file_type"));
        rft.setMetadadaFileName(ReadString(_EntityNode,"circe:metadata_file_name"));
        rft.setNodeFrameId(ReadString(_EntityNode,"circe:node_frame"));
        rft.setNodeFrameInfo(ReadString(_EntityNode,"circe:node_frame_info"));
        //Precision code
        rft.setbPrecisionCode(ReadString(_EntityNode,"circe:precision_code"));
        rft.setPrecisionCodeInit(ReadString(_EntityNode,"circe:precision_code_init"));
        //Transformation Parameters
        rft.setValuesNumber(ReadString(_EntityNode,"circe:values_number"));
        //unit of the rotation angles BEFORE init values (with unit conversion)
        rft.setValueUnit(ReadString(_EntityNode, "circe:values_unit"));
        rft.setInitValues(ReadString(_EntityNode,"circe:init_values"));

        rft.setCrsType(ReadString(_EntityNode,"circe:crs_type"));
    }

    // Load grid metadata (mainly to initialize boundaries)
    if (rft.getGridFile() != "" && rft.getGridFile() != "UNDEFINED")
    {
        rft.setGridFile(GS->metadataPath+rft.getGridFile());
        if (stat (rft.getGridFile().c_str(), &buffer) == 0)
        {
            rft.LoadGrid(LD_HEADER);
            rft.setArea();
        }
        //ntv2: sourceFrameId and targetFrameId are recorded in the grid header but their
        //values are not likely to be compliant with the metadata file being parsed.
        //So it reloaded here.
        rft.setSourceFrameId(ReadString(_EntityNode,"circe:source_frame"));
        rft.setEpochF(ReadString(_EntityNode,"circe:source_epoch"));
        rft.setTargetFrameId(ReadString(_EntityNode,"circe:target_frame"));
        rft.setEpochT(ReadString(_EntityNode,"circe:target_epoch"));
    }

    if (rft.getCRStype() == CT_GEOCENTRIC
     || rft.getCRStype() == CT_GEOGRAPHIC)
        GS->vGeodeticTransfo.push_back(rft);
    //else if (rft.getCRStype() == CT_GEOGRAPHIC)
        //GS->vGeodeticTransfo.push_back(rft);
    else if (rft.getCRStype() == CT_VERTICAL)
        GS->vVerticalTransfo.push_back(rft);
}

/**
* \brief Reads conversion parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[in] vOpams: vector of operationParameter.
* \param[out] GS: vector GS->vConversion contains a new element.
*/
void LoadConversion(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode, std::vector<operationParameter> *vOpams)
{
    tinyxml2::XMLElement *_SubNode;
    circe::Conversion conv;
    std::string unit, parameter_name;
    std::vector<circe::operationParameter>::iterator itOpam;
    double value;
    conv.reset();
    conv.setIGNid(ReadString(_EntityNode,"gml:id"));
    conv.parseNames(_EntityNode);
    conv.setInitParamMethodId(ReadHref(_EntityNode,"gml:usesMethod"));
    conv.setApplicationMethodId(ReadHref(_EntityNode,"gml:usesMethod"));
    conv.setApplicationInvMethodId(ReadHref(_EntityNode,"gml:usesMethod"));
    if (_EntityNode->FirstChildElement("gml:ParameterValue")) //circe XML
    {
        for (_SubNode = _EntityNode->FirstChildElement("gml:ParameterValue"); _SubNode ;
            _SubNode = _SubNode->NextSiblingElement("gml:ParameterValue") )
        {
            value = ReadParameterValue(_SubNode,&unit,&parameter_name);
            conv.setParameter(parameter_name,value,unit);
        }
    }
    else if (_EntityNode->FirstChildElement("gml:usesValue")) //IGNF
    {
        LoadOperationParameters(vOpams, _EntityNode);
        //some parameter names are set twice but only one is initialized
        for(itOpam=vOpams->begin();itOpam<vOpams->end();itOpam++)
             conv.setParameter(itOpam->name,itOpam->value,itOpam->unit);
    }
    conv.setPrimeMeridianId(ReadHref(_EntityNode,"gml:usesPrimeMeridian"));
    GS->vConversion.push_back(conv);
}

/**
* \brief Reads projected CRS parameters from a tinyxml2 element and writes them in a GeodeticSet instance.
* \param[in] _EntityNode: A tinyxml2::XMLElement* stemming from a Circe v5 XML file or IGNF.xml.
* \param[out] GS: vector GS->vCRS contains a new element.
*/
void LoadProjectedCRS(circe::GeodeticSet *GS, tinyxml2::XMLElement *_EntityNode)
{
    circe::CRS crs;
    crs.reset();
    crs.setIGNid(ReadString(_EntityNode,"gml:id"));
    crs.parseNames(_EntityNode);
    //crs.setStatus(ReadString(_EntityNode,"circe:status"));
    readBounds(_EntityNode, &crs);
    crs.setConversionId(ReadHref(_EntityNode,"gml:definedByConversion"));
    crs.setBaseGeographicCRSid(ReadHref(_EntityNode,"gml:baseGeographicCRS"));
    if (_EntityNode->FirstChildElement("circe:usesReferenceFrame"))
        crs.setReferenceFrameId(ReadHref(_EntityNode,"circe:usesReferenceFrame"));
    crs.setCRStype(CT_PROJECTED);
    GS->vCRS.push_back(crs);
}

/**
* \brief Loads a Circe v5 native XML metadata file into a GeodeticSet instance.
* \param[in] infile: A Circe v5 XML file path.
* \param[out] GS: A GeodeticSet instance initialized.
*/
void LoadCirceXMLmetadata(const std::string& infile, circe::GeodeticSet *GS)
{
    std::ifstream f(infile.c_str());
    if (!f.good())
    {
        f.close();
		return;
	}
    GS->reset();
//	if ((Message=DetectAccents(infile)) != "")
    //	return;
    tinyxml2::XMLDocument CirceXMLmetadata;
    tinyxml2::XMLElement *_CirceNode, *_EntityNode;
    tinyxml2::XMLError err_load = CirceXMLmetadata.LoadFile(infile.data());
	if (err_load)
        throw std::string(infile+":\n"+CirceXMLmetadata.ErrorName());
    std::vector<operationParameter> vOpams; //IGNF only

    _CirceNode = CirceXMLmetadata.FirstChildElement("CIRCE");
	if (!_CirceNode)
        return ;//"Tag not found : \n<CIRCE>";

    _CirceNode = _CirceNode->FirstChildElement("CT_CrsCatalogue");
    if (!_CirceNode)
        return ;//"Tag not found : \n<CT_CrsCatalogue>";

    for (_EntityNode = _CirceNode->FirstChildElement("gml:Ellipsoid");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:Ellipsoid") )
    {
        LoadEllipsoid(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:PrimeMeridian");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:PrimeMeridian") )
    {
        LoadPrimeMeridian(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("circe:geodeticReferenceSystem");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("circe:geodeticReferenceSystem") )
    {
        LoadgeodeticReferenceSystem(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("circe:geodeticReferenceFrame");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("circe:geodeticReferenceFrame") )
    {
        LoadgeodeticReferenceFrame(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:GeocentricCRS");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:GeocentricCRS") )
    {
        LoadGeocentricCRS(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:GeographicCRS");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:GeographicCRS") )
    {
        LoadGeographicCRS(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("circe:verticalReferenceSystem");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("circe:verticalReferenceSystem") )
    {
        LoadverticalReferenceSystem(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("circe:verticalReferenceFrame");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("circe:verticalReferenceFrame") )
    {
        LoadverticalReferenceFrame(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:VerticalCRS");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:VerticalCRS") )
    {
        LoadVerticalCRS(GS, _EntityNode);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("circe:frameTransformation");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("circe:frameTransformation") )
    {
        LoadFrameTransformation(GS, _EntityNode, &vOpams);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:Conversion");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:Conversion") )
    {
        LoadConversion(GS, _EntityNode, &vOpams);
    }
    for (_EntityNode = _CirceNode->FirstChildElement("gml:ProjectedCRS");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("gml:ProjectedCRS") )
    {
        LoadProjectedCRS(GS, _EntityNode);
    }
	//for (my $i=0; $i<=$#{TYPCOOR}; $i++) 
	//{
		//print "TYPCOOR:\t".${TYPCOOR}[$i]->{REG};
		//foreach my $co (split/\s+/,${TYPCOOR}[$i]->{COOR})
		//{
			//print "\t".$co;
		//}
		//print "\n";
	//}
	//for (my $i=0; $i<=$#{UNIT}; $i++) 
	//{
		//print "UNIT:\t".${UNIT}[$i]->{REG};
		//foreach my $un (split/\s+/,${UNIT}[$i]->{UNIT})
		//{
			//print "\t".$un;
		//}
		//print "\n";
	//}
}

/**
* \brief Loads IGFN.xml into a GeodeticSet instance.
* \param[in] infile: IGFN.xml file path.
* \param[out] GS: A GeodeticSet instance initialized.
*/
void LoadIGNFxmlMetadata(const std::string& infile, circe::GeodeticSet *GS)
{
    std::ifstream f(infile.c_str());
    if (!f.good())
    {
        f.close();
        return;
    }
    GS->reset();
//	if ((Message=DetectAccents(infile)) != "")
    //	return;
    tinyxml2::XMLDocument IGNFxmlMetadata;
    tinyxml2::XMLElement *IGNFnode, *_EntityNode, *_EntityNode2;
    tinyxml2::XMLError err_load = IGNFxmlMetadata.LoadFile(infile.data());
    if (err_load)
        throw std::string(infile+":\n"+IGNFxmlMetadata.ErrorName());
    std::vector<operationParameter> vOpams;

    IGNFnode = IGNFxmlMetadata.FirstChildElement("CT_CrsCatalogue");
    if (!IGNFnode)
        return ;//"Tag not found : \n<CT_CrsCatalogue>";

    for (_EntityNode = IGNFnode->FirstChildElement("ellipsoid");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("ellipsoid") )
    if (_EntityNode->FirstChildElement("gml:Ellipsoid"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:Ellipsoid");
        LoadEllipsoid(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("primeMeridian");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("primeMeridian") )
    if (_EntityNode->FirstChildElement("gml:PrimeMeridian"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:PrimeMeridian");
        LoadPrimeMeridian(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("datum");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("datum") )
    if (_EntityNode->FirstChildElement("gml:GeodeticDatum"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:GeodeticDatum");
        LoadgeodeticReferenceSystem(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("datum");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("datum") )
    if (_EntityNode->FirstChildElement("gml:GeodeticDatum"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:GeodeticDatum");
        LoadgeodeticReferenceFrame(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("crs");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("crs") )
    if (_EntityNode->FirstChildElement("gml:GeocentricCRS"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:GeocentricCRS");
        LoadGeocentricCRS(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("crs");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("crs") )
    if (_EntityNode->FirstChildElement("gml:GeographicCRS"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:GeographicCRS");
        LoadGeographicCRS(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("datum");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("datum") )
    if (_EntityNode->FirstChildElement("gml:VerticalDatum"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:VerticalDatum");
        LoadverticalReferenceSystem(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("datum");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("datum") )
    if (_EntityNode->FirstChildElement("gml:VerticalDatum"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:VerticalDatum");
        LoadverticalReferenceFrame(GS, _EntityNode2);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("crs");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("crs") )
    if (_EntityNode->FirstChildElement("gml:VerticalCRS"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:VerticalCRS");
        LoadVerticalCRS(GS, _EntityNode2);
    }
    LoadOperationParameterNames(&vOpams, IGNFnode); //IGNF only
    for (_EntityNode = IGNFnode->FirstChildElement("operation");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("operation") )
    if (_EntityNode->FirstChildElement("gml:Transformation"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:Transformation");
        LoadFrameTransformation(GS, _EntityNode2, &vOpams);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("operation");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("operation") )
    if (_EntityNode->FirstChildElement("gml:Conversion"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:Conversion");
        LoadConversion(GS, _EntityNode2, &vOpams);
    }
    for (_EntityNode = IGNFnode->FirstChildElement("crs");
         _EntityNode ;
         _EntityNode = _EntityNode->NextSiblingElement("crs") )
    if (_EntityNode->FirstChildElement("gml:ProjectedCRS"))
    {
        _EntityNode2 = _EntityNode->FirstChildElement("gml:ProjectedCRS");
        LoadProjectedCRS(GS, _EntityNode2);
    }
}

/**
* \brief Loads a Circe Legacy Metadata File into a LEGACY_METADATA_SET instance.
* \param[in] filedata: Circe Legacy Metadata file path.
* \param[out] lms: A LEGACY_METADATA_SET instance initialized.
* \throw a std:string containing the error message (if any).
*/
void LoadCirceLegacyMetadataFile(const std::string filedata, LEGACY_METADATA_SET *lms)
{
	ELG elg;
    LGO lgo;
	REG reg;
	REA rea;
	TSG0 tsg0;
	TSG tsg;
	TSV tsv;
	PRC prc;
	PROJ proj;
	GR1D gr1d;
	GR3D gr3d;
	ZNE zne;
    PMM pmm;
    GRMM grmm;
    TYPCOOR typcoor;
	UNIT unit;
	
	std::string hem[2] = { "N", "S" };
	double y0[2] = { 0., 10000000. };
	double latmin[2] = { 0., -90. };
	double latmax[2] = { 90., 0. };

	bool found = false;
	char seps[] = " \t";
    std::ifstream fi;
    fi.open(filedata.data(),std::ios::in);
	if (!fi.is_open()) 
        throw std::string(ArgumentValidationText[AV_BAD_METADATAFILENAME])+": "+filedata;
    char lili[_MAX_PATH_], *token;
	std::string version="V4";
    while (fi.getline(lili,_MAX_PATH_,'\n'))
	{
		if ( (token = strtok(lili, " ")) == NULL) 
			continue;
		//strstr to find "CIRCE" as a substring of token since the file may begin with a BOM
        if (!found && (strcmp(token, CIRCE_NAME.data())==0 || strstr(token, CIRCE_NAME.data())!=NULL
                    || strcmp(token, CIRCE_ALT_NAME.data())==0 || strstr(token, CIRCE_ALT_NAME.data())!=NULL))
        {
			if ((token = strtok(NULL, seps)) != NULL) version.assign(token);
			RemoveCR(&version);
			found = true;
		}
		else if (strcmp(token, "KEYREG") == 0)
		{
			if ((token = strtok(NULL, seps)) != NULL) lms->key_REG.assign(token);
			RemoveCR(&lms->key_REG);
		}
		else if (strcmp(token, "ELG") == 0)
		{
			//ELG num a b 1/f e2 name
			//ELG 001 6378388.0000 6356911.9461 297.000000 .006722670022 Hayford 1909 (International)
			//struct ELG {std::string num; double a; double b; double rf; double es; std::string name;};
			elg.num = elg.name = "";
			elg.a = elg.b = elg.rf = elg.es = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) elg.num.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&elg.a);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&elg.b);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&elg.rf);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&elg.es);
            if ( (token = strtok(NULL, seps)) != NULL) elg.name.assign(token);
			CompleteString(&elg.name);
			lms->vELG.push_back(elg);
		}
        else if (strcmp(token,"LGO")==0)
		{
            //LGO num unite value name
            //LGO 01 01   0.          Greenwich
            //struct LGO {std::string num; std::string unite; double value; std::string name;} lgo;
            lgo.num = lgo.unit = lgo.name = "";
            lgo.value = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) lgo.num.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) lgo.unit.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&lgo.value);
            if ( (token = strtok(NULL, seps)) != NULL) lgo.name.assign(token);
            CompleteString(&lgo.name);
            lms->vLG0.push_back(lgo);
		}
		else if (strcmp(token,"REG")==0)
		{
            //V4
            //REG num elg lgo dimension name
            //REG 002 010 02	2D	NTF (Paris)
            //V5
            //REG num elg lgo dimension short_name name
            //REG 002 010 02	2D	NTFP NTF (Paris)
            //struct REG {std::string num; std::string ELG; std::string LGO; std::string dimension; std::string name;} reg;
            reg.num = reg.ELG = reg.LGO = reg.dimension = reg.short_name = reg.name = "";
            if ( (token = strtok(NULL, seps)) != NULL) reg.num.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) reg.ELG.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) reg.LGO.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) reg.dimension.assign(token);
            if (version == "V5")
                if ( (token = strtok(NULL, seps)) != NULL) reg.short_name.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) reg.name.assign(token);
            if (version == "V4")
            {
                reg.short_name = reg.name;
                RemoveCR(&reg.short_name);
            }
            CompleteString(&reg.name);
            lms->vREG.push_back(reg);
		}
		else if (strcmp(token,"REA")==0)
		{
			//REA num Nom
			//REA 003 IGN69
			//struct REA {std::string num; std::string name;} rea;
            rea.num = rea.short_name = rea.name = "";
            if ( (token = strtok(NULL, seps)) != NULL) rea.num.assign(token);
            if (version == "V5")
                if ( (token = strtok(NULL, seps)) != NULL) rea.short_name.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) rea.name.assign(token);
            if (version == "V4")
            {
                rea.short_name = rea.name;
                RemoveCR(&rea.short_name);
            }
			CompleteString(&rea.name);
			lms->vREA.push_back(rea);
		}
		else if (strcmp(token,"TSG0")==0)
		{
			//TSG0 reg1 reg2 TX0 TY0 TZ0
			//TSG0  002 024 -168.     -60.     320.
			//struct TSG0 {std::string REG1; std::string REG2; double tx0; double ty0; double tz0} tsg0;
			tsg0.REG1 = tsg0.REG2 = "";
			tsg0.tx0 = tsg0.ty0 = tsg0.tz0 = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) tsg0.REG1.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) tsg0.REG2.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg0.tx0);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg0.ty0);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg0.tz0);
			lms->vTSG0.push_back(tsg0);
		}
		else if (strcmp(token,"TSG")==0)
		{
			//V4
			//TSG num reg1 reg2 prec Tx  Ty  Tz  Ech  Rx  Ry  Rz
			//TSG 10061 002 101 5  -84.      37.     437.    0         0         0         0
			//V5
			//TSG num reg1 reg2 prec Tx  Ty  Tz  Ech  Rx  Ry  Rz dTx  dTy  dTz  dEch  dRx  dRy  dRz Epoque
			//TSG 10797 423 706 3 -95.593 573.763 173.442 42.6265 -.9602 1.2510 -1.3918 0 0 0 0 0 0 0 0 0
			//TSG 99999 555 706 1 -0.00089 -0.00088 -0.02089 0.001820 0.000035 -0.000662 -0.000100 -0.00041 -0.00022 -0.00041 0 -0.000035 0.000662 0.000100 2005.0 0
			//struct TSG {std::string num; std::string REG1; std::string REG2; int prec; 
			//double tx; double ty; double tz; double ech; double rx; double ry; double rz;
			//double dtx; double dty; double dtz; double dech; double drx; double dry; double drz;
            //double epoch;} tsg;
			tsg.num = tsg.REG1 = tsg.REG2 = "";
			tsg.prec = 0;
            tsg.epoch = tsg.tx = tsg.ty = tsg.tz = tsg.ech = tsg.rx = tsg.ry = tsg.rz = 0.;
			tsg.dtx = tsg.dty = tsg.dtz = tsg.dech = tsg.drx = tsg.dry = tsg.drz = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) tsg.num.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) tsg.REG1.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) tsg.REG2.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%d",&tsg.prec);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.tx);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.ty);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.tz);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.ech);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.rx);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.ry);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.rz);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.dtx);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.dty);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.dtz);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.dech);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.drx);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.dry);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.drz);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&tsg.epoch);
			if (tsg.REG1 != tsg.REG2) //to prevent from transfos like I08/I08 that would make the operation fail
				lms->vTSG.push_back(tsg);
		}
		else if (strcmp(token, "TSV") == 0)
		{
            //Premier format caduc mais accepté si dT = Epoque = 0
            //TSV num reg_cible rea_cible rea_source prec T dT Epoque
            //TSV IGN69versEVRF2000 024 122 003 99 -0.486 0 0
            //Nouveau format pour formule de basculement
            //TSV num reg_cible rea_cible rea_source prec a1(m) lon0(deg) lat0(deg) a2(sec) a3(sec)
			tsv.num = tsv.REG = tsv.REA1 = tsv.REA2 = "";
			tsv.prec = 0;
            tsv.a1 = tsv.a2 = tsv.a3 = tsv.lon0 = tsv.lat0 = 0.;
			if ((token = strtok(NULL, seps)) != NULL) tsv.num.assign(token);
			if ((token = strtok(NULL, seps)) != NULL) tsv.REG.assign(token);
			if (atof(tsv.REG.data()) == 0) tsv.REG = "";
			if ((token = strtok(NULL, seps)) != NULL) tsv.REA2.assign(token);
			if (atof(tsv.REA2.data()) == 0) tsv.REA2 = "";
			if ((token = strtok(NULL, seps)) != NULL) tsv.REA1.assign(token);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%d", &tsv.prec);
            if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &tsv.a1);
            if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &tsv.lon0);
            if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &tsv.lat0);
            if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &tsv.a2);
            if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &tsv.a3);
            tsv.a2 = UnitConvert(tsv.a2, MU_SECOND , MU_RADIAN);
            tsv.a3 = UnitConvert(tsv.a3, MU_SECOND , MU_RADIAN);
            tsv.lon0 = UnitConvert(tsv.lon0, MU_DEGREE , MU_RADIAN);
            tsv.lat0 = UnitConvert(tsv.lat0, MU_DEGREE , MU_RADIAN);
            lms->vTSV.push_back(tsv);
		}
		else if (strcmp(token,"PRC")==0)
		{
			//V4
			//PRC num mode elg  X0  Y0   Lgo   unit  l0  j0  delta  j1  j2  c  type name
			//PRC 0101 2 010  600000.      200000.     02 03  0.  55.     0.99987734  0     0      1 1  Lambert 1 Nord
			//V5
			//PRC num mode  X0 Y0  Lgo  unit  l0  j0  delta  j1  j2  c  type name
            //PRC 0221 0  500000.  0. 01 10 -57.  0. 0.9996   0   0   0 2  UTM Nord fuseau 21
            //struct PRC {std::string num; int mode; double x0; double y0; std::string LGO; std::string unite_ang;
			//double l0; double j0; double delta; double j1; double j2; double c; int type; std::string name;} prc;
            prc.num = prc.LGO = prc.unit = prc.name = "";
			prc.mode = prc.type = 0;
			prc.x0 = prc.y0 = prc.l0 = prc.j0 = prc.delta = prc.j1 = prc.j2 = prc.c = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) prc.num.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%d",&prc.mode);
			if (prc.num == "UTM" && prc.mode == 999999) //defines the 120 UTM projection methods
			{
				prc.mode = 0;
				prc.unit = "02"; //degrees
				prc.LGO = "01";
				prc.x0 = 500000.;
				prc.j0 = prc.j1 = prc.j2 = prc.c = 0.;
				prc.delta = 0.9996;
				prc.type = 2;
				for (int iZone = 1; iZone <= 60; iZone++)
				{
					for (int iHem = 0; iHem < 2; iHem++)
					{
						prc.num = "UTM" + asString(iZone)+hem[iHem];
						prc.name = "UTM " + asString(iZone) + " " + hem[iHem];
						prc.y0 = y0[iHem];
						prc.l0 = 6*(iZone-31)+3.; //degrees
						lms->vPRC.push_back(prc);
					}
				}
			}
			else
			{
				if (version == "V4") token = strtok(NULL, seps); //ELG
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.x0);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.y0);
				if ((token = strtok(NULL, seps)) != NULL) prc.LGO.assign(token);
				if ((token = strtok(NULL, seps)) != NULL) prc.unit.assign(token);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.l0);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.j0);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.delta);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.j1);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.j2);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &prc.c);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%d", &prc.type);
				if ((token = strtok(NULL, seps)) != NULL) prc.name.assign(token);
				CompleteString(&prc.name);
				lms->vPRC.push_back(prc);
			}
		}
		else if (strcmp(token, "PROJ") == 0)
		{
			//V4
			//PROJ reg prc unite lonMin latMin lonMax latMax
			//PROJ 002 101 03 -7.1 53.5 11.0 57.0
			//V5
			//PROJ reg prc unite lonMin latMin lonMax latMax short_name name
			//PROJ 002 101 03 -7.1 53.5 11.0 57.0
			//struct PROJ {std::string REG; std::string PRC; std::string unite; double lonMin; double latMin;
			//double lonMax; double latMax; std::string name;} proj;
			proj.REG = proj.PRC = proj.unit = proj.short_name = proj.name = "";
			proj.lonMin = proj.latMin = proj.lonMax = proj.latMax = 0.;
			if ((token = strtok(NULL, seps)) != NULL) proj.REG.assign(token);
			if ((token = strtok(NULL, seps)) != NULL) proj.PRC.assign(token);
			if (proj.PRC == "UTM")
			{
				std::string regname="";
				if ((token = strtok(NULL, seps)) != NULL)
					regname.assign(token);
				else
					regname.assign(proj.REG);
				proj.unit = "02"; //degrees
				for (int iZone = 1; iZone <= 60; iZone++)
				{
					for (int iHem = 0; iHem < 2; iHem++)
					{
						proj.PRC = "UTM" + asString(iZone) + hem[iHem];
						proj.lonMin = 6 * (iZone - 31);
						proj.lonMax = 6 * (iZone - 30);
						proj.latMin = latmin[iHem];
						proj.latMax = latmax[iHem];
						proj.short_name = regname + proj.PRC;
						proj.name = "UTM " + asString(iZone) + hem[iHem];
						lms->vPROJ.push_back(proj);
					}
				}
			}
			else
			{
				if ((token = strtok(NULL, seps)) != NULL) proj.unit.assign(token);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &proj.lonMin);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &proj.latMin);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &proj.lonMax);
				if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &proj.latMax);
				if ((token = strtok(NULL, seps)) != NULL) proj.short_name.assign(token);
				if ((token = strtok(NULL, seps)) != NULL) proj.name.assign(token);
				CompleteString(&proj.name);
				lms->vPROJ.push_back(proj);
			}
		}
		else if (strcmp(token,"GR1D")==0)
		{
			//GR1D "filename" reg [rea]...[rea]
            //GR1D raf09.tac 024 003
            gr1d.filename = gr1d.REG = gr1d.REA1 = gr1d.REA2 = "";
            if ( (token = strtok(NULL, seps)) != NULL) gr1d.filename.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) gr1d.REG.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) gr1d.REA1.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) gr1d.REA2.assign(token);
            RemoveCR(&gr1d.REA1);
            RemoveCR(&gr1d.REA2);
            lms->vGR1D.push_back(gr1d);
		}
		else if (strcmp(token,"GR3D")==0)
		{
			//GR3D "filename" reg1 reg2
            //GR3D gr3df97a.tac 002 024
            //reg1 and/or reg2 may include epoch of a 4D frame
            //GR3D example_NC.tac I08@2015 547
            gr3d.filename = gr3d.REG1 = gr3d.REG2 = "";
            if ( (token = strtok(NULL, seps)) != NULL) gr3d.filename.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) gr3d.REG1.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) gr3d.REG2.assign(token);
            CompleteString(&gr3d.REG2); //to remove the final \r
            lms->vGR3D.push_back(gr3d);
		}
		else if (strcmp(token,"ZNE")==0)
		{
			//ZNE reg unite lonMin latMin lonMax latMax
			//ZNE 002 03 -7.1 45.9 11.0 57.0
			zne.REG = zne.unit = "";
			zne.lonMin = zne.latMin = zne.lonMax = zne.latMax = 0.;
            if ( (token = strtok(NULL, seps)) != NULL) zne.REG.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) zne.unit.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&zne.lonMin);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&zne.latMin);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&zne.lonMax);
            if ( (token = strtok(NULL, seps)) != NULL) sscanf(token,"%lf",&zne.latMax);
			lms->vZNE.push_back(zne);
		}
        else if (strcmp(token,"PMM")==0)
        {
			//PMM reg num dTx(m / y) dTy(m / y) dTz(m / y) dRx(mas / y) dRy(mas / y) dRz(mas / y) name
			//PMM I08 AM  0.00041  0.00022  0.00041 -0.190 -0.442     0.915      Amur
            pmm.num = pmm.REG = pmm.name = "";
			pmm.dtx = pmm.dty = pmm.dtz = pmm.drx = pmm.dry = pmm.drz = 0.;
            if ((token = strtok(NULL, seps)) != NULL) pmm.REG.assign(token);
            if ((token = strtok(NULL, seps)) != NULL) pmm.num.assign(token);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.dtx);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.dty);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.dtz);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.drx);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.dry);
			if ((token = strtok(NULL, seps)) != NULL) sscanf(token, "%lf", &pmm.drz);
			if ((token = strtok(NULL, seps)) != NULL) pmm.name.assign(token);
            CompleteString(&pmm.name);
            lms->vPMM.push_back(pmm);
        }
        else if (strcmp(token,"GRMM")==0)
        {
            //GRMM "filename" reg
            grmm.filename = grmm.REG = "";
            if ( (token = strtok(NULL, seps)) != NULL) grmm.filename.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) grmm.REG.assign(token);
            CompleteString(&grmm.REG); //to remove the final \r
            lms->vGRMM.push_back(grmm);
        }
        else if (strcmp(token,"TYPCOOR")==0)
		{
			//TYPCOOR reg [01:Cartesiennes] [02:Geographiques] [03:PLanes]
			//TYPCOOR 002 02 03
			typcoor.REG = typcoor.COOR = "";
            if ( (token = strtok(NULL, seps)) != NULL) typcoor.REG.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) typcoor.COOR.assign(token);
			CompleteString(&typcoor.COOR);
			lms->vTYPCOOR.push_back(typcoor);
		}
		else if (strcmp(token,"UNIT")==0)
		{
			//UNIT reg [unite]...[unite]
			//UNIT 002 03
            unit.REG = unit._UNIT = "";
            if ( (token = strtok(NULL, seps)) != NULL) unit.REG.assign(token);
            if ( (token = strtok(NULL, seps)) != NULL) unit._UNIT.assign(token);
            CompleteString(&unit._UNIT);
			lms->vUNIT.push_back(unit);
		}
	}
	fi.close();
    for (std::vector<REG>::iterator itREG=lms->vREG.begin(); itREG<lms->vREG.end(); itREG++)
    {
        for (std::vector<LGO>::iterator itLG0=lms->vLG0.begin(); itLG0<lms->vLG0.end(); itLG0++)
            if (itLG0->num == itREG->LGO)
            {
                itREG->from_greenwich=UnitConvert(itLG0->value, MEASURE_UNIT(getIndex(DecodeUnit(itLG0->unit),
                                           MeasureUnitAttributeText, MU_COUNT)), MU_RADIAN);
                break;
            }
    }

    return;
}

/**
* \brief Creates an id from a string.
* \param[in] str:  A std::string file path.
* \return str uppercased.
*/
std::string createID(std::string str)
{
    str = getFilename(str);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

/**
* \brief Decode (id and epoch) a frame id from a string.
* \param[in] str: A std::string frame id, generally "id" only and sometimes "id@epoch".
* \param[out] epoch: Pointer to a double, generally 0.
* \return id without epoch.
*/
std::string decodeID(const std::string& str, double *epoch)
{
    std::string str2;
    *epoch = 0.;
    size_t pos = std::string::npos;
    if ((pos = str.find("@")) == std::string::npos)
        return str;
    str2 = str.substr(0,pos);
    *epoch = atof(str.substr(pos+1).data());
    return str2;
}

/**
* \brief Writes a Circe v5 native XML file from a LEGACY_METADATA_SET structure.
* \param[in] outfile: Circe v5 native XML file path.
* \param[in] lms: A LEGACY_METADATA_SET instance.
*/
void WriteXMlfromCirceLegacyMetadata(const std::string outfile, const LEGACY_METADATA_SET& lms)
{
    FILE *fo;
    if ((fo=fopen(outfile.data(),"w")) == NULL)
		return;

    std::vector<ELG>::const_iterator itELG;
    std::vector<LGO>::const_iterator itLG0;
    std::vector<REG>::const_iterator itREG;
    std::vector<REA>::const_iterator itREA;
    std::vector<TSG0>::const_iterator itTSG0;
	std::vector<TSG>::const_iterator itTSG, itTSG2;
	std::vector<TSV>::const_iterator itTSV;
    std::vector<PMM>::const_iterator itPMM;
    std::vector<GRMM>::const_iterator itGRMM;
    std::vector<PRC>::const_iterator itPRC;
    std::vector<PROJ>::const_iterator itPROJ;
    std::vector<GR1D>::const_iterator itGR1D;
    std::vector<GR3D>::const_iterator itGR3D;
    std::vector<ZNE>::const_iterator itZNE;
//    std::vector<TYPCOOR>::const_iterator itTYPCOOR;
//    std::vector<UNIT>::const_iterator itUNIT;
	
    std::string name, num, unit, elg, lgo, num1, num2, values, prcm, prcnum, rvrsblty, metadatafilename;
    std::string nodeframe, nodeframeinfo, reg1, reg2;
	int iLG0, iLG0arr[2], nLG0, isKeyGeodeticFrame;
    char str4LG0[_MAX_PATH_], str2LG0[_MAX_PATH_], zonestring[2*_MAX_PATH_];
    std::string nameLG0[2], iniLGO[2], zoneUnit[2], numREG[2], strUsesForTransformation, str4LGOgreeenwich="0001";
    //double from_greenwich;
    double epoch1, epoch2;

    fprintf (fo,"\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<CIRCE>\n\
<CT_CrsCatalogue xmlns:gml=\"http://www.opengis.net/gml\" xmlns:gmd=\"http://www.isotc211.org/2005/gmd\">\n");

    if (lms.vELG.size()==0) throw (std::string("WriteXMlfromCirceLegacyMetadata: vELG.size()==0"));
    for (itELG=lms.vELG.begin(); itELG<lms.vELG.end(); itELG++)
	{
		num = "ELG"+itELG->num; 
        fprintf (fo,"\
<gml:Ellipsoid gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/ellipsoid/IGNF/%s</gml:identifier>\n\
    <gml:name>%s</gml:name>\n\
    <gml:name codeSpace=\"OGP\"></gml:name>\n\
    <gml:semiMajorAxis uom=\"METERS\">%.15lf</gml:semiMajorAxis>\n\
    <gml:semiMinorAxis uom=\"METERS\">%.15lf</gml:semiMinorAxis>\n\
    <circe:squareEccentricity>%.15lf</circe:squareEccentricity>\n\
    <gml:inverseFlattening>%.15lf</gml:inverseFlattening>\n\
</gml:Ellipsoid>\n", num.data(), num.data(), itELG->name.data(), itELG->a, itELG->b, itELG->es, itELG->rf);
	}
    if (lms.vLG0.size()==0) throw (std::string("WriteXMlfromCirceLegacyMetadata: vLG0.size()==0"));
    for (itLG0=lms.vLG0.begin(); itLG0<lms.vLG0.end(); itLG0++)
	{
        num = "LGO"+itLG0->num;
		unit = DecodeUnit(itLG0->unit);
        fprintf (fo,"\
<gml:PrimeMeridian gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/meridian/IGNF/%s</gml:identifier>\n\
    <gml:name>%s</gml:name>\n\
    <gml:name codeSpace=\"OGP\"></gml:name>\n\
    <gml:greenwichLongitude uom=\"%s\">%.15lf</gml:greenwichLongitude>\n\
</gml:PrimeMeridian>\n", num.data(), num.data(), itLG0->name.data(), unit.data(), itLG0->value);
	}
	//If the prime meridian of a frame is not Greenwich, two frames are implemented:
	//one with the native meridian, another one with Greenwich. The Greenwich one is
    //supposed to be a mandatory intermediate for frame transformation with WGS84-like systems.
    //So the non-Greenwich one as a "usesReferenceSystem" = the Greenwich one.
	//Greenwich meridian num is "01" or "001" or something like that.
    if (lms.vREG.size()==0) throw (std::string("WriteXMlfromCirceLegacyMetadata: vREG.size()==0"));
    for (itREG=lms.vREG.begin(); itREG<lms.vREG.end(); itREG++)
	{
        double lonMin[2], latMin[2], lonMax[2], latMax[2];
        isKeyGeodeticFrame = itREG->num == lms.key_REG ? 1 : 0;
        elg = "ELG"+itREG->ELG;
        zoneUnit[0] = zoneUnit[1] = "";
        if (lms.vZNE.size()>0)
        {
            for (itZNE=lms.vZNE.begin(); itZNE<lms.vZNE.end(); itZNE++)
                if (itREG->num == itZNE->REG)
            {
                zoneUnit[0] = zoneUnit[1] = DecodeUnit(itZNE->unit);
                lonMin[0] = lonMin[1] = itZNE->lonMin;
                latMin[0] = latMin[1] = itZNE->latMin;
                lonMax[0] = lonMax[1] = itZNE->lonMax;
                latMax[0] = latMax[1] = itZNE->latMax;
                break;
            }
            if (itZNE==lms.vZNE.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: REG "+itREG->short_name+": ZNE")+ErrorMessageText[EM_NOT_FOUND]);
        }
        else if (lms.vPROJ.size()>0)
        {
            for (itPROJ=lms.vPROJ.begin(); itPROJ<lms.vPROJ.end(); itPROJ++)
                if (itREG->num == itPROJ->REG)
            {
                zoneUnit[0] = zoneUnit[1] = DecodeUnit(itPROJ->unit);
                lonMin[0] = lonMin[1] = itPROJ->lonMin;
                latMin[0] = latMin[1] = itPROJ->latMin;
                lonMax[0] = lonMax[1] = itPROJ->lonMax;
                latMax[0] = latMax[1] = itPROJ->latMax;
                break;
            }
            if (itPROJ==lms.vPROJ.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: REG "+itREG->short_name+": PROJ")+ErrorMessageText[EM_NOT_FOUND]);
        }
        else
            throw (std::string("WriteXMlfromCirceLegacyMetadata: vZNE.size()==lms.vPROJ.size()==0"));
        MEASURE_UNIT zu = MEASURE_UNIT(getIndex(zoneUnit[1], MeasureUnitAttributeText, MU_COUNT));
        iLG0arr[0]=1;
		nLG0=1;
		nameLG0[0]="Greenwich";
        iniLGO[1]="";
        sscanf(itREG->LGO.data(),"%d",&iLG0);
        if (iLG0 != 1)
		{
			iLG0arr[1]=iLG0;
			nLG0=2;
            for (itLG0=lms.vLG0.begin(); itLG0<lms.vLG0.end(); itLG0++)
                if (itLG0->num == itREG->LGO)
				{
					nameLG0[1]=itLG0->name;
                    iniLGO[1] = itLG0->name[0];
                    //from_greenwich=UnitConvert(itLG0->value, MEASURE_UNIT(getIndex(DecodeUnit(itLG0->unit),
                      //                         MeasureUnitAttributeText, MU_COUNT)), MU_RADIAN);
                    lonMin[1] = UnitConvert(lonMin[1], zu, MU_RADIAN) - itREG->from_greenwich;
                    lonMax[1] = UnitConvert(lonMax[1], zu, MU_RADIAN) - itREG->from_greenwich;
                    latMin[1] = UnitConvert(latMin[1], zu , MU_RADIAN);
                    latMax[1] = UnitConvert(latMax[1], zu , MU_RADIAN);
                    zoneUnit[1] = "RADIANS";
                    break;
				}
            if (itLG0==lms.vLG0.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: REG "+itREG->short_name+": LGO")+ErrorMessageText[EM_NOT_FOUND]);
        }
		for (int i=0; i<nLG0; i++)
		{
            std::string nana = itREG->name;
            if (nLG0>1) nana += " "+nameLG0[i];
			sprintf(str4LG0,"%04d",iLG0arr[i]);
			sprintf(str2LG0,"%02d",iLG0arr[i]);
			num = "REG"+itREG->num+str4LG0;
			numREG[i] = num;
			if (i==0) strUsesForTransformation = num; //"Greenwich" frame, reused for i=1
            lgo = "LGO"+asString(str2LG0);

            fprintf (fo,"\
<circe:geodeticReferenceSystem gml:id=\"%s\">\n\
    <gml:name>%s</gml:name>\n\
    <circe:dimension>%s</circe:dimension>\n\
    <gmd:westBoundLongitude uom=\"%s\">%.15lf</gmd:westBoundLongitude>\n\
    <gmd:eastBoundLongitude uom=\"%s\">%.15lf</gmd:eastBoundLongitude>\n\
    <gmd:southBoundLatitude uom=\"%s\">%.15lf</gmd:southBoundLatitude>\n\
    <gmd:northBoundLatitude uom=\"%s\">%.15lf</gmd:northBoundLatitude>\n\
    <gml:usesPrimeMeridian xlink:href=\"%s\"/>\n\
    <gml:usesEllipsoid xlink:href=\"%s\"/>\n\
    <circe:usesForTransformation xlink:href=\"%s\"/>\n\
</circe:geodeticReferenceSystem>\n", num.data(), nana.data(), itREG->dimension.data(),
                    zoneUnit[i].data(), lonMin[i], zoneUnit[i].data(), lonMax[i],
                    zoneUnit[i].data(), latMin[i], zoneUnit[i].data(), latMax[i],
                    lgo.data(), elg.data(), strUsesForTransformation.data());

            fprintf (fo,"\
<circe:geodeticReferenceFrame gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/datum/IGNF/%s</gml:identifier>\n\
    <gml:name>%s</gml:name>\n\
	<circe:isKeyGeodeticFrame>%d</circe:isKeyGeodeticFrame>\n\
    <gml:name codeSpace=\"OGP\"></gml:name>\n\
    <gml:usesReferenceSystem xlink:href=\"%s\"/>\n\
</circe:geodeticReferenceFrame>\n", num.data(), num.data(), nana.data(), isKeyGeodeticFrame, num.data());

            if (itREG->dimension == "3D" || itREG->dimension == "4D")
            fprintf (fo,"\
<gml:GeocentricCRS gml:id=\"%s%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/crs/IGNF/TO_BE_DEFINED</gml:identifier>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <gml:name>geocentric</gml:name>\n\
    <gml:name codeSpace=\"OGP\">TO_BE_DEFINED</gml:name>\n\
    <circe:crs_type>GEOCENTRIC</circe:crs_type>\n\
    <circe:usesReferenceFrame xlink:href=\"%s\"/>\n\
</gml:GeocentricCRS>\n", itREG->short_name.data(), iniLGO[i].data(), num.data());

            fprintf (fo,"\
<gml:GeographicCRS gml:id=\"%s%sG\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/crs/IGNF/TO_BE_DEFINED</gml:identifier>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <gml:name>geographic</gml:name>\n\
    <gml:name codeSpace=\"OGP\">TO_BE_DEFINED</gml:name>\n\
    <circe:crs_type>GEOGRAPHIC</circe:crs_type>\n\
    <circe:usesReferenceFrame xlink:href=\"%s\"/>\n\
</gml:GeographicCRS>\n", itREG->short_name.data(), iniLGO[i].data(), num.data());


        }
//		if (nLG0==2)
//		{
//            values = asString(from_greenwich,15)+",0,0";
//            fprintf (fo,"\
//<circe:frameTransformation>\n\
//    <circe:status>AUTHORITATIVE</circe:status>\n\
//    <circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
//    <circe:crs_type>GEOGRAPHIC</circe:crs_type>\n\
//    <circe:source_frame>%s</circe:source_frame>\n\
//    <circe:target_frame>%s</circe:target_frame>\n\
//    <circe:values_number>3</circe:values_number>\n\
//    <circe:precision_code>YES</circe:precision_code>\n\
//    <circe:init_values>%s</circe:init_values>\n\
//    <circe:precision_code_init>0</circe:precision_code_init>\n\
//    <circe:layout>CONSTANT</circe:layout>\n\
//</circe:frameTransformation>\n", numREG[0].data(), numREG[1].data(), values.data());
//		}
	}
    for (itREA=lms.vREA.begin(); itREA<lms.vREA.end(); itREA++)
	{
        double lonMin=0., latMin=0., lonMax=0., latMax=0.;
        num = "REA"+itREA->num;
		if (lms.vZNE.size()>0)
		{
			for (itZNE = lms.vZNE.begin(); itZNE<lms.vZNE.end(); itZNE++)
				if (itREA->num == itZNE->REG)
				{
					unit = DecodeUnit(itZNE->unit);
					lonMin = itZNE->lonMin;
					latMin = itZNE->latMin;
					lonMax = itZNE->lonMax;
					latMax = itZNE->latMax;
					break;
				}
			//if (itZNE == lms.vZNE.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: REA " + itREA->short_name + ": ZNE") + ErrorMessageText[EM_NOT_FOUND]);
		}
		fprintf(fo, "\
<circe:verticalReferenceSystem gml:id=\"%s\">\n\
    <gml:name>%s</gml:name>\n\
    <gmd:westBoundLongitude uom=\"%s\">%.15lf</gmd:westBoundLongitude>\n\
    <gmd:eastBoundLongitude uom=\"%s\">%.15lf</gmd:eastBoundLongitude>\n\
    <gmd:southBoundLatitude uom=\"%s\">%.15lf</gmd:southBoundLatitude>\n\
    <gmd:northBoundLatitude uom=\"%s\">%.15lf</gmd:northBoundLatitude>\n\
</circe:verticalReferenceSystem>\n", num.data(), itREA->name.data(),
				unit.data(), lonMin, unit.data(), lonMax,
				unit.data(), latMin, unit.data(), latMax);

        fprintf (fo,"\
<circe:verticalReferenceFrame gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">%s</gml:identifier>\n\
    <gml:name>%s</gml:name>\n\
    <gml:name codeSpace=\"OGP\"></gml:name>\n\
    <gml:usesVerticalSystem xlink:href=\"%s\"/>\n\
</circe:verticalReferenceFrame>\n", num.data(), num.data(), itREA->name.data(), num.data());

        fprintf (fo,"\
<gml:VerticalCRS gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/crs/IGNF/TO_BE_DEFINED</gml:identifier>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <gml:name>vertical</gml:name>\n\
    <gml:name codeSpace=\"OGP\">TO_BE_DEFINED</gml:name>\n\
    <circe:crs_type>VERTICAL</circe:crs_type>\n\
    <circe:usesReferenceFrame xlink:href=\"%s\"/>\n\
</gml:VerticalCRS>\n", itREA->short_name.data(), num.data());

    }
    for (itGR3D=lms.vGR3D.begin(); itGR3D<lms.vGR3D.end(); itGR3D++)
	{
        reg1 = itGR3D->REG1;
        reg1 = decodeID(reg1, &epoch1);
        num1 = "REG"+reg1+str4LGOgreeenwich;
        reg2 = itGR3D->REG2;
        reg2 = decodeID(reg2, &epoch2);
        num2 = "REG"+reg2+str4LGOgreeenwich;
        num = createID(itGR3D->filename);
        //metadatafilename is the same as metadatafilename with ".xml" extension
        metadatafilename=replaceExtension(itGR3D->filename,std::string("xml"));
        values="0,0,0";
        for (itTSG0=lms.vTSG0.begin(); itTSG0<lms.vTSG0.end(); itTSG0++)
			if (itGR3D->REG1==itTSG0->REG1 && itGR3D->REG2==itTSG0->REG2)
                values = asString(itTSG0->tx0)+","+asString(itTSG0->ty0)+","+asString(itTSG0->tz0);
        fprintf (fo,"\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
    <circe:crs_type>GEOCENTRIC</circe:crs_type>\n\
    <circe:data_file_name>%s</circe:data_file_name>\n\
    <circe:metadata_file_name>%s</circe:metadata_file_name>\n\
    <circe:file_type></circe:file_type>\n\
    <circe:values_number>3</circe:values_number>\n\
    <circe:source_frame>%s</circe:source_frame>\n\
    <circe:source_epoch>%lf</circe:source_epoch>\n\
    <circe:target_frame>%s</circe:target_frame>\n\
    <circe:target_epoch>%lf</circe:target_epoch>\n\
    <circe:node_frame>UNDEFINED</circe:node_frame>\n\
    <circe:node_frame_info>TARGET</circe:node_frame_info>\n\
    <circe:init_values>%s</circe:init_values>\n\
</circe:frameTransformation>\n", num.data(), itGR3D->filename.data(), metadatafilename.data(),
                                num1.data(), epoch1, num2.data(), epoch2, values.data());
	}
    for (itTSG=lms.vTSG.begin(); itTSG<lms.vTSG.end(); itTSG++)
	{
        reg1 = itTSG->REG1;
        reg1 = decodeID(reg1, &epoch1);
        reg2 = itTSG->REG2;
        reg2 = decodeID(reg2, &epoch2);

        //The geographic extent of some constant transformations may be defined
        //with the ZNE keyword (not mandatory).
        strcpy(zonestring,"");
        double lonMin=0., latMin=0., lonMax=0., latMax=0.;
        for (itZNE=lms.vZNE.begin(); itZNE<lms.vZNE.end(); itZNE++)
            if (itTSG->num == itZNE->REG)
        {
            unit = DecodeUnit(itZNE->unit);
            lonMin = itZNE->lonMin;
            latMin = itZNE->latMin;
            lonMax = itZNE->lonMax;
            latMax = itZNE->latMax;
            sprintf (zonestring,"\
    <gmd:westBoundLongitude uom=\"%s\">%.15lf</gmd:westBoundLongitude>\n\
    <gmd:eastBoundLongitude uom=\"%s\">%.15lf</gmd:eastBoundLongitude>\n\
    <gmd:southBoundLatitude uom=\"%s\">%.15lf</gmd:southBoundLatitude>\n\
    <gmd:northBoundLatitude uom=\"%s\">%.15lf</gmd:northBoundLatitude>\n",
                    unit.data(), lonMin, unit.data(), lonMax,
                    unit.data(), latMin, unit.data(), latMax);
            break;
        }
        num1 = "REG"+reg1+str4LGOgreeenwich;
        num2 = "REG"+reg2+str4LGOgreeenwich;
        values = initValuesString(*itTSG);
        int values_number = initValuesNumber(*itTSG);
        rvrsblty = "REVERSE_SAME_PARAMETERS";
        for (itTSG2=lms.vTSG.begin(); itTSG2<lms.vTSG.end(); itTSG2++)
        {
            if (reg1==itTSG2->REG2 && reg2==itTSG2->REG1)
                rvrsblty ="DIRECT";
        }
		//<circe:values_unit>SECONDS</circe:values_unit>: unit of the rotation angles
        fprintf (fo,"\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:description>frame transformation</circe:description>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <circe:reversibility>%s</circe:reversibility>\n\
    <circe:crs_type>GEOCENTRIC</circe:crs_type>\n\
    <circe:source_frame>%s</circe:source_frame>\n\
    <circe:source_epoch>%lf</circe:source_epoch>\n\
    <circe:target_frame>%s</circe:target_frame>\n\
    <circe:target_epoch>%lf</circe:target_epoch>\n\
    <circe:values_number>%d</circe:values_number>\n\
    <circe:precision_code>YES</circe:precision_code>\n\
	<circe:init_values>%s</circe:init_values>\n\
	<circe:values_unit>SECONDS</circe:values_unit>\n\
    <circe:epoch>%lf</circe:epoch>\n\
    <circe:precision_code_init>%d</circe:precision_code_init>\n\
    <circe:layout>CONSTANT</circe:layout>\n\
%s\
</circe:frameTransformation>\n", itTSG->num.data(), rvrsblty.data(), num1.data(), epoch1, num2.data(),
                epoch2, values_number, values.data(), itTSG->epoch, itTSG->prec, zonestring);
	}

	for (itTSV = lms.vTSV.begin(); itTSV<lms.vTSV.end(); itTSV++)
	{
		//The geographic extent of some constant transformations may be defined
		//with the ZNE keyword (not mandatory).
		strcpy(zonestring, "");
		double lonMin = 0., latMin = 0., lonMax = 0., latMax = 0.;
		for (itZNE = lms.vZNE.begin(); itZNE<lms.vZNE.end(); itZNE++)
			if (itTSV->num == itZNE->REG)
			{
				unit = DecodeUnit(itZNE->unit);
				lonMin = itZNE->lonMin;
				latMin = itZNE->latMin;
				lonMax = itZNE->lonMax;
				latMax = itZNE->latMax;
				sprintf(zonestring, "\
	<gmd:westBoundLongitude uom=\"%s\">%.15lf</gmd:westBoundLongitude>\n\
	<gmd:eastBoundLongitude uom=\"%s\">%.15lf</gmd:eastBoundLongitude>\n\
	<gmd:southBoundLatitude uom=\"%s\">%.15lf</gmd:southBoundLatitude>\n\
	<gmd:northBoundLatitude uom=\"%s\">%.15lf</gmd:northBoundLatitude>\n",
						unit.data(), lonMin, unit.data(), lonMax,
						unit.data(), latMin, unit.data(), latMax);
				break;
			}
		if (itTSV->REA2 == "")
		{
			num1 = "REA" + itTSV->REA1;
			if (itTSV->REG != "")
				num2 = "REG" + itTSV->REG + str4LGOgreeenwich;
			else
				num2 = "";
			nodeframe = "UNDEFINED";
			nodeframeinfo = "TARGET";
		}
		else
		{
			num1 = "REA" + itTSV->REA1;
			num2 = "REA" + itTSV->REA2;
			if (itTSV->REG != "")
				nodeframe = "REG" + itTSV->REG + str4LGOgreeenwich;
			else
				nodeframe = "";
			nodeframeinfo = "OTHER";
		}
        values = asString(itTSV->a1) + "," + asString(itTSV->a2) + "," + asString(itTSV->a3) + "," + asString(itTSV->lon0) + "," + asString(itTSV->lat0);
        int values_number = 5;
		//<circe:values_unit>SECONDS</circe:values_unit>: unit of the rotation angles
		fprintf(fo, "\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:description>frame transformation</circe:description>\n\
	<circe:status>AUTHORITATIVE</circe:status>\n\
	<circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
	<circe:crs_type>VERTICAL</circe:crs_type>\n\
	<circe:source_frame>%s</circe:source_frame>\n\
	<circe:target_frame>%s</circe:target_frame>\n\
    <circe:node_frame>%s</circe:node_frame>\n\
    <circe:node_frame_info>%s</circe:node_frame_info>\n\
	<circe:values_number>%d</circe:values_number>\n\
	<circe:precision_code>YES</circe:precision_code>\n\
	<circe:init_values>%s</circe:init_values>\n\
	<circe:epoch>%lf</circe:epoch>\n\
	<circe:precision_code_init>%d</circe:precision_code_init>\n\
	<circe:layout>CONSTANT</circe:layout>\n\
%s\
</circe:frameTransformation>\n", itTSV->num.data(), num1.data(), num2.data(), nodeframe.data(), nodeframeinfo.data(), 
                                values_number, values.data(), 0., itTSV->prec, zonestring);
	}

	for (itPMM = lms.vPMM.begin(); itPMM<lms.vPMM.end(); itPMM++)
	{
		num = "PMM" + itPMM->num;
		num1 = "REG" + itPMM->REG + str4LGOgreeenwich;
		std::string values = "0,0,0,0,0,0,0," 
			+ asString(itPMM->dtx) + "," + asString(itPMM->dty) + "," + asString(itPMM->dtz) + ",0," 
			+ asString(itPMM->drx) + "," + asString(itPMM->dry) + "," + asString(itPMM->drz);
		fprintf(fo, "\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:description>plate motion model /%s/</circe:description>\n\
	<circe:status>AUTHORITATIVE</circe:status>\n\
    <circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
    <circe:crs_type>GEOCENTRIC</circe:crs_type>\n\
    <circe:source_frame>%s</circe:source_frame>\n\
    <circe:target_frame>%s</circe:target_frame>\n\
    <circe:values_number>14</circe:values_number>\n\
    <circe:precision_code>YES</circe:precision_code>\n\
	<circe:init_values>%s</circe:init_values>\n\
	<circe:values_unit>MASPERY</circe:values_unit>\n\
	<circe:epoch>0</circe:epoch>\n\
    <circe:precision_code_init>1</circe:precision_code_init>\n\
    <circe:layout>CONSTANT</circe:layout>\n\
    <gmd:westBoundLongitude uom=\"DEGREES\">-180.</gmd:westBoundLongitude>\n\
    <gmd:eastBoundLongitude uom=\"DEGREES\">180.</gmd:eastBoundLongitude>\n\
    <gmd:southBoundLatitude uom=\"DEGREES\">-90.</gmd:southBoundLatitude>\n\
    <gmd:northBoundLatitude uom=\"DEGREES\">90.</gmd:northBoundLatitude>\n\
</circe:frameTransformation>\n", num.data(), itPMM->num.data(), num1.data(), num1.data(), values.data());
	}

    for (itGRMM=lms.vGRMM.begin(); itGRMM<lms.vGRMM.end(); itGRMM++)
    {
        num1 = "REG"+itGRMM->REG+str4LGOgreeenwich;
        num = createID(itGRMM->filename);
        //metadatafilename is the same as metadatafilename with ".xml" extension
        metadatafilename=replaceExtension(itGRMM->filename,std::string("xml"));
        values="0,0,0";
        fprintf (fo,"\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:description>grid motion model</circe:description>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
    <circe:crs_type>GEOCENTRIC</circe:crs_type>\n\
    <circe:data_file_name>%s</circe:data_file_name>\n\
    <circe:metadata_file_name>%s</circe:metadata_file_name>\n\
    <circe:file_type></circe:file_type>\n\
    <circe:values_number>3</circe:values_number>\n\
    <circe:source_frame>%s</circe:source_frame>\n\
    <circe:target_frame>%s</circe:target_frame>\n\
    <circe:node_frame>UNDEFINED</circe:node_frame>\n\
    <circe:node_frame_info>TARGET</circe:node_frame_info>\n\
    <circe:init_values>%s</circe:init_values>\n\
</circe:frameTransformation>\n", num.data(), itGRMM->filename.data(),
                metadatafilename.data(), num1.data(), num1.data(), values.data());
    }

	
	for (itGR1D=lms.vGR1D.begin(); itGR1D<lms.vGR1D.end(); itGR1D++)
	{
        if (itGR1D->REA2 == "")
        {
            num1 = "REA"+itGR1D->REA1;
            num2 = "REG"+itGR1D->REG+str4LGOgreeenwich;
            nodeframe = "UNDEFINED";
            nodeframeinfo = "TARGET";
        }
        else
        {
            num1 = "REA"+itGR1D->REA1;
            num2 = "REA"+itGR1D->REA2;
            nodeframe = "REG"+itGR1D->REG+str4LGOgreeenwich;
            nodeframeinfo = "OTHER";
        }
        num = createID(itGR1D->filename);
        //metadatafilename is the same as metadatafilename with ".xml" extension
        metadatafilename=replaceExtension(itGR1D->filename,std::string("xml"));
        fprintf (fo,"\
<circe:frameTransformation gml:id=\"%s\">\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <circe:reversibility>REVERSE_SAME_PARAMETERS</circe:reversibility>\n\
    <circe:crs_type>VERTICAL</circe:crs_type>\n\
    <circe:data_file_name>%s</circe:data_file_name>\n\
    <circe:metadata_file_name>%s</circe:metadata_file_name>\n\
    <circe:file_type></circe:file_type>\n\
    <circe:values_number>1</circe:values_number>\n\
    <circe:init_values>0</circe:init_values>\n\
    <circe:source_frame>%s</circe:source_frame>\n\
    <circe:target_frame>%s</circe:target_frame>\n\
    <circe:node_frame>%s</circe:node_frame>\n\
    <circe:node_frame_info>%s</circe:node_frame_info>\n\
</circe:frameTransformation>\n", num.data(), itGR1D->filename.data(), metadatafilename.data(),
                                 num1.data(), num2.data(), nodeframe.data(), nodeframeinfo.data());
	}
    for (itPRC=lms.vPRC.begin(); itPRC<lms.vPRC.end(); itPRC++)
	{
		//PRC$num a un rapport avec celui de IGNF.xml <gml:Conversion gml:id=""> mais ce dernier a plus de caracteres, pourquoi ?
		num = "PRC"+itPRC->num;
		unit = DecodeUnit(itPRC->unit);
		
		//In circe, type and mode define projection methods: OK.
		//La correspondance avec les PRCM de IGNF.xml must be checked.
		//Where are the three methods (initialization, direct, reverse) in IGNF.xml ?
		prcm = "";
		     if (itPRC->type == 1 && itPRC->mode == 1) {prcm = "";} //LAMB_TG
        else if (itPRC->type == 1 && itPRC->mode == 2) {prcm = LCCTID;} //LAMBERT CONIQUE CONFORME - TANGENT AVEC REDUCTION D'ECHELLE
        else if (itPRC->type == 1 && itPRC->mode == 3) {prcm = LCCSID;} //LAMBERT CONIQUE CONFORME - SECANT AVEC PHI1 ET PHI2
        else if (itPRC->type == 2) {prcm = UTMNID;} //MERCATOR TRANSVERSE
        else if (itPRC->type == 3) {prcm = UTMSID;} //MERCATOR TRANSVERSE
        else if (itPRC->type == 5 && itPRC->mode == 1) {prcm = GLSBID;} //GAUSS-LABORDE - SPHERE BITANGENTE
        else if (itPRC->type == 5 && itPRC->mode == 2) {prcm = GLSEID;} //GAUSS-LABORDE - SPHERE EQUATORIALE
        else if (itPRC->type == 5 && itPRC->mode == 3) {prcm = GLSCID;} //GAUSS-LABORDE - SPHERE DE COURBURE MOYENNE
        else if (itPRC->type == 6) {prcm = LAEAID;} //LAMBERT AZIMUTHAL EQUAL AREA
        // SPSSID is the only stereographic to be implemented
        else if (itPRC->type == 9 && itPRC->mode == 1) {prcm = SMCSID;} //STEREOGRAPHIQUE - SPHERE DE COURBURE MOYENNE
        else if (itPRC->type == 9 && itPRC->mode == 5) {prcm = SPSTID;} //STEREOGRAPHIQUE - STEREOGRAPHIQUE POLAIRE - SUD - TANGENTE
        else if (itPRC->type == 9 && itPRC->mode == 7) {prcm = SPSSID;} //STEREOGRAPHIQUE - STEREOGRAPHIQUE POLAIRE - SUD - SECANTE

        int l;
        sscanf(itPRC->LGO.data(),"%d",&l);
        sprintf(str2LG0,"%02d",l);
        lgo = "LGO"+asString(str2LG0);

		//${PRC}[$i]->{c} code d'orientation des axes, unused by circe
        //lgo=>$champs[6-$dd],;
        fprintf (fo,"\
<gml:Conversion gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/coordinateOperation/IGNF/%s</gml:identifier>\n\
    <gml:name>%s</gml:name>\n\
    <gml:name codeSpace=\"OGP\">TO_BE_DEFINED</gml:name>\n\
    <gml:usesMethod xlink:href=\"%s\"/>\n\
    <gml:usesPrimeMeridian xlink:href=\"%s\"/>\n\
    <gml:ParameterValue uom=\"METERS\" gml:name=\"X0\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue uom=\"METERS\" gml:name=\"Y0\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue uom=\"%s\" gml:name=\"LAMBDA0\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue uom=\"%s\" gml:name=\"PHI0\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue uom=\"%s\" gml:name=\"PHI1\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue uom=\"%s\" gml:name=\"PHI2\">%.15lf</gml:ParameterValue>\n\
    <gml:ParameterValue gml:name=\"K0\">%.15lf</gml:ParameterValue>\n\
</gml:Conversion>\n", num.data(), num.data(), itPRC->name.data(),
                    prcm.data(), lgo.data(), itPRC->x0, itPRC->y0, unit.data(), itPRC->l0,
                    unit.data(), itPRC->j0, unit.data(), itPRC->j1, unit.data(), itPRC->j2, itPRC->delta);
	}
    for (itPROJ=lms.vPROJ.begin(); itPROJ<lms.vPROJ.end(); itPROJ++)
	{
        double lonMin=0., latMin=0., lonMax=0., latMax=0.;
        std::string shortn = itPROJ->short_name;
        std::string name = itPROJ->name;
        std::string reg = "REG"+itPROJ->REG;
		std::string unit = DecodeUnit(itPROJ->unit);
		std::string num = "PROJ"+itPROJ->REG+itPROJ->PRC;
		std::string prcnum = "";
        std::string prc = itPROJ->PRC;
        if (prc.length()==3)
            prc = "0"+prc;
        for (itPRC=lms.vPRC.begin(); itPRC<lms.vPRC.end(); itPRC++)
		{
            if (prc == itPRC->num)
			{
                //if (version != "V5")
                //    name = itPRC->name;
				prcnum = "PRC"+itPRC->num;

//                int l;
//                sscanf(itPRC->LGO.data(),"%d",&l);
//                sprintf(str4LG0,"%04d",l);
//                reg = "REG"+itPROJ->REG+str4LG0;
                break;
            }
		}
        if (itPRC==lms.vPRC.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: PROJ "+itPROJ->short_name+": PRC")+ErrorMessageText[EM_NOT_FOUND]);
        for (itREG=lms.vREG.begin(); itREG<lms.vREG.end(); itREG++)
        {
            if (itPROJ->REG == itREG->num)
            {
                int l;
                sscanf(itREG->LGO.data(),"%d",&l);
                sprintf(str4LG0,"%04d",l);
                reg = "REG"+itREG->num+str4LG0;
                MEASURE_UNIT zu = MEASURE_UNIT(getIndex(unit, MeasureUnitAttributeText, MU_COUNT));
                lonMin = UnitConvert(itPROJ->lonMin, zu, MU_RADIAN) - itREG->from_greenwich;
                lonMax = UnitConvert(itPROJ->lonMax, zu, MU_RADIAN) - itREG->from_greenwich;
                latMin = UnitConvert(itPROJ->latMin, zu , MU_RADIAN);
                latMax = UnitConvert(itPROJ->latMax, zu , MU_RADIAN);
                unit = "RADIANS";
                break;
            }
            if (itREG==lms.vREG.end()) throw (std::string("WriteXMlfromCirceLegacyMetadata: PROJ "+itPROJ->short_name+": REG")+ErrorMessageText[EM_NOT_FOUND]);
        }
        fprintf (fo,"\
<gml:ProjectedCRS gml:id=\"%s\">\n\
    <gml:identifier codeSpace=\"http://registre.ign.fr/ign\">http://registre.ign.fr/ign/IGNF/crs/IGNF/%s</gml:identifier>\n\
    <circe:status>AUTHORITATIVE</circe:status>\n\
    <gml:name>%s</gml:name>\n\
    <gml:name codeSpace=\"OGP\">TO_BE_DEFINED</gml:name>\n\
    <circe:crs_type>PROJECTED</circe:crs_type>\n\
    <gmd:westBoundLongitude uom=\"%s\">%.15lf</gmd:westBoundLongitude>\n\
    <gmd:eastBoundLongitude uom=\"%s\">%.15lf</gmd:eastBoundLongitude>\n\
    <gmd:southBoundLatitude uom=\"%s\">%.15lf</gmd:southBoundLatitude>\n\
    <gmd:northBoundLatitude uom=\"%s\">%.15lf</gmd:northBoundLatitude>\n\
    <gml:definedByConversion xlink:href=\"%s\"/>\n\
    <gml:baseGeographicCRS xlink:href=\"%s(TO_BE_DEFINED)\"/>\n\
    <circe:usesReferenceFrame xlink:href=\"%s\"/>\n\
</gml:ProjectedCRS>\n", shortn.data(), num.data(), name.data(), unit.data(), lonMin,
                    unit.data(), lonMax,
                    unit.data(), latMin, unit.data(), latMax, prcnum.data(),
                    reg.data(), reg.data());
	}
	//for (my $i=0; $i<=$#{TYPCOOR}; $i++) 
	//{
		//print "TYPCOOR:\t".${TYPCOOR}[$i]->{REG};
		//foreach my $co (split/\s+/,${TYPCOOR}[$i]->{COOR})
		//{
			//print "\t".$co;
		//}
		//print "\n";
	//}
	//for (my $i=0; $i<=$#{UNIT}; $i++) 
	//{
		//print "UNIT:\t".${UNIT}[$i]->{REG};
		//foreach my $un (split/\s+/,${UNIT}[$i]->{UNIT})
		//{
			//print "\t".$un;
		//}
		//print "\n";
	//}
    fprintf (fo,"</CT_CrsCatalogue>\n</CIRCE>");
    fclose(fo);
}



}
