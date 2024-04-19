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
* \file conversioncore.cpp
* \brief Implementation of conversion functions. The references to algorithms (algoXXXX) 
can be found at http://geodesie.ign.fr/index.php?page=algorithmes.
*/

#include "conversioncore.h"

/**\brief Polynom calculation*/
double polynome ( double x, int degre, double coef[])
{
    int i;

	double  val = 0;
	for (i = 0 ; i <= degre  ; i++)
	{
		val += coef[i]*pow( x , i ) ;  
	}
	return val;
}

/**\brief alg0001 Isometric Latitude*/
double LatIso(double phi, double e2)
  {
  double li;
  double e = sqrt(e2);

/*  if (fabs(phi) > M_PI_2)
    li = NAN; 
  else if (phi == M_PI_2)
    li = INF ;
  else if (phi == -M_PI_2)
    li = -INF ;
  else*/
    li = log(tan(M_PI_4+phi/2)) + e*log( (1-e*sin(phi))/(1+e*sin(phi)) )/2;
  return li;
  }


/**\brief alg0002 Reverse Isometric Latitude */
double LatIsoInv(double latiso, double e2)
{
	int LatIsoInv_niter;
	double l0, l1;
	double epsilon_phi = 1e-12;  // radians soit 0.01 mm par defaut
	double e = sqrt(e2);

  l1 = 2*atan(exp(latiso)) - M_PI_2;
  l0 = 100.;
  for (LatIsoInv_niter=0; fabs(l1-l0)>epsilon_phi; LatIsoInv_niter++)
  {
    l0 = l1;
    l1 = 2 * atan( exp(e*log((1+e*sin(l0))/(1-e*sin(l0)))/2) * exp(latiso) )
	 - M_PI_2;
  }
  return l1;
}

/**\brief alg0003 Geographic to Lambert Conformal Conic (LCC) Projected coordinates */
void GeoLambert(double e2, double n, double c, double lambdac,
		double xs, double ys,
		double lambda, double phi, double *x, double *y)
{
  double ex, dl;
  ex = exp(-n*LatIso(phi,e2));
  dl = n*(lambda-lambdac);
  *x = xs + c * ex * sin(dl);
  *y = ys - c * ex * cos(dl);
}

/**\brief alg0004 Lambert Conformal Conic (LCC) Projected to Geographic coordinates */
void LambertGeo(double e2, double n, double c, double lambdac,
	double xs, double ys, double *lambda, double *phi, double x, double y)
{
  double r, gamma, latiso;

  r = sqrt((x-xs)*(x-xs) + (y-ys)*(y-ys));
  gamma = atan((x-xs)/(ys-y));
  *lambda = lambdac+gamma/n;
  latiso = -log(fabs(r/c))/n;
  *phi = LatIsoInv(latiso, e2);
}

/**\brief alg0009 Geographic to Geocentric coordinates*/
void GeoCart(double a, double e2, double lambda, double phi, double h, double *x, double *y, double *z)
{
	  double gn;
	  gn = GrandeNormale(phi, a, e2);
	  *x = (gn+h)*cos(phi)*cos(lambda);
	  *y = (gn+h)*cos(phi)*sin(lambda);
	  *z = (gn*(1-e2)+h)*sin(phi);
}

/**\brief alg0014 Rotation about an axis */
void RotAxe(char axe,double angle,double ux,double uy,double uz,double *vx,double *vy,double *vz)
{
    switch (axe) {
    case 'x':
    case 'X':
        *vx=ux;
        *vy=uy*cos(angle)+uz*sin(angle);
        *vz=uz*cos(angle)-uy*sin(angle);
        break;
    case 'y':
    case 'Y':
        *vx=ux*cos(angle)-uz*sin(angle);
        *vy=uy;
        *vz=uz*cos(angle)+ux*sin(angle);
        break;
    case 'z':
    case 'Z':
        *vx=ux*cos(angle)+uy*sin(angle);
        *vy=uy*cos(angle)-ux*sin(angle);
        *vz=uz;
        break;
    default : break;
    }/*end switch*/
}

