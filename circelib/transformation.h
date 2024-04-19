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
* \file transformation.h
* \brief Definition of the Transformation class. 
*/
#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <cstdlib> //malloc, atof
#include <iostream>
#include <fstream>
#include <iterator>     // std::back_inserter
#include <vector>       // std::vector
#include <string>
#include "operation.h"
#include "units.h"
#include "stringtools.h"
#include "crs.h"
#include "StringResources.h"
#include "xmlparser.h"

namespace circe
{

const double EpsilonGrid = 1e-10;
const int CALC_VERT_DEFL = 5000;

#define TBCKEYW "GRID"
#define VERSION_BIN 3 //tbc version


/**\brief Codes for grid layout (node order in a grid file).*/
enum TRANSFO_LAYOUT
{
    L_UNDEF,
    L_SW2N2E,
    L_NW2E2S,
    L_NW2S2E,
    L_SW2E2N,
    L_SE2N2W,
    L_NE2W2S,
    L_NE2S2W,
    L_SE2W2N,
    L_CONSTANT,
    L_COUNT
};
static std::string TransfoLayoutText[L_COUNT] =
{
    "UNDEFINED","SW2N2E","NW2E2S","NW2S2E","SW2E2N","SE2N2W","NE2W2S","NE2S2W","SE2W2N","CONSTANT"
};

static std::string ListeExtensionTypeGrille[GF_COUNT] =
{
    "UNDEFINED","tac","tbc","gra","grd","gsb","gsb","gsa","asc","egm","dis","gta","gtx"
};

/**\brief  Codes for value types.*/
enum VALUE_TYPE
{
    V_UNDEF,
    V_INTEGER2,
    V_INTEGER4,
    V_REAL4,
    V_REAL8,
    V_COUNT
};
static std::string ValueTypeText[V_COUNT] =
{
    "UNDEFINED","INTEGER2","INTEGER4","REAL4","REAL8"
};

/**\brief Codes for value processing (the way a value will be extracted or interpolated from a grid).*/
enum VALUE_PROCESSING
{
    P_UNDEF,
    P_NODE_VALUE,
    P_BILINEAR,
    P_SPLINE,
    P_COUNT
};
static std::string ValueProcessingText[P_COUNT] =
{
    "UNDEFINED","NODE_VALUE","BILINEAR","SPLINE"
};

/**\brief  Codes for node frame info. Node frame is the geodetic frame of the transformation definition domain. 
* Despite its name, it applies to all kind of transformations (constant and grid).
- F_SOURCE: If the transformation source frame is the node frame.
- F_TARGET: If the transformation target frame is the node frame.
- F_OTHER: If neither transformation source nor target frame are geodetic (case of a vertical to vertical transformation), another frame is the node frame.
*/
enum NODE_FRAME_INFO
{
    F_UNDEF,
    F_SOURCE,
    F_TARGET,
    F_OTHER,
    F_COUNT
};
static const std::string NodeFrameInfoText[F_COUNT] =
{
    "UNDEFINED","SOURCE","TARGET","OTHER"
};

/**\brief  Codes for value location in a grid cell.*/
enum VALUES_LOCATION
{
    NODE,
    PIXEL_CENTER
};
static const std::string ValuesLocationText[2] =
{
    "NODE","PIXEL_CENTER"
};

/**
* \brief This class is relevant for either constant (generally 1-P, 3-P, 7-P, or 14-P) or grid (generally 1-P, 2-P or 3-P) transformations.
* Most of members refers to a grid transformation.
*/
class Transformation : public Operation
{
    // Constuction / Destruction
public:
    Transformation();
    virtual ~Transformation();
    void reset();
    Transformation(const Transformation &B) ; // copy constructor
    Transformation& operator=(const Transformation &B);

private:
	static void Transform7(OPERATION_APPLICATION oa, const std::vector<double>& tdr, double  x1, double  y1, double  z1, double *x2, double *y2, double *z2);
    static void Transform3(OPERATION_APPLICATION oa, const std::vector<double>& t, const double &rate_factor, double  x1, double  y1, double  z1, double *x2, double *y2, double *z2);
	static void InitSpline1D(const double *Y, const int n, double *R, double *Q);
	static double Spline1D(double X, const double *Y, const int n, double *R);
	static std::vector<Transformation>::iterator SelectTransfo(double l, double p, std::vector<Transformation>& vTransfo);
	static bool checkPrecisionCodeDefinition(int CodePrecision);
	static void setDoubleVector(std::string _value, std::vector<double>* doubleVector, int count);

public:
    static void ConversionCode2Precision(int CodePrecision, double phi, double *pL, double *pP, double a, double e2);
    static double ConversionCode2Precision(int CodePrecision);
    static void ConversionPrecision2Code(int *CodePrecision, double phi, double pL, double pP, double a, double e2);
    static int ConversionPrecision2Code(double p);
    static OPERATION_MESSAGE ApplyGeodeticTransformation(double l1, double p1, double h1, double x1, double y1, double z1, double *l2, double *p2, double *h2, double *x2, double *y2, double *z2, std::vector<Transformation>& vGeodeticTransfo, int *precision_code, int followUpTransfos);
    static OPERATION_MESSAGE ApplyVerticalTransformation(double l, double p, double w1, double *w2, std::vector<Transformation>& vVerticalTransfo, int *code, int followUpTransfos, std::string *pInfo, std::string *pInfo2);
    static OPERATION_MESSAGE ApplyVerticalTransformation2(pt4d *pt, double w1, double *w2, std::vector<Transformation>& vVerticalTransfo, int *code, int followUpTransfos);


// DATA

