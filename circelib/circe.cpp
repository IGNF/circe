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
* \file circe.cpp
* \brief Implementation of main Circe API, high-level and convenience functions.
*/
#include <iostream>
#include <vector>
#include <iomanip> //setprecision
#include <limits> //numeric_limits
#include <sys/stat.h>
/*#ifdef UNIX
#include <unistd.h> //getcwd,chdir
#else
#include <direct.h>
#endif*/
#include "circe.h"
#include "parser.h"
#include "metadata.h"
#include "files.h"
#include "pt4d.h"
#ifdef WITH_GDAL
#include "../dependances/gdal/circe_ogr.h"
#endif
#include "tinyxml2/tinyxml2.h"
#include "conversioncore.h"
#include <time.h>  //difftime, time
#ifdef UNIX
#include <dirent.h> //InitZoneList()
#elif defined WIN32
#define NOMINMAX //to avoid collision with std::numeric_limits<std::streamsize>::max()
#include <windows.h> //InitZoneList()
#endif

/**\brief The purpose of namespace circe is the application of operations (conversions and transformations) on sets of single points.*/
namespace circe
{

/**
* \brief Sets some fixed options according to file format.
* \param *go: a pointer to a geodeticOptions instance.
* \param[out]: boolean, true if the geodeticOptions instance was modified.
*/
bool setFormatDefaults(geodeticOptions *go, GeodeticSet* GS)
{
    if ((go->subformatId==FF_BASIC || go->subformatId==FF_NMEA_POINT) && go->formatId=="NMEA")
    {
        go->angularUnit = "DM"; //to be implemented !
        go->geodeticCRStype = CT_GEOGRAPHIC;
        go->geodeticCRSid = ((GeodeticReferenceFrame*)(GS->getCRS(go->geodeticCRSid)->getReferenceFrame()))->getGeographicCRSid();
        return true;
    }
    else if ((go->subformatId==FF_BASIC || go->subformatId==FF_KML_POINT) && go->formatId=="KML")
    {
        go->angularUnit = "DEGREES";
        go->geodeticCRStype = CT_GEOGRAPHIC;
        go->geodeticCRSid = ((GeodeticReferenceFrame*)(GS->getCRS(go->geodeticCRSid)->getReferenceFrame()))->getGeographicCRSid();
        return true;
    }
    return false;
}

/**
* \brief Calculates a duration.
* \param[in] circopt: circopt.verbose used (nothing displayed if false).
* \param[in] timer_beginning: a time_t for the time of beginning.
* \return a std::string containing the difference between current time and time of beginning, in seconds.
*/
std::string displayDuration(const circeOptions& circopt, time_t timer_beginning)
{
    if (!circopt.verbose) return "";
    time_t timer;
    time(&timer);
    double seconds = difftime(timer,timer_beginning);
    if (seconds==0)
        return "Duration: <1s\n";
    else
        return "Duration: "+asString(seconds,0)+" s\n";
}

/**
* \brief Messages displayed at the end of a process.
* \param[in] fo: the stream to write to.
* \param[in] circopt: circopt.verbose used (nothing displayed if false).
* \param[in] timer_beginning: a time_t for the time of beginning.
* \param[in] nl: line number (relevant for file processing).
*/
void endProcess(std::ofstream* fo, const circeOptions& circopt, time_t timer, unsigned int nl)
{
    if (!circopt.verbose) return;
    writeLog(fo, "\n"+asString(nl)+" completed"+"\n");
    writeLog(fo, displayDuration(circopt, timer)+"\n");
}


/**
* \brief Writes Vertical Frame name and grid name information, usefull when processing AUTHORITATIVE option.
*/
void writeVerticalInfo(GeodeticSet* GS, pt4d *p)
{
    if (GS->getVerticalReferenceFrameName(p->info) != "" || getFilename(p->info2) != "")
        p->info = GS->getVerticalReferenceFrameName(p->info)+" via "+getFilename(p->info2);
}

/*
\-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=EN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.xml" \-\-logPathname="circe.log"  470000 160000
\-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=EN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.xml" \-\-logPathname="circe.log" 470000 160000
\-\-sourceCRS=EPSG:320002103. \-\-sourceFormat=EN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.xml" \-\-logPathname="circe.log"  470000 160000
 */

/**
* \brief Processes a single pt4d.
* \param[in] circopt: processing options in a circeOptions structure. The pt4d is one of them.
* \return The output is written to the screen, with more text in verbose mode.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
void processPt4d(circeOptions circopt)
{
	return processPt4d(circopt, NULL);
}

void processPt4d(circeOptions circopt, GeodeticSet* GS)
{
    OPERATION_MESSAGE om;
    unsigned int idt=0;
    std::ofstream fo;
    std::ostringstream oss;
    bool js = (circopt.outputFormat=="JSON");
    openLog(&fo, circopt.logPathname);
    try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if (CO.getTransfoRequired())
            circopt.operationType = "transformation";
        else
            circopt.operationType = "conversion";
        if (js) oss << "{\n";
        oss << displayOptionsJSON(circopt);
        if ((om = CO.Operate(&(circopt.Pt4d))) != OM_OK)
            throw (OperationMessageText[om]);
        //Result in circopt.Pt4d
        writeVerticalInfo(GS,&(circopt.Pt4d));
        initBasicOstream(&oss);
        if (js) oss << "\"output\":{";
        //writeOutputLine(&oss, circopt, circopt.Pt4d, &idt, true);
        writeOutputLine(&oss, circopt, circopt.Pt4d, &idt);

		if (circopt.followUpTransfos)
        {
            if (js) oss << ",\n\"followUpTransfos\":\"";
            oss << CO.followUpTransfos();
            if (js) oss << "\"";
        }
        if (js) oss << "}";

        if (js) oss << "\n}";
        std::cout << oss.str();
	}
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
}

/**
* \brief Processes a single pt4d.
* \param[in] circopt: processing options in a circeOptions structure. The pt4d is one of them.
* \return The output pt4d.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
circe::pt4d process_pt4d(circeOptions circopt)
{
	return process_pt4d(circopt, NULL);
}

circe::pt4d process_pt4d(circeOptions circopt, GeodeticSet* GS)
{
    OPERATION_MESSAGE om;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if ((om = CO.Operate(&(circopt.Pt4d))) != OM_OK)
            throw std::string(OperationMessageText[om]);
        writeVerticalInfo(GS,&(circopt.Pt4d));
        circopt.Pt4d.info2 = CO.getDescription();
	}
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    return circopt.Pt4d;
}


/**
* \brief Processes a vector of pt4d.
* \param[in] circopt: Processing options in a circeOptions structure.
* \param[in] vPt4d: A pointer to the vector of pt4d, which is also output.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
void process_vector_pt4d(circeOptions circopt, std::vector<circe::pt4d>* vPt4d)
{
	process_vector_pt4d(circopt, vPt4d, NULL);
}

void process_vector_pt4d(circeOptions circopt, std::vector<circe::pt4d>* vPt4d, GeodeticSet* GS)
{
    OPERATION_MESSAGE om;
    std::vector<circe::pt4d>::iterator itPt4d;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if (CO.getoperationMessage() != OM_OK)
        {
            throw (OperationMessageText[CO.getoperationMessage()]);
        }
        for(itPt4d=vPt4d->begin();itPt4d<vPt4d->end();itPt4d++)
        {
            if ((om = CO.Operate(&(*itPt4d))) != OM_OK)
                throw std::string(OperationMessageText[om]);
            writeVerticalInfo(GS,&(*itPt4d));
		}
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
}

/**
* \brief Processes a vector of double precision reals.
* \param[in] circopt: Processing options in a circeOptions structure.
* \param[in] points: The vector of double in which 4 back-to-back values represent the coordinates of one point. It is also output.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
void process_double_vector(circeOptions circopt, std::vector<double>& points)
{
	return process_double_vector(circopt, points, NULL);
}

void process_double_vector(circeOptions circopt, std::vector<double>& points, GeodeticSet* GS)
{
    OPERATION_MESSAGE om;
    //std::vector<double>::iterator itPt;
	int nbPts = points.size() / 4;
    pt4d p;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(p.frmt));
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if (CO.getoperationMessage() != OM_OK)
        {
            throw (OperationMessageText[CO.getoperationMessage()]);
        }
        //for(itPt=points.begin();itPt<points.end();itPt++)
		for (int iPt = 0; iPt<nbPts; iPt++)
        {
            p.CRStypeInit=CO.getCurrentCRStype();
			//InitPoint(&p, *itPt, *(itPt + 1), *(itPt + 2), *(itPt + 3));
			InitPoint(&p, points[4 * iPt + 0], points[4 * iPt + 1], points[4 * iPt + 2], points[4 * iPt + 3]);
			if ((om = CO.Operate(&p)) != OM_OK)
                throw std::string(OperationMessageText[om]);
            writeVerticalInfo(GS,&p);
			copyPt4d(p, &points[4 * iPt + 0], &points[4 * iPt + 1], &points[4 * iPt + 2], &points[4 * iPt + 3]);
            //itPt+=4;
        }
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
}

/**
* \brief Processes an C-array of double precision reals.
* \param[in] circopt: Processing options in a circeOptions structure.
* \param[in] points: The array of double in which 4 back-to-back values represent the coordinates of one point. It is also output.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
void process_double_array(circeOptions circopt, double* points)
{
	process_double_array(circopt, points, NULL);
}

void process_double_array(circeOptions circopt, double* points, GeodeticSet* GS)
{
    OPERATION_MESSAGE om;
    pt4d p;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(p.frmt));
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if (CO.getoperationMessage() != OM_OK)
        {
            throw (OperationMessageText[CO.getoperationMessage()]);
        }
        for (int i=0;i<sizeof(points)/sizeof(*points);i++)
        {
            p.CRStypeInit=CO.getCurrentCRStype();
            InitPoint(&p,points[i],points[i+1],points[i+2],points[i+3]);
            if ((om = CO.Operate(&p)) != OM_OK)
                throw std::string(OperationMessageText[om]);
            writeVerticalInfo(GS,&p);
            copyPt4d(p,&points[i],&points[i+1],&points[i+2],&points[i+3]);
        }
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
}

/**
* \brief Processes basic file.
* \param[in] circopt: Processing options in a circeOptions structure.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string processIFStream(const circeOptions& circopt)
{
    return processIFStream(circopt, NULL, NULL, NULL);
}

std::string processIFStream(const circeOptions& circopt, GeodeticSet* GS, swain *sw)
{
    return processIFStream(circopt, NULL, GS, sw);
}

/**
* \brief Processes basic file.
* \param[in] circopt: Processing options in a circeOptions structure.
* \param[out] vPt4d: The vector of pt4d to write the result to.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
* \remark This function is reachable using these kind of options:
- \-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=EN.METERS. \-\-targetCRS=RGF93G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.xml" \-\-logPathname="circe.log" \-\-sourcePathname="./datafile.txt" \-\-targetPathname="./outfile.txt"
- \-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=IEN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=ILP.METERS.DEGREES \-\-metadataFile="data/IGNF.xml" \-\-sourcePathname="./datafile.txt" \-\-targetPathname="./outfile_IGNF.txt"
*/
std::string processIFStream(const circeOptions& circopt, std::vector<circe::pt4d>* vPt4d,
                            GeodeticSet* GS, swain *swn)
{
    pt4d p;
    OPERATION_MESSAGE om;
    time_t timer;
    DATA_VALIDATION dv;
    unsigned int idt, nl=0, ns=0;
    std::ifstream *fi;
    std::ofstream *ofs;
    std::ostringstream oss, oss_fut;
    bool bProcessVector = (vPt4d!=NULL && circopt.processAsVector);
    bool loadAsVector = (vPt4d!=NULL && circopt.loadAsVector);
    std::ofstream fo;
    std::string errMess, ret;
	openLog(&fo, circopt.logPathname);
	try
    {
        time(&timer);
		initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(p.frmt));

        // Open input file
        if (swn == NULL || swn->fi == NULL)
        {
            fi = new std::ifstream();
            fi->open(circopt.sourceOptions.pathname.data(),std::ifstream::in);
        }
        else
            fi = swn->fi;
    //    if (!fi.good())
    //    {
    //        fi.close();
    //        return;
    //    }
        if (!(fi->is_open()))
            throw ErrorMessageText[EM_CANNOT_OPEN]+circopt.sourceOptions.pathname;

        // Load metadata
        if (GS == NULL)
        {
            GS = new GeodeticSet();
            LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
        }
//        const CatOperation CO(circopt, GS);
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);
        if (CO.getoperationMessage() != OM_OK && !loadAsVector)
        {
            //*ofs << OperationMessageText[CO.getoperationMessage()] << std::endl;
            throw (OperationMessageText[CO.getoperationMessage()]);
        }
        //std::stringstream strStream;
		//strStream << fi.rdbuf();
		//std::string str = strStream.str();
		//std::istringstream iss(str);

