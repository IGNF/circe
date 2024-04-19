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
* \file geodeticset.cpp
* \brief Implementation of the GeodeticSet class.
*/

#include "geodeticset.h"
#include "metadata.h"

namespace circe
{

/**
* \brief Constructor.
*/
GeodeticSet::GeodeticSet()
{
    mb=NULL;
}

/**
* \brief Empties values of the members of a GeodeticSet instance.
*/
void GeodeticSet::reset()
{
    vEllipsoid.clear();
    vMeridian.clear();
    vGeodeticFrame.clear();
    vVerticalFrame.clear();
    vGeodeticSystem.clear();
    vVerticalSystem.clear();
    vGeodeticTransfo.clear();
    vVerticalTransfo.clear();
    vConversion.clear();
    vCRS.clear();
	keyGeodeticFrame = "";
    //metadataPath = ""; //ok
    mb=NULL;
}

/**
* \brief Returns a pointer to a GeodeticReferenceFrame whose id is input.
* \param[in] id: the GeodeticReferenceFrame id.
* \return a GeodeticReferenceFrame*
*/
GeodeticReferenceFrame* GeodeticSet::getGeodeticReferenceFrame(std::string id)
{
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    GeodeticReferenceFrame* pGRF = NULL;
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        if (itGF->getCirceId() == id)
            pGRF = &(*itGF);
    return pGRF;
}

/**
* \brief Returns a pointer to a CRS whose id is input.
* \param[in] id: the CRS id.
* \return a CRS*
*/
CRS* GeodeticSet::getCRS(std::string id)
{
    std::vector<circe::CRS>::iterator itCRS;
    CRS* pCRS = NULL;
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        if (itCRS->isCRSmatching(id))
            pCRS = &(*itCRS);
    return pCRS;
}

/**
* \brief Returns a pointer to a VerticalReferenceFrame whose id is input.
* \param[in] id: the VerticalReferenceFrame id.
* \return a VerticalReferenceFrame*
*/
std::string GeodeticSet::getVerticalReferenceFrameName(std::string id)
{
	if (id == "")
		return "";
	std::vector<circe::VerticalReferenceFrame>::iterator itVF;
	for (itVF = vVerticalFrame.begin(); itVF<vVerticalFrame.end(); itVF++)
		if (itVF->getCirceId() == id)
            return itVF->getName();
	return "";
}

/**
* \brief Returns a CRS id corresponding to a frame whose id is input.
* \param[in] frameId: the frame id.
* \return One and only one vertical CRS id for a vertical frame, or the first geodetic CRS id for a geodetic frame.
*/
std::string GeodeticSet::getCRSid(std::string frameId)
{
    std::vector<circe::CRS>::iterator itCRS;
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        if (itCRS->getReferenceFrameId() == frameId)
            return itCRS->getCirceId();
    return "";
}

/**
* \brief Determines the authority and the type of a CRS whose id is input.
* \param[in] CRSid: the CRS id.
* \param[out] CRStype: The CRS type as a string.
* \return The authority IGNF or EPSG.
*/
std::string GeodeticSet::getAuthority(std::string CRSid, std::string *CRStype)
{
    std::vector<circe::CRS>::iterator itCRS;
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
		if (itCRS->getCirceId() == afterChar(CRSid, ':'))
		{
			*CRStype = CRStypeDisplayText[itCRS->getCRStype()];
			return "IGNF";
		}
        else if (itCRS->getEPSGid() == afterChar(CRSid,':'))
		{
			*CRStype = CRStypeDisplayText[itCRS->getCRStype()];
			return "EPSG";
		}

    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        if (itGF->getCirceId() == afterChar(CRSid, ':'))
        {
            return "IGNF";
        }
        else if (itGF->getEPSGid() == afterChar(CRSid,':'))
        {
            return "EPSG";
        }

    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
        if (itVF->getCirceId() == afterChar(CRSid, ':'))
        {
            return "IGNF";
        }
        else if (itVF->getEPSGid() == afterChar(CRSid,':'))
        {
            return "EPSG";
        }

    return "none";
}

/**
* \brief Displays the ids of the objects of a GeodeticSet instance.
* \return A std::string containing the ids.
*/
std::string GeodeticSet::displayGeodeticSet()
{
    std::string str = "";
    std::vector<circe::Ellipsoid>::iterator itEll;
    std::vector<circe::Meridian>::iterator itMer;
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    std::vector<circe::GeodeticReferenceSystem>::iterator itGS;
    std::vector<circe::VerticalReferenceSystem>::iterator itVS;
    std::vector<circe::Transformation>::iterator itGT;
    std::vector<circe::Transformation>::iterator itVT;
    std::vector<circe::Conversion>::iterator itConv;
    std::vector<circe::CRS>::iterator itCRS;

    str += "\nMeridians\n";
    for(itMer=vMeridian.begin();itMer<vMeridian.end();itMer++)
        str += itMer->getCirceId()+"\n";

    str += "\nEllipsoids\n";
    for(itEll=vEllipsoid.begin();itEll<vEllipsoid.end();itEll++)
        str += itEll->getCirceId()+"\n";

    str += "\nGeodetic Systems\n";
    for(itGS=vGeodeticSystem.begin();itGS<vGeodeticSystem.end();itGS++)
        str += itGS->getCirceId()+"\n";

    str += "Geodetic Frames\n";
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        str += itGF->getCirceId()+"\n";

    str += "\nVertical Systems\n";
    for(itVS=vVerticalSystem.begin();itVS<vVerticalSystem.end();itVS++)
        str += itVS->getCirceId()+"\n";

    str += "\nVertical Frames\n";
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
        str += itVF->getCirceId()+"\n";

    str += "\nCoordinate Reference Systems\n";
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        str += itCRS->getCirceId()+"\n";

    str += "\nGeodetic Transformations\n";
    for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
        str += itGT->getCirceId()+"\n";

    str += "\nVertical Transformations\n";
    for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
        str += itVT->getCirceId()+"\n";

    str += "\nConversions\n";
    for(itConv=vConversion.begin();itConv<vConversion.end();itConv++)
        str += itConv->getCirceId()+"\n";

    return str;
}

/**
* \brief Displays the ids and the captions of the objects of a geodeticSubset instance.
* \param[in] gs: A geodeticSubset instance.
* \return A std::string containing the ids and the captions.
*/
std::string GeodeticSet::displayGeodeticSubset(const geodeticSubset& gs)
{
    std::string str = "";
    unsigned int i;

    str += "geodeticFrame ["+asString(gs.iGeodeticFrame)+"]\n";
    for (i=0;i<gs.geodeticFrameIdList.size();i++)
        str += gs.geodeticFrameIdList[i]+"\t"+gs.geodeticFrameDisplayList[i]+"\n";

    str += "\ngeodeticCRS["+asString(gs.iGeodeticCRS)+"]\n";
    for (i=0;i<gs.geodeticCRSidList.size();i++)
        str += gs.geodeticCRSidList[i]+"\t"+gs.geodeticCRSdisplayList[i]+"\n";

    str += "\nverticalFrame["+asString(gs.iVerticalFrame)+"]\n";
    for (i=0;i<gs.verticalFrameIdList.size();i++)
        str += gs.verticalFrameIdList[i]+"\t"+gs.verticalFrameDisplayList[i]+"\n";

    str += "\ndataFileFormat["+asString(gs.iDataFileFormat)+"]\n";
    for (i=0;i<gs.dataFileFormatList.size();i++)
        str += gs.dataFileFormatList[i]+"\t"+gs.dataFileFormatDisplayList[i]+"\n";

    str += "\ndataPointFormat["+asString(gs.iDataPointFormat)+"]\n";
    for (i=0;i<gs.dataPointFormatList.size();i++)
        str += gs.dataPointFormatList[i]+"\t"+gs.dataPointFormatDisplayList[i]+"\n";

    str += "\ndataUnit["+asString(gs.iDataUnit)+"]\n";
    for (i=0;i<gs.dataUnitList.size();i++)
        str += gs.dataUnitList[i]+"\t"+gs.dataUnitDisplayList[i]+"\n";

    return str;
}

/**
* \brief Displays the ids and the captions of the objects of an operationSubset instance.
* \param[in] os: An operationSubset instance.
* \return A std::string containing the ids and the captions of the source & target geodeticSubset and Transformations.
*/
std::string GeodeticSet::displayOperationSubset(const operationSubset& os)
{
    std::string str = "";
    unsigned int i;

    str += "\n\nSOURCE SUBSET\n";
    str += displayGeodeticSubset(os.sourceSubset);

    str += "\n\nTARGET SUBSET\n";
    str += displayGeodeticSubset(os.targetSubset);

    str += "GeodeticTransfo ["+asString(os.iGeodeticTransfo)+"]\n";
    for (i=0;i<os.geodeticTransfoIdList.size();i++)
        str += os.geodeticTransfoIdList[i]+"\t"+os.geodeticTransfoDisplayList[i]+"\n";

    str += "VerticalTransfo ["+asString(os.iVerticalTransfo)+"]\n";
    for (i=0;i<os.verticalTransfoIdList.size();i++)
        str += os.verticalTransfoIdList[i]+"\t"+os.verticalTransfoDisplayList[i]+"\n";

    return str;
}

/**
* \brief Displays the contents of a GeodeticSet and an operationSubset instances.
* \param[in] os: An operationSubset instance.
* \return A std::string containing the ids and the captions of the GeodeticSet and the operationSubset instances.
*/
std::string GeodeticSet::displayAll(const operationSubset& os)
{
    std::string str = "";
    str += "\nGEODETIC SET\n";
    str += displayGeodeticSet();
    str += getValidityArea(MU_DEGREE,"");
    str += "\nOPERATION SUBSET\n";
    str += displayOperationSubset(os);
    return str;
}

/**
* \brief Completes metadata after loading of a metada file.
*/
void GeodeticSet::CompleteMetadata()
{
    std::vector<circe::Ellipsoid>::iterator itEll;
    std::vector<circe::Meridian>::iterator itMer;
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    std::vector<circe::GeodeticReferenceSystem>::iterator itGS, itGS2;
    std::vector<circe::VerticalReferenceSystem>::iterator itVS;
    std::vector<circe::Transformation>::iterator itGT;
    std::vector<circe::Transformation>::iterator itVT;
    std::vector<circe::Conversion>::iterator itConv;
    std::vector<circe::CRS>::iterator itCRS, itCRS2;
    std::string sysId;

    //IGNF.xml (STPM50)
    //Some functions related to transformation don't check if they are geodetic or vertical transformations.
    //They only rely on CRS ids that must be unique.
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        for(itCRS2=itCRS+1;itCRS2<vCRS.end();itCRS2++)
            if (itCRS->getCirceId() == itCRS2->getCirceId())
                throw std::string(itCRS->getCirceId()+" non-unique CRS id. This is not valid in circe.");

    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
    {
        for(itGS=vGeodeticSystem.begin();itGS<vGeodeticSystem.end();itGS++)
        {
            if (itGF->getReferenceSystemId() == itGS->getCirceId())
            {
                itGF->setGeodeticReferenceSystem(&(*itGS));

                //IGNF.xml
                //The '1' refers to Greenwich meridian. If the system has another prime meridian, the
                //corresponding system related to Greenwich will be used to process transformations.
                //See "strUsesForTransformation" in metadata.cpp function WriteXMlfromCirceLegacyMetadata
                //for its construction in the case of Circe Legacy metadata.
                //But with IGNF this may not exists.
                if (itGS->getUsesForTransformation() == "")
                {
                    sysId = replaceLastChar(itGS->getCirceId(),'1');
                    if (sysId != itGS->getCirceId())
                    {
                        //check for existence
                        for(itGS2=vGeodeticSystem.begin();itGS2<vGeodeticSystem.end();itGS2++)
                            if (itGS2->getCirceId() == sysId)
                                break;
                        if (itGS2==vGeodeticSystem.end())
                            sysId = "";
                    }
                    itGS->setUsesForTransformation(sysId);
                }

                itGF->setUsesForTransformation(itGS->getUsesForTransformation());
                if (itGS->getArea()>0)
                    itGF->copyBounds(*itGS);
                itGF->setSystemDimension(itGS->getSystemDimension());
                for(itEll=vEllipsoid.begin();itEll<vEllipsoid.end();itEll++)
                {
                    if (itGS->getEllipsoidId() == itEll->getCirceId())
                    {
                        itGF->set_a(itEll->get_a());
                        itGF->set_b(itEll->get_b());
                        itGF->set_f(itEll->get_f());
                        itGF->set_e2(itEll->get_e2());
						itGF->setEllName(itEll->getName());
                    }
                }
                for(itMer=vMeridian.begin();itMer<vMeridian.end();itMer++)
                {
                    if (itGS->getPrimeMeridianId() == itMer->getCirceId())
                    {
                        itGF->setFromGreenwich(itMer->getFromGreenwich());
                    }
                }
            }
        }
    }
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
    {
        for(itVS=vVerticalSystem.begin();itVS<vVerticalSystem.end();itVS++)
        {
            if (itVF->getReferenceSystemId() == itVS->getCirceId())
            {
                itVF->setVerticalReferenceSystem(&(*itVS));
                if (itVS->getArea()>0)
                    itVF->copyBounds(*itVS);
            }
        }
    }

    for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
    {
        for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
        {
            if (itVT->getSourceFrameId() == itVF->getCirceId()
			 || itVT->getTargetFrameId() == itVF->getCirceId())
            {
                if ( fabs(itVT->getWestBound())<1e-12 && fabs(itVT->getEastBound())<1e-12
                  && fabs(itVT->getNorthBound())<1e-12 && fabs(itVT->getSouthBound())<1e-12)
                {
                    if (itVF->getArea()>0)
                        itVT->copyBounds(*itVF);
                }
                else if (itVT->getArea()>0)
                    itVF->copyBounds(*itVT);
            }
        }
		if (itVT->getArea()==0)
			for (itGF = vGeodeticFrame.begin(); itGF<vGeodeticFrame.end(); itGF++)
		{
			if (itVT->getTargetFrameId() == itGF->getCirceId())
			{
				if (fabs(itVT->getWestBound())<1e-12 && fabs(itVT->getEastBound())<1e-12
					&& fabs(itVT->getNorthBound())<1e-12 && fabs(itVT->getSouthBound())<1e-12)
				{
					if (itGF->getArea()>0)
						itVT->copyBounds(*itGF);
				}
				else if (itVT->getArea()>0)
					itGF->copyBounds(*itVT);
			}
		}
	}

    //IGNF.xml
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
    {
        switch (itCRS->getCRStype())
        {
        case CT_PROJECTED:
            for(itCRS2=vCRS.begin();itCRS2<vCRS.end();itCRS2++)
            if (itCRS2->getCRStype() == CT_GEOGRAPHIC
             && itCRS2->getCirceId() == itCRS->getBaseGeographicCRSid())
                itCRS->setReferenceFrameId(itCRS2->getReferenceFrameId());
            break;
        default:
            break;
        }
    }

    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
    {
        switch (itCRS->getCRStype())
        {
        case CT_GEOGRAPHIC:
        case CT_GEOCENTRIC:
            for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
            {
                if (itGF->getCirceId() == itCRS->getReferenceFrameId())
                {
                    itCRS->setReferenceFrame(&(*itGF));
                    if (itGF->getArea()>0)
                        itCRS->copyBounds(*itGF);
                    else if (itCRS->getArea()>0)
                    {
                        itGF->copyBounds(*itCRS);
                        itGF->getGeodeticReferenceSystem()->copyBounds(*itGF);
                    }
                }
            }
            break;
        case CT_PROJECTED:
            if (itCRS->getReferenceFrameId() != "")
                for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
            {
                if (itGF->getCirceId() == itCRS->getReferenceFrameId())
                {
                    itCRS->setReferenceFrame(&(*itGF));
                }
            }
            break;
        case CT_VERTICAL:
            for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
            {
                if (itVF->getCirceId() == itCRS->getReferenceFrameId())
                {
                    itCRS->setReferenceFrame(&(*itVF));
                    if (itVF->getArea()>0)
                        itCRS->copyBounds(*itVF);
                    else if (itCRS->getArea()>0)
                    {
                        itVF->copyBounds(*itCRS);
                        itVF->getVerticalReferenceSystem()->copyBounds(*itVF);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
    {
        switch (itCRS->getCRStype())
        {
        case CT_PROJECTED:
            for(itConv=vConversion.begin();itConv<vConversion.end();itConv++)
            {
                if (itConv->getCirceId() == itCRS->getConversionId())
                {
                    itCRS->setDefiningConversion(&(*itConv));
                }
            }
            break;
        default:
            break;
        }
    }

    //IGNF.xml
    for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
    {
        for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        {
            if (itGT->getSourceFrameId() == itCRS->getCirceId())
                itGT->setSourceFrameId(itCRS->getReferenceFrameId());
            if (itGT->getTargetFrameId() == itCRS->getCirceId())
                itGT->setTargetFrameId(itCRS->getReferenceFrameId());
        }
    }
    for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
    {
        for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        {
            if (itVT->getSourceFrameId() == itCRS->getCirceId())
                itVT->setSourceFrameId(itCRS->getReferenceFrameId());
            if (itVT->getTargetFrameId() == itCRS->getCirceId())
                itVT->setTargetFrameId(itCRS->getReferenceFrameId());
        }
        for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        {
            if (itVT->getSourceFrameId() == itGF->getCirceId())
            {
                itVT->setAf(itGF->get_a());
                itVT->setE2f(itGF->get_e2());
            }
            else if (itVT->getTargetFrameId() == itGF->getCirceId())
            {
                itVT->setAt(itGF->get_a());
                itVT->setE2t(itGF->get_e2());
            }
        }
        //If both source and target frames are not geodetic (i.e. transformation between
        //two vertical frames), only the node frame is geodetic.
        if (itVT->getAf()==0. && itVT->getE2f()==0. && itVT->getAt()==0. && itVT->getE2t()==0.)
            for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
                if (itVT->getNodeFrameId() == itGF->getCirceId())
                {
                    itVT->setAf(itGF->get_a());
                    itVT->setE2f(itGF->get_e2());
                }
    }

    for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
    {
        for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        {
            if (itGT->getSourceFrameId() == itGF->getCirceId())
            {
                itGT->setAf(itGF->get_a());
                itGT->setE2f(itGF->get_e2());
                if ( fabs(itGT->getWestBound())<1e-12 && fabs(itGT->getEastBound())<1e-12
                  && fabs(itGT->getNorthBound())<1e-12 && fabs(itGT->getSouthBound())<1e-12)
                {
                    if (itGF->getArea()>0)
                        itGT->copyBounds(*itGF);
                }
            }
            if (itGT->getTargetFrameId() == itGF->getCirceId())
            {
                itGT->setAt(itGF->get_a());
                itGT->setE2t(itGF->get_e2());
            }
        }
    }

    //This doesn't apply to IGNF.xml where conversions don't have such
    //"prime meridian" parameter.
    for(itConv=vConversion.begin();itConv<vConversion.end();itConv++)
    {
        for(itMer=vMeridian.begin();itMer<vMeridian.end();itMer++)
        {
            if (itConv->getPrimeMeridianId() == itMer->getCirceId())
            {
                itConv->setFromGreenwich(itMer->getFromGreenwich());
            }
        }
    }

    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
            if (itGF->getCirceId() == itCRS->getReferenceFrameId()
            && itCRS->getCRStype() == CT_GEOGRAPHIC)
                itGF->setGeographicCRSid(itCRS->getCirceId());

    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
    {
        itGF->setHasTransformationWithKeyFrame(false);
        for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
            if ( (itGT->getSourceFrameId() == itGF->getCirceId()
               && itGT->getTargetFrameId() == getKeyGeodeticFrame())
               ||(itGT->getTargetFrameId() == itGF->getCirceId()
               && itGT->getSourceFrameId() == getKeyGeodeticFrame()) )
                itGF->setHasTransformationWithKeyFrame(true);
    }
}

/**
* \brief Displays the list of available CRS ids.
* \return A std::string containing the list of available CRS ids.
* The geodetic CRSs are listed first, then the vertical CRSs, then the compound CRSs 
* (i.e. geodetic and vertical CRSs whose geographic domains intersect), with both IGNF and EPSG ids.
*/
std::string GeodeticSet::getCRSidList(std::string CRStype, const circeOptions& circopt)
{
    std::string CRSidList = "", kotes = "", frmt = circopt.outputFormat;
    std::vector<CRS>::iterator itCRSg, itCRSv;

    if (frmt == "JSON")
        kotes = "\"";

    // Geodetic CRS
    if (CRStype == "all" || CRStype == "geodetic")
    for(itCRSg=vCRS.begin();itCRSg<vCRS.end();itCRSg++) if (itCRSg->getCRStype() != CT_VERTICAL)
    {
        if (CRSidList != "")
            CRSidList += ",";
        //CRSidList += "IGNF:"+itCRSg->getCirceId();
        CRSidList += kotes+itCRSg->getCirceId()+kotes+":"+kotes+itCRSg->getName()+kotes;
//        if (itCRSg->getEPSGid()!="")
//            CRSidList += ",EPSG:"+itCRSg->getEPSGid();
    }

    // Vertical CRS
    if (CRStype == "all" || CRStype == "vertical")
    for(itCRSv=vCRS.begin();itCRSv<vCRS.end();itCRSv++) if (itCRSv->getCRStype() == CT_VERTICAL)
    {
        if (CRSidList != "")
            CRSidList += ",";
        //CRSidList += "IGNF:"+itCRSv->getCirceId();
        CRSidList += kotes+itCRSv->getCirceId()+kotes+":"+kotes+itCRSv->getName()+kotes;
//        if (itCRSv->getEPSGid()!="")
//            CRSidList += ",EPSG:"+itCRSv->getEPSGid();
    }

    // Compound CRS
    if (CRStype == "all" || CRStype == "compound")
    for(itCRSg=vCRS.begin();itCRSg<vCRS.end();itCRSg++) if (itCRSg->getCRStype() != CT_VERTICAL)
    {
        for(itCRSv=vCRS.begin();itCRSv<vCRS.end();itCRSv++) if (itCRSv->getCRStype() == CT_VERTICAL)
            if (itCRSv->intersects(*itCRSg))
        {
            if (CRSidList != "")
                CRSidList += ",";
            //CRSidList += "IGNF:"+itCRSg->getCirceId()+"."+itCRSv->getCirceId();
            CRSidList += kotes+itCRSg->getCirceId()+"."+itCRSv->getCirceId()+kotes+":"+kotes+itCRSg->getName()+"."+itCRSv->getName()+kotes;
//            if (itCRSg->getEPSGid()!="" && itCRSv->getEPSGid()!="")
//                CRSidList += ",EPSG:"+itCRSg->getEPSGid()+"."+itCRSv->getEPSGid();
        }
    }
    return displayOutput(circopt, CRSidList, "{", "}", false);
}

/**
* \brief Displays the validity area of a CRS whose id is input.
* \param[in] CRSid: The CRS id.
* \return A std::string containing the validity area.
*/
std::string GeodeticSet::getValidityArea(std::string CRSid, MEASURE_UNIT outUnit, std::string frmt)
{
    for(std::vector<CRS>::iterator itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        if (itCRS->isCRSmatching(CRSid))
            return itCRS->getValidityArea(outUnit,frmt);
//    return std::string("ID=")+CRSid+": CRS"+ErrorMessageText[EM_NOT_FOUND];
    return ErrorMessageText[EM_NOT_FOUND];
}


/**
* \brief Displays the validity area of a all the CRSs of a GeodeticSet.
* \return A std::string containing the validity areas.
*/
std::string GeodeticSet::getValidityArea(MEASURE_UNIT outUnit, std::string frmt)
{
    std::string validityArea;
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    std::vector<circe::GeodeticReferenceSystem>::iterator itGS;
    std::vector<circe::VerticalReferenceSystem>::iterator itVS;
    std::vector<circe::Transformation>::iterator itGT;
    std::vector<circe::Transformation>::iterator itVT;
    std::vector<circe::Conversion>::iterator itConv;
    std::vector<circe::CRS>::iterator itCRS;

    validityArea += "GeodeticFrame\n";
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
        validityArea += itGF->getValidityArea(outUnit,frmt);
    validityArea += "\nGeodeticSystem\n";
    for(itGS=vGeodeticSystem.begin();itGS<vGeodeticSystem.end();itGS++)
        validityArea += itGS->getValidityArea(outUnit,frmt);
    validityArea += "\nGeodeticTransfo\n";
    for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
        validityArea += itGT->getValidityArea(outUnit,frmt);
    validityArea += "\nVerticalFrame\n";
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
        validityArea += itVF->getValidityArea(outUnit,frmt);
    validityArea += "\nVerticalSystem\n";
    for(itVS=vVerticalSystem.begin();itVS<vVerticalSystem.end();itVS++)
        validityArea += itVS->getValidityArea(outUnit,frmt);
    validityArea += "\nVerticalTransfo\n";
    for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
        validityArea += itVT->getValidityArea(outUnit,frmt);
    validityArea += "\nConversion\n";
    for(itConv=vConversion.begin();itConv<vConversion.end();itConv++)
        validityArea += itConv->getValidityArea(outUnit,frmt);
    validityArea += "\nCRS\n";
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
        validityArea += itCRS->getValidityArea(outUnit,frmt);
    return validityArea;
}

/**
* \brief Initializes a full geodetic frame data set for HMIs.
* \param gss: A pointer to a geodeticSubset instance.
* \param[in] go: an instance of geodeticOptions.
*/
void GeodeticSet::getGeodeticFrameList(geodeticSubset* gss, const circe::geodeticOptions& go,
                                       std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
	gss->iGeodeticFrame = 0;
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
	gss->geodeticFrameIdList.clear();
	gss->geodeticFrameDisplayList.clear();
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
    {
		gss->geodeticFrameIdList.push_back(itGF->getCirceId());
		gss->geodeticFrameDisplayList.push_back(/*QString::fromLatin1(*/itGF->getName());
		if (itGF->getCirceId() == go.geodeticFrameId)
			gss->iGeodeticFrame = ii;
        ii++;
        if (*l != "getAllGeodeticSubsetLists")
        {
            if (*l != "") *l += ",";
            *l += kotes+itGF->getCirceId()+kotes+":"+kotes+itGF->getName()+kotes;
        }
    }
	if (gss->geodeticFrameDisplayList.size() == 0) gss->iGeodeticFrame = -1;
}

/**
* \brief Initializes the target geodetic frame data for HMIs.
* \param targetSubset: A pointer to a geodeticSubset instance.
* \param[in] CO: an instance of circeOptions.
*/
void GeodeticSet::getTargetGeodeticFrameList(geodeticSubset* targetSubset, const circe::circeOptions& CO,
                                             std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    targetSubset->iGeodeticFrame=0;
    std::vector<circe::GeodeticReferenceFrame>::iterator itGF;
    std::vector<circe::Transformation>::iterator itGT;
    std::vector<circe::Transformation> vTransfo;
    bool isReachableFrame=false;
    for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
    {
        if (itGT->getSourceFrameId() == CO.sourceOptions.usesForTransformation
         || itGT->getTargetFrameId() == CO.sourceOptions.usesForTransformation)
        {
            vTransfo.push_back(*itGT);
        }
    }
    targetSubset->geodeticFrameIdList.clear();
    targetSubset->geodeticFrameDisplayList.clear();
    //if (vTransfo.size() == 0)
      //  return;
    bool htwkf = getGeodeticReferenceFrame(CO.sourceOptions.usesForTransformation)->getHasTransformationWithKeyFrame();
    for(itGF=vGeodeticFrame.begin();itGF<vGeodeticFrame.end();itGF++)
    {
        isReachableFrame = false;
        if ( (CO.sourceOptions.usesForTransformation == itGF->getUsesForTransformation())
          || (itGF->getHasTransformationWithKeyFrame() && htwkf))
            isReachableFrame = true;
        else
        for(itGT=vTransfo.begin();itGT<vTransfo.end();itGT++)
            if (itGT->getSourceFrameId() == itGF->getUsesForTransformation()
             || itGT->getTargetFrameId() == itGF->getUsesForTransformation())
        {
            isReachableFrame = true;
            break;
        }

        if (isReachableFrame)
        {
            targetSubset->geodeticFrameIdList.push_back(itGF->getCirceId());
            targetSubset->geodeticFrameDisplayList.push_back(/*QString::fromLatin1(*/itGF->getName());
            if (itGF->getCirceId() == CO.targetOptions.geodeticFrameId)
                targetSubset->iGeodeticFrame = ii;
            ii++;
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += kotes+itGF->getCirceId()+kotes+":"+kotes+itGF->getName()+kotes;
            }
        }
    }
    if (targetSubset->geodeticFrameDisplayList.size()==0) targetSubset->iGeodeticFrame = -1;
}

/**
* \brief Initializes the geodetic transformation data for HMIs.
* \param OS: A pointer to an operationSubset instance.
* \param[in] CO: an instance of circeOptions.
*/
void GeodeticSet::getGeodeticTransfoList(operationSubset *OS, const circe::circeOptions& CO,
                                         std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    OS->iGeodeticTransfo=0;
    std::vector<circe::Transformation>::iterator itGT;
    OS->geodeticTransfoIdList.clear();
    OS->geodeticTransfoDisplayList.clear();
    std::string displayText="", displayTextAuthoritative="";
    for (int i=0; i<2; i++)
    {
        for(itGT=vGeodeticTransfo.begin();itGT<vGeodeticTransfo.end();itGT++)
        {
            if ((itGT->getSourceFrameId() == CO.sourceOptions.usesForTransformation
                 && itGT->getTargetFrameId() == CO.targetOptions.usesForTransformation)
                    || (itGT->getTargetFrameId() == CO.sourceOptions.usesForTransformation
                        && itGT->getSourceFrameId() == CO.targetOptions.usesForTransformation))
            {
                if (itGT->getLayout() == L_CONSTANT)
                    displayText = TransfoLayoutText[L_CONSTANT]+" "+asString(itGT->getValuesNumber())+"-param";
                else
                    displayText = /*QString::fromLatin1((*/itGT->getCirceId()/*+" "+GridTypeText[itGT->getGridType()]*/;
                // This is very important:
                // If the transformation is authoritative, let circe choose it automatically
                // (it may depend on each point location) and don't let writeCommandLine()
                // write the --transfoRequired argument.
                // On the other hand, if the transformation is not authoritative, Circe will
                // choose it only if it is parsed by the --transfoRequired argument.
                if (i==0)
                {
                    if (itGT->getStatus() == S_AUTHORITATIVE)
                        displayTextAuthoritative += displayText+"  ";
                }
                else
                {
                    if (itGT->getStatus() != S_AUTHORITATIVE)
                    {
                        OS->geodeticTransfoIdList.push_back(itGT->getCirceId());
                        OS->geodeticTransfoDisplayList.push_back(displayText);
                        if (itGT->getCirceId() == CO.idGeodeticTransfoRequired)
                            OS->iGeodeticTransfo = ii;
                        ii++;
                        if (*l != "getAllGeodeticSubsetLists")
                        {
                            if (*l != "") *l += ",";
                            *l += kotes+itGT->getCirceId()+kotes+":"+kotes+displayText+kotes;
                        }
                    }
                }
            }
        }
        if (i==0)
        {
            OS->geodeticTransfoIdList.push_back("");
            OS->geodeticTransfoDisplayList.push_back(displayTextAuthoritative);
            ii++;
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += ":"+kotes+displayTextAuthoritative+kotes;
            }
        }
    }
    if (OS->geodeticTransfoDisplayList.size()==0) OS->iGeodeticTransfo = -1;
}

/**
* \brief Initializes the CRS data for HMIs.
* \param gss: A pointer to a geodeticSubset instance.
* \param[in] go: an instance of geodeticOptions.
*/
void GeodeticSet::getGeodeticCRSlist(geodeticSubset* gss, const circe::geodeticOptions& go,
                                     std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    gss->iGeodeticCRS = 0;
    gss->geodeticCRSidList.clear();
    gss->geodeticCRSdisplayList.clear();
    std::vector<circe::CRS>::iterator itCRS;
    for(itCRS=vCRS.begin();itCRS<vCRS.end();itCRS++)
    {
        if (itCRS->getReferenceFrameId() == go.geodeticFrameId)
        {
            gss->geodeticCRSidList.push_back(itCRS->getCirceId());
            if (itCRS->getCRStype()==circe::CT_GEOCENTRIC)
//            if (itCRS->getCRStype()==circe::CT_GEOCENTRIC && ((circe::GeodeticReferenceFrame*)itCRS->getReferenceFrame())->getSystemDimension()>circe::SD_2D)
                gss->geodeticCRSdisplayList.push_back(CRStypeDisplayText[itCRS->getCRStype()]);
            else if (itCRS->getCRStype()==circe::CT_GEOGRAPHIC)
                gss->geodeticCRSdisplayList.push_back(CRStypeDisplayText[itCRS->getCRStype()]);
            else if (itCRS->getCRStype()==circe::CT_PROJECTED)
                gss->geodeticCRSdisplayList.push_back(/*QString::fromLatin1(*/itCRS->getName());
            if (itCRS->getCirceId() == go.geodeticCRSid)
                gss->iGeodeticCRS = ii;
            ii++;
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += kotes+itCRS->getCirceId()+kotes+":"+kotes+gss->geodeticCRSdisplayList.back()+kotes;
            }
        }
    }
    if (gss->geodeticCRSdisplayList.size()==0) gss->iGeodeticCRS = -1;
}

/**
* \brief Initializes the source vertical frame data for HMIs.
* \param sourceSubset: A pointer to a geodeticSubset instance.
* \param[in] sourceOptions: an instance of geodeticOptions.
*/
void GeodeticSet::getSourceVerticalFrameList(geodeticSubset* sourceSubset,
    const circe::geodeticOptions& sourceOptions, std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    CRS *sourceGeodeticCRS = getCRS(sourceOptions.geodeticCRSid);
    sourceSubset->iVerticalFrame=0;
    sourceSubset->verticalFrameIdList.clear();
    sourceSubset->verticalFrameDisplayList.clear();
    if (!(hasVertical(sourceOptions.formatId)))
        return;
    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
        if (!(sourceOptions.useGeodeticCRS) || itVF->intersects(*sourceGeodeticCRS))
    {
        sourceSubset->verticalFrameIdList.push_back(itVF->getCirceId());
        //fromLatin1 retrieves accented characters from ANSI data (originally read in a file)
        sourceSubset->verticalFrameDisplayList.push_back(/*QString::fromLatin1(*/itVF->getName());
        if (itVF->getCirceId() == sourceOptions.verticalFrameId)
            sourceSubset->iVerticalFrame = ii;
        ii++;
        if (*l != "getAllGeodeticSubsetLists")
        {
            if (*l != "") *l += ",";
            *l += kotes+itVF->getCirceId()+kotes+":"+kotes+itVF->getName()+kotes;
        }
    }
    if (sourceSubset->verticalFrameDisplayList.size()==0) sourceSubset->iVerticalFrame = -1;
}

/**
* \brief Initializes the target vertical frame data for HMIs.
* \param targetSubset: A pointer to a geodeticSubset instance.
* \param[in] CO: an instance of circeOptions.
*/
void GeodeticSet::getTargetVerticalFrameList(geodeticSubset* targetSubset, const circe::circeOptions& CO,
                                             std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    targetSubset->iVerticalFrame=0;
    std::vector<circe::VerticalReferenceFrame>::iterator itVF;
    std::vector<circe::Transformation>::iterator itVT;
    std::vector<circe::Transformation> vTransfo;
    vTransfo.clear();
    for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
    {
        if (itVT->getSourceFrameId() == CO.sourceOptions.usesForTransformation
         || itVT->getTargetFrameId() == CO.sourceOptions.usesForTransformation
         || itVT->getNodeFrameId()   == CO.sourceOptions.usesForTransformation
         || itVT->getSourceFrameId() == CO.targetOptions.usesForTransformation
         || itVT->getTargetFrameId() == CO.targetOptions.usesForTransformation
         || itVT->getNodeFrameId()   == CO.targetOptions.usesForTransformation)
//         || itVT->getSourceFrameId() == CO.sourceOptions.verticalFrameId
//         || itVT->getTargetFrameId() == CO.sourceOptions.verticalFrameId)
        {
            vTransfo.push_back(*itVT);
        }
    }
    targetSubset->verticalFrameIdList.clear();
    targetSubset->verticalFrameDisplayList.clear();
    if (!(hasVertical(CO.targetOptions.formatId)))
        return;
    //if (vTransfo.size() == 0)
      //  return;
    for(itVF=vVerticalFrame.begin();itVF<vVerticalFrame.end();itVF++)
    {
        if (circe::hasVertical(CO.sourceOptions.formatId)
                && CO.sourceOptions.verticalFrameId == itVF->getCirceId())
        {
            targetSubset->verticalFrameIdList.push_back(itVF->getCirceId());
            targetSubset->verticalFrameDisplayList.push_back(/*QString::fromLatin1(*/itVF->getName());
            if (itVF->getCirceId() == CO.targetOptions.verticalFrameId)
                targetSubset->iVerticalFrame = ii;
            ii++;
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += kotes+itVF->getCirceId()+kotes+":"+kotes+itVF->getName()+kotes;
            }
        }
        else
        for(itVT=vTransfo.begin();itVT<vTransfo.end();itVT++)
            if (itVT->getSourceFrameId() == itVF->getCirceId()
             || itVT->getTargetFrameId() == itVF->getCirceId())
            {
                targetSubset->verticalFrameIdList.push_back(itVF->getCirceId());
                targetSubset->verticalFrameDisplayList.push_back(/*QString::fromLatin1(*/itVF->getName());
                if (itVF->getCirceId() == CO.targetOptions.verticalFrameId)
                    targetSubset->iVerticalFrame = ii;
                ii++;
                if (*l != "getAllGeodeticSubsetLists")
                {
                    if (*l != "") *l += ",";
                    *l += kotes+itVF->getCirceId()+kotes+":"+kotes+itVF->getName()+kotes;
                }
                break;
            }
    }
    if (targetSubset->verticalFrameDisplayList.size()==0) targetSubset->iVerticalFrame = -1;
}


/**
* \brief
*/
bool GeodeticSet::isVerticalFrameMatching(std::string str, STATUS st, const geodeticOptions& go)
{
    if (str == go.verticalFrameId || (st == S_AUTHORITATIVE && go.verticalFrameAuthoritative))
        return true;
    return false;
}

/**
* \brief Initializes the vertical transformation data for HMIs.
* \param OS A pointer to an operationSubset instance.
* \param[in] CO an instance of circeOptions.
*/
void GeodeticSet::getVerticalTransfoList(operationSubset *OS, const circe::circeOptions& CO,
                                         std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    int ii=0;
    OS->iVerticalTransfo=0;
    std::string displayText="", displayTextAuthoritative="";
    std::vector<circe::Transformation>::iterator itVT;
    OS->verticalTransfoIdList.clear();
    OS->verticalTransfoDisplayList.clear();
    for (int i=0; i<2; i++)
    {
        for(itVT=vVerticalTransfo.begin();itVT<vVerticalTransfo.end();itVT++)
        {
            if ((itVT->getSourceFrameId() == CO.sourceOptions.usesForTransformation
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.sourceOptions))
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.sourceOptions)
              && itVT->getTargetFrameId() == CO.sourceOptions.usesForTransformation)
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.sourceOptions)
              && itVT->getTargetFrameId() == CO.targetOptions.usesForTransformation)
             || (itVT->getSourceFrameId() == CO.targetOptions.usesForTransformation
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.sourceOptions))
             || (itVT->getSourceFrameId() == CO.sourceOptions.usesForTransformation
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.targetOptions))
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.targetOptions)
              && itVT->getTargetFrameId() == CO.sourceOptions.usesForTransformation)
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.targetOptions)
              && itVT->getTargetFrameId() == CO.targetOptions.usesForTransformation)
             || (itVT->getSourceFrameId() == CO.targetOptions.usesForTransformation
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.targetOptions))
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.sourceOptions)
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.targetOptions))
             || (isVerticalFrameMatching(itVT->getSourceFrameId(), itVT->getStatus(), CO.targetOptions)
              && isVerticalFrameMatching(itVT->getTargetFrameId(), itVT->getStatus(), CO.sourceOptions)))
            {
                if (itVT->getLayout() == circe::L_CONSTANT)
                    displayText = circe::TransfoLayoutText[circe::L_CONSTANT]+" "+circe::asString(itVT->getValuesNumber())+"-param";
                else
                    displayText = /*QString::fromLatin1((*/itVT->getCirceId()/*+" "+circe::GridTypeText[itVT->getGridType()]*/;
                // This is very important:
                // If the transformation is authoritative, let circe choose it automatically
                // (it may depend on each point location) and don't let writeCommandLine()
                // write the --transfoRequired argument.
                // On the other hand, if the transformation is not authoritative, Circe will
                // choose it only if it is parsed by the --transfoRequired argument.
                if (i==0)
                {
                    if (itVT->getStatus() == S_AUTHORITATIVE)
                        displayTextAuthoritative += displayText+"  ";
                }
                else
                {
                    if (itVT->getStatus() != circe::S_AUTHORITATIVE)
                    {
                        OS->verticalTransfoIdList.push_back(itVT->getCirceId());
                        OS->verticalTransfoDisplayList.push_back(displayText);
                        if (itVT->getCirceId() == CO.idVerticalTransfoRequired)
                            OS->iVerticalTransfo = ii;
                        ii++;
                        if (*l != "getAllGeodeticSubsetLists")
                        {
                            if (*l != "") *l += ",";
                            *l += kotes+itVT->getCirceId()+kotes+":"+kotes+displayText+kotes;
                        }
                    }
                 }
            }
        }
        if (i==0)
        {
            OS->verticalTransfoIdList.push_back("");
            OS->verticalTransfoDisplayList.push_back(displayTextAuthoritative);
            ii++;
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += ":"+kotes+displayTextAuthoritative+kotes;
            }
        }
    }
    if (OS->verticalTransfoDisplayList.size()==0) OS->iVerticalTransfo = -1;
}

