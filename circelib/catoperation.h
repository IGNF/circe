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
* \file catoperation.h
* \brief Definition of the CatOperation class.
*/
#ifndef CATOPERATION_H
#define CATOPERATION_H

#include "units.h"
#include "parser.h"
#include "geodeticset.h"
#include "StringResources.h"
#include "pt4d.h"

namespace circe
{

/**
* \brief Minima and maxima of each coordinate type in each unit.
0 indicates a meaningless values; any processing leading to 0 shouldn't occur.
1e12 is an arbitrary huge value but not the maximum real value.
*/
static double definitionDomain[CT_COUNT][MU_COUNT][2] =
{
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 4e13, 4.1e13 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { -6.2831853072, 6.2831853072 }, { -1296000, 1296000 }, { -360, 360 }, { -360, 360 }, { -360, 360 }, { -400, 400 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { -1e12, 1e12 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { -1e12, 1e12 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
};

/**
* \brief This class is used by CompoundOperation
* to initialize and apply a concatenation of operations.
*/
class CatOperation : public CirceObject
{
private:
	int TransfoRequired, FollowUpTransfos;
    std::vector<Transformation> vRelevantGeodeticTransfo, vRelevantVertical2VerticalTransfo;
    std::vector<Transformation> vRelevantVerticalGeodetic1Transfo, vRelevantVerticalGeodetic2Transfo;
    double sourceMeridianTranslation, targetMeridianTranslation;
    Conversion *SourceConversion, *TargetConversion;
    GeodeticReferenceFrame *SourceGeodeticFrame, *TargetGeodeticFrame;
    VerticalReferenceFrame *SourceVerticalFrame, *TargetVerticalFrame;
    bool bGeographicCoordRequiredForGeodeticTransfo;
    CRS_TYPE CurrentCRStype, TargetCRStype1, SourceCRStype2, TargetCRStype2;
    bool noSourceEllHeight, noTargetEllHeight, noSourceVertical, noTargetVertical;
    CRS* SourceGeodeticCRS;
    CRS* TargetGeodeticCRS;
    CRS* SourceVerticalCRS;
    CRS* TargetVerticalCRS;
    OPERATION_MESSAGE operationMessage;
    MEASURE_UNIT sourceLinearUnitId, sourceAngularUnitId, targetLinearUnitId, targetAngularUnitId;
    double sourceEpoch, targetEpoch;
    std::string operationOutput;
    bool extraOut;

	static bool IsTransfoMatching(std::vector<Transformation>::iterator itT, std::string id1u, std::string id2u, std::string *idG, CRS_TYPE ct);
	static Conversion* InitConversion(CRS* geodeticCRS);
	static bool checkDefinitionDomain(const double _value, const CRS_TYPE& ct, const MEASURE_UNIT& mu);
    static std::string check4thD(double *ep, circe::SYSTEM_DIMENSION dim);

public:
    CatOperation(){;}
    ~CatOperation();
    CatOperation(const CatOperation &B) ; // copy constructor
    CatOperation(const circeOptions &circopt, GeodeticSet& GS);
    void Init(const circeOptions &circopt, GeodeticSet& GS);
    void reset();
    OPERATION_MESSAGE Operate(pt4d*p1);
	int  getTransfoRequired() const { return TransfoRequired; }
	int  getFollowUpTransfos() const { return FollowUpTransfos; }
	std::vector<Transformation> getvRelevantGeodeticTransfo() const { return vRelevantGeodeticTransfo; }
    std::vector<Transformation> getvRelevantVertical2VerticalTransfo() const {return vRelevantVertical2VerticalTransfo;}
    std::vector<Transformation> getvRelevantVerticalGeodetic1Transfo() const {return vRelevantVerticalGeodetic1Transfo;}
    std::vector<Transformation> getvRelevantVerticalGeodetic2Transfo() const {return vRelevantVerticalGeodetic2Transfo;}
    double getsourceMeridianTranslation() const {return sourceMeridianTranslation;}
    double gettargetMeridianTranslation() const {return targetMeridianTranslation;}
    Conversion* getSourceConversion() const {return SourceConversion;}
    Conversion* getTargetConversion() const {return TargetConversion;}
    GeodeticReferenceFrame* getSourceGeodeticFrame() const {return SourceGeodeticFrame;}
    GeodeticReferenceFrame* getTargetGeodeticFrame() const {return TargetGeodeticFrame;}
    VerticalReferenceFrame* getSourceVerticalFrame() const {return SourceVerticalFrame;}
	VerticalReferenceFrame* getTargetVerticalFrame() const { return TargetVerticalFrame; }
	CRS* getSourceGeodeticCRS() const { return SourceGeodeticCRS; }
	CRS* getTargetGeodeticCRS() const { return TargetGeodeticCRS; }
	CRS* getSourceVerticalCRS() const { return SourceVerticalCRS; }
	CRS* getTargetVerticalCRS() const { return TargetVerticalCRS; }
	bool getbGeographicCoordRequiredForGeodeticTransfo() const { return bGeographicCoordRequiredForGeodeticTransfo; }
    CRS_TYPE getCurrentCRStype() const {return CurrentCRStype;}
    CRS_TYPE getTargetCRStype1() const {return TargetCRStype1;}
    CRS_TYPE getSourceCRStype2() const {return SourceCRStype2;}
    CRS_TYPE getTargetCRStype2() const {return TargetCRStype2;}
    bool getnoSourceEllHeight() const {return noSourceEllHeight;}
    bool getnoTargetEllHeight() const {return noTargetEllHeight;}
    bool getnoSourceVertical() const {return noSourceVertical;}
    bool getnoTargetVertical() const {return noTargetVertical;}
    OPERATION_MESSAGE getoperationMessage() const {return operationMessage;}
    MEASURE_UNIT getSourceLinearUnitId() const {return sourceLinearUnitId;}
    MEASURE_UNIT getSourceAngularUnitId() const {return sourceAngularUnitId;}
    MEASURE_UNIT getTargetLinearUnitId() const {return targetLinearUnitId;}
    MEASURE_UNIT getTargetAngularUnitId() const {return targetAngularUnitId;}
    double getsourceEpoch() const {return sourceEpoch;}
    double gettargetEpoch() const {return targetEpoch;}
    std::string getoperationOutput() const {return operationOutput;}
    std::string followUpTransfos();
    std::string getVerticalGridNames();
    std::string displayDescription(const circeOptions &circopt, std::string what);
    bool getextraOut() const {return extraOut;}

    void setoperationMessage(OPERATION_MESSAGE _value) {operationMessage=_value;}

    bool GetTransformation (const std::string& idG1, const std::string& idG2, const std::string& idV1,
                            const std::string& idV2, GeodeticSet GS, LOADING_TYPE lt,
                            const std::string& idGeodeticTransfoRequired,
                            const std::string& idVerticalTransfoRequired, bool forceIdentity);
    bool GetITRFtransformation(const std::string& idG1, double sourceEpoch,  const std::string& idG2, double targetEpoch,
                                GeodeticSet GS, const std::string& boundaryFilename, LOADING_TYPE lt);
    bool Get4Dtransformation(const std::string& idG1, double sourceEpoch, const std::string& idG2, double targetEpoch,
                                GeodeticSet GS, const std::string& boundaryFilename, LOADING_TYPE lt);
    bool eraseSupersededGrid(std::vector<Transformation> *v1, std::vector<Transformation> *v2);

};

} //namespace circe

#endif // CATOPERATION_H