		// Prepare output
        if (bProcessVector || loadAsVector)
		{
			vPt4d->clear();
		}
		else
		{
			initBasicOstream(&oss);
            if (circopt.followUpTransfos)
                initBasicOstream(&oss_fut);
            if (swn == NULL || swn->fo == NULL)
                ofs = new std::ofstream();
            else
                ofs = swn->fo;
            openOutputFile(ofs, circopt, CO.displayDescription(circopt,"all"), CO.getTransfoRequired());
        }

        // Process input data
        while (!fi->eof())
        {
            // Read
            //if ((++nl)%1000==0) std::cout << "\r"+asString(nl);
//            if ((++nl) % 100000 == 0) std::cout << asString(nl) + ": " + displayDuration(circopt, timer) + "\n";
            ++nl;
            switch (circopt.sourceOptions.subformatId)
            {
            case FF_BASIC:
                if (getSeparator(circopt.separator)==' ')
                    dv = readBasicLine(fi, circopt, circopt.sourceOptions.formatId, &p, &idt);
                else
                    dv = readBasicLine2(fi, circopt, circopt.sourceOptions.formatId, &p, &idt);
                break;
            case FF_NMEA_POINT:
                //dv = readNMEAline(&fi, &p, &idt);
                break;
            default:
                break;
            }
            if (hasIdentifier(circopt.targetOptions.formatId))
                errMess = "*"+p.id+" ";
            else
                errMess = "*";
            if (dv == DV_EOF)
            {
                nl--;
                break;
            }
            if (dv == DV_COMMENT)
            {
                nl--;
                continue;
            }
            if (dv != DV_OK)
            {
                //errMess = std::string("::") + circopt.sourceOptions.pathname + std::string("::") + asString(nl) + std::string("::") + asString(idt + 1) + "::" + circe::DataValidationText[dv] + "::\n";
                //writeLog(&fo, errMess);
                //writeLog(&fo, displayDuration(circopt, timer)+"\n");
                errMess += asString(nl) + ":" + asString(idt + 1) + ":" + circe::DataValidationText[dv] + "\n";
                oss << errMess;
				//ofs << oss.str();
				//closeOutputFile(&ofs, circopt);
				//return;
			}
            else if (bProcessVector || loadAsVector)
            {
                vPt4d->push_back(p);
            }
            else
            {
                // Operate single point
                om = CO.Operate(&p);
                writeVerticalInfo(GS,&p);
                // Output
                if (om != OM_OK)
                    oss << errMess << " " << OperationMessageText[om] << std::endl;
                else
                {
                    //writeOutputLine(&oss, circopt, p, &idt, true);
                    writeOutputLine(&oss, circopt, p, &idt);
                    ns++;
                    if (circopt.followUpTransfos)
                    {
                        //writeOutputLine(&oss_fut, circopt, p, &idt, true);
                        oss_fut << CO.followUpTransfos() << std::endl;
                    }
                }
            }
            if (nl%1000==0)
            {
                if (GS->mb!=NULL)
                    GS->mb->messageEdit(asString(nl));
                else
                    std::cout << "\r"+asString(nl);
            }
        }
        fi->close();
        if (bProcessVector)
        {
            process_vector_pt4d(circopt,vPt4d);
            writeVectorPt4d(circopt, *vPt4d);
        }
        else if (!loadAsVector)
        {
            *ofs << oss.str();
            closeOutputFile(ofs,circopt);
            if (circopt.followUpTransfos)
            {
                std::ofstream ofs_fut;
                ofs_fut.open((circopt.targetOptions.pathname+"_transfos").data(),std::ofstream::out);
                ofs_fut << oss_fut.str();
                ofs_fut.close();
            }
        }
        endProcess(&fo, circopt, timer, nl);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    if (GS->mb==NULL)
        std::cout << "\r" << std::setw(asString(nl).length()) << " " << "\r";
    return displayOutput(circopt, OperationMessageText[circe::OM_NB_PROCESSED_POINTS]+": "+asString(ns)+"/"+asString(nl), "{", "}", true);
}


