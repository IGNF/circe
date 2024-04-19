#include "circe_ogr.h"
#include "../../circelib/circe.h"
#include "../../circelib/stringtools.h"
#include "../../circelib/metadata.h"
#include <sys/stat.h>
#include <cstdio> //remove

namespace circe
{

void processPoint(CatOperation& CO, OGRPoint* pointIn, OGRPoint* pointOut)
{
    OPERATION_MESSAGE om;
    pt4d p;
    p.CRStypeInit=CO.getCurrentCRStype();
    InitPoint(&p, pointIn->getX(), pointIn->getY(), pointIn->getZ(), 0.);
    if ((om = CO.Operate(&p)) != OM_OK)
        throw (OperationMessageText[om]);
    if (p.CRStypeInit==CT_GEOCENTRIC)
    {
        pointOut->setX(p.x);
        pointOut->setY(p.y);
        pointOut->setZ(p.z);
    }
    else if (p.CRStypeInit==CT_GEOGRAPHIC)
    {
        pointOut->setX(p.l);
        pointOut->setY(p.p);
        pointOut->setZ(p.h);
    }
    else if (p.CRStypeInit==CT_PROJECTED)
    {
        pointOut->setX(p.e);
        pointOut->setY(p.n);
        pointOut->setZ(p.h);
    }
}

void processLineString(CatOperation& CO, OGRLineString* lineStringIn, OGRLineString* lineStringOut)
{
    OGRPoint ptIn, ptOut;
    lineStringOut->empty();
    for (int iPt=0; iPt<lineStringIn->getNumPoints(); iPt++)
    {
        lineStringIn->getPoint(iPt, &ptIn);
        processPoint(CO, &ptIn, &ptOut);
        lineStringOut->addPoint(&ptOut);
    }
}

void processPolygon(CatOperation& CO, OGRPolygon* polygonIn, OGRPolygon* polygonOut)
{
    polygonOut->empty();
    OGRLinearRing* poLinearRing = polygonIn->getExteriorRing();
    OGRLinearRing lr;
    processLineString(CO, poLinearRing, &lr);
    polygonOut->addRing(&lr);
    for (int iPg=0; iPg<((OGRPolygon*)polygonIn)->getNumInteriorRings(); iPg++)
    {
        poLinearRing = polygonIn->getInteriorRing(iPg);
        processLineString(CO, poLinearRing, &lr);
        polygonOut->addRing(&lr);
    }
}

void processMultiPolygon(CatOperation& CO, OGRMultiPolygon* polygonMultiIn, OGRMultiPolygon* polygonMultiOut)
{
    OGRPolygon* poPolygon, pg;
    polygonMultiOut->empty();
    for (int iG=0; iG<polygonMultiIn->getNumGeometries(); iG++)
    {
        poPolygon = (OGRPolygon*)polygonMultiIn->getGeometryRef(iG);
        processPolygon(CO, poPolygon, &pg);
        polygonMultiOut->addGeometry(&pg);
    }
}
/*
--sourceCRS=RGF93CC45. --sourceFormat=.METERS. --targetCRS=RGF93G. --targetFormat=.METERS.DEGREES --metadataFile="data/DataFRnew.xml" --logPathname="/home/sbranchu/CPP/projets/circe_coord_tests/circe.log" --sourcePathname="/home/sbranchu/fichiers_test/shp/IN_ILOT_S_CC45/IN_ILOT_S.shp" --targetPathname="/home/sbranchu/fichiers_test/shp/IN_ILOT_S_CC45/test_IN_ILOT_S_out.shp"
--sourceCRS=RGF93CC45. --sourceFormat=.METERS. --targetCRS=RGF93G. --targetFormat=.METERS.DEGREES --metadataFile="D:\Circev5\4.dev\circe_test/DataFRnew.xml" --metadataFile="D:\Circev5\4.dev\circe_test/DataFRnew.xml" --sourcePathname="D:\CPPQT\tests\ogr_test\shp\bordeaux/IN_ILOT_S_CC45/IN_ILOT_S.shp" --targetPathname="D:\CPPQT\tests\ogr_test\shp\bordeaux/IN_ILOT_S_CC45/test_IN_ILOT_S_out.shp"
--sourceCRS=RGF93G. --sourceFormat=.METERS.DEGREES --targetCRS=RGF93G. --targetFormat=.METERS.DEGREES --metadataFile="data/DataFRnew.xml" --logPathname="/home/sbranchu/CPP/projets/circe_coord_tests/circe.log" --sourcePathname="/home/sbranchu/fichiers_test/kml/annuaire-athis-mons.kml" --targetPathname="/home/sbranchu/fichiers_test/kml/test_annuaire-athis-mons_out.kml"
--sourceCRS=RGF93G. --sourceFormat=.METERS.DEGREES --targetCRS=RGF93G. --targetFormat=.METERS.DEGREES --metadataFile="D:\Circev5\4.dev\circe_test/DataFRnew.xml" --metadataFile="D:\Circev5\4.dev\circe_test/DataFRnew.xml" --sourcePathname="D:\CPPQT\tests\ogr_test\kml/annuaire-athis-mons.kml" --targetPathname="D:\CPPQT\tests\ogr_test\kml/test_annuaire-athis-mons_out.kml"
*/
void processOGRdataSource(circeOptions circopt, GeodeticSet* GS)
{
    OGRFeature *poFeatureIn, *poFeatureOut;
    OGRLayer   *poLayerIn, *poLayerOut;

    std::string message;
    std::ofstream fo;
    fo.open(circopt.logPathname.data(),std::ofstream::out);
    try
    {
        if (GS == NULL)
        {
            GS = new GeodeticSet();
            LoadGeodeticSet(circopt.metadataFile, GS, circopt.verbose);
        }
        CatOperation CO(circopt, *GS);

        OGRRegisterAll();

        // input
        std::string dirName = "/home/sbranchu/fichiers_test/shp/";
    //    std::string fileName = dirName+"ne_50m_graticules_all/ne_50m_graticules_1.shp";
    //    std::string fileName = dirName+"ne_10m_admin_0_countries.shp";
    //    std::string fileName = dirName+"10m-geography-regions-elevation-points/10m_geography_regions_elevation_points.shp";
        //    std::string fileName = dirName+"10m-geographic-lines/ne_10m_geographic_lines.shp";
        //    std::string fileName = dirName+"10m-geography-marine-polys/ne_10m_geography_marine_polys.shp";
        std::string fileName = circopt.sourceOptions.pathname;

        OGRDataSource *poDSin = OGRSFDriverRegistrar::Open( fileName.data(), FALSE );
        if( poDSin == NULL )
        {
            message = ErrorMessageText[EM_CANNOT_OPEN]+"\n"+fileName;
            writeLog(&fo, message);
            throw std::string(message);
        }


        //poLayer = poDS->GetLayerByName( "point" );
        poLayerIn = poDSin->GetLayer( 0 );
        if( poLayerIn == NULL )
        {
            message = ErrorMessageText[EM_NO_LAYER_FOUND];
            writeLog(&fo, message);
            throw std::string(message);
        }

        int nl = poDSin->GetLayerCount();
        printf( "nl:%d,\n",nl++ );

        //int nFeature=0;

        // ouput
        std::string fileNameOut = circopt.targetOptions.pathname;
        struct stat buffer;
        if (stat (fileNameOut.c_str(), &buffer) == 0) remove (fileNameOut.c_str());

        std::string targetExt = getExtension(fileNameOut);
        std::string driverName = "";

//        for(int i=0; apszExtensions[i][0] != NULL; i++)
//        {
//            if (strcmp(pszDestExtension, apszExtensions[i][0])==0
//             && strcmp(pszDriverName, apszExtensions[i][1])!=0)
//            {
//                printf(
//                        "Warning: The target file has a '%s' extension, which is normally used by the %s driver,\n"
//                        "but the requested output driver is %s. Is it really what you want ?\n",
//                        pszDestExtension,
//                        apszExtensions[i][1],
//                        pszDriverName);
//                break;
//            }
//        }

        for(int i=0; Extensions[i][0] != ""; i++)
        {
            if (targetExt == Extensions[i][0])
            {
                driverName = Extensions[i][1];
                message = "found: "+ driverName;
                writeLog(&fo, message);
            }
        }

        OGRSFDriver *poDriver;

        for(int i = 0; i < OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount(); i++)
            writeLog(&fo, OGRSFDriverRegistrar::GetRegistrar()->GetDriver(i)->GetName());

        poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName.c_str());
        if (poDriver == NULL)
        {
            message = driverName+std::string(": ")+ErrorMessageText[EM_BAD_TARGET_FILE_EXTENSION];
            writeLog(&fo, message);
            throw std::string(message);
        }

        OGRDataSource *poDSout = poDriver->CreateDataSource( fileNameOut.data(), NULL );
        if( poDSout == NULL )
        {
            message = ErrorMessageText[EM_TARGET_FILE_CREATION_FAILED];
            writeLog(&fo, message);
            throw std::string(message);
        }

        poLayerOut = poDSout->CreateLayer(fileNameOut.data(), poLayerIn->GetSpatialRef(),
                                                              poLayerIn->GetLayerDefn()->GetGeomType());
        //Get the error messages from CreateLayer ?
        //e.g.
        //ERROR 1: Failed to create file .shp file.
        //ERROR 4: Failed to open Shapefile `/home/sbranchu/fichiers_test/shp/IN_ILOT_S_CC

        if( poLayerOut == NULL )
        {
            message = ErrorMessageText[EM_LAYER_CREATION_FAILED];
            writeLog(&fo, message);
            throw std::string(message);
        }

    //    OGRFieldDefn oField( "Name", OFTString );
    //    oField.SetWidth(32);
    //    if( poLayerOut->CreateField( &oField ) != OGRERR_NONE )
    //    {
    //        printf( "Creating Name field failed.\n" );
    //        exit( 1 );
    //    }

        OGRFeatureDefn *poFDefn = poLayerIn->GetLayerDefn();
        for(int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn oField( poLayerIn->GetFeature(0)->GetFieldDefnRef(iField) );
            if( poLayerOut->CreateField( &oField ) != OGRERR_NONE )
            {
                message = ErrorMessageText[EM_NAME_FIELD_CREATION_FAILED];
                writeLog(&fo, message);
                throw std::string(message);
            }
        }

        poLayerIn->ResetReading();
        while( (poFeatureIn = poLayerIn->GetNextFeature()) != NULL )
        {
//    printf( "%d:",nFeature++ );
            OGRFeatureDefn *poFDefn = poLayerIn->GetLayerDefn();
            int iField;

//            for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
//            {
//                OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

//                if( poFieldDefn->GetType() == OFTInteger )
//                    printf( "%d,", poFeatureIn->GetFieldAsInteger( iField ) );
//                else if( poFieldDefn->GetType() == OFTReal )
//                    printf( "%.3f,", poFeatureIn->GetFieldAsDouble(iField) );
//                else if( poFieldDefn->GetType() == OFTString )
//                    printf( "%s,", poFeatureIn->GetFieldAsString(iField) );
//                else
//                    printf( "%s,", poFeatureIn->GetFieldAsString(iField) );
//            }

            OGRGeometry *poGeometry;
            OGRPoint pt;
            OGRLineString ls;
            OGRPolygon pg;
            OGRMultiPolygon mp;

            poGeometry = poFeatureIn->GetGeometryRef();

            char plop[100];
            sprintf (plop,"%s",poGeometry->getGeometryName());

            //int eza = poGeometry->getGeometryType();
            if( poGeometry != NULL )
            {
                poFeatureOut = OGRFeature::CreateFeature( poLayerOut->GetLayerDefn() );

                // Process geometry

                switch (wkbFlatten(poGeometry->getGeometryType()))
                {
                    case wkbUnknown:
//                        printf ("+wkbUnknown+\n");
                        break;
                    case wkbPoint:
//                        printf ("+wkbPoint+\n");
                        processPoint(CO, (OGRPoint*)poGeometry, &pt);
                        poFeatureOut->SetGeometry( &pt );
                        break;
                    case wkbLineString:
//                        printf ("+wkbLineString+\n");
                        processLineString(CO, (OGRLineString*)poGeometry, &ls);
                        poFeatureOut->SetGeometry( &ls );
                        break;
                    case wkbPolygon:
//                        printf ("+wkbPolygon+\n");
                        processPolygon(CO, (OGRPolygon*)poGeometry, &pg);
                        poFeatureOut->SetGeometry(&pg);
                        break;
                    case wkbMultiPoint:
//                        printf ("+wkbMultiPoint+\n");
                        break;
                    case wkbMultiLineString:
//                        printf ("+wkbMultiLineString+\n");
                        break;
                    case wkbMultiPolygon:
//                        printf ("+wkbMultiPolygon+\n");
                        processMultiPolygon(CO, (OGRMultiPolygon*)poGeometry, &mp);
                        poFeatureOut->SetGeometry(&mp);
                        break;
                    case wkbGeometryCollection:
//                        printf ("+wkbGeometryCollection+\n");
                        break;
                    default:
                        printf ("+default+\n");
                        break;
                }

                // Copy attributes

                for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
                {
                    OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

                    if( poFieldDefn->GetType() == OFTInteger )
                        poFeatureOut->SetField(iField, poFeatureIn->GetFieldAsInteger(iField));
                    else if( poFieldDefn->GetType() == OFTReal )
                        poFeatureOut->SetField(iField, poFeatureIn->GetFieldAsDouble(iField));
                    else if( poFieldDefn->GetType() == OFTString )
                        poFeatureOut->SetField(iField, poFeatureIn->GetFieldAsString(iField));
                    else
                        poFeatureOut->SetField(iField, poFeatureIn->GetFieldAsString(iField));
                }
                if( poLayerOut->CreateFeature( poFeatureOut ) != OGRERR_NONE )
                {
                    message = ErrorMessageText[EM_FEATURE_CREATION_FAILED];
                    writeLog(&fo, message);
                    throw std::string(message);
                }
				OGRFeature::DestroyFeature(poFeatureOut);
			}
            OGRFeature::DestroyFeature( poFeatureIn );
        }

        OGRDataSource::DestroyDataSource( poDSin );
        OGRDataSource::DestroyDataSource( poDSout );
    }
    catch (std::string str)
    {
        writeLog(&fo, "processOGRdataSource: Exception.\n");
        throw str;
    }
        fo.close();
}


} //namespace circe
