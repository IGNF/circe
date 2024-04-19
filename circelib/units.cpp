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
* \file units.cpp
* \brief Implementation of unit management.
*/

#include "units.h"
#include "stringtools.h"
#include <sstream> //std::ostringstream
#include <iomanip> //testUnits
#include <iostream>//testUnits
#include <cstdlib>//testUnits //rand
#include <locale>  // std::locale, std::isdigit
//#include <ctype.h>//isdigit
#include <cstdio> //remove
#include <regex>

namespace circe
{

/**
* \brief Decodes a std::string unit code in a MeasureUnitAttributeText element.
* \param[in] code: a std::string unit code stemming from a Legacy Metadata File (mainly the numbers) or IGNF.xml.
* \return a std::string containing a MeasureUnitAttributeText element.
*
* \see MeasureUnitAttributeText.
*/
std::string DecodeUnit(const std::string& code)
{
    if (code=="01" || code=="rad") {return "RADIANS";}
    else if (code=="02" || code=="deg" || code=="dec") {return "DEGREES";}
    else if (code=="03" || code=="gr") {return "GONS";}
    else if (code=="05" || code=="sec") {return "SECONDS";}
    else if (code=="10") {return "DMS";}
    else if (code=="12") {return "DM";}
    else if (code=="m") {return "METERS";}
    else if (code=="ppm") {return "PPM";}
    else if (code=="dpmy") {return "DEGPERMY";}
	else if (code=="mpy") { return "MASPERY"; }
    else return code;
}

double dec_round(double z, int ndec)
{
    double p = pow(10.,ndec);
    int sign;
    if (z < 0)
    {
        sign = -1;
        z = -z;
    }
    else
        sign = 1;
    return sign * floor(z*p + .5) / p;
}

double trunk(double z, int ndec)
{
    double p = pow(10.,ndec);
    int sign;
    if (z < 0)
    {
        sign = -1;
        z = -z;
    }
    else
        sign = 1;
    return sign * floor(z*p) / p;
}

/**
* \brief Converts a sexagesimal double value in a DMS instance.
* \param[in] a: a sexagesimal value in decimal format (DD.MMSSSSSSSS).
* \param[out] dms: The value in a DMS instance.
*/
void double_to_dms(long double a, DMS *dms, unsigned int n_ang)
{
    //max sprintf length of a long double
    static char str[LPREC+2];
    sprintf(str, "%*.*Lf", LPREC, LPREC-5, a<0 ? -a : a);
    memmove(str+10, str+9, LPREC-8);
    str[9]='.';
    sscanf(str, "%d.%2d%lf", &dms->deg, &dms->min, &dms->sec);
    dms->neg = (a < 0);
    //20.000 may go 19.999999 instead of 19.599999
    dms->sec = dec_round(dms->sec,n_ang-4);
    if (dms->min > 90)
        dms->min -= 40;
    if (dms->sec > 90)
        dms->sec -= 40;
    if (dms->sec >= 60)
    {
        dms->sec -= 60;
        dms->min += 1;
    }
    if (dms->min >= 60)
    {
        dms->min -= 60;
        dms->deg += 1;
    }
}

/**
* \brief Converts a sexagesimal double value in a DMS instance.
* \param[in] a: a sexagesimal value in decimal format (DD.MMSSSSSSSS).
* \param[out] dms: The value in a DMS instance.
*/
void double_to_dms_2(double a, DMS *dms, unsigned int n_ang)
{
    //max sprintf length of a long double
    static char str[LPREC+2];
    sprintf(str, "%*.*lf", LPREC, LPREC-5, a<0 ? -a : a);
    memmove(str+10, str+9, LPREC-8);
    str[9]='.';
    sscanf(str, "%d.%2d%lf", &dms->deg, &dms->min, &dms->sec);
    dms->neg = (a < 0);
    dms->sec = dec_round(dms->sec,n_ang-4);
    if (dms->sec == 100. || dms->sec == 60.)
    {
        dms->sec = 0.;
        dms->min += 1;
    }
    if (dms->min == 100 || dms->min == 60)
    {
        dms->min = 0;
        dms->deg += 1;
    }
}

/**
* \brief Converts a DMS instance in a sexagesimal double value.
* \param[in] dms: The value in a DMS instance.
* \return A sexagesimal value in decimal format (DD.MMSSSSSSSS).
*/
long double dms_to_double(DMS *dms)
{
    long double a;
    a = dms->deg + dms->min/100.L + dms->sec/10000.L;
    return dms->neg ? -a : a;
    //There is still a rounding issue when converting from long double to double:
    //19.5999999 goes 19.6000, but it's a printing matter.
}

/**
* \brief Converts a DM instance in a sexagesimal value in decimal format.
* \param[in] dms: The value in a DM instance.
* \return A sexagesimal value in decimal format (DD.MMMMMMMMMMM).
*/
long double dm_to_double(DM *dm)
{
    long double a;
    a = dm->deg + dm->min/100.L;
    return dm->neg ? -a : a;
}

/**
* \brief Converts a sexagesimal value in decimal format in a DM instance.
* \param[in] a: A sexagesimal value in decimal format (DD.MMMMMMMMMMM).
* \param[out] dms: The value in a DM instance.
*/
void double_to_dm(long double a, DM *dm)
{
    //max sprintf length of a long double
    static char str[LPREC+2];
    sprintf(str, "%*.*Lf", LPREC, LPREC-5, a<0 ? -a : a);
    memmove(str+8, str+7, LPREC-6);
    str[7]='.';
    sscanf(str, "%d.%lf", &dm->deg, &dm->min);
    dm->neg = (a < 0);
    //20.000 goes 19.999999 instead of 19.599999
    if (dm->min > 90)
        dm->min -= 40;
    if (dm->min >= 60)
    {
        dm->min -= 60;
        dm->deg += 1;
    }
}

/**
* \brief Converts a sexagesimal value in decimal format in a DM instance.
* \param[in] a: A sexagesimal value in decimal format (DD.MMMMMMMMMMM).
* \param[out] dms: The value in a DM instance.
*/
void double_to_dm_2(double a, DM *dm, unsigned int n_ang)
{
    //max sprintf length of a long double
    static char str[LPREC+2];
    sprintf(str, "%*.*lf", LPREC, LPREC-5, a<0 ? -a : a);
    memmove(str+8, str+7, LPREC-6);
    str[7]='.';
    sscanf(str, "%d.%lf", &dm->deg, &dm->min);
    dm->neg = (a < 0);
    //20.000 goes 19.999999 instead of 19.599999
    dm->min = dec_round(dm->min,n_ang-2);
    if (dm->min == 100 || dm->min == 60)
    {
        dm->min = 0;
        dm->deg += 1;
    }
}

/**
* \brief Converts a decimal degree value in a sexagesimal value in decimal format.
* \param[in] val: A decimal degree value.
* \return A sexagesimal value in decimal format (DD.MMSSSSSSSS).
*/
long double deci_to_sexadms(long double val)
{
    DMS dms;
    long double x;

    x=fabs(val);
    dms.deg = (int)x;
    x -= dms.deg;
    x *= 60;
    x = dec_round(x,8);
    dms.min = (int)x;
    x -= dms.min;
    x = dec_round(x,8);
    dms.sec = 60*x;
    dms.neg = (val<0);
    return dms_to_double(&dms);
}

/**
* \brief Converts a sexagesimal value in decimal format in a decimal degree value.
* \param[in] val: A sexagesimal value in decimal format (DD.MMSSSSSSSS).
* \return A decimal degree value.
*/
long double sexadms_to_deci(long double val)
{
    DMS dms;
    long double result;

    //double_to_dms(val, &dms);
    double_to_dms(val, &dms, 9);
    result = dms.deg + dms.min/60.L + dms.sec/3600.L;
    return dms.neg ? -result : result;
}

/**
* \brief Converts a sexagesimal value in decimal format in a decimal degree value.
* \param[in] val: A sexagesimal value in decimal format (DD.MMMMMMMMM).
* \return A decimal degree value.
*/
long double sexadm_to_deci(long double val)
{
    DM dm;
    long double result;

    double_to_dm(val, &dm);
    result = dm.deg + dm.min/60.L;
    return dm.neg ? -result : result;
}

/**
* \brief Converts a decimal degree value in a sexagesimal value in decimal format.
* \param[in] val: A decimal degree value.
* \return A sexagesimal value in decimal format (DD.MMMMMMMMM).
*/
long double deci_to_sexadm(long double val)
{
    DM dm;
    long double x;

    x=fabs(val);
    dm.deg = (int)x;
    x -= dm.deg;
    dm.min = 60*x;
    dm.neg = (val<0);
    return dm_to_double(&dm);
}

/**
* \brief Converts a value from a unit to another.
* \param[in] v: the value.
* \param[in] u1: the MEASURE_UNIT code for input unit.
* \param[in] u2: the MEASURE_UNIT code for output unit.
* \throw a std:string containing the error message (if any).
* \return the converted value.
*/
double UnitConvert(double v, MEASURE_UNIT u1, MEASURE_UNIT u2)
{
    if (u1==u2 || v==0. || u1==MU_METER || u2==MU_METER) return v;

    //from u1 to decimal degrees
    switch (u1)
    {
    case MU_SECOND:
        v /= 3600.;
        break;
    case MU_DEGREE:
        break;
    case MU_RADIAN:
        v *= RAD2DEG;
        break;
    case MU_DMS:
        v = sexadms_to_deci(v);
        break;
    case MU_DM:
        v = sexadm_to_deci(v);
        break;
    case MU_GON:
		v *= 0.9;
		break;
	case MU_DEGPERMY:
		v *= 1e-6;
		break;
	case MU_MASPERY:
		v /= 3600000.;
		break;
	default:
        throw std::string("UnitConvert: unimplemented unit "+asString(int(u1))+" (value="+asString(v)+")");
        break;
    }
    //from decimal degrees to u2
    switch (u2)
    {
    case MU_SECOND:
        v *= 3600.;
        break;
    case MU_DEGREE:
        break;
    case MU_RADIAN:
        v *= DEG2RAD;
        break;
    case MU_DMS:
        v = deci_to_sexadms(v);
        break;
    case MU_DM:
        v = deci_to_sexadm(v);
        break;
    case MU_GON:
        v /= 0.9;
        break;
	case MU_DEGPERMY:
		v *= 1e6;
		break;
	case MU_MASPERY:
		v *= 3600000.;
		break;
	default:
        throw std::string("UnitConvert: unimplemented unit "+asString(int(u2))+" (value="+asString(v)+")");
        break;
    }
    return v;
}

void test_deci_sexadms(std::ofstream *fo, double doudou1)
{
    double doudou2 = deci_to_sexadms(doudou1);
    double doudou3 = sexadms_to_deci(doudou2);
    double diff = doudou3-doudou1;
    if (fabs(diff) < 1e-13) diff=0;
    *fo << std::setw(11) << doudou1 << " ";
    *fo << std::setw(11) << doudou2 << " ";
    *fo << std::setw(11) << doudou3 << " ";
    *fo << std::setw(11) << diff << std::endl;
}

void testUnits(std::string filout)
{
    int nPoints = 1000;
    std::ofstream fo(filout.data());
    fo << std::setw(10) << std::setprecision(10) << std::setfill(' ');
    double doudou1, domin=-180., domax=180.;
    doudou1 = -179.9;
    test_deci_sexadms(&fo, doudou1);
    for (int iPoint=0; iPoint<nPoints; iPoint++)
    {
        doudou1 = (domin+(domax-domin)*(rand()%100000)/100000);
        test_deci_sexadms(&fo, doudou1);
    }
    for (int iPoint=0; iPoint<nPoints; iPoint++)
    {
        doudou1 = (domin+iPoint/100.);
        test_deci_sexadms(&fo, doudou1);
    }
    fo.close();
}

/**
* \brief Converts a std::string in a sexagesimal value in decimal format.
* \param[in] str A std::string.
* \param[in] what "longitude DMS" or "longitude DM" or "latitude DMS" or "latitude DM"
* \param[in] bValid Boolean true if the coordinate must be validated.
* \return A sexagesimal value in decimal format (DD.MMSSSSSSSS or DD.MMMMMMMMMMM).
*
* \remark The input format is rather loose, it may be for instance:
*- -55.4409977
*-   055°44'09.97" W
*-  -055°44'09.97"
*- W 055°44'09.97"
*-   055 44 09.97 W
*-  -055 44 09.97
*- W 055 44 09.97
*- etc.
*/
double readDMS(std::string str, std::string what, bool bValid)
{
    double coord_as_double = 0.;
    DMS dms;
    DM dm;
    dms.neg = dm.neg = false;
    dms.deg = dms.min = dm.deg = 0;
    dms.sec = dm.min = 0.;
    std::locale loc;
    std::string sign_lst="";

    // What is it about

    bool bLongitude = (what.find("longitude") != std::string::npos);
    bool bLatitude = (what.find("latitude") != std::string::npos);
    bool bDMS = (what.find("DMS") != std::string::npos);
    bool bDM = ((what.find("DM") != std::string::npos) && !bDMS);

    // Regular expressions corresponding to valid DMS or DM values

    // Valid sign symbols that may be written at the beginning or the end of
    // the input string (and even at both ends !- should be avoided).
    if (bLongitude)
        sign_lst = "([-+WwEe ]+)?";
    else if (bLatitude)
        sign_lst = "([-+NnSs ]+)?";

    // DMS: Degrees (int), minutes (int), seconds (real) separated by spaces or symbols
    std::regex str_expr_dms_3 (sign_lst+"[0-9]+[° ]+[0-9]+[' ]+[0-9.]+([\" ]+)?"+sign_lst);

    // DMS: Degrees (int), minutes (int) (without seconds) separated by spaces or symbols
    std::regex str_expr_dms_2 (sign_lst+"[0-9]+[° ]+[0-9]+([' ]+)?"+sign_lst);

    // DM: Degrees (int), minutes (real) separated by spaces or symbols
    std::regex str_expr_dm_2  (sign_lst+"[0-9]+[° ]+[0-9.]+([' ]+)?"+sign_lst);

    // DMS or DM: Sexagesimal value in decimal format.
    std::regex str_expr_dm_s_1 (sign_lst+"[0-9.]+([° ]+)?"+sign_lst);

    // Does input string fit to a regex ?

    if  ( !( (bDMS && regex_match (str,str_expr_dms_3))
          || (bDMS && regex_match (str,str_expr_dms_2))
          || (bDM  && regex_match (str,str_expr_dm_2))
          || (regex_match (str,str_expr_dm_s_1)) ))
    {
        if (bValid)
            throw std::string(ArgumentValidationText[AV_BAD_DMS_VALUE]+" : "+str);
        else
            return 0.;
    }

    // Copy numerical values (separated by spaces or symbols) into a vector.

    const std::regex coord_sep("[°'\" A-Za-z]+");
    std::sregex_token_iterator it ( str.begin(), str.end(), coord_sep, -1 );
    std::sregex_token_iterator itend;
    std::vector<std::string> vec(it, itend);
    if (vec[0]=="") // if the input string begins with a space
        vec.erase(vec.begin());

    // Convert values into sexagesimal value in decimal format.

    if (vec.size()==1)
    {
        // DMS or DM: Already in decimal format.
        coord_as_double = atof(vec[0].data());
        if (str.find_first_of("SsWw") != std::string::npos)
            coord_as_double = -coord_as_double;
//        long double a = coord_as_double;
        if (bDMS)
        {
            double_to_dms_2(coord_as_double, &dms, 10);
            if (bValid && (dms.sec<0. || dms.sec>=60. || dms.min<0 || dms.min>=60))
                throw std::string(ArgumentValidationText[AV_BAD_DMS_VALUE]+" : "+str);
            coord_as_double = dms_to_double(&dms);
        }
        else if (bDM)
        {
            double_to_dm_2(coord_as_double, &dm, 10);
            if (bValid && (dm.min<0. || dm.min>=60.))
               throw std::string(ArgumentValidationText[AV_BAD_DM_VALUE]+" : "+str);
            coord_as_double = dm_to_double(&dm);
        }
    }
    else
    {
        if (bDMS)
        {
            if (vec.size()>0)
            {
                dms.deg = atoi(vec[0].data());
                dms.neg = (dms.deg<0);
                if (dms.neg) dms.deg = -dms.deg;
            }
            if (vec.size()>1)
                dms.min = atoi(vec[1].data());
            if (vec.size()>2)
                dms.sec = atof(vec[2].data());
            if (str.find_first_of("SsWw") != std::string::npos)
                dms.neg = true;
            coord_as_double = dms_to_double(&dms);
            if (bValid && (dms.sec<0. || dms.sec>=60. || dms.min<0 || dms.min>=60))
                   throw std::string(ArgumentValidationText[AV_BAD_DMS_VALUE]+" : "+str);
        }
        else if (bDM)
        {
            if (vec.size()>0)
            {
                dm.deg = atoi(vec[0].data());
                dm.neg = (dm.deg<0);
                if (dm.neg) dm.deg = -dm.deg;
            }
            if (vec.size()>1)
                dm.min = atof(vec[1].data());
            if (str.find_first_of("SsWw") != std::string::npos)
                dm.neg = true;
            coord_as_double = dm_to_double(&dm);
            if (bValid && (dm.min<0. || dm.min>=60.))
                   throw std::string(ArgumentValidationText[AV_BAD_DM_VALUE]+" : "+str);
        }
    }
    return coord_as_double;
}

void test_decimal_separator()
{
	std::ofstream fo;
	fo.open("_test_decimal_separator.tmp");
	fo << "1.5";
	fo.close();
	std::ifstream fi;
	fi.open("_test_decimal_separator.tmp");
	float flo;
	fi >> flo;
	fi.close();
    remove("_test_decimal_separator.tmp");
	if (flo != 1.5)
		throw std::string("!!! set point as decimal separator !!!");
}

/**
* \brief Writes a sexagesimal double value in a human-readable form.
* \param[in] aa: a sexagesimal value in decimal format (DD.MMSSSSSSSS).
* \return: The value in a human-readable form (...°...'...").
*/
std::string writeDMS (double aa, std::string type, unsigned int n_ang, bool plainDMS)
{
    DMS dms;
    char aff[20], frmt_sec[20], frmt[20];

    double_to_dms((long double)(aa), &dms, n_ang);

    if (!plainDMS)
    {
        std::ostringstream ss;
        ss << std::setprecision(n_ang) << std::fixed << dms_to_double(&dms);
        return ss.str();
    }

    if (n_ang == 0)
    {
        if      (type=="lat") sprintf(aff,"%02d°",dms.deg);
        else if (type=="lon") sprintf(aff,"%03d°",dms.deg);
        else if (type=="rot") sprintf(aff,"%03d ",dms.deg);
    }
    else if (n_ang < 3)
    {
        if      (type=="lat") sprintf(aff,"%02d°%02d'",dms.deg, dms.min);
        else if (type=="lon") sprintf(aff,"%03d°%02d'",dms.deg, dms.min);
        else if (type=="rot") sprintf(aff,"%03d %02d ",dms.deg, dms.min);
    }
    else
    {
        if (n_ang < 5)
            n_ang = 0;
        else
            n_ang -= 4;
        if (n_ang==0) sprintf(frmt_sec,"%s","%02.0lf");
        else sprintf(frmt_sec,"%s%d.%d%s","%0",n_ang+3,n_ang,"lf");
        if      (type=="lat") sprintf(frmt,"%s%s%s","%02d°%02d'",frmt_sec,"\"");
        else if (type=="lon") sprintf(frmt,"%s%s%s","%03d°%02d'",frmt_sec,"\"");
        else if (type=="rot") sprintf(frmt,"%s%s","%03d %02d ",frmt_sec);
        sprintf(aff,frmt,dms.deg, dms.min, dms.sec);
    }

    std::string ret=aff;
    if      ( dms.neg && type=="lat") return ret+"S ";
    else if (!dms.neg && type=="lat") return ret+"N ";
    else if ( dms.neg && type=="lon") return ret+"W ";
    else if (!dms.neg && type=="lon") return ret+"E ";
    else if ( dms.neg && type=="rot") return "- "+ret;
    else if (!dms.neg && type=="rot") return "  "+ret;
    return "";
}


/**
* \brief Writes a sexagesimal double value in a human-readable form.
* \param[in] aa: a sexagesimal value in decimal format (DD.MMMMMMMMM).
* \return: The value in a human-readable form (...°...').
*/
std::string writeDM (double aa, std::string type, unsigned int n_ang, bool plainDMS)
{
    DM dm;
    char aff[20], frmt_min[20], frmt[20];

    //double_to_dm((long double)(aa), &dm, n_ang);
    double_to_dm((long double)(aa), &dm);

    if (!plainDMS)
    {
        std::ostringstream ss;
        ss << std::setprecision(n_ang) << std::fixed << dm_to_double(&dm);
        return ss.str();
    }

    if (n_ang == 0)
    {
        if      (type=="lat") sprintf(aff,"%02d°",dm.deg);
        else if (type=="lon") sprintf(aff,"%03d°",dm.deg);
        else if (type=="rot") sprintf(aff,"%03d ",dm.deg);
    }
    else if (n_ang < 3)
    {
        if      (type=="lat") sprintf(aff,"%02d°%02.0lf'",dm.deg, dm.min);
        else if (type=="lon") sprintf(aff,"%03d°%02.0lf'",dm.deg, dm.min);
        else if (type=="rot") sprintf(aff,"%03d %02.0lf ",dm.deg, dm.min);
    }
    else
    {
        n_ang -= 2;
        sprintf(frmt_min,"%s%d.%d%s","%0",n_ang+3,n_ang,"lf");
        if      (type=="lat") sprintf(frmt,"%s%s%s","%02d°",frmt_min,"'");
        else if (type=="lon") sprintf(frmt,"%s%s%s","%03d°",frmt_min,"'");
        else if (type=="rot") sprintf(frmt,"%s%s","%03d ",frmt_min);
        sprintf(aff,frmt,dm.deg, dm.min);
    }

    std::string ret=aff;
    if      ( dm.neg && type=="lat") return ret+"S ";
    else if (!dm.neg && type=="lat") return ret+"N ";
    else if ( dm.neg && type=="lon") return ret+"W ";
    else if (!dm.neg && type=="lon") return ret+"E ";
    else if ( dm.neg && type=="rot") return "- "+ret;
    else if (!dm.neg && type=="rot") return "  "+ret;
    return "";
}

}