/*
\-\-sourceCRS=WGS84G. \-\-sourceFormat=KML.METERS.DEGREES \-\-targetCRS=WGS84. \-\-targetFormat=XYZ.METERS. \-\-metadataFile="data/DataFRnew.txt" \-\-logPathname="circe.log" \-\-sourcePathname="./test_FR_NTF_L2e_alter.kml" \-\-targetPathname="./test_test_FR_NTF_L2e_alter.xyz"
\-\-sourceCRS=WGS84G. \-\-sourceFormat=KML.METERS.DEGREES \-\-targetCRS=WGS84. \-\-targetFormat=GPX.METERS.DEGREES \-\-metadataFile="data/DataFRnew.txt" \-\-logPathname="circe.log" \-\-sourcePathname="./test_FR_NTF_L2e_alter.kml" \-\-targetPathname="./test_test_FR_NTF_L2e_alter.gpx"
\-\-sourceCRS=WGS84G. \-\-sourceFormat=GPX.METERS.DEGREES \-\-targetCRS=WGS84G. \-\-targetFormat=ILPH.METERS.DEGREES \-\-metadataFile="data/DataFRnew.txt" \-\-logPathname="circe.log" \-\-sourcePathname="./test_test_FR_NTF_L2e_alter.gpx" \-\-targetPathname="./test_test_FR_NTF_L2e_alter.geo"
 */
/**
* \brief Processes a XML (KML or GPX) point file.
* \param[in] circopt: Processing options in a circeOptions structure.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
//KML files simple point format. This is mainly intended to process KML files created by circe.
std::string processXMLdoc(const circeOptions &circopt)
{
    return processXMLdoc(circopt, NULL,NULL);
}

std::string processXMLdoc(const circeOptions &circopt, GeodeticSet* GS, swain *swn)
{
    pt4d p;
    OPERATION_MESSAGE om;
    time_t timer;
    std::ofstream ofs;
    std::ostringstream oss;
    std::string rootTag, pointTag, message;
    unsigned int idt, nl=0, ns=0;
    //OteAccents(Fichier, Fichier+"_tmp");
    tinyxml2::XMLDocument XMLdoc;
    tinyxml2::XMLElement *XMLroot, *XMLelem;
    tinyxml2::XMLError err_load;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
        time(&timer);
		initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(p.frmt));

        switch (circopt.sourceOptions.subformatId)
        {
        case FF_KML_POINT:
            rootTag = "kml";
            pointTag = "Placemark";
            break;
        case FF_GPX_POINT:
            rootTag = "gpx";
            pointTag = "wpt";
            break;
        default:
            throw std::string("not a circe XML file.")+"\n";
            break;
        }

        err_load = XMLdoc.LoadFile(circopt.sourceOptions.pathname.data());
        if (err_load)
        {
            message.assign(XMLdoc.ErrorName());
            throw std::string(":::")+message+":::\n";
        }
        XMLroot = XMLdoc.FirstChildElement(rootTag.data());
        if (!XMLroot)
            throw std::string(":::")+circe::DataValidationText[DV_TAG_NOT_FOUND]+":::\n";

        //Just to be not too strict with KML format, but KML is far more complex than that.
        //Here we accept some sub-tags ...
        if ((circopt.sourceOptions.subformatId) == FF_KML_POINT)
        {
            if (XMLroot->FirstChildElement("Document"))
                XMLroot = XMLroot->FirstChildElement("Document");
            if (XMLroot->FirstChildElement("Folder"))
                XMLroot = XMLroot->FirstChildElement("Folder");
        }

		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
//        CatOperation CO(circopt, *GS);
        CompoundOperation CO(circopt, *GS);

        initBasicOstream(&oss);
        openOutputFile(&ofs, circopt, "", CO.getTransfoRequired());

        if (CO.getoperationMessage() != OM_OK)
        {
            ofs << OperationMessageText[CO.getoperationMessage()] << std::endl;
            throw (OperationMessageText[CO.getoperationMessage()]);
        }

        for (XMLelem = XMLroot->FirstChildElement(pointTag.data());
             XMLelem ;
             XMLelem = XMLelem->NextSiblingElement(pointTag.data()) )
        {
//            if ((dv = readKMLline(XMLelem, &p)) != DV_OK)
//                std::cout << DataValidationText[dv] << std::endl;
            if ((circopt.sourceOptions.subformatId) == FF_KML_POINT)
                readKMLline(XMLelem, &p);
            else if ((circopt.sourceOptions.subformatId) == FF_GPX_POINT)
                readGPXline(XMLelem, &p);
            om = CO.Operate(&p);
            writeVerticalInfo(GS,&p);
            if (om != OM_OK)
                oss << OperationMessageText[om] << std::endl;
            else
            {
                writeOutputLine(&oss, circopt, p, &idt);
                ns++;
            }
            if ((++nl)%1000==0)
            {
                if (GS->mb!=NULL)
                    GS->mb->messageEdit(asString(nl));
                else
                    std::cout << "\r"+asString(nl);
            }
        }
        ofs << oss.str();
        closeOutputFile(&ofs,circopt);
        endProcess(&fo, circopt, timer, 1);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    if (GS->mb==NULL)
        std::cout << "\r" << std::setw(asString(nl).length()) << " " << "\r";
    return displayOutput(circopt, OperationMessageText[circe::OM_NB_PROCESSED_POINTS]+": "+asString(ns)+"/"+asString(nl), "{", "}", true);
}


/**
* \brief Processes automatic tests or operates a set of points from one CRS to various CRSs.
* \param[in] testPathname: Path to the file containing the list of files to process.
* \param[in] mode: A MULTIPLE_CRS_PROCESSING code.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
* \remark
*
- mode ==  MP_TEST \n
Example of test file containing 3 CRS definition lines and 1 point expressed in the 3 CRSs. \n
TEST TERRE ADELIE \n
./DataTA2.txt \n
3 1 \n
--sourceCRS=PGP50G.PETRELS52 --sourceFormat=ILPV.METERS.DMS \n
1 \n
1      139.54583680933480      -66.41163966406715       31.53 \n
--sourceCRS=PGP50STPSTA.PETRELS52 --sourceFormat=IENV.METERS. \n
1 \n
1	296294.12	234824.90	31.53 \n
--sourceCRS=RGTAAF07.UNDEFINED --sourceFormat=IXYZ.METERS. \n
1 \n
1	-1936437.863	1630128.465	-5834717.797\n
*
- mode ==  MP_CALC \n
Example of calculation file containing 3 CRS definition lines and 1 point expressed in the 1st CRS. \n
TERRE ADELIE \n
./DataTA2.txt \n
3 1 \n
--sourceCRS=PGP50G.PETRELS52 --sourceFormat=ILPV.METERS.DMS \n
1 \n
1      139.54583680933480      -66.41163966406715       31.53 \n
--sourceCRS=PGP50STPSTA.PETRELS52 --sourceFormat=IENV.METERS. \n
--sourceCRS=RGTAAF07.UNDEFINED --sourceFormat=IXYZ.METERS. \n
*/
/*
 * To run tests:
\-\-runTest \-\-sourcePathname="./circe_test.txt"
 */