/**\brief alg0015 Spheric to Geocentric coordinates */
void SpherCart(double lambda, double phi, double *x, double *y, double *z)
{
  *x = cos(phi)*cos(lambda);
  *y = cos(phi)*sin(lambda);
  *z = sin(phi);
}

/**\brief alg0016 Geocentric to Spheric coordinates*/
void CartSpher(double x, double y, double z, double *lambda, double *phi)
{
  double r;
  r = sqrt(x*x+y*y);
  if (r)
    {
    *lambda=2*atan(y/(x+r));
    *phi=atan(z/r);
    }
  else
    {
    *lambda=0;
    *phi=M_PI_2*z/fabs(z);
    }
}

/**\brief algo0019 Parameters of Tangent Lambert Conformal Conic (LCC) Projection*/
void CoefProjLambTan(double a, double e2, double lambda0, double phi0, double k0,
	double x0, double y0, double *lambdac, double *n, double *c,
	double *xs, double *ys)
{
	/* ici, on dit (abusivement) que phi0 est la latitude de tangence */
	double r = k0 * GrandeNormale(phi0,a,e2) / tan(phi0);
	*lambdac = lambda0;
	*n = sin(phi0);
	*c = r * exp(*n * LatIso(phi0,e2));
	*xs = x0;
	*ys = y0 + r;
}

/**\brief algo0021 length of the prime vertical, distance to the 
* intersection of the normal with the semi-minor axis */
double GrandeNormale(double phi, double a, double e2)
{
	return a/sqrt(1 - e2*sin(phi)*sin(phi));
}

  
/**\brief Radius of curvature in the plane of the meridian (radius of curvature of the meridian ellipse).
* cf Map Projections, A Working Manual, page 24, eq 4-18*/
double RMer(double a, double e2, double phi)
{
	return GrandeNormale(phi,a,e2)*(1 - e2)/(1 - e2*sin(phi)*sin(phi));
}

/**\brief Length of a small arc of meridian.
* cf Map Projections, A Working Manual, page 25, eq 4-19*/
double LMer(double a, double e2, double phi, double dphi)
{
	return RMer(a,e2,phi)*dphi;
}

/**\brief Length of a small arc of parallel.
* cf Map Projections, A Working Manual, page 25, eq 4-21*/
double LPar(double a, double e2, double phi, double dlambda)
{
	return GrandeNormale(phi,a,e2)*cos(phi)*dlambda;
}

/**\brief Vertical Deflection ... Simplistic ...
* */
void VertDeflection(double a, double e2, double phi, double dphi, double dlambda,
                    double dNwe, double dNsn, double *xi, double *eta)
{
//    printf ("VertDeflection:phi=%lf,dphi=%lf,dlambda=%lf,dNwe=%lf,dNsn=%lf\n",phi,dphi,dlambda,dNwe,dNsn);
    *xi = dNsn / LMer(a, e2, phi, dphi);
    *eta = dNwe / LPar(a, e2, phi, dlambda);
}

/**\brief alg0025 Coefficients for Meridian arc length.*/
void CoeffArcMer(double e2, double c[DEGRE+1])
  {
  double coef[DEGRE+1][DEGRE+1]={
	{1, -1./4,  -3./64,   -5./256, -175/16384.},
	{0, -3./8,  -3./32, -45./1024,  -105./4096},
	{0,     0, 15./256,  45/1024.,   525/16384.},
	{0,     0,       0, -35/3072.,  -175/12288.},
	{0,     0,       0,         0,  315/131072.}};
  int i;

  for (i=0; i<=DEGRE; i++)
    c[i] = polynome(e2, DEGRE, coef[i]);
  }

/**\brief alg0026  Meridian arc length from the equator to the geographic 
* latitude phi (for a unit semi-major axis).*/
double DevArcMer(double phi, double e2)
  {
  double beta, coef[5];
  int k;

  CoeffArcMer(e2, coef);
  beta = coef[0]*phi;
  for (k=1; k<5; k++)
    beta += coef[k]*sin(2*k*phi);
  return beta;
  }