	// Internal management
private:
	/**\brief  INTERNAL DATA: Input grid file stream.*/
	std::ifstream fin;
	
	/**\brief  INTERNAL DATA: Output grid file stream.*/
	std::ofstream fout;

	/**\brief  INTERNAL DATA: Header size in bytes.*/
	int	Offset; 

	/**\brief  INTERNAL DATA: Container for storing grid values as integers 2 bytes.*/
	std::vector<short>	VecValI2;

	/**\brief  INTERNAL DATA: Container for storing grid values as integers 4 bytes.*/
	std::vector<int> 	VecValI4;

	/**\brief  INTERNAL DATA: Container for storing grid values as floating points 4 bytes.*/
	std::vector<float>	VecValR4;

	/**\brief  INTERNAL DATA: Container for storing grid values as double precision floating points 8 bytes.*/
	std::vector<double>	VecValR8;

	/**\brief  INTERNAL DATA: Container for storing precision codes.*/
	std::vector<int>	VecPrec;

	/**\brief  INTERNAL DATA: Loading type.*/
	LOADING_TYPE LoadingType;

	/**\brief  INTERNAL DATA: Contains the values of the transformations actually used to perform an operation.*/
	std::string FollowUp;

	/**\brief  INTERNAL DATA: Encoding type.*/
	ENCODING_TYPE	Encoding;

	// Internal management getters & setters 
public:
	ENCODING_TYPE getEncoding() const { return Encoding; }
    LOADING_TYPE getLoadingType() const {return LoadingType;}
	std::string	getFollowUp() const { return FollowUp; }

    void setEncoding(std::string _value){if(_value=="")return; Encoding=(ENCODING_TYPE)getIndex(_value,EncodingTypeText,ET_COUNT);}
    void setLoadingType(std::string _value){if(_value=="")return; LoadingType=(LOADING_TYPE)getIndex(_value,LoadingTypeText,LD_COUNT);}
    void setFollowUp(std::string _value){ FollowUp = _value; }

	// General data
private:
	/**\brief  GENERAL DATA: Grid type*/
	GRID_FORMAT_INFO GridType;

	/**\brief  GENERAL DATA: Grid file path*/
	std::string	GridFile;

	/**\brief  GENERAL DATA: Grid metadata file path*/
	std::string	MetadadaFileName;

	/**\brief  GENERAL DATA: Producer*/
	std::string	Origin;

	/**\brief  GENERAL DATA: Date of creation*/
	std::string	Created;

	/**\brief  GENERAL DATA: Version*/
	std::string	Version;

