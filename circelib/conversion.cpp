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
* \file conversion.cpp
* \brief Implementation of the Conversion class and its child classes.
*/

#include "conversion.h"
#include "stringtools.h"

namespace circe
{

/**
* \brief Constructor.
*/
Conversion::Conversion()
{

}

/**
* \brief Copy constructor.
*/
Conversion::Conversion(const Conversion &B) : Operation(B)
{
	a = B.a;
	e2 = B.e2;
	fromGreenwich = B.fromGreenwich;
	lambda0 = B.lambda0;
	phi0 = B.phi0;
	phi1 = B.phi1;
	phi2 = B.phi2;
	k0 = B.k0;
	x0 = B.x0;
	y0 = B.y0;
	initParamMethodId = B.initParamMethodId;
	applicationMethodId = B.applicationMethodId;
	applicationInvMethodId = B.applicationInvMethodId;
	sourceCRSid = B.sourceCRSid;
	targetCRSid = B.targetCRSid;
	primeMeridianId = B.primeMeridianId;
	addFromGreenwich = B.addFromGreenwich;
	bCalcScaleConv = B.bCalcScaleConv;
}

/**
* \brief Copy constructor.
*/
Conversion::Conversion(const Conversion &B, double _a, double _e2) : Operation(B)
{
	a = _a;
	e2 = _e2;
	fromGreenwich = B.fromGreenwich;
    lambda0 = B.lambda0;
    phi0 = B.phi0;
    phi1 = B.phi1;
    phi2 = B.phi2;
    k0 = B.k0;
    x0 = B.x0;
    y0 = B.y0;
    initParamMethodId = B.initParamMethodId;
    applicationMethodId = B.applicationMethodId;
    applicationInvMethodId = B.applicationInvMethodId;
    sourceCRSid = B.sourceCRSid;
    targetCRSid = B.targetCRSid;
    primeMeridianId = B.primeMeridianId;
    addFromGreenwich = B.addFromGreenwich;
    bCalcScaleConv = B.bCalcScaleConv;
	if (addFromGreenwich) lambda0 += fromGreenwich;
}

/**
* \brief Parameter initialization.
*/
void Conversion::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
}

/**
* \brief Empties values of the members of an instance.
*/
void Conversion::reset()
{
    Operation::reset();
    a = e2 = fromGreenwich = lambda0 = phi0 = phi1 = phi2 = k0 = x0 = y0 = 0.;
    initParamMethodId = applicationMethodId = applicationInvMethodId = sourceCRSid = targetCRSid = primeMeridianId = "";
    addFromGreenwich = bCalcScaleConv = false;
}

/**
* \brief Sets the definition parameters of a Conversion instance.
* \param[in] parameter_name: a std::string for the parameter name
* \param[in] _value: the double value of the parameter
* \param[in] unit: a std::string for the angular parameter unit
* \remark The linear values are supposed to be in meters and are not converted. The angular values are converted in radians.
*/
void Conversion::setParameter(std::string parameter_name, double _value, std::string unit)
{
    if (_value == 0) return; //crucial for IGNF.xml (some parameter names are set twice but only one is initialized)
    MEASURE_UNIT _unit=(MEASURE_UNIT)getIndex(unit,MeasureUnitAttributeText,MU_COUNT);
    if (parameter_name=="X0") //m
        x0 = _value;
    else if (parameter_name=="Y0") //m
        y0 = _value;
    else if (parameter_name=="LAMBDA0") //angle
        lambda0 = UnitConvert(_value, _unit, MU_RADIAN);
    else if (parameter_name=="PHI0") //angle
        phi0 = UnitConvert(_value, _unit, MU_RADIAN);
    else if (parameter_name=="PHI1") //angle
        phi1 = UnitConvert(_value, _unit, MU_RADIAN);
    else if (parameter_name=="PHI2") //angle
        phi2 = UnitConvert(_value, _unit, MU_RADIAN);
    else if (parameter_name=="K0") //without dimension
        k0 = _value;
}

