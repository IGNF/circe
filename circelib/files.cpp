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
* \file files.cpp
* \brief Implementation of point data input and output.
*/

#include <limits> //numeric_limits
#include <iomanip> //setprecision, setw
#include <sstream> //istringstream
#include "files.h"
#include "stringtools.h"
//#include <QDebug>

namespace circe
{

/**
* \brief Initializes a std::ostream for further output.
* \param fo: A pointer to a std::ostream.
*/
void initBasicOstream(std::ostream *fo)
{
    *fo << std::fixed << std::setfill(' ');
    fo->imbue( std::locale::classic() );
}

/**
* \brief Writes the content of a pt4d vector to a file.
* \param[in] circopt: processing options in a circeOptions structure.
* \param[in] vPt4d: A pointer to the vector of pt4d.
*/
void writeVectorPt4d(const circeOptions &circopt, const std::vector<circe::pt4d> &vPt4d)
{
    std::ofstream ofs;
    std::ostringstream oss;
    initBasicOstream(&oss);
    displayVectorPt4d (&oss, vPt4d, circopt);
    openOutputFile(&ofs, circopt, "", false);
    ofs << oss.str();
    closeOutputFile(&ofs,circopt);
    return;
}

/**
* \brief Writes the content of a vector of pt4d in a std::ostream.
* \param[in] formatId: data format as a std::string.
* \param fo: A pointer to a std::ostream.
* \param[in] vPt4d: A vector of pt4d instance.
*/
void displayVectorPt4d(std::ostream *fo, const std::vector<circe::pt4d>& vPt4d, const circeOptions &circopt)
{
	std::vector<circe::pt4d>::const_iterator itPt4d;
	unsigned int idt;
	for (itPt4d = vPt4d.begin(); itPt4d<vPt4d.end(); itPt4d++)
	{
        //itPt4d->transfoIndex = 0; //don't write transformation precision information
        writeBasicLine(fo, circopt, *itPt4d, &idt, circopt.targetOptions.formatId);
		*fo << "\n";
	}
}

/**
* \brief Opens a std::ofstream and writes a header if needed.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ofstream.
* \throw a std:string containing the error message (if any).
*/
void openOutputFile(std::ofstream *fo, const circeOptions &circopt, const std::string& str, int TransfoRequired)
{
    if (!fo->is_open())
        fo->open(circopt.targetOptions.pathname.data(),std::ofstream::out);
    if (!(fo->is_open()))
        throw(DataValidationText[DV_CANNOT_OPEN_OUTPUT])+": "+circopt.targetOptions.pathname;
    switch (circopt.targetOptions.subformatId)
    {
    case FF_BASIC:
        initBasicOstream(fo);
        writeBasicHeader2(fo, circopt, str, TransfoRequired);
        break;
    case FF_KML_POINT:
        writeKMLheader(fo,circopt);
        break;
    case FF_GPX_POINT:
        writeGPXheader(fo,circopt);
        break;
    case FF_NMEA_POINT:
        break;
    default:
        break;
    }
    return;
}

/**
* \brief Writes a single point data in a std::ofstream.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ofstream.
* \param[in] p: A pt4d instance.
* \param[in] writePrec: True to write precision information.
* \param[out] fo: A pointer to the int position in the data format where an error occured (if any).
*/
void writeOutputLine(std::ostream *fo, const circeOptions& circopt, const pt4d &p, unsigned int *idt)
{
    switch (circopt.targetOptions.subformatId)
    {
    case FF_BASIC:
        writeBasicLine(fo, circopt, p, idt, circopt.targetOptions.formatId);
        *fo << std::endl;
        break;
    case FF_KML_POINT:
        writeKMLline(fo, p);
        break;
    case FF_GPX_POINT:
        writeGPXline(fo, p);
        break;
    case FF_NMEA_POINT:
        //writeNMEAline(fo, p);
        break;
    default:
        break;
    }
    return;
}

/**
* \brief Writes a tail if needed and closes a std::ofstream.
* \param[in] circopt: processing options in a circeOptions structure.
* \param[in] fo: A pointer to a std::ofstream.
*/
void closeOutputFile(std::ofstream *fo, const circeOptions& circopt)
{
    switch (circopt.targetOptions.subformatId)
    {
    case FF_BASIC:
        break;
    case FF_KML_POINT:
        writeKMLtail(fo);
        break;
    case FF_GPX_POINT:
        writeGPXtail(fo);
        break;
    case FF_NMEA_POINT:
        break;
    default:
        break;
    }
    fo->close();
}

/**
* \brief Reads a single point data in a std::ifstream.
* \param[in] lnfrmt: data format.
* \param fi: A pointer to a std::ifstream.
* \param[out] p: A pointer to a pt4d instance.
* \param[out] fo: A pointer to the int position in the data format where an error occured (if any).
* \return A DATA_VALIDATION code to be converted to an error message (if any).
*/
//not much format validation to save performance (but other domain validation in Operate)
DATA_VALIDATION readBasicLine(std::ifstream *fi, const circeOptions &circopt,
                              const std::string& lnfrmt,  pt4d *p, unsigned int *idt)
{
    std::string foo;
    p->e=p->n=p->l=p->p=p->h=p->x=p->y=p->z=p->w=0.;
    p->id = p->info = "";
    char fp = fi->peek();
    if (fp == '*' || fp == '\r' || fp == '\n')
    {
        fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return DV_COMMENT;
    }

	//std::string line;
	//std::getline(*fi, line);
	//std::istringstream iss(line);

    if (fi->eof())
        return DV_EOF;
    for ((*idt)=0; (*idt)<lnfrmt.size(); (*idt)++)
    {
        try {
        switch (lnfrmt[*idt])
        {
        case 'I':
            *fi >> p->id;
            break;
        case 'X':
            *fi >> p->x;
            break;
        case 'Y':
            *fi >> p->y;
            break;
        case 'Z':
            *fi >> p->z;
            break;
        case 'L':
            if (circopt.sourceOptions.angularUnit == "DMS")
            {
                *fi >> foo;
                p->l = readDMS(foo, "longitude DMS", true);
            }
            else if (circopt.sourceOptions.angularUnit == "DM")
            {
                *fi >> foo;
                p->l = readDMS(foo, "longitude DM", true);
            }
            else
                *fi >> p->l;
            break;
        case 'P':
            if (circopt.sourceOptions.angularUnit == "DMS")
            {
                *fi >> foo;
                p->p = readDMS(foo, "latitude DMS", true);
            }
            else if (circopt.sourceOptions.angularUnit == "DM")
            {
                *fi >> foo;
                p->p = readDMS(foo, "latitude DM", true);
            }
            else
                *fi >> p->p;
            break;
        case 'H':
            *fi >> p->h;
            break;
        case 'E':
            *fi >> p->e;
            break;
        case 'N':
            *fi >> p->n;
            break;
        case 'V':
            *fi >> p->w;
            break;
        default:
            *fi >> foo; //use any other character to skip a field
            break;
        }
        } catch (std::string str) {
            fi->clear();
            fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return DV_BAD_COORD;
        }
        if (fi->fail())
		{
			fi->clear();
			fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return DV_BAD_FORMAT;
		}
    }
    //skip other fields if any
    fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//            char next;
//            while(fi->get(next))
//            {
//                if (next == '\n')  // If the file has been opened in
//                {
//                    break;        // text mode then it will correctly decode the
//                }                  // platform specific EOL marker into '\n'
//            }
    return DV_OK;
}

/**
* \brief Reads a single point data in a std::ifstream.
* \param[in] lnfrmt: data format.
* \param fi: A pointer to a std::ifstream.
* \param[out] p: A pointer to a pt4d instance.
* \param[out] fo: A pointer to the int position in the data format where an error occured (if any).
* \return A DATA_VALIDATION code to be converted to an error message (if any).
*/
//not much format validation to save performance (but other domain validation in Operate)
DATA_VALIDATION readBasicLine2(std::ifstream *fi, const circeOptions &circopt,
                              const std::string& lnfrmt,  pt4d *p, unsigned int *idt)
{
    std::string foo;
    p->e=p->n=p->l=p->p=p->h=p->x=p->y=p->z=p->w=0.;
    p->id = p->info = "";
    char fp = fi->peek();
    if (fp == '*' || fp == '\r' || fp == '\n')
    {
        fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return DV_COMMENT;
    }

    if (fi->eof())
        return DV_EOF;

    char sep = getSeparator(circopt.separator);
    //qDebug("::%c::",sep);
    std::string val;
    std::string line;
    std::getline(*fi, line);
    std::istringstream iline(line);

    try {
        for ((*idt)=0; (*idt)<lnfrmt.size(); (*idt)++)
        {
            std::getline(iline, val, sep);
            //qDebug(":%s:",val.data());
            if (val.length()==0 && sep==' ')
            {
                (*idt)--;
                continue;
            }
            //qDebug("%s",val.data());
            switch (lnfrmt[*idt])
            {
            case 'I':
                p->id = val;
                break;
            case 'X':
                p->x = std::stod(val);
                break;
            case 'Y':
                p->y = std::stod(val);
                break;
            case 'Z':
                p->z = std::stod(val);
                break;
            case 'L':
                if (circopt.sourceOptions.angularUnit == "DMS")
                    p->l = readDMS(val, "longitude DMS", true);
                else if (circopt.sourceOptions.angularUnit == "DM")
                    p->l = readDMS(val, "longitude DM", true);
                else
                    p->l = std::stod(val);
                break;
            case 'P':
                if (circopt.sourceOptions.angularUnit == "DMS")
                    p->p = readDMS(val, "latitude DMS", true);
                else if (circopt.sourceOptions.angularUnit == "DM")
                    p->p = readDMS(val, "latitude DM", true);
                else
                    p->p = std::stod(val);
                break;
            case 'H':
                p->h = std::stod(val);
                break;
            case 'E':
                p->e = std::stod(val);
                break;
            case 'N':
                p->n = std::stod(val);
                break;
            case 'V':
                p->w = std::stod(val);
                break;
            default:
                foo = val; //use any other character to skip a field
                break;
            }
        }

    } catch (...) {
        return DV_BAD_FORMAT;
    }
    return DV_OK;
}

/**
* \brief Writes a header in a std::ofstream.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ofstream.
*/
//setw is not sticky.
void writeBasicHeader(std::ostream *fo, const circeOptions &circopt, const std::string& str, bool writePrec)
{
    int idt;
	*fo << str;
    *fo << "*!\n*!";
    for (idt=0; idt<circopt.targetOptions.formatId.size(); idt++)
    {
        switch (circopt.targetOptions.formatId[idt])
        {
        case 'I':
            *fo << "      id      ";
            break;
        case 'X':
            *fo << "      X       ";
            break;
        case 'Y':
			*fo << "      Y       ";
            break;
        case 'Z':
			*fo << "      Z       ";
            break;
        case 'L':
            *fo << "   Longitude   ";
            break;
        case 'P':
            *fo << "   Latitude    " ;
            break;
        case 'H':
            *fo << "    he    ";
            break;
        case 'E':
            *fo << "     E      ";
            break;
        case 'N':
			*fo << "     N      ";
            break;
        case 'V':
            *fo << "    H     ";
            break;
        case 'C':
            *fo << " " << InterfaceElementText[IE_CONVERGENCE];
            break;
        case 'S':
            *fo << " " << InterfaceElementText[IE_SCALE_FACTOR];
            break;
        case 'D':
            *fo << " " << InterfaceElementText[IE_DEFLECTION] << " (eta xi \")";
            break;
        default:
            break;
        }
    }
	if (circopt.targetOptions.verticalCRSid == "AUTHORITATIVE")
		*fo << "  " << std::setw(32) << std::left << " [ vert.info. ] ";
    if (writePrec)
    {
        *fo << std::setw(12) << "geod.prec.";
        *fo << std::setw(12) << "vert.prec.";
    }
	*fo << std::endl;
    return;
}

/**
* \brief Writes a header in a std::ofstream.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ofstream.
*/
//setw is not sticky.
void writeBasicHeader2(std::ostream *fo, const circeOptions &circopt, const std::string& str, int TransfoRequired)
{
    int idt;
    *fo << str;
    *fo << "*!\n*!";
    for (idt=0; idt<circopt.targetOptions.formatId.size(); idt++)
    {
        switch (circopt.targetOptions.formatId[idt])
        {
        case 'I':
            *fo << "id; ";
            break;
        case 'X':
            *fo << "X; ";
            break;
        case 'Y':
            *fo << "Y; ";
            break;
        case 'Z':
            *fo << "Z; ";
            break;
        case 'L':
            *fo << "Longitude; ";
            break;
        case 'P':
            *fo << "Latitude; ";
            break;
        case 'H':
            *fo << "he; ";
            break;
        case 'E':
            *fo << "E; ";
            break;
        case 'N':
            *fo << "N; ";
            break;
        case 'V':
            *fo << "H; ";
            break;
        case 'C':
            *fo << " " << InterfaceElementText[IE_CONVERGENCE] << ";";
            break;
        case 'S':
            if (circopt.noPrintSppm)
                *fo << " " << InterfaceElementText[IE_SCALE_FACTOR] << ";";
            else
                *fo << " " << InterfaceElementText[IE_SCALE_ERROR] << ";";
            break;
        case 'D':
            if (TransfoRequired&T_VERTICAL_REQUIRED)
                *fo << " " << InterfaceElementText[IE_DEFLECTION] << " eta (\"); " << InterfaceElementText[IE_DEFLECTION] << " xi (\"); ";
            break;
        default:
            break;
        }
    }
    if (circopt.targetOptions.verticalCRSid == "AUTHORITATIVE")
        *fo << "[ vert.info. ]; ";
    if (TransfoRequired&T_GEODETIC_REQUIRED)
        *fo << std::setw(12) << "geod.prec.; ";
    if (TransfoRequired&T_VERTICAL_REQUIRED)
        *fo << std::setw(12) << "vert.prec.; ";
    *fo << std::endl;
    return;
}

/**
* \brief Writes a single point data in a std::ostream.
* \param[in] lnfrmt: data format.
* \param fo: A pointer to a std::ostream.
* \param[in] p: A pointer to a pt4d instance.
* \param[out] idt: A pointer to the int position in the data format where an error occured (if any).
* \throw a std:string containing the error message (if any).
*/
//setw is not sticky.
void writeBasicLine(std::ostream *fo, const circeOptions &circopt,
                                const pt4d& p, unsigned int *idt, std::string lnfrmt)
{
    //std::string lnfrmt = circopt.targetOptions.formatId;
    bool js = ((circopt.outputFormat=="JSON") && (circopt.sourceOptions.pathname == ""));
    for ((*idt)=0; (*idt)<lnfrmt.size(); (*idt)++)
    {
        if (js)
        {
            if ((*idt)>0) *fo << ",\n";
            *fo << "\"" << lnfrmt[*idt] << "\":\"";
        }
        switch (lnfrmt[*idt])
        {
        case 'I':
			if (p.id != "")
            {
                if (!js) *fo << std::setw(15);
                *fo << std::left;
                *fo << p.id;
                if (!js) *fo << " ";
            }
            break;
        case 'X':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(p.frmt.n_m + 10/*13*/);
            *fo << std::right;
            *fo << p.x;
            break;
        case 'Y':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(p.frmt.n_m + 10/*13*/);
            *fo << std::right;
            *fo << p.y;
            break;
        case 'Z':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(p.frmt.n_m + 10/*13*/);
            *fo << std::right;
            *fo << p.z;
            break;
//        case 'L':
//            *fo << std::setprecision(p.frmt.n_ang/*9*/) << std::setw(p.frmt.n_ang+6/*15*/) << std::right << p.l;
//            break;
//        case 'P':
//            *fo << std::setprecision(p.frmt.n_ang/*9*/) << std::setw(p.frmt.n_ang + 6/*15*/) << std::right << p.p ;
//            break;
        case 'L':
            if (circopt.targetOptions.angularUnit == "DMS")
            {
                if (!js) *fo << std::setw(20);
                *fo << std::right;
                *fo << writeDMS(p.l, "lon", p.frmt.n_ang, circopt.plainDMS);
            }
            else if (circopt.targetOptions.angularUnit == "DM")
            {
                if (!js) *fo << std::setw(20);
                *fo << std::right;
                *fo << writeDM(p.l, "lon", p.frmt.n_ang, circopt.plainDMS);
            }
            else
            {
                if (!js) *fo << std::setw(/*p.frmt.n_ang+6*/16);
                *fo << std::right;
                *fo << std::setprecision(p.frmt.n_ang/*9*/) << p.l;
            }
            break;
        case 'P':
            if (circopt.targetOptions.angularUnit == "DMS")
            {
                if (!js) *fo << std::setw(19);
                *fo << std::right;
                *fo << writeDMS(p.p, "lat", p.frmt.n_ang, circopt.plainDMS);
            }
            else if (circopt.targetOptions.angularUnit == "DM")
            {
                if (!js) *fo << std::setw(19);
                *fo << std::right;
                *fo << writeDM(p.p, "lat", p.frmt.n_ang, circopt.plainDMS);
            }
            else
            {
                if (!js) *fo << std::setw(/*p.frmt.n_ang+6*/15);
                *fo << std::right;
                *fo << std::setprecision(p.frmt.n_ang/*9*/);
                *fo << p.p ;
            }
            break;
        case 'H':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(p.frmt.n_m + 7/*10*/);
            *fo << std::right;
            *fo << p.h;
            break;
        case 'E':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(13);
            *fo << std::right;
            *fo << p.e;
            break;
        case 'N':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(13);
            *fo << std::right;
            *fo << p.n;
            break;
        case 'V':
            *fo << std::setprecision(p.frmt.n_m/*3*/);
            if (!js) *fo << std::setw(10);
            *fo << std::right;
            *fo << p.w;
            break;
        case 'C':
            *fo << std::setprecision(6);
            if (!js) *fo << std::setw(13);
            *fo << std::right;
            *fo << p.conv;
            break;
        case 'S':
            if (!js) *fo << std::setw(10);
            *fo << std::right;
            if (circopt.noPrintSppm)
               *fo << std::setprecision(7) << 1+1e-6*p.scfact;
            else
                *fo << std::setprecision(1) << p.scfact;
            break;
        case 'D':
            //if (TransfoRequired&T_VERTICAL_REQUIRED)
                if (p.transfoIndex&T_VERTICAL_REQUIRED)
            {
                if (!js) *fo << std::setw(6);
                *fo << std::right;
                if (fabs(p.eta)<1e-2 || fabs(p.eta)<1e-2)
                    *fo << std::scientific;
                *fo << std::setprecision(1) << std::setw(6) << p.eta  << " ";
                *fo << std::setprecision(1) << std::setw(6) << p.xi;
            }
            break;
        default:
            break;
        }
        if (js) *fo << "\"";
        if (fo->fail())
            throw(DataValidationText[DV_BAD_FORMAT]);
        //*fo << " ";
    }
    if (circopt.targetOptions.verticalCRSid == "AUTHORITATIVE"/* && p.info != ""*/)
    {
        if (js)
            *fo << ",\n\"vertical_info\":\"" << p.info << "\"";
        else
            *fo << "  " << std::setw(32) << std::left << " [ "+p.info+" ] ";
    }
    if (p.transfoIndex&T_GEODETIC_REQUIRED)
    {
		//if (p.Gprec != 0)
        if (js) *fo << ",\n\"geodetic_precision\":\"";
        if (!js) *fo << std::setw(12);
        *fo << precisionCode2Text(p.Gprec);
    }
    if (p.transfoIndex&T_VERTICAL_REQUIRED)
    {
        //if (p.Vprec != 0)
        if (js) *fo << "\",\n\"vertical_precision\":\"";
        if (!js) *fo << std::setw(12);
        *fo << precisionCode2Text(p.Vprec);
        if (js) *fo << "\"";
    }
	//    *fo << std::endl;
    return;
}


/**
* \brief Initializes pt4d member values from a tinyxml2 element.
* \param[in] XMLelem: A tinyxml2::XMLElement* stemming from a KML file.
* \param[out] p: A pointer to a pt4d instance.
* \throw a std:string containing the error message (if any).
*/
void readKMLline(tinyxml2::XMLElement *XMLelem, pt4d *p)
{
    std::istringstream ss;
    char coco[_MAX_PATH_], seps[]=",", *token;
    tinyxml2::XMLElement *XMLelem2;
    p->id = ReadString(XMLelem,"name");
	p->info = "";
    XMLelem2 = XMLelem->FirstChildElement("Point");
    if (!XMLelem2)
        throw DataValidationText[DV_TAG_NOT_FOUND];
    XMLelem2 = XMLelem2->FirstChildElement("coordinates");
    if (!XMLelem2)
        throw DataValidationText[DV_TAG_NOT_FOUND];
    //Points only
    if (XMLelem2 && XMLelem2->GetText())
    {
        strcpy(coco,XMLelem2->GetText());
        if ( (token = strtok(coco, seps)) != NULL)
            sscanf(token,"%lf",&(p->l));
        else
            throw DataValidationText[DV_BAD_FORMAT];
        if ( (token = strtok(NULL, seps)) != NULL)
            sscanf(token,"%lf",&(p->p));
        else
            throw DataValidationText[DV_BAD_FORMAT];
        if ( (token = strtok(NULL, seps)) != NULL)
            sscanf(token,"%lf",&(p->h));
        else
            throw DataValidationText[DV_BAD_FORMAT];
        /*
        ss.str(XMLelem2->GetText());
        if (!ss.eof()) getline(ss, circopt->targetGeodeticCRSid, ',');
        */
    }
    return;
}

/**
* \brief Writes a header if a KML file.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ofstream.
*/
void writeKMLheader(std::ostream *fo, const circeOptions& circopt)
{
    *fo << "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n\
<kml> \n\
<Document> \n\
<name> from circe " << circopt.sourceOptions.pathname << "</name>  \n\
<StyleMap id=\"circe_point\"> \n\
<Pair> \n\
    <key>normal</key> \n\
    <styleUrl>#circe_point_normal</styleUrl> \n\
</Pair> \n\
<Pair> \n\
    <key>highlight</key> \n\
    <styleUrl>#circe_point_highlight</styleUrl> \n\
</Pair> \n\
</StyleMap> \n\
<Style id=\"circe_point_highlight\"> \n\
<IconStyle> \n\
    <color>ff00ffff</color> \n\
    <scale>1.1</scale> \n\
    <Icon> \n\
        <href>http://maps.google.com/mapfiles/kml/paddle/red-circle.png</href> \n\
    </Icon> \n\
</IconStyle> \n\
<LabelStyle> \n\
    <color>ffffffff</color> \n\
    <scale>0.8</scale> \n\
</LabelStyle> \n\
</Style> \n\
<Style id=\"circe_point_normal\"> \n\
<IconStyle> \n\
    <color>ff00ffff</color> \n\
    <scale>0.8</scale> \n\
    <Icon> \n\
        <href>http://maps.google.com/mapfiles/kml/paddle/red-circle.png</href> \n\
    </Icon> \n\
</IconStyle> \n\
<LabelStyle> \n\
    <color>ffffffff</color> \n\
    <scale>0.9</scale> \n\
</LabelStyle> \n\
</Style> \n";

}

/**
* \brief Writes a single point data in a KML file.
* \param fo: A pointer to a std::ofstream.
* \param[in] p: A pt4d instance.
*/
/*<description> for Qgis*/
void writeKMLline(std::ostream *fo, const pt4d& p)
{
    *fo <<"	<Placemark>\n\
        <name>" << p.id << "</name> \n\
        <styleUrl>#circe_point</styleUrl> \n\
            <description>" << asString(p.h,13) << "</description> \n\
        <Point> \n\
            <coordinates>" << asString(p.l,13) << "," << asString(p.p,13) << "," << asString(p.h,13) << "</coordinates> \n\
        </Point> \n	</Placemark> \n";
}

/**
* \brief Writes a tail if a KML file.
* \param fo: A pointer to a std::ofstream.
*/
void writeKMLtail(std::ostream *fo)
{
    *fo << "\
</Document> \n\
</kml> \n";
}


/**
* \brief Initializes pt4d member values from a tinyxml2 element.
* \param[in] XMLelem: A tinyxml2::XMLElement* stemming from a GPX file.
* \param[out] p: A pointer to a pt4d instance.
*/
void readGPXline(tinyxml2::XMLElement *XMLelem, pt4d *p)
{
    std::string foo;
    p->id = ReadString(XMLelem,"name");
	p->info = "";
    p->l = ReadDouble(XMLelem,"lon",&foo);
    p->p = ReadDouble(XMLelem,"lat",&foo);
    p->h = ReadDouble(XMLelem,"ele",&foo);
    return;
}

/**
* \brief Writes a header if a GPX file.
* \param[in] circopt: processing options in a circeOptions structure.
* \param fo: A pointer to a std::ostream.
*/
void writeGPXheader(std::ostream *fo, const circeOptions& circopt)
{
    *fo << "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n\
<gpx> \n";
}

/**
* \brief Writes a single point data in a GPX file.
* \param fo: A pointer to a std::ofstream.
* \param[in] p: A pt4d instance.
*/
void writeGPXline(std::ostream *fo, const pt4d& p)
{
    *fo << "<wpt lat=\"" << asString(p.p,13) << "\" lon=\"" << asString(p.l,13) << "\">";
    *fo << "<ele>" << asString(p.h,4) << "</ele><name>" << p.id << "</name></wpt>\n";

}

/**
* \brief Writes a tail if a GPX file.
* \param fo: A pointer to a std::ofstream.
*/
void writeGPXtail(std::ostream *fo)
{
    *fo << "\
</gpx> \n";
}

/**
* \brief Reads a single point data in a NMEA file.
* \param fi: A pointer to a std::ifstream.
* \param[out] p: A pointer to a pt4d instance.
* \param[out] fo: A pointer to the int position in the data format where an error occured (if any).
* \return A DATA_VALIDATION code to be converted to an error message (if any).
*/
/*
       $GPGGA,094329,4850.6673414,N,00225.5016900,E,1,18,2.0,126.737,M,0.0,M,,*78
       $GPGGA       : Type de trame
       064036.289   : Trame envoyée à 06h40m36,289s (heure UTC)
       4836.5375,N  : Latitude 48,608958° Nord = 48°36'32.25" Nord
       00740.9373,E : Longitude 7,682288° Est = 7°40'56.238" Est
       1            : Type de positionnement (le 1 est un positionnement GPS)
       04           : Nombre de satellites utilisés pour calculer les coordonnées
       3.2          : Précision horizontale ou HDOP (Horizontal dilution of precision)
       200.2,M      : Altitude 200,2, en mètres
       ,,,,,0000    : D'autres informations peuvent être inscrites dans ces champs
       *0E          : Somme de contrôle de parité, un simple XOR sur les caractères précédents
*/
/*
 * Coordinates in DM (Degrees Minutes)
 */
/*
DATA_VALIDATION readNMEAline(std::ifstream *fi, pt4d *p, unsigned int *idt)
{
    std::string field;
    *idt = 1;
    if (!(getline(*fi,field,','))) return DV_EOF;
    if (field!="$GPGGA")
    {
        fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return DV_COMMENT;
    }
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    p->id = field; //date
	p->info = "";
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    p->p = atof(field.data())/100.; //DMS
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    if (field == "S") p->p *= -1.;
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    p->l = atof(field.data())/100.; //DMS
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    if (field == "W") p->l *= -1.;
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    (*idt)++;
    if (!(getline(*fi,field,','))) return DV_BAD_FORMAT;
    p->w = atof(field.data());
    //skip other fields
    fi->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return DV_OK;
}
*/
/*
       * #include <stdio.h>
       #include <string.h>

       int checksum(char *s) {
           int c = 0;

           while(*s)
               c ^= *s++;

           return c;
       }

       int main()
       {
           char mystring[] = "GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A";

           printf("String: %s\nChecksum: 0x%02X\n", mystring, checksum(mystring));

           return 0;
       }*/

/*
 * Coordinates in DM (Degrees Minutes)
 */
/*
void writeNMEAline(std::ostream *fo, const pt4d& p)
{
//    double pl = fabs(100.*UnitConvert(p.l, angularUnit, MU_DMS));
//    double pp = fabs(100.*UnitConvert(p.p, angularUnit, MU_DMS));
    double pl = fabs(100.*p.l);
    double pp = fabs(100.*p.p);
    std::string EW = (p.l>=0?"E":"W");
    std::string NS = (p.p>=0?"N":"S");
    *fo << "$GPGGA," << p.id << ",";
    *fo << std::setprecision(4) << std::fixed << std::setfill('0') << std::setw(9) << pp << "," << NS << ",";
    *fo << std::setprecision(4) << std::fixed << std::setfill('0') << std::setw(10) << pl << "," << EW << ",,,," << asString(p.h) << ",,,,,\n" ;

}
*/

}
