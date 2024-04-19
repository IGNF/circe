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
* \file transformation.cpp
* \brief Implementation of the Transformation class.
*/
#include <algorithm>  // std::transform
#include <sstream> //opérateurs binaires sur std::string
#include <cmath>
#include <ctime> //_strdate
#include <sys/stat.h>
#include <iomanip> //setw
#include "transformation.h"
#include "metadata.h"
#include "conversioncore.h" //M_PI, GrandeNormale, RMer, CartGeo56, GeoCart
//#include <QDebug>

namespace circe
{

double NINT(double a)
{
    if (a>0) return floor(a+0.5);
    else if (a<0) return ceil(a-0.5);
    else return 0;
}

/**
* \brief Constructor.
*/
Transformation::Transformation()
{
    mb = NULL;
    //reset();
}

/**
* \brief Empties values of the members of a Transformation instance.
*/
void Transformation::reset()
{
    Operation::reset();
    mb = NULL;
    // Internal management
//    fin     = NULL;
    Encoding	=	ET_UNDEF;
    //Fichier 		=	NULL;
    Offset			=	0;
    LoadingType         = LD_UNDEF;

    // General data
    GridType		=	GF_UNDEF;
    GridFile = "UNDEFINED";
    MetadadaFileName = "UNDEFINED";
    Origin = "UNDEFINED";
    Created = "UNDEFINED";
    Version = "UNDEFINED";
    Keyword = "UNDEFINED";
    Description	= "UNDEFINED";
	FollowUp = "";

    // Node data
    Layout		=	L_UNDEF;
    N_nodes			=	0;
    N_col			=	0;
    N_lines			=	0;
    SN_inc			=	0.;
    WE_inc			=	0.;
    WestBound			=	0.;
    EastBound			=	0.;
    SouthBound			=	0.;
    NorthBound			=	0.;
    NodeCRStype	=	CT_UNDEF;
    IncCoordUnit		=	MU_DEGREE;
    NodeIncCoordUnit	=	MU_DEGREE;
    bNodeCoordWritten	=	false;
    bPrecisionCode		=	false;
    bPositiveWest			=	false;

    // Value data
    //InitValues[0]	=	0.;
    crs_type = CT_UNDEF;
    Epoch = 0.;
    PrecisionCodeInit = 0;
    bDelimiter 	=	1;
    bPixelCenter	=	0;
    bFinalLineNumber	=	0;
    N_val			=	0;
    ValueType			=	0;
    ValueSize		=	0;
    NodeSize            =   0;
    UnknownValue		=	0.;
    bUnknownValue2zero	=	0;
    //Translation[0]	=	0.;
    RateFactor          =   1.;
    MinValue			=	0.;
    MaxValue			=	0.;
    MeanValue			=	0.;
    ValueUnit		=	MU_METER;
    InterpolationType = P_UNDEF;
    InterpolationOrder =	0;

    //Geodetic systems
    SourceFrameId	=	"";
    TargetFrameId	=	"";
    NodeFrameId     =   "";
    NodeFrameInfo	=	F_UNDEF;
    Af				=	0.;
    Bf				=	0.;
    E2f				=	0.;
    EpochF          =   0.;
    At				=	0.;
    Bt				=	0.;
    E2t				=	0.;
    EpochT          =   0.;
}

/**
* \brief Copy constructor.
*/
Transformation::Transformation(const Transformation &B) : Operation(B)
{
//    fin = B.fin;
    ReadElementI4 = B.ReadElementI4;
    ReadElementR8 = B.ReadElementR8;
    ReadElementBin = B.ReadElementBin;
    ReadElementTab = B.ReadElementTab;
    WriteElementI4 = B.WriteElementI4;
    WriteElementR8 = B.WriteElementR8;
    WriteElementBin = B.WriteElementBin;
    WriteElementTab = B.WriteElementTab;
    LoadingType = B.LoadingType;
    //fin and fout not copied out here because no need for copy constrcutor when
    //files are open.
    ReadElementTab = B.ReadElementTab;
    GridFile = B.GridFile;
    MetadadaFileName = B.MetadadaFileName;
    //if (B.fin.is_open()) fin.open(GridFile);
    //if (B.fout.is_open()) fout.open(GridFile);
    Origin		=	B.Origin;
    Created		=	B.Created;
    Version			=	B.Version;
    Keyword			=	B.Keyword;
    GridType		=	B.GridType;
    NodeCRStype	=	B.NodeCRStype;
    InterpolationType =	B.InterpolationType;
    ValueUnit		=	B.ValueUnit;
    IncCoordUnit		=	B.IncCoordUnit;
    NodeIncCoordUnit	=	B.NodeIncCoordUnit;
    Encoding	=	B.Encoding;
    bNodeCoordWritten	=	B.bNodeCoordWritten;
    bPrecisionCode		=	B.bPrecisionCode;
    bPositiveWest			=	B.bPositiveWest;
    bPixelCenter	=	B.bPixelCenter;
    crs_type        = B.crs_type;
    Epoch           = B.Epoch;
    PrecisionCodeInit= B.PrecisionCodeInit;
    bDelimiter		=	B.bDelimiter;
    bFinalLineNumber	=	B.bFinalLineNumber;
    Description		=	B.Description;
	FollowUp	= B.FollowUp;
    InterpolationOrder =	B.InterpolationOrder;
    N_nodes			=	B.N_nodes;
    N_col			=	B.N_col;
    N_lines			=	B.N_lines;
    N_val			=	B.N_val;
    ValueType			=	B.ValueType;
    ValueSize		=	B.ValueSize;
    NodeSize        =   B.NodeSize;
    UnknownValue		=	B.UnknownValue;
    bUnknownValue2zero	=	B.bUnknownValue2zero;
    Offset			=	B.Offset;
    Layout		=	B.Layout;
    SN_inc			=	B.SN_inc;
    WE_inc			=	B.WE_inc;
    WestBound			=	B.WestBound;
    EastBound			=	B.EastBound;
    SouthBound			=	B.SouthBound;
    NorthBound			=	B.NorthBound;
    RateFactor          =   B.RateFactor;
    MinValue			=	B.MinValue;
    MaxValue			=	B.MaxValue;
    MeanValue			=	B.MeanValue;
    SourceFrameId	=	B.SourceFrameId;
    TargetFrameId	=	B.TargetFrameId;
    NodeFrameId     =	B.NodeFrameId;
    NodeFrameInfo	=	B.NodeFrameInfo;
    Af				=	B.Af;
    Bf				=	B.Bf;
    E2f				=	B.E2f;
    EpochF          =   B.EpochF;
    At				=	B.At;
    Bt				=	B.Bt;
    E2t				=	B.E2t;
    EpochT          =   B.EpochT;

    InitValues.reserve(B.InitValues.size());
    copy(B.InitValues.begin(),B.InitValues.end(),back_inserter(InitValues));

    Translation.reserve(B.Translation.size());
    copy(B.Translation.begin(),B.Translation.end(),back_inserter(Translation));
    if (Encoding == ET_UNDEF || Encoding == ET_ASCII)
    {
        switch (ValueType)
        {
            case 1:
                VecValI2.reserve(B.VecValI2.size());
                copy(B.VecValI2.begin(),B.VecValI2.end(),back_inserter(VecValI2));
                break;
            case 2:
                VecValI4.reserve(B.VecValI4.size());
                copy(B.VecValI4.begin(),B.VecValI4.end(),back_inserter(VecValI4));
                break;
            case 3:
                VecValR4.reserve(B.VecValR4.size());
                copy(B.VecValR4.begin(),B.VecValR4.end(),back_inserter(VecValR4));
                break;
            case 4:
                VecValR8.reserve(B.VecValR8.size());
                copy(B.VecValR8.begin(),B.VecValR8.end(),back_inserter(VecValR8));
                break;
            default:
                break;
        }
        VecPrec.reserve(B.VecPrec.size());
        copy(B.VecPrec.begin(),B.VecPrec.end(),back_inserter(VecPrec));
    }
}


/**
* \brief Copy operator.
*/
Transformation& Transformation::operator=(const Transformation &B)
{
//    fin = B.fin;
    ReadElementI4 = B.ReadElementI4;
    ReadElementR8 = B.ReadElementR8;
    ReadElementBin = B.ReadElementBin;
    ReadElementTab = B.ReadElementTab;
    WriteElementI4 = B.WriteElementI4;
    WriteElementR8 = B.WriteElementR8;
    WriteElementBin = B.WriteElementBin;
    WriteElementTab = B.WriteElementTab;
    LoadingType = B.LoadingType;
    GridFile = B.GridFile;
    MetadadaFileName = B.MetadadaFileName;
    //if (B.fin.is_open()) fin.open(GridFile);
    //if (B.fout.is_open()) fout.open(GridFile);
    Origin		=	B.Origin;
    Created		=	B.Created;
    Version			=	B.Version;
    Keyword			=	B.Keyword;
    GridType		=	B.GridType;
    NodeCRStype	=	B.NodeCRStype;
    InterpolationType =	B.InterpolationType;
    ValueUnit		=	B.ValueUnit;
    IncCoordUnit		=	B.IncCoordUnit;
    NodeIncCoordUnit	=	B.NodeIncCoordUnit;
    Encoding	=	B.Encoding;
    bNodeCoordWritten	=	B.bNodeCoordWritten;
    bPrecisionCode		=	B.bPrecisionCode;
    bPositiveWest			=	B.bPositiveWest;
    bPixelCenter	=	B.bPixelCenter;
    crs_type        = B.crs_type;
    Epoch           = B.Epoch;
    PrecisionCodeInit= B.PrecisionCodeInit;
    bDelimiter		=	B.bDelimiter;
    bFinalLineNumber	=	B.bFinalLineNumber;
    Description		=	B.Description;
	FollowUp	= B.FollowUp;
    InterpolationOrder =	B.InterpolationOrder;
    N_nodes			=	B.N_nodes;
    N_col			=	B.N_col;
    N_lines			=	B.N_lines;
    N_val			=	B.N_val;
    ValueType			=	B.ValueType;
    ValueSize		=	B.ValueSize;
    NodeSize        =   B.NodeSize;
    UnknownValue		=	B.UnknownValue;
    bUnknownValue2zero	=	B.bUnknownValue2zero;
    Offset			=	B.Offset;
    Layout		=	B.Layout;
    SN_inc			=	B.SN_inc;
    WE_inc			=	B.WE_inc;
    WestBound			=	B.WestBound;
    EastBound			=	B.EastBound;
    SouthBound			=	B.SouthBound;
    NorthBound			=	B.NorthBound;
    RateFactor          =   B.RateFactor;
    MinValue			=	B.MinValue;
    MaxValue			=	B.MaxValue;
    MeanValue			=	B.MeanValue;
    SourceFrameId	=	B.SourceFrameId;
    TargetFrameId	=	B.TargetFrameId;
    NodeFrameId     =	B.NodeFrameId;
    NodeFrameInfo	=	B.NodeFrameInfo;
    Af				=	B.Af;
    Bf				=	B.Bf;
    E2f				=	B.E2f;
    EpochF          =   B.EpochF;
    At				=	B.At;
    Bt				=	B.Bt;
    E2t				=	B.E2t;
    EpochT          =   B.EpochT;

    InitValues.reserve(B.InitValues.size());
    copy(B.InitValues.begin(),B.InitValues.end(),back_inserter(InitValues));

    Translation.reserve(B.Translation.size());
    copy(B.Translation.begin(),B.Translation.end(),back_inserter(Translation));
    if (Encoding == ET_UNDEF || Encoding == ET_ASCII)
    {
        switch (ValueType)
        {
            case 1:
                VecValI2.reserve(B.VecValI2.size());
                copy(B.VecValI2.begin(),B.VecValI2.end(),back_inserter(VecValI2));
                break;
            case 2:
                VecValI4.reserve(B.VecValI4.size());
                copy(B.VecValI4.begin(),B.VecValI4.end(),back_inserter(VecValI4));
                break;
            case 3:
                VecValR4.reserve(B.VecValR4.size());
                copy(B.VecValR4.begin(),B.VecValR4.end(),back_inserter(VecValR4));
                break;
            case 4:
                VecValR8.reserve(B.VecValR8.size());
                copy(B.VecValR8.begin(),B.VecValR8.end(),back_inserter(VecValR8));
                break;
            default:
                break;
        }
        VecPrec.reserve(B.VecPrec.size());
        copy(B.VecPrec.begin(),B.VecPrec.end(),back_inserter(VecPrec));
    }

    return *this;
}

/**
* \brief Destructor.
*/
Transformation::~Transformation()
{
//    std::cout << GridFile+" destroyed\n";
    if (fin)
    {
    fin.close();
    fin.clear();
    }
}

/**
* \brief Displays the Transformation metadata (grid-oriented).
* \return a std::string containing the metadata.
*/
std::string Transformation::DisplayMetaData()
{
    std::ostringstream oss("");

    // General data
    oss << "Données générales\n\n";
    oss << "type       \t" << GridTypeText[(int)(GridType)] << "\n";
    oss << "GridFile        \t" << GridFile << "\n";
    oss << "MetadadaFileName\t" << MetadadaFileName << "\n";
    oss << "Origin\t" << Origin << "\n";
    oss << "Created   \t" << Created << "\n";
    oss << "Version    \t" << Version << "\n";
    oss << "mot clé    \t" << Keyword << "\n";
    oss << "Encoding   \t" << EncodingTypeText[(int)Encoding] << "\n";
    oss << "Description\t" << Description << "\n";

    //Node data
    oss << "\nNode data\n\n";
    oss << "Array Layout         \t" << Layout << "\n";
    oss << "Number of nodes      \t" << N_nodes << "\n";
    oss << "Number of columns    \t" << N_col << "\n";
    oss << "Number of lines      \t" << N_lines << "\n";
    oss << "West-East increment  \t" << WE_inc << "\n";
    oss << "South-North increment\t" << SN_inc << "\n";
    oss << "West Bound           \t" << WestBound << "\n";
    oss << "East Bound           \t" << EastBound << "\n";
    oss << "South Bound          \t" << SouthBound << "\n";
    oss << "North Bound          \t" << NorthBound << "\n";
    oss << "Node coordinates type\t" << CRStypeAttributeText[(int)NodeCRStype] << "\n";

    oss << "Increment measure unit\t" << MeasureUnitAttributeText[(int)IncCoordUnit] << "\n";
    oss << "Node measure unit     \t" << MeasureUnitAttributeText[(int)NodeIncCoordUnit] << "\n";
    oss << "Coordinates written in file?\t" << bNodeCoordWritten << "\n";
    oss << "Precision code written in file?\t" << bPrecisionCode << "\n";
    oss << "West-positive?           \t" << bPositiveWest << "\n";

    //Value data
    oss << "\nValue data\n\n";
    oss << "InitValues     ";
    for (int i=0;i<N_val;i++){oss << "\t" << InitValues[i]; }
    oss << "\n";
    oss << "crs_type ?     \t" << crs_type << "\n";
    oss << "PrecisionCodeInit ?\t" << PrecisionCodeInit << "\n";
    oss << "Centre pixel ?     \t" << bPixelCenter << "\n";
    oss << "Separateur ?       \t" << bDelimiter << "\n";
    oss << "NumLigneFin ?      \t" << bFinalLineNumber << "\n";
    oss << "# valeurs          \t" << N_val << "\n";
    oss << "Type des valeurs   \t" << ValueType << "\n";
    oss << "Taille des valeurs \t" << ValueSize << " bytes\n";
    oss << "Valeur inconnue    \t" << UnknownValue << "\n";
    oss << "bUnknownValue2zero ?     \t" << bUnknownValue2zero << "\n";
    oss << "Translation     ";
    oss << "to be done";
//    for (int i=0;i<N_val;i++){oss << "\t" << Translation[i]); }
    oss << "\n";
    oss << "Facteur multiplicatif   \t" << RateFactor << "\n";
    oss << "Valeur minimale   \t" << MinValue << "\n";
    oss << "Valeur maximale   \t" << MaxValue << "\n";
    oss << "Valeur moyenne    \t" << MeanValue << "\n";
    oss << "Unité des valeurs \t" << MeasureUnitAttributeText[(int)ValueUnit] << "\n";
    oss << "Interpolation     \t" << ValueProcessingText[(int)InterpolationType] << "\n";
    oss << "Ordre             \t" << InterpolationOrder << "\n";

    //Geodetic systems
    oss << "\nGeodetic systems\n\n";
    oss << "SourceFrameId \t" << SourceFrameId << "\n";
    oss << "TargetFrameId \t" << TargetFrameId << "\n";
    oss << "NodeFrameId \t"   << NodeFrameId << "\n";
    oss << "Système        \t" << NodeFrameInfoText[(int)NodeFrameInfo] << "\n";
    oss << "Af             \t" << Af << "\n";
    oss << "Bf             \t" << Bf << "\n";
    oss << "E2f            \t" << E2f << "\n";
    oss << "EpochF         \t" << EpochF << "\n";
    oss << "At             \t" << At << "\n";
    oss << "Bt             \t" << Bt << "\n";
    oss << "E2t            \t" << E2t << "\n";
    oss << "EpochT         \t" << EpochT << "\n";

    return oss.str();
}

/**
* \brief Writes the Transformation metadata in a XML file (grid-oriented).
* \param[in] XMLfilePath: Pathname of the XML file. If empty, the file name will be the same as the grid name with xml extension.
* \return 0 if terminated with success.
*/
int Transformation::WriteXML(std::string *XMLfilePath)
{
    std::string str = "";

    if (*XMLfilePath == "")
        *XMLfilePath = replaceExtension(GridFile, std::string("xml"));

    std::ofstream fifo(XMLfilePath->data());

    fifo << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
    fifo << "<GRID_HEADER>\n";

    fifo << "\t<GRID_FORMAT_INFO>" << GridTypeText[(int)(GridType)] << "</GRID_FORMAT_INFO>\n";
    // problème avec les accents //fifo << "\t<NAME>" << GridFile << "</NAME>\n";
    // problème avec les accents //fifo << "\t<METADATANAME>" << MetadadaFileName << "</METADATANAME>\n";
    fifo << "\t<ORIGIN>" << Origin << "</ORIGIN>\n";
    fifo << "\t<CREATED>" << Created << "</CREATED>\n";
    fifo << "\t<VERSION>" << Version << "</VERSION>\n";
    fifo << "\t<KEY>" << Keyword << "</KEY>\n";
    // problème avec les accents //fifo << "\t<DESC>" << Description << "</DESC>\n";
    fifo << "\t<ENDIANNESS>" << EncodingTypeText[(int)Encoding] << "</ENDIANNESS>\n";
    fifo << "\t<LOADING>" << LoadingTypeText[(int)LoadingType] << "</LOADING>\n";

    fifo << "\t<LAYOUT>" << TransfoLayoutText[Layout] << "</LAYOUT>\n";
    fifo << "\t<N_NODES>" << asString(N_nodes) << "</N_NODES>\n";
    fifo << "\t<N_COLUMNS>" << asString(N_col) << "</N_COLUMNS>\n";
    fifo << "\t<N_LINES>" << asString(N_lines) << "</N_LINES>\n";
    fifo << "\t<X_INC>" << (WE_inc==0?"UNDEFINED":asString(WE_inc,15)) << "</X_INC>\n";
    fifo << "\t<Y_INC>" << (SN_inc==0?"UNDEFINED":asString(SN_inc,15)) << "</Y_INC>\n";
    if (bPositiveWest)
    {
        fifo << "\t<X_MIN>" << asString(-EastBound,15) << "</X_MIN>\n";
        fifo << "\t<X_MAX>" << asString(-WestBound,15) << "</X_MAX>\n";
    }
    else
    {
        fifo << "\t<X_MIN>" << asString(WestBound,15) << "</X_MIN>\n";
        fifo << "\t<X_MAX>" << asString(EastBound,15) << "</X_MAX>\n";
    }
    fifo << "\t<Y_MIN>" << asString(SouthBound,15) << "</Y_MIN>\n";
    fifo << "\t<Y_MAX>" << asString(NorthBound,15) << "</Y_MAX>\n";
    fifo << "\t<NODE_COORD_TYPE>" << CRStypeAttributeText[(int)NodeCRStype] << "</NODE_COORD_TYPE>\n";
    fifo << "\t<INC_UNIT>" << MeasureUnitAttributeText[(int)IncCoordUnit] << "</INC_UNIT>\n";
    fifo << "\t<NODE_COORD_UNIT>" << MeasureUnitAttributeText[(int)NodeIncCoordUnit] << "</NODE_COORD_UNIT>\n";
    fifo << "\t<NODE_COORD_WRITTEN>" << BoolText[bNodeCoordWritten] << "</NODE_COORD_WRITTEN>\n";
    fifo << "\t<EAST_TO_WEST>" << BoolText[bPositiveWest] << "</EAST_TO_WEST>\n";

    if (N_val>0) fifo << "\t<INIT_VALUES>" << asString(InitValues[0]) << "";
    for (int i=0;i<N_val-1;i++) {fifo << "," << asString(InitValues[i]);}
    if (N_val>0) fifo << "</INIT_VALUES>\n";
    fifo << "\t<VALUES_LOCATION>" << (bPixelCenter==1?"PIXEL_CENTER":"NODE") << "</VALUES_LOCATION>\n";
    fifo << "\t<CRS_TYPE>" << CRStypeAttributeText[(int)crs_type] << "</CRS_TYPE>\n";
    fifo << "\t<PRECISION_INIT>" << asString(PrecisionCodeInit) << "</PRECISION_INIT>\n";
    fifo << "\t<SEP>" << BoolText[bDelimiter] << "</SEP>\n";
    //fifo << "\t<>" << BoolText[bFinalLineNumber] << "</>\n";
    fifo << "\t<VALUES_NUMBER>" << (N_val==0?"UNDEFINED":asString(N_val)) << "</VALUES_NUMBER>\n";
    fifo << "\t<CODE_PREC>" << BoolText[bPrecisionCode] << "</CODE_PREC>\n";
    fifo << "\t<VALUES_TYPE>" << ValueTypeText[(int)ValueType] << "</VALUES_TYPE>\n";
    fifo << "\t<VALUE_UNK>" << asString(UnknownValue) << "</VALUE_UNK>\n";
    fifo << "\t<VALUE_UNK_TO_ZERO>" << BoolText[bUnknownValue2zero] << "</VALUE_UNK_TO_ZERO>\n";
    if (N_val>0) fifo << "\t<TRANSLATION>" << asString(Translation[0]) << "";
    for (int i=0;i<N_val-1;i++) {fifo << "," << asString(Translation[i]);}
    if (N_val>0) fifo << "</TRANSLATION>\n";
    fifo << "\t<RATE_FACTOR>" << asString(RateFactor) << "</RATE_FACTOR>\n";
    fifo << "\t<VALUES_MIN>" << asString(MinValue) << "</VALUES_MIN>\n";
    fifo << "\t<VALUES_MAX>" << asString(MaxValue) << "</VALUES_MAX>\n";
    fifo << "\t<VALUES_UNIT>" << MeasureUnitAttributeText[(int)ValueUnit] << "</VALUES_UNIT>\n";
    fifo << "\t<INTERPOLATION_TYPE>" << ValueProcessingText[(int)InterpolationType] << "</INTERPOLATION_TYPE>\n";
    fifo << "\t<INTERPOLATION_ORDER>" << (InterpolationOrder==0?"UNDEFINED":asString(InterpolationOrder)) << "</INTERPOLATION_ORDER>\n";
    //oss << "Valeur moyenne    \t" <<  << "\n",MeanValue; str.append(cstr;
    //oss << "Taille des valeurs \t" <<  << " octets\n",ValueSize;    str.append(cstr;

    fifo << "\t<source_frame>" << SourceFrameId << "</source_frame>\n";
    fifo << "\t<target_frame>" << TargetFrameId << "</target_frame>\n";
    fifo << "\t<node_frame>" << NodeFrameId << "</node_frame>\n";
    fifo << "\t<node_frame_info>" << NodeFrameInfoText[(int)NodeFrameInfo] << "</node_frame_info>\n";
    fifo << "\t<MAJOR_F>" << (Af==0?"UNDEFINED":asString(Af,4)) << "</MAJOR_F>\n";
    fifo << "\t<MINOR_F>" << (Bf==0?"UNDEFINED":asString(Bf,4)) << "</MINOR_F>\n";
    fifo << "\t<SQ_EXC_F>" << (E2f==0?"UNDEFINED":asString(E2f,12)) << "</SQ_EXC_F>\n";
    fifo << "\t<source_epoch>" << (EpochF==0?"UNDEFINED":asString(EpochF,3)) << "</source_epoch>\n";
    fifo << "\t<MAJOR_T>" << (At==0?"UNDEFINED":asString(At,4)) << "</MAJOR_T>\n";
    fifo << "\t<MINOR_T>" << (Bt==0?"UNDEFINED":asString(Bt,4)) << "</MINOR_T>\n";
    fifo << "\t<SQ_EXC_T>" << (E2t==0?"UNDEFINED":asString(E2t,12))<< "</SQ_EXC_T>\n";
    fifo << "\t<target_epoch>" << (EpochT==0?"UNDEFINED":asString(EpochT,3)) << "</target_epoch>\n";

    fifo << "</GRID_HEADER>\n";
    fifo.close();
    return 0;
}

/**
* \brief Reads the Transformation metadata in a XML file (grid-oriented).
* \param[in] XMLfilePath: Pathname of the XML file.
* \return 0 if terminated with success.
* \throw str A std:string containing the error message if any.
*/
int Transformation::ReadXML(std::string XMLfilePath)
{
    if (XMLfilePath == "")
        XMLfilePath = replaceExtension(GridFile, std::string("xml"));

    std::ifstream FicIn;
    FicIn.open(XMLfilePath.data());
    if (!FicIn.is_open())
        return 1;//"Fichier "+XMLfilePath+" introuvable";
    FicIn.close();
        //#endif

    tinyxml2::XMLDocument XMLdoc;
    tinyxml2::XMLElement *_nodeGridHeader;
    tinyxml2::XMLError err_load = XMLdoc.LoadFile(XMLfilePath.data());
    if (err_load)
        return 1;

    _nodeGridHeader = XMLdoc.FirstChildElement("GRID_HEADER");
    if (!_nodeGridHeader)
        throw std::string("GRID_HEADER ")+DataValidationText[DV_TAG_NOT_FOUND];

    //setGridType ??
    //This syntax reinitializes the metadata event if the element is not found !
    //GridFile.assign(ReadString(_nodeGridHeader,"NAME"));
    setGridFile(ReadString(_nodeGridHeader,"NAME"));
    setMetadadaFileName(ReadString(_nodeGridHeader,"METADATANAME"));
    setOrigin(ReadString(_nodeGridHeader,"ORIGIN"));
    setCreated(ReadString(_nodeGridHeader,"CREATED"));
    setVersion(ReadString(_nodeGridHeader,"VERSION"));
    setKeyword(ReadString(_nodeGridHeader,"KEY"));
    setDescription(ReadString(_nodeGridHeader,"DESC"));
    setEncoding(ReadString(_nodeGridHeader,"ENDIANNESS"));
    setLoadingType(ReadString(_nodeGridHeader,"LOADING"));

    setLayout(ReadString(_nodeGridHeader,"LAYOUT"));
    setN_nodes(ReadString(_nodeGridHeader,"N_NODES"));
    setN_col(ReadString(_nodeGridHeader,"N_COLUMNS"));
    setN_lines(ReadString(_nodeGridHeader,"N_LINES"));
    setWE_inc(ReadString(_nodeGridHeader,"X_INC"));
    setSN_inc(ReadString(_nodeGridHeader,"Y_INC"));
    setWestBound(ReadString(_nodeGridHeader,"X_MIN"));
    setEastBound(ReadString(_nodeGridHeader,"X_MAX"));
    setSouthBound(ReadString(_nodeGridHeader,"Y_MIN"));
    setNorthBound(ReadString(_nodeGridHeader,"Y_MAX"));
    setNodeCRStype(ReadString(_nodeGridHeader,"NODE_COORD_TYPE"));
    setIncCoordUnit(ReadString(_nodeGridHeader,"INC_UNIT"));
    setNodeIncCoordUnit(ReadString(_nodeGridHeader,"NODE_COORD_UNIT"));
    setbNodeCoordWritten(ReadString(_nodeGridHeader,"NODE_COORD_WRITTEN"));
    setbPositiveWest(ReadString(_nodeGridHeader,"EAST_TO_WEST"));
    setbPixelCenter(ReadString(_nodeGridHeader,"VALUES_LOCATION"));
    setCrsType(ReadString(_nodeGridHeader,"CRS_TYPE"));
    setInitValues(ReadString(_nodeGridHeader,"INIT_VALUES"));
    setPrecisionCodeInit(ReadString(_nodeGridHeader,"PRECISION_INIT"));
    setbDelimiter(ReadString(_nodeGridHeader,"SEP"));
    //if ((_nodeName=_nodeGridHeader->FirstChildElement(""))&&(valueNode=_nodeName->FirstChild()))
    //	for(i = 0; i<2; i++) if (strcmp(valueNode->Value(),BoolText[i].data())==0) bFinalLineNumber = i;
    setValuesNumber(ReadString(_nodeGridHeader,"VALUES_NUMBER"));
    setbPrecisionCode(ReadString(_nodeGridHeader,"CODE_PREC"));
    setValueType(ReadString(_nodeGridHeader,"VALUES_TYPE"));
    setUnknownValue(ReadString(_nodeGridHeader,"VALUE_UNK"));
    setbUnknownValue2zero(ReadString(_nodeGridHeader,"VALUE_UNK_TO_ZERO"));
    setTranslation(ReadString(_nodeGridHeader,"TRANSLATION"));
    setRateFactor(ReadString(_nodeGridHeader,"RATE_FACTOR"));
    setMinValue(ReadString(_nodeGridHeader,"VALUES_MIN"));
    setMaxValue(ReadString(_nodeGridHeader,"VALUES_MAX"));
    setValueUnit(ReadString(_nodeGridHeader,"VALUES_UNIT"));
    setInterpolationType(ReadString(_nodeGridHeader,"INTERPOLATION_TYPE"));
    setInterpolationOrder(ReadString(_nodeGridHeader,"INTERPOLATION_ORDER"));
    setSourceFrameId(ReadString(_nodeGridHeader,"source_frame"));
    setEpochF(ReadString(_nodeGridHeader,"source_epoch"));
    setTargetFrameId(ReadString(_nodeGridHeader,"target_frame"));
    setEpochT(ReadString(_nodeGridHeader,"target_epoch"));
    setNodeFrameId(ReadString(_nodeGridHeader,"node_frame"));
    setNodeFrameInfo(ReadString(_nodeGridHeader,"node_frame_info"));
    setAf(ReadString(_nodeGridHeader,"MAJOR_F"));
    setBf(ReadString(_nodeGridHeader,"MINOR_F"));
    setE2f(ReadString(_nodeGridHeader,"SQ_EXC_F"));
    setAt(ReadString(_nodeGridHeader,"MAJOR_T"));
    setBt(ReadString(_nodeGridHeader,"MINOR_T"));
    setE2t(ReadString(_nodeGridHeader,"SQ_EXC_T"));

    return 0;
}


/**
* \brief Returns the grid node geodetic frame id.
* \return a std::string representing the grid node geodetic frame id.
*/
std::string Transformation::getNodeFrameId() const
{
	switch (NodeFrameInfo)
	{
	case F_SOURCE:
		return SourceFrameId;
		break;
	case F_TARGET:
		return TargetFrameId;
		break;
	case F_OTHER:
		return NodeFrameId;
		break;
	default:
		return "";
		break;
	}
	return "";
}

/**
* \brief Converts the transformation parameters read in a metadata file into standard units.
* \remark The translations and translation rates are supposed to be in meters and are not converted.
* \remark The rotations and rotation rate unit are given by this->getValueUnit(). The values are converted into radians.
* \remark The scale and scale rate unit are supposed to be in ppm and are multiplied by 1e-6.
*/
//MODIF: In place of MU_SECOND, getValueUnit() returns the unit of the rotation angles 
void Transformation::ConvertParameters()
{
    //Transformation between vertical frames => return (final unit in XML)
    if (InitValues.size()==5) return;
    //Geodetic transformation
	if (InitValues.size()>3) InitValues[3] *= 1e-6; //scale
	if (InitValues.size()>6) //rotation
	{
		InitValues[4] = UnitConvert(InitValues[4], getValueUnit(), MU_RADIAN);
		InitValues[5] = UnitConvert(InitValues[5], getValueUnit(), MU_RADIAN);
		InitValues[6] = UnitConvert(InitValues[6], getValueUnit(), MU_RADIAN);
	}
	if (InitValues.size() == 14)
	{
		InitValues[10] *= 1e-6; //scale
		InitValues[11] = UnitConvert(InitValues[11], getValueUnit(), MU_RADIAN);
		InitValues[12] = UnitConvert(InitValues[12], getValueUnit(), MU_RADIAN);
		InitValues[13] = UnitConvert(InitValues[13], getValueUnit(), MU_RADIAN);
	}
}

/**
* \brief Applies the parameter rates to calculate the transformation values at a given epoch.
* \param[in] ep: the epoch (decimal year).
* \param[out] this->InitValues[]: the transformation values.
*/
void Transformation::toEpoch(double ep)
{
	if (InitValues.size() != 14)
		return;
	if (ep < 1900 || ep > 2100)
		return;
	double de = ep - Epoch;
    //int drctn = (getOperationApplication() != OA_DIRECT) ? -1 : 1;
    for (int i = 0; i<7; i++) InitValues[i] += InitValues[i + 7] * de;
}

/**
* \brief Applies the parameter rates to calculate the transformation values at a given epoch.
* \param[in] de: the epoch difference (decimal year).
* \param[in] values: array of double to be added to the transformation values.
* \param[out] this->InitValues[]: the transformation values.
*/
void Transformation::toEpoch(double de, const double values[7])
{
    if (InitValues.size() < 7)
        return;
    if (InitValues.size() < 14)
    {
        for (int i = 0; i<7; i++)
            InitValues[i] = values[i];
        return;
    }
	//if (ep < 1900 || ep > 2100)
		//return;
	//double de = ep - Epoch;
	for (int i = 0; i<7; i++) InitValues[i] = InitValues[i + 7] * de + values[i];
}

/**
* \brief Checks wether a legacy Circe precision code is valid or not.
* \param[in] CodePrecision: the int precision code.
* \return true if the precision code is valid.
*/
/* Precision codes
0:	no information
8:	from .5 to 1cm
7:	from 1  to 2cm
1:	from 1  to 5cm
2:	from 5  to 10cm
3:	from 10 to 20cm
4:	from 20 to 50cm
5:	from 1  to 2m
6:	from 2  to 5m
10:	< 1m
11:	from 1 to 5 cm
99:	> 1m
*/
bool Transformation::checkPrecisionCodeDefinition(int CodePrecision)
{
	switch (CodePrecision)
	{
    case 0:
    case 8:
    case 7:
    case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 10:
	case 11:
	case 99:
		return true;
		break;
	default:
		break;
	}
	return false;
}

/**
* \brief Converts a legacy Circe precision code into angular precisions in longitude and latitude.
* \param[in] CodePrecision: the int precision code.
* \param[in] phi: the latitude (rad).
* \param[in] a: the semi-major axis of the ellipsoid (m).
* \param[in] e2: the squared eccentricity of the ellipsoid (m).
* \param[out] pL: the precision converted into the length of a small parallel arc (rad).
* \param[out] pP: the precision converted into the length of a small meridian arc (rad).
*/
void Transformation::ConversionCode2Precision(int CodePrecision, double phi, double *pL, double *pP, double a, double e2)
{
	double p = 0.;
	switch (CodePrecision)
	{
	case 0: p = 0.; break;
    case 8: p = 0.01; break;
    case 7: p = 0.02; break;
    case 1: p = 0.05; break;
    case 2: p = 0.1; break;
	case 3: p = 0.2; break;
	case 4: p = 0.5; break;
	case 5: p = 2.; break;
	case 6: p = 5.; break;
	case 10: p = 1.; break;
	case 11: p = 0.05; break;
	case 99: p = 0.; break;
	default: p = 0.; break;
	}
	p /= sqrt(2.); //precision of the magnitude (and not each coordinate's)
	*pL = p / GrandeNormale(phi, a, e2) / cos(phi); // p = precision East
	*pP = p / RMer(a, e2, phi); // p = precision North
	return;
}

/**
* \brief Converts a legacy Circe precision code into precision.
* \param[in] CodePrecision: the int precision code.
* \return The precision converted into length.
*/
double Transformation::ConversionCode2Precision(int CodePrecision)
{
    double p = 0.;
    switch (CodePrecision)
    {
    case 0: p = 0.; break;
    case 8: p = 0.01; break;
    case 7: p = 0.02; break;
    case 1: p = 0.05; break;
    case 2: p = 0.1; break;
    case 3: p = 0.2; break;
    case 4: p = 0.5; break;
    case 5: p = 2.; break;
    case 6: p = 5.; break;
    case 10: p = 1.; break;
    case 11: p = 0.05; break;
    case 99: p = 0.; break;
    default: p = 0.; break;
    }
    return p;
}

/**
* \brief Converts angular precisions in longitude and latitude into a legacy Circe precision code.
* \param[in] phi: the latitude (rad).
* \param[in] a: the semi-major axis of the ellipsoid (m).
* \param[in] e2: the squared eccentricity of the ellipsoid (m).
* \param[in] pL: the length of a small parallel arc (rad).
* \param[in] pP: the length of a small meridian arc (rad).
* \param[out] CodePrecision: the int precision code.
*/
void Transformation::ConversionPrecision2Code(int *CodePrecision, double phi, double pL, double pP, double a, double e2)
{
	double p = sqrt(pow(pL*GrandeNormale(phi, a, e2)*cos(phi), 2) + pow(pP*RMer(a, e2, phi), 2));
    if (p <= 0.01) *CodePrecision = 8;
    else if (p <= 0.02) *CodePrecision = 7;
    else if (p <= 0.05) *CodePrecision = 1;
    else if (p <= 0.1) *CodePrecision = 2;
	else if (p <= 0.2) *CodePrecision = 3;
	else if (p <= 0.5) *CodePrecision = 4;
	else if (p <= 1.) *CodePrecision = 10;
	else if (p <= 2.) *CodePrecision = 5;
	else if (p <= 5.) *CodePrecision = 6;
	else *CodePrecision = 99;
	return;
}

/**
* \brief Converts length precision into a legacy Circe precision code.
* \param[in] p: the length precision.
* \return The int precision code.
*/
int Transformation::ConversionPrecision2Code(double p)
{
    int cp = 99;
    if (p <= 0.01) cp = 8;
    else if (p <= 0.02) cp = 7;
    else if (p <= 0.05) cp = 1;
    else if (p <= 0.1) cp = 2;
    else if (p <= 0.2) cp = 3;
    else if (p <= 0.5) cp = 4;
    else if (p <= 1.) cp = 10;
    else if (p <= 2.) cp = 5;
    else if (p <= 5.) cp = 6;
    return cp;
}

/**
* \brief Converts a string containing comma-separated values into a vector of double.
* \param[in] _value: a string containing comma-separated values.
* \param[in] count: the number of values.
* \param[out] doubleVector: the vector of double.
*/
void Transformation::setDoubleVector(std::string _value, std::vector<double>* doubleVector, int count)
{
	char coco[_MAX_PATH_], seps[] = ",", *token;
	if (count == 0)
		return;
	if (_value == "")
	{
		for (int i = 0; i<count; i++)
			(*doubleVector)[i] = 0.;
		return;
	}
	strcpy(coco, _value.data());
	token = strtok(coco, seps);
	doubleVector->resize(count);
	sscanf(token, "%lf", &((*doubleVector)[0]));
	for (int i = 1; i<count; i++)
	{
		token = strtok(NULL, seps);
		sscanf(token, "%lf", &((*doubleVector)[i]));
	}
}


/**
* \brief Initializes the grid type according to the grid file extension which may not be the usual one but specific to Circe. Initializes default metadata values according to grid type.
* \param[in] grid_index: a code to force the grid type.
* \return 0 if terminated with success.
* \throw str A std:string containing the error message if any.
*/
int Transformation::InitGridType(int grid_index)
{
    std::string ext = getExtension(GridFile);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

    //By default binary grid endiannes is the system's one
    //(can be overriden by xml value)
    bool BigEndianSystem = true;
    unsigned char SwapTest[2] = {1,0};
    if(*(short *)SwapTest == 1)
    BigEndianSystem = false;

    Encoding = ET_ASCII;

    if (grid_index != 0)
    {
        GridType = (GRID_FORMAT_INFO)(grid_index);
    }
    else
    {
             if (ext=="TAC") GridType = GF_SGN;
        else if (ext=="TBC")
        {
            GridType = GF_SGN_BIN;
            if (BigEndianSystem)
                Encoding = ET_BIG_ENDIAN;
            else
                Encoding = ET_LITTLE_ENDIAN;
        }
        else if (ext=="GRA") GridType = GF_GRAVSOFT;
        else if (ext=="GRD") GridType = GF_SURFER;
        else if (ext=="GSA") GridType = GF_NTV2_ASC;
        else if (ext=="GSB")
        {
            unsigned char SwapTest[2] = {1, 0};
            if (*(short *)SwapTest == 1) //little endian
            {
                GridType = GF_NTV2_BIN_L;
                Encoding = ET_LITTLE_ENDIAN;
            }
            else //big endian
            {
                GridType = GF_NTV2_BIN_B;
                Encoding = ET_BIG_ENDIAN;
            }
        }
        else if (ext=="ASC") GridType = GF_ESRI;
        else if (ext=="EGM") GridType = GF_EGM;
        else if (ext=="DIS") GridType = GF_DIS;
        else if (ext=="GTX") GridType = GF_GTX_BIN;
        else if (ext=="GTA") GridType = GF_GTX_ASC;
        else
        {
            GridType = GF_UNDEF;
            throw std::string("Transformation::InitGridType: GridType "+asString(GridType)+ErrorMessageText[EM_NOT_VALID]+"\n");
        }
    }

    //Default values.
    //They should be implemented in xml files.
    if (GridType == GF_NTV2_ASC || GridType == GF_NTV2_BIN_L || GridType == GF_NTV2_BIN_B)
    {
        ValueUnit = MU_SECOND;
        NodeIncCoordUnit = MU_SECOND;
        IncCoordUnit = MU_SECOND;
        bPositiveWest = 1;
        bPixelCenter = 0;
        Layout = L_SE2W2N;
        bNodeCoordWritten = 0;
        N_val = 4;
        ValueType = 3;
        bPrecisionCode = 0;
    }
    else if ((GridType == GF_SGN || GridType == GF_SGN_BIN) && N_val == 2)
    {
        ValueUnit = MU_SECOND;
        NodeIncCoordUnit = MU_DEGREE;
        IncCoordUnit = MU_DEGREE;
        bPositiveWest = 0;
    }
    else if (GridType == GF_GRAVSOFT)
    {
        Layout = L_NW2E2S;
        bNodeCoordWritten = 0;
        N_val = 1;
        bPrecisionCode = 0;
        ValueUnit = MU_METER;
        NodeIncCoordUnit = MU_DEGREE;
        IncCoordUnit = MU_DEGREE;
        bPositiveWest = 0;
    }
    else if (GridType == GF_GTX_BIN || GridType == GF_GTX_ASC)
    {
        Layout = L_SW2E2N;
        bNodeCoordWritten = 0;
        N_val = 1;
        bPrecisionCode = 0;
        ValueUnit = MU_METER;
        NodeIncCoordUnit = MU_DEGREE;
        IncCoordUnit = MU_DEGREE;
        bPositiveWest = 0;
    }
    else
    {
        ValueUnit = MU_METER;
        NodeIncCoordUnit = MU_DEGREE;
        IncCoordUnit = MU_DEGREE;
        bPositiveWest = 0;
    }

//    if (Encoding == ET_UNDEF)
//        Encoding = (ENCODING_TYPE)ListeCodageBinaireGrille[(int)(GridType)];

    return 0;
}


/**
* \brief Initializes the encoding type and the function pointers for reading and writing values in grid files.
* \param[in] bSystemBinary: a boolean, if true the encoding type will follow the system's endianness.
* \return 0 if terminated with success.
* \throw str A std:string containing the error message if any.
*/
int Transformation::InitAccesElement(bool bSystemBinary)
{
    bool BigEndianSystem = true;
    unsigned char SwapTest[2] = {1,0};
    if(*(short *)SwapTest == 1)
        BigEndianSystem = false;
    if (bSystemBinary)
    {
        if (BigEndianSystem)
            Encoding = ET_BIG_ENDIAN;
        else
            Encoding = ET_LITTLE_ENDIAN;
    }

    switch (ValueType)
    {
        case 1: ValueSize = sizeof(short); break;
        case 2: ValueSize = sizeof(int); break;
        case 3: ValueSize = sizeof(float); break;
        case 4: ValueSize = sizeof(double); break;
        default:
            //throw "ValueType "+asString(ValueType)+ErrorMessageText[EM_NOT_VALID]+"\n("+GridFile+")";
            // ?
            ValueType = 4;
            ValueSize = sizeof(double);
            break;
    }

//    if (LoadingType == LD_BINARY && Encoding != ET_ASCII)
        if (Encoding != ET_ASCII)
    {
        if ( (Encoding == ET_LITTLE_ENDIAN && !BigEndianSystem)
               || (Encoding == ET_BIG_ENDIAN && BigEndianSystem))
        {
            ReadElementI4 = &Transformation::ReadI4bin;
            ReadElementR8 = &Transformation::ReadR8bin;
            WriteElementI4 = &Transformation::WriteI4bin;
            WriteElementR8 = &Transformation::WriteR8bin;
            switch (ValueType)
            {
                case 1:
                    ReadElementBin = &Transformation::ReadI2bin_d;
                    WriteElementBin = &Transformation::WriteI2bin_d;
                    break;
                case 2:
                    ReadElementBin = &Transformation::ReadI4bin_d;
                    WriteElementBin = &Transformation::WriteI4bin_d;
                    break;
                case 3:
                    ReadElementBin = &Transformation::ReadR4bin_d;
                    WriteElementBin = &Transformation::WriteR4bin_d;
                    break;
                case 4:
                    ReadElementBin = &Transformation::ReadR8bin;
                    WriteElementBin = &Transformation::WriteR8bin;
                    break;
                default:
                    break;
            }
        }
        else if ( (Encoding == ET_BIG_ENDIAN && !BigEndianSystem)
               || (Encoding == ET_LITTLE_ENDIAN && BigEndianSystem))
        {
            ReadElementI4 = &Transformation::ReadI4bin_Swap;
            ReadElementR8 = &Transformation::ReadR8bin_Swap;
            WriteElementI4 = &Transformation::WriteI4bin_Swap;
            WriteElementR8 = &Transformation::WriteR8bin_Swap;
            switch (ValueType)
            {
                case 1:
                    ReadElementBin = &Transformation::ReadI2bin_d_Swap;
                    WriteElementBin = &Transformation::WriteI2bin_d_Swap;
                    break;
                case 2:
                    ReadElementBin = &Transformation::ReadI4bin_d_Swap;
                    WriteElementBin = &Transformation::WriteI4bin_d_Swap;
                    break;
                case 3:
                    ReadElementBin = &Transformation::ReadR4bin_d_Swap;
                    WriteElementBin = &Transformation::WriteR4bin_d_Swap;
                    break;
                case 4:
                    ReadElementBin = &Transformation::ReadR8bin_Swap;
                    WriteElementBin = &Transformation::WriteR8bin_Swap;
                    break;
                default:
                    break;
            }
        }
        else
            throw std::string("Transformation::InitAccesElement: Encoding "+asString(Encoding)+ErrorMessageText[EM_NOT_VALID]+"\n");
    }
    else
    {
        switch (ValueType)
        {
            case 1:
                ReadElementTab = &Transformation::ReadI2tab;
                WriteElementTab = &Transformation::WriteI2tab;
                break;
            case 2:
                ReadElementTab = &Transformation::ReadI4tab;
                WriteElementTab = &Transformation::WriteI4tab;
                break;
            case 3:
                ReadElementTab = &Transformation::ReadR4tab;
                WriteElementTab = &Transformation::WriteR4tab;
                break;
            case 4:
                ReadElementTab = &Transformation::ReadR8tab;
                WriteElementTab = &Transformation::WriteR8tab;
                break;
            default:
                break;
        }
    }
    return 0;
}

int Transformation::InitVector(int taille)
{
    Encoding = ET_ASCII;
    switch (ValueType)
    {
        case 1:
            VecValI2.resize(taille);
            break;
        case 2:
            VecValI4.resize(taille);
            break;
        case 3:
            VecValR4.resize(taille);
            break;
        case 4:
            VecValR8.resize(taille);
            break;
        default:
            break;
    }
    return 0;
}

/**
* \brief Calculates the line & column indices and the coordinates of a node index according to the grid layout.
* \param[in] node: node index.
* \param[out] XY: coordinates.
* \param[out] C: column index.
* \param[out] L: line index.
*/
void Transformation::NodeCoord(int node, double XY[3], int *C, int *L)
{
    switch (Layout)
    {
        case L_SW2N2E:
            *C = div(node,N_lines).quot;
            *L = div(node,N_lines).rem;
            break;
        case L_NW2E2S:
            *C = div(node,N_col).rem;
            *L = N_lines-1-div(node,N_col).quot;
            break;
        case L_NW2S2E:
            *C = div(node,N_lines).quot;
            *L = N_lines-1-div(node,N_lines).rem;
            break;
        case L_SW2E2N:
            *C = div(node,N_col).rem;
            *L = div(node,N_col).quot;
            break;
        case L_SE2N2W:
            *C = N_col-1-div(node,N_lines).quot;
            *L = div(node,N_lines).rem;
            break;
        case L_NE2W2S:
            *C = N_col-1-div(node,N_col).rem;
            *L = N_lines-1-div(node,N_col).quot;
            break;
        case L_NE2S2W:
            *C = N_col-1-div(node,N_lines).quot;
            *L = N_lines-1-div(node,N_lines).rem;
            break;
        case L_SE2W2N:
            *C = N_col-1-div(node,N_col).rem;
            *L = div(node,N_col).quot;
            break;
        default:
            break;
    }

    XY[0] = WestBound + (*C)*WE_inc;
    XY[1] = SouthBound + (*L)*SN_inc;
    XY[2] = 0.;
}

/**
* \brief Calculates the node index from the line & column indices according to the grid layout.
* \param[in] i: column index.
* \param[in] j: line index.
* \return node index.
*/
int Transformation::NodePosition(int i, int j) const
{
    int Nb=-1;
    switch (Layout)
    {
        case L_SW2N2E:
            Nb = N_lines*i+j;
            break;
        case L_NW2E2S:
            Nb = N_col*(N_lines-1-j)+i;
            break;
        case L_NW2S2E:
            Nb = N_lines*(i+1)-j-1;
            break;
        case L_SW2E2N:
            Nb = N_col*j+i;
            break;
        case L_SE2N2W:
            Nb = N_lines*(N_col-1-i)+j;
            break;
        case L_NE2W2S:
            //Nb = N_col*(N_lines-1-j)+N_col-1-i;
            Nb = N_col*(N_lines-j)-1-i;
            break;
        case L_NE2S2W:
            Nb = N_lines*(N_col-i)-j-1;
            break;
        case L_SE2W2N:
            Nb = N_col*j+N_col-1-i;
            break;
        default:
            break;
    }
    return Nb;
}

std::string Transformation::ReadStringBin (int nc)
{
    char cstr[_MAX_PATH_];
    std::string str;
    fin.read(cstr, nc);
    cstr[nc]='\0';
    str.assign(cstr);
    return str;
}

short Transformation::ReadI2bin ()
{
    short sV;
    fin.read((char*)&sV,sizeof(short));
    return sV;
}
double Transformation::ReadI2bin_d ()
{
    short sV;
    fin.read((char*)&sV,sizeof(short));
    return (double)sV;
}
short Transformation::ReadI2bin_Swap ()
{
    union
    {
        short s;
        unsigned char b[2];
    } dat;
    unsigned char sV[2];
    fin.read((char*)&sV,2);
    dat.b[0] = sV[1];
    dat.b[1] = sV[0];
    return dat.s;
}
double Transformation::ReadI2bin_d_Swap ()
{
    return (double)ReadI2bin_Swap();
}
int Transformation::ReadI4bin ()
{
    int iV;
    fin.read((char*)&iV,sizeof(int));
    return iV;
}
double Transformation::ReadI4bin_d ()
{
    int iV;
    fin.read((char*)&iV,sizeof(int));
    return (double)iV;
}
int Transformation::ReadI4bin_Swap ()
{
    union
    {
        int i;
        unsigned char b[4];
    } dat;
    unsigned char iV[4];
    fin.read((char*)&iV,4);
    dat.b[0] = iV[3];
    dat.b[1] = iV[2];
    dat.b[2] = iV[1];
    dat.b[3] = iV[0];
    return dat.i;
}
double Transformation::ReadI4bin_d_Swap ()
{
    return (double)ReadI4bin_Swap();
}
float Transformation::ReadR4bin ()
{
    float fV;
    fin.read((char*)&fV,sizeof(float));
    return fV;
}
double Transformation::ReadR4bin_d ()
{
    float fV;
    fin.read((char*)&fV,sizeof(float));
    return (double)fV;
}
float Transformation::ReadR4bin_Swap ()
{
    union
    {
        float f;
        unsigned char b[4];
    } dat;
    unsigned char fV[4];
    fin.read((char*)&fV,4);
    dat.b[0] = fV[3];
    dat.b[1] = fV[2];
    dat.b[2] = fV[1];
    dat.b[3] = fV[0];
    return dat.f;
}
double Transformation::ReadR4bin_d_Swap ()
{
    return (double)ReadR4bin_Swap();
}
double Transformation::ReadR8bin ()
{
    double dV;
    fin.read((char*)&dV,sizeof(double));
    return dV;
}
double Transformation::ReadR8bin_Swap ()
{
    union
    {
        double d;
        unsigned char b[8];
    } dat;
    unsigned char dV[8];
    fin.read((char*)&dV,8);
    dat.b[0] = dV[7];
    dat.b[1] = dV[6];
    dat.b[2] = dV[5];
    dat.b[3] = dV[4];
    dat.b[4] = dV[3];
    dat.b[5] = dV[2];
    dat.b[6] = dV[1];
    dat.b[7] = dV[0];
    return dat.d;
}

double Transformation::ReadI2tab (int Position)
{
    return (double)VecValI2[Position];
}
double Transformation::ReadI4tab (int Position)
{
    return (double)VecValI4[Position];
}
double Transformation::ReadR4tab (int Position)
{
    return (double)VecValR4[Position];
}
double Transformation::ReadR8tab (int Position)
{
    return VecValR8[Position];
}

void Transformation::WriteStringBin (const std::string str, int nc)
{
    char cstr[_MAX_PATH_];
    std::string bibi=str;
    if (nc > bibi.size())
        bibi.append(nc-bibi.size(),' ');
    strcpy(cstr,bibi.data());
    fout.write(cstr,nc);
    return;
}

void Transformation::WriteStringBin (const char cstr[_MAX_PATH_], int nc)
{
    fout.write(cstr,nc);
    return;
}

void Transformation::WriteI2bin (const short sV)
{
    fout.write((char*)&sV,sizeof(short));
    return ;
}
void Transformation::WriteI2bin_d (const double sV)
{
    short bibi = short(NINT(sV));
    fout.write((char*)&bibi,sizeof(short));
    return ;
}
void Transformation::WriteI2bin_Swap (const short sV)
{
    union
    {
        short s;
        unsigned char b[2];
    } dat;
    unsigned char bibi[2];
    dat.s=sV;
    bibi[1] = dat.b[0];
    bibi[0] = dat.b[1];
    fout.write((char*)&bibi,2);
    return;
}
void Transformation::WriteI2bin_d_Swap (const double sV)
{
    WriteI2bin_Swap(short(NINT(sV)));
}
void Transformation::WriteI4bin (const int iV)
{
    fout.write((char*)&iV,sizeof(int));
    return;
}
void Transformation::WriteI4bin_d (const double iV)
{
    int bibi = int(NINT(iV));
    fout.write((char*)&bibi,sizeof(int));
    return ;
}
void Transformation::WriteI4bin_Swap (const int iV)
{
    union
    {
        int i;
        unsigned char b[4];
    } dat;
    unsigned char bibi[4];
    dat.i=iV;
    bibi[3] = dat.b[0];
    bibi[2] = dat.b[1];
    bibi[1] = dat.b[2];
    bibi[0] = dat.b[3];
    fout.write((char*)&bibi,4);
    return;
}
void Transformation::WriteI4bin_d_Swap (const double iV)
{
    WriteI4bin_Swap(int(NINT(iV)));
}
void Transformation::WriteR4bin (const float fV)
{
    fout.write((char*)&fV,sizeof(float));
    return ;
}
void Transformation::WriteR4bin_d (const double fV)
{
    float bibi = float(fV);
    fout.write((char*)&bibi,sizeof(float));
    return ;
}
void Transformation::WriteR4bin_Swap (const float fV)
{
    union
    {
        float f;
        unsigned char b[4];
    } dat;
    unsigned char bibi[4];
    dat.f=fV;
    bibi[3] = dat.b[0];
    bibi[2] = dat.b[1];
    bibi[1] = dat.b[2];
    bibi[0] = dat.b[3];
    fout.write((char*)&bibi,4);
    return;
}
void Transformation::WriteR4bin_d_Swap (const double fV)
{
    WriteR4bin_Swap((float)fV);
}
void Transformation::WriteR8bin (const double dV)
{
    fout.write((char*)&dV,sizeof(double));
    return ;
}
void Transformation::WriteR8bin_Swap (const double dV)
{
    union
    {
        double d;
        unsigned char b[8];
    } dat;
    unsigned char bibi[8];
    dat.d = dV;
    bibi[0] = dat.b[7];
    bibi[1] = dat.b[6];
    bibi[2] = dat.b[5];
    bibi[3] = dat.b[4];
    bibi[4] = dat.b[3];
    bibi[5] = dat.b[2];
    bibi[6] = dat.b[1];
    bibi[7] = dat.b[0];
    fout.write((char*)&bibi,8);
    return ;
}

void Transformation::WriteI2tab (const double dV, int Position)
{
    VecValI2[Position] = short(NINT(dV));
}
void Transformation::WriteI4tab (const double dV, int Position)
{
    VecValI4[Position] = int(NINT(dV));
}
void Transformation::WriteR4tab (const double dV, int Position)
{
    VecValR4[Position] = float(dV);
}
void Transformation::WriteR8tab (const double dV, int Position)
{
    VecValR8[Position] = dV;
}

int Transformation::WriteElement(const double dV, int Position)
{
    if (Encoding == ET_ASCII)
        (this->*WriteElementTab)(dV, Position);
    else
        (this->*WriteElementBin)(dV);
    return 0;
}


/**
* \brief Return the grid data at a node from its line & column indices.
* \param[in] i: column index.
* \param[in] j: line index.
* \param[out] VV: an array of double containing the transformation values. Its memory space must have been allocated by the calling function.
* \param[out] precision_code: the precision code.
* \throw str A std:string containing the error message if any.
*/
void Transformation::Element(int i, int j, double *VV, int *precision_code)
{
    int k, Nb;

    if (i<0 || i>=N_col || j<0 || j>=N_lines)
    {
        for (k=0;k<N_val;k++) VV[k] = UnknownValue;
        *precision_code = 0;
        return;
    }

    Nb = NodePosition(i, j);

    if (LoadingType == LD_ARRAY)
    {
        for (k=0;k<N_val;k++)
            VV[k]=(this->*ReadElementTab)(int(Nb*N_val+k));
        if (bPrecisionCode) *precision_code=VecPrec[Nb];
        else *precision_code = 0;
    }
    else if (LoadingType == LD_BINARY)
    {
        if (!fin.is_open())
        {
            fin.open(GridFile.data(),std::ifstream::in|std::ifstream::binary);
            if (!fin.is_open())
                throw std::string("Transformation::Element: ")+ErrorMessageText[EM_CANNOT_OPEN]+" "+GridFile;
        }
        fin.seekg(Offset+Nb*NodeSize, std::ios::beg);
        for (k=0;k<N_val;k++)
            VV[k] = (this->*ReadElementBin)();
        if (bPrecisionCode) *precision_code = ReadI4bin();
        else *precision_code = 0;
    }
    else
        throw std::string("Transformation::Element: LoadingType="+asString(int(LoadingType))+" (shouldn't occur)\n");

    if (bUnknownValue2zero == 1) for (k=0;k<N_val;k++)
        if (VV[k] == UnknownValue) VV[k] = 0.;
}

/**
* \brief Performs a bilinear interpolation.
* \param[in] x, y: coordinates of the position to interpolate at.
* \param[out] VV: an array of double containing the transformation values. Its memory space must have been allocated by the calling function.
* \param[out] precision_code: the precision code.
* \throw str A std:string containing the error message if any.
*/
int Transformation::BilinearInterpolation(double x, double y, double *VV, int *code)
{
    int      C, L, i, P1, P2, P3, P4;
    double   dx, dy, *V1, *V2, *V3, *V4;
    bool     calc_vert_defl = (*code == CALC_VERT_DEFL);

    C = floor((x - WestBound)/WE_inc+EpsilonGrid);
    L = floor((y - SouthBound)/SN_inc+EpsilonGrid);

//    dx = (x - WestBound)/WE_inc;
//    dy = (y - SouthBound)/SN_inc;
//    C = dec_round(dx,EpsilonGrid);
//    L = dec_round(dy,EpsilonGrid);

    //Stop if the point is at more than an increment distance from boundaries.
    //This should not occur in circe since the point position has been checked before.
    if (C < -1 || C >= N_col || L < -1 || L >= N_lines)
    {
        for (i=0;i<N_val;i++)
        {
            if (bUnknownValue2zero == 1) VV[i] = 0.;
            else VV[i] = UnknownValue;
        }
        throw std::string ("Transformation::BilinearInterpolation: out of grid "+GridFile);
        return 2;
    }

    dx = (x - WestBound)/WE_inc - C;
    dy = (y - SouthBound)/SN_inc - L;

    V1 = (double*)malloc(N_val*sizeof(double));
    V2 = (double*)malloc(N_val*sizeof(double));
    V3 = (double*)malloc(N_val*sizeof(double));
    V4 = (double*)malloc(N_val*sizeof(double));

    Element (C,   L,   V1, &P1);
    Element (C,   L+1, V2, &P2);
    Element (C+1, L,   V3, &P3);
    Element (C+1 ,L+1, V4, &P4);

    //Outside boundaries, less than an increment.
    if (C == -1 && L == -1)
    {
        for (i=0;i<N_val;i++) V1[i] = V2[i] = V3[i] = V4[i];
        *code = P4;
    }
    else if (C == -1 && L == N_lines-1)
    {
        for (i=0;i<N_val;i++) V4[i] = V1[i] = V2[i] = V3[i];
        *code = P3;
    }
    else if (C == N_col-1 && L == -1)
    {
        for (i=0;i<N_val;i++) V3[i] = V4[i] = V1[i] = V2[i];
        *code = P2;
    }
    else if (C == N_col-1 && L == N_lines-1)
    {
        for (i=0;i<N_val;i++) V2[i] = V3[i] = V4[i] = V1[i];
        *code = P1;
    }
    else if (C == -1)
    {
        for (i=0;i<N_val;i++)
        {
            V2[i] = V4[i];
            V1[i] = V3[i];
        }
        if (dy < 0.5) *code = P3;
        else *code = P4;
    }
    else if (C == N_col-1)
    {
        for (i=0;i<N_val;i++)
        {
            V4[i] = V2[i];
            V3[i] = V1[i];
        }
        if (dy < 0.5) *code = P1;
        else *code = P2;
    }
    else if (L == -1)
    {
        for (i=0;i<N_val;i++)
        {
            V1[i] = V2[i];
            V3[i] = V4[i];
        }
        if (dx < 0.5) *code = P2;
        else *code = P4;
    }
    else if (L == N_lines-1)
    {
        for (i=0;i<N_val;i++)
        {
            V2[i] = V1[i];
            V4[i] = V3[i];
        }
        if (dx < 0.5) *code = P1;
        else *code = P3;
    }
    else
    {
        //if (P1 < P2) P1 = P2;
        //if (P1 < P3) P1 = P3;
        //if (P1 < P4) P1 = P4;
        //*precision_code = P1;

        //The precision code is the code of the nearest node, so that we get the good
        //one when the point is close to a node.
        if (dx < 0.5)
        {
            if (dy < 0.5) *code = P1;
            else *code = P2;
        }
        else
        {
            if (dy < 0.5) *code = P3;
            else *code = P4;
        }
    }

    for (i=0;i<N_val;i++)
    {
        //to be improved
        if (UnknownValue!=0.
            && (V1[i]==UnknownValue || V2[i]==UnknownValue || V3[i]==UnknownValue || V4[i]==UnknownValue))
            VV[i] = UnknownValue;
        else
            VV[i] = Translation[i] + (1-dx)*(1-dy)*V1[i] + (1-dx)*dy*V2[i] + dx*(1-dy)*V3[i] + dx*dy*V4[i];
    }

    if (calc_vert_defl)
    {
        double xi, eta;
        VertDeflection(At, E2t, y, SN_inc, WE_inc, (V1[0]-V3[0]+V2[0]-V4[0])/2.,(V1[0]-V2[0]+V3[0]-V4[0])/2., &xi, &eta);
        //VertDeflection(6378249, .006803487646, y, SN_inc, WE_inc, -V3[0]+V1[0], -V2[0]+V1[0], &xi, &eta);
        //VertDeflection(6378137, .006694380025, y, SN_inc, WE_inc, (V1[0]-V3[0]+V2[0]-V4[0])/2.,(V1[0]-V2[0]+V3[0]-V4[0])/2., &xi, &eta);
        VV[1] = xi;
        VV[2] = eta;
//        qDebug("phi:%lf",y);
//        qDebug("V3-V1:%lf",V3[0]-V1[0]);
//        qDebug("V2-V1:%lf",V2[0]-V1[0]);
//        qDebug("SN_inc:%lf °",UnitConvert(SN_inc, MU_RADIAN, MU_DEGREE));
//        qDebug("WE_inc:%lf °",UnitConvert(WE_inc, MU_RADIAN, MU_DEGREE));
//        qDebug("xi:%lf \"",UnitConvert(VV[1], MU_RADIAN, MU_SECOND));
//        qDebug("eta:%lf \"",UnitConvert(VV[2], MU_RADIAN, MU_SECOND));
    }

//    char NomFicSuivi[_MAX_PATH];
//    sprintf(NomFicSuivi,"%s.followup",GridFile.data());
//    FILE *FicSuivi=fopen(NomFicSuivi,"a");
//    fprintf(FicSuivi,"L=%.10lf\tP=%.10lf\tC=%d\tL=%d\tdx=%.8lf\tdy=%.10lf",UnitConvert(x,MU_RADIAN,MU_DEGREE),UnitConvert(y,MU_RADIAN,MU_DEGREE),C,L,dx,dy);
//    fprintf(FicSuivi,"\n1\tL=%.10lf\tP=%.10lf\t",UnitConvert(WestBound+WE_inc*C,MU_RADIAN,MU_DEGREE),UnitConvert(SouthBound+SN_inc*L,MU_RADIAN,MU_DEGREE));
//    for (i=0;i<N_val;i++)
//        fprintf(FicSuivi,"\t%.10lf",V1[i]);
//    fprintf(FicSuivi,"\n2\tL=%.10lf\tP=%.10lf\t",UnitConvert(WestBound+WE_inc*C,MU_RADIAN,MU_DEGREE),UnitConvert(SouthBound+SN_inc*(L+1),MU_RADIAN,MU_DEGREE));
//    for (i=0;i<N_val;i++)
//        fprintf(FicSuivi,"\t%.10lf",V2[i]);
//    fprintf(FicSuivi,"\n3\tL=%.10lf\tP=%.10lf\t",UnitConvert(WestBound+WE_inc*(C+1),MU_RADIAN,MU_DEGREE),UnitConvert(SouthBound+SN_inc*L,MU_RADIAN,MU_DEGREE));
//    for (i=0;i<N_val;i++)
//        fprintf(FicSuivi,"\t%.10lf",V3[i]);
//    fprintf(FicSuivi,"\n4\tL=%.10lf\tP=%.10lf\t",UnitConvert(WestBound+WE_inc*(C+1),MU_RADIAN,MU_DEGREE),UnitConvert(SouthBound+SN_inc*(L+1),MU_RADIAN,MU_DEGREE));
//    for (i=0;i<N_val;i++)
//        fprintf(FicSuivi,"\t%.10lf",V4[i]);
//    fprintf(FicSuivi,"\n \tL=%.10lf\tP=%.10lf\t",UnitConvert(x,MU_RADIAN,MU_DEGREE),UnitConvert(y,MU_RADIAN,MU_DEGREE));
//    for (i=0;i<N_val;i++) fprintf(FicSuivi,"\t%.10lf",VV[i]);
//        fprintf(FicSuivi,"\n\n");
//    if (FicSuivi != NULL)
//        fclose (FicSuivi);

    free(V1);
    free(V2);
    free(V3);
    free(V4);

    return 0;
}


/**
* \brief Performs a spline interpolation.
* \param[in] window_size: window size of the spline interpolation.
* \param[in] x, y: coordinates of the position to interpolate at.
* \param[out] VV: an array of double containing the transformation values. Its memory space must have been allocated by the calling function.
* \param[out] precision_code: the precision code.
* \throw str A std:string containing the error message if any.
*/
int Transformation::SplineInterpolation(int window_size, double x, double y, double *VV, int *code)
{
    int      i, c, l, k, C, L, C0, L0, P;//, P1, P2, P3, P4;
    double   dx, dy;
    bool     bValInconnue=false;

    double *V = (double*)malloc(N_val*sizeof(double));
    double *T = (double*)malloc(window_size*window_size*N_val*sizeof(double));
    double *A = (double*)malloc(window_size*sizeof(double));
    double *R = (double*)malloc(window_size*sizeof(double));
    double *HC = (double*)malloc(window_size*sizeof(double));
    double *Q = (double*)malloc(window_size*sizeof(double));

    dx = (x - WestBound)/WE_inc;
    dy = (y - SouthBound)/SN_inc;
    C = floor(dx+EpsilonGrid);
    L = floor(dy+EpsilonGrid);
//    C = dec_round(dx,EpsilonGrid);
//    L = dec_round(dy,EpsilonGrid);

    if (C < 0 || C >= N_col || L < 0 || L >= N_lines)
    {
        for (i=0;i<N_val;i++)
        {
            if (bUnknownValue2zero == 1) VV[i] = 0.;
            else VV[i] = UnknownValue;
        }
        throw std::string ("Transformation::SplineInterpolation: out of grid");
        return 2;
    }

    //Window centering
//    C0 = C - dec_round(window_size/2.,EpsilonGrid) + 1;
//    L0 = L - dec_round(window_size/2.,EpsilonGrid) + 1;
    C0 = C - int(window_size/2) + 1;
    L0 = L - int(window_size/2) + 1;

    //Window shift if partially outside the grid boundaries.
    if (C0<0) C0=0;
    if (L0<0) L0=0;
    if (N_col < C0+window_size) C0 = N_col-window_size;
    if (N_lines < L0+window_size) L0 = N_lines-window_size;

    //Window elements.
    for (c=0; c<window_size; c++) for (l=0; l<window_size; l++)
    {
        Element (C0+c, L0+l, V, &P);
        for (k=0;k<N_val;k++)
        {
            if (UnknownValue != 0. && V[k] == UnknownValue)
                bValInconnue = true;
            else
                T[(l*window_size+c)*N_val+k] = V[k];
        }
        //The precision code is the code of the nearest node, so that we get the good
        //one when the point is close to a node.
        if (abs(dx-C0-c) <= 0.5 && abs(dy-L0-l) <= 0.5)
            *code = P;
    }

    //Spline calculation
    for (k=0;k<N_val;k++)
    {
        if (bValInconnue)
            VV[k] = UnknownValue;
        else
        {
            for (c=0; c<window_size; c++)
            {
                for (l=0; l<window_size; l++) A[l] = T[(c*window_size+l)*N_val+k];
                InitSpline1D(A, window_size, R, Q);
                HC[c] = Spline1D(dx-C0+1, A, window_size, R);
            }
            InitSpline1D(HC, window_size, R, Q);
            VV[k] = Spline1D(dy-L0+1, HC, window_size, R);
        }
    }

    ////precision
    //Element (C,   L,   V, &P1);
    //Element (C,   L+1, V, &P2);
    //Element (C+1, L,   V, &P3);
    //Element (C+1 ,L+1, V, &P4);
    //if (P1 < P2) P1 = P2;
    //if (P1 < P3) P1 = P3;
    //if (P1 < P4) P1 = P4;
    //*precision_code = P1;

    free(V);
    free(T);
    free(A);
    free(R);
    free(HC);
    free(Q);

    return 0;
}

void Transformation::InitSpline1D(const double *Y, const int n, double *R, double *Q)
{
    int k;

    Q[0]=0.0;
    R[0]=0.0;

    for(k=1;k<=n-2;k++)
    {
        double P=Q[k-1]/2+2;
        Q[k] = -0.5/P;
        R[k] = (3*(Y[k+1]-2*Y[k]+Y[k-1])-R[k-1]/2)/P;
    }

    R[n-1]=0.0;

    for(k=n-2;k>=1;k--)
    {
        R[k] = Q[k]*R[k+1]+R[k];
    }
}

double Transformation::Spline1D(double X, const double *Y, const int n, double *R)
{

    double XX,N,SPLINE;
    int j;

    if (X<1)
    {
        SPLINE = Y[0]+(X-1)*(Y[1]-Y[0]-R[1]/6);
    }
    else if (X>n)
    {
        SPLINE = Y[n] + (X-n)*(Y[n-1]-Y[n-2]+R[n-2]/6);
    }
    else
    {
        XX = modf(X,&N);
        j=(int)N;

        SPLINE = Y[j-1] + XX*((Y[j]-Y[j-1]-R[j-1]/3-R[j]/6)+XX*(R[j-1]/2+XX*(R[j]-R[j-1])/6));
    }
    return SPLINE;
}

/**
* \brief Performs an interpolation.
* \param[in] unite_ini: A MEASURE_UNIT code for the unit of the input coordinates.
* \param[in] l, p: coordinates of the position to interpolate at.
* \param[out] VV: an array of double containing the transformation values. Its memory space must have been allocated by the calling function.
* \param[out] code: the precision code.
* \return 0 if terminated with success.
*/
int Transformation::Interpolate(MEASURE_UNIT unite_ini, double l, double p, double *VV, int *code)
{
    int return_code = 0;
    bool calc_vert_defl = (*code == CALC_VERT_DEFL);

    l = UnitConvert(l, unite_ini, NodeIncCoordUnit);
    p = UnitConvert(p, unite_ini, NodeIncCoordUnit);

    if (InterpolationType == P_BILINEAR)
    {
        return_code = BilinearInterpolation (l, p, VV, code);
    }
    else if (InterpolationType == P_SPLINE)
    {
        if (calc_vert_defl)
            BilinearInterpolation (l, p, VV, code); //provisoire, pour eta xi
        //if (InterpolationOrder == 0) InterpolationOrder = 4;
        return_code = SplineInterpolation(InterpolationOrder, l, p, VV, code);
    }
    return return_code;
}

/**
* \brief Reads TAC (Transformation Ascii Circe) grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderTAC()
{
    int lyt;
    fin >> WestBound >> EastBound >> SouthBound >> NorthBound >> WE_inc >> SN_inc;
    fin >> lyt >> bNodeCoordWritten >> N_val >> bPrecisionCode;
    Layout = (TRANSFO_LAYOUT)lyt;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++)
        fin >> Translation[i];
    getline(fin,Description);

    N_col = (int)NINT((EastBound-WestBound)/WE_inc)+1;
    N_lines = (int)NINT((NorthBound-SouthBound)/SN_inc)+1;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE;
    IncCoordUnit = MU_DEGREE;
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    Encoding = ET_ASCII;

    return;
}

/**
* \brief Reads Gravsoft grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderGravsoft()
{
    fin >> SouthBound >> NorthBound >> WestBound >> EastBound >> SN_inc >> WE_inc ;

    Layout = L_NW2E2S;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;
    N_col = (int)NINT((EastBound-WestBound)/WE_inc)+1;
    N_lines = (int)NINT((NorthBound-SouthBound)/SN_inc)+1;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE;
    IncCoordUnit = MU_DEGREE;
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads EGM text grid (from gridget_1min.exe) header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderEGM()
{
    fin >> NorthBound >> WestBound >> SN_inc >> WE_inc >> N_lines >> N_col;

    WE_inc /= 60.;
    SN_inc /= 60.;

    Layout = L_NW2E2S;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;

    EastBound = WestBound+WE_inc*(N_col-1);
    SouthBound = NorthBound-SN_inc*(N_lines-1);

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE; //...
    IncCoordUnit = MU_DEGREE; //...
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads GTX text grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
/*
Lower-left-Latitude Lower-left-Longitude deltaLatitude deltaLongitude nRows nColumns
the height value of the grid point #1
the height value of the grid point #2
...

where:
Element 	Description
Lower-left Latitude 	The geodetic latitude of the lower left corner of the grid. Decimal degrees, positive North, origin from equator.
Lower-left Longitude 	The geodetic longitude of the lower left corner of the grid. Decimal degrees, positive East, origin from Greenwich.
deltaLatitude 	The latitude spacing between adjacent grid points in the Latitude direction. Decimal degrees, non-zero, positive.
deltaLongitude 	The longitude spacing between adjacent grid points in the Longitude direction. Decimal degrees, non-zero, positive.
nRows 	The number of rows comprising the grid. Non-zero, positive integer.
nColumns 	The number of columns comprising the grid. Non-zero, positive integer.
The height value of  a grid point 	Decimal value.
Units are meters, with exception of the VERTCON transformation grids in millimeters. and NADCON shifting grids in arc seconds.
Null values are expressed by -88.8888 irrespective of the nature of the grid. Null values enable encoding of irregular datum transformation fields, such as those for tidal datums.
*/
void Transformation::ReadHeaderGTX()
{
    fin >> SouthBound >> WestBound >> SN_inc >> WE_inc >> N_lines >> N_col;

    Layout = L_SW2E2N;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;

    EastBound = WestBound+WE_inc*(N_col-1);
    NorthBound = SouthBound+SN_inc*(N_lines-1);

    UnknownValue = -88.8888;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE; //...
    IncCoordUnit = MU_DEGREE; //...
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads GTX binary grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
/*
Binary Transformation Grid File Format

The binary transformation grid files use a layout similar to the ASCII transformation grid
format. The only difference is that grid data are java binary format. Data types used in
binary transformation grid files include:
Type 	Description
Integer 	4 bytes or 32-bit signed
Double 	8 bytes or double-precision 64-bit IEEE 754
Float 	4 bytes or single-precision 32-bit IEEE 754

The transformation grid file in binary format has the following layout. Note: the height
values of grid points are saved as Float since October 1, 2009.
Element 	Type 	Description
Lower-left Latitude 	Double 	The geodetic latitude of the lower left corner of the grid. Decimal degrees, positive North, origin from equator.
Lower-left Longitude 	Double 	The geodetic longitude of the lower left corner of the grid. Decimal degrees, positive East, origin from Greenwich.
deltaLatitude 	Double 	The latitude spacing between adjacent grid points in the Latitude direction. Decimal degrees, non-zero, positive.
deltaLongitude 	Double 	The longitude spacing between adjacent grid points in the Longitude direction. Decimal degrees, non-zero, positive.
nRows 	Integer 	The number of rows comprising the grid. Non-zero, positive integer.
nColumns 	Integer 	The number of columns comprising the grid. Non-zero, positive integer.
The height value of  a grid point 	(Double) Float 	Decimal value.
Units are meters, with exception of the VERTCON transformation grids in millimeters. and NADCON shifting grids in arc seconds.
Null values are expressed by -88.8888 irrespective of the nature of the grid. Null values enable encoding of irregular datum transformation fields, such as those for tidal datums.*/
void Transformation::ReadHeaderGTXbin()
{
    //Encoding = BIG_ENDIAN;
    //InitEndian(false); //on attend du big-endian
    Offset=0;
    fin.seekg(0, std::ios::beg);
    SouthBound = (this->*ReadElementR8)();
    WestBound = (this->*ReadElementR8)();
    SN_inc = (this->*ReadElementR8)();
    WE_inc = (this->*ReadElementR8)();
    N_lines = (this->*ReadElementI4)();
    N_col = (this->*ReadElementI4)();
    Offset = (int)fin.tellg();

    Layout = L_SW2E2N;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;

    EastBound = WestBound+WE_inc*(N_col-1);
    NorthBound = SouthBound+SN_inc*(N_lines-1);

    UnknownValue = -88.8888;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE; //...
    IncCoordUnit = MU_DEGREE; //...
    bPositiveWest = 0;
    //ValueType = 3;
    ValueUnit = MU_METER;
    Encoding = ET_BIG_ENDIAN;
    return;
}

/**
* \brief Reads Surfer text grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderSurfer()
{
    char Keyword[_MAX_PATH_];
    double min_val, max_val;
    fin >> Keyword >> N_lines >> WestBound >> EastBound >> SouthBound >> NorthBound >> min_val >> max_val;

    Layout = L_SW2E2N;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;

    WE_inc = (EastBound-WestBound)/(N_col-1);
    SN_inc = (NorthBound-SouthBound)/(N_lines-1);

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE; //...
    IncCoordUnit = MU_DEGREE; //...
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads BDALTI DIS text grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderDIS()
{
    //int pos=0;
    double X_NW, Y_NW, X_NE, Y_NE, X_SW, Y_SW, X_SE, Y_SE, bid;

    std::string Header, str;
    fin >> str;
    if (str != "ORTH")
        throw std::string("ORTH ")+DataValidationText[DV_TAG_NOT_FOUND];
    fin >> X_NW >> Y_NW >> X_NE >> Y_NE >> X_SW >> Y_SW >> X_SE >> Y_SE;
    if (X_NW != X_SW || Y_NW != Y_NE || X_NE != X_SE || Y_SW != Y_SE)
        throw std::string("non orthogonal grid");
    WestBound = X_NW;
    EastBound = X_NE;
    SouthBound = Y_SW;
    NorthBound = Y_NW;
    fin >> WE_inc >> SN_inc >> str;
    if (str == "METRES")
        NodeIncCoordUnit = IncCoordUnit = MU_METER;
    fin >> N_col >> N_lines;
    fin >> str; //==========
    fin >> bid; //0.0000000
    fin >> bid; //1.0000000
    fin >> str; //==========
    fin >> MinValue >> MaxValue >> str;
    if (str == "METRES")
        ValueUnit = MU_METER;
    fin >> ValueSize;

    //The DIS header is as long as a grid line (blank padded)
    getline(fin,Header);

    Layout = L_NW2E2S;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    bPositiveWest = 0;
    ValueType = 1;
    ValueUnit = MU_METER;

    bDelimiter = 0;
    bFinalLineNumber = 1;
    Encoding = ET_ASCII;

    return;
}


/**
* \brief Reads ESRI text grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderESRI()
{
    bool bFinHeader = false;
    int ncols, nrows, NODATA_value;
    double xllcorner, yllcorner, cellsize;
    std::string str, Keyword;
    while (!bFinHeader)
    {
        fin >> Keyword;
        if (Keyword == "ncols") fin >> ncols;
        else if (Keyword == "nrows") fin >> nrows;
        else if (Keyword == "xllcorner") fin >> xllcorner;
        else if (Keyword == "yllcorner") fin >> yllcorner;
        else if (Keyword == "cellsize") fin >> cellsize;
        else if (Keyword == "NODATA_value")
        {
            fin >> NODATA_value;
            bFinHeader = true;
        }
        else
            throw Keyword+ErrorMessageText[EM_UNFORESEEN]+"in\n"+GridFile;
    }

    bPixelCenter = 1;
    WestBound = (xllcorner + cellsize/2.)/3600.;
    SouthBound = (yllcorner + cellsize/2.)/3600.;
    bPixelCenter = 0;
    WE_inc = cellsize/3600.;
    SN_inc = cellsize/3600.;
    N_col = ncols;
    N_lines = nrows;
    EastBound = WestBound+WE_inc*(N_col-1);
    NorthBound = SouthBound+SN_inc*(N_lines-1);

    Layout = L_NW2E2S;
    bNodeCoordWritten = 0;
    N_val = 1;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;
    UnknownValue = (double)NODATA_value;

    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE;
    IncCoordUnit = MU_DEGREE;
    bPositiveWest = 0;
    ValueType = 1;
    ValueUnit = MU_METER;
    InterpolationType = P_BILINEAR;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads NTV2 text grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderNtv2Asc()
{
    std::string str, FirstDataElement;
    int i, Number_of_Overview_Header_Records, Number_of_Sub_File_Header_Records;
    int Number_of_Grid_Shift_Sub_Files;
    double b;
    char dummy[9];

    getline(fin,str);
    FirstDataElement = str.substr(0,8);
    if (FirstDataElement != "NUM_OREC")
        throw std::string("NUM_OREC ")+DataValidationText[DV_TAG_NOT_FOUND];
    sscanf(str.substr(8).data(),"%d",&Number_of_Overview_Header_Records);
    for (i=0; i<Number_of_Overview_Header_Records-1; i++)
    {
        getline(fin,str);
        FirstDataElement = str.substr(0,8);
        if (FirstDataElement == "NUM_SREC")
        {
            sscanf(str.substr(8).data(),"%d",&Number_of_Sub_File_Header_Records);
        }
        else if (FirstDataElement == "NUM_FILE")
        {
            sscanf(str.substr(8).data(),"%d",&Number_of_Grid_Shift_Sub_Files);
            if (Number_of_Grid_Shift_Sub_Files != 1)
                throw std::string("NUM_FILE != 1")+ErrorMessageText[EM_UNFORESEEN];
        }
        else if (FirstDataElement == "GS_TYPE ")
        {
            if (str.substr(8,7) == "SECONDS")
            {
                ValueUnit = MU_SECOND;
                NodeIncCoordUnit = MU_SECOND;
                IncCoordUnit = MU_SECOND;
            }
            else
            {
                throw std::string("GS_TYPE")+ErrorMessageText[EM_UNFORESEEN];
            }
        }
        else if (FirstDataElement == "VERSION ") {;}
        else if (FirstDataElement == "SYSTEM_F")
        {
            sscanf(str.substr(8).data(),"%s",dummy); //correspondance identifiants ?
            SourceFrameId.assign(dummy);
        }
        else if (FirstDataElement == "SYSTEM_T")
        {
            sscanf(str.substr(8).data(),"%s",dummy); //correspondance identifiants ?
            TargetFrameId.assign(dummy);
        }
        else if (FirstDataElement == "MAJOR_F ") sscanf(str.substr(8).data(),"%lf",&Af);
        else if (FirstDataElement == "MINOR_F ")
        {
            sscanf(str.substr(8).data(),"%lf",&b);
            E2f = 1 - pow(b/Af,2);
        }
        else if (FirstDataElement == "MAJOR_T ") sscanf(str.substr(8).data(),"%lf",&At);
        else if (FirstDataElement == "MINOR_T ")
        {
            sscanf(str.substr(8).data(),"%lf",&b);
            E2t = 1 - pow(b/At,2);
        }
        else throw FirstDataElement+ErrorMessageText[EM_UNFORESEEN]+"in\n"+GridFile;
    }
    for (i=0; i<Number_of_Sub_File_Header_Records; i++)
    {
        getline(fin,str);
        FirstDataElement = str.substr(0,8);
        if (FirstDataElement == "SUB_NAME") Description = str.substr(8);
        else if (FirstDataElement == "PARENT  ") {;}
        else if (FirstDataElement == "CREATED ") {;}
        else if (FirstDataElement == "UPDATED ") {;}
        else if (FirstDataElement == "S_LAT   ") sscanf(str.substr(8).data(),"%lf",&SouthBound);
        else if (FirstDataElement == "N_LAT   ") sscanf(str.substr(8).data(),"%lf",&NorthBound);
        else if (FirstDataElement == "E_LONG  ") sscanf(str.substr(8).data(),"%lf",&WestBound);
        else if (FirstDataElement == "W_LONG  ") sscanf(str.substr(8).data(),"%lf",&EastBound);
        else if (FirstDataElement == "GS_COUNT") sscanf(str.substr(8).data(),"%d",&N_nodes);
        else if (FirstDataElement == "LAT_INC ") sscanf(str.substr(8).data(),"%lf",&SN_inc);
        else if (FirstDataElement == "LONG_INC") sscanf(str.substr(8).data(),"%lf",&WE_inc);
        else throw FirstDataElement+ErrorMessageText[EM_UNFORESEEN]+"in\n"+GridFile;
    }

    bPixelCenter = 0;
    Layout = L_SE2W2N;
    bNodeCoordWritten = 0;
    N_val = 4;
    ValueType = 3;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;
    N_col = (int)NINT((EastBound-WestBound)/WE_inc)+1;
    N_lines = (int)NINT((NorthBound-SouthBound)/SN_inc)+1;
    if (N_nodes != N_col*N_lines)
        throw std::string("N_nodes != N_col*N_lines ")+ErrorMessageText[EM_INCONSISTENT];
    bPositiveWest = 1;
    Encoding = ET_ASCII;
    return;
}

/**
* \brief Reads NTV2 binary grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderNtv2Bin()
{
    std::string FirstDataElement, Chaine;
    int i, Number_of_Overview_Header_Records;
    int Number_of_Sub_File_Header_Records, Number_of_Grid_Shift_Sub_Files;
    double b;

    int OFFSET_A_PRIORI = 352;

    fin.seekg(0, std::ios::beg);
    Offset=0;
    if ((FirstDataElement = ReadStringBin(8)) != "NUM_OREC")
        throw std::string("NUM_OREC ")+DataValidationText[DV_TAG_NOT_FOUND];
    Number_of_Overview_Header_Records = (this->*ReadElementI4)();
    (this->*ReadElementI4)();
    Offset += 16;
    for (i=0; i<Number_of_Overview_Header_Records-1; i++)
    {
        FirstDataElement = ReadStringBin(8);
        if (FirstDataElement == "NUM_SREC")
        {
            Number_of_Sub_File_Header_Records  = (this->*ReadElementI4)();
            ReadI4bin();
        }
        else if (FirstDataElement == "NUM_FILE")
        {
            Number_of_Grid_Shift_Sub_Files  = (this->*ReadElementI4)();
            if (Number_of_Grid_Shift_Sub_Files != 1)
                throw std::string("NUM_FILE != 1")+ErrorMessageText[EM_UNFORESEEN];
            ReadI4bin();
        }
        else if (FirstDataElement == "GS_TYPE ")
        {
            Chaine = ReadStringBin(8);
            if (Chaine == "SECONDS ")
            {
                ValueUnit = MU_SECOND;
                NodeIncCoordUnit = MU_SECOND;
                IncCoordUnit = MU_SECOND;
            }
            else
            {
                throw std::string("GS_TYPE")+ErrorMessageText[EM_UNFORESEEN];
            }
        }
        else if (FirstDataElement == "VERSION ") Chaine = ReadStringBin(8);
        else if (FirstDataElement == "SYSTEM_F") SourceFrameId = ReadStringBin(8); //correspondance identifiants ?
        else if (FirstDataElement == "SYSTEM_T") TargetFrameId = ReadStringBin(8); //correspondance identifiants ?
        else if (FirstDataElement == "MAJOR_F ") Af  = (this->*ReadElementR8)();
        else if (FirstDataElement == "MINOR_F ")
        {
            b  = (this->*ReadElementR8)();
            E2f = 1 - pow(b/Af,2);
        }
        else if (FirstDataElement == "MAJOR_T ") At  = (this->*ReadElementR8)();
        else if (FirstDataElement == "MINOR_T ")
        {
            b  = (this->*ReadElementR8)();
            E2t = 1 - pow(b/At,2);
        }
        else throw "FirstDataElement: "+FirstDataElement+ErrorMessageText[EM_UNFORESEEN];
        Offset += 16;
    }
    for (i=0; i<Number_of_Sub_File_Header_Records; i++)
    {
        FirstDataElement=ReadStringBin(8);
        if (FirstDataElement == "SUB_NAME") Description = ReadStringBin(8);
        else if (FirstDataElement == "PARENT  ") Chaine = ReadStringBin(8);
        else if (FirstDataElement == "CREATED ") Chaine = ReadStringBin(8);
        else if (FirstDataElement == "UPDATED ") Chaine = ReadStringBin(8);
        else if (FirstDataElement == "S_LAT   ") SouthBound  = (this->*ReadElementR8)();
        else if (FirstDataElement == "N_LAT   ") NorthBound  = (this->*ReadElementR8)();
        else if (FirstDataElement == "E_LONG  ") WestBound  = (this->*ReadElementR8)();
        else if (FirstDataElement == "W_LONG  ") EastBound  = (this->*ReadElementR8)();
        else if (FirstDataElement == "LAT_INC ") SN_inc  = (this->*ReadElementR8)();
        else if (FirstDataElement == "LONG_INC") WE_inc  = (this->*ReadElementR8)();
        else if (FirstDataElement == "GS_COUNT")
        {
            N_nodes  = (this->*ReadElementI4)();
            (this->*ReadElementI4)();
        }
        else throw "FirstDataElement: "+FirstDataElement+ErrorMessageText[EM_UNFORESEEN];
        Offset += 16;
    }

    if (Offset != OFFSET_A_PRIORI)
        throw std::string("Offset ")+ErrorMessageText[EM_INCONSISTENT];

    bPixelCenter = 0;
    Layout = L_SE2W2N;
    bNodeCoordWritten = 0;
    N_val = 4;
    ValueType = 3;
    bPrecisionCode = 0;
    Translation.resize(N_val);
    for (int i=0;i<N_val;i++) Translation[i] = 0.;
    N_col = (int)NINT((EastBound-WestBound)/WE_inc)+1;
    N_lines = (int)NINT((NorthBound-SouthBound)/SN_inc)+1;
    if (N_nodes != N_col*N_lines)
        throw std::string("N_nodes != N_col*N_lines ")+ErrorMessageText[EM_INCONSISTENT];

    bPositiveWest = 1;
    //InterpolationType = ;

    return;
}

/**
* \brief Reads TBC (Transformation Binary Circe) grid header ; loads native header metadata ; initializes other metadata.
* \param[in] this->fin is open.
* \param[out]this->fin positioned at the end of the header.
*/
void Transformation::ReadHeaderTBC()
{
    int i;
    std::string str;

    if (!fin.is_open())
    {
//            fin.clear();
        fin.open(GridFile.data(),std::ifstream::in|std::ifstream::binary);
        if (!fin.is_open())
            throw std::string("Transformation::ReadHeaderTBC: ")+ErrorMessageText[EM_CANNOT_OPEN]+" "+GridFile;
    }
//    fin.seekg(0, std::ios::beg);
    std::string Keyword = ReadStringBin(strlen(TBCKEYW)+1);
    if (Keyword != TBCKEYW)
        throw std::string(DataValidationText[DV_TAG_NOT_FOUND]+" "+std::string(TBCKEYW)+"["+Keyword+"]"+"("+GridFile+")");

    int Version = ReadI4bin();

    switch(Version)
    {
    case 2 :
        Offset = (this->*ReadElementI4)();
        WestBound = (this->*ReadElementR8)();
        EastBound = (this->*ReadElementR8)();
        SouthBound = (this->*ReadElementR8)();
        NorthBound = (this->*ReadElementR8)();
        WE_inc = (this->*ReadElementR8)();
        SN_inc = (this->*ReadElementR8)();
        Layout = (TRANSFO_LAYOUT)(this->*ReadElementI4)();
        bNodeCoordWritten = (this->*ReadElementI4)();
        N_val = (this->*ReadElementI4)();
        ValueType = 4;
        bPrecisionCode = (this->*ReadElementI4)();
        Translation.resize(N_val);
        for (i=0;i<N_val;i++) Translation[i] = (this->*ReadElementR8)();
        Description = ReadStringBin((this->*ReadElementI4)());
        break;
    case 3 :
        Offset = ReadI4bin();
        WestBound = (this->*ReadElementR8)();
        EastBound = (this->*ReadElementR8)();
        SouthBound = (this->*ReadElementR8)();
        NorthBound = (this->*ReadElementR8)();
        WE_inc = (this->*ReadElementR8)();
        SN_inc = (this->*ReadElementR8)();
        Layout = (TRANSFO_LAYOUT)(this->*ReadElementI4)();
        bNodeCoordWritten = (this->*ReadElementI4)();
        N_val = (this->*ReadElementI4)();
        ValueType = (this->*ReadElementI4)(); //difference with version 2
        bPrecisionCode = (this->*ReadElementI4)();
        Translation.resize(N_val);
        for (i=0;i<N_val;i++) Translation[i] = (this->*ReadElementR8)();
        Description = ReadStringBin((this->*ReadElementI4)());
        break;
    default :
        throw asString(Version)+" "+ErrorMessageText[EM_BAD_VERSION_NUMBER]+"\n("+GridFile+")";
        break;
    }

    N_col = (int)NINT((EastBound-WestBound)/WE_inc)+1;
    N_lines = (int)NINT((NorthBound-SouthBound)/SN_inc)+1;

    bPixelCenter = 0;
    N_nodes = N_col*N_lines;
    NodeIncCoordUnit = MU_DEGREE;
    IncCoordUnit = MU_DEGREE;
    bPositiveWest = 0;
    ValueUnit = MU_METER;
    return;
}

/**
* \brief Writes TBC (Transformation Binary Circe) grid header.
* \param[in] this->fout is open.
* \param[out] this->fout positioned at the end of the header.
* \return 0 if terminated with success.
*/
int Transformation::WriteHeaderTBC()
{
    //string Keyword = TBCKEYW;
    //WriteStringBin (Keyword, Keyword.size());
    char *magique;
    magique=(char*)malloc((strlen(TBCKEYW)+1)*sizeof(char));
    sprintf(magique,"%s",TBCKEYW);
    WriteStringBin(magique,strlen(TBCKEYW)+1);
    free(magique);
    (this->*WriteElementI4)(int(VERSION_BIN));
    int DebutHeader = (int)fout.tellp();
    Offset = 0;
    (this->*WriteElementI4)(Offset);
    (this->*WriteElementR8)(WestBound);
    (this->*WriteElementR8)(EastBound);
    (this->*WriteElementR8)(SouthBound);
    (this->*WriteElementR8)(NorthBound);
    (this->*WriteElementR8)(WE_inc);
    (this->*WriteElementR8)(SN_inc);
    (this->*WriteElementI4)(Layout);
    (this->*WriteElementI4)(bNodeCoordWritten);
    (this->*WriteElementI4)(N_val);
    (this->*WriteElementI4)(ValueType);
    (this->*WriteElementI4)(bPrecisionCode);
    for (int i=0;i<N_val;i++) (this->*WriteElementR8)(Translation[i]);
    (this->*WriteElementI4)(Description.size());
    WriteStringBin(Description,Description.size());
    Offset = (int)fout.tellp();
    fout.seekp(DebutHeader, std::ios::beg);
    (this->*WriteElementI4)(Offset);
    fout.seekp(Offset, std::ios::beg);

    //Encoding ?;
    return 0;
}


/**
* \brief Modifies (to unify further processing) and validates grid metadata.
* \param[in] bModify: true if modifications are allowed.
* \return a std::string containing a list of invalid metadata, that is empty if all the metadata are valid.
*/
std::string Transformation::ModifyAndValidateMetaData(bool bModify)
{
    std::string Message = std::string("");

    //Some modifications to unify processing
    if (bModify)
    {
        //NTV2 special: longitudes in mainstream direction
        if (bPositiveWest)
        {
            double doudou = WestBound;
            WestBound = -EastBound;
            EastBound = -doudou;
        }

        //Bounds regularization
        if ((NodeIncCoordUnit == MU_DEGREE) && (WestBound > 360. || EastBound > 360.))
        {
            WestBound -= 360.;
            EastBound -= 360.;
        }

        //Units converted into RADIANS to allow a direct interpolation of
        //points without unit conversion.
        WE_inc = UnitConvert(WE_inc, IncCoordUnit, MU_RADIAN);
        SN_inc = UnitConvert(SN_inc, IncCoordUnit, MU_RADIAN);
        WestBound = UnitConvert(WestBound, NodeIncCoordUnit, MU_RADIAN);
        EastBound = UnitConvert(EastBound, NodeIncCoordUnit, MU_RADIAN);
        SouthBound = UnitConvert(SouthBound, NodeIncCoordUnit, MU_RADIAN);
        NorthBound = UnitConvert(NorthBound, NodeIncCoordUnit, MU_RADIAN);
        NodeIncCoordUnit = MU_RADIAN;
    }

    // Other initializations

    NodeSize = ValueSize*N_val;
    if (bPrecisionCode) NodeSize += sizeof(int);

    setArea();

    // Validation

    // Internal management
    if (Encoding<=ET_UNDEF || Encoding>=ET_COUNT)
        Message += "+ Encoding "+asString(Encoding)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (LoadingType<=LD_UNDEF || LoadingType>=LD_COUNT)
        Message += "+ LoadingType "+asString(LoadingType)+ErrorMessageText[EM_NOT_VALID]+"\n";

    // General data
    if (GridType<=GF_UNDEF || GridType>=GF_COUNT)
        Message += "+ GridType "+asString(GridType)+ErrorMessageText[EM_NOT_VALID]+"\n";

    // Node data
    if (Layout<=L_UNDEF || Layout>=L_COUNT)
        Message += "+ Layout "+asString(Layout)+ErrorMessageText[EM_NOT_VALID]+"\n";
//    if (NodeCRStype<=CT_UNDEF || NodeCRStype>=CT_COUNT)
//        Message += "+ NodeCRStype "+asString(NodeCRStype)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (IncCoordUnit<=MU_UNDEF || IncCoordUnit>=MU_COUNT)
        Message += "+ IncCoordUnit "+asString(IncCoordUnit)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (NodeIncCoordUnit<=MU_UNDEF || NodeIncCoordUnit>=MU_COUNT)
        Message += "+ NodeIncCoordUnit "+asString(NodeIncCoordUnit)+ErrorMessageText[EM_NOT_VALID]+"\n";

    if (WestBound >= EastBound)
        Message += std::string("+ WestBound >= EastBound")+ErrorMessageText[EM_INCONSISTENT]+"\n";
    if (SouthBound >= NorthBound)
        Message += std::string("+ SouthBound >= NorthBound")+ErrorMessageText[EM_INCONSISTENT]+"\n";
    if (NodeIncCoordUnit == MU_SECOND || NodeIncCoordUnit == MU_DEGREE)
    {
        double long_max = 360.*DEG2RAD, lat_max = 90.*DEG2RAD;
        if (fabs(WestBound)>long_max || fabs(EastBound)>long_max || fabs(SouthBound)>lat_max
            || fabs(NorthBound)>lat_max || WestBound == EastBound || SouthBound == NorthBound)
            Message += std::string("+ WestBound / EastBound / SouthBound / NorthBound")+ErrorMessageText[EM_INCONSISTENT]+"\n";
    }
    if (WE_inc < 1e-12)
        Message += "+ WE_inc "+asString(WE_inc)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (SN_inc < 1e-12)
        Message += "+ SN_inc "+asString(SN_inc)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (fabs(double(N_col - NINT((EastBound-WestBound)/WE_inc) - 1)) > 1e-10)
        Message += std::string("+ N_col / EastBound / WestBound / WE_inc")+ErrorMessageText[EM_INCONSISTENT]+"\n";
    if (fabs(double(N_lines - NINT((NorthBound-SouthBound)/SN_inc) - 1)) > 1e-10)
        Message += std::string("+ N_lines / NorthBound / SouthBound / SN_inc")+ErrorMessageText[EM_INCONSISTENT]+"\n";

    // Value data
    if (crs_type<=CT_UNDEF || crs_type>=CT_COUNT)
        Message += "+ crs_type "+asString(crs_type)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (Epoch != 0 && (Epoch<=1900 || Epoch>=2100))
        Message += "+ Epoch "+asString(Epoch)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (!(checkPrecisionCodeDefinition(PrecisionCodeInit)))
        Message += "+ PrecisionCodeInit "+asString(PrecisionCodeInit)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (N_val<0 || N_val>14)
        Message += "+ N_val "+asString(N_val)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (ValueType<1 || ValueType>4)
        Message += "+ ValueType "+asString(ValueType)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (ValueUnit<=MU_UNDEF || ValueUnit>=MU_COUNT)
        Message += "+ ValueUnit "+asString(ValueUnit)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (InterpolationType<=P_UNDEF || InterpolationType>=P_COUNT)
        InterpolationType = P_BILINEAR;
//        Message += "+ InterpolationType "+asString(InterpolationType)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (InterpolationType == P_SPLINE && InterpolationOrder <4)
        Message += "+ InterpolationOrder "+asString(InterpolationOrder)+ErrorMessageText[EM_NOT_VALID]+"\n";

    // Geodetic systems
    if (NodeFrameInfo<=F_UNDEF || NodeFrameInfo>=F_COUNT) Message += "+ NodeFrameInfo "+asString(NodeFrameInfo)+ErrorMessageText[EM_NOT_VALID]+"\n";
    if (NodeFrameInfo==F_OTHER && NodeFrameId=="") Message += "+ NodeFrameId "+ErrorMessageText[EM_UNINITIALIZED]+"\n";

    if (Message != "" && GridFile != "")
        Message = GridFile+" :\n"+Message;
    return Message;
}

/**
* \brief Initializes grid metadata, validates them and load the grid according to the loading type.
* \param[in] lt: a LOADING_TYPE code for the loading type. \n
  LD_BINARY   open the binary file for further interpolation \n
  LD_ARRAY    load into memory for further interpolation \n
  LD_HEADER   read metadata only (header and xml) \n
  LD_XML      read metadata only (xml only)
* \return 0 if terminated with success.
* \throw str A std:string containing the error message if any.
* \remark    Metadata initialization:
    This is a delicate matter. The following is done :

    - Initialization of GridType according to grid file extension.

    - Reading the XML metadata file if it exists (function ReadXML).
      Very few metadata must be set in the XML file (since most of them is set otherwise):
      NTV2 grids: Their endianness must be set in the XML file.
      All grids: By default, InterpolationType = P_BILINEAR. If not, set it in the XML file.
      Nevertheless, a complete XML file may help in user's understanding.

    - Initialization of encoding type and of internal data & function management.

    - In format-dependant functions:
      Read the native grid header ; load native header metadata ; initialize derived
      metadata ; initialize format-dependant fixed metadata.

    - In function ModifyAndValidateMetaData:
      Some metadata may be modified to unify processings.
      Then the metadata validation is carried out.

    - Last but not least:
      The geodetic metadata (source and target CRS etc.) is not initialized within
      the Transformation class, but from the general metadata file (IGNF.xml etc.)

 */
int Transformation::LoadGrid(LOADING_TYPE lt)
{
    std::string Message="";
    int ret;
    bool bLectureXML = false;
    struct stat buffer;

    if (Layout == L_CONSTANT)
        return 0;

    // 1. GridFile must be initialized.

    if (GridFile == "")
        throw std::string("GridFile ")+ErrorMessageText[EM_UNINITIALIZED];
    if (stat (GridFile.c_str(), &buffer) != 0)
        throw std::string("Transformation::LoadGrid: ")+GridFile+ErrorMessageText[EM_NOT_FOUND]+"\n";
    std::ifstream finc(GridFile.data(),std::ios::in);
    if (!finc.good())
        //data race
        throw std::string("Transformation::LoadGrid: ")+ErrorMessageText[EM_CANNOT_OPEN]+"\n"+GridFile;
    finc.close();


    // 2. Metadata initialization.

    // 2.1. Initialization of GridType

    LoadingType = lt;

    if (GridType == GF_UNDEF)
        InitGridType(0);

    // 2.2. Reading the XML metadata file if it exists.

    if ((ret=ReadXML("")) == 0) bLectureXML = true;

    // 2.3. Initialization of GridType

      if (GridType == GF_UNDEF)
        InitGridType(0);

    InitAccesElement(false);

    if (LoadingType == LD_XML)
    {
        if (ret != 0) throw std::string("XML ")+ErrorMessageText[EM_NOT_FOUND];
        return 0;
    }

    // 2.4. Opening the file and reading the header.


    switch (GridType)
    {
        case GF_SGN:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderTAC();
        break;
        case GF_GRAVSOFT:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderGravsoft();
        break;
        case GF_DIS:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderDIS();
        break;
        case GF_EGM:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderEGM();
        break;
        case GF_SURFER:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderSurfer();
        break;
        case GF_NTV2_ASC:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderNtv2Asc();
            if (LoadingType == LD_BINARY)
                LoadingType = LD_ARRAY; //don't convert into binary grid
        break;
        case GF_SGN_BIN:
            fin.open(GridFile.data(),std::ios::in|std::ios::binary);
            ReadHeaderTBC();
        break;
        case GF_NTV2_BIN_L:
        case GF_NTV2_BIN_B:
            fin.open(GridFile.data(),std::ios::in|std::ios::binary);
            ReadHeaderNtv2Bin();
        break;
        case GF_ESRI:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderESRI();
        break;
        case GF_GTX_ASC:
            fin.open(GridFile.data(),std::ios::in);
            ReadHeaderGTX();
        break;
        case GF_GTX_BIN:
            fin.open(GridFile.data(),std::ios::in|std::ios::binary);
            ReadHeaderGTXbin();
        break;
        default:
            throw std::string("GridType ")+ErrorMessageText[EM_UNINITIALIZED];
        break;
    }
    if (fin.fail())
        throw std::string("Transformation::LoadGrid: error reading header");


    // 3. Metadata validation.



    //if (Encoding == ET_ASCII && LoadingType == LD_BINARY)
    //then we'll switch to a binary grid file and come back
    //here and validate the metadata.

    if (Encoding != ET_ASCII || LoadingType != LD_BINARY)
    if ((Message=ModifyAndValidateMetaData(true)) != "")
    {
        if (bLectureXML) Message += "\n(from header or XML)";
        throw Message;
    }


    // 4. Loading the grid.

    if (LoadingType == LD_HEADER)
    {
        if (fin.is_open())
        {
            fin.close();
            fin.clear();
        }
        return 0;
    }
    if (Encoding == ET_ASCII)
    {
        LoadTextFile();
        if (LoadingType == LD_BINARY)
            LoadGrid(LoadingType);
    }
    else
        LoadBinaryFile();

    return 0;
}


/**
* \brief Loads values from any text grid file into memory or in a TBC file according to LoadingType.
* \param[in] this->fin: is open and positioned at the end of the header.
* \param[out] One of the data vectors: is filled according to the values type.
* \return 0 if terminated with success.
*/
int Transformation::LoadTextFile()
{
    double bid1, bid2, dV;
    int i=0, j, k, l, entier, curcol, pc=0;
    short sV;
    int iV;
    float fV;
    char vava[20];
    int slength=0;
    std::string s, t, u;
    if (mb!=NULL)
        u = mb->getCurrentMessage();

    struct stat buffer;

    bool BigEndianSystem = true;
    unsigned char SwapTest[2] = {1,0};
    if(*(short *)SwapTest == 1)
    BigEndianSystem = false;

    switch (LoadingType)
    {
    case LD_BINARY :
        //Encoding
        if (BigEndianSystem)
            Encoding = ET_BIG_ENDIAN;
        else
            Encoding = ET_LITTLE_ENDIAN;

        //The text grid file is already open
        GridType = GF_SGN_BIN;
        t = GridFile;
        GridFile = replaceExtension(GridFile, ListeExtensionTypeGrille[GridType]);
        if (stat (GridFile.c_str(), &buffer) == 0)
        {
            fin.close();
            fin.clear();
            return 0;
        }
        s = "Création du fichier binaire ";
        if (mb!=NULL)
            mb->messageEdit(t+" => "+GridFile+" ...");
        break;

    case LD_ARRAY :
        Encoding = ET_ASCII;
        s = "\rLoading grid ";
        break;

    default :
        break;
    }

//    std::cout << s << " " << GridFile << "... ";
    int prgStep = N_col*N_lines/100;

    switch (LoadingType)
    {
    case LD_BINARY :
        fout.open(GridFile.data(),std::ios::out|std::ios::binary);
        if (!fout.is_open())
            throw ErrorMessageText[EM_CANNOT_OPEN]+"\n"+GridFile;
        InitAccesElement(true);
        WriteHeaderTBC();
        break;

    case LD_ARRAY :
        switch (ValueType)
        {
            case 1:
//            VecValI2.resize(N_col*N_lines*N_val);
            VecValI2.reserve(N_col*N_lines*N_val);
                break;
            case 2:
//            VecValI4.resize(N_col*N_lines*N_val);
            VecValI4.reserve(N_col*N_lines*N_val);
                break;
            case 3:
//            VecValR4.resize(N_col*N_lines*N_val);
            VecValR4.reserve(N_col*N_lines*N_val);
                break;
            case 4:
//            VecValR8.resize(N_col*N_lines*N_val);
            VecValR8.reserve(N_col*N_lines*N_val);
                break;
            default:
                break;
        }
//        VecPrec.resize(N_col*N_lines);
        VecPrec.reserve(N_col*N_lines);
        break;

    default :
        break;
    }

    curcol = j = l = 0;
    do
    {
        if (curcol++ == N_col)
        {
            if (bFinalLineNumber)
            {
                fin >> vava;
                curcol = 1;
            }
        }

        if (bNodeCoordWritten)
            fin >> bid1 >> bid2;

        for (k=0;k<N_val;k++)
        {
            switch (ValueType)
            {
                case 1:
                    if (!bDelimiter)
                    {
                        fin.get(vava,ValueSize+1);
                        sscanf(vava,"%hd",&sV);
                    }
                    else
                        fin >> sV;
                    switch (LoadingType)
                    {
                    case LD_BINARY :
                        WriteI2bin(sV);
                        break;
                    case LD_ARRAY :
//                        VecValI2[j++] = sV;
                        VecValI2.push_back(sV);
                        break;
                    default :
                        break;
                    }
                    break;
                case 2:
                    if (!bDelimiter)
                    {
                        fin.get(vava,ValueSize+1);
                        sscanf(vava,"%d",&iV);
                    }
                    else
                        fin >> iV;
                    switch (LoadingType)
                    {
                    case LD_BINARY :
                        WriteI4bin(iV);
                        break;
                    case LD_ARRAY :
//                        VecValI4[j++] = iV;
                        VecValI4.push_back(iV);
                        break;
                    default :
                        break;
                    }
                    break;
                case 3:
                    if (!bDelimiter)
                    {
                        fin.get(vava,ValueSize+1);
                        sscanf(vava,"%f",&fV);
                    }
                    else
                        fin >> fV;
                    switch (LoadingType)
                    {
                    case LD_BINARY :
                        WriteR4bin(fV);
                        break;
                    case LD_ARRAY :
//                        VecValR4[j++] = fV;
                        VecValR4.push_back(fV);
                        break;
                    default :
                        break;
                    }
                    break;
                case 4:
                    if (!bDelimiter)
                    {
                        fin.get(vava,ValueSize+1);
                        sscanf(vava,"%lf",&dV);
                    }
                    else
                        fin >> dV;
                    switch (LoadingType)
                    {
                    case LD_BINARY :
                        WriteR8bin(dV);
                        break;
                    case LD_ARRAY :
//                        VecValR8[j++] = dV;
                        VecValR8.push_back(dV);
                        break;
                    default :
                        break;
                    }
                    break;
                default:
                    break;
            }
        }

        if (bPrecisionCode) //code de précision
        {
            fin >> entier;

            switch (LoadingType)
            {
            case LD_BINARY :
                WriteI4bin(entier);
                break;
            case LD_ARRAY :
                VecPrec.push_back(entier);
//                VecPrec[l] = entier;
                break;
            default :
                break;
            }
        }
        l++;

        if(++i==prgStep)
        {
            i=0;
            std::string mm = t+" => "+GridFile+" ... "+asString(pc++)+"%";
            slength = mm.length();
            if (mb!=NULL)
                mb->messageEdit(mm);
            else
                std::cout << "\r"+mm;
        }
    }
    while (!fin.eof() && l<N_col*N_lines);

    switch (LoadingType)
    {
    case LD_BINARY :
    case LD_ARRAY :
        break;
    default :
        break;
    }
    fin.close();
    fin.clear();
    fout.close();
    fout.clear();
    if (mb!=NULL)
        mb->messageEdit(u);

    if (mb==NULL)
        std::cout << "\r" << std::setw(slength) << " " << "\r";

    return 0;
}


/**
* \brief Loads values from a binary grid file into memory.
* \param[in] this->fin: is open and positioned at the end of the header.
* \param[out] One of the data vectors: is filled according to the values type.
* \return 0 if terminated with success.
*/
int Transformation::LoadBinaryFile()
{
    int i=0, j, k, l;
    std::string s = "Loading grid ";

    if (LoadingType == LD_BINARY)
    {
        fin.open(GridFile.data(),std::ios::in|std::ios::binary);
        if (!fin.is_open())
            throw std::string("Transformation::LoadBinaryFile: ")+ErrorMessageText[EM_CANNOT_OPEN]+" "+GridFile;
        return 0;
    }
//    InitGridType(0);
//    if (GridType == GF_SGN_BIN)
//    {
//        ReadHeaderTBC();
//    }
//    if (GridType == GF_NTV2_BIN_L || GridType == GF_NTV2_BIN_B)
//    {
//        ReadHeaderNtv2Bin();
//    }

    //ValideHeader();

    std::cout << s << " " << GridFile << "...";
    int prgStep = N_col*N_lines/100;

    switch (ValueType)
    {
        case 1:
            VecValI2.resize(N_col*N_lines*N_val);
            break;
        case 2:
            VecValI4.resize(N_col*N_lines*N_val);
            break;
        case 3:
            VecValR4.resize(N_col*N_lines*N_val);
            break;
        case 4:
            VecValR8.resize(N_col*N_lines*N_val);
            break;
        default:
            break;
    }
    VecPrec.resize(N_col*N_lines);

    fin.seekg(Offset, std::ios::beg);
    j = l = 0;
    do
    {
        for (k=0;k<N_val;k++)
        {
            switch (ValueType)
            {
                case 1:
                    VecValI2[j++] = ReadI2bin();
                    break;
                case 2:
                    VecValI4[j++] = ReadI4bin();
                    break;
                case 3:
                    VecValR4[j++] = ReadR4bin();
                    break;
                case 4:
                    VecValR8[j++] = ReadR8bin();
                    break;
                default:
                    break;
            }
        }
        if (bPrecisionCode) VecPrec[l] = ReadI4bin();
        l++;

        if(++i==prgStep)
        {
            i=0;
//            cout << "|";
        }
    }
    while (!fin.eof() && l<N_col*N_lines);

//    std::cout << "\n";

    Encoding = ET_UNDEF;
    fin.close();
    return 0;
}

/**
* \brief Applies a 7-parameter transformation (a similarity) to a geocentric coordinate set.
* \param[in] oa: An OPERATION_APPLICATION code to use the relevant transformation formula.
* \param[in] tdr: A 7-parameter transformation double vector.
* \param[in] x1, y1, z1: input geocentric coordinate set.
* \param[out] x2, y2, z2: output geocentric coordinate set.
*/
void Transformation::Transform7(OPERATION_APPLICATION oa,
	const std::vector<double>& tdr,
	double  x1, double  y1, double  z1,
	double *x2, double *y2, double *z2)
{
	if (oa == OA_DIRECT)
	{
		*x2 = x1 + tdr[0] + tdr[3] * x1 - tdr[6] * y1 + tdr[5] * z1;
		*y2 = y1 + tdr[1] + tdr[6] * x1 + tdr[3] * y1 - tdr[4] * z1;
		*z2 = z1 + tdr[2] - tdr[5] * x1 + tdr[4] * y1 + tdr[3] * z1;
	}
	else if (oa == OA_REVERSE_SAME_METHOD)
	{
		*x2 = x1 - tdr[0] - tdr[3] * x1 + tdr[6] * y1 - tdr[5] * z1;
		*y2 = y1 - tdr[1] - tdr[6] * x1 - tdr[3] * y1 + tdr[4] * z1;
		*z2 = z1 - tdr[2] + tdr[5] * x1 - tdr[4] * y1 - tdr[3] * z1;
	}
	else if (oa == OA_REVERSE_SAME_PARAMETERS)
	{
		// alg0063

		double x, y, z;
		double e, denom;

		//Translation first
		x1 -= tdr[0];
		y1 -= tdr[1];
		z1 -= tdr[2];

		/* inversion of matrix A such as AX = (1+d)X + R^X :
		1+d   -rz    ry
		rz	  1+d   -rx
		-ry    rx   1+d
		*/

		e = 1 + tdr[3];
		denom = e * (e*e + tdr[4] * tdr[4] + tdr[5] * tdr[5] + tdr[6] * tdr[6]);	/* proche de 1 donc non nul */
		x = ((e*e + tdr[4] * tdr[4]) * x1 + (e*tdr[6] + tdr[4] * tdr[5]) * y1 + (-e*tdr[5] + tdr[4] * tdr[6]) * z1) / denom;
		y = ((-e*tdr[6] + tdr[5] * tdr[4]) * x1 + (e*e + tdr[5] * tdr[5]) * y1 + (e*tdr[4] + tdr[5] * tdr[6]) * z1) / denom;
		z = ((e*tdr[5] + tdr[6] * tdr[4]) * x1 + (-e*tdr[4] + tdr[6] * tdr[5]) * y1 + (e*e + tdr[6] * tdr[6]) * z1) / denom;

		*x2 = x;
		*y2 = y;
		*z2 = z;
	}
}

/**
* \brief Applies a 3-parameter transformation (a translation) to a geocentric coordinate set.
* \param[in] oa: An OPERATION_APPLICATION code to use the relevant transformation formula.
* \param[in] tdr: A 3-parameter transformation double vector.
* \param[in] x1, y1, z1: input geocentric coordinate set.
* \param[out] x2, y2, z2: output geocentric coordinate set.
*/
void Transformation::Transform3(OPERATION_APPLICATION oa,
    const std::vector<double>& t, const double& rate_factor,
	double  x1, double  y1, double  z1,
	double *x2, double *y2, double *z2)
{
	if (oa == OA_DIRECT)
	{
        *x2 = x1 + t[0]*rate_factor;
        *y2 = y1 + t[1]*rate_factor;
        *z2 = z1 + t[2]*rate_factor;
	}
	else // (oa==R_REVERSE_SAME_METHOD || oa==R_REVERSE_SAME_PARAMETERS)
	{
        *x2 = x1 - t[0]*rate_factor;
        *y2 = y1 - t[1]*rate_factor;
        *z2 = z1 - t[2]*rate_factor;
	}
}

/**
* \brief Outputs the source and target epochs in an application context.
* \param[out] epF: pointer to the source epoch.
* \param[out] epT: pointer to the target epoch.
*/
void Transformation::GetApplicationFrameEpochs(double *epF, double *epT)
{
    if (getOperationApplication() == OA_DIRECT)
    {
        *epF = EpochF;
        *epT = EpochT;
    }
    else
    {
        *epF = EpochT;
        *epT = EpochF;
    }
}

/**
* \brief Selects a Transformation relevant to the given coordinates within a Transformation vector.
* \param[in] l: given longitude.
* \param[in] p: given latitude.
* \param[in] vTransfo: A Transformation vector.
* \return A std::vector<Transformation>::iterator pointing to the selected Transformation.
* \remark The input transformations are all relevant regarding the CRSs involved. If there are more than one transformation, the selection depends on the point processed. Here is the selection.
* \remark Rule 1: The point is within the transformation boundaries.
* \remark Rule 2: Selection of the transformation that has the smallest area, from the following principle: a more precise transfo may exists on a smallest area.
*/
//std::vector<Transformation>::const_iterator SelectTransfo(double l, double p,
//                                                           const std::vector<Transformation>& vTransfo)
std::vector<Transformation>::iterator Transformation::SelectTransfo(double l, double p,
	std::vector<Transformation>& vTransfo)
{
	double area = 1e99;
	//    std::vector<Transformation>::const_iterator itT, itTsel=vTransfo.end();
	std::vector<Transformation>::iterator itT, itTsel = vTransfo.end();
	for (itT = vTransfo.begin(); itT<vTransfo.end(); itT++)
	{
		if (itT->isInside(l, p)
			&& itT->getArea() < area)
		{
			itTsel = itT;
			area = itT->getArea();
		}
	}
	return itTsel;
}

/**
* \brief Applies a Geodetic Transformation to a coordinate set.
* \param[in] l1, p1, h1: input geographic coordinate set.
* \param[in] x1, y1, z1: input geocentric coordinate set.
* \param[in] vGeodeticTransfo: A vector of Transformation instances.
* \param[out] l2, p2, h2: output geographic coordinate set.
* \param[out] x2, y2, z2: output geocentric coordinate set.
* \param[out] precision_code: the int precision code.
* \return An OPERATION_MESSAGE which is a code for an error message if it is not OM_OK.
*/
OPERATION_MESSAGE Transformation::ApplyGeodeticTransformation(double l1, double p1, double h1, double x1, double y1, double z1,
	double *l2, double *p2, double *h2, double *x2, double *y2, double *z2,
	//                                              const std::vector<Transformation>& vGeodeticTransfo, int *precision_code)
	std::vector<Transformation>& vGeodeticTransfo, int *precision_code, int followUpTransfos)
{
	double conv = 1e-7;
	//double RTS[7]={0.,0.,0.,0.,0.,0.,1.},
	std::vector<double> RTS;
	int i, code = 0, drctn = 1;
	bool done;
	double *VV;
	//    std::vector<Transformation>::const_iterator itTsel = SelectTransfo(l1 ,p1 ,vGeodeticTransfo); //frame ?
	std::vector<Transformation>::iterator itTsel = SelectTransfo(l1, p1, vGeodeticTransfo); //frame ?
	if (itTsel == vGeodeticTransfo.end())
		return OM_GEODETIC_TRANSFO_NOT_FOUND_FOR_THIS_POINT;
	//std::cout << itTsel->getCirceId() + "  ";

	//l1 += itTsel->getPreviousMeridianTranslation(); //generally 0

	if (itTsel->getLayout() == L_CONSTANT)
	{
		if (itTsel->getValuesNumber() == 3)
            Transform3(itTsel->getOperationApplication(), itTsel->getInitValues(),
                       itTsel->RateFactor, x1, y1, z1, x2, y2, z2);
		else if (itTsel->getValuesNumber() == 7 || itTsel->getValuesNumber() == 14)
			Transform7(itTsel->getOperationApplication(), itTsel->getInitValues(), x1, y1, z1, x2, y2, z2);
		*precision_code = itTsel->getPrecisionCodeInit();
		if (followUpTransfos == 1)
		{
			for (i = 0; i < itTsel->getValuesNumber(); i++)
                itTsel->FollowUp += asString(itTsel->getInitValues()[i]) + " ";
            //itTsel->FollowUp += "\n";
		}
        //plate motion model (source and target frames are the same): erase artifactual vertical shift
        if (itTsel->getIGNid().substr(0,3) == std::string("PMM"))
        {
            CartGeo56(*x2, *y2, *z2, itTsel->At, itTsel->E2t, l2, p2, h2);
            GeoCart(itTsel->At, itTsel->E2t, *l2, *p2, h1, x2, y2, z2);
        }
    }
	else
	{
        VV = (double*)malloc(itTsel->getValuesNumber()*sizeof(double)); //à faire une seule fois au début

		//Official transformation : the geographic coordinates must be expressed in 
		//the target CRS.
		//if (itTsel->getStatus()==S_AUTHORITATIVE)
		//if (itTsel->getGridType()==GF_SGN)
		if (itTsel->getNodeFrameInfo() == F_TARGET)
		{
			if (itTsel->getOperationApplication() == OA_DIRECT)
			{
				//Begin with approximate (mean) transformation
				RTS = itTsel->getInitValues();
				do
				{
                    Transform3(itTsel->getOperationApplication(), RTS,
                               itTsel->RateFactor, x1, y1, z1, x2, y2, z2);
					CartGeo56(*x2, *y2, *z2, itTsel->At, itTsel->E2t, l2, p2, h2);
					//Maybe transform grid units in radians first of all to avoid these conversions
					code = itTsel->Interpolate(MU_RADIAN, *l2, *p2, VV, precision_code);
					done = true;
					for (i = 0; i<itTsel->N_val; i++) if (fabs(RTS[i] - VV[i]) >= conv) done = false;
					if (code == 2)
						done = true;
					for (i = 0; i<itTsel->N_val; i++) RTS[i] = VV[i];
				} while (!done);
			}
			else //Current coordinates are in the target CRS of the transformation.
			{
				CartGeo56(x1, y1, z1, itTsel->At, itTsel->E2t, &l1, &p1, &h1);
				//Maybe transform grid units in radians first of all to avoid these conversions
				code = itTsel->Interpolate(MU_RADIAN, l1, p1, VV, precision_code);
				RTS.reserve(itTsel->N_val);
				for (i = 0; i<itTsel->N_val; i++) RTS.push_back(VV[i]);
                Transform3(itTsel->getOperationApplication(), RTS,
                           itTsel->RateFactor, x1, y1, z1, x2, y2, z2);
			}
		}
		else if (itTsel->getGridType() == GF_NTV2_BIN_L
			|| itTsel->getGridType() == GF_NTV2_BIN_B
			|| itTsel->getGridType() == GF_NTV2_ASC)
		{
			code = itTsel->Interpolate(MU_RADIAN, l1, p1, VV, precision_code);
			if (itTsel->getOperationApplication() != OA_DIRECT)
				drctn = -1;
			*l2 = l1 - drctn*VV[1] * SEC2RAD;
			*p2 = p1 + drctn*VV[0] * SEC2RAD;
		}

		if (followUpTransfos == 1)
		{
			for (i = 0; i < itTsel->getValuesNumber(); i++)
				itTsel->FollowUp += asString(VV[i]) + " ";
            //itTsel->FollowUp += "\n";
		}
	}

	if (code != 0)
		return OM_INTERPOLATION_FAILED;

	return OM_OK;
}


/**
* \brief Applies a Vertical Transformation to a coordinate set.
* \param[in] l: input longitude.
* \param[in] p: input latitude.
* \param[in] w1: input vertical coordinate.
* \param[in] vVerticalTransfo: A vector of Transformation instances.
* \param[out] w2: output vertical coordinate.
* \param[out] code: the int precision code.
* \return An OPERATION_MESSAGE which is a code for an error message if it is not OM_OK.
*/
OPERATION_MESSAGE Transformation::ApplyVerticalTransformation(double l, double p, double w1, double *w2,
    std::vector<Transformation>& vVerticalTransfo, int *code, int followUpTransfos,
    std::string *pInfo, std::string *pInfo2)
	//const std::vector<Transformation>& vVerticalTransfo, int *precision_code)
{
	//    std::vector<Transformation>::const_iterator itTsel = SelectTransfo(l ,p ,vVerticalTransfo); //frame ?
	std::vector<Transformation>::iterator itTsel = SelectTransfo(l, p, vVerticalTransfo); //frame ?
	if (itTsel == vVerticalTransfo.end())
		return OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THIS_POINT;
    int return_code = 0, drctn = 1;
    double VV[3];

	if (itTsel->getOperationApplication() != OA_DIRECT)
		drctn = -1;

    pInfo->assign(itTsel->getSourceFrameId());
    pInfo2->assign(itTsel->getGridFile());
	if (itTsel->getLayout() == L_CONSTANT)
	{
		*w2 = w1 + drctn*itTsel->getInitValues()[0];
        *code = itTsel->getPrecisionCodeInit();
		if (followUpTransfos == 1)
			itTsel->FollowUp += asString(drctn*itTsel->getInitValues()[0]) + "\n";
	}
	else
	{
        return_code = itTsel->Interpolate(MU_RADIAN, l, p, VV, code);
		*w2 = w1 + drctn*VV[0];
		if (followUpTransfos == 1)
            itTsel->FollowUp += asString(drctn*VV[0]) + "\n";
	}
    if (return_code != 0)
		return OM_INTERPOLATION_FAILED;
	return OM_OK;
}

/**
* \brief Applies a Vertical Transformation to a coordinate set.
* \param[in] pt: A pt4d instance.
* \param[in] w1: input vertical coordinate.
* \param[in] vVerticalTransfo: A vector of Transformation instances.
* \param[out] w2: output vertical coordinate.
* \param[in] code: a processing code.
* \return An OPERATION_MESSAGE which is a code for an error message if it is not OM_OK.
*/
OPERATION_MESSAGE Transformation::ApplyVerticalTransformation2(pt4d *pt, double w1, double *w2,
    std::vector<Transformation>& vVerticalTransfo, int *code, int followUpTransfos)
{
    std::vector<Transformation>::iterator itTsel = SelectTransfo(pt->l, pt->p, vVerticalTransfo); //frame ?
    if (itTsel == vVerticalTransfo.end())
        return OM_VERTICAL_TRANSFO_NOT_FOUND_FOR_THIS_POINT;
    int return_code = 0, drctn = 1;
    double VV[3];
    bool calc_vert_defl = (*code == CALC_VERT_DEFL);

    if (itTsel->getOperationApplication() != OA_DIRECT)
        drctn = -1;

    pt->info.assign(itTsel->getSourceFrameId());
    pt->info2.assign(itTsel->getGridFile());
    if (itTsel->getLayout() == L_CONSTANT)
    {
        if (itTsel->getValuesNumber() == 1)
            *w2 = w1 + drctn*itTsel->getInitValues()[0];
        else if (itTsel->getValuesNumber() == 5)
            // The node frame a and e2 were copied to the source frame by GeodeticSet::CompleteMetadata().
            *w2 = w1 + drctn*ShifTnTilT (itTsel->getInitValues()[0], itTsel->getInitValues()[1], itTsel->getInitValues()[2],
                                         itTsel->getInitValues()[3], itTsel->getInitValues()[4],
                                         itTsel->getAf(), itTsel->getE2f(), pt->l, pt->p);
        *code = itTsel->getPrecisionCodeInit();
        if (followUpTransfos == 1)
            itTsel->FollowUp += asString(drctn*itTsel->getInitValues()[0]) + "\n";
    }
    else
    {
        return_code = itTsel->Interpolate(MU_RADIAN, pt->l, pt->p, VV, code);
        *w2 = w1 + drctn*VV[0];
        if (followUpTransfos == 1)
            itTsel->FollowUp += asString(drctn*VV[0]) + "\n";
        if (calc_vert_defl)
        {
            pt->xi = VV[1];
            pt->eta = VV[2];
        }
    }
    pt->Vprec = *code;
    if (return_code != 0)
        return OM_INTERPOLATION_FAILED;
    return OM_OK;
}


} //namespace circe