/**\brief alg0028 Coefficients for Direct Mercator Projection.*/
void CoefProjMercTrDir(double e2, double c[DEGRE+1])
  {
  double coef[DEGRE+1][DEGRE+1]={
	{1, -1./4,  -3./64,   -5./256,      -175/16384.},
	{0,  1./8,  -1./96,  -9./1024,     -901/184320.},
	{0,     0, 13./768,  17/5120.,     -311/737280.},
	{0,     0,       0, 61/15360.,      899/430080.},
	{0,     0,       0,         0, 49561./41287680.}};
  int i;

  for (i=0; i<=DEGRE; i++)
    c[i] = polynome(e2, DEGRE, coef[i]);
  }

/**\brief alg0029 Coefficients for Reverse Mercator Transverse Projection.*/
void CoefProjMercTrInv(double e2, double c[DEGRE+1])
{
  double coef[DEGRE+1][DEGRE+1]={
	{1, -1./4, -3./64,   -5./256,    -175/16384.},
	{0,  1./8,  1./48,   7./2048,       1/61440.},
	{0,     0, 1./768,   3./1280,    559/368640.},
	{0,     0,      0, 17/30720.,    283/430080.},
	{0,     0,      0,         0, 4397/41287680.}};
  int i;

  for (i=0; i<=DEGRE; i++)
    c[i] = polynome(e2, DEGRE, coef[i]);

}

/**\brief alg0030 Geographic to Mercator Transverse Projected coordinates.*/
void GeoMercTr(double lambda0, double n, double xs, double ys,
	double e2, double lambda, double phi, double *x, double *y)
{
	double phi1, lambda1, l, ls, c[5];

  int i;

	CoefProjMercTrDir(e2, c);
	l = LatIso(phi, e2);
	lambda1 = M_PI_2;
	if ( l == +INF )
		phi1 = M_PI_2;
	else
	{
		if ( l == -INF )
			phi1 = -M_PI_2;
		else
		{
			phi1 = asin(sin(lambda-lambda0)/cosh(l));
			lambda1 = atan(sinh(l)/cos(lambda-lambda0));
		}
	}
	ls = LAT_CROISSANTE(phi1);

    std::complex <double> z(lambda1 , ls) ;
    std::complex <double> zz ;
	zz = n * c[0] * z ;
	for ( i=1 ; i<5 ; i++ ) 
		zz +=  n * c[i] * sin( (2.0*i*z) );
	*x = xs + zz.imag();
	*y = ys + zz.real();

}

/**\brief Mercator Transverse Projected to Geographic coordinates (complex method).*/
void MercTrGeo(double lambda0, double n, double xs, double ys, double e2,
	double *lambda, double *phi, double x, double y)
{
	double l, ls, phi1, c[5];
	int i;

	CoefProjMercTrInv(e2, c);

    std::complex <double>z ( (y-ys)/(n*c[0]), (x-xs)/(n*c[0]) ) ;
    std::complex <double>zp ( (y-ys)/(n*c[0]), (x-xs)/(n*c[0]) ) ;
	l = z.real();
	ls = z.imag();
	for (i=1; i<5; i++)
	{
	    z -= c[i] * sin( (2.0*i*zp) );
		l = z.real();
		ls = z.imag();
	}

	*lambda = lambda0 + atan(sinh(ls)/cos(l));
	phi1 = asin(sin(l)/cosh(ls));
	l = LAT_CROISSANTE(phi1);
	*phi = LatIsoInv(l, e2);
}

/**\brief  alg0052 Coefficients for Mercator Transverse Projection.*/
void CoefProjMercTr(double a, double e2, double lambda0, double phi0,
		double k0, double x0, double y0, double *lambdac, double *n,
		double *xs, double *ys)
  {
  double beta/*,coef[5]*/;
//  CoeffArcMer(e2, coef);
  beta = DevArcMer(phi0, e2);
  *lambdac = lambda0;
  *n = k0*a;
  *xs = x0;
  *ys = y0 - *n * beta;
  }

/**\brief alg0034 Geographic to Gauss-Laborde Projected coordinates.*/
void GeoGaussLab(double c, double n1, double n2, double xs, double ys, 
						double e2, double lambdac, double lambda, double phi,
						double *x, double *y)
{
	double LAMBDA, ls;
	LAMBDA = n1 * (lambda-lambdac);
	ls = c + n1*LATISO(phi);
	*x = xs + n2 * LAT_CROISSANTE(asin(sin(LAMBDA)/cosh(ls)));
	*y = ys + n2 * atan(sinh(ls)/cos(LAMBDA));
}