void processMultipleCRSdefinitionFile(std::string testPathname, MULTIPLE_CRS_PROCESSING mode)
{
    GeodeticSet GS;
    pt4d p;
    std::string test_header, boundaryfilename="", retMess;
    std::string definitionLine = "";
	std::string message, test_data_pathname, logfile, metadataFilename;
	int nFTest, iFTest, i, n, nCRS, iCRS, iCRS1, iCRS2, nPoint, iPoint, nCRS1;
	unsigned int idt;
	std::vector<std::string> vDefinitionLine;
    struct PointTest {int num; double c1; double c2; double c3; double c4; double conv; double scfact;};
    PointTest **T;
    bool bCompare;
    OPERATION_MESSAGE om;
//    ARGUMENT_VALIDATION av;
    time_t timer;
    circeOptions circopt, circopt2;
    initDefOptions(&circopt);
    initDefOptions(&circopt2);
	double largerExpectedShift = 1, maxShift = 0;

    std::string fTest;
    std::ifstream ifs;
    std::ofstream ofs;
    std::vector<std::string> tabFTest;

    //Examples of definition lines
    //\-\-sourceCRS=IGN72UTM58S.NGNC \-\-sourceFormat=IENVCS.METERS.RADIANS
    //\-\-sourceCRS=REUN47G.REUN89 \-\-sourceFormat=ILPV.METERS.DMS
    //\-\-sourceCRS=ITRF2008G.UNDEFINED \-\-sourceFormat=ILPH.METERS.DMS \-\-sourceEpoch=2015.315
    //\-\-sourceCRS=NTFPG.AUTHORITATIVE \-\-sourceFormat=ILPV.METERS.GONS

    //When defining an operation, the elements of the target definition line become
    //the target elements of the source definition line (which becomes an operation line).

	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
        std::ifstream circeTestFile(testPathname.data());
        if (!circeTestFile.is_open())
            throw std::string(ErrorMessageText[EM_CANNOT_OPEN]+testPathname);
        circeTestFile >> nFTest;
        if (circeTestFile.fail())
            throw "error reading number of test files in "+testPathname;
        for (i=0; i<nFTest; i++)
        {
            circeTestFile >> fTest;
            tabFTest.push_back(fTest);
        }
        circeTestFile.close();
        testPathname = getDirname(testPathname);

        time(&timer);
        for (iFTest=0; iFTest<nFTest; iFTest++)
        {
            test_data_pathname = testPathname+tabFTest[iFTest]+".txt";
            logfile = testPathname+"test"+tabFTest[iFTest]+".log";
            vDefinitionLine.clear();

            ifs.open(test_data_pathname.data(),std::ifstream::in);
            if (!ifs.is_open())
                throw ErrorMessageText[EM_CANNOT_OPEN]+test_data_pathname;

            getline(ifs,test_header);
            std::cout << test_header << std::endl;

            //Metadata
			ifs >> metadataFilename;
			circopt.metadataFile = metadataFilename;
            LoadGeodeticSet(circopt.metadataFile, &GS, circopt.verbose);

			//In the new test file version, the metadata filename is followed by the boundary filename.
			//The following code treats both old and new versions.
			//This should be dealt with using a test file version number.
			ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (!isdigit(ifs.peek()))
				ifs >> boundaryfilename;
			else
				boundaryfilename = "";
			//end of new test file version

            ifs >> nCRS >> nPoint >> largerExpectedShift;
			getline(ifs, definitionLine); //get CR of current line
			T = (PointTest**)malloc(nCRS*sizeof(PointTest*));
            for (iCRS=0;iCRS<nCRS;iCRS++)
            {
                T[iCRS] = (PointTest*)malloc(nPoint*sizeof(PointTest));
                for (iPoint=0;iPoint<nPoint;iPoint++)
                {
                    T[iCRS][iPoint].num = iPoint+1;
                    //Default value
                    T[iCRS][iPoint].c1 = T[iCRS][iPoint].c2 = T[iCRS][iPoint].c3 = T[iCRS][iPoint].c4 = 9999.;
                    T[iCRS][iPoint].conv = T[iCRS][iPoint].scfact = 0.;
                }
                getline(ifs,definitionLine);
                //extract command line
                vDefinitionLine.push_back(definitionLine);
                parseArgString(definitionLine, &circopt);
//                if ((av = parseArgString(definitionLine, &circopt)) != AV_OK)
//                    throw asString(iCRS)+" "+ArgumentValidationText[av]+"\n";

				//mode == MP_CALC: From the second definition line and on, the definition line is not 
				//followed by point coordinates (or if it is, they won't be used), and it may be indicated 
				//by a 0 or by nothing (in this case we find directly the next definition line).
				if (isdigit(ifs.peek()))
				{
					ifs >> n;
					for (i = 0; i < n; i++)
					{
						ifs >> iPoint;
						if (circopt.sourceOptions.formatId.find_first_of('V') != std::string::npos)
							//i.e. without ellipsoidal height (may be a 3D system nevertheless)
							//because in test files there is H or V not both
						{
							ifs >> T[iCRS][iPoint - 1].c1 >> T[iCRS][iPoint - 1].c2 >> T[iCRS][iPoint - 1].c4;
							T[iCRS][iPoint - 1].c3 = 0.;
						}
						else
						{
							ifs >> T[iCRS][iPoint - 1].c1 >> T[iCRS][iPoint - 1].c2 >> T[iCRS][iPoint - 1].c3;
							T[iCRS][iPoint - 1].c4 = 0.;
						}
                        if (circopt.extraOut == 1)
							ifs >> T[iCRS][iPoint - 1].conv >> T[iCRS][iPoint - 1].scfact;
						std::cout << asString(iPoint) + " " + asString(T[iCRS][iPoint - 1].c1) + " " + asString(T[iCRS][iPoint - 1].c2) + " " + asString(T[iCRS][iPoint - 1].c3) << std::endl;
					}
					std::cout << " completed" << std::endl;
					getline(ifs, definitionLine); //get CR of current line
				}
            }
            ifs.close();
            ifs.clear();

            ofs.open(logfile.data(),std::ios::out);
            ofs.imbue( std::locale::classic() ) ; 
            ofs << std::fixed << std::setfill(' ');

            if (!ofs.is_open())
                throw ErrorMessageText[EM_CANNOT_OPEN]+logfile;
            //ofs << GS.vGeodeticTransfo[0].DisplayMetaData();
			if (mode == MP_TEST)
			{
				ofs << test_header << std::endl << "Max expected shift " << asString(largerExpectedShift) << std::endl << "all residuals in millimeters (geographic converted along meridians and parallels)" << std::endl;
				nCRS1 = nCRS;
			}
			else
			{
				ofs << test_header << std::endl << metadataFilename << std::endl;
				if (boundaryfilename != "") ofs << boundaryfilename << std::endl;
				ofs << nCRS << " " << nPoint << std::endl;
				nCRS1 = 1;
			}
			for (iCRS1=0;iCRS1<nCRS1;iCRS1++)
            {
                for (iCRS2=0;iCRS2<nCRS;iCRS2++)
                    //if(iCRS1!=iCRS2)
                {
                    parseArgString(vDefinitionLine[iCRS1], &circopt);
                    parseArgString(vDefinitionLine[iCRS2], &circopt2);
                    initTarget(&circopt, circopt2);

                    definitionLine = "("+asString(iCRS1+1)+" to "+asString(iCRS2+1)+") "+displayCRS(circopt);
                    std::cout << "\r"+definitionLine+"                      ";
					if (mode == MP_TEST)
						ofs << definitionLine << std::endl;
					else
						ofs << vDefinitionLine[iCRS2] << std::endl << nPoint << std::endl;
					//NTF and ED50 (2D) have their MSL heights "AUTHORITATIVE" but there is no vertical
                    //transformation between them (Circe would look for one because of "AUTHORITATIVE")
                    //so the vertical CRSs are set to "UNDEFINED".
/*                    if ((circopt.sourceOptions.geodeticCRSid.compare(0,3,"NTF")==0 || circopt.sourceOptions.geodeticCRSid.compare(0,4,"ED50")==0)
                     && (circopt.targetOptions.geodeticCRSid.compare(0,3,"NTF")==0 || circopt.targetOptions.geodeticCRSid.compare(0,4,"ED50")==0))
                    {
                        circopt.sourceOptions.verticalCRSid=circopt.targetOptions.verticalCRSid="";
                    }*/

                    //NC: The test files were created using the constant transformations whereas there are
                    //grid transformations between the same systems.
                    if ((circopt.sourceOptions.geodeticCRSid.compare(0,5,"NEA74")==0 || circopt.sourceOptions.geodeticCRSid.compare(0,6,"RGNC91")==0)
                     && (circopt.targetOptions.geodeticCRSid.compare(0,5,"NEA74")==0 || circopt.targetOptions.geodeticCRSid.compare(0,6,"RGNC91")==0))
                    {
                        circopt.idGeodeticTransfoRequired = "10997";
                    }
                    if ((circopt.sourceOptions.geodeticCRSid.compare(0,5,"IGN72")==0 || circopt.sourceOptions.geodeticCRSid.compare(0,6,"RGNC91")==0)
                     && (circopt.targetOptions.geodeticCRSid.compare(0,5,"IGN72")==0 || circopt.targetOptions.geodeticCRSid.compare(0,6,"RGNC91")==0))
                    {
                        circopt.idGeodeticTransfoRequired = "10730";
                    }

                    circopt.runTest = 1;
					circopt.boundaryFile = boundaryfilename;
//                    CatOperation CO(circopt, GS);
                    CompoundOperation CO(circopt, GS);
                    if (CO.getoperationMessage() != OM_OK)
                    {
                       ofs << OperationMessageText[CO.getoperationMessage()] << std::endl;
                       CO.reset();
                       continue;
                    }
                    for (iPoint=0;iPoint<nPoint;iPoint++)
                        if ( T[iCRS1][iPoint].c1!=9999. && T[iCRS1][iPoint].c2!=9999. && T[iCRS1][iPoint].c3 != 9999.
                         && ((T[iCRS2][iPoint].c1!=9999. && T[iCRS2][iPoint].c2!=9999. && T[iCRS2][iPoint].c3 != 9999.)
							|| mode == MP_CALC))
                    {
                        //p.CRStypeInit=CO.getCurrentCRStype();
                        p.CRStypeInit=CO.getCO1().getCurrentCRStype();
                        InitPoint(&p,T[iCRS1][iPoint].c1,T[iCRS1][iPoint].c2,T[iCRS1][iPoint].c3,T[iCRS1][iPoint].c4);
                        p.frmt.n_ang = circopt.Pt4d.frmt.n_ang;//+1; //9 instead of 8
						p.frmt.n_m = circopt.Pt4d.frmt.n_m;
						om = CO.Operate(&p);
                        writeVerticalInfo(&GS,&p);
                        message = OperationMessageText[om];
                        //fail if om == OM_POINT_OUTSIDE_CRS_TYPE_DOMAIN
                        //if (CO.getTransfoRequired()&T_GEODETIC_REQUIRED)
                        if (CO.getTransfoRequired()&T_GEODETIC_REQUIRED || !CO.getuseCO1only())
                            p.CRStypeInit=CO.getTargetCRStype2();
                        else
                            p.CRStypeInit=CO.getTargetCRStype1();
						if (mode == MP_TEST)
							bCompare = ComparePoint2(CO.getTargetConversion()->get_a(), CO.getTargetConversion()->get_e2(),
							&p, T[iCRS2][iPoint].c1, T[iCRS2][iPoint].c2, T[iCRS2][iPoint].c3, T[iCRS2][iPoint].c4,
							CO.getTargetLinearUnitId(), CO.getTargetAngularUnitId(), &retMess, &maxShift);
						else
							bCompare = false;
						if (message != "")
							retMess = "                                      ";
						//Write output
						if (mode == MP_TEST || message == "")
						{
							ofs << T[iCRS1][iPoint].num << " ";
                            p.transfoIndex = 0; //don't write transformation precision information
                            writeBasicLine(&ofs, circopt, p, &idt, circopt.targetOptions.formatId);
							if (bCompare)
								ofs << retMess << " " << message;
							else
								ofs << message;
							if (maxShift >= largerExpectedShift && message == "")
							{
								std::string alertmess = "**** shift ("+asString(maxShift)+") larger than expected (" + asString(largerExpectedShift) + ") ****";
								ofs << alertmess << std::endl;
								std::cout << alertmess << std::endl;
							}
							else
								ofs << std::endl;
						}
                    }
                    CO.reset();
                }
            }
            ofs.close();
            ofs.clear();
            for (iCRS=0;iCRS<nCRS;iCRS++) free(T[iCRS]);
            free(T);
        }
        endProcess(&fo, circopt, timer, 1);
		std::cout << "\n\nTest completed. Check log files.\n";
	}
    catch (std::string str)
    {
        if (ofs.is_open())
            ofs << str;
        //writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
}

