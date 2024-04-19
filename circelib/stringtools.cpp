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
* \file stringtools.cpp
* \brief Implementation of string tools.
*/

#include "stringtools.h"
#include <cstring> //strtok
#include <sstream> //ostringstream
#include <iomanip> //setprecision, setfill, setw

namespace circe
{

std::string TrimTrailingZeros(std::string str)
{
    for(std::string::size_type s=str.length()-1; s>0; --s)
    {
        if(str[s] == '0') str.erase(s,1);
        else break;
    }
    return str;
}

/**
* \brief Opens an ofstream.
* \param[in] str: A filename.
*/
void openLog(std::ofstream* fo, const std::string& str)
{
    if (str == "")
        return;
    fo->open(str.data(), std::ofstream::out);
}

/**
* \brief Writes a string in an ofstream.
* \param[in] str: A std::string.
*/
void writeLog(std::ofstream* fo, const std::string& str)
{
    //std::cout << str << std::endl;
    if (fo->fail())
        return;
    //std::cout << str << std::endl;
    *fo << str << std::endl;
}

/**
* \brief Gets the index of a string in an array.
* \param[in] _value: A std::string.
* \param[in] textlist: A std::string array.
* \param[in] count: The array size.
* \return The index (default value = 0).
*/
int getIndex(const std::string& _value, const std::string* textlist, int count)
{
    for(int i = 0; i<count; i++)
        if (_value == textlist[i])
            return i;
    return 0;
}

/**
* \brief Converts a C-style string in a std::string.
* \param[in] chaine: A C-style string.
* \return A std::string.
*/
std::string asString(char *chaine)
{
    std::string ss;
    ss.assign(chaine);
    return ss;
}

/**
* \brief Converts an int in a std::string.
* \param[in] x: An int.
* \return A std::string.
*/
std::string asString(const int& x)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << x;
    return ss.str();
}

/**
* \brief Converts an unsigned int in a std::string.
* \param[in] x: An unsigned int.
* \return A std::string.
*/
std::string asString(const unsigned int& x)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << x;
    return ss.str();
}

/**
* \brief Converts a long int in a std::string.
* \param[in] x: A long int.
* \return A std::string.
*/
std::string asString(const long& x)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << x;
    return ss.str();
}

/**
* \brief Converts a double in a std::string.
* \param[in] x: A double.
* \return A std::string.
*/
std::string asString(const double& x)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << x;
    return ss.str();
}

/**
* \brief Converts a double in a std::string, with a defined precision.
* \param[in] x: A double.
* \param[in] precision: The number of decimal digits.
* \return A std::string.
*/
std::string asString(const double x, const int precision)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << std::fixed << std::setprecision(precision) << x;
    return TrimTrailingZeros(ss.str());
}

/**
* \brief Converts a double in a std::string, with defined precision and width.
* \param[in] x: A double.
* \param[in] precision: The number of decimal digits.
* \param[in] largeur: The width.
* \return A std::string.
*/
std::string asString(const double x, const int precision, const int largeur)
{
    std::ostringstream ss;
    ss.imbue( std::locale::classic() ) ;
    ss << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(largeur) << x;
    return ss.str();
}

/**
* \brief Returns the substring after the last occurrence of a char in a std::string.
* \param[in] _value: The std::string.
* \param[in] _char: The char.
* \return A substring or the whole string itself if the char is not found.
*/
std::string afterChar(const std::string& _value, char _char)
{
    std::size_t h=_value.find_last_of(_char);
    if (h<_value.size())
        return _value.substr(h+1);
    else
        return _value;
}

/**
* \brief Changes a std::string by removing the carriage return and line feed.
* \param str: Pointer to the std::string.
*/
void RemoveCR(std::string *str)
{
    size_t a = str->find_first_of("\n\r");
    if (a != std::string::npos)
    str->replace(a,a,"");
}

/**
* \brief Changes a std::string by adding the rest of the line.
* \param str: Pointer to the std::string the reading of which has been initialized in the calling function by
* a strtok instruction like: token = strtok(some_char_array, " ").
*/
void CompleteString(std::string *str)
{
    char seps[]=" \t";
    char *token = strtok(NULL, seps);
    while (token != NULL)
    {
        *str += " ";
        *str += token;
        token = strtok(NULL, seps);
    }
    RemoveCR(str);
}

/**
* \brief Changes the extension of a pathname.
* \param[in] pathname: The pathname as a std::string.
* \param[in] newExt: The new extension.
* \return pathname with the new extension.
*/
std::string replaceExtension(std::string pathname, std::string newExt)
{
    return pathname.substr(0,pathname.find_last_of('.'))+"."+newExt;
}

/**
* \brief Gets the extension of a pathname.
* \param[in] pathname: The pathname as a std::string.
* \return The extension.
*/
std::string getExtension(std::string pathname)
{
    std::size_t p=pathname.find_last_of('.');
    if (p<pathname.size())
        return pathname.substr(p+1);
    else
        return std::string("");
}

/**
* \brief Gets the filename of a pathname, without extension.
* \param[in] pathname: The pathname as a std::string.
* \return The filename.
*/
std::string getFilename(std::string pathname)
{
    int start;
    std::size_t sep = pathname.find_last_of("/\\");
    if (sep>pathname.size())
        start = 0;
    else
        start = sep+1;
    std::size_t p=pathname.find_first_of('.',start);
    if (p>pathname.size())
        p = pathname.size();
    return pathname.substr(start,p-start);
}

/**
* \brief Gets the directory of a pathname.
* \param[in] pathname: The pathname as a std::string.
* \return The directory.
*/
std::string getDirname(std::string pathname)
{
    std::size_t sep = pathname.find_last_of("/\\");
    if (sep>pathname.size())
        return "";
    else
        return pathname.substr(0,sep+1);
}

/**
* \brief Replaces the last char of a string.
* \param[in] str: The std::string.
* \param[in] ch: The char.
* \return The changed string.
*/
std::string replaceLastChar(std::string str, char ch)
{
    return str.substr(0,str.size()-1)+ch;
}

/**
* \brief Replaces all occurences of a char in a string.
* \param[in] str: The std::string.
* \param[in] chi: The input char.
* \param[in] cho: The output char.
* \return The changed string.
*/
std::string replaceChar(std::string str, char chi, char cho)
{
    for(std::string::size_type s=0; s<str.length(); s++)
    {
        if(str[s] == chi) str[s] = cho;
    }
    return str;
}


}

