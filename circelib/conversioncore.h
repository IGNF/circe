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
* \file conversioncore.h
* \brief Definition of conversion functions.
*/

#if !defined CONVERSIONCORE_H
#define CONVERSIONCORE_H

#include <cstdio>
#include <cmath>
#include <cassert>
#include <complex>

//constants already defined in math.h l.372
#define M_PI		3.14159265358979323846	// extrait de tc\include\math.h
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962	/* pi/4 */
#define DEGRE		4						// pour le calcul des polynomes

#define INF			1.79769313e308  //max sur 8 bits

#define LATISO(X)		LatIso(X, e2)
#define LAT_CROISSANTE(X)	LatIso(X, 0.)
#define LATISO_INV(X)		LatIsoInv(X, e2)

double LatIso(double phi, double e2);
void GeoLambert(double e2, double n, double c, double lambdac, double xs, double ys, double lambda, double phi, double *x, double *y);
void LambertGeo(double e2, double n, double c, double lambdac, double xs, double ys, double *lambda, double *phi, double x, double y);
void GeoCart(double a, double e2, double lambda, double phi, double h, double *x, double *y, double *z);
void RotAxe(char axe,double angle,double ux,double uy,double uz,double *vx,double *vy,double *vz);
void SpherCart(double lambda, double phi, double *x, double *y, double *z);
void CartSpher(double x, double y, double z, double *lambda, double *phi);
void CoefProjLambTan(double a, double e2, double lambda0, double phi0, double k0, double x0, double y0, double *lambdac, double *n, double *c, double *xs, double *ys);
double polynome(double x , int n , double c[]);
double GrandeNormale(double phi, double a, double e2);
double RMer(double a, double e2, double phi);
double LMer(double a, double e2, double phi, double dphi);
double LPar(double a, double e2, double phi, double dlambda);
void VertDeflection(double a, double e2, double phi, double dphi, double dlambda, double dNwe, double dNsn, double *xi, double *eta);
double LParMerDist(double a, double e2, double phi, double dlambda, double dphi, double *lpar, double *lmer);
void CoeffArcMer(double e2, double c[DEGRE+1]);
double DevArcMer(double phi, double e2);
void CoefProjMercTrDir(double e2, double c[DEGRE+1]);
void GeoMercTr(double lambda0, double n, double xs, double ys, double e2, double lambda, double phi, double *x, double *y);
void MercTrGeo(double lambda0, double n, double xs, double ys, double e2, double *lambda, double *phi, double x, double y);
void GeoGaussLab(double c, double n1, double n2, double xs, double ys, double e2, double lambdac, double lambda, double phi, double *x, double *y);
void GaussLabGeo(double c, double n1, double n2, double xs, double ys, double e2, double lambdac, double *lambda, double *phi, double x, double y);
void ScaleConvGaussLab(double a, double e2, double lambda, double phi, double n1, double n2, double c, double lambdac, double *conv, double *scfact);
void CoefProjSterObl(int choix, double a, double e2, double lambda0, double phi0, double k0, double x0, double y0, double *lambdac, double *phic, double *c, double *n1, double *n2, double *xs, double *ys);
void CoefProjGaussLab(int choix, double a, double e2, double lambda0, double phi0, double k0, double x0, double y0, double *lambdac, double *phic, double *c, double *n1, double *n2, double *xs, double *ys);
void CoefProjMercTr(double a, double e2, double lambda0, double phi0, double k0, double x0, double y0, double *lambdac, double *n, double *xs,double *ys);
void CoefProjLambSec(double a, double e2, double lambda0, double phi0, double phi1, double phi2, double x0, double y0, double *lambdac, double *n, double *c, double *xs, double *ys);
void CartGeo56( double x, double y, double z, double a, double e2, double *lambda, double *phi, double *h);
void ConstLambTgt(double a, double e2, double phi1, double phi2, double *phit, double *k0);
void ModConvSterObl ( double a, double lambda, double phi, double e2, double c, double n1, double n2,double lambdac,double phic, double *conv, double *scfact);
void ScaleConvLamb(double lambda, double phi, double a, double e2, double n, double c, double lambda0, double *conv, double *scfact);
void ScaleConvMercTr(double a, double lambda, double phi, double n, double lambdac, double xs, double k0, double x, double *conv, double *scfact);
void GeoSterPolSud(double c, double n1, double n2, double xs, double ys, double e2, double lambdac, double phic, double l, double p, double* x, double* y);
void SterPolSudGeo(double n1, double n2, double xs, double ys, double e2, double c, double lambdac, double phic, double* l, double* p, double x, double y);
double q_(double e2, double phi);
double m_(double e2, double phi);
void CoefProjLAEA(double a, double e2, double phi0, double *qp, double *beta1, double *Rq, double *D);
void GeoLAEA(double a, double e2, double lambda0, double phi0, double qp, double Rq, double beta1, double D, double x0, double y0, double lambda, double phi, double *x, double *y);
void LAEAgeo(double a, double e2, double lambda0, double phi0, double qp, double Rq, double beta1, double D, double x0, double y0, double *lambda, double *phi, double x, double y);
void testGeoSterPolSud();
double ShifTnTilT(double a1, double a2, double a3, double lambda0, double phi0, double a, double e2, double lambda, double phi);

#endif // !defined CONVERSIONCORE_H