/**
* \brief Adds fromGreenwich to lambda0.
* \remark
ED50EULAMB and ED50EUCART: CRS referenced to Greenwich but conversion lambda0 related to Paris.
This doesn't seem to be present in IGNF.xml where conversions don't have such
"prime meridian" parameter (how are these two projections registered ?).
*/
void Conversion::setAddFromGreenwich(bool _value)
{ 
	addFromGreenwich = _value; 
	//if (addFromGreenwich) lambda0 += fromGreenwich;
}

/**
* \brief Constructor.
*/
ProjectionLCC::ProjectionLCC()
{

}
/**
* \brief Copy constructor.
*/
ProjectionLCC::ProjectionLCC(const Conversion &B, double _a, double _e2) : Conversion(B,_a,_e2)
{
    InitParam(B.getInitParamMethodId(), "", "");
}
/**
* \brief Copy constructor.
*/
ProjectionLCC::ProjectionLCC(const ProjectionLCC &B) : Conversion(B)
{
    lambdac = B.lambdac;
    n = B.n;
    c = B.c;
    xs = B.xs;
    ys = B.ys;
}
/**
* \brief Empties values of the members of an instance.
*/
void ProjectionLCC::reset()
{
    Conversion::reset();
    lambdac = n = c = xs = ys = 0.;
}
/**
* \brief Parameter initialization.
*/
void ProjectionLCC::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
    // if (addFromGreenwich) lambda0+=fromGreenwich;
    //Lambert conique conforme tangente avec réduction d'échelle
	//Lambert Conformal Conic with scale factor
    //EPSG 9801type_proj
    if (getInitParamMethodId()==LCCTID||getInitParamMethodId()==LCCTIB)
		CoefProjLambTan(a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &n, &c, &xs, &ys);
	//Lambert conique conforme sécante avec phi1 et phi2
	//Lambert Conformal Conic with two standard parallels
	//EPSG 9802
    else if (getInitParamMethodId()==LCCSID)
        CoefProjLambSec(a, e2, lambda0, phi0, phi1, phi2, x0, y0, &lambdac, &n, &c, &xs, &ys);
	
}
/**\brief Geographic to Projected coordinates.*/
void ProjectionLCC::Apply(double l, double p, double *x, double *y, double *conv, double *scfact)
{
    GeoLambert(e2, n, c, lambdac, xs, ys, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvLamb(l, p, a, e2, n, c, lambda0, conv, scfact);
    else
        *conv = *scfact = 0.;
}
/**\brief Projected to Geographic coordinates.*/
void ProjectionLCC::ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact)
{
    LambertGeo(e2, n, c, lambdac, xs, ys, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvLamb(*l, *p, a, e2, n, c, lambda0, conv, scfact);
    else
        *conv = *scfact = 0.;
}

/**
* \brief Constructor.
*/
ProjectionTMerc::ProjectionTMerc()
{

}
/**
* \brief Empties values of the members of an instance.
*/
void ProjectionTMerc::reset()
{
    Conversion::reset();
    lambdac = n = c = xs = ys = 0.;
}

/**
* \brief Copy constructor.
*/
ProjectionTMerc::ProjectionTMerc(const Conversion &B, double _a, double _e2) : Conversion(B, _a, _e2)
{
    InitParam(B.getInitParamMethodId(), "", "");
}

/**
* \brief Copy constructor.
*/
ProjectionTMerc::ProjectionTMerc(const ProjectionTMerc &B) : Conversion(B)
{
    lambdac = B.lambdac;
    n = B.n;
    c = B.c;
    xs = B.xs;
    ys = B.ys;
}

/**
* \brief Parameter initialization.
*/
void ProjectionTMerc::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
    // if (addFromGreenwich) lambda0+=fromGreenwich;
    //Mercator Transverse (pour UTM nord)
	//Transverse Mercator
	//EPSG 9807
    if (getInitParamMethodId()==UTMNID)
        CoefProjMercTr(a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &n, &xs, &ys);

	//Mercator Transverse (pour UTM sud)
	//Transverse Mercator
	//EPSG 9807 9808 ?
    else if (getInitParamMethodId()==UTMSID)
        CoefProjMercTr(a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &n, &xs, &ys);
}
/**\brief Geographic to Projected coordinates.*/
void ProjectionTMerc::Apply(double l, double p, double *x, double *y, double *conv, double *scfact)
{
    GeoMercTr(lambda0, n, xs, ys, e2, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvMercTr(a, l, p, n, lambdac, xs, k0, *x, conv, scfact);
    else
        *conv = *scfact = 0.;
}
/**\brief Projected to Geographic coordinates.*/
void ProjectionTMerc::ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact)
{
    MercTrGeo(lambda0, n, xs, ys, e2, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvMercTr(a, *l, *p, n, lambdac, xs, k0, x, conv, scfact);
    else
        *conv = *scfact = 0.;
}

/**
* \brief Constructor.
*/
ProjectionGLab::ProjectionGLab()
{

}
/**
* \brief Copy constructor.
*/
ProjectionGLab::ProjectionGLab(const Conversion &B, double _a, double _e2) : Conversion(B, _a, _e2)
{
    InitParam(B.getInitParamMethodId(), "", "");
}
/**
* \brief Copy constructor.
*/
ProjectionGLab::ProjectionGLab(const ProjectionGLab &B) : Conversion(B)
{
    lambdac = B.lambdac;
    phic = B.phic;
    n1 = B.n1;
    n2 = B.n2;
    c = B.c;
    xs = B.xs;
    ys = B.ys;
}
/**
* \brief Empties values of the members of an instance.
*/
void ProjectionGLab::reset()
{
    Conversion::reset();
    lambdac = phic = n1 = n2 = c = xs = ys = 0.;
}

/**
* \brief Parameter initialization.
*/
void ProjectionGLab::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
    // if (addFromGreenwich) lambda0+=fromGreenwich;
    //Gauss-Laborde – sphère de courbure moyenne
	//EPSG 9813 ?
    int choice=0;
    if (getInitParamMethodId()==GLSBID)
        choice = 1;
    else if (getInitParamMethodId()==GLSEID)
        choice = 2;
    else if (getInitParamMethodId()==GLSCID)
        choice = 3;
    //m_NomREG = "urn:ogc:def:crs:EPSG::3727"; //La Reunion
    //type_proj = 3; //sphere de courbure (pour La Reunion)
    CoefProjGaussLab(choice, a, e2, lambda0, phi0, k0, x0, y0,
        &lambdac, &phic, &c, &n1, &n2, &xs, &ys);
}
/**\brief Geographic to Projected coordinates.*/
void ProjectionGLab::Apply(double l, double p, double *x, double *y, double *conv, double *scfact)
{
    GeoGaussLab(c, n1, n2, xs, ys, e2, lambdac, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvGaussLab(a, e2, l, p, n1, n2, c, lambdac, conv, scfact);
    else
        *conv = *scfact = 0.;
}
/**\brief Projected to Geographic coordinates.*/
void ProjectionGLab::ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact)
{
    GaussLabGeo(c, n1, n2, xs, ys, e2, lambdac, l, p, x, y);
    if (bCalcScaleConv)
        ScaleConvGaussLab(a, e2, *l, *p, n1, n2, c, lambdac, conv, scfact);
    else
        *conv = *scfact = 0.;
}