	/**\brief  GENERAL DATA: Keyword*/
	std::string	Keyword;

	/**\brief  GENERAL DATA: Description*/
	std::string	Description;

	// General data getters & setters 
public:
	GRID_FORMAT_INFO getGridType() const { return GridType; }
    std::string	getGridFile() const {return GridFile;}
    std::string	getMetadadaFileName() const {return MetadadaFileName;}
    std::string	getOrigin() const {return Origin;}
    std::string	getCreated() const {return Created;}
    std::string	getVersion() const {return Version;}
    std::string	getKeyword() const {return Keyword;}
	std::string	getDescription() const { return Description; }

    void setGridType(std::string _value){if(_value=="")return; GridType=(GRID_FORMAT_INFO)getIndex(_value,GridTypeText,GF_COUNT);}
    void setGridFile(std::string _value){if(_value=="")return; GridFile=_value;}
    void setMetadadaFileName(std::string _value) {if(_value=="")return; MetadadaFileName=_value;}
    void setOrigin(std::string _value){if(_value=="")return; Origin=_value;}
    void setCreated(std::string _value){if(_value=="")return; Created=_value;}
    void setVersion(std::string _value){if(_value=="")return; Version=_value;}
    void setKeyword(std::string _value){if(_value=="")return; Keyword=_value;}
	void setDescription(std::string _value){ if (_value == "")return; Description = _value; }

	// Node data (except geodetic system, see under)
private:
	/**\brief  GRID NODE DATA (except L_CONSTANT for a constant transformation): 1 ^>   2 >v   3 v>   4 >^   5 ^<   6 <v   7 v<   8 <^*/
	TRANSFO_LAYOUT Layout;

	/**\brief  GRID NODE DATA: Number of nodes*/
	int  N_nodes;

	/**\brief  GRID NODE DATA: Number of columns*/
	int  N_col;

	/**\brief  GRID NODE DATA: Number of lines*/
	int  N_lines;

	/**\brief  GRID NODE DATA: Column increment*/
	double	WE_inc;

	/**\brief  GRID NODE DATA: Line increment*/
	double	SN_inc;

	/**\brief  GRID NODE DATA: Coordinate type of grid nodes*/
	CRS_TYPE NodeCRStype;

	/**\brief  GRID NODE DATA: Node and increment unit*/
	MEASURE_UNIT NodeIncCoordUnit;

	/**\brief  GRID NODE DATA: Increment unit (if different from NodeIncCoordUnit, will be converted into NodeIncCoordUnit at grid loading)*/
	MEASURE_UNIT IncCoordUnit;

	/**\brief  GRID NODE DATA: The grid file contains the node coordinates (0 false, 1 true)*/
	int		bNodeCoordWritten;

	/**\brief  GRID NODE DATA: The grid file contains the precision codes (0 false, 1 true)*/
	int 	bPrecisionCode;

	/**\brief  GRID NODE DATA: In the native format, west abscissa are positive  (0 false, 1 true)*/
	int     bPositiveWest;

	// Node data getters & setters (except geodetic system, see under)
public:
	TRANSFO_LAYOUT getLayout() const { return Layout; }
    int getN_nodes() const {return N_nodes;}
    int getN_col() const {return N_col;}
    int getN_lines() const {return N_lines;}
    double getWE_inc() const {return WE_inc;}
    double getSN_inc() const {return SN_inc;}
    CRS_TYPE getNodeCRStype() const {return NodeCRStype;}
    MEASURE_UNIT getNodeIncCoordUnit() const {return NodeIncCoordUnit;}
    MEASURE_UNIT getIncCoordUnit() const {return IncCoordUnit;}
    int getbNodeCoordWritten() const {return bNodeCoordWritten;}
    int getbPrecisionCode() const {return bPrecisionCode;}
    int getbPositiveWest() const {return bPositiveWest;}

