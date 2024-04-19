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
* \file parser.cpp
* \brief function arguments  =>  [parser]  =>  circe options.
* \brief The options available to the users follow some of the POSIX recommendations. An option may be input by:
* - a long option name;
* - a long option name with an argument separated by a '=' without spaces;
* - a short option name;
* - a short option name with an argument with or without a separating space.
*
* \brief But as for multiple short option names without argument, unlike POSIX recommendations, ‘-abc’ is NOT equivalent to ‘-a -b -c’.
*
* \brief Hopefully the uppercase version of a lowercase key has a logical relation with it (it is often a source/target relation).
*/

#include <sys/stat.h>
#include "parser.h"
#include "files.h"
#include <cstdio> //sscanf
#include <cstdlib> //malloc, free
#include <cstring> //strcpy,strtok
#include <sstream> //istringstream
#include <iomanip> //setfill
#include <iostream> //cout
#include <iterator>
#include <cassert>
#include "stringtools.h"


namespace circe
{

/**
* \brief Option default initialization.
* \param circopt: A pointer to a circeOptions instance.
*/
void initDefOptions(circeOptions *circopt)
{
    circopt->appCopyright = "";
    circopt->appDescription = "";
    circopt->appHelp = "";
    circopt->appStyle = "";
    circopt->appTitle = "";
    circopt->appVersion = "";
    circopt->appUserManual = "";
    circopt->dataMode = DM_UNDEF;
    circopt->metadataFile = "";
	circopt->boundaryFile = "";
    circopt->menuTitle_zone = "";
    circopt->menuTitle_display = "";
    circopt->menuTitle_tool = "";
    circopt->menuTitle_help = "";
    circopt->sourceOptions.geodeticCRSid = circopt->sourceOptions.verticalCRSid = "";
    circopt->targetOptions.geodeticCRSid = circopt->targetOptions.verticalCRSid = "";
    circopt->idGeodeticTransfoRequired = circopt->idVerticalTransfoRequired = "";
    circopt->sourceOptions.formatId = circopt->sourceOptions.angularUnit = "";
    circopt->targetOptions.formatId = circopt->targetOptions.angularUnit = "";
    circopt->sourceOptions.linearUnit = "METERS";
    circopt->targetOptions.linearUnit = "METERS";
    circopt->sourceOptions.epoch = circopt->targetOptions.epoch = 0.;
	circopt->displayHelp = circopt->runTest = circopt->multipleCRS = circopt->quiet = 0;
    circopt->verbose = 0;
    circopt->extraOut = 0;
    circopt->sourceOptions.pathname = circopt->targetOptions.pathname = "";
    circopt->sourceOptions.subformatId = circopt->targetOptions.subformatId = FF_BASIC;
    circopt->processAsVector = 0;
    circopt->loadAsVector = 0;
    circopt->getCRSlist = 0;
    circopt->getZoneList = 0;
    circopt->getValidityArea = 0;
    circopt->getAuthority = 0;
	circopt->followUpTransfos = 0;
    circopt->gridLoading = "UNDEFINED";
    //circopt->logPathname = "circe.log"; //ok
    InitDefPoint(&(circopt->Pt4d));
    circopt->optionFile = "";
    circopt->logoFilename = "";
    circopt->imageFilename = "";
    circopt->iconFilename = "";
    circopt->sourceOptions.useGeodeticCRS = circopt->targetOptions.useGeodeticCRS = true,
    circopt->sourceOptions.useVerticalCRS = circopt->targetOptions.useVerticalCRS = false;
    circopt->sourceOptions.verticalFrameAuthoritative = circopt->targetOptions.verticalFrameAuthoritative = false;
    circopt->printCS = false;
    circopt->displayBoundingBox = false;
    circopt->noPrintSppm = false;
    circopt->scrollArea = false;
    circopt->noUseVerticalFrameAuthoritative = false;
    circopt->plainDMS = false;
    circopt->sourceOptions.geodeticFrameId = circopt->targetOptions.geodeticFrameId = "";
    circopt->sourceOptions.verticalFrameId = circopt->targetOptions.verticalFrameId = "";
    circopt->sourceOptions.usesForTransformation = circopt->targetOptions.usesForTransformation = "";
    circopt->sourceOptions.geodeticCRStype = circopt->targetOptions.geodeticCRStype = CT_UNDEF;
	circopt->displayPrecision = 0.001; // 1 mm
    circopt->getSourceGeodeticFrame = 0;
    circopt->getTargetGeodeticFrame = 0;
    circopt->getSourceGeodeticCRS = 0;
    circopt->getTargetGeodeticCRS = 0;
    circopt->getSourceVerticalCRS = 0;
    circopt->getTargetVerticalCRS = 0;
    circopt->getDataFileFormat = 0;
    circopt->getDataPointFormat = 0;
    circopt->getUom = 0;
    circopt->getGeodeticTransfo = 0;
    circopt->getVerticalTransfo = 0;
    circopt->getAllGeodeticSubsetLists = 0;
    circopt->outputFormat = "";
    circopt->operationType = "";
    circopt->separator = "_";
}

/**
* \brief Sets circopt2.sourceOptions as circopt1->targetOptions.
* \param circopt1, circopt2: Pointers to circeOptions instances.
*/
void initTarget(circeOptions *circopt1, circeOptions circopt2)
{
    circopt1->targetOptions.geodeticCRSid = circopt2.sourceOptions.geodeticCRSid;
    circopt1->targetOptions.verticalCRSid = circopt2.sourceOptions.verticalCRSid;
    circopt1->targetOptions.formatId = circopt2.sourceOptions.formatId;
    circopt1->targetOptions.subformatId = circopt2.sourceOptions.subformatId;
    circopt1->targetOptions.linearUnit = circopt2.sourceOptions.linearUnit;
    circopt1->targetOptions.angularUnit = circopt2.sourceOptions.angularUnit;
    circopt1->targetOptions.epoch = circopt2.sourceOptions.epoch;
    circopt1->targetOptions.pathname = circopt2.sourceOptions.pathname;
    circopt1->extraOut = circopt2.extraOut;
}

/**
* \brief Copy values of an instance (circopt1) of circeOptions into another (circopt2).
* \param circopt1, circopt2: Pointers to circeOptions instances.
*/
void copyOptions(circeOptions *circopt2, const circeOptions &circopt1)
{
    copyGeodeticOptions(circopt1.sourceOptions, &(circopt2->sourceOptions));
    copyGeodeticOptions(circopt1.targetOptions, &(circopt2->targetOptions));
    copyPt4d(&(circopt2->Pt4d), circopt1.Pt4d);
    circopt2->appCopyright = circopt1.appCopyright;
    circopt2->appDescription = circopt1.appDescription;
    circopt2->appHelp = circopt1.appHelp;
    circopt2->appStyle = circopt1.appStyle;
    circopt2->appTitle = circopt1.appTitle;
    circopt2->appVersion = circopt1.appVersion;
    circopt2->appUserManual = circopt1.appUserManual;
    circopt2->dataMode = circopt1.dataMode;
    circopt2->metadataFile = circopt1.metadataFile;
    circopt2->boundaryFile = circopt1.boundaryFile;
    circopt2->menuTitle_zone = circopt1.menuTitle_zone;
    circopt2->menuTitle_display = circopt1.menuTitle_display;
    circopt2->menuTitle_tool = circopt1.menuTitle_tool;
    circopt2->menuTitle_help = circopt1.menuTitle_help;
    circopt2->idGeodeticTransfoRequired = circopt1.idGeodeticTransfoRequired;
    circopt2->idVerticalTransfoRequired = circopt1.idVerticalTransfoRequired;
    circopt2->displayHelp = circopt1.displayHelp;
    circopt2->runTest = circopt1.runTest;
    circopt2->multipleCRS = circopt1.multipleCRS;
    circopt2->quiet = circopt1.quiet;
    circopt2->verbose = circopt1.verbose;
    circopt2->extraOut = circopt1.extraOut;
    circopt2->processAsVector = circopt1.processAsVector;
    circopt2->loadAsVector = circopt1.loadAsVector;
    circopt2->getCRSlist = circopt1.getCRSlist;
    circopt2->getZoneList = circopt1.getZoneList;
    circopt2->getValidityArea = circopt1.getValidityArea;
    circopt2->getAuthority = circopt1.getAuthority;
    circopt2->followUpTransfos = circopt1.followUpTransfos;
    circopt2->gridLoading = circopt1.gridLoading;
    circopt2->logPathname = circopt1.logPathname;
    circopt2->optionFile = circopt1.optionFile;
    circopt2->logoFilename = circopt1.logoFilename;
    circopt2->imageFilename = circopt1.imageFilename;
    circopt2->iconFilename = circopt1.iconFilename;
    circopt2->printCS = circopt1.printCS;
    circopt2->displayBoundingBox = circopt1.displayBoundingBox;
    circopt2->noPrintSppm = circopt1.noPrintSppm;
    circopt2->scrollArea = circopt1.scrollArea;
    circopt2->noUseVerticalFrameAuthoritative = circopt1.noUseVerticalFrameAuthoritative;
    circopt2->plainDMS = circopt1.plainDMS;
    circopt2->displayPrecision = circopt1.displayPrecision;
    circopt2->getSourceGeodeticFrame = circopt1.getSourceGeodeticFrame;
    circopt2->getTargetGeodeticFrame = circopt1.getTargetGeodeticFrame;
    circopt2->getSourceGeodeticCRS = circopt1.getSourceGeodeticCRS;
    circopt2->getTargetGeodeticCRS = circopt1.getTargetGeodeticCRS;
    circopt2->getSourceVerticalCRS = circopt1.getSourceVerticalCRS;
    circopt2->getTargetVerticalCRS = circopt1.getTargetVerticalCRS;
    circopt2->getDataFileFormat = circopt1.getDataFileFormat;
    circopt2->getDataPointFormat = circopt1.getDataPointFormat;
    circopt2->getUom = circopt1.getUom;
    circopt2->getGeodeticTransfo = circopt1.getGeodeticTransfo;
    circopt2->getVerticalTransfo = circopt1.getVerticalTransfo;
    circopt2->getAllGeodeticSubsetLists = circopt1.getAllGeodeticSubsetLists;
    circopt2->outputFormat = circopt1.outputFormat;
    circopt2->operationType = circopt1.operationType;
    circopt2->separator = circopt1.separator;
}

/**
* \brief Swaps values of two instances of geodeticOptions.
* \param go1, go2: Pointers to geodeticOptions instances.
*/
void swapGeodeticOptions(geodeticOptions *go1, geodeticOptions *go2)
{
    geodeticOptions got;
    copyGeodeticOptions(*go1,&got);
    copyGeodeticOptions(*go2,go1);
    copyGeodeticOptions(got,go2);
}

/**
* \brief Copy values of an instance (go1) of geodeticOptions into another (go2).
* \param go1, go2: Pointers to geodeticOptions instances.
*/
void copyGeodeticOptions(const geodeticOptions &go1, geodeticOptions *go2)
{
    go2->geodeticFrameId             = go1.geodeticFrameId;
    go2->geodeticCRSid               = go1.geodeticCRSid;
    go2->verticalFrameId             = go1.verticalFrameId;
    go2->verticalCRSid               = go1.verticalCRSid;
    go2->usesForTransformation       = go1.usesForTransformation;
    go2->formatId                    = rmExtraFormat(go1.formatId);
    go2->linearUnit                  = go1.linearUnit;
    go2->angularUnit                 = go1.angularUnit;
    go2->pathname                    = go1.pathname;
    go2->epoch                       = go1.epoch;
    go2->subformatId                 = go1.subformatId;
    go2->useGeodeticCRS              = go1.useGeodeticCRS;
    go2->useVerticalCRS              = go1.useVerticalCRS;
    go2->verticalFrameAuthoritative  = go1.verticalFrameAuthoritative;
    go2->geodeticCRStype             = go1.geodeticCRStype;
}

/**
* \brief Displays help for user available options.
*/
std::string displayHelp()
{
    std::wstringstream oss(L"");
    assert(sizeof(circeArgumentDefinitions));
    int iopt, nopt=sizeof(circeArgumentDefinitions)/sizeof(*circeArgumentDefinitions), igroup=0;
    bool found=true;
    oss << "circelib version " << CL_VERSION << std::endl << std::endl;
	oss << HELP_HEADER;
	oss << std::endl;
    while (found)
    //stops at index of a group without option (even if there is a higher option group)
    {
        found=false;
        //don't output if key < 0
        for (iopt=0;iopt<nopt;iopt++)
            if (circeArgumentDefinitions[iopt].key >= 0 && circeArgumentDefinitions[iopt].group == igroup)
        {
            oss << std::left << L"-";
            oss << std::left << std::setfill(L' ') << std::setw(10) << char(circeArgumentDefinitions[iopt].key);
            oss << std::left << L"--";
            oss << std::left << std::setfill(L' ') << std::setw(23) << circeArgumentDefinitions[iopt].name;
			if (circeArgumentDefinitions[iopt].arg)
                oss << L"ok    ";
			else
                oss << L"      ";
            oss << L"" << OptionsHelpText[circeArgumentDefinitions[iopt].doc] << L"\n";
            found=true;
        }
        ++igroup;
    }
	oss << std::endl;
	oss << HELP_SYNTAX;

    //return oss.str();
    std::wcout << oss.str();
    return "";
}

/**
* \brief Displays options.
* \param circopt: A circeOptions instance.
* \return a std::string of options.
*/
std::string displayOptions(const circeOptions& circopt)
{
    std::string s="";
    std::ostringstream ss;
//    s += "CIRCE OPTIONS\n";
    s += "OPTIONS\n";
    s += "metadataFile=              " + circopt.metadataFile + "\n";
	s += "boundaryFile=              " + circopt.boundaryFile + "\n";
	s += "logPathname=               " + circopt.logPathname + "\n";
    s += "SOURCE OPTIONS\n"+displayGeodeticOptions(circopt.sourceOptions);
    s += "TARGET OPTIONS\n"+displayGeodeticOptions(circopt.targetOptions);
    s += "extraOut=                  "+asString(circopt.extraOut)+"\n";
    s += "TRANSFORMATIONS\n";
    s += "idGeodeticTransfoRequired= "+circopt.idGeodeticTransfoRequired+"\n";
    s += "idVerticalTransfoRequired= "+circopt.idVerticalTransfoRequired+"\n";
	s += "displayPrecision= "         +asString(circopt.displayPrecision) + "\n";
    s += "POINT\n";
    initBasicOstream(&ss);
	unsigned int idt;
    writeBasicLine(&ss, circopt, circopt.Pt4d, &idt, circopt.targetOptions.formatId);
	s += "Pt4d=                " + ss.str() + "\n";
    return s;
}

/**
* \brief Displays options in JSON format.
* \param circopt: A circeOptions instance.
* \return a std::string of options.
*/
std::string displayOptionsJSON(const circeOptions& circopt)
{
    std::string s="";
    if (circopt.outputFormat != "JSON")
        return s;
    std::ostringstream ss;
    s += "\"operation\":\"" + circopt.operationType + "\",\n";
    s += "\"metadataFile\":\"" + circopt.metadataFile + "\",\n";
    s += "\"boundaryFile\":\"" + circopt.boundaryFile + "\",\n";
    s += "\"logPathname\":\"" + circopt.logPathname + "\",\n";
    s += "\"source\":{\n"+displayGeodeticOptionsJSON(circopt.sourceOptions)+"},\n";
    if (circopt.operationType=="" || circopt.operationType=="conversion"
            || circopt.operationType=="transformation")
    {
        s += "\"target\":{\n"+displayGeodeticOptionsJSON(circopt.targetOptions)+"},\n";
        s += "\"transformations\":{\n";
        s += "\"idGeodeticTransfoRequired\":\""+circopt.idGeodeticTransfoRequired+"\",\n";
        s += "\"idVerticalTransfoRequired\":\""+circopt.idVerticalTransfoRequired+"\"\n},\n";
    }
    s += "\"displayPrecision\":\""+asString(circopt.displayPrecision) + "\",\n";
    s += "\"input\":{\n";
    initBasicOstream(&ss);
    unsigned int idt;
    writeBasicLine(&ss, circopt, circopt.Pt4d, &idt, circopt.sourceOptions.formatId);
    s += ss.str() + "},\n";
    return s;
}

/**
* \brief Displays geodetic options.
* \param go: A geodeticOptions instance.
* \return a std::string of geodetic options.
*/
std::string displayGeodeticOptions(const geodeticOptions& go)
{
    std::string s="";
    s += "GeodeticCRSid=       "+go.geodeticCRSid+"\n";
    s += "VerticalCRSid=       "+go.verticalCRSid+"\n";
    s += "FormatId=            "+go.formatId+"\n";
    s += "LinearUnit=          "+go.linearUnit+"\n";
    s += "AngularUnit=         "+go.angularUnit+"\n";
    s += "Pathname=            "+go.pathname+"\n";
    s += "Epoch=               "+asString(go.epoch)+"\n";
    return s;
}

/**
* \brief Displays geodetic options in JSON format.
* \param go: A geodeticOptions instance.
* \return a std::string of geodetic options.
*/
std::string displayGeodeticOptionsJSON(const geodeticOptions& go)
{
    std::string s="";
    s += "\"GeodeticCRSid\":\""+go.geodeticCRSid+"\",\n";
    s += "\"VerticalCRSid\":\""+go.verticalCRSid+"\",\n";
    s += "\"FormatId\":\""+go.formatId+"\",\n";
    s += "\"LinearUnit\":\""+go.linearUnit+"\",\n";
    s += "\"AngularUnit\":\""+go.angularUnit+"\",\n";
    s += "\"Pathname\":\""+go.pathname+"\",\n";
    s += "\"Epoch=\":\""+asString(go.epoch)+"\"\n";
    return s;
}

/**
* \brief Displays the source and target CRS ids.
* \param circopt: A circeOptions instance.
* \return a std::string containing the ids.
*/
std::string displayCRS(circeOptions circopt)
{
    std::string sourceCRS = circopt.sourceOptions.geodeticCRSid;
    if (circopt.sourceOptions.epoch != 0)
        sourceCRS += "@"+asString(circopt.sourceOptions.epoch,3);
    std::string targetCRS = circopt.targetOptions.geodeticCRSid;
    if (circopt.targetOptions.epoch != 0)
        targetCRS += "@"+asString(circopt.targetOptions.epoch);
    return sourceCRS+"."+circopt.sourceOptions.verticalCRSid+" to "+targetCRS+"."+circopt.targetOptions.verticalCRSid;
}

/**
* \brief Displays an error message about an invalid argument in a C argument array.
* \param[in] argv: C argument array.
* \param[in] iargv: index of invalid argument.
* \param[in] av: an ARGUMENT_VALIDATION code.
* \return a std::string containing the error message.
*/
std::string displayInvalidArgument(char *argv[], int iargv, ARGUMENT_VALIDATION av)
{
    if(iargv < sizeof(argv)/sizeof(*argv))
        return std::string(":::")+argv[iargv]+":"+circe::ArgumentValidationText[av]+":::\n";
    else
        return std::string(":::")+circe::ArgumentValidationText[av]+":::\n";
}

/**
* \brief Asesses wether the basic data format contains an ellipsoid height 
* (explicitly with an 'H' or implicitly with an 'X' that points to geocentric coordinates).
* \param[in] frmt: The data format as a string.
* \return A boolean.
*/
bool hasEllHeight(std::string frmt)
{
    if (frmt.find_first_of('H') != std::string::npos
     || frmt.find_first_of('X') != std::string::npos)
        return true;
    else
        return false;
}

/**
* \brief Asesses wether the basic data format contains a vertical coordinate (explicitly with a 'V').
* \param[in] frmt: The data format as a string.
* \return A boolean.
*/
bool hasVertical(std::string frmt)
{
    if (frmt.find_first_of('V') != std::string::npos)
        return true;
    else
        return false;
}

/**
* \brief Asesses wether the basic data format contains an identifier (explicitly with an 'I').
* \param[in] frmt: The data format as a string.
* \return A boolean.
*/
bool hasIdentifier(std::string frmt)
{
    if (frmt.find_first_of('I') != std::string::npos)
        return true;
    else
        return false;
}

/**
* \brief Switches between 'H' and 'V' (vertical coordinate) in a basic data format.
* \param[in] frmt: The data format as a string.
* \return A boolean.
*/
bool changeVerticalComponent(std::string *frmt, char char_in, const char* char_out)
{
    std::size_t pos;
    if ((pos=frmt->find_first_of(char_in)) != std::string::npos)
    {
        frmt->replace(pos,1,char_out);
        return true;
    }
    else
        return false;
}

/**
* \brief Asesses wether the basic data format contains the convergence (explicitly with a 'C').
* \param[in] frmt: The data format as a string.
* \return A boolean.
*/
bool hasConvergence(std::string frmt)
{
    if (frmt.find_first_of('C') != std::string::npos)
        return true;
    else
        return false;
}

/**
* \brief Parses a single argument.
* \param[in] key: The char switch.
* \param[in] arg: The C-string value following the char switch, if any.
* \param[out] circopt: The circeOptions instance updated according to the argument.
*/
void parseOptions (int key, char *arg, circeOptions *circopt)
{
//    circeOptions *circopt = static_cast<circeOptions*>(state->input);
    std::istringstream ss;
    std::string s="";

    if (arg)
        ss.str(arg);
    switch (key)
    {
    case 'm':
        if (!ss.eof()) getline(ss, circopt->metadataFile);
        break;
    case 's':
        if (!ss.eof()) getline(ss, circopt->sourceOptions.geodeticCRSid, '.');
        if (!ss.eof()) getline(ss, circopt->sourceOptions.verticalCRSid, '.');
        if (circopt->sourceOptions.geodeticCRSid=="UNDEFINED") circopt->sourceOptions.geodeticCRSid="";
        if (circopt->sourceOptions.verticalCRSid=="UNDEFINED") circopt->sourceOptions.verticalCRSid="";
        break;
    case 'S':
        if (!ss.eof()) getline(ss, circopt->targetOptions.geodeticCRSid, '.');
        if (!ss.eof()) getline(ss, circopt->targetOptions.verticalCRSid, '.');
        if (circopt->targetOptions.geodeticCRSid=="UNDEFINED") circopt->targetOptions.geodeticCRSid="";
        if (circopt->targetOptions.verticalCRSid=="UNDEFINED") circopt->targetOptions.verticalCRSid="";
        break;
    case 'i':
        if (!ss.eof()) getline(ss, circopt->sourceOptions.geodeticFrameId, '.');
        if (!ss.eof()) getline(ss, circopt->sourceOptions.verticalFrameId, '.');
        if (circopt->sourceOptions.geodeticFrameId=="UNDEFINED") circopt->sourceOptions.geodeticFrameId="";
        if (circopt->sourceOptions.verticalFrameId=="UNDEFINED") circopt->sourceOptions.verticalFrameId="";
        break;
    case 'I':
        if (!ss.eof()) getline(ss, circopt->targetOptions.geodeticFrameId, '.');
        if (!ss.eof()) getline(ss, circopt->targetOptions.verticalFrameId, '.');
        if (circopt->targetOptions.geodeticFrameId=="UNDEFINED") circopt->targetOptions.geodeticFrameId="";
        if (circopt->targetOptions.verticalFrameId=="UNDEFINED") circopt->targetOptions.verticalFrameId="";
        break;
    case 'r':
        if (!ss.eof()) getline(ss, circopt->idGeodeticTransfoRequired, '.');
        if (!ss.eof()) getline(ss, circopt->idVerticalTransfoRequired, '.');
        break;
    case 'f':
        if (!ss.eof()) getline(ss, circopt->sourceOptions.formatId, '.');
        if (!ss.eof()) getline(ss, circopt->sourceOptions.linearUnit, '.');
        if (!ss.eof()) getline(ss, circopt->sourceOptions.angularUnit, '.');
        break;
    case 'F':
        if (!ss.eof()) getline(ss, circopt->targetOptions.formatId, '.');
        if (!ss.eof()) getline(ss, circopt->targetOptions.linearUnit, '.');
        if (!ss.eof()) getline(ss, circopt->targetOptions.angularUnit, '.');
        break;
    case 'e':
        if (!ss.eof())
        {
            getline(ss, s);
            sscanf(s.data(),"%lf",&(circopt->sourceOptions.epoch));
        }
        break;
    case 'E':
        if (!ss.eof())
        {
            getline(ss, s);
            sscanf(s.data(),"%lf",&(circopt->targetOptions.epoch));
        }
        break;
    case 'p':
        if (!ss.eof()) getline(ss, circopt->sourceOptions.pathname);
        circopt->sourceOptions.pathname = replaceChar(circopt->sourceOptions.pathname,'@',' ');
        break;
    case 'P':
        if (!ss.eof()) getline(ss, circopt->targetOptions.pathname);
        circopt->targetOptions.pathname = replaceChar(circopt->targetOptions.pathname,'@',' ');
        break;
    case 'h':
        circopt->displayHelp = 1;
        circopt->operationType = "displayHelp";
        break;
	case 't':
		circopt->runTest = 1;
        circopt->operationType = "runTest";
        break;
	case 'T':
		circopt->multipleCRS = 1;
        circopt->operationType = "multipleCRS";
        break;
	case 'R':
		circopt->followUpTransfos = 1;
        break;
	case 'q':
        circopt->verbose = 0;
        circopt->quiet = 1;
        break;
    case 'Q':
        circopt->verbose = 1;
        circopt->quiet = 0;
        break;
    case 'c':
        circopt->processAsVector = 1;
        break;
    case 'C':
        circopt->loadAsVector = 1;
        break;
    case 'd':
        circopt->plainDMS = 1;
        break;
    case 'D':
        circopt->noPrintSppm = 1;
        break;
    case 'g':
        if (!ss.eof()) getline(ss, circopt->gridLoading);
        break;
    case 'o':
        if (!ss.eof()) getline(ss, circopt->logPathname);
        break;
    case 'O':
        if (!ss.eof()) getline(ss, circopt->outputFormat);
        break;
    case 'b':
		if (!ss.eof()) getline(ss, circopt->boundaryFile);
		break;
	case 'n':
		if (!ss.eof())
		{
			getline(ss, s);
			sscanf(s.data(), "%lf", &(circopt->displayPrecision));
		}
		break;
    case 'N':
        if (!ss.eof()) getline(ss, circopt->separator);
        break;
    case 'a':
        circopt->getValidityArea = 1;
        circopt->operationType = "getValidityArea";
        break;
    case 'l':
        circopt->getCRSlist = 1;
        circopt->operationType = "getCRSlist";
        break;
    case 'L':
        circopt->getZoneList = 1;
        circopt->operationType = "getZoneList";
        break;
    case 'u':
        circopt->getAuthority = 1;
        circopt->operationType = "getAuthority";
        break;
    case 'U':
        circopt->getUom = 1;
        circopt->operationType = "getUom";
        break;
    case 'v':
        circopt->getDataFileFormat = 1;
        circopt->operationType = "getDataFileFormat";
        break;
    case 'V':
        circopt->getDataPointFormat = 1;
        circopt->operationType = "getDataPointFormat";
        break;
    case 'w':
        circopt->getGeodeticTransfo = 1;
        circopt->operationType = "getGeodeticTransfo";
        break;
    case 'W':
        circopt->getVerticalTransfo = 1;
        circopt->operationType = "getVerticalTransfo";
        break;
    case 'x':
        circopt->getSourceGeodeticFrame = 1;
        circopt->operationType = "getSourceGeodeticFrame";
        break;
    case 'X':
        circopt->getTargetGeodeticFrame = 1;
        circopt->operationType = "getTargetGeodeticFrame";
        break;
    case 'y':
        circopt->getSourceGeodeticCRS = 1;
        circopt->operationType = "getSourceGeodeticCRS";
        break;
    case 'Y':
        circopt->getTargetGeodeticCRS = 1;
        circopt->operationType = "getTargetGeodeticCRS";
        break;
    case 'z':
        circopt->getSourceVerticalCRS = 1;
        circopt->operationType = "getSourceVerticalCRS";
        break;
    case 'Z':
        circopt->getTargetVerticalCRS = 1;
        circopt->operationType = "getTargetVerticalCRS";
        break;
    default:
        throw std::string(ArgumentValidationText[AV_ERR_UNKNOWN]);
    }
    return;
}

/**
* \brief Validates and completes a data format.
* \param[in] what: "source" or "target".
* \param circopt: A pointer to a circeOptions instance.
* \throw a std:string containing the error message (if any).
*/
void scrutinizeFormat(circeOptions *circopt, std::string what)
{
    std::string frmt, vCid, gCid;
    std::size_t found;
    bool bGeocentric = false, bGeographic = false, bProjected = false;
    FILE_FORMAT _ff;
    if (what == "source")
    {
        frmt = circopt->sourceOptions.formatId;
        if (frmt=="")
        {
            // The format is supposed to be inferred from the source data file
            // and will be read/written by OGR
            circopt->sourceOptions.subformatId  = FF_GDAL_OGR;
            return;
        }
        //getIndex returns 0 if frmt not in list.
        _ff=(FILE_FORMAT)getIndex(frmt,FileFormatText,FF_COUNT);
        if (_ff > FF_BASIC)
        {
            circopt->sourceOptions.subformatId  = _ff;
            return;
        }
        gCid = circopt->sourceOptions.geodeticCRSid;
        vCid = circopt->sourceOptions.verticalCRSid;
    }
    else if (what == "target")
    {
        frmt = circopt->targetOptions.formatId;
        if (frmt=="" || frmt==FileFormatText[FF_GDAL_OGR])
        {
            // The format is supposed to be inferred from the source data file
            // and will be read/written by OGR (output written in the same format)
            circopt->targetOptions.formatId  = FF_GDAL_OGR;
            return;
        }
        _ff=(FILE_FORMAT)getIndex(frmt,FileFormatText,FF_COUNT);
        if (_ff > FF_BASIC)
        {
            circopt->targetOptions.subformatId  = _ff;
            return;
        }
        gCid = circopt->targetOptions.geodeticCRSid;
        vCid = circopt->targetOptions.verticalCRSid;
    }

    //FF_BASIC only

    //"XYZ"
    if ( (found=frmt.find_first_of('X')) != std::string::npos )
    {
        if (frmt.substr(found,3) == "XYZ")
            bGeocentric = true;
        else
            throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;
    }
    else if (frmt.find_first_of("YZ") != std::string::npos)
        throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;

    //"L" and "P"
    if (frmt.find_first_of('L') != std::string::npos)
    {
        if (frmt.find_first_of('P') != std::string::npos)
            bGeographic = true;
        else
            throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;
    }
    else if (frmt.find_first_of('P') != std::string::npos)
        throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;

    //"E" and "N"
    if (frmt.find_first_of('E') != std::string::npos)
    {
        if (frmt.find_first_of('N') != std::string::npos)
            bProjected = true;
        else
            throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;
    }
    else if (frmt.find_first_of('N') != std::string::npos)
        throw std::string(ArgumentValidationText[AV_INVALID_GEODETIC_FORMAT])+": "+frmt;

    //"H"
    if (!bGeographic && !bProjected && (frmt.find_first_of('H') != std::string::npos))
        bGeographic = true;

    //Geodetic info consistency
//    if (bGeocentric && bGeographic)
//        return AV_INVALID_GEODETIC_FORMAT;
//    if (bGeocentric && bProjected)
//        return AV_INVALID_GEODETIC_FORMAT;
//    if (bGeographic && bProjected)
//        return AV_INVALID_GEODETIC_FORMAT;
    if (!circopt->getUom && (bGeocentric || bGeographic || bProjected))
    {
        if (gCid == "")
            throw std::string(ArgumentValidationText[AV_GEODETIC_CRS_SHOULD_NOT_BE_UNDEFINED]);
    }

    //For display purpose
    if (what == "source")
    {
        if (bGeocentric) circopt->Pt4d.CRStypeInit = CT_GEOCENTRIC;
        else if (bGeographic) circopt->Pt4d.CRStypeInit = CT_GEOGRAPHIC;
        else if (bProjected) circopt->Pt4d.CRStypeInit = CT_PROJECTED;
    }

    // Initialize geodeticCRStype to get Unity Of Mesure
    if (circopt->getUom)
    {
        if (what == "source")
        {
            if (bGeocentric) circopt->sourceOptions.geodeticCRStype = CT_GEOCENTRIC;
            else if (bGeographic) circopt->sourceOptions.geodeticCRStype = CT_GEOGRAPHIC;
            else if (bProjected) circopt->sourceOptions.geodeticCRStype = CT_PROJECTED;
        }
        else if (what == "target")
        {
            if (bGeocentric) circopt->targetOptions.geodeticCRStype = CT_GEOCENTRIC;
            else if (bGeographic) circopt->targetOptions.geodeticCRStype = CT_GEOGRAPHIC;
            else if (bProjected) circopt->targetOptions.geodeticCRStype = CT_PROJECTED;
        }
    }

    //"V"
    //Vertical info consistency
    if (!circopt->getUom &&  (found=frmt.find_first_of('V')) != std::string::npos )
    {
        if (vCid == "")
            throw std::string(ArgumentValidationText[AV_VERTICAL_CRS_SHOULD_NOT_BE_UNDEFINED]);
    }

    //"CSD" "D"
    if ( (found=frmt.find_first_of('C')) != std::string::npos )
    {
        if (frmt[found+1] == 'S')
            circopt->extraOut = 1;
        else
        {
            circopt->extraOut = 0;
            throw std::string(ArgumentValidationText[AV_INVALID_ARGUMENT])+": "+frmt;
        }
    }
    if ( (found=frmt.find_first_of('D')) != std::string::npos )
        circopt->extraOut = 1;

    return;
}

/**
* \brief Removes the last characters of a format string if they point extra output.
* \param frmt The format string.
* \return The format string without extra output characters.
*/
std::string rmExtraFormat(std::string frmt)
{
    if (frmt.length()<1)
        return frmt;
    if (frmt.substr(frmt.length()-1,frmt.length()) == "D")
        frmt = frmt.substr(0,frmt.length()-1);
    if (frmt.length()<2)
        return frmt;
    if (frmt.substr(frmt.length()-2,frmt.length()) == "CS")
        frmt = frmt.substr(0,frmt.length()-2);
    return frmt;
}

/**
* \brief Validates and completes source and target data formats.
* \param circopt : A pointer to a circeOptions instance.
*/
void scrutinizeOptions(circeOptions *circopt)
{
    scrutinizeFormat(circopt,std::string("source"));
    scrutinizeFormat(circopt,std::string("target"));
}

/**
* \brief Parses the coordinates from a commande line (associated with a basic data format).
* \param[in] argc: size of argv.
* \param[in] argv: C-array of arguments.
* \param circopt: A pointer to a circeOptions instance.
* \param iargv: Current reading position in argv.
* \param[out] idt: Current reading position in data format.
* \throw a std:string containing the error message (if any).
*/
void parsePt4dCoordinates(int argc, char **argv, circe::circeOptions *circopt, int *iargv, int *idt)
{
    std::string argTracking = "";
    *idt=0;
    while (*iargv<argc)
    {
        std::istringstream ss(argv[*iargv]);
        argTracking += " "+ss.str();
        switch (circopt->sourceOptions.formatId[*idt])
        {
        case 'I':
            ss >> circopt->Pt4d.id;
            break;
        case 'X':
            ss >> circopt->Pt4d.x;
            break;
        case 'Y':
            ss >> circopt->Pt4d.y;
            break;
        case 'Z':
            ss >> circopt->Pt4d.z;
            break;
        case 'L':
            ss >> circopt->Pt4d.l;
            break;
        case 'P':
            ss >> circopt->Pt4d.p;
            break;
        case 'H':
            ss >> circopt->Pt4d.h;
            break;
        case 'E':
            ss >> circopt->Pt4d.e;
            break;
        case 'N':
            ss >> circopt->Pt4d.n;
            break;
        case 'V':
            ss >> circopt->Pt4d.w;
            break;
        default:
            throw std::string(ArgumentValidationText[AV_INVALID_OPTION_ARGUMENT])+": "+argTracking;
            break;
        }
        if (ss.fail())
            throw std::string(ArgumentValidationText[AV_INVALID_OPTION_ARGUMENT])+": "+argTracking;
        (*iargv)++;
        if (++(*idt) > circopt->sourceOptions.formatId.size())
            throw std::string(ArgumentValidationText[AV_BAD_COORDINATE_INPUT])+": "+argTracking;
    }
    if (*idt < circopt->sourceOptions.formatId.size())
        throw std::string(ArgumentValidationText[AV_BAD_COORDINATE_INPUT])+": "+argTracking;
    return;
}

/**
* \brief Parses the argument array from a commande line.
* \param[in] argc: size of argv.
* \param[in] argv: C-array of arguments.
* \param circopt: A pointer to a circeOptions instance.
* \param[out] iargv: Current reading position in argv.
* \throw a std:string containing the error message (if any).
*/
void parseArgArray(circe::circeOptions *circopt, int argc, char **argv, int *iargv)
{
    //char *arg='\0';
    char arg[_MAX_PATH_];
	strcpy(arg,"\0");
    assert(sizeof(circeArgumentDefinitions));
    int iopt, nopt=sizeof(circeArgumentDefinitions)/sizeof(*circeArgumentDefinitions), key, idt;
    size_t le;
    *iargv=0;
    initDefOptions(circopt);
    while (++(*iargv)<argc)
    {
        key = 0;
        if (strncmp(argv[*iargv],"--",2)==0)
        {
            //long option name
            for (iopt=0;iopt<nopt;iopt++)
                if (strncmp(argv[*iargv]+2,circeArgumentDefinitions[iopt].name,strlen(circeArgumentDefinitions[iopt].name))==0)
                    break;
            if (iopt==nopt)
                throw std::string(ArgumentValidationText[AV_INVALID_LONG_OPTION_NAME])+": "+std::string(argv[*iargv]);
            key = circeArgumentDefinitions[iopt].key;
            le = strlen(circeArgumentDefinitions[iopt].name);
            if (circeArgumentDefinitions[iopt].arg)
            {
                //long option name with an argument separated by a "=" without spaces
                if (strlen(argv[*iargv]) == le+3)
                    throw std::string(ArgumentValidationText[AV_INVALID_OPTION_WITHOUT_ARGUMENT])+": "+std::string(argv[*iargv]);
                if (argv[*iargv][le+2] != '=')
                    throw std::string(ArgumentValidationText[AV_INVALID_LONG_OPTION_NAME])+": "+std::string(argv[*iargv]);
                strcpy(arg, argv[*iargv]+le+3);
            }
        }
        else if (argv[*iargv][0]=='-')
        {
            //short option name
            for (iopt=0;iopt<nopt;iopt++)
                if (argv[*iargv][1]==circeArgumentDefinitions[iopt].key)
                    break;
            if (iopt==nopt)
            {
                // It may be a negative coordinate.
                // Try and hope ...
                parsePt4dCoordinates(argc, argv, circopt, iargv, &idt);
                return;
//                return AV_INVALID_SHORT_OPTION_NAME;
            }
            key = circeArgumentDefinitions[iopt].key;
            if (circeArgumentDefinitions[iopt].arg)
            {
                //short option name with an argument ...
                if (strlen(argv[*iargv])>2)
                //...without a separating space
                    strcpy(arg, argv[*iargv]+2);
                else
                {
                    //...with a separating space
                    if (++(*iargv)==argc)
                        throw std::string(ArgumentValidationText[AV_INVALID_OPTION_WITHOUT_ARGUMENT])+": "+std::string(argv[--(*iargv)]);
                    strcpy(arg, argv[*iargv]);
                }
            }
            //multiple short option names without argument:
            //Unlike POSIX recommendations, ‘-abc’ is NOT equivalent to ‘-a -b -c’
            //but it could be ! (to be implemented)
        }
        else
        //The end of the command line may include input point coordinates.
        //The source basic format must have been defined.
        {
            parsePt4dCoordinates(argc, argv, circopt, iargv, &idt);
            return;
        }
        parseOptions (key, arg, circopt);
    }
    return;
}

/**
* \brief Parses a commande line as a std::string.
* \param[in] definitionLine: a commande line as a std::string.
* \param circopt: A pointer to a circeOptions instance.
*/
void parseArgString(std::string definitionLine, circeOptions *circopt)
{
    std::istringstream str(definitionLine);
    std::istream_iterator<std::string> beg(str), end;
    std::vector<std::string> tokens(beg, end);

    int argc=tokens.size()+1, iargv;
    char **argv;
    argv=(char**)malloc(argc*sizeof(char*));
    argv[0]=(char*)malloc(sizeof(char));
    for(unsigned int i=0;i<tokens.size();i++)
    {
        argv[i+1]=(char*)malloc((tokens[i].length()+1)*sizeof(char));
        strcpy(argv[i+1],tokens[i].data());
    }
    parseArgArray(circopt, argc, argv, &iargv);
    for(int i=0;i<argc;i++) free(argv[i]);
    free(argv);
//    if (av != AV_OK)
//        throw (displayInvalidArgument(argv, iargv, av));
    scrutinizeFormat(circopt,std::string("source"));
	initFormats(circopt->displayPrecision, circopt->targetOptions.angularUnit, &(circopt->Pt4d.frmt));
    return;
}

/**
* \brief Loads options from a XML file.
* \param[in] circeOptionFile: the Circe option file path.
* \param circopt: A pointer to a circeOptions instance.
* \throw a std:string containing the error message (if any).
*/
std::string readXMLOptionFile(circeOptions *circopt, std::string circeOptionFile) /*throw(std::string)*/
{
    std::ofstream fo;
	openLog(&fo, circopt->logPathname);
	try
    {
        initDefOptions(circopt);
        std::string message;
        circopt->optionFile = circeOptionFile;

        std::string XMLfilePath=circeOptionFile;
        struct stat buffer;
        if (stat (XMLfilePath.c_str(), &buffer) != 0)
            throw circe::ErrorMessageText[circe::EM_CANNOT_OPEN]+"\n"+XMLfilePath;

        tinyxml2::XMLDocument XMLdoc;
        tinyxml2::XMLElement *_node;
        tinyxml2::XMLError err_load = XMLdoc.LoadFile(XMLfilePath.data());
        if (err_load)
        {
    //            std::CO->t << std::string(":::")+circe::DataValidationText[DV_LOADING_ERROR]+":::\n";
            message.assign(XMLdoc.ErrorName());
            throw message;
        }

        if (!(_node = XMLdoc.FirstChildElement("CIRCE_OPTIONS")))
            throw std::string("CIRCE_OPTIONS ")+circe::DataValidationText[circe::DV_TAG_NOT_FOUND];
        std::string foo;

        circopt->appTitle = circe::ReadString(_node,"appTitle");
        circopt->appCopyright = circe::ReadString(_node,"appCopyright");
        circopt->appVersion = circe::ReadString(_node,"appVersion");
        circopt->appDescription = circe::ReadString(_node,"appDescription");
        circopt->appHelp = circe::ReadString(_node,"appHelp");
        circopt->appStyle = circe::ReadString(_node,"appStyle");
        circopt->appUserManual = circe::ReadString(_node,"appUserManual");
        circopt->sourceOptions.pathname = circe::ReadString(_node,"sourceDataPathname");
        circopt->sourceOptions.geodeticFrameId = circe::ReadString(_node,"sourceGeodeticFrameId");
        circopt->sourceOptions.verticalFrameId = circe::ReadString(_node,"sourceVerticalFrameId");
        circopt->sourceOptions.geodeticCRSid = circe::ReadString(_node,"sourceGeodeticCRSid");
        circopt->sourceOptions.verticalCRSid = circe::ReadString(_node,"sourceVerticalCRSid");
        circopt->sourceOptions.geodeticCRStype = circe::CRS_TYPE(circe::ReadInt(_node,"sourceGeodeticCRStype"));
        circopt->sourceOptions.angularUnit = circe::ReadString(_node,"sourceDataUnit");
        circopt->sourceOptions.formatId = circe::ReadString(_node,"sourceDataFormat");
        circopt->sourceOptions.epoch = circe::ReadDouble(_node,"sourceEpoch",&foo);
        circopt->sourceOptions.verticalFrameAuthoritative = circe::ReadBool(_node,"sourceVerticalFrameAuthoritative");
        circopt->sourceOptions.useGeodeticCRS = circe::ReadBool(_node,"useSourceGeodeticCRS");
        circopt->sourceOptions.useVerticalCRS = circe::ReadBool(_node,"useSourceVerticalCRS");
        circopt->targetOptions.pathname = circe::ReadString(_node,"targetDataPathname");
        circopt->targetOptions.geodeticFrameId = circe::ReadString(_node,"targetGeodeticFrameId");
        circopt->targetOptions.verticalFrameId = circe::ReadString(_node,"targetVerticalFrameId");
        circopt->targetOptions.geodeticCRSid = circe::ReadString(_node,"targetGeodeticCRSid");
        circopt->targetOptions.verticalCRSid = circe::ReadString(_node,"targetVerticalCRSid");
        circopt->targetOptions.geodeticCRStype = circe::CRS_TYPE(circe::ReadInt(_node,"targetGeodeticCRStype"));
        circopt->targetOptions.angularUnit = circe::ReadString(_node,"targetDataUnit");
        circopt->targetOptions.formatId = circe::ReadString(_node,"targetDataFormat");
        circopt->targetOptions.epoch = circe::ReadDouble(_node,"targetEpoch",&foo);
        circopt->targetOptions.verticalFrameAuthoritative = circe::ReadBool(_node,"targetVerticalFrameAuthoritative");
        circopt->targetOptions.useGeodeticCRS = circe::ReadBool(_node,"useTargetGeodeticCRS");
        circopt->targetOptions.useVerticalCRS = circe::ReadBool(_node,"useTargetVerticalCRS");
        circopt->idGeodeticTransfoRequired = circe::ReadString(_node,"geodeticTransfoId");
        circopt->idVerticalTransfoRequired = circe::ReadString(_node,"verticalTransfoId");
		circopt->metadataFile = circe::ReadString(_node, "metadataFile");
		circopt->boundaryFile = circe::ReadString(_node, "boundaryFile");
        circopt->logoFilename = circe::ReadString(_node, "logoFilename");
        circopt->imageFilename = circe::ReadString(_node, "imageFilename");
        circopt->iconFilename = circe::ReadString(_node, "iconFilename");
        circopt->menuTitle_help = circe::ReadString(_node, "menuTitle_help");
        circopt->menuTitle_tool = circe::ReadString(_node, "menuTitle_tool");
        circopt->menuTitle_zone = circe::ReadString(_node, "menuTitle_zone");
        circopt->menuTitle_display = circe::ReadString(_node, "menuTitle_display");
        circopt->dataMode = circe::DATA_MODE(circe::ReadInt(_node,"dataMode"));
        circopt->printCS = circe::ReadBool(_node,"printCS");
        circopt->displayBoundingBox = circe::ReadBool(_node,"displayBoundingBox");
        circopt->noPrintSppm = circe::ReadBool(_node,"noPrintSppm");
        circopt->noUseVerticalFrameAuthoritative = circe::ReadBool(_node,"noUseVerticalFrameAuthoritative");
        circopt->scrollArea = circe::ReadBool(_node,"scrollArea");
        circopt->plainDMS = circe::ReadBool(_node,"plainDMS");
        circopt->displayPrecision = circe::ReadDouble(_node, "displayPrecision", &foo);
        circopt->separator = circe::ReadString(_node, "separator");
        circopt->Pt4d.id = circe::ReadString(_node,"id");
        circopt->Pt4d.x = circe::ReadDouble(_node,"X",&foo);
        circopt->Pt4d.y = circe::ReadDouble(_node,"Y",&foo);
        circopt->Pt4d.z = circe::ReadDouble(_node,"Z",&foo);
        circopt->Pt4d.l = circe::ReadDouble(_node,"L",&foo);
        circopt->Pt4d.p = circe::ReadDouble(_node,"P",&foo);
        circopt->Pt4d.h = circe::ReadDouble(_node,"H",&foo);
        circopt->Pt4d.e = circe::ReadDouble(_node,"E",&foo);
        circopt->Pt4d.n = circe::ReadDouble(_node,"N",&foo);
        circopt->Pt4d.w = circe::ReadDouble(_node,"V",&foo);
        initFormats(circopt->displayPrecision, circopt->targetOptions.angularUnit, &(circopt->Pt4d.frmt));
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
* \brief Writes options to a XML file.
* \param[in] circopt: A circeOptions instance.
* \throw a std:string containing the error message (if any).
*/
std::string writeXMLOptionFile(const circeOptions& circopt) /*throw(std::string)*/
{
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
        std::ofstream fifo(circopt.optionFile.data());
        fifo.imbue( std::locale::classic() ) ; //no thousand separator
        fifo << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        fifo << "<CIRCE_OPTIONS>\n";
        fifo << "\t<appTitle>" << circopt.appTitle << "</appTitle>\n";
        fifo << "\t<appCopyright>" << circopt.appCopyright << "</appCopyright>\n";
        fifo << "\t<appVersion>" << circopt.appVersion << "</appVersion>\n";
        fifo << "\t<appDescription>" << circopt.appDescription << "</appDescription>\n";
        fifo << "\t<appHelp>" << circopt.appHelp << "</appHelp>\n";
        fifo << "\t<appStyle>" << circopt.appStyle << "</appStyle>\n";
        fifo << "\t<appUserManual>" << circopt.appUserManual << "</appUserManual>\n";
        fifo << "\t<dataMode>" << circopt.dataMode << "</dataMode>\n";
        fifo << "\t<logoFilename>" << circopt.logoFilename << "</logoFilename>\n";
        fifo << "\t<imageFilename>" << circopt.imageFilename << "</imageFilename>\n";
        fifo << "\t<iconFilename>" << circopt.iconFilename << "</iconFilename>\n";
        fifo << "\t<sourceDataPathname>" << circopt.sourceOptions.pathname << "</sourceDataPathname>\n";
        fifo << "\t<targetDataPathname>" << circopt.targetOptions.pathname << "</targetDataPathname>\n";
		fifo << "\t<metadataFile>" << circopt.metadataFile << "</metadataFile>\n";
		fifo << "\t<boundaryFile>" << circopt.boundaryFile << "</boundaryFile>\n";
        fifo << "\t<menuTitle_help>" << circopt.menuTitle_help << "</menuTitle_help>\n";
        fifo << "\t<menuTitle_tool>" << circopt.menuTitle_tool << "</menuTitle_tool>\n";
        fifo << "\t<menuTitle_zone>" << circopt.menuTitle_zone << "</menuTitle_zone>\n";
        fifo << "\t<menuTitle_display>" << circopt.menuTitle_display << "</menuTitle_display>\n";
        fifo << "\t<sourceGeodeticFrameId>" << circopt.sourceOptions.geodeticFrameId << "</sourceGeodeticFrameId>\n";
        fifo << "\t<sourceVerticalFrameId>" << circopt.sourceOptions.verticalFrameId << "</sourceVerticalFrameId>\n";
        fifo << "\t<targetGeodeticFrameId>" << circopt.targetOptions.geodeticFrameId << "</targetGeodeticFrameId>\n";
        fifo << "\t<targetVerticalFrameId>" << circopt.targetOptions.verticalFrameId << "</targetVerticalFrameId>\n";
        fifo << "\t<sourceGeodeticCRSid>" << circopt.sourceOptions.geodeticCRSid << "</sourceGeodeticCRSid>\n";
        fifo << "\t<sourceVerticalCRSid>" << circopt.sourceOptions.verticalCRSid << "</sourceVerticalCRSid>\n";
        fifo << "\t<targetGeodeticCRSid>" << circopt.targetOptions.geodeticCRSid << "</targetGeodeticCRSid>\n";
        fifo << "\t<targetVerticalCRSid>" << circopt.targetOptions.verticalCRSid << "</targetVerticalCRSid>\n";
        fifo << "\t<geodeticTransfoId>" << circopt.idGeodeticTransfoRequired << "</geodeticTransfoId>\n";
        fifo << "\t<verticalTransfoId>" << circopt.idVerticalTransfoRequired << "</verticalTransfoId>\n";
        fifo << "\t<sourceGeodeticCRStype>" << circopt.sourceOptions.geodeticCRStype << "</sourceGeodeticCRStype>\n";
        fifo << "\t<targetGeodeticCRStype>" << circopt.targetOptions.geodeticCRStype << "</targetGeodeticCRStype>\n";
        fifo << "\t<sourceDataFormat>" << circopt.sourceOptions.formatId << "</sourceDataFormat>\n";
        fifo << "\t<sourceEpoch>" << asString(circopt.sourceOptions.epoch,4) << "</sourceEpoch>\n";
        fifo << "\t<targetDataFormat>" << circopt.targetOptions.formatId << "</targetDataFormat>\n";
        fifo << "\t<targetEpoch>" << asString(circopt.targetOptions.epoch,4) << "</targetEpoch>\n";
        fifo << "\t<sourceDataUnit>" << circopt.sourceOptions.angularUnit << "</sourceDataUnit>\n";
        fifo << "\t<targetDataUnit>" << circopt.targetOptions.angularUnit << "</targetDataUnit>\n";
        fifo << "\t<sourceVerticalFrameAuthoritative>" << circopt.sourceOptions.verticalFrameAuthoritative << "</sourceVerticalFrameAuthoritative>\n";
        fifo << "\t<targetVerticalFrameAuthoritative>" << circopt.targetOptions.verticalFrameAuthoritative << "</targetVerticalFrameAuthoritative>\n";
        fifo << "\t<useSourceGeodeticCRS>" << circopt.sourceOptions.useGeodeticCRS << "</useSourceGeodeticCRS>\n";
        fifo << "\t<useTargetGeodeticCRS>" << circopt.targetOptions.useGeodeticCRS << "</useTargetGeodeticCRS>\n";
        fifo << "\t<useSourceVerticalCRS>" << circopt.sourceOptions.useVerticalCRS << "</useSourceVerticalCRS>\n";
        fifo << "\t<useTargetVerticalCRS>" << circopt.targetOptions.useVerticalCRS << "</useTargetVerticalCRS>\n";
        fifo << "\t<printCS>" << circopt.printCS << "</printCS>\n";
        fifo << "\t<displayBoundingBox>" << circopt.displayBoundingBox << "</displayBoundingBox>\n";
        fifo << "\t<noPrintSppm>" << circopt.noPrintSppm << "</noPrintSppm>\n";
        fifo << "\t<noUseVerticalFrameAuthoritative>" << circopt.noUseVerticalFrameAuthoritative << "</noUseVerticalFrameAuthoritative>\n";
        fifo << "\t<scrollArea>" << circopt.scrollArea << "</scrollArea>\n";
        fifo << "\t<plainDMS>" << circopt.plainDMS << "</plainDMS>\n";
        fifo << "\t<displayPrecision>" << circe::asString(circopt.displayPrecision) << "</displayPrecision>\n";
        fifo << "\t<separator>" << circopt.separator << "</separator>\n";
        fifo << "\t<id>" << circopt.Pt4d.id << "</id>\n";
        fifo << "\t<X>" << circe::asString(circopt.Pt4d.x,4) << "</X>\n";
        fifo << "\t<Y>" << circe::asString(circopt.Pt4d.y,4) << "</Y>\n";
        fifo << "\t<Z>" << circe::asString(circopt.Pt4d.z,4) << "</Z>\n";
        fifo << "\t<L>" << circe::asString(circopt.Pt4d.l,10) << "</L>\n";
        fifo << "\t<P>" << circe::asString(circopt.Pt4d.p,10) << "</P>\n";
        fifo << "\t<H>" << circe::asString(circopt.Pt4d.h,4) << "</H>\n";
        fifo << "\t<E>" << circe::asString(circopt.Pt4d.e,4) << "</E>\n";
        fifo << "\t<N>" << circe::asString(circopt.Pt4d.n,4) << "</N>\n";
        fifo << "\t<V>" << circe::asString(circopt.Pt4d.w,4) << "</V>\n";
        fifo << "</CIRCE_OPTIONS>\n";
        fifo.close();
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
* \brief Converts a single-char string separator into a character.
* \param[in] sep: A string separator.
* \return A separator as a char.
*/
char getSeparator(std::string sep)
{
    for (int ifs=0;ifs<FS_END;ifs++)
        if (FieldSeparatorText[ifs] == sep)
            sep = SeparatorValues[ifs];
    if (sep=="" || sep==" " || sep=="_")
        return ' ';
    else if (sep=="\\t")
        return '\t';
    else
        return sep[0];
}

/**
* \brief Writes a command line string.
* \param[in] circopt: A circeOptions instance.
* \return A command line as a  std::string.
* \throw a std:string containing the error message (if any).
* \remark
Command line examples:
- \-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=EN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.txt" \-\-sourcePathname="./dafile.txt" \-\-targetPathname="./outfile.txt"
- \-\-sourceCRS=NTFLAMB3. \-\-sourceFormat=EN.METERS. \-\-targetCRS=WGS84G. \-\-targetFormat=LP.METERS.DEGREES \-\-metadataFile="data/DataFRnew.txt"  470000 160000
 */
std::string writeCommandLine(const circeOptions& circopt) /*throw(std::string)*/
{
    std::string s="", sgci="", tgci="", svci="", tvci="";
	std::ofstream fo;
	openLog(&fo, circopt.logPathname);
	try
    {
		s += "--metadataFile=" + circopt.metadataFile + " ";
		if (circopt.boundaryFile != "")
			s += "--boundaryFile=" + circopt.boundaryFile + " ";
        if (circopt.logPathname != "")
            s += "--logPathname=" + circopt.logPathname + " ";
		if (circopt.sourceOptions.useGeodeticCRS) sgci = circopt.sourceOptions.geodeticCRSid;
        if (circopt.sourceOptions.useVerticalCRS)
        {
            if (circopt.sourceOptions.verticalFrameAuthoritative) svci = "AUTHORITATIVE";
            else svci = circopt.sourceOptions.verticalCRSid;
        }
        if (circopt.targetOptions.useGeodeticCRS) tgci = circopt.targetOptions.geodeticCRSid;
        if (circopt.targetOptions.useVerticalCRS)
        {
            if (circopt.targetOptions.verticalFrameAuthoritative) tvci = "AUTHORITATIVE";
            else tvci = circopt.targetOptions.verticalCRSid;
        }
        s += "--sourceCRS="+sgci+"."+svci+" ";
        s += "--sourceFormat="+circopt.sourceOptions.formatId+"."+circopt.sourceOptions.linearUnit+"."+circopt.sourceOptions.angularUnit+" ";
        if (circopt.sourceOptions.epoch != 0.) s += "--sourceEpoch="+asString(circopt.sourceOptions.epoch,4)+" ";
        s += "--targetCRS="+tgci+"."+tvci+" ";
        s += "--targetFormat="+circopt.targetOptions.formatId+"."+circopt.targetOptions.linearUnit+"."+circopt.targetOptions.angularUnit+" ";
        if (circopt.targetOptions.epoch != 0.) s += "--targetEpoch="+asString(circopt.targetOptions.epoch,4)+" ";
        if (circopt.idGeodeticTransfoRequired != "" || circopt.idVerticalTransfoRequired != "")
        {
            s += "--transfoRequired="+circopt.idGeodeticTransfoRequired+"."+circopt.idVerticalTransfoRequired+" ";
        }
        s += "--displayPrecision=" + asString(circopt.displayPrecision) + " ";
        if (getSeparator(circopt.separator) != ' ')
            s += "--separator=" + circopt.separator + " ";
        if (circopt.plainDMS)
            s += "--plainDMS ";
        if (circopt.noPrintSppm)
            s += "--noPrintSppm ";
        s += "--gridLoading=BINARY ";
        switch(circopt.dataMode)
        {
            case circe::DM_POINT:
            //s += "--gridLoading=BINARY ";
            for (int ifrmt=0; ifrmt<circopt.sourceOptions.formatId.size(); ifrmt++)
            {
                switch (circopt.sourceOptions.formatId[ifrmt])
                {
                case 'I':
                    s += circopt.Pt4d.id+" ";
                    break;
                case 'X':
                    s += circe::asString(circopt.Pt4d.x,4)+" ";
                    break;
                case 'Y':
                    s += circe::asString(circopt.Pt4d.y,4)+" ";
                    break;
                case 'Z':
                    s += circe::asString(circopt.Pt4d.z,4)+" ";
                    break;
                case 'L':
                    s += circe::asString(circopt.Pt4d.l,10)+" ";
                    break;
                case 'P':
                    s += circe::asString(circopt.Pt4d.p,10)+" ";
                    break;
                case 'H':
                    s += circe::asString(circopt.Pt4d.h,4)+" ";
                    break;
                case 'E':
                    s += circe::asString(circopt.Pt4d.e,4)+" ";
                    break;
                case 'N':
                    s += circe::asString(circopt.Pt4d.n,4)+" ";
                    break;
                case 'V':
                    s += circe::asString(circopt.Pt4d.w,4)+" ";
                    break;
                default:
                    break;
                }
            }
            break;
            case circe::DM_FILE:
                s += "--sourcePathname="+replaceChar(circopt.sourceOptions.pathname,' ','@')+" ";
                s += "--targetPathname="+replaceChar(circopt.targetOptions.pathname,' ','@')+" ";
            break;
            default:
            break;
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
    return s;
}

std::string displayOutput(const circeOptions& circopt, const std::string& str,
                          const std::string& brckt1, const std::string& brckt2, bool bWriteMessage)
{
    if (circopt.outputFormat == "JSON")
    {
        if (bWriteMessage)
            return "{\n"+displayOptionsJSON(circopt)+"\"output\":"+brckt1+"\"message\":\""+str+"\""+brckt2+"\n}";
        else
            return "{\n"+displayOptionsJSON(circopt)+"\"output\":"+brckt1+str+brckt2+"\n}";
    }
    else
        return str;
}

} //namespace circe