/**
* \brief Parses C-arguments into a circeOptions structure and runs the processing.
* \param[in] argc: size of argv.
* \param[in] argv: C-array of arguments.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string process(int argc, char *argv[])
{
//    std::locale::global( std::locale( "en_US.UTF-8" ) );
    std::locale::global( std::locale::classic() );
    //test_decimal_separator();
    int iargv;
    circeOptions circopt;
    std::ofstream fo;
	try
    {
        initDefOptions(&circopt);
        parseArgArray(&circopt, argc, argv, &iargv);
        initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(circopt.Pt4d.frmt));
        openLog(&fo, circopt.logPathname);
        scrutinizeOptions (&circopt);
        return processOptions(circopt);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (displayOutput(circopt, str, "{", "}", true));
    }
    fo.close();
    return "";
}

/**
* \brief Parses C-arguments into a circeOptions structure and runs the processing.
* \param[in] argc: size of argv.
* \param[in] argv: C-array of arguments.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
int process2(int argc, char *argv[])
{
//    std::locale::global( std::locale( "en_US.UTF-8" ) );
    std::locale::global( std::locale::classic() );
    //test_decimal_separator();
    int iargv;
    circeOptions circopt;
    std::ofstream fo;
    std::string message;
    try
    {
        initDefOptions(&circopt);
        parseArgArray(&circopt, argc, argv, &iargv);
        initFormats(circopt.displayPrecision, circopt.targetOptions.angularUnit, &(circopt.Pt4d.frmt));
        openLog(&fo, circopt.logPathname);
        scrutinizeOptions (&circopt);
        std::cerr << processOptions(circopt);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        std::cerr << displayOutput(circopt, str, "{", "}", true);
        return 1;
    }
    fo.close();
    return 0;
}

/**
* \brief Parses an argument string into a circeOptions structure and runs the processing.
* \param[in] argLine: argument string.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string process(const std::string& argLine)
{
	return process(argLine, NULL);
}
std::string process(const std::string& argLine, GeodeticSet* GS) /*throw(std::string)*/
{
    return process(argLine, GS, NULL);
}
std::string process(const std::string& argLine, GeodeticSet* GS, swain *s) /*throw(std::string)*/
{
//    std::locale::global( std::locale( "en_US.UTF-8" ) );
    std::locale::global(std::locale::classic());
    //test_decimal_separator();
	circeOptions circopt;
    std::ofstream fo;
	try
    {
        initDefOptions(&circopt);
        parseArgString(argLine, &circopt);
        openLog(&fo, circopt.logPathname);
        scrutinizeOptions (&circopt);
        return processOptions(circopt, GS, s);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (displayOutput(circopt, str, "{", "}", true));
    }
    fo.close();
    return "";
}

/**
* \brief Parses an argument string into a circeOptions structure and runs the processing of vector of double precision reals.
* \param[in] argLine: argument string.
* \param[in] points: The vector of double in which 4 back-to-back values represent the coordinates of one point. It is also output.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string process(const std::string& argLine, std::vector<double> &points) /*throw(std::string)*/
{
	return process(argLine, points,NULL);
}

std::string process(const std::string& argLine, std::vector<double> &points, GeodeticSet* GS) /*throw(std::string)*/
{
//    std::locale::global( std::locale( "en_US.UTF-8" ) );
    std::locale::global(std::locale::classic());
    //test_decimal_separator();
	circeOptions circopt;
    std::ofstream fo;
	try
    {
        initDefOptions(&circopt);
        parseArgString(argLine, &circopt);
        openLog(&fo, circopt.logPathname);
        scrutinizeOptions (&circopt);
        process_double_vector(circopt, points, GS);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (displayOutput(circopt, str, "{", "}", true));
    }
    fo.close();
	return "";
}

/**
* \brief Parses an argument string into a circeOptions structure and runs the processing of a single point.
* \param[in] argLine: argument string.
* \return The output pt4d.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
circe::pt4d process_pt4d(const std::string& argLine) /*throw(std::string)*/
{
	return process_pt4d(argLine, NULL);
}

circe::pt4d process_pt4d(const std::string& argLine, GeodeticSet* GS) /*throw(std::string)*/
{
    pt4d p;
    InitDefPoint(&p);
//    std::locale::global( std::locale( "en_US.UTF-8" ) );
    std::locale::global(std::locale::classic());
    //test_decimal_separator();
	circeOptions circopt;
    std::ofstream fo;
	try
    {
        initDefOptions(&circopt);
        parseArgString(argLine, &circopt);
        openLog(&fo, circopt.logPathname);
        scrutinizeOptions(&circopt);
        p = process_pt4d(circopt, GS);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (displayOutput(circopt, str, "{", "}", true));
    }
    fo.close();
    return p;
}