/**\brief alg0035 Gauss-Laborde Projected to Geographic coordinates.*/
void GaussLabGeo(double c, double n1, double n2, double xs, double ys, 
						double e2, double lambdac, double *lambda, double *phi, 
						double x, double y)
{
	double LAMBDA, ls;
	LAMBDA = atan( sinh((x-xs)/n2)/cos((y-ys)/n2) );
	ls = LAT_CROISSANTE(asin(sin((y-ys)/n2)/cosh((x-xs)/n2)));
	*lambda = lambdac + LAMBDA/n1;
	*phi = LATISO_INV((ls-c)/n1);
}

/**\brief alg0043 Parameters of Oblique Stereographic Projection.*/
void CoefProjSterObl(int choix,double a,double e2,double lambda0,double phi0,
          double k0,double x0,double y0,double *lambdac,
          double *phic,double *c,double *n1,double *n2, double *xs, double *ys)
{
double e = sqrt(e2);
double f = exp(e/2*log((1-e)/(1+e)));
double r0;
 switch (choix){
  case 1:/*sphere courbure*/
   *lambdac=lambda0;
   *n1=sqrt(1+e2*pow(cos(phi0),4)/(1-e2));
   *phic=asin(sin(phi0)/ *n1);
   *c=LatIso(*phic,0)-*n1*LatIso(phi0,e2);
   *n2=k0*a*sqrt(1-e2)/(1-e2*pow(sin(phi0),2));
   *xs=x0;
   *ys=y0;
  break;

  case 2:/*sphere bitangente*/
   *lambdac=lambda0;
   *n1=1;
   *phic=phi0;
   *c=LatIso(*phic,0)-*n1*LatIso(phi0,e2);
   *n2=k0*a/sqrt(1-e2*pow(sin(phi0),2));
   *xs=x0;
   *ys=y0;
  break;

  case 3:/*sphere equatoriale*/
   *lambdac=lambda0;
   *n1=1;
   *phic=LatIsoInv(LatIso(phi0,e2),0);
   *c=0;
   *n2=k0*a*cos(phi0)/sqrt(1-e2*pow(sin(phi0),2));
   *xs=x0;
   *ys=y0;
  break;

  case 4:/*Tangent North Polar Stereographic with scale factor */
   *lambdac=lambda0;
   *n1=1;
   *phic=M_PI_2;
   *c=0;
   *n2=a*k0/sqrt(1-e2)*f;
   *xs=x0;
   *ys=y0;
  break;

   case 5:/*Tangent South Polar Stereographic with scale factor */
    *lambdac=lambda0;
    *n1=1;
    *phic=-M_PI_2;
    *c=0;
    *n2=a*k0/sqrt(1-e2)*f;
    *xs=x0;
    *ys=y0;
   break;

 case 6:/*Secant North Polar Stereographic (at latitude phi0) */
  *lambdac=lambda0;
  *n1=1;
  *phic=M_PI_2;
  *c=0;
     r0=a*cos(phi0)/sqrt(1-e2*pow(sin(phi0),2));
  *n2=r0*exp(LatIso(phi0,e2))/2.;
  *xs=x0;
  *ys=y0-r0;
 break;

 case 7:/*Secant South Polar Stereographic (at latitude phi0) */
  *lambdac=lambda0;
  *n1=1;
  *phic=-M_PI_2;
  *c=0;
     r0=a*cos(phi0)/sqrt(1-e2*pow(sin(phi0),2));
  *n2=r0*exp(-LatIso(phi0,e2))/2.;
  *xs=x0;
  *ys=y0-r0;
 break;

 default:break;
 }/*end switch*/
}

