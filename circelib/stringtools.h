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
* \file stringtools.h
* \brief Definition of string tools.
*/

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H
#include <string>
#include <fstream>
#include <iostream>
#ifdef UI_LANGUAGE_FRENCH
#include "StringResourcesFR.h"
#else
#include "StringResourcesEN.h"
#endif
namespace circe
{
std::string TrimTrailingZeros(std::string str);
void openLog(std::ofstream* fo, const std::string& str);
void writeLog(std::ofstream* fo, const std::string& str);
int getIndex(const std::string& _value, const std::string* textlist, int count);
std::string asString(char *chaine);
std::string asString(const int& x);
std::string asString(const unsigned int& x);
std::string asString(const long& x);
std::string asString(const double& x);
std::string asString(const double x, const int precision);
std::string asString(const double x, const int precision, const int largeur);
std::string afterChar(const std::string& _value, char _char);
void CompleteString(std::string *str);
void RemoveCR(std::string *str);
std::string replaceExtension(std::string pathname, std::string newExt);
std::string getDirname(std::string pathname);
std::string getExtension(std::string pathname);
std::string getFilename(std::string pathname);
std::string replaceLastChar(std::string str, char ch);
std::string replaceChar(std::string str, char chi, char cho);
}
#endif // STRINGTOOLS_H

