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
* \file compoundoperation.cpp
* \brief Implementation of the CompoundOperation class.
*/
#include "compoundoperation.h"

namespace circe
{
/**
* \brief Destructor.
*/
CompoundOperation::~CompoundOperation()
{
    /*
     delete CO1;
    if (useCO1only)
        return;
    if (useCO2)
        delete CO2;
    delete CO3;
*/
}

/**
* \brief Copy constructor.
*/
CompoundOperation::CompoundOperation(const CompoundOperation &B) : CirceObject(B)
{
    CO1 = B.CO1;
    CO2 = B.CO2;
    CO3 = B.CO3;
    operationMessage = B.operationMessage;
    useCO1only = B.useCO1only;
    useCO2 = B.useCO2;
    Description = B.Description;
    TransfoRequired = B.TransfoRequired;
    CurrentCRStype = B.CurrentCRStype;
    TargetCRStype1 = B.TargetCRStype1;
    TargetCRStype2 = B.TargetCRStype2;
    targetLinearUnitId = B.targetLinearUnitId;
    targetAngularUnitId = B.targetAngularUnitId;
    TargetConversion = B.TargetConversion;
}

/**
* \brief Empties values of the members of a CompoundOperation instance.
*/
void CompoundOperation::reset()
{
    CirceObject::reset();
    CO1.reset();
    CO2.reset();
    CO3.reset();
    operationMessage = OM_OK;
    useCO1only = true;
    useCO2 = false;
    Description = "";
    TransfoRequired = 0;
    CurrentCRStype = CT_UNDEF;
    TargetCRStype1 = CT_UNDEF;
    TargetCRStype2 = CT_UNDEF;
    targetLinearUnitId = MU_UNDEF;
    targetAngularUnitId = MU_UNDEF;
    TargetConversion = NULL;
}

/**
* \brief Initializes hub options.
* \param go: Pointer to geodeticOptions instances.
*/
/*
void CompoundOperation::defineHubOptions(geodeticOptions *go, GeodeticSet* GS,
                                    const std::string &idGp)
{
    go->geodeticFrameId = idGp;
    go->geodeticCRSid = GS->getGeodeticReferenceFrame(idGp)->getGeographicCRSid();
    go->angularUnit = "DEGREES";
    go->geodeticCRStype = CT_GEOGRAPHIC;
}
*/

void CompoundOperation::changeEpochs(CatOperation *CO, double *source_epoch, double *target_epoch)
{
    if (CO->getoperationMessage() == OM_SOURCE_EPOCH_CHANGED)
    {
        *target_epoch = CO->getsourceEpoch();
        CO->setoperationMessage(OM_OK);
        operationMessage = OM_OK;
    }
    else if (CO->getoperationMessage() == OM_TARGET_EPOCH_CHANGED)
    {
        *source_epoch = CO->gettargetEpoch();
        CO->setoperationMessage(OM_OK);
        operationMessage = OM_OK;
    }
}

CompoundOperation::CompoundOperation(const circeOptions &circopt, GeodeticSet& GS)
{
    geodeticOptions hubGeodeticOptions;
    circeOptions circopt1, circopt2, circopt3;

    std::string idGp = GS.getKeyGeodeticFrame();
    CO1.Init(circopt, GS);
    if (CO1.getoperationMessage() == OM_OK)
    {
        Description = CO1.displayDescription(circopt,"all");
        TransfoRequired = CO1.getTransfoRequired();
        getCurrentState(&CO1);
    }
    if ( CO1.getoperationMessage() == OM_OK
      ||(CO1.getoperationMessage() != OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THESE_CRS
      && CO1.getoperationMessage() != OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THESE_CRS
      && CO1.getoperationMessage() != OM_SOURCE_EPOCH_CHANGED
      && CO1.getoperationMessage() != OM_TARGET_EPOCH_CHANGED)
      || idGp == "")
    {
        operationMessage = CO1.getoperationMessage();
        useCO1only = true;
        return;
    }
    // Here the CRS ids are valid (if not, the function has already returned) so the following lines must work !
    bool htwkf1 = GS.getGeodeticReferenceFrame(((GeodeticReferenceFrame*)(GS.getCRS(circopt.sourceOptions.geodeticCRSid)->getReferenceFrame()))->getUsesForTransformation())->getHasTransformationWithKeyFrame();
    bool htwkf2 = GS.getGeodeticReferenceFrame(((GeodeticReferenceFrame*)(GS.getCRS(circopt.targetOptions.geodeticCRSid)->getReferenceFrame()))->getUsesForTransformation())->getHasTransformationWithKeyFrame();
    if (!htwkf1 || !htwkf2)
    {
        operationMessage = CO1.getoperationMessage();
        useCO1only = true;
        return;
    }

    // Go on if a geodetic and/or a vertical transformation is required but not found
    // and a "hub geodetic frame" is predefined.
    useCO1only = false;
    copyGeodeticOptions(circopt.sourceOptions, &hubGeodeticOptions);
    hubGeodeticOptions.geodeticFrameId = idGp;
    hubGeodeticOptions.geodeticCRSid = GS.getGeodeticReferenceFrame(idGp)->getGeographicCRSid();
    hubGeodeticOptions.angularUnit = "DEGREES";
    hubGeodeticOptions.geodeticCRStype = CT_GEOGRAPHIC;

    //The input source and target epoch may be erroneously non-zero with a non-4D frame.
    double source_epoch = circopt.sourceOptions.epoch;
    double target_epoch = circopt.targetOptions.epoch;
    if (((GeodeticReferenceFrame*)(GS.getCRS(circopt.sourceOptions.geodeticCRSid)->getReferenceFrame()))->getSystemDimension() != SD_4D)
        source_epoch = 0.;
    if (((GeodeticReferenceFrame*)(GS.getCRS(circopt.targetOptions.geodeticCRSid)->getReferenceFrame()))->getSystemDimension() != SD_4D)
        target_epoch = 0.;

/*
    if TransfoRequired&T_GEODETIC_REQUIRED)
    {
*/
        if (hasIdentifier(circopt.sourceOptions.formatId))
            hubGeodeticOptions.formatId = "ILPH";
        else
            hubGeodeticOptions.formatId = "LPH";

        // source => hub (source epoch)
        copyOptions(&circopt1, circopt);
        copyGeodeticOptions(hubGeodeticOptions, &(circopt1.targetOptions));
        if (source_epoch != 0)
            circopt1.targetOptions.epoch = source_epoch;
        else
            circopt1.targetOptions.epoch = target_epoch;
        CO1.Init(circopt1, GS);
        getCurrentState(&CO1);
        if ( CO1.getoperationMessage() != OM_OK
          && CO1.getoperationMessage() != OM_SOURCE_EPOCH_CHANGED
          && CO1.getoperationMessage() != OM_TARGET_EPOCH_CHANGED )
        {
            operationMessage = CO1.getoperationMessage();
            return;
        }

        // hub => target (target epoch)
        copyOptions(&circopt3, circopt);
        copyGeodeticOptions(hubGeodeticOptions, &(circopt3.sourceOptions));
        if (target_epoch != 0)
            circopt3.sourceOptions.epoch = target_epoch;
        else
            circopt3.sourceOptions.epoch = source_epoch;
        CO3.Init(circopt3, GS);
//        getCurrentState(&CO3);
        if (CO3.getoperationMessage() != OM_OK
            && CO3.getoperationMessage() != OM_SOURCE_EPOCH_CHANGED
            && CO3.getoperationMessage() != OM_TARGET_EPOCH_CHANGED )
        {
            operationMessage = CO3.getoperationMessage();
            useCO1only = true;
            return;
        }

        // hub => hub (change epochs)

        //Faire des vérifications de cohérence
        changeEpochs(&CO1, &source_epoch, &target_epoch);
        changeEpochs(&CO3, &source_epoch, &target_epoch);

        useCO2 = false;
        if (source_epoch != target_epoch && source_epoch != 0 && target_epoch != 0)
        {
            copyOptions(&circopt2, circopt);
            copyGeodeticOptions(hubGeodeticOptions, &(circopt2.sourceOptions));
            copyGeodeticOptions(hubGeodeticOptions, &(circopt2.targetOptions));
            circopt2.sourceOptions.epoch = source_epoch;
            circopt2.targetOptions.epoch = target_epoch;
            CO2.Init(circopt2, GS);
            //getCurrentState(CO2);
            if (CO2.getoperationMessage() != OM_OK)
            {
                operationMessage = CO2.getoperationMessage();
                return;
            }
            useCO2 = true;
        }

/*    }
    else if (CO1.getTransfoRequired()&T_VERTICAL_REQUIRED)
    {
        //hubGeodeticOptions is a copy of circopt.sourceOptions;
        //hubGeodeticOptions.formatId refers to geodetic coordinates (like XYZ, LPH, ENH ...);
        //No direct vertical transformation was found; we hope to find it with
        //the hub geodetic frame (H.hub => V).
        //The reverse transformation is not handled: for instance LPV won't be transformed
        //into LPH since it would be expressed in the hub frame and would require
        //another transformation into the source one.
//        if (!(changeVerticalComponent(&(hubGeodeticOptions.formatId),'H',"V")))
//            throw std::string(ArgumentValidationText[AV_ERR_UNKNOWN]);
        hubGeodeticOptions.formatId = "LPV";

        // source => hub (source epoch)
        copyOptions(&circopt1, circopt);
        copyGeodeticOptions(hubGeodeticOptions, &(circopt1.targetOptions));
        circopt1.targetOptions.epoch = circopt1.sourceOptions.epoch;
        CO1 = new CatOperation(circopt1, GS);
        CurrentCRStype = CO1.CurrentCRStype;
        if (CO1.getoperationMessage() != OM_OK)
        {
            operationMessage = CO1.getoperationMessage();
            return;
        }


    }
    else //shouldn't happen
    {
        throw std::string(ArgumentValidationText[AV_ERR_UNKNOWN]);
    }
*/

//    TransfoRequired |= T_GEODETIC_REQUIRED;

    Description = "";
    if (useCO1only)
        Description = CO1.displayDescription(circopt,"all");
    else if (CO1.getsourceEpoch() != CO1.gettargetEpoch())
        Description = CO1.displayDescription(circopt1,"via_target")+CO3.displayDescription(circopt3,"target");
    else
        Description = CO1.displayDescription(circopt1,"source")+CO3.displayDescription(circopt3,"via_source");
    operationMessage = OM_OK;
    return;
}

void CompoundOperation::getCurrentState(CatOperation *CO)
{
    CurrentCRStype = CO->getCurrentCRStype();
    TargetCRStype1 = CO->getTargetCRStype1();
    TargetCRStype2 = CO->getTargetCRStype2();
    targetLinearUnitId = CO->getTargetLinearUnitId();
    targetAngularUnitId = CO->getTargetAngularUnitId();
    TargetConversion = CO->getTargetConversion();
}

int combinePrecision(int prec1, int prec2)
{
    double p1 = Transformation::ConversionCode2Precision(prec1);
    double p2 = Transformation::ConversionCode2Precision(prec2);
    return Transformation::ConversionPrecision2Code(sqrt(pow(p1,2)+pow(p2,2)));
}

/**
* \brief Operates a pt4d from the source CRS to the target CRS.
* \param p1 (input and output): Pointer to the pt4d to be operated.
* \return An OPERATION_MESSAGE which is a code for an error message if it is not OM_OK.
*/
OPERATION_MESSAGE CompoundOperation::Operate(pt4d*p1)
{
    OPERATION_MESSAGE om;
    pt4d p2;
    p1->transfoIndex = TransfoRequired;
    copyPt4d(&p2, *p1);
    if ( (om = operationMessage) != OM_OK || ((om = CO1.Operate(p1)) != OM_OK) || useCO1only )
    {
        getCurrentState(&CO1);
        return om;
    }
    int Gprecode = p1->Gprec;
    int Vprecode = p1->Vprec;
    if (useCO2)
    {
        if ((om = CO2.Operate(p1)) != OM_OK)
        {
            getCurrentState(&CO2);
            return om;
        }
        Gprecode = combinePrecision(p1->Gprec,Gprecode);
        //Vprecode = combinePrecision(p1->Vprec,Vprecode);
    }
    om = CO3.Operate(p1);
    if (TransfoRequired&T_GEODETIC_REQUIRED)
    {
        p1->Gprec = combinePrecision(p1->Gprec,Gprecode);
        p1->Vprec = combinePrecision(p1->Vprec,Vprecode);
    }
    else
    {
        if (Vprecode == 0)
            p1->Vprec = combinePrecision(p1->Vprec,Gprecode);
        else
            p1->Vprec = combinePrecision(p1->Gprec,Vprecode);
        p1->Gprec = 0;
    }
    getCurrentState(&CO3);
    return om;
}


/**
* \brief Writes the values of the transformations actually used to perform the operation.
* \return A std::string containing the values of the transformations actually used to perform the operation.
*/
std::string CompoundOperation::followUpTransfos()
{
    std::string str = CO1.followUpTransfos();
    if (useCO1only)
        return str;
    if (useCO2)
        str += CO2.followUpTransfos();
    str += CO3.followUpTransfos();
    if (str != "")
        str = "transfos used: " + str;
    return str;
}

/**
* \brief Displays the description of the source, hub and target CRSs.
* \return The description.
*/
std::string CompoundOperation::displayDescription(const circeOptions &circopt, std::string what)
{
    return getDescription();
}

}