    void setLayout(std::string _value){if(_value=="")return; Layout=(TRANSFO_LAYOUT)getIndex(_value,TransfoLayoutText,L_COUNT);}
    void setN_nodes(std::string _value){if(_value=="")return; N_nodes=atoi(_value.data());}
    void setN_col(std::string _value){if(_value=="")return; N_col=atoi(_value.data());}
    void setN_lines(std::string _value){if(_value=="")return; N_lines=atoi(_value.data());}
    void setWE_inc(std::string _value){if(_value=="")return; WE_inc=atof(_value.data());}
    void setSN_inc(std::string _value){if(_value=="")return; SN_inc=atof(_value.data());}  
    void setNodeCRStype(std::string _value){if(_value=="")return; NodeCRStype=(CRS_TYPE)getIndex(_value,CRStypeAttributeText,CT_COUNT);}
    void setNodeIncCoordUnit(std::string _value){if(_value=="")return; NodeIncCoordUnit=(MEASURE_UNIT)getIndex(_value,MeasureUnitAttributeText,MU_COUNT);}
    void setIncCoordUnit(std::string _value){if(_value=="")return; IncCoordUnit=(MEASURE_UNIT)getIndex(_value,MeasureUnitAttributeText,MU_COUNT);}
    void setbNodeCoordWritten(std::string _value){if(_value=="")return; bNodeCoordWritten=(CIRCE_BOOL)getIndex(_value,BoolText,2);}
    void setbPrecisionCode(std::string _value){if(_value=="")return; bPrecisionCode=(CIRCE_BOOL)getIndex(_value,BoolText,2);}
    void setbPositiveWest(std::string _value){if(_value=="")return; bPositiveWest=(CIRCE_BOOL)getIndex(_value,BoolText,2);}

	// Value data
private:
	/**\brief  VALUE DATA: constant transformation parameters, or approximate (mean) parameters.*/
	std::vector<double> InitValues;

	/**\brief  VALUE DATA: type of coordinates the transformation is applied to.*/
	CRS_TYPE crs_type;

	/**\brief  VALUE DATA: For a 14-parameter transformation, definition epoch of the parameters.*/
	double  Epoch;

	/**\brief  VALUE DATA: Precision code assigned to InitValues.*/
	int     PrecisionCodeInit;

	/**\brief  VALUE DATA: If true, add a half-increment to the grid limits.*/
	int 	bPixelCenter;

	/**\brief  VALUE DATA: True if values are separated by a space in the grid text file.*/
	int		bDelimiter;

	/**\brief  VALUE DATA: True if the line number is written at the end of the line (BDAlti DIS).*/
	int		bFinalLineNumber;

	/**\brief  VALUE DATA: Number of values at each node of the grid (except the precision code) or number of transformation parameters.*/
	int		N_val;

	/**\brief  VALUE DATA: 1:int*2/short ; 2:int*4/int ; 3:real*4/float ; 4:real*8/double.*/
	int 	ValueType;

	/**\brief  VALUE DATA: Value size in bytes (generally inferred from ValueType).*/
	int 	ValueSize; 

	/**\brief  VALUE DATA: Global size of node data in bytes: NodeSize = ValueSize*N_val [+sizeof(int)].*/
	int		NodeSize;  

	/**\brief  VALUE DATA: Value representing an unknown value.*/
	double  UnknownValue;

	/**\brief  VALUE DATA: True to change UnknownValue into zero.*/
    int    bUnknownValue2zero;

	/**\brief  VALUE DATA: N_val values to be added to interpolated values.*/
	std::vector<double> Translation;

    /**\brief  VALUE DATA: Rate factor to multiply values.*/
    double RateFactor;

	/**\brief  VALUE DATA: Minimum value of the grid parameters (may be usefull if N_val == 1)*/
	double	MinValue;

	/**\brief  VALUE DATA: Maximum value of the grid parameters (may be usefull if N_val == 1)*/
	double	MaxValue; 

	/**\brief  VALUE DATA: Mean value of the grid parameters (may be usefull if N_val == 1)*/
	double	MeanValue;

	/**\brief  VALUE DATA: Value unit (rotation unit in case of 7-parameter or 14-parameter transformation).*/
	MEASURE_UNIT ValueUnit;	

	/**\brief  VALUE DATA: Interpolation type.*/
	VALUE_PROCESSING InterpolationType;