/**
* \brief Constructor.
*/
ProjectionLAEA::ProjectionLAEA()
{

}
/**
* \brief Copy constructor.
*/
ProjectionLAEA::ProjectionLAEA(const Conversion &B, double _a, double _e2) : Conversion(B, _a, _e2)
{
    InitParam(B.getInitParamMethodId(), "", "");
}
/**
* \brief Copy constructor.
*/
ProjectionLAEA::ProjectionLAEA(const ProjectionLAEA &B) : Conversion(B) 
{
    qp = B.qp;
    beta1 = B.beta1;
    Rq = B.Rq;
    D = B.D;
}
/**
* \brief Empties values of the members of an instance.
*/
void ProjectionLAEA::reset()
{
    Conversion::reset();
    qp = beta1 = Rq = D = 0.;
}

/**
* \brief Parameter initialization.
*/
void ProjectionLAEA::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
    // if (addFromGreenwich) lambda0+=fromGreenwich;
    //Lambert Azimuthal Equal-Area
	//EPSG 9820
    if (getInitParamMethodId()==LAEAID)
		//m_NomREG = "urn:ogc:def:crs:EPSG::3035"; //ETRS
        CoefProjLAEA(a, e2, phi0, &qp, &beta1, &Rq, &D);

}
/**\brief Geographic to Projected coordinates.*/
void ProjectionLAEA::Apply(double l, double p, double *x, double *y, double *conv, double *scfact)
{
    GeoLAEA(a, e2, lambda0, phi0, qp, Rq, beta1, D, x0, y0, l, p, x, y);
    *conv = *scfact = 0.;
}
/**\brief Projected to Geographic coordinates.*/
void ProjectionLAEA::ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact)
{
    LAEAgeo(a, e2, lambda0, phi0, qp, Rq, beta1, D, x0, y0, l, p, x, y);
    *conv = *scfact = 0.;
}