/**
* \brief Initializes the format data for HMIs.
* \param[in] gd: a SYSTEM_DIMENSION value.
* \param[in] dm: a DATA_MODE value.
* \param[in] go: a geodeticOptions instance.
* \param[out] iDataFormat: index of the selected data format.
* \param[out] dataFormatList: vector of data format (underlying formats).
* \param[out] dataFormatDisplayList: vector of data format display list (seen by the user).
*/
void GeodeticSet::getDataFormatList(int* iDataFormat, std::vector<std::string>* dataFormatList,
                                    std::vector<std::string>* dataFormatDisplayList,
                                    circe::SYSTEM_DIMENSION gd,
                                    const circe::geodeticOptions& go, DATA_MODE dm,
                                    std::string* l, std::string frmt)
{
//    //test
//    std::ofstream fo;
//    fo.open("D:\\Circev5_msvc2015\\workspace\\test\\suivi_formats.log", std::ofstream::app);
//    fo << dm << " " << *l << " " << frmt << "" << std::endl;
//    fo.close();
//    //fin test
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    *iDataFormat = 0;
    dataFormatList->clear();
    dataFormatDisplayList->clear();
    int i, i1, i2, iP=0;
    switch(go.geodeticCRStype)
    {
    case circe::CT_GEOCENTRIC:
        i1 = circe::UF_UNDEF+1;
        i2 = circe::UF_GEOCENTRIC_END;
        break;
    case circe::CT_GEOGRAPHIC:
        i1 = circe::UF_GEOCENTRIC_END+1;
        iP = circe::UF_GEOGRAPHIC_POINT_END;
        i2 = circe::UF_GEOGRAPHIC_3D_BEGIN;
        break;
    case circe::CT_PROJECTED:
        i1 = circe::UF_GEOGRAPHIC_END+1;
        iP = circe::UF_PROJECTED_POINT_END;
        i2 = circe::UF_PROJECTED_3D_BEGIN;
        break;
    default:
        i1 = 0;
        i2 = 0;
        break;
    }
    for (i=i1; i<i2; i++)
    {
        if (i==iP)
        {
            if (dm == circe::DM_POINT) break;
            else continue;
        }
        //if (dm == circe::DM_POINT && i > i1+3) break;
//        if (dm == circe::DM_POINT && i > i1+1) break;
//        if (dm == circe::DM_POINT && circe::uiFormatText[i].substr(0,2) == "id") continue;
        if (vVerticalFrame.size()==0 && hasVertical(circe::uiFormat[i])) continue;
        dataFormatList->push_back(circe::uiFormat[i]);
        dataFormatDisplayList->push_back(circe::uiFormatText[i].data());
        if (*l != "getAllGeodeticSubsetLists")
        {
            if (*l != "") *l += ",";
            *l += kotes+circe::uiFormat[i]+kotes+":"+kotes+circe::uiFormatText[i]+kotes;
        }
    }
    if (gd > circe::SD_2D)
    {
        switch(go.geodeticCRStype)
        {
        case circe::CT_GEOGRAPHIC:
            i1 = circe::UF_GEOGRAPHIC_3D_BEGIN+1;
            iP = circe::UF_GEOGRAPHIC_3D_POINT_END;
            i2 = circe::UF_GEOGRAPHIC_END;
            break;
        case circe::CT_PROJECTED:
            i1 = circe::UF_PROJECTED_3D_BEGIN+1;
            iP = circe::UF_PROJECTED_3D_POINT_END;
            i2 = circe::UF_PROJECTED_END;
            break;
        default:
            i1 = 0;
            i2 = 0;
            break;
        }
        for (i=i1; i<i2; i++)
        {
            if (i==iP)
            {
                if (dm == circe::DM_POINT) break;
                else continue;
            }
            //if (dm == circe::DM_POINT && i > i1+3) break;
//            if (dm == circe::DM_POINT && i > i1+1) break;
//            if (dm == circe::DM_POINT && circe::uiFormatText[i].substr(0,2) == "id") continue;
            dataFormatList->push_back(circe::uiFormat[i]);
            dataFormatDisplayList->push_back(circe::uiFormatText[i].data());
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += kotes+circe::uiFormat[i]+kotes+":"+kotes+circe::uiFormatText[i]+kotes;
            }
        }
    }
    if (dm == circe::DM_FILE)
    {
#ifdef WITH_COMPLEX_FORMATS
        for (i=circe::UF_PROJECTED_END+1; i<circe::UF_COUNT; i++)
#else
        for (i=circe::UF_PROJECTED_END+1; i<circe::UF_PROJECTED_END; i++)
#endif
        {
            dataFormatList->push_back(circe::uiFormat[i]);
            dataFormatDisplayList->push_back(circe::uiFormatText[i].data());
            if (*l != "getAllGeodeticSubsetLists")
            {
                if (*l != "") *l += ",";
                *l += kotes+circe::uiFormat[i]+kotes+":"+kotes+circe::uiFormatText[i]+kotes;
            }
        }
    }
    std::string str = rmExtraFormat(go.formatId);
    for (int i=0;i<dataFormatList->size();i++)
        if ((*dataFormatList)[i]==str)
            *iDataFormat=i;
}