/**
* \brief Calls functions depending on the options.
* \param[in] circopt: processing options in a circeOptions structure.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string processOptions(circeOptions circopt)
{
    swain *s;
    return processOptions(circopt, NULL, s);
}

std::string processOptions(circeOptions circopt, GeodeticSet* GS, swain *s)
{
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
        std::vector<circe::pt4d> vPt4d;
        if (circopt.dataMode == DM_FILE)
            setFormatDefaults(&(circopt.sourceOptions),GS);
        if (circopt.verbose)
            std::cout << displayOptionsJSON(circopt);
        if (circopt.displayHelp)
            return displayHelp();
		if (circopt.runTest)
			processMultipleCRSdefinitionFile(circopt.sourceOptions.pathname, MP_TEST);
		else if (circopt.multipleCRS)
			processMultipleCRSdefinitionFile(circopt.sourceOptions.pathname, MP_CALC);
        else if (circopt.getCRSlist)
            return getCRSList(circopt, GS);
        else if (circopt.getZoneList)
        {
            //std::cout << displayOptionsJSON(circopt);
            return InitZoneList(circopt);
        }
        else if (circopt.getValidityArea)
        {
            MEASURE_UNIT sourceLinearUnitId = (MEASURE_UNIT)getIndex(circopt.sourceOptions.angularUnit,MeasureUnitAttributeText,MU_COUNT);
            if (sourceLinearUnitId == MU_UNDEF) sourceLinearUnitId = MU_DEGREE;
            return getValidityArea(circopt, GS, sourceLinearUnitId);
        }
        else if (circopt.getAuthority)
            return getAuthority(circopt, GS);
        else if (circopt.getDataFileFormat || circopt.getDataPointFormat || circopt.getUom
                 || circopt.getGeodeticTransfo || circopt.getSourceGeodeticCRS
                 || circopt.getSourceGeodeticFrame || circopt.getSourceVerticalCRS
                 || circopt.getTargetGeodeticCRS || circopt.getTargetGeodeticFrame
                 || circopt.getTargetVerticalCRS || circopt.getVerticalTransfo)
        {
            //operationSubset OS;
            //return getGeodeticSubset(&OS, &circopt, GS);
            return getGeodeticSubsetLists(&circopt, GS);
        }
#ifdef WITH_GDAL
        else if (circopt.sourceOptions.subformatId == FF_GDAL_OGR)
            processOGRdataSource(circopt, GS);
#endif
		else if (circopt.sourceOptions.subformatId == FF_KML_POINT
			|| circopt.sourceOptions.subformatId == FF_GPX_POINT)
            return processXMLdoc(circopt, GS,s);
		else if (circopt.sourceOptions.pathname == "")
			processPt4d(circopt, GS);
        else if (circopt.processAsVector) //here for test purpose
            return processIFStream(circopt,&vPt4d,GS,NULL); //something to be done with vPt4d ...
        else
            return processIFStream(circopt,GS,s);
    }
    catch (std::string str)
    {
        //writeLog(&fo, CIRCE_ERROR);
        //writeLog(&fo, str);
        //fo.close();
        throw (str);
    }
    fo.close();
	return "";
}

// WCTS

/**
* \brief Writes the list of CRS ids.
* \param[in] circopt: processing options in a circeOptions structure.
* \return The list of CRS ids.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
std::string getCRSList(circeOptions circopt) /*throw(std::string)*/
{
	return getCRSList(circopt, NULL);
}

std::string getCRSList(circeOptions circopt, GeodeticSet* GS) /*throw(std::string)*/
{
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
        return GS->getCRSidList("all",circopt);
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    return "";
}


/**
* \brief Writes the validity area of a CRS.
* \param[in] circopt: processing options in a circeOptions structure.
* \return The validity area.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
/*
\-\-metadataFile="./DataNCnew.xml" \-\-sourceCRS=IGN72UTM58S.NGNC \-\-getValidityArea
 */
std::string getValidityArea(circeOptions circopt, MEASURE_UNIT outUnit) /*throw(std::string)*/
{
    return getValidityArea(circopt, NULL, outUnit);
}

std::string getValidityArea(circeOptions circopt, GeodeticSet* GS, MEASURE_UNIT outUnit) /*throw(std::string)*/
{
    std::string message1, message2;
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
        if (circopt.sourceOptions.geodeticCRSid != "")
        {
            message1 = GS->getValidityArea(circopt.sourceOptions.geodeticCRSid, outUnit, circopt.outputFormat);
            if (message1 == ErrorMessageText[EM_NOT_FOUND])
                return displayOutput(circopt, message1, "{", "}", true);
        }
        if (circopt.sourceOptions.verticalCRSid != "")
        {
            message2 = GS->getValidityArea(circopt.sourceOptions.verticalCRSid, outUnit, circopt.outputFormat);
            if (message2 == ErrorMessageText[EM_NOT_FOUND])
                return displayOutput(circopt, message2, "{", "}", true);
		}
        else
            return displayOutput(circopt, message1, "", "", false);
        return displayOutput(circopt, message1+",\n"+message2, "[", "]", false);
//        if (circopt.outputFormat == "JSON")
//            return "{\n"+displayOptionsJSON(circopt)+message1+"\n"+message2+"\n}";
//        else
//            return message1+"\n"+message2;
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    return "";
}

/**
* \brief Writes the authority of a CRS.
* \param[in] circopt: processing options in a circeOptions structure.
* \return The authority.
* \throw str A std:string containing the error message if any. The message is also written in the log file.
*/
/*
Various ids of the same CRS
\-\-metadataFile="./DataNCnew.xml" \-\-sourceCRS=IGN72UTM58S.NGNC \-\-getAuthority
\-\-metadataFile="./IGNF.xml" \-\-sourceCRS=IGN72UTM58S.NCAL69 \-\-getAuthority
The following doesn't work because Circe doesn't read the CompoundCRSs:
\-\-metadataFile="./IGNF.xml" \-\-sourceCRS=EPSG::910906377 \-\-getAuthority
Instead use the composition of the ids:
\-\-metadataFile="./IGNF.xml" \-\-sourceCRS=EPSG::310548358.5753 \-\-getAuthority
 */
std::string getAuthority(circeOptions circopt)
{
	return getAuthority(circopt, NULL);
}

std::string getAuthority(circeOptions circopt, GeodeticSet* GS)
{
    std::string message1="", message2="", kotes = "", ret = "";
	std::string gCRStype = "", vCRStype="";
	std::ofstream fo;
    if (circopt.outputFormat == "JSON")
        kotes = "\"";
    openLog(&fo, circopt.logPathname);
	try
    {
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
		}
        if (circopt.sourceOptions.geodeticCRSid != "")
        {
			message1 = GS->getAuthority(circopt.sourceOptions.geodeticCRSid, &gCRStype);
            if (circopt.sourceOptions.verticalCRSid == "")
                ret = kotes+message1+kotes+","+kotes+gCRStype+kotes;
        }
        if (circopt.sourceOptions.verticalCRSid != "")
        {
			message2 = GS->getAuthority(circopt.sourceOptions.verticalCRSid, &vCRStype);
            if (circopt.sourceOptions.geodeticCRSid == "")
                ret = kotes+message2+kotes+","+kotes+vCRStype+kotes;
        }
        if (message1 == message2)
            ret = kotes+message2+kotes+","+kotes+gCRStype+"."+vCRStype+kotes;
        else if (ret == "")
            ret = "none";
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    return displayOutput(circopt, ret, "{", "}", false);
}