/**\brief alg0046 Parameters of Gauss-Laborde Projection.*/
void CoefProjGaussLab(int choix, double a, double e2, double lambda0,
					 double phi0, double k0, double x0, double y0, 
					 double *lambdac, double *phic, double *c, 
                     double *n1, double *n2, double *xs, double *ys)
{
	switch (choix)
    {
    case 1: //BITAN:
		*lambdac = lambda0;
		*n1 = 1;
		*phic = phi0;
		*c = LatIso(*phic,0) - LatIso(phi0,e2);
		*n2 = k0 * a / sqrt(1-e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0;
		break;
    case 2: //EQUA:
		*lambdac = lambda0;
		*n1 = 1;
		*phic = LatIsoInv(LatIso(phi0,e2),0);
		*c = 0;
		*n2 = k0 * a * cos(phi0)/(sqrt(1-e2*sin(phi0)*sin(phi0))*cos(*phic));
		*xs = x0;
		*ys = y0;
		break;
    case 3: //COURB:
		*lambdac = lambda0;
		*n1 = sqrt(1 + e2*pow(cos(phi0),4)/(1-e2));
		*phic = asin(sin(phi0)/ *n1);
		*c = LatIso(*phic,0) - *n1 * LatIso(phi0,e2);
		*n2 = k0 * a * sqrt(1-e2)/(1-e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0 - *n2 * *phic;
		break;
    default:
		break;
    }
    //printf("%d %.11lf %.11lf %.12lf %.12lf %.4lf %.4lf %.4lf\n",choix,*lambdac,*phic,*c,*n1,*n2,*xs,*ys);
}

/**\brief alg0054 Parameters of Secant Lambert Conformal Conic (LCC) Projection.*/
void CoefProjLambSec(double a, double e2, double lambda0, double phi0,
		double phi1, double phi2, double x0, double y0,
		double *lambdac, double *n, double *c, double *xs, double *ys)
{
	double n1, n2, l1, l2;
	n1 = GrandeNormale(phi1, a, e2) * cos(phi1);
	n2 = GrandeNormale(phi2, a, e2) * cos(phi2);
	l1 = LatIso(phi1, e2);
	l2 = LatIso(phi2, e2);
	*lambdac = lambda0;
	*n = log(n2/n1) / (l1-l2);
	*c = n1 * exp(*n*l1) / *n;
	*xs = x0;
	*ys = y0 + *c * exp(- *n * LatIso(phi0, e2));
}

/**\brief alg0056 Geocentric to Geographic coordinates (BOWRING non-iterative method).*/
void CartGeo56(double x, double y, double z, double a, double e2, double *lambda, double *phi, double *h)
{
	double p, r, mu;
	p = sqrt(x*x + y*y);
	r = sqrt(x*x + y*y + z*z);
	mu = atan(z/p * (sqrt(1-e2) + a*e2/r));
	*phi = atan((z*sqrt(1-e2) + a*e2*pow(sin(mu),3))/
		(sqrt(1-e2)*(p - a*e2*pow(cos(mu),3))));
	*h = p*cos(*phi) + z*sin(*phi) - a*sqrt(1-e2*sin(*phi)*sin(*phi));
	*lambda = 2*atan(y/(x+p));
}

/**\brief algo 57*/
void ConstLambTgt(double a, double e2, double phi1, double phi2, double *phit, double *k0)
{
	double n, n1, n2, l1, l2, c;
	n1 = GrandeNormale(phi1,a,e2) * cos(phi1);
	n2 = GrandeNormale(phi2,a,e2) * cos(phi2);
	l1 = LatIso(phi1,e2);
	l2 = LatIso(phi2,e2);
	n = log(n2/n1) / (l1-l2);
	c = n1 * exp(n*l1)/n;
	*phit = asin(n);
	*k0 = c * tan(*phit) / (GrandeNormale(*phit,a,e2) * exp(n*LatIso(*phit,e2)));
}

/**\brief alg0059 Scale factor and convergence of Oblique Stereographic Projection.*/
void ModConvSterObl ( double a, double lambda, double phi, double e2, double c, double n1,
             double n2,double lambdac,double phic, double *conv, double *scfact)
{
    double LAMBDA,LS,PHI,ux,uy,uz,vx,vy,vz,r,dux,duy,duz,dvx,dvy,dvz;
    double modlin;

    LS = c+n1*LatIso(phi,e2);
    PHI = LatIsoInv (LS,0);
    LAMBDA = n1*(lambda-lambdac);
    SpherCart(PHI,LAMBDA,&ux,&uy,&uz);
    RotAxe ('y',M_PI_2-phic,ux,uy,uz,&vx,&vy,&vz);
    r = sqrt(ux*ux+uy*uy);
    dux = -ux*uz/r;
    duy = -uy*uz/r;
    duz = r;
    RotAxe ('y',M_PI_2-phic,dux,duy,duz,&dvx,&dvy,&dvz);
    *conv = atan((dvy*(1+vz)-dvz*vy)/(dvx*(1+vz)-dvz*vx));
    modlin = (n1*2*n2*cos(PHI)/cos(phi)*sqrt(1-e2*sin(phi)*sin(phi)))/(1+vz)/a;
    *scfact = (modlin-1)*1e5;
}

/**\brief alg0060 Scale factor and convergence of LCC Projection.*/
void ScaleConvLamb(double lambda, double phi, double a, double e2, double n,
        double c, double lambda0, double *conv, double *scfact)
{
	*conv = (lambda0 - lambda)*n;
	*scfact = n*c*exp(-n*LatIso(phi,e2)) / (GrandeNormale(phi,a,e2) * cos(phi));
	//ppm
	*scfact = (*scfact-1)*1e6;
}

/**\brief Scale factor and convergence of Mercator Transverse Projection.*/
void ScaleConvMercTr(double a, double lambda, double phi, double n, double lambdac,
                     double xs, double k0, double x, double *conv, double *scfact)
{
    *scfact = k0 * cosh( ( x - xs ) / a );
    *conv = -atan(sin(phi)*tan(lambda - lambdac));
    //_u_conv = U_RAD;
    //ppm
    *scfact = (*scfact-1)*1e6;
}

/**\brief Scale factor and convergence of Gauss-Laborde Projection.*/
void ScaleConvGaussLab(double a, double e2, double lambda, double phi, double n1,
                     double n2, double c, double lambdac, double *conv, double *scfact)
{
    double LAMBDA, ls;
    LAMBDA = n1 * ( lambda - lambdac );
    ls = c + n1*LATISO( phi );
    *conv =  atan (-1.0 * tanh(ls) * tan(LAMBDA) );
    *scfact = n1 * n2 * sqrt( 1 - e2 * sin(phi) * sin(phi) )
            / ( a * cos(phi) * sqrt( cosh(ls)*cosh(ls) - sin(LAMBDA)*sin(LAMBDA) ) );
    //ppm
    *scfact = (*scfact-1)*1e6;
}

/**\brief alg0069 Geographic to Secant South Polar Stereographic Projected coordinates.*/
void GeoSterPolSud(double c, double n1, double n2, double xs, double ys, double e2,
                   double lambdac, double phic, double l, double p, double* x, double* y)
{
    double LAMBDA = n1*(l-lambdac); //n1=1
    double LS = c+n1*LatIso(p,e2); //c=0
    double PHI = LatIsoInv (LS,0);
    double gg = 2*n2*(cos(PHI)/(1-sin(PHI)));
    *x = xs + gg*sin(LAMBDA);
    *y = ys + gg*cos(LAMBDA);
}

/**\brief alg0070 Secant South Polar Stereographic Projected to Geographic coordinates.*/
// This algo has to be reviewed. It is correct but in a strange shape (see rho = ...).
void SterPolSudGeo(double n1, double n2, double xs, double ys, double e2, double c,
                   double lambdac, double phic, double* l, double* p, double x, double y)
{
    double LAMBDA, PHI, LAMBDAp, PHIp, rho;
    double r = sqrt(pow(x-xs,2)+pow(y-ys,2));
    if (r==0)
        LAMBDAp = 0.;
    else
        LAMBDAp = 2*atan2(x-xs,r-y+ys);
    PHIp = M_PI_2 - 2*atan2(r,2*n2);
    rho = sqrt(pow(cos(PHIp)*cos(LAMBDAp),2) + pow(cos(PHIp)*sin(LAMBDAp),2));
    if (rho==0)
    {
        int sgn = -sin(PHIp)>0?1:-1;
        LAMBDA = 0.;
        PHI = sgn*M_PI_2;
    }
    else
    {
        LAMBDA = 2*atan2(cos(PHIp)*sin(LAMBDAp),rho-cos(PHIp)*cos(LAMBDAp));
        PHI = atan2(-sin(PHIp),rho);
    }
    *l = lambdac + LAMBDA;
    *p = LatIsoInv(LatIso(PHI,0),e2);
}

/**\brief alg0070 Test of Secant South Polar Stereographic Projection.*/
void testGeoSterPolSud()
{
    int choix = 7;
    double a, e2, lambda0, phi0, k0, x0, y0, lambdac, phic, c, n1, n2, xs, ys, l, p, x, y;
    a = 6378388;
    e2 = pow(0.081991889979,2);
    lambda0 = 2.44346095279; //140°
    phi0 = -1.16937059884; //-67°
    x0 = 300000;
    y0 = 200000;
    k0 = 0.960272946; // cas tangent
    CoefProjSterObl(choix, a, e2, lambda0, phi0, k0, x0, y0, &lambdac, &phic, &c, &n1, &n2, &xs, &ys);
    printf ("CoefProjSterObl:lambdac=%.10lf;n2=%.4lf;xs=%.4lf;ys=%.4lf\n",lambdac,n2,xs,ys);
    c = 0;
    n1 = 1;
    n2 = 6104416.791;
    xs = 300000;
    ys = -2299363.482;
    e2 = pow(0.08199188998,2);
    lambdac = 2.443460953;
    phic=-M_PI_2;
    l = 0;
    p = -1.161516617;
    GeoSterPolSud(c, n1, n2, xs, ys, e2, lambdac, phic, l, p, &x, &y);
    printf ("GeoSterPolSud:l=%.9lf;p=%.9lf/x=%.3lf;y=%.3lf\n",l,p,x,y);
    x = -1338844.577;
    y = -4252462.395;
    SterPolSudGeo(n1, n2, xs, ys, e2, c, lambdac, phic, &l, &p, x, y);
    printf ("SterPolSudGeo:x=%.3lf;y=%.3lf/l=%.10lf;p=%.10lf\n",x,y,l,p);
    l = 1.570796327;
    p = -1.308996939;
    GeoSterPolSud(c, n1, n2, xs, ys, e2, lambdac, phic, l, p, &x, &y);
    printf ("GeoSterPolSud:l=%.9lf;p=%.9lf/x=%.3lf;y=%.3lf\n",l,p,x,y);
    x = -939319.538;
    y = -1259450.915;
    SterPolSudGeo(n1, n2, xs, ys, e2, c, lambdac, phic, &l, &p, x, y);
    printf ("SterPolSudGeo:x=%.3lf;y=%.3lf/l=%.10lf;p=%.10lf\n",x,y,l,p);
}

/**\brief alg0072 Parameters of Lambert Azimuthal Equal Area (LAEA) Projection.*/
double q_(double e2, double phi)
{
	double e = sqrt(e2);
	double sp = sin(phi);
	double esp = e*sp;
	double mesp = 1 - esp;
	double pesp = 1 + esp;
	return (1-e2) * ( sp/(1-pow(esp,2)) - log(mesp/pesp)/2./e );
}

/**\brief alg0072 Parameters of Lambert Azimuthal Equal Area (LAEA) Projection.*/
double m_(double e2, double phi)
{
	return cos(phi) / sqrt(1-e2*pow(sin(phi),2));
}

/**\brief alg0072 Parameters of Geographic to Lambert Azimuthal Equal Area (LAEA) Projection.*/
void CoefProjLAEA(double a, double e2, double phi0,
			   double *qp, double *beta1, double *Rq, double *D)
{
	double e = sqrt(e2);
	double mesp = 1 - e;
	double pesp = 1 + e;
	double m1 = m_(e2, phi0);
	*qp = (1-e2) * ( 1/(1-e2) - log(mesp/pesp)/2./e );
	*beta1 = asin(q_(e2, phi0) / (*qp));
	*Rq = a*sqrt((*qp)/2);
	*D = a*m1/(*Rq)/cos(*beta1);
}

/**\brief alg0072 Geographic to Lambert Azimuthal Equal Area (LAEA) Projected coordinates.*/
void GeoLAEA(double a, double e2, double lambda0, double phi0, double qp,
			 double Rq, double beta1, double D, double x0, double y0, 
			 double lambda, double phi, double *x, double *y)
{
	double rho, beta, B;
	if (phi0 == M_PI_2)
	{
		rho = a*sqrt(qp - q_(e2, phi));
		*x = x0 + rho*sin(lambda-lambda0);
		*y = y0 - rho*cos(lambda-lambda0);
	}
	else if (phi0 == -M_PI_2)
	{
		rho = a*sqrt(qp + q_(e2, phi));
		*x = x0 + rho*sin(lambda-lambda0);
		*y = y0 + rho*cos(lambda-lambda0);
	}
	else
	{
		beta = asin(q_(e2, phi) / qp);
		B = Rq * sqrt(2/ (1+ sin(beta1)*sin(beta)
								  + cos(beta1)*cos(beta)*cos(lambda-lambda0)));
		*x = x0 + B*D*cos(beta)*sin(lambda-lambda0);
		*y = y0 + B/D*(cos(beta1)*sin(beta)
						 -sin(beta1)*cos(beta)*cos(lambda-lambda0));
	}
}

/**\brief alg0073 Lambert Azimuthal Equal Area (LAEA) Projected to Geographic coordinates.*/
void LAEAgeo(double a, double e2, double lambda0, double phi0, double qp, 
			 double Rq,	double beta1, double D, double x0, double y0,
			 double *lambda, double *phi, double x, double y)
{
	double epsilon_phi = 1e-12 ;
	double rho, ce, q, phiI, phiF;
	x -= x0;
	y -= y0;
	if (fabs(phi0) == M_PI_2)
	{
		rho = sqrt(pow(x,2)+pow(y,2));
		if (phi0 == -M_PI_2)
		{
			*lambda = lambda0 + atan2(x,y);
			q = -qp + (pow(rho/a,2));
		}
		else
		{
			*lambda = lambda0 + atan2(x,-y);
			q = qp - (pow(rho/a,2));
		}
		phiF = asin(q/2.);
		if (fabs(q) == fabs(qp))
		{
			*phi = M_PI_2*q/fabs(q);
		}
		else
		{
			do
			{
				phiI = phiF;
				phiF = phiI + pow((1-e2*pow(sin(phiI),2)), 2)/2./cos(phiI)/(1-e2)*(q-q_(e2, phiI));
			}
            while (fabs(phiF-phiI) > epsilon_phi);
			*phi = phiF;
		}
	}
	else
	{
		rho = sqrt(pow(x/D,2)+pow(D*y,2));
		if (rho == 0)
		{
			*lambda = lambda0;
			*phi = phi0;
		}
		else
		{
			ce = 2*asin(rho/2./Rq);
			q = qp*(cos(ce)*sin(beta1) + D*y*sin(ce)*cos(beta1)/rho);
			if (fabs(q) == fabs(qp))
			{
				*phi = M_PI_2*q/fabs(q);
			}
			else
			{
				*lambda = lambda0 + atan2( x*sin(ce), 
					(D*rho*cos(beta1)*cos(ce) - pow(D,2)*y*sin(beta1)*sin(ce) ));
				phiF = asin(q/2.);
				do
				{
					phiI = phiF;
					phiF = phiI + pow((1-e2*pow(sin(phiI),2)), 2)/2./cos(phiI)/(1-e2)*(q-q_(e2, phiI));
				}
                while (fabs(phiF-phiI) > epsilon_phi);
				*phi = phiF;
			}
		}
	}
}

/**\brief https://www.crs-geo.eu/crs-national.htm
For instance FR_MARS / NH to EVRF2000  */
double ShifTnTilT (double a1, double a2, double a3, double lambda0, double phi0, double a, double e2,
                 double lambda, double phi)
{
    return a1 + a2*RMer(a,e2,phi)*(phi-phi0) + a3*GrandeNormale(phi,a,e2)*(lambda-lambda0)*cos(phi);
}