/**
* \brief Initializes the unit data for HMIs.
* \param gss: a geodeticSubset instance.
* \param[in] what: "source" or "target".
* \param[in] go: a geodeticOptions instance.
*/
void GeodeticSet::getDataUnitList(std::string what, geodeticSubset* gss, const circe::geodeticOptions& go,
                                  std::string* l, std::string frmt)
{
    std::string kotes = "";
    if (frmt == "JSON")
        kotes = "\"";
    gss->iDataUnit = 0;
    gss->dataUnitList.clear();
    gss->dataUnitDisplayList.clear();
    int i, i1, i2;
    switch(go.geodeticCRStype)
    {
    case circe::CT_GEOCENTRIC:
        i1 = 0;
        i2 = 0;
        break;
    case circe::CT_GEOGRAPHIC:
        i1 = circe::MU_UNDEF+1;
        i2 = circe::MU_METER;
        break;
    case circe::CT_PROJECTED:
        if (what=="target")
        {
            i1 = circe::MU_UNDEF+1;
            i2 = circe::MU_METER;
        }
        else
        {
            i1 = 0;
            i2 = 0;
        }
        break;
    default:
        i1 = 0;
        i2 = 0;
        break;
    }
    for (i=i1; i<i2; i++)
    {
        gss->dataUnitList.push_back(circe::MeasureUnitAttributeText[i].data());
        gss->dataUnitDisplayList.push_back(circe::MeasureUnitDisplayText[i].data());
        if (*l != "getAllGeodeticSubsetLists")
        {
            if (*l != "") *l += ",";
            *l += kotes+circe::MeasureUnitAttributeText[i]+kotes+":"+kotes+circe::MeasureUnitDisplayText[i]+kotes;
        }
        if ((circe::MeasureUnitAttributeText[i] == go.angularUnit))
            gss->iDataUnit = i-1;
    }
}

