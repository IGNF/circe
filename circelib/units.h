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
* \file units.h
* \brief Definition of unit management.
*/

#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <cmath>
#include <vector>
#include <cfloat>  //LDBL_DIG (=18)
#include <cstring> //memmove
#include <time.h>  //time_t, struct tm, difftime, time, mktime
#include <fstream>
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif

namespace circe
{
#define _MAX_PATH_ 256
#define LPREC (LDBL_DIG+3)
#define DEG2RAD 0.017453292519943295 //M_PI/180.
#define RAD2DEG 57.295779513082323 //180./M_PI
#define SEC2RAD 4.8481368110953598e-06  //M_PI/180./3600.

/** \brief Accomodation for sexagesimal coordinates in Degree Minute Second (DMS) format.*/
struct DMS
{
    bool neg;
    int deg;
    int min;
    double sec;
} ;

/** \brief Accomodation for sexagesimal coordinates in Degree Minute (DM) format.*/
struct DM
{
    bool neg;
    int deg;
    double min;
} ;

double dec_round(double z, int ndec);
double trunk(double z, int ndec);
void double_to_dms(long double a, DMS *dms, unsigned int n_ang);
void double_to_dms_2(double a, DMS *dms, unsigned int n_ang);
long double dms_to_double(DMS *dms);
long double dm_to_double(DM *dm);
void double_to_dm(long double a, DM *dm);
void double_to_dm_2(double a, DM *dm, unsigned int n_ang);
long double deci_to_sexadms(long double val);
long double sexadms_to_deci(long double val);
long double sexadm_to_deci(long double val);
long double deci_to_sexadm(long double val);
double readDMS(std::string str, std::string what, bool bValide);

std::string DecodeUnit(const std::string& code);
double UnitConvert(double v, MEASURE_UNIT u1, MEASURE_UNIT u2);
void test_deci_sexadms(std::ofstream *fo, double doudou1);
void testUnits(std::string filout);
void test_decimal_separator();
std::string writeDMS (double aa, std::string type, unsigned int n_ang, bool plainDMS);
std::string writeDM (double aa, std::string type, unsigned int n_ang, bool plainDMS);

}
#endif // TOOLS_H
