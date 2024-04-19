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
* \file pt4d.cpp
* \brief Implementation of pt4d management.
*
\see The function writeBasicLine outputs a pt4d content according to a data format.
*/

#include <iomanip> //setprecision
#include <algorithm> //max
#include "pt4d.h"
#include "files.h"
#include "conversioncore.h"
#include "stringtools.h"
namespace circe
{

/**
* \brief Converts a precision in meters into a number of decimal digits in coordinate display.
* \param[in] prec: a value in meters.
* \param[out] frmt: A displayFormat instance.
*/
void initFormats(double prec, std::string angularUnit, displayFormat *frmt)
{
	if (angularUnit == "")
		angularUnit = "DEGREES";
	prec = fabs(prec);
	if (prec == 0 || prec < 1e-10)
		frmt->n_m = 3;
	else if (prec >= 1)
		frmt->n_m = 0;
	else
		frmt->n_m = unsigned(fabs(ceil(log10(prec) - 0.5)));

	// meters=> angularUnit //1e-5 deg/m
    prec = UnitConvert(prec*1e-6, MU_DEGREE, (MEASURE_UNIT)getIndex(angularUnit, MeasureUnitAttributeText, MU_COUNT));
	if (prec >= 1)
		frmt->n_ang = 0;
	else
		frmt->n_ang = unsigned(fabs(ceil(log10(prec) - 0.5)));
}

/**
* \brief Writes an element of a pt4d instance in a std::string.
* \param[in] c: an element of a data format as a char.
* \param[in] p: A pt4d instance.
* \return the element as a std::string.
*/
std::string displayElement(const pt4d& p, char c)
{
    std::ostringstream ss;
    initBasicOstream(&ss);
    switch (c)
    {
    case 'I':
        ss << p.id;
        break;
    case 'X':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.x;
        break;
    case 'Y':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.y;
        break;
    case 'Z':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.z;
        break;
    case 'L':
        if (p.frmt.angularUnit == "DMS")
            ss << writeDMS(p.l, "lon", p.frmt.n_ang, p.frmt.plainDMS);
        else if (p.frmt.angularUnit == "DM")
            ss << writeDM(p.l, "lon", p.frmt.n_ang, p.frmt.plainDMS);
        else
            ss << std::setprecision(p.frmt.n_ang/*9*/) << p.l;
        break;
    case 'P':
        if (p.frmt.angularUnit == "DMS")
            ss << writeDMS(p.p, "lat", p.frmt.n_ang, p.frmt.plainDMS);
        else if (p.frmt.angularUnit == "DM")
            ss << writeDM(p.p, "lat", p.frmt.n_ang, p.frmt.plainDMS);
        else
            ss << std::setprecision(p.frmt.n_ang/*9*/) << p.p ;
        break;
    case 'H':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.h;
        break;
    case 'E':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.e;
        break;
    case 'N':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.n;
        break;
    case 'V':
        ss << std::setprecision(p.frmt.n_m/*3*/) << p.w;
        break;
    case 'C':
        ss << std::setprecision(6) << p.conv;
        break;
    case 'S':
        ss << std::setprecision(1) << p.scfact;
        break;
    case 'D':
        if (fabs(p.eta)<1e-2 || fabs(p.eta)<1e-2)
            ss << std::scientific << std::setprecision(1) << p.eta  << " " << p.xi;
        else
            ss << std::setprecision(1) << p.eta  << " " << p.xi;
        break;
    default:
        break;
    }
    return ss.str();
}

/**
* \brief Writes an epoch in a std::string.
* \param[in] e: an epoch as a double (decimal year).
* \return the epoch as a std::string.
*/
std::string displayEpoch(double e)
{
    std::ostringstream ss;
    initBasicOstream(&ss);
    ss << std::setprecision(3) << e;
    return ss.str();
}

/**
* \brief Copies a pt4d instance in another.
* \param[in] p2: A pt4d instance.
* \param[out] p1: A pointer to a pt4d instance.
*/
void copyPt4d(pt4d *p1, const pt4d& p2)
{
    p1->id = p2.id;
    p1->x = p2.x;
    p1->y = p2.y;
    p1->z = p2.z;
    p1->l = p2.l;
    p1->p = p2.p;
    p1->h = p2.h;
    p1->e = p2.e;
    p1->n = p2.n;
    p1->w = p2.w;
    p1->conv = p2.conv;
    p1->scfact = p2.scfact;
    p1->xi = p2.xi;
    p1->eta = p2.eta;
    p1->CRStypeInit = p2.CRStypeInit;
	p1->Gprec = p2.Gprec;
	p1->Vprec = p2.Vprec;
	p1->frmt.n_m = p2.frmt.n_m;
	p1->frmt.n_ang = p2.frmt.n_ang;
    p1->frmt.plainDMS = p2.frmt.plainDMS;
    p1->info = p2.info;
    p1->info2 = p2.info2;
    p1->transfoIndex = p2.transfoIndex;
}

/**
* \brief Copies vertical data of a pt4d instance in another.
* \param[in] p2: A pt4d instance.
* \param[out] p1: A pointer to a pt4d instance.
*/
void copyPt4dVertical(pt4d *p1, const pt4d& p2)
{
    p1->w = p2.w;
    p1->xi = p2.xi;
    p1->eta = p2.eta;
    p1->Vprec = p2.Vprec;
    p1->info = p2.info;
    p1->info2 = p2.info2;
}

/**
* \brief Initializes a pt4d instance with default values.
* \param p: A pointer to a pt4d instance.
*/
void InitDefPoint(pt4d *p)
{
    p->e=p->n=p->l=p->p=p->h=p->x=p->y=p->z=p->w=p->conv=p->scfact=p->xi=p->eta=0.;
    p->id = p->info = p->info2 = "";
    p->CRStypeInit = CT_UNDEF;
	p->Gprec = p->Vprec = 0;
    p->transfoIndex = 0;
	initFormats(0, "", &(p->frmt));
}

/**
* \brief Initializes a pt4d instance with values according to the coordinate type.
* \param p: A pointer to a pt4d instance.
* \param[in] c1,c2,c3,c4: The values.
*/
void InitPoint(pt4d *p, double c1, double c2, double c3, double c4)
{
    p->e=p->n=p->l=p->p=p->h=p->x=p->y=p->z=p->w=0.;
    p->id = p->info = p->info2 = "";
    if (p->CRStypeInit==CT_GEOCENTRIC)
    {
        p->x = c1;
        p->y = c2;
        p->z = c3;
    }
    else if (p->CRStypeInit==CT_GEOGRAPHIC)
    {
        p->l = c1;
        p->p = c2;
        p->h = c3;
    }
    else if (p->CRStypeInit==CT_PROJECTED)
    {
        p->e = c1;
        p->n = c2;
        p->h = c3;
    }
    p->w = c4;
    p->conv = p->scfact = p->xi = p->eta = 0.;
}

/**
* \brief Initializes values from a pt4d instance according to the coordinate type.
* \param p[in]: A pt4d instance.
* \param[out] c1,c2,c3,c4: The values.
*/
void copyPt4d(const pt4d& p, double *c1, double *c2, double *c3, double *c4)
{
    if (p.CRStypeInit==CT_GEOCENTRIC)
    {
        *c1 = p.x;
        *c2 = p.y;
        *c3 = p.z;
    }
    else if (p.CRStypeInit==CT_GEOGRAPHIC)
    {
        *c1 = p.l;
        *c2 = p.p;
        *c3 = p.h;
    }
    else if (p.CRStypeInit==CT_PROJECTED)
    {
        *c1 = p.e;
        *c2 = p.n;
        *c3 = p.h;
    }
    *c4 = p.w;
}

/**
* \brief Converts a legacy Circe precision code to a text.
*/
std::string precisionCode2Text(int CodePrecision)
{
	std::string txt = "";
	switch (CodePrecision)
	{
        case  8: txt = "< 1 cm"; break;
        case  7: txt = "< 2 cm"; break;
        case  1: txt = "< 5 cm"; break;
        case  2: txt = "5 / 10 cm"; break;
		case  3: txt = "10 / 20 cm"; break;
		case  4: txt = "20 / 50 cm"; break;
		case  5: txt = "1 / 2 m"; break;
		case  6: txt = "2 / 5 m"; break;
		case 10: txt = "0.5 / 1 m"; break;
		case 11: txt = "< 5 cm"; break;
		default: txt = "no info"; break;
	}
	return txt;
}

/**
* \brief Compares a pt4d instance with reference values according to the coordinate type.
* \param p: A pointer to a pt4d instance.
* \param[in] c1,c2,c3,c4: The reference values.
* \return A std::string containing an information about raw coordinate shift exceeding the threshold.
*/
std::string ComparePoint(pt4d *p, double c1, double c2, double c3, double c4)
{
    std::string retMess="";
    double meterGeocentricThreshold=1e-3;
    double degreeThreshold=1e-9;
    double meterProjectedThreshold=1e-3;
    if (p->CRStypeInit==CT_GEOCENTRIC)
    {
        if (fabs(p->x-c1)>meterGeocentricThreshold)
            retMess += " X";
        if (fabs(p->y-c2)>meterGeocentricThreshold)
            retMess += " Y";
        if (fabs(p->z-c3)>meterGeocentricThreshold)
            retMess += " Z";
    }
    else if (p->CRStypeInit==CT_GEOGRAPHIC)
    {
        if (fabs(p->l-c1)>degreeThreshold)
            retMess += " L";
        if (fabs(p->p-c2)>degreeThreshold)
            retMess += " P";
        if (fabs(p->h-c3)>meterGeocentricThreshold)
            retMess += " H";
    }
    else if (p->CRStypeInit==CT_PROJECTED)
    {
        if (fabs(p->e-c1)>meterProjectedThreshold)
            retMess += " E";
        if (fabs(p->n-c2)>meterProjectedThreshold)
            retMess += " N";
        //heights are tested with geographic coordinates
        //if (fabs(p->h-c3)>meterProjectedThreshold)
        //    retMess += " H";
    }
    if (fabs(p->w-c4)>meterProjectedThreshold)
        retMess += " W";
    if (retMess != "")
        retMess = "bad coordinates: "+retMess;
    return retMess;
}


/**
* \brief Compares a pt4d instance with reference values according to the coordinate type.
* \param[in] a: the semi-major axis of the ellipsoid (m).
* \param[in] e2: the squared eccentricity of the ellipsoid (m).
* \param[in] p: A pointer to a pt4d instance.
* \param[in] c1,c2,c3,c4: The reference values.
* \param[in] linearUnitId: A MEASURE_UNIT code for linear unit (unused, meter is assumed).
* \param[in] angularUnitId: A MEASURE_UNIT code for angular unit.
* \param[out] retMess: A std::string containing coordinate shifts against the threshold (one millimeter), 
all residuals being in millimeters (geographic shifts converted along meridians and parallels).
* \return true if at least one coordinate exceeds a threshold (1 mm).
*/
bool ComparePoint2(double a, double e2, pt4d *p, double c1, double c2, double c3, double c4,
                   MEASURE_UNIT linearUnitId, MEASURE_UNIT angularUnitId, std::string *retMess, double *maxShift)
{
//    double a = 6378137;
//    double e2=.006694380036;
    *retMess="";
	*maxShift = 0;
    double meterGeocentricThreshold=1e-3;
    double meterProjectedThreshold=1e-3;
    double shift1,shift2,shift3,shift4;
    if (p->CRStypeInit==CT_GEOCENTRIC)
    {
        shift1 = (p->x-c1)/meterGeocentricThreshold;
        shift2 = (p->y-c2)/meterGeocentricThreshold;
        shift3 = (p->z-c3)/meterGeocentricThreshold;
        *retMess = "  X:" +asString(shift1,1,5)+"  Y:" +asString(shift2,1,5)+"  Z:" +asString(shift3,1,5);
    }
    else if (p->CRStypeInit==CT_GEOGRAPHIC)
    {
        double dlambda = UnitConvert(p->l,angularUnitId,MU_RADIAN) - UnitConvert(c1,angularUnitId,MU_RADIAN);
        if (dlambda > M_PI) dlambda -= 2*M_PI;
        else if (dlambda < -M_PI) dlambda += 2*M_PI;
        double dphi = UnitConvert(p->p,angularUnitId,MU_RADIAN) - UnitConvert(c2,angularUnitId,MU_RADIAN);
        double gn = GrandeNormale(p->p,a,e2);
        double lpar = gn*cos(p->p)*dlambda;
        double rmer = gn*(1 - e2)/(1 - e2*pow(sin(p->p),2));
        double lmer = rmer*dphi;
        shift1 = lpar/meterGeocentricThreshold;
        shift2 = lmer/meterGeocentricThreshold;
        shift3 = (p->h-c3)/meterGeocentricThreshold;
        *retMess = "  L:" +asString(shift1,1,5)+"  P:" +asString(shift2,1,5)+"  H:" +asString(shift3,1,5);
    }
    else if (p->CRStypeInit==CT_PROJECTED)
    {
        shift1 = (p->e-c1)/meterProjectedThreshold;
        shift2 = (p->n-c2)/meterProjectedThreshold;
        shift3 = 0;
        *retMess = "  E:" +asString(shift1,1,5)+"  N:" +asString(shift2,1,5);
    }
    shift4 = (p->w-c4)/meterProjectedThreshold;
    *retMess += "  W:" +asString(shift4,1,5);
	*maxShift = std::max(fabs(shift1), std::max(fabs(shift2), std::max(fabs(shift3), fabs(shift4))));
    if (fabs(shift1)>=1 || fabs(shift2)>=1 || fabs(shift3)>=1 || fabs(shift4)>=1)
        return true;
    else
        return false;
}
}