	/**\brief  VALUE DATA: Order of spline interpolation.*/
	int     InterpolationOrder;

	// Value data getters & setters 
public:
	CRS_TYPE getCRStype() const { return crs_type; }
    std::vector<double> getInitValues() const{return InitValues;}
    double getEpoch() const{return Epoch;}
    int getPrecisionCodeInit() const{return PrecisionCodeInit;}
    int getbPixelCenter() const{return bPixelCenter;}
    int getbDelimiter() const{return bDelimiter;}
    int getbFinalLineNumber() const{return bFinalLineNumber;}
    int getValuesNumber() const {return N_val;}
    int getValueType() const{return ValueType;}
    int getValueSize() const{return ValueSize;}
    int getNodeSize() const{return NodeSize;}
    double getUnknownValue() const{return UnknownValue;}
    int getbUnknownValue2zero() const{return bUnknownValue2zero;}
    std::vector<double> getTranslation() const{return Translation;}
    double getRateFactor() const{return RateFactor;}
    double getMinValue() const{return MinValue;}
    double getMaxValue() const{return MaxValue;}
    double getMeanValue() const{return MeanValue;}
    MEASURE_UNIT getValueUnit() const{return ValueUnit;}
    VALUE_PROCESSING getInterpolationType() const{return InterpolationType;}
    int getInterpolationOrder() const{return InterpolationOrder;}

    void ConvertParameters();
    void setCrsType(std::string _value){if(_value=="")return; crs_type=(CRS_TYPE)getIndex(_value,CRStypeAttributeText,CT_COUNT);}
    void setEpoch(std::string _value) {if(_value=="")return; Epoch=atof(_value.data());}
    void setEpoch(double _value){if(_value==0)return; Epoch=_value;}
    void setInitValues(std::string _value) {if(_value=="")return; setDoubleVector(_value,&InitValues,N_val); ConvertParameters();}
    void setPrecisionCodeInit(std::string _value){if(_value=="")return; PrecisionCodeInit=atoi(_value.data());}
    void setbPixelCenter(std::string _value){if(_value=="")return; bPixelCenter=(VALUES_LOCATION)getIndex(_value,ValuesLocationText,2);}
    void setbDelimiter(std::string _value){if(_value=="")return; bDelimiter=(CIRCE_BOOL)getIndex(_value,BoolText,2);}
    void setbFinalLineNumber(std::string _value){if(_value=="")return; bFinalLineNumber=(CIRCE_BOOL)getIndex(_value,BoolText,2);}
    void setValuesNumber(std::string _value){if(_value=="")return; N_val=atoi(_value.data());}
    void setValueType(std::string _value){if(_value=="")return; ValueType=(VALUE_TYPE)getIndex(_value,ValueTypeText,V_COUNT);}
    void setValueSize(std::string _value){if(_value=="")return; ValueSize=atoi(_value.data());}
    void setNodeSize(std::string _value){if(_value=="")return; NodeSize=atoi(_value.data());}
    void setUnknownValue(std::string _value){if(_value=="")return; UnknownValue=atof(_value.data());}
    void setbUnknownValue2zero(std::string _value){if(_value=="")return; bUnknownValue2zero=(CIRCE_BOOL)getIndex(_value,BoolText,2);}
    void setTranslation(std::string _value) {if(_value=="")return; setDoubleVector(_value,&Translation,N_val);}
    void setRateFactor(std::string _value){if(_value=="")return; RateFactor=atof(_value.data());}
    void setMinValue(std::string _value){if(_value=="")return; MinValue=atof(_value.data());}
    void setMaxValue(std::string _value){if(_value=="")return; MaxValue=atof(_value.data());}
    void setMeanValue(std::string _value){if(_value=="")return; MeanValue=atof(_value.data());}
    void setValueUnit(std::string _value){if(_value=="")return; ValueUnit=(MEASURE_UNIT)getIndex(_value,MeasureUnitAttributeText,MU_COUNT);}
    void setInterpolationType(std::string _value){if(_value=="")return; InterpolationType=(VALUE_PROCESSING)getIndex(_value,ValueProcessingText,P_COUNT);}
    void setInterpolationOrder(std::string _value){if(_value=="")return; InterpolationOrder=atoi(_value.data());}

