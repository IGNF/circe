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
* \file files.h
* \brief Definition of point data input and output.
*/

#ifndef FILES
#define FILES
#include <fstream>
#include "units.h"
#include "pt4d.h"
#include "parser.h"
#include "xmlparser.h"

#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif

namespace circe
{
//general
void openOutputFile(std::ofstream *fo, const circeOptions& circopt, const std::string& str, int TransfoRequired);
void writeOutputLine(std::ostream *fo, const circeOptions& circopt, const pt4d &p, unsigned int *idt);
void closeOutputFile(std::ofstream *fo, const circeOptions &circopt);
//pt4d
void writeVectorPt4d(const circeOptions &circopt, const std::vector<circe::pt4d> &vPt4d);
void displayVectorPt4d(std::ostream *fo, const std::vector<circe::pt4d>& vPt4d, const circeOptions &circopt);
//basic file
void initBasicOstream(std::ostream *fo);
DATA_VALIDATION readBasicLine(std::ifstream *fi, const circeOptions &circopt, const std::string& lnfrmt,  pt4d *p, unsigned int *idt);
DATA_VALIDATION readBasicLine2(std::ifstream *fi, const circeOptions &circopt, const std::string& lnfrmt,  pt4d *p, unsigned int *idt);
void writeBasicHeader(std::ostream *fo, const circeOptions& circopt, const std::string& str, bool writePrec);
void writeBasicHeader2(std::ostream *fo, const circeOptions& circopt, const std::string& str, int TransfoRequired);
void writeBasicLine(std::ostream *fo, const circeOptions &circopt, const pt4d &p, unsigned int *idt, std::string lnfrmt);
//KML file
void readKMLline(tinyxml2::XMLElement *XMLelem, pt4d *p);
void writeKMLheader(std::ostream *fo, const circeOptions& circopt);
void writeKMLline(std::ostream *fo, const pt4d& p);
void writeKMLtail(std::ostream *fo);
//GPX file
void readGPXline(tinyxml2::XMLElement *XMLelem, pt4d *p);
void writeGPXheader(std::ostream *fo, const circeOptions& circopt);
void writeGPXline(std::ostream *fo, const pt4d& p);
void writeGPXtail(std::ostream *fo);
//NMEA
//DATA_VALIDATION readNMEAline(std::ifstream *fi, pt4d *p, unsigned int *idt);
//void writeNMEAline(std::ostream *fo, const pt4d& p);

}
#endif // FILES