/**
* \brief Constructor.
*/
ProjectionSouthPolarStereographic::ProjectionSouthPolarStereographic()
{

}
/**
* \brief Copy constructor.
*/
ProjectionSouthPolarStereographic::ProjectionSouthPolarStereographic(const Conversion &B, double _a, double _e2) : Conversion(B, _a, _e2)
{
    InitParam(B.getInitParamMethodId(), "", "");
}
/**
* \brief Copy constructor.
*/
ProjectionSouthPolarStereographic::ProjectionSouthPolarStereographic(const ProjectionSouthPolarStereographic &B) : Conversion(B)
{
    lambdac = B.lambdac;
    phic = B.phic;
    n1 = B.n1;
    n2 = B.n2;
    c = B.c;
    xs = B.xs;
    ys = B.ys;
    //e2 = B.e2; //e2 ?
}
/**
* \brief Empties values of the members of an instance.
*/
void ProjectionSouthPolarStereographic::reset()
{
    Conversion::reset();
    lambdac = phic = n1 = n2 = c = xs = ys = 0.;
}

/**
* \brief Parameter initialization.
*/
void ProjectionSouthPolarStereographic::InitParam(std::string IPMid, std::string AMid, std::string AIMid)
{
    setStatus("AUTHORITATIVE");
    setReversibility("DIRECT");
    setInitParamMethodId(IPMid);
    setApplicationMethodId(AMid);
    setApplicationInvMethodId(AIMid);
    // if (addFromGreenwich) lambda0+=fromGreenwich;
    if (getInitParamMethodId()==SMCSID)
        CoefProjSterObl(1, a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &phic, &c, &n1, &n2, &xs, &ys);
    else if (getInitParamMethodId()==SPSTID)
        CoefProjSterObl(5, a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &phic, &c, &n1, &n2, &xs, &ys);
    else if (getInitParamMethodId()==SPSSID) // The only one to be implemented in Apply and ApplyInv
        //Stéréographique polaire sud Terre Adélie
        CoefProjSterObl(7, a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &phic, &c, &n1, &n2, &xs, &ys);
}
/**\brief Geographic to Projected coordinates.*/
void ProjectionSouthPolarStereographic::Apply(double l, double p, double *x, double *y, double *conv, double *scfact)
{
    GeoSterPolSud(c, n1, n2, xs, ys, e2, lambdac, phic, l, p, x, y);
    if (bCalcScaleConv)
        ModConvSterObl (a, l, p, e2, c, n1, n2, lambdac, phic, conv, scfact);
    else
        *conv = *scfact = 0.;
}
/**\brief Projected to Geographic coordinates.*/
void ProjectionSouthPolarStereographic::ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact)
{
    SterPolSudGeo(n1, n2, xs, ys, e2, c, lambdac, phic, l, p, x, y);
    if (bCalcScaleConv)
        ModConvSterObl (a, *l, *p, e2, c, n1, n2, lambdac, phic, conv, scfact);
    else
        *conv = *scfact = 0.;
}


//EPSG 9836
//Geographic/topocentric conversions 


} //namespace circe