	//Geodetic Frame Data ("From" frame "To" frame)
private:
	/**\brief  GEODETIC FRAME DATA: Source Frame id.*/
	std::string SourceFrameId;
	
	/**\brief  GEODETIC FRAME DATA: Target frame id.*/
	std::string TargetFrameId;

	/**\brief  GEODETIC FRAME DATA: Node frame id (may be equal to SourceFrameId or TargetFrameId).*/
	std::string NodeFrameId;

	/**\brief  GEODETIC FRAME DATA: Semi-major axis of the ellipsoid (m) associated with the source frame.*/
	double  Af;

	/**\brief  GEODETIC FRAME DATA: Semi-minor axis of the ellipsoid (m) associated with the source frame.*/
	double  Bf;

	/**\brief  GEODETIC FRAME DATA: Squared eccentricity of the ellipsoid (m) associated with the source frame.*/
	double  E2f;

    /**\brief  GEODETIC FRAME DATA: Source frame epoch.*/
    double  EpochF;

    /**\brief  GEODETIC FRAME DATA: Semi-major axis of the ellipsoid (m) associated with the target frame.*/
	double  At;

	/**\brief  GEODETIC FRAME DATA: Semi-minor axis of the ellipsoid (m) associated with the target frame.*/
	double  Bt;

	/**\brief  GEODETIC FRAME DATA: Squared eccentricity of the ellipsoid (m) associated with the target frame.*/
	double  E2t;

    /**\brief  GEODETIC FRAME DATA: Target frame epoch.*/
    double  EpochT;

	/**\brief  GEODETIC FRAME DATA: Node frame info.*/
	NODE_FRAME_INFO NodeFrameInfo;

	//Geodetic Frame Data getters & setters
public:
	std::string getSourceFrameId() const{ return SourceFrameId; }
    std::string getTargetFrameId() const{return TargetFrameId;}
    std::string getNodeFrameId() const;
    NODE_FRAME_INFO getNodeFrameInfo() const{return NodeFrameInfo;}
    double getAf() const{return Af;}
    double getBf() const{return Bf;}
    double getE2f() const{return E2f;}
    double getEpochF() const{return EpochF;}
    double getAt() const{return At;}
    double getBt() const{return Bt;}
    double getE2t() const{return E2t;}
    double getEpochT() const{return EpochT;}
    void GetApplicationFrameEpochs(double *epF, double *epT);

