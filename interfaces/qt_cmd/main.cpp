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
#include "../../circelib/circe.h"
#ifdef UI_LANGUAGE_FRENCH
#include "../../circelib/StringResourcesFR.h"
#else
#include "../../circelib/StringResourcesEN.h"
#endif
#include <iomanip> //setprecision, setfill, setw
//#include <QString>

//#if defined WIN32
//int wmain(int argc, wchar_t *argv[])
//#else
int main(int argc, char *argv[])
//#endif
{
    std::locale::global(std::locale::classic());
    int iargv;
    circe::circeOptions circopt;
    std::ofstream fo;
    circe::swain s;
    s.fi = NULL;
    s.fo = NULL;
    try
    {
        circe::initDefOptions(&circopt);
        circe::parseArgArray(&circopt, argc, argv, &iargv);
        circe::initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(circopt.Pt4d.frmt));
        circe::openLog(&fo, circopt.logPathname);
        circe::scrutinizeOptions (&circopt);
//        s.fi = new std::ifstream();
//        s.fi->open(QString(circopt.sourceOptions.pathname.data()).toLocal8Bit());
//        qDebug("%s",circopt.sourceOptions.pathname.data());
//        if (!(s.fi->is_open()))
//            throw circe::ErrorMessageText[circe::EM_CANNOT_OPEN]+circopt.sourceOptions.pathname;
        std::cout << circe::processOptions(circopt,NULL,&s) << std::endl;
        fo.close();
    }
    catch (std::string str)
    {
        circe::writeLog(&fo, circe::CIRCE_ERROR);
        circe::writeLog(&fo, str);
        fo.close();
        std::cout << circe::displayOutput(circopt, str, "{", "}", true) << std::endl;
    }
    catch(std::exception e)
    {
       std::cout << "Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
      std::cout << "Non-processed exception!" << std::endl;
    }
}
