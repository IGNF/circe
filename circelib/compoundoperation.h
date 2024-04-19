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
* \file compoundoperation.h
* \brief Definition of the CompoundOperation class.
*/
#ifndef COMPOUNDOPERATION_H
#define COMPOUNDOPERATION_H

#include "catoperation.h"

namespace circe
{

/**
* \brief This class is directly used by Circe API, high-level and convenience functions to initialize and apply a composition of concatenated operations.
*/
class CompoundOperation : public CirceObject
{
private:
    CatOperation CO1, CO2, CO3;
    OPERATION_MESSAGE operationMessage;
    bool useCO1only, useCO2;
    std::string Description;
    int TransfoRequired;
    CRS_TYPE CurrentCRStype, TargetCRStype1, TargetCRStype2;
    MEASURE_UNIT targetLinearUnitId, targetAngularUnitId;
    Conversion *TargetConversion;
public:
    CompoundOperation();
    ~CompoundOperation();
    CompoundOperation(const CompoundOperation &B) ; // copy constructor
    //no setter since all members are initialized in the constructor
    //(circe usage: const CompoundOperation CO(circopt, GS)
    CompoundOperation(const circeOptions &circopt, GeodeticSet& GS);
    void reset();
    OPERATION_MESSAGE Operate(pt4d*p1);
//    void CompoundOperation::defineHubOptions(geodeticOptions *go, GeodeticSet *GS, const std::string &idGp);
    std::string followUpTransfos();
    std::string displayDescription(const circeOptions &circopt, std::string what);
    void getCurrentState(CatOperation *CO);
    CatOperation getCO1() const {return CO1;}
    CatOperation getCO2() const {return CO2;}
    CatOperation getCO3() const {return CO3;}
    OPERATION_MESSAGE getoperationMessage() const {return operationMessage;}
    bool getuseCO1only() const {return useCO1only;}
    bool getuseCO2() const {return useCO2;}
    std::string getDescription() const {return Description;}
    int getTransfoRequired() const {return TransfoRequired;}
    CRS_TYPE getCurrentCRStype() const {return CurrentCRStype;}
    CRS_TYPE getTargetCRStype1() const {return TargetCRStype1;}
    CRS_TYPE getTargetCRStype2() const {return TargetCRStype2;}
    MEASURE_UNIT getTargetLinearUnitId() const {return targetLinearUnitId;}
    MEASURE_UNIT getTargetAngularUnitId() const {return targetAngularUnitId;}
    Conversion* getTargetConversion() const {return TargetConversion;}
    void changeEpochs(CatOperation *CO, double *source_epoch, double *target_epoch);
};

}
#endif // COMPOUNDOPERATION_H