    void setSourceFrameId(std::string _value) {if(_value=="")return; SourceFrameId=_value;}
    void setTargetFrameId(std::string _value) {if(_value=="")return; TargetFrameId=_value;}
    void setNodeFrameId(std::string _value) {if(_value=="")return; NodeFrameId=_value;}
    void setNodeFrameInfo(std::string _value){if(_value=="")return; NodeFrameInfo=(NODE_FRAME_INFO)getIndex(_value,NodeFrameInfoText,F_COUNT);}
    void setAf(std::string _value){if(_value=="")return; Af=atof(_value.data());}
    void setBf(std::string _value){if(_value=="")return; Bf=atof(_value.data());}
    void setE2f(std::string _value){if(_value=="")return; E2f=atof(_value.data());}
    void setEpochF(std::string _value){if(_value=="")return; EpochF=atof(_value.data());}
    void setAt(std::string _value){if(_value=="")return; At=atof(_value.data());}
    void setBt(std::string _value){if(_value=="")return; Bt=atof(_value.data());}
    void setE2t(std::string _value){if(_value=="")return; E2t=atof(_value.data());}
    void setEpochT(std::string _value){if(_value=="")return; EpochT=atof(_value.data());}
    void setAf(double _value){if(_value==0)return; Af=_value;}
    void setBf(double _value){if(_value==0)return; Bf=_value;}
    void setE2f(double _value){if(_value==0)return; E2f=_value;}
    void setEpochF(double _value){if(_value==0)return; EpochF=_value;}
    void setAt(double _value){if(_value==0)return; At=_value;}
    void setBt(double _value){if(_value==0)return; Bt=_value;}
    void setE2t(double _value){if(_value==0)return; E2t=_value;}
    void setEpochT(double _value){if(_value==0)return; EpochT=_value;}



// METHODS

// Elementary reading/writing
private:
    int    (Transformation::*ReadElementI4)();
    double (Transformation::*ReadElementR8)();
    double (Transformation::*ReadElementBin)();
    double (Transformation::*ReadElementTab)(int Position);
    void   (Transformation::*WriteElementI4)(const int iV);
    void   (Transformation::*WriteElementR8)(const double dV);
    void   (Transformation::*WriteElementBin)(const double dV);
    void   (Transformation::*WriteElementTab)(const double dV, int Position);
    std::string ReadStringBin (int nc) ;
    short  ReadI2bin();
    double ReadI2bin_d();
    short  ReadI2bin_Swap();
    double ReadI2bin_d_Swap();
    int    ReadI4bin();
    double ReadI4bin_d();
    int    ReadI4bin_Swap();
    double ReadI4bin_d_Swap();
    float  ReadR4bin();
    double ReadR4bin_d();
    float  ReadR4bin_Swap();
    double ReadR4bin_d_Swap();
    double ReadR8bin();
    double ReadR8bin_Swap();
    double ReadI2tab(int Position);
    double ReadI4tab(int Position);
    double ReadR4tab(int Position);
    double ReadR8tab(int Position);
    void   WriteStringBin (const std::string str, int nc);
    void   WriteStringBin (const char cstr[_MAX_PATH_], int nc);
    void   WriteI2bin (const short sV);
    void   WriteI2bin_d (const double sV);
    void   WriteI2bin_Swap (const short sV);
    void   WriteI2bin_d_Swap (const double sV);
    void   WriteI4bin (const int iV);
    void   WriteI4bin_d (const double iV);
    void   WriteI4bin_Swap (const int iV);
    void   WriteI4bin_d_Swap (const double iV);
    void   WriteR4bin (const float fV);
    void   WriteR4bin_Swap (const float fV);
    void   WriteR4bin_d (const double fV);
    void   WriteR4bin_d_Swap (const double fV);
    void   WriteR8bin (const double dV);
    void   WriteR8bin_Swap (const double dV);
    void   WriteI2tab (const double dV, int Position);
    void   WriteI4tab (const double dV, int Position);
    void   WriteR4tab (const double dV, int Position);
    void   WriteR8tab (const double dV, int Position);

private:
    int LoadBinaryFile();
    int LoadTextFile();
    void NodeCoord(int node, double XY[3], int *C, int *L);
    void Element(int i, int j, double *VV, int *precision_code);

    int WriteHeaderTBC();
    int BilinearInterpolation (double x, double y, double *VV, int *code);
    int SplineInterpolation(int window_size, double x, double y, double *VV, int *code);
    void ReadHeaderESRI();
    void ReadHeaderTBC();
    void ReadHeaderDIS();
    void ReadHeaderEGM();
    void ReadHeaderGravsoft();
    void ReadHeaderGTX();
    void ReadHeaderGTXbin();
    void ReadHeaderNtv2Asc();
    void ReadHeaderNtv2Bin();
    void ReadHeaderTAC();
    void ReadHeaderSurfer();
    int NodePosition(int i, int j) const;

public:
    std::string DisplayMetaData();
    int LoadGrid(LOADING_TYPE lt);
    int WriteElement(double dV, int Position);
    //int WriteGravsoft();
    int WriteXML(std::string *XMLfilePath);
    int ReadXML(std::string XMLfilePath);
    int InitGridType(int grid_index);
    int InitAccesElement(bool bSystemBinary);
    int InitVector(int taille);
    std::string ModifyAndValidateMetaData(bool bModify);
    int Interpolate (MEASURE_UNIT unite_ini, double l, double p, double *VV, int *code);
    void toEpoch(double ep);
	void toEpoch(double ep, const double values[7]);

};

} //namespace circe

#endif // TRANSFORMATION_H
