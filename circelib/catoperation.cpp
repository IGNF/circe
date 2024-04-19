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
* \file catoperation.cpp
* \brief Implementation of the CatOperation class.
*/
#include <sys/stat.h>
#include "catoperation.h"
namespace circe
{

/**
* \brief Destructor.
*/
CatOperation::~CatOperation()
{

}

/**
* \brief Copy constructor.
*/
CatOperation::CatOperation(const CatOperation &B) : CirceObject(B)
{
    TransfoRequired = B.TransfoRequired;
    vRelevantGeodeticTransfo = B.vRelevantGeodeticTransfo;
    vRelevantVertical2VerticalTransfo = B.vRelevantVertical2VerticalTransfo;
    vRelevantVerticalGeodetic1Transfo = B.vRelevantVerticalGeodetic1Transfo;
    vRelevantVerticalGeodetic2Transfo = B.vRelevantVerticalGeodetic2Transfo;
    sourceMeridianTranslation = B.sourceMeridianTranslation;
    targetMeridianTranslation = B.targetMeridianTranslation;
    SourceConversion = B.SourceConversion;
    TargetConversion = B.TargetConversion;
    SourceGeodeticFrame = B.SourceGeodeticFrame;
    TargetGeodeticFrame = B.TargetGeodeticFrame;
    SourceVerticalFrame = B.SourceVerticalFrame;
    TargetVerticalFrame = B.TargetVerticalFrame;
    bGeographicCoordRequiredForGeodeticTransfo = B.bGeographicCoordRequiredForGeodeticTransfo;
    CurrentCRStype = B.CurrentCRStype;
    TargetCRStype1 = B.TargetCRStype1;
    SourceCRStype2 = B.SourceCRStype2;
    TargetCRStype2 = B.TargetCRStype2;
    noSourceEllHeight = B.noSourceEllHeight;
    noTargetEllHeight = B.noTargetEllHeight;
    noSourceVertical = B.noSourceVertical;
    noTargetVertical = B.noTargetVertical;
    SourceGeodeticCRS = B.SourceGeodeticCRS;
    TargetGeodeticCRS = B.TargetGeodeticCRS;
    SourceVerticalCRS = B.SourceVerticalCRS;
    TargetVerticalCRS = B.TargetVerticalCRS;
    operationMessage = B.operationMessage;
    sourceLinearUnitId = B.sourceLinearUnitId;
    sourceAngularUnitId = B.sourceAngularUnitId;
    targetLinearUnitId = B.targetLinearUnitId;
    targetAngularUnitId = B.targetAngularUnitId;
    sourceEpoch = B.sourceEpoch;
    targetEpoch = B.targetEpoch;
    operationOutput = B.operationOutput;
    extraOut = B.extraOut;
}

/**
* \brief Empties values of the members of a CatOperation instance.
*/
void CatOperation::reset()
{
    CirceObject::reset();
    TransfoRequired = 0;
    std::vector<Transformation>::iterator itT;
//    for(itT=vRelevantGeodeticTransfo.begin();itT!=vRelevantGeodeticTransfo.end();itT++)
//        itT->reinit();
//    for(itT=vRelevantVertical2VerticalTransfo.begin();itT!=vRelevantVertical2VerticalTransfo.end();itT++)
//        itT->reinit();
//    for(itT=vRelevantVerticalGeodetic1Transfo.begin();itT!=vRelevantVerticalGeodetic1Transfo.end();itT++)
//        itT->reinit();
//    for(itT=vRelevantVerticalGeodetic2Transfo.begin();itT!=vRelevantVerticalGeodetic2Transfo.end();itT++)
//        itT->reinit();

    vRelevantGeodeticTransfo.clear();
    vRelevantVertical2VerticalTransfo.clear();
    vRelevantVerticalGeodetic1Transfo.clear();
    vRelevantVerticalGeodetic2Transfo.clear();
    sourceMeridianTranslation = targetMeridianTranslation = 0.;
    SourceConversion = TargetConversion = NULL;
    SourceGeodeticFrame = TargetGeodeticFrame = NULL;
    SourceVerticalFrame = TargetVerticalFrame = NULL;
    bGeographicCoordRequiredForGeodeticTransfo = false;
    CurrentCRStype = TargetCRStype1 = SourceCRStype2 = TargetCRStype2 = CT_UNDEF;
    noSourceEllHeight = noTargetEllHeight = noSourceVertical = noTargetVertical = true;
    SourceGeodeticCRS = TargetGeodeticCRS = SourceVerticalCRS = TargetVerticalCRS = NULL;
    operationMessage = OM_OK;
    sourceLinearUnitId = sourceAngularUnitId = targetLinearUnitId = targetAngularUnitId = MU_UNDEF;
    sourceEpoch = targetEpoch = 0.;
    operationOutput = "";
    extraOut = false;
}

/**
* \brief Checks wether a transformation is relevant or not.
* \param[in] itT: iterator pointing to a Transformation.
* \param[in] id1u: id of the source frame.
* \param[in] id2u: id of the target frame.
* \param[in] ct: CRS_TYPE.
* \param[out] idG: code of the grid nodes' geodetic frame if needed.
* \return true if the transformation is relevant.
*/
/* If we arrived here, this test was passed (not "continued"):
                if ((idVerticalTransfoRequired!="" && idVerticalTransfoRequired!=itT->getCirceId())
                 || (idVerticalTransfoRequired=="" && itT->getStatus()!=S_AUTHORITATIVE)) continue;
  so if (id1u == "AUTHORITATIVE") or (id2u == "AUTHORITATIVE")
  then itT->getStatus()==S_AUTHORITATIVE
 */
bool CatOperation::IsTransfoMatching(std::vector<Transformation>::iterator itT, std::string id1u,
                       std::string id2u, std::string *idG, CRS_TYPE ct)
{
    bool bFound = false;
    if (itT->getCRStype()!=ct && ct==CT_VERTICAL) return false;
    std::string idT1 = itT->getSourceFrameId();
    std::string idT2 = itT->getTargetFrameId();
    if (((idT1==id1u||id1u=="AUTHORITATIVE")&&(idT2==id2u))
     || ((idT1==id1u)&&(idT2==id2u||id2u=="AUTHORITATIVE")))
    {
        bFound = true;
        itT->setOperationApplication("DIRECT");
    }
    else if (itT->IsReversible()
        && (((idT1==id2u||id2u=="AUTHORITATIVE")&&(idT2==id1u))
         || ((idT1==id2u)&&(idT2==id1u||id1u=="AUTHORITATIVE"))))
    {
        bFound = true;
        itT->setOperationApplication(itT->getOperationReversibility());
    }

    if (bFound)
    {
        *idG=itT->getNodeFrameId();
    }

    return bFound;
}

/**
* \brief Initializes vectors of transformations.
* \param[in] idG1: code of the source geodetic frame.
* \param[in] idG2: code of the target geodetic frame.
* \param[in] idV1: code of the source vertical frame.
* \param[in] idV2: code of the target vertical frame.
* \param[in] GS: a GeodeticSet loaded from a metadata file.
* \param[in] idGeodeticTransfoRequired: code of the geodetic transformation required by the user (optional). If the user does not require any transformation, the authoritative ones will be used.
* \param[in] idVerticalTransfoRequired: code of the vertical transformation required by the user (optional). If the user does not require any transformation, the authoritative ones will be used.
* \param[in] lt: a LOADING_TYPE to load relevant grids.
* \param[in] forceIdentity: true if the identity of idG1 and idG2 is accepted (case of a plate motion model).
* \param[out] this->TransfoRequired: bitmask for required types of transformations.
* \param[out] this->vRelevantGeodeticTransfo: vector of relevant geodetic transformations.
* \param[out] this->vRelevantVertical2VerticalTransfo: vector of relevant vertical/vertical transformations.
* \param[out] this->vRelevantVerticalGeodetic1Transfo: vector of relevant vertical/source geodetic transformations.
* \param[out] this->vRelevantVerticalGeodetic2Transfo: vector of relevant vertical/target geodetic transformations.
* \return true if at least one of the four relevant transformation vectors has been initialized.
*/
//GS is modified (some data members of Transformation objects)
bool CatOperation::GetTransformation (const std::string& idG1, const std::string& idG2,
                        const std::string& idV1, const std::string& idV2, GeodeticSet GS, LOADING_TYPE lt,
                        const std::string& idGeodeticTransfoRequired,
                        const std::string& idVerticalTransfoRequired, bool forceIdentity)
{
    bool bSearchForVerticalVertical=false, bSourceVertEqTargetVert=false;
    bool bSearchForGeodetic1Vertical=false, bSearchForGeodetic2Vertical=false;
    bool bSearchForVerticalGeodetic1=false, bSearchForVerticalGeodetic2=false;
    bool bGeodeticTransfoFound=false, bVerticalTransfoFound=false;
    std::vector<Transformation>::iterator itT;
    std::string idG;
//    mb->messageSend("inside GetTransformation  ...");

    TransfoRequired = T_UNDEF;
    //Is a geodetic transformation required ?
    if (idG1!="" && idG2!="" && (idG1!=idG2 || forceIdentity))
    //Yes if the geodetic frames are initialized and different
    {
        TransfoRequired |= T_GEODETIC_REQUIRED;
        //Search the available transformations
        for(itT=GS.vGeodeticTransfo.begin();itT!=GS.vGeodeticTransfo.end();itT++)
        {
            //This may be a relevant transformation in two cases:
            // - the user has required a transformation and this is it;
            // - the user didn't require a transfo and this one is authoritative.
            if ((idGeodeticTransfoRequired!="" && idGeodeticTransfoRequired!=itT->getCirceId())
             || (idGeodeticTransfoRequired=="" && itT->getStatus()!=S_AUTHORITATIVE)) continue;
            if (!IsTransfoMatching(itT, idG1, idG2, &idG, CT_UNDEF)) continue;
            bGeodeticTransfoFound = true;
            //if (itT->getLayout() != L_CONSTANT)
            itT->mb = mb;
            itT->LoadGrid(lt); //problem for big grids ?
            itT->setArea();
            vRelevantGeodeticTransfo.push_back(*itT);
        }
        if (!bGeodeticTransfoFound)
        {
            operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
            return false;
        }
    }

    //Is a vertical transformation required ?
    if ((idV1!="" || idV2!="") && !(!(TransfoRequired&T_GEODETIC_REQUIRED) && noTargetEllHeight && noTargetVertical))
    {
        //oui si au moins un des deux reperes verticaux est initialise et si les deux sont differents
        TransfoRequired |= T_VERTICAL_REQUIRED;
        if (idV1!="" && idV2!="" && (idV1!=idV2 || forceIdentity))
            bSearchForVerticalVertical = true;
        //Si les deux reperes sont initialises, c'est que l'utilisateur demande une transfo entre reperes verticaux.
        else
        {
            if (idV1!="" && idV2!="" && idV1==idV2)
                bSourceVertEqTargetVert = true;
            //Sinon, si au moins un repere geodesique est initialise et qu'il y a une hauteur,
            //c'est une transfo hauteur=>altitude ou altitude=>hauteur.
//Test supprimé provisoirement car certains tests ne passent pas.
//En effet, en cas de transformation géodésique, des altitudes peuvent être
//transformées en hauteur pour réaliser la transformation, même si la cible
//verticale est aussi une altitude (à verifier).
//Ex: --sourceCRS=RGTAAFUTM42S.AUTHORITATIVE --sourceFormat=ENV.METERS. --targetCRS=KERGK0UTM42S.AUTHORITATIVE --targetFormat=ENV.METERS. --metadataFile=DataKERnew.txt 599797.212 4560172.072 20.000
//            if (!noSourceEllHeight || !noTargetEllHeight)
//            {
                if (idG1!="" && idV2!="" && !noSourceEllHeight && !bSourceVertEqTargetVert)
                    bSearchForGeodetic1Vertical = true;
                if (idG1!="" && idV1!="")
                    bSearchForVerticalGeodetic1 = true;
                if (TransfoRequired&T_GEODETIC_REQUIRED && idV2!="" && !bSourceVertEqTargetVert)
                    bSearchForGeodetic2Vertical = true;
                if (TransfoRequired&T_GEODETIC_REQUIRED && idV1!="")
                    bSearchForVerticalGeodetic2 = true;
            }
//        }
        //Search the available transformations
        for(itT=GS.vVerticalTransfo.begin();itT!=GS.vVerticalTransfo.end();itT++)
        {
            //This may be a relevant transformation in two cases:
            // - the user has required a transformation and this is it;
            // - the user didn't require a transfo and this one is authoritative.
            if ((idVerticalTransfoRequired!="" && idVerticalTransfoRequired!=itT->getCirceId())
             || (idVerticalTransfoRequired=="" && itT->getStatus()!=S_AUTHORITATIVE)) continue;
            //source vert.h. => target vert.h.
            if (bSearchForVerticalVertical )
            {
                if (IsTransfoMatching(itT, idV1, idV2, &idG, CT_VERTICAL))
                {
                    //Indication of the geodetic frame required for grid interpolation if needed
                    if (idG == "") TransfoRequired |= T_VERTICAL_VERTICAL_GEOD0_REQUIRED;
                    else if (idG == idG1) TransfoRequired |= T_VERTICAL_VERTICAL_GEOD1_REQUIRED;
                    else if (idG == idG2) TransfoRequired |= T_VERTICAL_VERTICAL_GEOD2_REQUIRED;
                    else continue;
                    itT->mb = mb;
                    itT->LoadGrid(lt); //problem for big grids ?
                    itT->setArea();
                    vRelevantVertical2VerticalTransfo.push_back(*itT);
                    bVerticalTransfoFound = true;
                }
            }
            //source ell.h. => target vert.h.
            if (bSearchForGeodetic1Vertical)
            {
                if (IsTransfoMatching(itT, idG1, idV2, &idG, CT_VERTICAL))
                {
                    TransfoRequired |= T_GEODETIC1_VERTICAL_REQUIRED;
                    itT->mb = mb;
                    itT->LoadGrid(lt); //problem for big grids ?
                    itT->setArea();
                    vRelevantVerticalGeodetic1Transfo.push_back(*itT);
                    bVerticalTransfoFound = true;
                }
            }
            //target ell.h. => target vert.h.
            if (bSearchForGeodetic2Vertical)
            {
                if (IsTransfoMatching(itT, idG2, idV2, &idG, CT_VERTICAL))
                {
                    TransfoRequired |= T_GEODETIC2_VERTICAL_REQUIRED;
                    itT->mb = mb;
                    itT->LoadGrid(lt); //problem for big grids ?
                    itT->setArea();
                    vRelevantVerticalGeodetic2Transfo.push_back(*itT);
                    bVerticalTransfoFound = true;
                }
            }
            //source vert.h. => source ell.h.
            if (bSearchForVerticalGeodetic1)
            {
                if (IsTransfoMatching(itT, idV1, idG1, &idG, CT_VERTICAL))
                {
                    TransfoRequired |= T_VERTICAL_GEODETIC1_REQUIRED;
                    itT->mb = mb;
                    itT->LoadGrid(lt); //problem for big grids ?
                    itT->setArea();
                    vRelevantVerticalGeodetic1Transfo.push_back(*itT);
                    bVerticalTransfoFound = true;
                }
            }
            //source vert.h. => target ell.h.
            if (bSearchForVerticalGeodetic2)
            {
                if (IsTransfoMatching(itT, idV1, idG2, &idG, CT_VERTICAL))
                {
                    TransfoRequired |= T_VERTICAL_GEODETIC2_REQUIRED;
                    itT->mb = mb;
                    itT->LoadGrid(lt); //problem for big grids ?
                    itT->setArea();
                    vRelevantVerticalGeodetic2Transfo.push_back(*itT);
                    bVerticalTransfoFound = true;
                }
            }
        }
        if (idV1!=idV2 && !bVerticalTransfoFound)
        {
            operationMessage = OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
            return false;
        }
    }

    return true;
}

/**
* \brief Initializes a vector of transformations between two realizations of the ITRS.
* \param[in] idG1: id of the source ITRS realization (ITRFyy1).
* \param[in] idG2: id of the target ITRS realization (ITRFyy2).
* \param[in] sourceEpoch: the epoch of the source coordinates.
* \param[in] targetEpoch: the epoch of the target coordinates.
* \param[in] GS: a GeodeticSet loaded from a metadata file, containing inter alia the plate motion
* models expressed in an intermediary ITRS realization (ITRFyyI).
* \param[in] boundaryFilename: the path to the tectonic plate boundary file.
* \param[out] this->vRelevantGeodeticTransfo vector of relevant transformations (one per plate),
* compositions of ITRFyy1/ITRFyyI(sourceEpoch) + ITRFyyI(sourceEpoch/targetEpoch)
* + ITRFyyI/ITRFyy2 (targetEpoch), with boundaries attached.
* \return true if a relevant transformation vector has been initialized.
*/
bool CatOperation::GetITRFtransformation(const std::string& idG1, double sourceEpoch,
                                         const std::string& idG2, double targetEpoch,
                            GeodeticSet GS, const std::string& boundaryFilename, LOADING_TYPE lt)
{
	std::vector<Transformation>::iterator itT;
	TransfoRequired = T_UNDEF;
	int drctn = 1;
	double values[7] = { 0, 0, 0, 0, 0, 0, 0};
	std::string idGp = "";
	std::vector<poly> polys;
	pt4d pt;
    bool bRelevantTransfoFound;
	//if (sourceEpoch > 1900 && sourceEpoch < 2100)
	//if (targetEpoch > 1900 && targetEpoch < 2100)

	TransfoRequired |= T_GEODETIC_REQUIRED;
	operationMessage = OM_OK;

	//idGp: id of intermediary ITRS realization with plate motion models (one PMM per plate).
	//GS has plate motion models for one and only one Geodetic Frame => get the first one.
	//  for (itT = GS.vGeodeticTransfo.begin(); itT != GS.vGeodeticTransfo.end(); itT++)
	//  	if (itT->getDescription().substr(0, 18) == "plate motion model")
    //  {
    //      idGp = itT->getSourceFrameId();
    //      break;
    //  }
	//  if (itT == GS.vGeodeticTransfo.end())
	//  	return false;

	idGp = GS.getKeyGeodeticFrame();
	if (idGp == "")
		return false;

    if (idG1 != idGp)
    {
        vRelevantGeodeticTransfo.clear();
        bRelevantTransfoFound = GetTransformation(idG1, idGp, "", "", GS, lt, "", "", true);
        if (bRelevantTransfoFound)
        {
            for (itT = vRelevantGeodeticTransfo.begin(); itT != vRelevantGeodeticTransfo.end(); itT++)
                if (itT->getDescription().substr(0, 20) == "frame transformation")
            {
                itT->toEpoch(sourceEpoch);
                break;
            }
            drctn = (itT->getOperationApplication() != OA_DIRECT) ? -1 : 1;
            for (int i = 0; i < 7; i++)
				values[i] += drctn*itT->getInitValues()[i];
        }
        else
		{
			operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
			return false;
		}
	}

    if (idG2 != idGp)
    {
        vRelevantGeodeticTransfo.clear();
        bRelevantTransfoFound = GetTransformation(idGp, idG2, "", "", GS, lt, "", "", true);
        if (bRelevantTransfoFound)
        {
            for (itT = vRelevantGeodeticTransfo.begin(); itT != vRelevantGeodeticTransfo.end(); itT++)
                if (itT->getDescription().substr(0, 20) == "frame transformation")
            {
                itT->toEpoch(targetEpoch);
                break;
            }
            drctn = (itT->getOperationApplication() != OA_DIRECT) ? -1 : 1;
            for (int i = 0; i < 7; i++)
				values[i] += drctn*itT->getInitValues()[i];
        }
        else
		{
			operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
			return false;
		}
	}

    vRelevantGeodeticTransfo.clear();
    double de = targetEpoch - sourceEpoch;
    if (de != 0)
    {
        //PMM: source frame == target frame
        bRelevantTransfoFound = GetTransformation(idGp, idGp, "", "", GS, lt, "", "", true);
        if (bRelevantTransfoFound)
        {
            bool bBoundaryFileRequired = false;
            for (itT = vRelevantGeodeticTransfo.begin(); itT < vRelevantGeodeticTransfo.end(); itT++)
            {
                if (itT->getDescription().substr(0, 18) == "plate motion model")
                {
                    //A plate motion model can be combined with another constant transformation.
                    bBoundaryFileRequired = true;
                    break;
                }
                else
                {
                    //A grid motion model cannot be combined with another transformation.
                    //Epoch change in a single frame is accepted.
                    if ((idG1 != idGp || idG2 != idGp) && de != 0)
                    {
                        operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
                        return false;
                    }
                    if (itT->getDescription().substr(0, 17) == "grid motion model" || itT->getLayout() < L_CONSTANT)
                    {
                        itT->setRateFactor(asString(targetEpoch-sourceEpoch));
                    }
                }
            }
            struct stat buffer;
            if (bBoundaryFileRequired)
            {
                if ( boundaryFilename=="" || stat((GS.metadataPath+boundaryFilename).c_str(),&buffer)!=0)
                    throw std::string(ArgumentValidationText[AV_BAD_BOUNDARYFILENAME]);
                else
                    CirceBoundedObject::loadPolygons(GS.metadataPath+boundaryFilename, polys, pt, 0);
            }
            for (itT = vRelevantGeodeticTransfo.begin(); itT < vRelevantGeodeticTransfo.end(); itT++)
            {
                //Combination of the 3 (at most) transformation parameter sets
                itT->toEpoch(de, values);
                if (itT->getDescription().substr(0, 18) == "plate motion model")
                {
                    //Attach the boundary
                    itT->setHasPolygonBound(true);
                    for (int iPoly = 0; iPoly < polys.size(); iPoly++)
                        if (std::string("PMM") + polys[iPoly].id == itT->getIGNid())
                            itT->setPolygonBound(&(polys[iPoly]));
                }
            }

        }
        else
            return false;
    }
    else
    {
        bRelevantTransfoFound = GetTransformation(idGp, idGp, "", "", GS, lt, "", "", true);
        for (itT = vRelevantGeodeticTransfo.begin(); itT < vRelevantGeodeticTransfo.end(); itT++)
        {
            //Combination of the 3 (at most) transformation parameter sets
            itT->toEpoch(de, values);
        }
    }

	return true;
}

/**
* \brief Initializes a vector of transformations between two realizations of the ITRS.
* \param[in] idG1: id of the source ITRS realization.
* \param[in] idG2: id of the target ITRS realization.
* \param[in] sourceEpoch: the epoch of the source coordinates.
* \param[in] targetEpoch: the epoch of the target coordinates.
* \param[in] GS: a GeodeticSet loaded from a metadata file, containing inter alia the (grid or
* plate) motion models expressed in the key ITRS realization.
* \param[in] boundaryFilename: the path to the tectonic plate boundary file.
* \param[out] this->vRelevantGeodeticTransfo vector of relevant transformations.
* \return true if a relevant transformation vector has been initialized.
* \remark There are two exclusive cases :
* -# idG1 != idG2 and targetEpoch == sourceEpoch :
*   vRelevantGeodeticTransfo contains the transformation at the source epoch.
* -# idG1 == idG2 == GS.keyGeodeticFrame and targetEpoch != sourceEpoch :
*   vRelevantGeodeticTransfo contains one transformation per plate (with the epoch difference
*   applied and the boundaries attached) or a grid motion model.
*/
bool CatOperation::Get4Dtransformation(const std::string& idG1, double sourceEpoch, const std::string& idG2, double targetEpoch,
                            GeodeticSet GS, const std::string& boundaryFilename, LOADING_TYPE lt)
{
    std::vector<Transformation>::iterator itT;
    TransfoRequired = T_UNDEF;
    std::vector<poly> polys;
    bool bRelevantTransfoFound;
    pt4d pt;
    double values[7] = { 0, 0, 0, 0, 0, 0, 0};
    //if (sourceEpoch > 1900 && sourceEpoch < 2100)
    //if (targetEpoch > 1900 && targetEpoch < 2100)

    double de = targetEpoch - sourceEpoch;

    TransfoRequired |= T_GEODETIC_REQUIRED;
    vRelevantGeodeticTransfo.clear();
    if (idG1 != idG2 && de != 0.)
    {
        bRelevantTransfoFound = GetTransformation(idG1, idG2, "", "", GS, lt, "", "", false);
        operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
        return false;
    }
    else if (idG1 == idG2 && de == 0.)
    {
//        bRelevantTransfoFound = GetTransformation(idG1, idG2, "", "", GS, lt, "", "", false);
//        operationMessage = OM_OK;
//        return true;
        return false; //because GetTransformation will be called after, with a full argument list.
    }
    else if (idG1 != idG2)
    {
        bRelevantTransfoFound = GetTransformation(idG1, idG2, "", "", GS, lt, "", "", false);
        if (bRelevantTransfoFound)
        {
            for (itT = vRelevantGeodeticTransfo.begin(); itT != vRelevantGeodeticTransfo.end(); itT++)
                if (itT->getDescription().substr(0, 20) == "frame transformation")
            {
                itT->toEpoch(sourceEpoch);
                break;
            }
            // drctn = (itT->getOperationApplication() != OA_DIRECT) ? -1 : 1;
            // for (int i = 0; i < 7; i++)
                // values[i] += drctn*itT->getInitValues()[i];
        }
        else
        {
            operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
            return false;
        }
    }
    else if (de != 0)
    {
        bRelevantTransfoFound = GetTransformation(idG1, idG2, "", "", GS, lt, "", "", true);
        //PMM: source frame == target frame
        if (idG1 != GS.getKeyGeodeticFrame())
        {
            operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
            return false;
        }
        if (bRelevantTransfoFound)
        {
            bool bBoundaryFileRequired = false;
            for (itT = vRelevantGeodeticTransfo.begin(); itT < vRelevantGeodeticTransfo.end(); itT++)
            {
                if (itT->getDescription().substr(0, 18) == "plate motion model")
                {
                    bBoundaryFileRequired = true;
                    break;
                }
                else if (itT->getDescription().substr(0, 17) == "grid motion model"
                         || itT->getLayout() < L_CONSTANT)
                    itT->setRateFactor(asString(de));
            }
            struct stat buffer;
            if (bBoundaryFileRequired)
            {
                if ( boundaryFilename=="" || stat((GS.metadataPath+boundaryFilename).c_str(),&buffer)!=0)
                    throw std::string(ArgumentValidationText[AV_BAD_BOUNDARYFILENAME]);
                else
                    CirceBoundedObject::loadPolygons(GS.metadataPath+boundaryFilename, polys, pt, 0);
            }
            for (itT = vRelevantGeodeticTransfo.begin(); itT < vRelevantGeodeticTransfo.end(); itT++)
            {
                if (itT->getDescription().substr(0, 18) == "plate motion model")
                {
                    itT->toEpoch(de, values);
                    //Attach the boundary
                    itT->setHasPolygonBound(true);
                    for (int iPoly = 0; iPoly < polys.size(); iPoly++)
                        if (std::string("PMM") + polys[iPoly].id == itT->getIGNid())
                            itT->setPolygonBound(&(polys[iPoly]));
                }
            }
        }
        else
        {
            operationMessage = OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS;
            return false;
        }
    }
    return true;
}

/**
* \brief Initializes the Conversion	associated with a CRS, possibly as one of its child classes if it's a projection.
* \param geodeticCRS: a pointer to a CRS instance.
* \return A pointer to a Conversion instance.
*/
//This function modifies conv = geodeticCRS->getDefiningConversion() which may be a pointer
//to a member of a GeodeticSet.
Conversion* CatOperation::InitConversion(CRS* geodeticCRS)
{
    std::string projectionMethodId="";
    circe::Conversion *myConversion, *conv;
    circe::GeodeticReferenceFrame *geodeticFrame = (GeodeticReferenceFrame*)geodeticCRS->getReferenceFrame();
    if (geodeticCRS->getCRStype() == CT_PROJECTED)
    {
        conv = geodeticCRS->getDefiningConversion();
		if (conv->getPrimeMeridianId() != "")
		{
			if (geodeticFrame->getGeodeticReferenceSystem()->getPrimeMeridianId()
				!= conv->getPrimeMeridianId())
				conv->setAddFromGreenwich(true);
			else
				conv->setAddFromGreenwich(false);
		}
		projectionMethodId = conv->getInitParamMethodId();
		double _a = geodeticFrame->get_a();
		double _e2 = geodeticFrame->get_e2();
		if (projectionMethodId == LCCTID || projectionMethodId == LCCTIB) myConversion = new ProjectionLCC(*conv,_a,_e2);
		else if (projectionMethodId == LCCSID) myConversion = new ProjectionLCC(*conv, _a, _e2);
		else if (projectionMethodId == UTMNID) myConversion = new ProjectionTMerc(*conv, _a, _e2);
		else if (projectionMethodId == UTMSID) myConversion = new ProjectionTMerc(*conv, _a, _e2);
        else if (projectionMethodId == GLSBID) myConversion = new ProjectionGLab(*conv, _a, _e2);
        else if (projectionMethodId == GLSEID) myConversion = new ProjectionGLab(*conv, _a, _e2);
        else if (projectionMethodId == GLSCID) myConversion = new ProjectionGLab(*conv, _a, _e2);
        else if (projectionMethodId == LAEAID) myConversion = new ProjectionLAEA(*conv, _a, _e2);
        else if (projectionMethodId==SMCSID || projectionMethodId==SPSSID
			|| projectionMethodId == SPSTID) myConversion = new ProjectionSouthPolarStereographic(*conv, _a, _e2);
        else
            throw std::string("InitConversion: DefiningConversion->initParamMethodId ")+ErrorMessageText[EM_UNINITIALIZED];
	}
    else
    {
        myConversion = new Conversion();
        myConversion->set_a(geodeticFrame->get_a());
        myConversion->set_e2(geodeticFrame->get_e2());
        myConversion->InitParam("", "", "");
    }
    return myConversion;
}

/**
* \brief Checks wether an epoch is valid and associated with a 4D frame.
* \param ep: a double that represents an epoch (a decimal year).
* \param dim: a SYSTEM_DIMENSION.
* \return true if the epoch is valid (arbitrarily between 1900 and 2100) and the frame dimension is 4D.
*/
std::string CatOperation::check4thD(double *ep, circe::SYSTEM_DIMENSION dim)
{
//    if (dim != SD_4D)
//        *ep = 0;
//    if (*ep == 0)
//		return "";
    if (dim != SD_4D)
    {
        *ep = 0;
        return "";
    }
    if (*ep > 1900 && *ep < 2100 && dim == SD_4D)
		return "4Dok";
	else
		return "4Dfail";
}

/**
* \brief Search the first vector for a transformation whose description contains
*        a string like ">>>grid_name<<<" where grid_name is a grid file name without extension.
*        If found, search the second vector for a transformation whose grid file name
*        is equal to the name found plus the extension (.tbc).
*        Context : A new grid of geoid associated with a new geodetic reference frame ;
*                  and a transformation between the the two frames is requested,
*                  along with a vertical transformation.
* \param std::vector<Transformation> *v1.
* \param std::vector<Transformation> *v2.
* \return "yes" if everything is found which means that the first transformation
*        is superseded by the second, which is interpreted in this version as if
*        the first vector as a whole was superseded (because in general there is a single element).
*/
bool CatOperation::eraseSupersededGrid(std::vector<Transformation> *v1, std::vector<Transformation> *v2)
{
    std::vector<Transformation>::iterator itT1, itT2;
    size_t posI = std::string::npos, posF = std::string::npos;
    std::string gridName="";
    for(itT1=v1->begin();itT1!=v1->end();itT1++)
    {
        if ((posI = itT1->getDescription().find(">>>")) != std::string::npos
          &&(posF = itT1->getDescription().find("<<<")) != std::string::npos)
        {
            gridName = itT1->getDescription().substr(posI+3,posF-posI-3)+"."+ListeExtensionTypeGrille[GF_SGN_BIN];
            for(itT2=v2->begin();itT2!=v2->end();itT2++)
                if (gridName == itT2->getGridFile())
                    return true;
        }
    }
    return false;
}


/**
* \brief CatOperation constructor.
* \param GS: a GeodeticSet loaded from a metadata file.
* \param circopt: a set of circeOptions that will be used to select the relevant data from GS.
* \return An instance of CatOperation designed to operate points according to circopt.
*/
CatOperation::CatOperation(const circeOptions &circopt, GeodeticSet& GS)
{
    Init(circopt, GS);
}
void CatOperation::Init(const circeOptions &circopt, GeodeticSet& GS)
{

    bool bRelevantTransfoFound;
    std::string idV1 = "", idV2 = "", id1u="", id2u="";
	std::vector<Transformation>::iterator itT;
    double epF, epT;

//    try
//    {
        reset();

        //if (GS.mb!=NULL)
            //GS.mb->messageEdit("inside CatOperation !!!");
        if (GS.mb!=NULL)
            mb = GS.mb;
        //if (GS.mb!=NULL)
            //mb->messageSend("Creating  ...");

        operationOutput = displayOptionsJSON(circopt);

        //units
        sourceLinearUnitId = (MEASURE_UNIT)getIndex(circopt.sourceOptions.linearUnit,MeasureUnitAttributeText,MU_COUNT);
        sourceAngularUnitId = (MEASURE_UNIT)getIndex(circopt.sourceOptions.angularUnit,MeasureUnitAttributeText,MU_COUNT);
        targetLinearUnitId = (MEASURE_UNIT)getIndex(circopt.targetOptions.linearUnit,MeasureUnitAttributeText,MU_COUNT);
        targetAngularUnitId = (MEASURE_UNIT)getIndex(circopt.targetOptions.angularUnit,MeasureUnitAttributeText,MU_COUNT);

        //CRSs
        if (/*circopt.sourceOptions.useGeodeticCRS &&*/ circopt.sourceOptions.geodeticCRSid != "")
        {
            if (!(SourceGeodeticCRS = GS.getCRS(circopt.sourceOptions.geodeticCRSid)))
            {
                operationMessage = OM_SOURCE_GEODETIC_CRS_NOT_FOUND;
                return;
            }
        }
        if (/*circopt.targetOptions.useGeodeticCRS &&*/ circopt.targetOptions.geodeticCRSid != "")
        {
            if (!(TargetGeodeticCRS = GS.getCRS(circopt.targetOptions.geodeticCRSid)))
            {
                operationMessage = OM_TARGET_GEODETIC_CRS_NOT_FOUND;
                return;
            }
        }
        if (/*circopt.sourceOptions.useVerticalCRS &&*/ circopt.sourceOptions.verticalCRSid != ""
                && circopt.sourceOptions.verticalCRSid != "AUTHORITATIVE")
        {
            if (!(SourceVerticalCRS = GS.getCRS(circopt.sourceOptions.verticalCRSid)))
            {
                operationMessage = OM_SOURCE_VERTICAL_CRS_NOT_FOUND;
                return;
            }
        }
        if (/*circopt.targetOptions.useVerticalCRS &&*/ circopt.targetOptions.verticalCRSid != ""
                && circopt.targetOptions.verticalCRSid != "AUTHORITATIVE")
        {
            if (!(TargetVerticalCRS = GS.getCRS(circopt.targetOptions.verticalCRSid)))
            {
                operationMessage = OM_TARGET_VERTICAL_CRS_NOT_FOUND;
                return;
            }
        }
        //None of the 4 CRS is mandatory but there must be at least one source CRS and one target CRS
        if (!(SourceGeodeticCRS) && !(SourceVerticalCRS))
        {
            operationMessage = OM_SOURCE_FRAMES_NOT_INITIALIZED;
            return;
        }
        if (!(TargetGeodeticCRS) && !(TargetVerticalCRS))
        {
            operationMessage = OM_TARGET_FRAMES_NOT_INITIALIZED;
            return;
        }

        //details
        extraOut = circopt.extraOut;

        //frames and conversions
        if (SourceGeodeticCRS)
        {
            //The current coordinate type is the source CRS type to begin with.
            CurrentCRStype = SourceGeodeticCRS->getCRStype();
            SourceGeodeticFrame = (circe::GeodeticReferenceFrame*)SourceGeodeticCRS->getReferenceFrame();
            if (SourceGeodeticFrame)
                id1u = SourceGeodeticFrame->getUsesForTransformation();
            else
                throw std::string("CatOperation constructor: ")+circopt.sourceOptions.geodeticCRSid+std::string(": SourceGeodeticFrame NULL pointer");
            SourceConversion = InitConversion(SourceGeodeticCRS);
        }
        if (TargetGeodeticCRS)
        {
            TargetGeodeticFrame = (circe::GeodeticReferenceFrame*)TargetGeodeticCRS->getReferenceFrame();
            if (TargetGeodeticFrame)
                id2u = TargetGeodeticFrame->getUsesForTransformation();
            else
                throw std::string("CatOperation constructor: ")+circopt.targetOptions.geodeticCRSid+std::string(": TargetGeodeticFrame NULL pointer");
            TargetConversion = InitConversion(TargetGeodeticCRS);
            TargetConversion->setbCalcScaleConv(circopt.extraOut);
        }
        noSourceVertical = false;
        if (SourceVerticalCRS)
        {
            SourceVerticalFrame = (VerticalReferenceFrame*)SourceVerticalCRS->getReferenceFrame();
            if (SourceVerticalFrame)
                idV1 = SourceVerticalFrame->getCirceId();
        }
        else if (circopt.sourceOptions.verticalCRSid == "AUTHORITATIVE")
            idV1 = circopt.sourceOptions.verticalCRSid;
        else
            noSourceVertical = true;
        noTargetVertical = false;
        if (TargetVerticalCRS)
        {
            TargetVerticalFrame = (VerticalReferenceFrame*)TargetVerticalCRS->getReferenceFrame();
            if (TargetVerticalFrame)
                idV2 = TargetVerticalFrame->getCirceId();
        }
        else if (circopt.targetOptions.verticalCRSid == "AUTHORITATIVE")
            idV2 = circopt.targetOptions.verticalCRSid;
        else
            noTargetVertical = true;

        //Initialization of noSourceEllHeight and noTargetEllHeight.
        //This works only for basic formats ('H' included or not).
        if (circopt.sourceOptions.subformatId == FF_BASIC && hasEllHeight(circopt.sourceOptions.formatId))
//         || SourceGeodeticFrame->getSystemDimension() == SD_3D)
            noSourceEllHeight = false;
        else
            noSourceEllHeight = true;
        if (circopt.targetOptions.subformatId == FF_BASIC && hasEllHeight(circopt.targetOptions.formatId))
//         || TargetGeodeticFrame->getSystemDimension() != SD_2D)
            noTargetEllHeight = false;
        else
            noTargetEllHeight = true;

        //Grid loading type
        LOADING_TYPE lt = (LOADING_TYPE)getIndex(circopt.gridLoading,LoadingTypeText,LD_COUNT);
        if (lt == LD_UNDEF)
        {
            if (circopt.runTest) // Small number of points in each test file
                lt = LD_BINARY;
            else // ...... to be done ......
                lt = LD_ARRAY;
        }

		//4th dimension
		sourceEpoch = circopt.sourceOptions.epoch;
        std::string checkSource4D = check4thD(&sourceEpoch, SourceGeodeticFrame->getSystemDimension());
		if (checkSource4D == "4Dfail")
		{
			operationMessage = OM_SOURCE_EPOCH_FRAME_NOT_VALID;
			return;
		}
		targetEpoch = circopt.targetOptions.epoch;
        std::string checkTarget4D = check4thD(&targetEpoch, TargetGeodeticFrame->getSystemDimension());
		if (checkTarget4D == "4Dfail")
		{
			operationMessage = OM_TARGET_EPOCH_FRAME_NOT_VALID;
			return;
		}


        //Find relevant transformations if required.
		//At this stage, the transformations found will all be relevant regarding the CRSs involved. 
		//If more than one transformation is found, the transformation eventually used will depend 
		//on each point processed (transformation.cpp SelectTransfo).

		TransfoRequired = T_UNDEF;
		bRelevantTransfoFound = false;
		//Search for compound transformations if needed.
        if (checkSource4D == "4Dok" && checkTarget4D == "4Dok" && GS.getKeyGeodeticFrame() != "")
//            bRelevantTransfoFound = GetITRFtransformation(id1u, sourceEpoch, id2u, targetEpoch, GS, circopt.boundaryFile, lt);
            bRelevantTransfoFound = Get4Dtransformation(id1u, sourceEpoch, id2u, targetEpoch, GS, circopt.boundaryFile, lt);
        if (!bRelevantTransfoFound)
		//Search for direct transformations.
			bRelevantTransfoFound = GetTransformation (id1u, id2u, idV1, idV2, GS, lt,
                                circopt.idGeodeticTransfoRequired,
                                circopt.idVerticalTransfoRequired, false);
		if ((TransfoRequired && !bRelevantTransfoFound))
			return;

		FollowUpTransfos = circopt.followUpTransfos;

		//sourceEpoch or targetEpoch are non zero.
        //Transformations between ITRFXXXX@yyyy.yyy and a local frame.
        //Case 1: Integrated, uniform 14-parameter transfo: no predefined epoch
        //        for the 4D frame, may be any epoch.
        //Case 2: (Grid) There is a predefined epoch for the 4D frame; may need
        //        the application of a velocity model to change epochs (with
        //        another CatOperation instance).
        if (checkSource4D == "4Dok" && checkTarget4D != "4Dok")
			for (itT = vRelevantGeodeticTransfo.begin(); itT<vRelevantGeodeticTransfo.end(); itT++)
        {
            itT->GetApplicationFrameEpochs(&epF, &epT);
            if (epF != 0. && epF != sourceEpoch)
            {
                operationMessage = OM_SOURCE_EPOCH_CHANGED;
                sourceEpoch = epF;
            }
            itT->toEpoch(sourceEpoch);
//            itT->setRateFactor(asString(targetEpoch-sourceEpoch));
        }
		else if (checkSource4D != "4Dok" && checkTarget4D == "4Dok")
            for(itT=vRelevantGeodeticTransfo.begin();itT<vRelevantGeodeticTransfo.end();itT++)
        {
            itT->GetApplicationFrameEpochs(&epF, &epT);
            if (epT != 0. && epT != targetEpoch)
            {
                operationMessage = OM_TARGET_EPOCH_CHANGED;
                targetEpoch = epT;
            }
            itT->toEpoch(targetEpoch);
//            itT->setRateFactor(asString(targetEpoch-sourceEpoch));
        }

		//Initialize Geodetic Coordinate Types
		if (TransfoRequired&T_GEODETIC_REQUIRED)
		{
			//La transfo se fera en coordonnees cartesiennes ou Geographic dans les deux reperes.
			TargetCRStype1 = SourceCRStype2 = vRelevantGeodeticTransfo[0].getCRStype();
			TargetCRStype2 = TargetGeodeticCRS->getCRStype();

			if (SourceGeodeticFrame
				&& (SourceGeodeticFrame->getUsesForTransformation()
				!= SourceGeodeticFrame->getCirceId()))
				sourceMeridianTranslation = SourceGeodeticFrame->getPrimeMeridianValue();
			if (TargetGeodeticFrame
				&& (TargetGeodeticFrame->getUsesForTransformation()
				!= TargetGeodeticFrame->getCirceId()))
				targetMeridianTranslation = TargetGeodeticFrame->getPrimeMeridianValue();
		}
		else
		{
			TargetCRStype1 = TargetGeodeticCRS->getCRStype();
			//            if (CurrentCRStype == TargetCRStype1)
			//return true;
			if (SourceGeodeticFrame
				&& (SourceGeodeticFrame->getUsesForTransformation()
				!= SourceGeodeticFrame->getCirceId()))
				sourceMeridianTranslation = SourceGeodeticFrame->getPrimeMeridianValue();
			if (TargetGeodeticFrame
				&& (TargetGeodeticFrame->getUsesForTransformation()
				!= TargetGeodeticFrame->getCirceId()))
				sourceMeridianTranslation -= TargetGeodeticFrame->getPrimeMeridianValue();
		}

		if (TransfoRequired&T_VERTICAL_REQUIRED)
		{
			if (vRelevantVerticalGeodetic1Transfo.size()>0 && vRelevantVerticalGeodetic2Transfo.size()>0)
            {
                /*
                qDebug() << "1:" << bin << TransfoRequired;
                if (eraseSupersededGrid(&vRelevantVerticalGeodetic1Transfo, &vRelevantVerticalGeodetic2Transfo))
                {
                    vRelevantVerticalGeodetic1Transfo.clear();
                    TransfoRequired ^= T_GEODETIC1_VERTICAL_REQUIRED;
                    qDebug() << "2:" << bin << TransfoRequired;
                }
                else if (eraseSupersededGrid(&vRelevantVerticalGeodetic2Transfo, &vRelevantVerticalGeodetic1Transfo))
                {
                    vRelevantVerticalGeodetic2Transfo.clear();
                    TransfoRequired ^= T_GEODETIC2_VERTICAL_REQUIRED;
                    qDebug() << "3:" << bin << TransfoRequired;
                }
                else
                */
                    operationMessage = OM_AMBIGUOUS_VERTICAL_TRANSFORMATION;
				return;
			}
		}

        //check compatibility of CRS type and measure unit
        switch (CurrentCRStype)
        {
        case CT_GEOCENTRIC:
        case CT_PROJECTED:
        case CT_VERTICAL:
            if (definitionDomain[CurrentCRStype][sourceLinearUnitId][0] == 0
             && definitionDomain[CurrentCRStype][sourceLinearUnitId][1] == 0)
            {
                operationMessage = OM_CRS_TYPE_MEASURE_UNIT_INCOMPATIBLE;
                return;
            }
            break;
        case CT_GEOGRAPHIC:
            if (definitionDomain[CurrentCRStype][sourceAngularUnitId][0] == 0
             && definitionDomain[CurrentCRStype][sourceAngularUnitId][1] == 0)
            {
                operationMessage = OM_CRS_TYPE_MEASURE_UNIT_INCOMPATIBLE;
                return;
            }
            break;
        default:
            operationMessage = OM_CRS_TYPE_MEASURE_UNIT_INCOMPATIBLE;
            return;
            break;
        }

//    }
//    catch (std::string str)
//    {
//        throw str;
//    }
}

/**
* \brief Checks wether a double value (of a certain MEASURE_UNIT) is within the definition domain of the CRS_TYPE.
* \return true or false.
*/
//definitionDomain[CT_GEOGRAPHIC][MU_RADIAN] != {0,0} should have been checked before processing points
bool CatOperation::checkDefinitionDomain(const double _value, const CRS_TYPE& ct, const MEASURE_UNIT& mu)
{
    if (_value < definitionDomain[ct][mu][0] || _value > definitionDomain[ct][mu][1])
        return false;
    else
        return true;
}

/**
* \brief Writes the values of the transformations actually used to perform the operation.
* \return A std::string containing the values of the transformations actually used to perform the operation.
*/
std::string CatOperation::followUpTransfos()
{
	std::string str = "";
	for (int i = 0; i < getvRelevantGeodeticTransfo().size(); i++)
		if (getvRelevantGeodeticTransfo()[i].getFollowUp() != "")
            str += getvRelevantGeodeticTransfo()[i].getIGNid() + " ( " + getvRelevantGeodeticTransfo()[i].getFollowUp() + " ) ";
	for (int i = 0; i < getvRelevantVertical2VerticalTransfo().size(); i++)
		if (getvRelevantVertical2VerticalTransfo()[i].getFollowUp() != "")
			str += getvRelevantVertical2VerticalTransfo()[i].getIGNid() + "\n" + getvRelevantVertical2VerticalTransfo()[i].getFollowUp();
	for (int i = 0; i < getvRelevantVerticalGeodetic1Transfo().size(); i++)
		if (getvRelevantVerticalGeodetic1Transfo()[i].getFollowUp() != "")
			str += getvRelevantVerticalGeodetic1Transfo()[i].getIGNid() + "\n" + getvRelevantVerticalGeodetic1Transfo()[i].getFollowUp();
	for (int i = 0; i < getvRelevantVerticalGeodetic2Transfo().size(); i++)
		if (getvRelevantVerticalGeodetic2Transfo()[i].getFollowUp() != "")
			str += getvRelevantVerticalGeodetic2Transfo()[i].getIGNid() + "\n" + getvRelevantVerticalGeodetic2Transfo()[i].getFollowUp();
//	if (str != "")
//		str = "transfos used:\n" + str;
	return str;
}

/**
* \brief Writes the names of the vertical grids actually used to perform the operation.
* \return A std::string containing the names of the vertical grids actually used to perform the operation.
*/
std::string CatOperation::getVerticalGridNames()
{
    std::string str = "";
    for (int i = 0; i < getvRelevantVertical2VerticalTransfo().size(); i++)
//        if (getvRelevantVertical2VerticalTransfo()[i].getFollowUp() != "")
            str += getFilename(getvRelevantVertical2VerticalTransfo()[i].getGridFile())+" ";
    for (int i = 0; i < getvRelevantVerticalGeodetic1Transfo().size(); i++)
//        if (getvRelevantVerticalGeodetic1Transfo()[i].getFollowUp() != "")
            str += getFilename(getvRelevantVerticalGeodetic1Transfo()[i].getGridFile())+" ";
    for (int i = 0; i < getvRelevantVerticalGeodetic2Transfo().size(); i++)
//        if (getvRelevantVerticalGeodetic2Transfo()[i].getFollowUp() != "")
            str += getFilename(getvRelevantVerticalGeodetic2Transfo()[i].getGridFile())+" ";
    return str;
}

/**
* \brief Displays the description of the source and target CRSs.
* \return The description.
*/
std::string CatOperation::displayDescription(const circeOptions &circopt, std::string what)
{
	std::string str="";
    if (what=="all" || what=="source" || what=="via_source" || what=="via_target")
    {
        if (what=="via_source")
            str += "*!\n*!VIA:";
        else
            str += "*!" + CRSdescriptionElements[CD_SOURCE] + ": ";
        if (circopt.sourceOptions.pathname != "")
            str += circopt.sourceOptions.pathname;
        str += "\n";
        if (circopt.sourceOptions.subformatId != FF_BASIC)
        {
            str += "*!FORMAT: " + FileFormatText[circopt.sourceOptions.subformatId] + "\n";
        }
        str += "*!" + CRSdescriptionElements[CD_CODE] + ": " + circopt.sourceOptions.geodeticCRSid + "." + circopt.sourceOptions.verticalCRSid + "\n";
        if (SourceGeodeticCRS)
        {
            str += SourceGeodeticCRS->displayDescription();
            if (SourceGeodeticCRS->getCRStype() == CT_GEOGRAPHIC)
                str += "*!" + CRSdescriptionElements[CD_UNIT] + ": " + MeasureUnitDisplayText[sourceAngularUnitId] + "\n";
            if (SourceGeodeticFrame->getSystemDimension()==circe::SD_4D)
                str += "*!" + CRSdescriptionElements[CD_EPOCH] + ": " + asString(sourceEpoch,3) + "\n";
        }
        if (SourceVerticalCRS)
        {
            str += SourceVerticalCRS->displayDescription();
        }
    }
    if (what=="all" || what=="target" || what=="via_source" || what=="via_target")
    {
        if (what=="via_target")
            str += "*!\n*!VIA:\n";
        else
            str += "*!\n*!" + CRSdescriptionElements[CD_TARGET] + ":\n";
        if (circopt.targetOptions.subformatId != FF_BASIC)
        {
            str += "*!FORMAT: " + FileFormatText[circopt.targetOptions.subformatId] + "\n";
        }
        str += "*!" + CRSdescriptionElements[CD_CODE] + ": " + circopt.targetOptions.geodeticCRSid + "." + circopt.targetOptions.verticalCRSid + "\n";
        if (TargetGeodeticCRS)
        {
            str += TargetGeodeticCRS->displayDescription();
            if (TargetGeodeticCRS->getCRStype() == CT_GEOGRAPHIC)
                str += "*!" + CRSdescriptionElements[CD_UNIT] + ": " + MeasureUnitDisplayText[targetAngularUnitId] + "\n";
            else if (hasConvergence(circopt.targetOptions.formatId))
                str += "*!" + CRSdescriptionElements[CD_UNIT] + "(" + InterfaceElementText[IE_CONVERGENCE] + "): " + MeasureUnitDisplayText[targetAngularUnitId] + "\n";
            if (TargetGeodeticFrame->getSystemDimension()==circe::SD_4D)
                str += "*!" + CRSdescriptionElements[CD_EPOCH] + ": " + asString(targetEpoch,3) + "\n";
        }
        if (TargetVerticalCRS)
        {
            str += TargetVerticalCRS->displayDescription();
        }
        if (circopt.targetOptions.verticalCRSid != "AUTHORITATIVE")
        {
            std::string str2 = getVerticalGridNames();
            if (str2 != "")
                str += "*!" + CRSdescriptionElements[CD_GRID] +": " + str2 + "\n";
        }
    }
    return str;
}

/**
* \brief Operates a pt4d from the source CRS to the target CRS.
* \param p1 (input and output): Pointer to the pt4d to be operated.
* \return An OPERATION_MESSAGE which is a code for an error message if it is not OM_OK.
*/
//OPERATION_MESSAGE CatOperation::Operate(pt4d*p1) const
OPERATION_MESSAGE CatOperation::Operate(pt4d*p1)
{
    bool bGeocentricCoordOK = false;
    OPERATION_MESSAGE om = OM_OK;
    pt4d p2;
    //int codeVert = CALC_VERT_DEFL;
    int codeVert = extraOut?CALC_VERT_DEFL:0;


//    try
//    {
        //If source and target CRS are the same, don't go out because something may be done
        //(unit change only by now).

        //Initialization OK ?
        if (operationMessage != OM_OK)
            return operationMessage;

        InitDefPoint(&p2);

        for(std::vector<Transformation>::iterator itT=vRelevantGeodeticTransfo.begin();itT!=vRelevantGeodeticTransfo.end();itT++)
            itT->setFollowUp("");

        CRS_TYPE CurrentCRStype = this->CurrentCRStype;
        p1->CRStypeInit = CurrentCRStype;
        p2.id = p1->id;
		p2.frmt.n_ang = p1->frmt.n_ang;
		p2.frmt.n_m = p1->frmt.n_m;
        p2.transfoIndex = p1->transfoIndex;
		//ell.h. may be updated later by vertical transfo if there is one
        if (noSourceEllHeight)
            p1->h = p1->w;
        //Operate in Source Geodetic Frame
        if (CurrentCRStype == CT_GEOGRAPHIC)
        {
            if (!(checkDefinitionDomain(p1->l, CurrentCRStype, sourceAngularUnitId))
             || !(checkDefinitionDomain(p1->p, CurrentCRStype, sourceAngularUnitId)))
                return OM_POINT_OUTSIDE_CRS_TYPE_DOMAIN;
            p1->l = UnitConvert(p1->l, sourceAngularUnitId, MU_RADIAN);
            p1->p = UnitConvert(p1->p, sourceAngularUnitId, MU_RADIAN);
        }
        else if (CurrentCRStype == CT_PROJECTED)
        {
            if (!(checkDefinitionDomain(p1->e, CurrentCRStype, sourceLinearUnitId))
             || !(checkDefinitionDomain(p1->n, CurrentCRStype, sourceLinearUnitId)))
                return OM_POINT_OUTSIDE_CRS_TYPE_DOMAIN;
            SourceConversion->ApplyInv(p1->e,p1->n,&p1->l,&p1->p,&p1->conv,&p1->scfact);
            CurrentCRStype = CT_GEOGRAPHIC;
        }
        //always need CT_GEOGRAPHIC at least to check isInside()
        else if (CurrentCRStype == CT_GEOCENTRIC)
        {
            if (!(checkDefinitionDomain(pow(p1->x,2)+pow(p1->y,2)+pow(p1->z,2), CurrentCRStype, sourceLinearUnitId)))
                return OM_POINT_OUTSIDE_CRS_TYPE_DOMAIN;
            //Geocentric=>Geographic
            CartGeo56(p1->x,p1->y,p1->z,SourceConversion->get_a(),
                                        SourceConversion->get_e2(),&p1->l,&p1->p,&p1->h);
//            p1->l += sourceMeridianTranslation; //non-zero if (TransfoRequired)
            if (TargetCRStype1 != CT_GEOCENTRIC)
            {
                CurrentCRStype = CT_GEOGRAPHIC;
            }
        }
        else
            throw std::string ("CatOperation::Operate: CurrentCRStype ")+ErrorMessageText[EM_UNINITIALIZED];

        //Don't test because geographic coordinates are necessarily calculated
        //but CurrentCRStype may be CT_GEOCENTRIC (see above).
//        if (CurrentCRStype != CT_GEOGRAPHIC)
//            throw std::string ("CatOperation::Operate: CurrentCRStype != CT_GEOGRAPHIC (shouldn't occur)");

        //Boundaries are related to Greenwich meridian, not the system native meridian.
        //if (!SourceGeodeticCRS->isInside(p1->l+SourceGeodeticFrame->getPrimeMeridianValue(),p1->p))
        if (!SourceGeodeticCRS->isInside(p1->l,p1->p))
        {
            if (targetAngularUnitId != MU_UNDEF)
            {
                p1->l = UnitConvert(p1->l, MU_RADIAN, targetAngularUnitId);
                p1->p = UnitConvert(p1->p, MU_RADIAN, targetAngularUnitId);
            }
            return OM_POINT_OUTSIDE_SOURCE_GEODETIC_CRS;
        }

        if (CurrentCRStype == CT_GEOGRAPHIC)
        {
            p1->l += sourceMeridianTranslation; //non-zero if (TransfoRequired) [no]
        }

        if (!(TransfoRequired&T_GEODETIC_REQUIRED)
                //&& !TargetGeodeticCRS->isInside(p1->l+SourceGeodeticFrame->getPrimeMeridianValue(),p1->p))
                && !TargetGeodeticCRS->isInside(p1->l,p1->p))
        {
            if (targetAngularUnitId != MU_UNDEF)
            {
                p1->l = UnitConvert(p1->l, MU_RADIAN, targetAngularUnitId);
                p1->p = UnitConvert(p1->p, MU_RADIAN, targetAngularUnitId);
            }
            return OM_POINT_OUTSIDE_TARGET_GEODETIC_CRS_IN_SOURCE_GEODETIC_FRAME;
        }

        //Apply Vertical Transformation with grid interpolation in the source geodetic frame.
        if (TransfoRequired&T_VERTICAL_REQUIRED)
        {
            om = OM_OK;
            // Vertical => Vertical
            if (TransfoRequired&T_VERTICAL_VERTICAL_GEOD1_REQUIRED)
            {
                //om = Transformation::ApplyVerticalTransformation(p1->l, p1->p, p1->w, &p2.w, vRelevantVertical2VerticalTransfo, &p1->Vprec, FollowUpTransfos, &p1->info, &p1->info2);
                om = Transformation::ApplyVerticalTransformation2(p1, p1->w, &p2.w, vRelevantVertical2VerticalTransfo, &codeVert, FollowUpTransfos);
                p1->w = p2.w;
            }
            // Ell. height => vert height
            else if (TransfoRequired&T_GEODETIC1_VERTICAL_REQUIRED)
            {
                //om = Transformation::ApplyVerticalTransformation(p1->l, p1->p, p1->h, &p1->w, vRelevantVerticalGeodetic1Transfo, &p1->Vprec, FollowUpTransfos, &p1->info, &p1->info2);
                om = Transformation::ApplyVerticalTransformation2(p1, p1->h, &p1->w, vRelevantVerticalGeodetic1Transfo, &codeVert, FollowUpTransfos);
                if (!noTargetVertical)
                    copyPt4dVertical(&p2, *p1);
            }
            // vert height => Ell. height
            else if (TransfoRequired&T_VERTICAL_GEODETIC1_REQUIRED)
                om = Transformation::ApplyVerticalTransformation2(p1, p1->w, &p1->h, vRelevantVerticalGeodetic1Transfo, &codeVert, FollowUpTransfos);
                //om = Transformation::ApplyVerticalTransformation(p1->l, p1->p, p1->w, &p1->h, vRelevantVerticalGeodetic1Transfo, &p1->Vprec, FollowUpTransfos, &p1->info, &p1->info2);
            if (om != OM_OK) return om;
        }

        if (CurrentCRStype == CT_GEOGRAPHIC && TargetCRStype1 == CT_GEOCENTRIC)
        {
            //Geographic=>Geocentric
//            p1->l += sourceMeridianTranslation; //non-zero if (TransfoRequired) [no]
            GeoCart(SourceConversion->get_a(), SourceConversion->get_e2(),
                    p1->l,p1->p,p1->h,&p1->x,&p1->y,&p1->z);
            CurrentCRStype = CT_GEOCENTRIC;
        }
        else if (CurrentCRStype == CT_GEOGRAPHIC && TargetCRStype1 == CT_PROJECTED)
        {
            //Geographic=>Projected
            TargetConversion->Apply(p1->l,p1->p,&p1->e,&p1->n,&p1->conv,&p1->scfact);
            CurrentCRStype = CT_PROJECTED;
        }
        if (CurrentCRStype != TargetCRStype1)
            throw std::string ("CatOperation::Operate: CurrentCRStype != TargetCRStype1 (shouldn't occur)");// false; //shouldn't occur

        //Apply Geodetic Transformation and Convert in Target Geodetic Frame
        if (TransfoRequired&T_GEODETIC_REQUIRED)
        {
//            if (!noTargetVertical || bSourceVertEqTargetVert)
            if (!noTargetVertical)
            {
                p2.w = p1->w; //may be usefull
                p2.Vprec = p1->Vprec;
            }

            //The coordinates of IGN grids nodes are expressed in the target frame, so a geodetic
            //transformation at the geocentric level will calculate target geographic coordinates
            //for grid interpolation. They are saved here to avoid a new calculation in case of a
            //grid interpolation (irrelevant if the transformation is constant).
            if (TargetCRStype1 == CT_GEOCENTRIC)
            {
                om = Transformation::ApplyGeodeticTransformation(p1->l,p1->p,p1->h,p1->x,p1->y,p1->z,
                                            &p2.l,&p2.p,&p2.h,&p2.x,&p2.y,&p2.z,
                                            vRelevantGeodeticTransfo,&p2.Gprec,FollowUpTransfos);
                if (om != OM_OK)
                {
                    copyPt4d(p1, p2);
                    return om;
                }
                if (targetMeridianTranslation == 0.)
                    bGeocentricCoordOK = true;
                else
                    bGeocentricCoordOK = false;
                //always need CT_GEOGRAPHIC at least to check isInside()
                //Geocentric=>Geographic
                CartGeo56(p2.x,p2.y,p2.z,TargetConversion->get_a(),
                                                TargetConversion->get_e2(),&p2.l,&p2.p,&p2.h);
            }
            else if (TargetCRStype1 == CT_GEOGRAPHIC)
            {
				om = Transformation::ApplyGeodeticTransformation(p1->l, p1->p, p1->h, p1->x, p1->y, p1->z,
                                            &p2.l,&p2.p,&p2.h,&p2.x,&p2.y,&p2.z,
											vRelevantGeodeticTransfo, &p2.Gprec, FollowUpTransfos);
                if (om != OM_OK)
                {
                    copyPt4d(p1, p2);
                    return om;
                }
            }
            CurrentCRStype = CT_GEOGRAPHIC;
            p2.l -= targetMeridianTranslation;
            //Boundaries are no more related to Greenwich meridian, but to the system native meridian.
            if (!TargetGeodeticCRS->isInside(p2.l,p2.p))
            {
                if (targetAngularUnitId != MU_UNDEF)
                {
                    p2.l = UnitConvert(p2.l, MU_RADIAN, targetAngularUnitId);
                    p2.p = UnitConvert(p2.p, MU_RADIAN, targetAngularUnitId);
                }
                copyPt4d(p1, p2);
                return OM_POINT_OUTSIDE_TARGET_GEODETIC_CRS_IN_TARGET_GEODETIC_FRAME;
            }

            //Apply Vertical Transformation with grid interpolation in the target geodetic frame.
            //qDebug() << "4:" << bin << TransfoRequired;
            if (TransfoRequired&T_VERTICAL_REQUIRED)
            {
                om = OM_OK;
                // Vertical => Vertical
                if (TransfoRequired&T_VERTICAL_VERTICAL_GEOD2_REQUIRED)
                    om = Transformation::ApplyVerticalTransformation2(&p2, p1->w, &p2.w, vRelevantVertical2VerticalTransfo, &codeVert, FollowUpTransfos);
                    //om = Transformation::ApplyVerticalTransformation(p2.l, p2.p, p1->w, &p2.w, vRelevantVertical2VerticalTransfo, &p2.Vprec, FollowUpTransfos, &p2.info, &p2.info2);
                // Ell. height => vert height
                else if (TransfoRequired&T_GEODETIC2_VERTICAL_REQUIRED)
                    om = Transformation::ApplyVerticalTransformation2(&p2, p2.h, &p2.w, vRelevantVerticalGeodetic2Transfo, &codeVert, FollowUpTransfos);
                    //om = Transformation::ApplyVerticalTransformation(p2.l, p2.p, p2.h, &p2.w, vRelevantVerticalGeodetic2Transfo, &p2.Vprec, FollowUpTransfos, &p2.info, &p2.info2);
                // vert height => Ell. height
                else if (TransfoRequired&T_VERTICAL_GEODETIC2_REQUIRED)
                {
                    om = Transformation::ApplyVerticalTransformation2(&p2, p1->w, &p2.h, vRelevantVerticalGeodetic2Transfo, &codeVert, FollowUpTransfos);
                    //om = Transformation::ApplyVerticalTransformation(p2.l, p2.p, p1->w, &p2.h, vRelevantVerticalGeodetic2Transfo, &p2.Vprec, FollowUpTransfos, &p2.info, &p2.info2);
                    bGeocentricCoordOK = false;
                }
                if (om != OM_OK)
                {
                    copyPt4d(p1, p2);
                    return om;
                }
            }

            if (CurrentCRStype == CT_GEOGRAPHIC && TargetCRStype2 == CT_GEOCENTRIC)
            {
                //Geographic=>Geocentric
                if (!bGeocentricCoordOK)
                    GeoCart(TargetConversion->get_a(), TargetConversion->get_e2(),
                        p2.l,p2.p,p2.h,&p2.x,&p2.y,&p2.z);
                CurrentCRStype = CT_GEOCENTRIC;
            }
            else if (CurrentCRStype == CT_GEOGRAPHIC && TargetCRStype2 == CT_PROJECTED)
            {
                //Geographic=>Projected
                TargetConversion->Apply(p2.l,p2.p,&p2.e,&p2.n,&p2.conv,&p2.scfact);
                CurrentCRStype = CT_PROJECTED;
            }
            if (CurrentCRStype != TargetCRStype2)
                throw std::string ("CatOperation::Operate: CurrentCRStype != TargetCRStype2 (shouldn't occur)");// false; //shouldn't occur

        }

        //Apply Vertical Transformation without grid interpolation.
        if (TransfoRequired&T_VERTICAL_REQUIRED)
        {
            if (TransfoRequired&T_VERTICAL_VERTICAL_GEOD0_REQUIRED)
            {
                //om = Transformation::ApplyVerticalTransformation(p1->l, p1->p, p1->w, &p1->w, vRelevantVertical2VerticalTransfo, &p1->Vprec, FollowUpTransfos, &p1->info, &p1->info2);
                om = Transformation::ApplyVerticalTransformation2(p1, p1->w, &p1->w, vRelevantVertical2VerticalTransfo, &codeVert, FollowUpTransfos);
                if (om != OM_OK)
                {
                    copyPt4d(p1, p2);
                    return om;
                }
            }
        }

        if (TransfoRequired&T_GEODETIC_REQUIRED)
        {
            p2.CRStypeInit = CurrentCRStype;
            if (targetAngularUnitId != MU_UNDEF)
            {
                p2.l = UnitConvert(p2.l, MU_RADIAN, targetAngularUnitId);
                p2.p = UnitConvert(p2.p, MU_RADIAN, targetAngularUnitId);
            }
            if (TargetConversion->getbCalcScaleConv())
            {
                p2.conv = UnitConvert(p2.conv, MU_RADIAN, targetAngularUnitId);
            }
            p2.xi = UnitConvert(p2.xi, MU_RADIAN, MU_SECOND);
            p2.eta = UnitConvert(p2.eta, MU_RADIAN, MU_SECOND);
            //Define TargetGeodeticFrame->setSystemDimension("3D") (from the metadata)
            //somewhere if you really need the ellipsoidal height.
            if (noTargetEllHeight)
                p2.h = 0.;
//            if (noTargetVertical && !bSourceVertEqTargetVert)
            if (noTargetVertical)
                p2.w = 0;
            //Back into p1
            copyPt4d(p1, p2);
        }
        else
        {
            p1->CRStypeInit = CurrentCRStype;
            if (targetAngularUnitId != MU_UNDEF)
            {
                p1->l = UnitConvert(p1->l, MU_RADIAN, targetAngularUnitId);
                p1->p = UnitConvert(p1->p, MU_RADIAN, targetAngularUnitId);
            }
            if (TargetConversion->getbCalcScaleConv())
            {
                p1->conv = UnitConvert(p1->conv, MU_RADIAN, targetAngularUnitId);
            }
            p1->xi = UnitConvert(p1->xi, MU_RADIAN, MU_SECOND);
            p1->eta = UnitConvert(p1->eta, MU_RADIAN, MU_SECOND);
            //Define TargetGeodeticFrame->setSystemDimension("3D") (from the metadata)
            //somewhere if you really need the ellipsoidal height.
            if (noTargetEllHeight)
                p1->h = 0.;
//            if (noTargetVertical && !bSourceVertEqTargetVert)
            if (noTargetVertical)
                p1->w = 0;
        }
//    }
//    catch (std::string str)
//    {
//        throw str;
//    }
    return OM_OK;
}


}  //namespace circe
