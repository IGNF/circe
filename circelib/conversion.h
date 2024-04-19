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
* \file conversion.h
* \brief Definition of the Conversion class and its child classes.
*/

#ifndef CONVERSION_H
#define CONVERSION_H

#include "operation.h"
#include "conversioncore.h"

namespace circe
{

/**\brief Code for Tangent Lambert Conformal Conic (LCC) with scale factor Projection Method*/
#define LCCTID "PRCM014from2Dto2D"

/**\brief Another code for Tangent Lambert Conformal (LCC) Conic with scale factor Projection Method*/
#define LCCTIB "PRCM012from2Dto2D"

/**\brief Code for Secant Lambert Conformal Conic (LCC) with two standard parallels Projection Method*/
#define LCCSID "PRCM013from2Dto2D"

/**\brief Code for Universal Transverse Mercator (UTM) Northern Hemisphere Projection Method*/
#define UTMNID "PRCM020from2Dto2D"

/**\brief Code for Universal Transverse Mercator (UTM) Southern Hemisphere Projection Method*/
#define UTMSID "PRCM030from2Dto2D"

/**\brief Code for Gauss-Laborde with bitangent sphere Projection Method*/
#define GLSBID "GLSBIDfrom2Dto2D"

/**\brief Code for Gauss-Laborde with equatorial sphere Projection Method*/
#define GLSEID "GLSEIDfrom2Dto2D"

/**\brief Code for Gauss-Laborde with mean curvature sphere Projection Method*/
#define GLSCID "PRCM053from2Dto2D"

/**\brief Code for Lambert Azimuthal Equal Area (LAEA) Projection Method*/
#define LAEAID "PRCM015from2Dto2D"

/**\brief Code for Oblique Stereographic Projection with mean curvature sphere Projection Method*/
#define SMCSID "PRCM093from2Dto2D"

/**\brief Code for Tangent South Oblique Stereographic Projection Method*/
#define SPSTID "PRCM094from2Dto2D"

/**\brief Code for Secant South Oblique Stereographic Projection Method*/
#define SPSSID "PRCM095from2Dto2D"

#define BONNID "PRCM060from2Dto2D" //BONNE
#define DIMEID "PRCM070from2Dto2D" //MERCATOR DIRECTE
#define TRMEID "PRCM040from2Dto2D" //MERCATOR TRANSVERSE

/**\brief This class manages conversion operations i.e. between geocentric, geographic and projected coordinates within a single reference frame.*/
class Conversion: public Operation
{
protected:
    double a, e2;
    double lambda0, phi0,  phi1, phi2, k0, x0, y0;
    double fromGreenwich;
    std::string initParamMethodId, applicationMethodId, applicationInvMethodId;
    std::string sourceCRSid, targetCRSid, primeMeridianId;
    bool addFromGreenwich;
    bool bCalcScaleConv;
public:
    Conversion();
	Conversion(const Conversion &B); // copy constructor
	Conversion(const Conversion &B, double _a, double _e2); // copy constructor
	void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid) ;
    virtual void Apply(double l, double p, double *x, double *y, double *conv, double *scfact){*x=l;*y=p;*conv=*scfact=0.;}
    virtual void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact){*l=x;*p=y;*conv=*scfact=0.;}

    void setInitParamMethodId(std::string id) {initParamMethodId=id;}
    void setApplicationMethodId(std::string id) {applicationMethodId=id;}
    void setApplicationInvMethodId(std::string id) {applicationInvMethodId=id;}
    void set_a(double _a) {a=_a;}
    void set_e2(double _e2) {e2=_e2;}
    void setFromGreenwich(double _value) {fromGreenwich=_value;}
    void setParameter(std::string parameter_name,double _value,std::string unit);
    void setPrimeMeridianId(std::string id) {primeMeridianId=id;}
	void setAddFromGreenwich(bool _value);
    void setbCalcScaleConv(bool _value) {bCalcScaleConv=_value;}

    std::string getInitParamMethodId() const {return initParamMethodId;}
    std::string getApplicationMethodId() const {return applicationMethodId;}
    std::string getApplicationInvMethodId() const {return applicationInvMethodId;}
    double get_a() const {return a;}
    double get_e2() const {return e2;}
    double getFromGreenwich() const {return fromGreenwich;}
    std::string getPrimeMeridianId() const {return primeMeridianId;}
    bool getAddFromGreenwich() const {return addFromGreenwich;}
    bool getbCalcScaleConv() const {return bCalcScaleConv;}
};

/**\brief This class manages operations between geographic and Lambert Conformal Conic (LCC) Projected coordinates.*/
class ProjectionLCC : public Conversion
{
private:
    double lambdac, n, c, xs, ys;
public:
    ProjectionLCC();
	ProjectionLCC(const Conversion &B, double _a, double _e2); // copy constructor
    ProjectionLCC(const ProjectionLCC &B) ; // copy constructor
    void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid);
    void Apply(double l, double p, double *x, double *y, double *conv, double *scfact);
    void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact);
};
	
/**\brief This class manages operations between geographic and Mercator Transverse Projected coordinates.*/
class ProjectionTMerc : public Conversion
{
private:
    double lambdac, n, c, xs, ys;
public:
    ProjectionTMerc();
	ProjectionTMerc(const Conversion &B, double _a, double _e2); // copy constructor
    ProjectionTMerc(const ProjectionTMerc &B) ; // copy constructor
    void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid);
    void Apply(double l, double p, double *x, double *y, double *conv, double *scfact);
    void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact);
};
	
/**\brief This class manages operations between geographic and Gauss-Laborde Projected coordinates.*/
class ProjectionGLab : public Conversion
{
private:
    double lambdac, phic, c, n1, n2, xs, ys;
    int type_proj;
public:
    ProjectionGLab();
	ProjectionGLab(const Conversion &B, double _a, double _e2); // copy constructor
    ProjectionGLab(const ProjectionGLab &B) ; // copy constructor
    void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid);
    void Apply(double l, double p, double *x, double *y, double *conv, double *scfact);
    void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact);
};
	
/**\brief This class manages operations between geographic and Lambert Azimuthal Equal Area (LAEA) Projected coordinates.*/
class ProjectionLAEA : public Conversion
{
private:
    double qp, beta1, Rq, D;
public:
    ProjectionLAEA();
	ProjectionLAEA(const Conversion &B, double _a, double _e2); // copy constructor
    ProjectionLAEA(const ProjectionLAEA &B) ; // copy constructor
    void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid);
    void Apply(double l, double p, double *x, double *y, double *conv, double *scfact);
    void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact);
};

/**\brief This class manages operations between geographic and South Polar Stereographic Projected coordinates.*/
class ProjectionSouthPolarStereographic : public Conversion
{
private:
    double c, n1, n2, xs, ys, lambdac, phic;
public:
    ProjectionSouthPolarStereographic();
	ProjectionSouthPolarStereographic(const Conversion &B, double _a, double _e2); // copy constructor
    ProjectionSouthPolarStereographic(const ProjectionSouthPolarStereographic &B) ; // copy constructor
    void reset();
    void InitParam(std::string IPMid, std::string AMid, std::string AIMid);
    void Apply(double l, double p, double *x, double *y, double *conv, double *scfact);
    void ApplyInv(double x, double y, double *l, double *p, double *conv, double *scfact);
};

} //namespace circe

#endif // CONVERSION_H
