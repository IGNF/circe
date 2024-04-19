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
* \file circe.h
* \brief Definition of main Circe API, high-level and convenience functions.
*/
#ifndef CIRCE_H
#define CIRCE_H

//Add lib dependencies to build the project with gdal.
//#pragma comment is a compiler directive which indicates Visual C++ to leave a comment
//in the generated object file. The comment can then be read by the linker when it processes
//object files.
//#pragma comment(lib, libname) tells the linker to add the 'libname' library to the list of
//library dependencies, as if you had added it in the project properties at
//Linker->Input->Additional dependencies
#ifdef WITH_GDAL
#if _MSC_VER >= 1900
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"odbc32.lib")
#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"user32.lib")
#endif

#include "geodeticset.h"
#include "catoperation.h"
#include "compoundoperation.h"
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif

namespace circe
{
bool setFormatDefaults(geodeticOptions *go, GeodeticSet *GS);
std::string displayDuration(const circeOptions& circopt, time_t timer_beginning);
void endProcess(std::ofstream *fo, const circeOptions &circopt, time_t timer, unsigned int nl);
void writeVerticalInfo(GeodeticSet* GS, pt4d *p);
std::string process(int argc, char *argv[]);
#ifdef WIN32
extern "C"  __declspec(dllexport) int process2(int argc, char *argv[]);
#else
int process2(int argc, char *argv[]);
#endif
void processMultipleCRSdefinitionFile(std::string testPathname, MULTIPLE_CRS_PROCESSING mode);

std::string getAuthority(circeOptions circopt);
std::string getAuthority(circeOptions circopt, GeodeticSet* GS);
void process_double_array(circeOptions circopt, double* points);
void process_double_array(circeOptions circopt, double* points, GeodeticSet* GS);
void process_double_vector(circeOptions circopt, std::vector<double>& points);
void process_double_vector(circeOptions circopt, std::vector<double>& points, GeodeticSet* GS);
std::string processIFStream(const circeOptions &circopt);
std::string processIFStream(const circeOptions &circopt, GeodeticSet* GS);
std::string processIFStream(const circeOptions& circopt, std::vector<circe::pt4d>* vPt4d, GeodeticSet* GS, swain *swn);
std::string processOptions(circeOptions circopt);
std::string processOptions(circeOptions circopt, GeodeticSet* GS, swain *s);
circe::pt4d process_pt4d(circeOptions circopt);
circe::pt4d process_pt4d(circeOptions circopt, GeodeticSet* GS);
void process_vector_pt4d(circeOptions circopt, std::vector<pt4d> *vPt4d);
void process_vector_pt4d(circeOptions circopt, std::vector<pt4d> *vPt4d, GeodeticSet* GS);
void processPt4d(circeOptions circopt);
void processPt4d(circeOptions circopt, GeodeticSet* GS);
std::string processXMLdoc(const circeOptions& circopt);
std::string processXMLdoc(const circeOptions& circopt, GeodeticSet* GS, swain *swn);

std::string getCRSList(circeOptions circopt) /*throw(std::string)*/;
std::string getGeodeticSubset(operationSubset *OS, circeOptions *CO) /*throw(std::string)*/;
std::string getValidityArea(circeOptions circopt, MEASURE_UNIT outUnit) /*throw(std::string)*/;
std::string process(const std::string &argLine) /*throw(std::string)*/;
std::string process(const std::string& argLine, std::vector<double> &points) /*throw(std::string)*/;
circe::pt4d process_pt4d(const std::string &argLine) /*throw(std::string)*/;
std::string getCRSList(circeOptions circopt, GeodeticSet* GS) /*throw(std::string)*/;
std::string getGeodeticSubset(operationSubset *OS, circeOptions *CO, GeodeticSet* GS) /*throw(std::string)*/;
std::string getGeodeticSubsetLists(circeOptions* circopt, GeodeticSet* GS);
std::string getValidityArea(circeOptions circopt, GeodeticSet* GS, MEASURE_UNIT outUnit) /*throw(std::string)*/;
std::string process(const std::string &argLine, GeodeticSet* GS) /*throw(std::string)*/;
std::string process(const std::string& argLine, GeodeticSet* GS, swain *s) /*throw(std::string)*/;
std::string process(const std::string& argLine, std::vector<double> &points, GeodeticSet* GS) /*throw(std::string)*/;
circe::pt4d process_pt4d(const std::string &argLine, GeodeticSet* GS) /*throw(std::string)*/;
void LoadGeodeticSet(std::string filedata, GeodeticSet *GS, int verbose) /*throw(std::string)*/;
std::string InitZoneList(const circeOptions &circopt);

} //namespace circe

#endif // CIRCE_H