/**
* \brief Initializes data subsets according to the options selected by the user, in order to populate widgets in user-friendly HMIs.
* \param OS: a pointer to an operationSubset instance.
* \param CO: a pointer to a circeOptions instance.
* \remark getGeodeticSubset belongs to Circe API and is not a member of GeodeticSet.
*/
std::string getGeodeticSubset(operationSubset *OS, circeOptions* CO) /*throw(std::string)*/
{
	return getGeodeticSubset(OS, CO, NULL);
}
/*
std::string getGeodeticSubset(operationSubset *OS, circeOptions* CO, GeodeticSet* GS)
{
	std::ofstream fo;
	openLog(&fo, CO->logPathname);
	try
	{
		if (GS == NULL)
		{
			GS = new GeodeticSet();
			LoadGeodeticSet(CO->metadataFile, GS, false);
		}

		GS->initGeodeticIds(&CO->sourceOptions, &OS->sourceSubset);
		GS->initGeodeticIds(&CO->targetOptions, &OS->targetSubset);
        GS->getGeodeticFrameList(&OS->sourceSubset, CO->sourceOptions, NULL);
        GS->getGeodeticCRSlist(&OS->sourceSubset, CO->sourceOptions, NULL);
		if (GS->getKeyGeodeticFrame() == "")
            GS->getTargetGeodeticFrameList(&OS->targetSubset, *CO, NULL);
		else //A key Geodetic Frame is used to determine compound transformations that are cumbersome in current context
            GS->getGeodeticFrameList(&OS->targetSubset, CO->targetOptions, NULL);
        GS->getGeodeticCRSlist(&OS->targetSubset, CO->targetOptions, NULL);
        GS->getGeodeticTransfoList(OS, *CO, NULL);
        GS->getSourceVerticalFrameList(&OS->sourceSubset, CO->sourceOptions, NULL);
        GS->getTargetVerticalFrameList(&OS->targetSubset, *CO, NULL);
        GS->getVerticalTransfoList(OS, *CO, NULL);

		CO->sourceOptions.verticalCRSid = GS->getCRSid(CO->sourceOptions.verticalFrameId);
		CO->targetOptions.verticalCRSid = GS->getCRSid(CO->targetOptions.verticalFrameId);

		GS->getDataFormatList(&(OS->sourceSubset.iDataFileFormat),
			&(OS->sourceSubset.dataFileFormatList),
			&(OS->sourceSubset.dataFileFormatDisplayList),
			OS->sourceSubset.geodeticDimension,
            CO->sourceOptions, DM_FILE, NULL);
		GS->getDataFormatList(&(OS->sourceSubset.iDataPointFormat),
			&(OS->sourceSubset.dataPointFormatList),
			&(OS->sourceSubset.dataPointFormatDisplayList),
			OS->sourceSubset.geodeticDimension,
            CO->sourceOptions, DM_POINT, NULL);
        GS->getDataUnitList(std::string("source"), &OS->sourceSubset, CO->sourceOptions, NULL);

        if (CO->sourceOptions.formatId == "GDAL_OGR")
            CO->targetOptions.formatId = "GDAL_OGR";
        GS->getDataFormatList(&(OS->targetSubset.iDataFileFormat),
			&(OS->targetSubset.dataFileFormatList),
			&(OS->targetSubset.dataFileFormatDisplayList),
			OS->targetSubset.geodeticDimension,
            CO->targetOptions, DM_FILE, NULL);
		GS->getDataFormatList(&(OS->targetSubset.iDataPointFormat),
			&(OS->targetSubset.dataPointFormatList),
			&(OS->targetSubset.dataPointFormatDisplayList),
			OS->targetSubset.geodeticDimension,
            CO->targetOptions, DM_POINT, NULL);
        GS->getDataUnitList(std::string("target"), &OS->targetSubset, CO->targetOptions, NULL);

		std::ofstream fo2((CO->logPathname + ".subset").data(), std::ofstream::out);
		fo2 << displayOptions(*CO);
		fo2 << GS->displayAll(*OS);
		fo2.close();
	}
	catch (std::string str)
	{
		writeLog(&fo, CIRCE_ERROR);
		writeLog(&fo, str);
		fo.close();
		throw (str);
	}
	fo.close();
	return "";
}
*/
std::string getGeodeticSubset(operationSubset *OS, circeOptions* CO, GeodeticSet* GS) /*throw(std::string)*/
{
    std::string sl = "";

    if (CO->getAllGeodeticSubsetLists)
        sl = "getAllGeodeticSubsetLists";
    std::ofstream fo;
    openLog(&fo, CO->logPathname);
    try
    {
        if (GS == NULL)
        {
            GS = new GeodeticSet();
            LoadGeodeticSet(CO->metadataFile, GS, false);
        }

        if (!CO->getUom)
        {
            GS->initGeodeticIds(&CO->sourceOptions, &OS->sourceSubset);
            GS->initGeodeticIds(&CO->targetOptions, &OS->targetSubset);
        }

        if (CO->getAllGeodeticSubsetLists || CO->getSourceGeodeticFrame)
            GS->getGeodeticFrameList(&OS->sourceSubset, CO->sourceOptions, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getSourceGeodeticCRS)
            GS->getGeodeticCRSlist(&OS->sourceSubset, CO->sourceOptions, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getTargetGeodeticFrame)
        {
            //if (GS->getKeyGeodeticFrame() == "")
                GS->getTargetGeodeticFrameList(&OS->targetSubset, *CO, &sl, CO->outputFormat);
            //else //A key Geodetic Frame is used to determine compound transformations that are cumbersome in current context
                //GS->getGeodeticFrameList(&OS->targetSubset, CO->targetOptions, &sl, CO->outputFormat);
        }

        if (CO->getAllGeodeticSubsetLists || CO->getTargetGeodeticCRS)
            GS->getGeodeticCRSlist(&OS->targetSubset, CO->targetOptions, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getGeodeticTransfo)
            GS->getGeodeticTransfoList(OS, *CO, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getSourceVerticalCRS)
            GS->getSourceVerticalFrameList(&OS->sourceSubset, CO->sourceOptions, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getTargetVerticalCRS)
            GS->getTargetVerticalFrameList(&OS->targetSubset, *CO, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || CO->getVerticalTransfo)
            GS->getVerticalTransfoList(OS, *CO, &sl, CO->outputFormat);

        CO->sourceOptions.verticalCRSid = GS->getCRSid(CO->sourceOptions.verticalFrameId);
        CO->targetOptions.verticalCRSid = GS->getCRSid(CO->targetOptions.verticalFrameId);

        if (CO->getAllGeodeticSubsetLists || (CO->getDataFileFormat && CO->sourceOptions.geodeticCRSid!=""))
        {
            GS->getDataFormatList(&(OS->sourceSubset.iDataFileFormat),
                &(OS->sourceSubset.dataFileFormatList),
                &(OS->sourceSubset.dataFileFormatDisplayList),
                OS->sourceSubset.geodeticDimension,
                CO->sourceOptions, DM_FILE, &sl, CO->outputFormat);
            CO->getDataFileFormat = 0;
        }

        if (CO->getAllGeodeticSubsetLists || (CO->getDataPointFormat && CO->sourceOptions.geodeticCRSid!=""))
        {
            GS->getDataFormatList(&(OS->sourceSubset.iDataPointFormat),
                &(OS->sourceSubset.dataPointFormatList),
                &(OS->sourceSubset.dataPointFormatDisplayList),
                OS->sourceSubset.geodeticDimension,
                CO->sourceOptions, DM_POINT, &sl, CO->outputFormat);
            CO->getDataPointFormat = 0;
        }

        if (CO->getAllGeodeticSubsetLists || (CO->getUom && CO->sourceOptions.formatId!=""))
        {
            GS->getDataUnitList(std::string("source"), &OS->sourceSubset, CO->sourceOptions, &sl, CO->outputFormat);
            CO->getUom = 0;
        }

        if (CO->sourceOptions.formatId == "GDAL_OGR")
            CO->targetOptions.formatId = "GDAL_OGR";

        if (CO->getAllGeodeticSubsetLists || (CO->getDataFileFormat && CO->targetOptions.geodeticCRSid!=""))
            GS->getDataFormatList(&(OS->targetSubset.iDataFileFormat),
                &(OS->targetSubset.dataFileFormatList),
                &(OS->targetSubset.dataFileFormatDisplayList),
                OS->targetSubset.geodeticDimension,
                CO->targetOptions, DM_FILE, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || (CO->getDataPointFormat && CO->targetOptions.geodeticCRSid!=""))
            GS->getDataFormatList(&(OS->targetSubset.iDataPointFormat),
                &(OS->targetSubset.dataPointFormatList),
                &(OS->targetSubset.dataPointFormatDisplayList),
                OS->targetSubset.geodeticDimension,
                CO->targetOptions, DM_POINT, &sl, CO->outputFormat);

        if (CO->getAllGeodeticSubsetLists || (CO->getUom && CO->targetOptions.formatId!=""))
            GS->getDataUnitList(std::string("target"), &OS->targetSubset, CO->targetOptions, &sl, CO->outputFormat);

//        std::ofstream fo2((CO->logPathname + ".subset").data(), std::ofstream::out);
//        fo2 << displayOptions(*CO);
//        fo2 << GS->displayAll(*OS);
//        fo2.close();
    }
    catch (std::string str)
    {
        writeLog(&fo, CIRCE_ERROR);
        writeLog(&fo, str);
        fo.close();
        throw (str);
    }
    fo.close();
    return sl;
}