/**
* \brief Initializes geodetic ids.
* \param gs: a geodeticSubset instance.
* \param go: a geodeticOptions instance.
*/
void GeodeticSet::initGeodeticIds(circe::geodeticOptions* go, geodeticSubset* gs)
{
    GeodeticReferenceFrame *geodeticFrame = NULL;
    if (go->geodeticCRSid == "")
        go->geodeticCRSid = getCRSid(go->geodeticFrameId);
    CRS* geodeticCRS = getCRS(go->geodeticCRSid);
    if (geodeticCRS != NULL)
        geodeticFrame = (GeodeticReferenceFrame*)geodeticCRS->getReferenceFrame();
    int i=0;
    //IGNF.xml: possible GeocentricCRS with usesGeodeticDatum == ERROR_EMPTY_xlink:href
    while (geodeticCRS == NULL || geodeticFrame == NULL)
    {
        geodeticCRS = &(vCRS[i++]);
        geodeticFrame = (GeodeticReferenceFrame*)geodeticCRS->getReferenceFrame();
    }
    go->geodeticCRSid = geodeticCRS->getCirceId();
    go->geodeticFrameId = geodeticFrame->getCirceId();
    go->geodeticCRStype = geodeticCRS->getCRStype();
    go->usesForTransformation = geodeticFrame->getUsesForTransformation();
    gs->geodeticDimension = geodeticFrame->getSystemDimension();

    if (go->verticalFrameId == "" && go->verticalCRSid != "")
    {
        VerticalReferenceFrame *verticalFrame = NULL;
        CRS* verticalCRS = getCRS(go->verticalCRSid);
        if (verticalCRS != NULL)
        {
            verticalFrame = (VerticalReferenceFrame*)verticalCRS->getReferenceFrame();
            go->verticalFrameId = verticalFrame->getCirceId();
        }
    }
}

} //namespace circe