std::string getGeodeticSubsetLists(circeOptions* circopt, GeodeticSet* GS)
{
    if (!( circopt->getDataFileFormat || circopt->getDataPointFormat || circopt->getUom
        || circopt->getGeodeticTransfo || circopt->getSourceGeodeticCRS
        || circopt->getSourceGeodeticFrame || circopt->getSourceVerticalCRS
        || circopt->getTargetGeodeticCRS || circopt->getTargetGeodeticFrame
        || circopt->getTargetVerticalCRS || circopt->getVerticalTransfo))
        return "";
    try
    {
        std::string CRStype;
        if (GS == NULL)
        {
            GS = new GeodeticSet();
            LoadGeodeticSet(circopt->metadataFile, GS, false);
        }
        if (circopt->getSourceGeodeticCRS)
        {
            if (circopt->sourceOptions.geodeticFrameId == "")
                return GS->getCRSidList("geodetic",*circopt);
            if (GS->getAuthority(circopt->sourceOptions.geodeticFrameId, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_SOURCE_FRAMES_NOT_INITIALIZED]);
        }
        else if (circopt->getSourceVerticalCRS)
        {
            if (circopt->sourceOptions.geodeticCRSid == "")
                return GS->getCRSidList("vertical",*circopt);
            if (GS->getAuthority(circopt->sourceOptions.geodeticCRSid, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_SOURCE_GEODETIC_CRS_NOT_FOUND]);
        }
        else if (circopt->getTargetGeodeticFrame)
        {
            if (circopt->sourceOptions.geodeticCRSid == "")
                return "";
            if (GS->getAuthority(circopt->sourceOptions.geodeticCRSid, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_SOURCE_GEODETIC_CRS_NOT_FOUND]);
        }
        else if (circopt->getTargetGeodeticCRS)
        {
            if (circopt->sourceOptions.geodeticCRSid == "")
                return "";
            if (GS->getAuthority(circopt->sourceOptions.geodeticCRSid, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_SOURCE_GEODETIC_CRS_NOT_FOUND]);
            if (GS->getAuthority(circopt->targetOptions.geodeticFrameId, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_TARGET_FRAMES_NOT_INITIALIZED]);
        }
        else if (circopt->getTargetVerticalCRS)
        {
            if (circopt->sourceOptions.geodeticCRSid == "" || circopt->targetOptions.geodeticCRSid == "")
                return "";
            if (GS->getAuthority(circopt->sourceOptions.geodeticCRSid, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_SOURCE_GEODETIC_CRS_NOT_FOUND]);
            if (GS->getAuthority(circopt->targetOptions.geodeticCRSid, &CRStype).find("none")!=std::string::npos)
                throw (OperationMessageText[OM_TARGET_GEODETIC_CRS_NOT_FOUND]);
        }

        operationSubset OS;
        std::string ret = getGeodeticSubset(&OS, circopt, GS);
        return displayOutput(*circopt, ret, "{", "}", false);
    }
    catch (std::string str)
    {
        throw (str);
    }
}

/**
* \brief Loads a metadata file.
* \param[in] filedata: A metadata file path.
* \param[in] verbose: true in verbose mode.
* \param[out] GS: A GeodeticSet instance initialized.
* \throw a std:string containing the error message (if any).
*/
void LoadGeodeticSet(std::string filedata, GeodeticSet* GS, int verbose) /*throw(std::string)*/
{
	struct stat buffer;
	try
    {
        if (stat(filedata.c_str(), &buffer) != 0)
            throw std::string(ArgumentValidationText[AV_BAD_METADATAFILENAME])+": "+filedata;

		GS->metadataPath = getDirname(filedata);
		if (getExtension(filedata) == "txt")
		{
			LEGACY_METADATA_SET lms;
			if (verbose) std::cout << "loading " << filedata << "..." << std::endl;
            LoadCirceLegacyMetadataFile(filedata, &lms);
			filedata = replaceExtension(filedata, std::string("xml"));
			WriteXMlfromCirceLegacyMetadata(filedata, lms);
		}
		if (getExtension(filedata) == "xml")
		{
			if (verbose) std::cout << "loading " << filedata << "..." << std::endl;
			if (getFilename(filedata).substr(0, 4) == "IGNF")
				LoadIGNFxmlMetadata(filedata, GS);
			else
				LoadCirceXMLmetadata(filedata, GS);
			GS->CompleteMetadata();
		}
		else
			throw std::string(ArgumentValidationText[AV_BAD_METADATAFILE_EXTENSION]);
	}
	catch (std::string str)
	{
		throw (str);
	}
}


/**
* \brief Prints the geographical zones handled by Circe.
* \return The list of geographical zone names separated by commas.
* \remark A geographical zone name is the name of a subdirectory of the working
* directory containing a Circe xml option file i.e. a file whose name begins
* with OPTION_FILE_PREFIX.
*/
#ifdef UNIX
std::string InitZoneList(const circeOptions& circopt)
{
    DIR * rep = opendir(".");
    char subDirname[256];
    bool found;
    std::string zoneList="", kotes = "", fina, frmt = circopt.outputFormat;
    circeOptions co;
    if (frmt == "JSON")
        kotes = "\"";
    if (rep != NULL)
    {
        struct dirent * entry;
        while ((entry = readdir(rep)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                sprintf(subDirname,"./%s", entry->d_name);
                DIR * subRep = opendir(subDirname);
                if (subRep != NULL)
                {
                    //printf("%s\n", subDirname);
                    struct dirent * subEntry;
                    found = false;
                    while ((subEntry = readdir(subRep)) != NULL)
                    {
                        if (! (subEntry->d_type == DT_DIR) )
                        {
                            fina = subEntry->d_name;
                            //printf("%s\n", fina.data());
                            if (fina.substr(0,13) == OPTION_FILE_PREFIX)
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (found)
                    {
                        std::string sdn(entry->d_name);
                        readXMLOptionFile(&co, sdn+"/"+fina);
                        if (zoneList != "")
                            zoneList += ",";
                        zoneList += kotes+sdn+kotes+":"+kotes+co.metadataFile+kotes;
                    }
                }
            }
        }
        closedir(rep);
    }
    return displayOutput(circopt, zoneList, "{", "}", false);
}
#elif defined WIN32
std::string InitZoneList(const circeOptions& circopt)
{
    std::string frmt = circopt.outputFormat;
    std::wstring diname, zoneList=L"", kotes = L"";
    WIN32_FIND_DATA File, subFile;
    HANDLE hSearch, hSubSearch;
    TCHAR optFileName[_MAX_PATH], optFile[_MAX_PATH];
    if (frmt == "JSON")
        kotes = L"\"";
    //hSearch = FindFirstFile(LPCWSTR(dataPathName.c_str()), &File);
    hSearch = FindFirstFile(LPCWSTR("*"), &File);
    if (hSearch != INVALID_HANDLE_VALUE)
    {
        do
        {
//            printf("%s\n", LPCWSTR(File.cFileName));
//            std::wcout << File.cFileName << std::endl;
            if (File.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                swprintf(optFileName,L"%s\\%s*",File.cFileName,OPTION_FILE_PREFIX_W);
                hSubSearch = FindFirstFile(LPCWSTR(optFileName), &subFile);
                if (hSubSearch != INVALID_HANDLE_VALUE)
                {
                    swprintf(optFile,L"%s%s%s:%s%s%s",
                             kotes.data(),File.cFileName,kotes.data(),kotes.data(),subFile.cFileName,kotes.data());
                    diname.assign(optFile);
                    if (zoneList != L"")
                        zoneList += L",";
                    zoneList += diname;
                }
                FindClose(hSubSearch);
            }
        } while (FindNextFile(hSearch, &File));
        FindClose(hSearch);
    }
    if (frmt == "JSON")
        zoneList = L"\"output\":{"+zoneList+L"}";
    std::wcout << zoneList;
    return "";
}
#else
std::string InitZoneList(const circeOptions& circopt)
{
    return "";
}

#endif

} //namespace circe
