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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QMargins>
#include <QMessageBox>
#include <QStyleFactory>
#include <QToolButton>
#include <QProcess>
#include <QDesktopServices>
#include <QScrollArea>
#include <QDesktopWidget>
#include <QClipboard>
#include <QThread>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

//#include <direct.h> //chdir
#include <iomanip> //setprecision
#include <sys/stat.h>
#include "../../circelib/circe.h"
#include "../../circelib/files.h"
#include "../../circelib/geodeticset.h"
#include "../../circelib/metadata.h"
#include "../../circelib/stringtools.h"

void MainWindow::displayMessageFromCircelib(const QString &str)
{
    //Retrieve the last number in the string (if any).
    int num = 100;
    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatchIterator matches = rx.globalMatch(str);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        num = match.captured(0).toInt();
    }
    if (num>1000)
    {
        num = num/1000;
        if (num%100 == 0)
            dspSwap = !dspSwap;
    }
    std::string colele, colstr;
    if (dspSwap)
        colele = circe::asString(int( ( (1.-(num%100)/100.)*0.5+0.5 ) *255));
    else
        colele = circe::asString(int( ( (num%100)/100.*0.5+0.5 ) *255));
    //cyan = #00FFFF = rgb(0,255,255)
    //*0.5+0.5 to mitigate the darkest color
    colstr = "rgb(0,"+colele+","+colele+")";

    labelMessage.setText(str);
    labelMessage.setStyleSheet("QLabel { background-color : "+QString(colstr.data())+"; color : black; font: bold 12px;}");
    labelMessage.repaint();
}

void MainWindow::displayCommandLineWhileEditing()
{
    labelMessage.setText(circe::writeCommandLine(CO).data());
    labelMessage.setWordWrap(true);
    labelMessage.setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelMessage.setStyleSheet("QLabel { background-color : grey; color : black; font: bold 12px;}");
}

void displayMessage(std::string Message, std::string Caption, std::string imageFilename)
{
    QMessageBox msgBox;
    msgBox.setText(Message.data());
    msgBox.setIconPixmap(QPixmap(imageFilename.data())); //workspace image
    //msgBox.setIconPixmap(QPixmap(":/images/circe12cinq.png")); //resource image
    msgBox.setWindowTitle(Caption.data());
    msgBox.exec();
}

void displayMessage(const char* Message)
{
    QMessageBox msgBox;
    msgBox.setText(Message);
    msgBox.exec();
}

void MainWindow::buttonProcess_clicked()
{
    circe::pt4d p;
    std::ostringstream ss;
    std::string ret;
    struct stat buffer;
    circe::swain s;
    s.fi = NULL;
    try
    {
        GS.mb->setCurrentMessage("...");
        labelMessage.setText(QString("..."));
        labelMessage.setStyleSheet("QLabel { background-color : grey; color : black; font: bold 12px;}");
        labelMessage.repaint();
        dspSwap = false;
        switch(CO.dataMode)
        {
            case circe::DM_POINT:
                // DMS value validations
                if ((circe::MEASURE_UNIT)getIndex(CO.sourceOptions.angularUnit,circe::MeasureUnitAttributeText,circe::MU_COUNT)
                        == circe::MU_DMS)
                {
                    CO.Pt4d.l = circe::readDMS(lineditSourceL.text().toStdString(),"longitude DMS",true);
                    CO.Pt4d.p = circe::readDMS(lineditSourceP.text().toStdString(),"latitude DMS",true);
                }
                else if ((circe::MEASURE_UNIT)getIndex(CO.sourceOptions.angularUnit,circe::MeasureUnitAttributeText,circe::MU_COUNT)
                        == circe::MU_DM)
                {
                    CO.Pt4d.l = circe::readDMS(lineditSourceL.text().toStdString(),"longitude DM",true);
                    CO.Pt4d.p = circe::readDMS(lineditSourceP.text().toStdString(),"latitude DM",true);
                }
                p = circe::process_pt4d(circe::writeCommandLine(CO), &GS);
                p.frmt.angularUnit = CO.targetOptions.angularUnit;
                p.frmt.plainDMS = CO.plainDMS;
                lineditTargetI.setText(p.id.data());
                lineditTargetX.setText(circe::displayElement(p,'X').data());
                lineditTargetY.setText(circe::displayElement(p,'Y').data());
                lineditTargetZ.setText(circe::displayElement(p,'Z').data());
                lineditTargetL.setText(circe::displayElement(p,'L').data());
                lineditTargetP.setText(circe::displayElement(p,'P').data());
                lineditTargetH.setText(circe::displayElement(p,'H').data());
                lineditTargetE.setText(circe::displayElement(p,'E').data());
                lineditTargetN.setText(circe::displayElement(p,'N').data());
                lineditTargetV.setText(circe::displayElement(p,'V').data());
                lineditTargetC.setText(circe::displayElement(p,'C').data());
                lineditTargetD.setText(circe::displayElement(p,'D').data());
                if (CO.noPrintSppm)
                {
                    ss.imbue( std::locale::classic() );
                    ss << std::fixed << std::setfill(' ') << std::setprecision(7) << 1+1e-6*p.scfact;
                    lineditTargetS.setText(ss.str().data());
                }
                else
                    lineditTargetS.setText(circe::displayElement(p,'S').data());
                if (p.info != "")
                    p.info = " [ "+p.info+" ] ";
                labelMessage.setText((std::string("geod.prec. ")+circe::precisionCode2Text(p.Gprec)+"   "
                                    +std::string("vert.prec. ")+circe::precisionCode2Text(p.Vprec)
                                     + std::string(" ")+p.info).data());
                                     //+"\n\n"+p.info2).data());
                labelMessage.setStyleSheet("QLabel { background-color : green; color : black; font: bold 12px;}");
            break;
            case circe::DM_FILE:
                s.fi = new std::ifstream();
                s.fi->open(QString(CO.sourceOptions.pathname.data()).toLocal8Bit());
                s.fo = new std::ofstream();
                s.fo->open(QString(CO.targetOptions.pathname.data()).toLocal8Bit());
//                if (stat((CO.targetOptions.pathname).c_str(), &buffer) == 0)
                ret = circe::process(circe::writeCommandLine(CO), &GS, &s);
                labelMessage.setText(ret.data());
                labelMessage.setStyleSheet("QLabel { background-color : green; color : black; font: bold 12px;}");
            break;
            case circe::DM_LIST:
                ;
            break;
            default:
            break;
        }
    }
    catch (std::string str)
    {
        labelMessage.setText(str.data());
        labelMessage.setStyleSheet("QLabel { background-color : red; color : black; font: bold 12px;}");
        lineditTargetY.setText("0");
        lineditTargetZ.setText("0");
        lineditTargetX.setText("0");
        lineditTargetL.setText("0");
        lineditTargetP.setText("0");
        lineditTargetH.setText("0");
        lineditTargetE.setText("0");
        lineditTargetN.setText("0");
        lineditTargetV.setText("0");
        lineditTargetC.setText("0");
        lineditTargetS.setText("0");
        lineditTargetD.setText("0");
    }
}

std::string MainWindow::getValidityAreaCommandLine(std::string angularUnit, std::string CRSid)
{
    std::string s = "--getValidityArea --metadataFile=" + CO.metadataFile + " ";
    s += "--sourceFormat=.."+angularUnit+" ";
    s += "--sourceCRS="+CRSid;
//    return "WSEN"+circe::process(s, &GS);
    return circe::process(s, &GS);
}
void MainWindow::displaySourceValidityArea()
{
    if (!CO.displayBoundingBox)
    {
        labelSourceGeodeticCRSinfo.setVisible(false);
        labelSourceVerticalCRSinfo.setVisible(false);
        return;
    }
    if (CO.sourceOptions.useGeodeticCRS)
    {
        labelSourceGeodeticCRSinfo.setText(getValidityAreaCommandLine(CO.sourceOptions.angularUnit, CO.sourceOptions.geodeticCRSid).data());
        labelSourceGeodeticCRSinfo.setVisible(true);
    }
    if (CO.sourceOptions.useVerticalCRS && !CO.sourceOptions.verticalFrameAuthoritative
            && (CO.sourceOptions.verticalFrameId != ""))
    {
        labelSourceVerticalCRSinfo.setText(getValidityAreaCommandLine(CO.sourceOptions.angularUnit, CO.sourceOptions.verticalCRSid).data());
        labelSourceVerticalCRSinfo.setVisible(true);
    }
    else
    {
        labelSourceVerticalCRSinfo.setText("");
        labelSourceVerticalCRSinfo.setVisible(false);
    }
}
void MainWindow::displayTargetValidityArea()
{
    if (!CO.displayBoundingBox)
    {
        labelTargetGeodeticCRSinfo.setVisible(false);
        labelTargetVerticalCRSinfo.setVisible(false);
        return;
    }
    if (CO.targetOptions.useGeodeticCRS)
    {
        labelTargetGeodeticCRSinfo.setText(getValidityAreaCommandLine(CO.targetOptions.angularUnit, CO.targetOptions.geodeticCRSid).data());
        labelTargetGeodeticCRSinfo.setVisible(true);
    }
    if (CO.targetOptions.useVerticalCRS && !CO.targetOptions.verticalFrameAuthoritative
            && (CO.targetOptions.verticalFrameId != ""))
    {
        labelTargetVerticalCRSinfo.setText(getValidityAreaCommandLine(CO.targetOptions.angularUnit, CO.targetOptions.verticalCRSid).data());
        labelTargetVerticalCRSinfo.setVisible(true);
    }
    else
    {
        labelTargetVerticalCRSinfo.setText("");
        labelTargetVerticalCRSinfo.setVisible(false);
    }
}

MainWindow::~MainWindow()
{
    circe::writeXMLOptionFile(CO);
    delete ui;
}

void MainWindow::InitData(std::string circeOptionFile)
{
    try
    {
//        QString plop = QDir::currentPath();
        std::locale::global( std::locale::classic() );
        //circe::test_decimal_separator();
        circe::readXMLOptionFile(&CO, circeOptionFile);
        bUpdateCRSid = false; //keep the value read in circeOptionFile
        CO.getAllGeodeticSubsetLists = 1;
        if (CO.dataMode != circe::DM_FILE && CO.dataMode != circe::DM_POINT)
            CO.dataMode = circe::DM_POINT;
        //CO.logPathname = "circe.log";
//        struct stat buffer;
//        if (stat (CO.logoPathname.c_str(), &buffer) != 0)
//            throw std::string(CO.logoPathname+circe::ErrorMessageText[circe::EM_NOT_FOUND]+"\n");

        circe::LoadGeodeticSet(CO.metadataFile, &GS, CO.verbose);
        circe::getGeodeticSubset(&OS, &CO, &GS);
        GS.mb = &mh;
    }
    catch (std::string str)
    {
        displayMessage(str,CO.appTitle.data(),CO.imageFilename);
        exit(1);
    }

    setWindowTitle(CO.appTitle.data());
}

QString checkOptionFile(QString dir)
{
    QDir didi;
    didi.setCurrent(dir);
    //QStringList nameFilters(QString::fromWCharArray(OPTION_FILE_PREFIX)+"*");
    QStringList nameFilters(QString(OPTION_FILE_PREFIX)+"*");
    QFileInfoList list = didi.entryInfoList(nameFilters);
    if (list.size()>0)
    {
        QFileInfo fileInfo = list.at(0);
        return fileInfo.fileName();
    }
    else
        return "";
}

QDateTime getOptionFileDate(QString dir)
{
    QDir didi;
    didi.setCurrent(dir);
    //QStringList nameFilters(QString::fromWCharArray(OPTION_FILE_PREFIX)+"*");
    QStringList nameFilters(QString(OPTION_FILE_PREFIX)+"*");
    QFileInfoList list = didi.entryInfoList(nameFilters);
    if (list.size()>0)
    {
        QFileInfo fileInfo = list.at(0);
        return fileInfo.lastModified();
    }
    else
        return qt0;
}

std::string MainWindow::InitZones()
{
    std::string cofile="";
    int icof;
    QDir didi;
    didi.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    QStringList nameFilters("*");
    QFileInfoList list = didi.entryInfoList(nameFilters);
    menuZone = menuBar()->addMenu(tr(circe::InterfaceElementText[circe::IE_ZONE].data()));
    QDateTime lastModTime = qt0;
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo dirInfo = list.at(i);
        QString fifi = dirInfo.fileName();
        if (checkOptionFile(circeDataDir+fifi) != "")
        {
            //fifi = circe::replaceChar(dirInfo.fileName().toStdString(),'_',' ').data();
            QAction *menuAction = menuZone->addAction(fifi,this,SLOT(menuZone_triggered()));
            menuAction->setObjectName(fifi);
            QDateTime dt = getOptionFileDate(circeDataDir+fifi);
            if (cofile == "" || dt > lastModTime)
            {
                lastModTime = dt;
                cofile = checkOptionFile(circeDataDir+fifi).toStdString();
                icof = i;
            }
        }
    }
    if (cofile!="")
        didi.setCurrent(circeDataDir+list.at(icof).fileName());
    return cofile;
}
void MainWindow::menuZone_triggered()
{
    circe::writeXMLOptionFile(CO);
    QObject *senderObj = sender();
    QString tag = senderObj->objectName();
    QDir didi;
    didi.setCurrent(QString(circeDataDir+tag));
    InitWidgetValues(checkOptionFile(tag).toStdString());
    bUpdateCRSid = true;
}

void MainWindow::UpdateSourcePointDisplay(circe::pt4d* point4d)
{
    point4d->frmt.angularUnit = CO.targetOptions.angularUnit;
    point4d->frmt.plainDMS = CO.plainDMS;
    lineditSourceI.setText(point4d->id.data());
    lineditSourceX.setText(circe::displayElement(*point4d,'X').data());
    lineditSourceY.setText(circe::displayElement(*point4d,'Y').data());
    lineditSourceZ.setText(circe::displayElement(*point4d,'Z').data());
    lineditSourceL.setText(circe::displayElement(*point4d,'L').data());
    lineditSourceP.setText(circe::displayElement(*point4d,'P').data());
    lineditSourceH.setText(circe::displayElement(*point4d,'H').data());
    lineditSourceE.setText(circe::displayElement(*point4d,'E').data());
    lineditSourceN.setText(circe::displayElement(*point4d,'N').data());
    lineditSourceV.setText(circe::displayElement(*point4d,'V').data());
}

void MainWindow::InitWidgetValues(std::string circeOptionFile)
{
    InitData(circeOptionFile);

    setWindowIcon(QIcon(CO.iconFilename.data())); //workspace image
    //setWindowIcon(QIcon(":/images/circeV5.png"));//resource image

    groupboxSourceGeodeticCRS->setChecked(CO.sourceOptions.useGeodeticCRS);
    groupboxSourceVerticalCRS->setChecked(CO.targetOptions.useVerticalCRS);
    checkboxSourceVerticalAuthoritative->setChecked(CO.sourceOptions.verticalFrameAuthoritative);
    checkboxSourceVerticalAuthoritative_stateChanged(checkboxSourceVerticalAuthoritative->checkState());
    checkboxSourceVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));

    lineditSourceDataFile.setText(CO.sourceOptions.pathname.data());
    UpdateSourcePointDisplay(&CO.Pt4d);
    lineditSourceEpoch.setText(circe::displayEpoch(CO.sourceOptions.epoch).data());

    groupboxTargetGeodeticCRS->setChecked(CO.targetOptions.useGeodeticCRS);
    groupboxTargetVerticalCRS->setChecked(CO.targetOptions.useVerticalCRS);
    checkboxTargetVerticalAuthoritative->setChecked(CO.targetOptions.verticalFrameAuthoritative);
    checkboxTargetVerticalAuthoritative_stateChanged(checkboxTargetVerticalAuthoritative->checkState());
    checkboxTargetVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));

    lineditTargetDataFile.setText(CO.targetOptions.pathname.data());
    lineditTargetEpoch.setText(circe::displayEpoch(CO.targetOptions.epoch).data());
    checkboxPrintCS_stateChanged(CO.printCS);


    populateComboBox(comboboxSourceGeodeticFrame, OS.sourceSubset.geodeticFrameDisplayList, OS.sourceSubset.iGeodeticFrame,"InitWidgetValues");
/*
    populateComboBox(comboboxSourceGeodeticCRS, OS.sourceSubset.geodeticCRSdisplayList, OS.sourceSubset.iGeodeticCRS);
    populateComboBox(comboboxSourceVerticalFrame, OS.sourceSubset.verticalFrameDisplayList, OS.sourceSubset.iVerticalFrame);
    populateSourceFormat();
    populateComboBox(comboboxSourceAngularUnit, OS.sourceSubset.dataUnitDisplayList, OS.sourceSubset.iDataUnit);
    lineditSourceEpoch.setVisible(OS.sourceSubset.geodeticDimension == circe::SD_4D);

    populateComboBox(comboboxTargetGeodeticFrame, OS.targetSubset.geodeticFrameDisplayList, OS.targetSubset.iGeodeticFrame);
    populateComboBox(comboboxTargetGeodeticCRS, OS.targetSubset.geodeticCRSdisplayList, OS.targetSubset.iGeodeticCRS);
    populateComboBox(comboboxTargetVerticalFrame, OS.targetSubset.verticalFrameDisplayList, OS.targetSubset.iVerticalFrame);
    populateTargetFormat();
    populateComboBox(comboboxTargetAngularUnit, OS.targetSubset.dataUnitDisplayList, OS.targetSubset.iDataUnit);
    lineditTargetEpoch.setVisible(OS.targetSubset.geodeticDimension == circe::SD_4D);

    populateComboBox(comboboxGeodeticTransfo, OS.geodeticTransfoDisplayList, OS.iGeodeticTransfo);
    populateComboBox(comboboxVerticalTransfo, OS.verticalTransfoDisplayList, OS.iVerticalTransfo);
    comboboxGeodeticTransfo.setEnabled(OS.geodeticTransfoDisplayList.size()>1);
    comboboxVerticalTransfo.setEnabled(OS.verticalTransfoDisplayList.size()>1);
*/
    if (CO.dataMode == circe::DM_POINT)
        comboboxSourceFormat_currentIndexChanged(OS.sourceSubset.iDataPointFormat);
    else
        comboboxSourceFormat_currentIndexChanged(OS.sourceSubset.iDataFileFormat);
    comboboxSourceAngularUnit_currentIndexChanged(comboboxSourceAngularUnit->currentIndex());
    comboboxTargetFormat_currentIndexChanged(comboboxTargetFormat->currentIndex());
    comboboxTargetAngularUnit_currentIndexChanged(comboboxTargetAngularUnit->currentIndex());

    tabwidgetSourceDataFile->setCurrentIndex(CO.dataMode-1);
    tabwidgetTargetDataFile->setCurrentIndex(CO.dataMode-1);

    displaySourceValidityArea();
    displayTargetValidityArea();

    menuDisplay_Bounding->setChecked(CO.displayBoundingBox);
    menuDisplay_CS->setChecked(CO.printCS);
    menuDisplay_DMS->setChecked(CO.plainDMS);
    menuDisplay_scrollArea->setChecked(CO.scrollArea);
    menuTool_Authoritative->setChecked(!CO.noUseVerticalFrameAuthoritative);
    labelLogo->setStyleSheet(("background-image:url("+CO.logoFilename+"); background-position: center; background-repeat: no;").data());
    //labelLogo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    if (GS.vVerticalFrame.size() <= 1)
    {
        CO.noUseVerticalFrameAuthoritative = 1;
        menuTool_Authoritative->setVisible(false);
    }
    else
        menuTool_Authoritative->setVisible(true);
    checkboxSourceVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));
    checkboxTargetVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));

    groupboxSourceVerticalCRS->setVisible(GS.vVerticalFrame.size()>0);
    groupboxTargetVerticalCRS->setVisible(GS.vVerticalFrame.size()>0);
    groupboxVerticalTransfo->setVisible(GS.vVerticalFrame.size()>0);

    lineditTargetEpoch.setEnabled(GS.getKeyGeodeticFrame()!="");
}

MainWindow::MainWindow(std::string circeOptionFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    this->setMaximumHeight(200);
//    setFont(QFont ("Arial", 10, QFont::Normal));

    connect(&mh, SIGNAL(messageSignal(const QString &)), this,
            SLOT(displayMessageFromCircelib(const QString &)), Qt::DirectConnection);

    // WIDGET LAYOUT

    redrawPt4dLayout = true;

    // Menu

    circeDataDir = QDir::currentPath()+"/";
    if (circe::getExtension(circeOptionFile) != "xml")
    {
        circeOptionFile = InitZones();
        InitData(circeOptionFile);
        if (CO.menuTitle_zone != "")
            menuZone->setTitle(CO.menuTitle_zone.data());
    }
    else
        InitData(circeOptionFile);

    menuDisplay = menuBar()->addMenu(tr(circe::InterfaceElementText[circe::IE_DISPLAY].data()));
    if (CO.menuTitle_display != "") menuDisplay->setTitle(CO.menuTitle_display.data());
    menuDisplay_Layout = menuDisplay->addAction(circe::InterfaceElementText[circe::IE_LAYOUT].data(),this,SLOT(menuDisplay_Layout_triggered()));
    menuDisplay_Bounding = menuDisplay->addAction(circe::InterfaceElementText[circe::IE_BOUNDINGBOX].data(),this,SLOT(menuDisplay_Bounding_triggered()));
    menuDisplay_CS = menuDisplay->addAction(circe::InterfaceElementText[circe::IE_PRINT_CS].data(),this,SLOT(menuDisplay_CS_triggered()));
    menuDisplay_DMS = menuDisplay->addAction(circe::InterfaceElementText[circe::IE_PLAIN_DMS].data(),this,SLOT(menuDisplay_DMS_triggered()));
    menuDisplay_scrollArea = menuDisplay->addAction(circe::InterfaceElementText[circe::IE_SCROLLAREA].data(),this,SLOT(menuDisplay_scrollArea_triggered()));
    menuDisplay_Bounding->setCheckable(true);
    menuDisplay_CS->setCheckable(true);
    menuDisplay_DMS->setCheckable(true);
    menuDisplay_scrollArea->setCheckable(true);
    menuTool = menuBar()->addMenu(tr(circe::InterfaceElementText[circe::IE_TOOL].data()));
    if (CO.menuTitle_tool != "") menuTool->setTitle(CO.menuTitle_tool.data());
    menuTool_CopyCL = menuTool->addAction(circe::InterfaceElementText[circe::IE_COPYCL].data(),this,SLOT(menuTool_CopyCL_triggered()),QKeySequence(tr("Ctrl+c")));
    menuTool_Clear = menuTool->addAction(circe::InterfaceElementText[circe::IE_CLEAR].data(),this,SLOT(menuTool_Clear_triggered()),QKeySequence(tr("Ctrl+e")));
    menuTool_Swap = menuTool->addAction(circe::InterfaceElementText[circe::IE_SWAP].data(),this,SLOT(menuTool_Swap_triggered()),QKeySequence(tr("Ctrl+i")));
    menuTool_CopyRes = menuTool->addAction(circe::InterfaceElementText[circe::IE_COPYRES].data(),this,SLOT(menuTool_CopyTargetPoint_triggered()),QKeySequence(tr("Ctrl+t")));
    menuTool_Authoritative = menuTool->addAction(circe::InterfaceElementText[circe::IE_AUTHORITATIVE].data(),this,SLOT(menuTool_Authoritative_triggered()),QKeySequence(tr("Ctrl+v")));
    menuTool_Authoritative->setCheckable(true);

    menuSeparator = menuTool->addMenu(circe::InterfaceElementText[circe::IE_SEPARATOR].data());
    QStringList strlistSeparators; //= (QStringList() << "_" << "," << ";" << "\t");
    strlistSeparators << QString(CO.separator.data());
    for (int ifs=0;ifs<circe::FS_END;ifs++)
    {
        if (circe::FieldSeparatorText[ifs] != CO.separator)
            strlistSeparators << QString(circe::FieldSeparatorText[ifs].data());
    }
    menuTool_Separator = new QWidgetAction(this);
    comboboxSeparator = new QComboBox();
    comboboxSeparator->setEditable(true);
    comboboxSeparator->addItems(strlistSeparators);
    menuTool_Separator->setDefaultWidget(comboboxSeparator);
    menuSeparator->addAction(menuTool_Separator);
    connect(comboboxSeparator, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxSeparator_currentTextChanged(QString)));

    menuHelp = menuBar()->addMenu(tr(circe::InterfaceElementText[circe::IE_HELP].data()));
    if (CO.menuTitle_help != "") menuHelp->setTitle(CO.menuTitle_help.data());
    menuHelp_Doc = menuHelp->addAction(circe::InterfaceElementText[circe::IE_DOCUMENTATION].data(),this,SLOT(menuHelp_Doc_triggered()),QKeySequence(tr("Ctrl+d")));
    menuHelp_User = menuHelp->addAction(circe::InterfaceElementText[circe::IE_USERMANUAL].data(),this,SLOT(menuHelp_User_triggered()),QKeySequence(tr("Ctrl+u")));
    menuHelp_About = menuHelp->addAction(circe::InterfaceElementText[circe::IE_ABOUT].data(),this,SLOT(menuHelp_About_triggered()),QKeySequence(tr("Ctrl+a")));

    // Logo

    QToolButton* buttonProcess = new QToolButton(this);
//    QSplitter *logoSplitter = new QSplitter(this);
//    logoSplitter->setOrientation(Qt::Horizontal);
//    logoSplitter->addWidget(buttonProcess);


    // Source Geodetic & Vertical frame & CRS + data

    QSplitter *splitterH1 = new QSplitter(this);
    selectBtnSourceDataFile = new QPushButton();
    groupboxSourceData = new QGroupBox();
    groupboxSourceGeodeticCRS = new QGroupBox();
    groupboxSourceVerticalCRS = new QGroupBox();
    comboboxSourceGeodeticFrame = new QComboBox();
    comboboxSourceGeodeticCRS = new QComboBox();
    comboboxSourceVerticalFrame = new QComboBox();
    comboboxSourceAngularUnit = new QComboBox();
    comboboxSourceFormat = new QComboBox();
    checkboxSourceVerticalAuthoritative = new QCheckBox();
    tabwidgetSourceDataFile = new QTabWidget();
    setDataLayout("Input", "CRS", groupboxSourceData, tabwidgetSourceDataFile, comboboxSourceAngularUnit,
                  comboboxSourceFormat, &lineditSourceEpoch, selectBtnSourceDataFile,
                  &lineditSourceDataFile, NULL, groupboxSourceVerticalCRS, comboboxSourceVerticalFrame,
                  checkboxSourceVerticalAuthoritative,&labelSourceVerticalCRSinfo,
                  groupboxSourceGeodeticCRS, comboboxSourceGeodeticFrame,
                  comboboxSourceGeodeticCRS, &labelSourceGeodeticCRSinfo,
                  true,NULL,&labelSourceDataInfo,"",NULL);

    // Geodetic and Vertical transformations

    QSplitter *splitterH2 = new QSplitter(this);
    groupboxGeodeticTransfo = new QGroupBox();
    groupboxVerticalTransfo = new QGroupBox();
    comboboxGeodeticTransfo = new QComboBox();
    comboboxVerticalTransfo = new QComboBox();
    setTransfoLayout(splitterH2, Qt::Horizontal,
                      groupboxGeodeticTransfo, comboboxGeodeticTransfo,
                      groupboxVerticalTransfo, comboboxVerticalTransfo);


    // Target Geodetic & Vertical frame & CRS + data

    QSplitter *splitterH3 = new QSplitter(this);
    selectBtnTargetDataFile = new QPushButton();
    groupboxTargetData = new QGroupBox();
    groupboxTargetGeodeticCRS = new QGroupBox();
    groupboxTargetVerticalCRS = new QGroupBox();
    comboboxTargetGeodeticFrame = new QComboBox();
    comboboxTargetGeodeticCRS = new QComboBox();
    comboboxTargetVerticalFrame = new QComboBox();
    comboboxTargetAngularUnit = new QComboBox();
    comboboxTargetFormat = new QComboBox();
    checkboxTargetVerticalAuthoritative = new QCheckBox();
    tabwidgetTargetDataFile = new QTabWidget();
    labelLogo = new QLabel ();
    setDataLayout("Output", "CRS",groupboxTargetData, tabwidgetTargetDataFile, comboboxTargetAngularUnit,
                  comboboxTargetFormat, &lineditTargetEpoch, selectBtnTargetDataFile,
                  &lineditTargetDataFile, NULL, groupboxTargetVerticalCRS, comboboxTargetVerticalFrame,
                  checkboxTargetVerticalAuthoritative,&labelTargetVerticalCRSinfo,
                  groupboxTargetGeodeticCRS, comboboxTargetGeodeticFrame,
                  comboboxTargetGeodeticCRS, &labelTargetGeodeticCRSinfo,
                  true,buttonProcess,&labelTargetDataInfo,CO.logoFilename.data(),labelLogo);

    // Command-line display

    displayCommandLineWhileEditing();

    // Set up the widgets at their locations

    std::string mainDirection = CO.appStyle;
    QSplitter *splitter = new QSplitter(this);

    if (CO.scrollArea)
    {
        QScrollArea *scrollarea = new QScrollArea(this);
        scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollarea->setWidget(splitter);
        scrollarea->setWidgetResizable(true);//always fit content to window
        this->setCentralWidget(scrollarea);
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        if (mainDirection == "Horizontal")
        {
            scrollarea->setMinimumWidth(850);
            scrollarea->setMinimumHeight(450);
        }
        else
        {
            scrollarea->setMinimumWidth(600);
            scrollarea->setMinimumHeight(600);
            scrollarea->setMaximumHeight(screenGeometry.height());
        }
    }
    else
    {
        setCentralWidget(splitter);
    }

    if (mainDirection == "Horizontal")
    {
        QList<int> liV;
        liV << 1 << 1 << 1 << 1 << 1 << 1;
        splitter->setSizes(liV);
        splitter->setOrientation(Qt::Vertical);

        QSplitter *splitterST = new QSplitter(this);
        splitterST->setOrientation(Qt::Horizontal);
        QSplitter *splitterS = new QSplitter(this);
        splitterS->setOrientation(Qt::Vertical);
        splitterS->addWidget(splitterH1);
        splitterS->addWidget(groupboxSourceData);
        splitterST->addWidget(splitterS);
        QSplitter *splitterT = new QSplitter(this);
        splitterT->setOrientation(Qt::Vertical);
        splitterT->addWidget(splitterH3);
        splitterT->addWidget(groupboxTargetData);
        splitterST->addWidget(splitterT);

        QSplitter *middleSplitter = new QSplitter(this);
        middleSplitter->setOrientation(Qt::Horizontal);
//        middleSplitter->addWidget(logoSplitter);
        middleSplitter->addWidget(splitterH2);

        splitter->addWidget(splitterST);
        splitter->addWidget(middleSplitter);
        splitter->addWidget(&labelMessage);
    }
    else
    {
        QList<int> liV;
        liV << 1 << 2 << 1 << 1 << 1 << 1;
        splitter->setOrientation(Qt::Vertical);
        splitter->setSizes(liV);
        splitter->addWidget(splitterH1);
        splitter->addWidget(groupboxSourceData);
        splitter->addWidget(splitterH3);
        splitter->addWidget(groupboxTargetData);
        QSplitter *middleSplitter = new QSplitter(this);
        middleSplitter->setOrientation(Qt::Horizontal);
//        middleSplitter->addWidget(logoSplitter);
        middleSplitter->addWidget(splitterH2);
        splitter->addWidget(middleSplitter);
        splitter->addWidget(&labelMessage);
    }


    // SIGNALS & SLOTS

    connect(groupboxSourceGeodeticCRS, SIGNAL(toggled(bool)), this, SLOT(groupboxSourceGeodeticCRS_toggled(bool)));
    connect(groupboxTargetGeodeticCRS, SIGNAL(toggled(bool)), this, SLOT(groupboxTargetGeodeticCRS_toggled(bool)));
    connect(groupboxSourceVerticalCRS, SIGNAL(toggled(bool)), this, SLOT(groupboxSourceVerticalCRS_toggled(bool)));
    connect(groupboxTargetVerticalCRS, SIGNAL(toggled(bool)), this, SLOT(groupboxTargetVerticalCRS_toggled(bool)));
    connect(comboboxSourceGeodeticFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxSourceGeodeticFrame_currentIndexChanged(int)));
    connect(comboboxSourceVerticalFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxSourceVerticalFrame_currentIndexChanged(int)));
    connect(comboboxSourceGeodeticCRS, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxSourceGeodeticCRS_currentIndexChanged(int)));
    connect(checkboxSourceVerticalAuthoritative, SIGNAL(stateChanged(int)), this, SLOT(checkboxSourceVerticalAuthoritative_stateChanged(int)));
    connect(selectBtnSourceDataFile, SIGNAL(clicked()), this, SLOT(selectBtnSourceDataFile_clicked()));
    connect(tabwidgetSourceDataFile , SIGNAL(currentChanged(int)),this,SLOT(tabwidgetSourceDataFile_currentChanged(int)));
    connect(comboboxSourceFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxSourceFormat_currentIndexChanged(int)));
    connect(comboboxSourceAngularUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxSourceAngularUnit_currentIndexChanged(int)));
    connect(comboboxTargetGeodeticFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxTargetGeodeticFrame_currentIndexChanged(int)));
    connect(comboboxTargetVerticalFrame, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxTargetVerticalFrame_currentIndexChanged(int)));
    connect(comboboxTargetGeodeticCRS, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxTargetGeodeticCRS_currentIndexChanged(int)));
    connect(checkboxTargetVerticalAuthoritative, SIGNAL(stateChanged(int)), this, SLOT(checkboxTargetVerticalAuthoritative_stateChanged(int)));
    connect(selectBtnTargetDataFile, SIGNAL(clicked()), this, SLOT(selectBtnTargetDataFile_clicked()));
    connect(tabwidgetTargetDataFile, SIGNAL(currentChanged(int)),this,SLOT(tabwidgetTargetDataFile_currentChanged(int)));
    connect(comboboxTargetFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxTargetFormat_currentIndexChanged(int)));
    connect(comboboxTargetAngularUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxTargetAngularUnit_currentIndexChanged(int)));
    connect(comboboxGeodeticTransfo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxGeodeticTransfo_currentIndexChanged(int)));
    connect(comboboxVerticalTransfo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxVerticalTransfo_currentIndexChanged(int)));
    connect(&lineditSourceI, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceI_textChanged(QString)));
    connect(&lineditSourceX, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceX_textChanged(QString)));
    connect(&lineditSourceY, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceY_textChanged(QString)));
    connect(&lineditSourceZ, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceZ_textChanged(QString)));
    connect(&lineditSourceE, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceE_textChanged(QString)));
    connect(&lineditSourceN, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceN_textChanged(QString)));
    connect(&lineditSourceH, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceH_textChanged(QString)));
    connect(&lineditSourceL, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceL_textChanged(QString)));
    connect(&lineditSourceP, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceP_textChanged(QString)));
    connect(&lineditSourceV, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceV_textChanged(QString)));
    connect(&lineditSourceEpoch, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceEpoch_textChanged(QString)));
    connect(&lineditTargetEpoch, SIGNAL(textChanged(QString)), this, SLOT(lineditTargetEpoch_textChanged(QString)));
    connect(&lineditSourceDataFile, SIGNAL(textChanged(QString)), this, SLOT(lineditSourceDataFile_textChanged(QString)));
    connect(&lineditTargetDataFile, SIGNAL(textChanged(QString)), this, SLOT(lineditTargetDataFile_textChanged(QString)));
    connect(buttonProcess, SIGNAL(clicked()), this, SLOT(buttonProcess_clicked()));

    InitWidgetValues(circeOptionFile);
    bUpdateCRSid = true;
    statusBar()->showMessage(tr(""));
    setFocus(); //focus on the main window, not on a combobox or another widget
}

void MainWindow::setPt4dLayout(QString what, QTabWidget *tabwidgeDataFile, const std::string& lnfrmt, QLineEdit *linedit_I,
                   QLineEdit *linedit_X, QLineEdit *linedit_Y, QLineEdit *linedit_Z, QLineEdit *linedit_L,
                   QLineEdit *linedit_P, QLineEdit *linedit_H, QLineEdit *linedit_E, QLineEdit *linedit_N,
                   QLineEdit *linedit_V, QLineEdit *linedit_C, QLineEdit *linedit_S, QLineEdit *linedit_D)
{
    bool oldState = tabwidgeDataFile->blockSignals(true);
    bool displayMore = (CO.printCS && (what=="target"));
    int cI = tabwidgeDataFile->currentIndex();
    tabwidgeDataFile->removeTab(1);
    QGridLayout *gridLayoutData = new QGridLayout();
    unsigned int ifrmt;
    //int i0col=2, i1col=2, i2col=2;
    int labW=100, editW=160, iseq=0, iseq2=0;
    QLabel *lab;
    QString labD(circe::InterfaceElementText[circe::IE_DEFLECTION].data());
    labD += QString(" ")+QChar(0x03B7); //eta
    labD += QString(" ")+QChar(0x03BE); //xi
    labD += " (\")";
    for (ifrmt=0; ifrmt<lnfrmt.size(); ifrmt++)
    {
        switch (lnfrmt[ifrmt])
        {
        case 'I':
            linedit_I->setMaximumWidth(50);
            lab = new QLabel("id");
            lab->setMaximumWidth(labW);
            gridLayoutData->addWidget(lab,0,0,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_I,0,1);
            iseq++;
            if (!displayMore)
                iseq++;
            break;
        case 'X':
            linedit_X->setMaximumWidth(editW);
            lab = new QLabel("X");
            lab->setMaximumWidth(labW);
            gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_X,0,iseq++);
            break;
        case 'Y':
            linedit_Y->setMaximumWidth(editW);
            lab = new QLabel("Y");
            lab->setMaximumWidth(labW);
            gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_Y,0,iseq++);
            break;
        case 'Z':
            linedit_Z->setMaximumWidth(editW);
            lab = new QLabel("Z");
            lab->setMaximumWidth(labW);
            gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_Z,0,iseq++);
            break;
        case 'L':
            linedit_L->setMaximumWidth(editW);
            lab = new QLabel("Longitude");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_L,iseq++,1);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_L,0,iseq++);
            }
            break;
        case 'P':
            linedit_P->setMaximumWidth(editW);
            lab = new QLabel("Latitude");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_P,iseq++,1);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_P,0,iseq++);
            }
            break;
        case 'H':
            linedit_H->setMaximumWidth(editW);
            lab = new QLabel("h");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_H,iseq++,1,Qt::AlignLeft);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_H,0,iseq++);
            }
            break;
        case 'E':
            linedit_E->setMaximumWidth(editW);
            lab = new QLabel("E");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_E,iseq++,1,Qt::AlignLeft);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_E,0,iseq++,Qt::AlignLeft);
            }
            break;
        case 'N':
            linedit_N->setMaximumWidth(editW);
            lab = new QLabel("N");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_N,iseq++,1,Qt::AlignLeft);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_N,0,iseq++,Qt::AlignLeft);
            }
            break;
        case 'V':
            linedit_V->setMaximumWidth(editW);
            lab = new QLabel("H");
            lab->setMaximumWidth(labW);
            if (displayMore)
            {
                gridLayoutData->addWidget(lab,iseq,0,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_V,iseq++,1,Qt::AlignLeft);
            }else{
                gridLayoutData->addWidget(lab,0,iseq++,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_V,0,iseq++);
            }
            break;
        case 'C':
            linedit_C->setMaximumWidth(labW);
            lab = new QLabel(circe::InterfaceElementText[circe::IE_CONVERGENCE].data());
            lab->setMaximumWidth(editW);
            gridLayoutData->addWidget(lab,iseq2,2,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_C,iseq2++,3);
            break;
        case 'S':
            linedit_S->setMaximumWidth(labW);
            lab = new QLabel(this);
            lab->setMaximumWidth(editW);
            gridLayoutData->addWidget(lab,iseq2,2,Qt::AlignRight);
            gridLayoutData->addWidget(linedit_S,iseq2++,3);
            if (CO.noPrintSppm)
                lab->setText(QString(circe::InterfaceElementText[circe::IE_SCALE_FACTOR].data()));
            else
                lab->setText(QString(circe::InterfaceElementText[circe::IE_SCALE_ERROR].data()));
            break;
        case 'D':
            //if (circe::hasVertical(CO.targetOptions.formatId))
            //{
                linedit_D->setMaximumWidth(labW);
                lab = new QLabel(labD);
                lab->setMaximumWidth(editW);
                gridLayoutData->addWidget(lab,iseq2,2,Qt::AlignRight);
                gridLayoutData->addWidget(linedit_D,iseq2++,3);
            //}
            break;
        default:
            break;
        }
    }

//    gridLayoutData->setMargin(0);
//    gridLayoutData->setSpacing(0);
    QWidget *groupbox1 = new QWidget();
    groupbox1->setLayout(gridLayoutData);
    tabwidgeDataFile->insertTab(1,groupbox1,QString(circe::InterfaceElementText[circe::IE_POINT].data()));
    tabwidgeDataFile->setCurrentIndex(cI);
    tabwidgeDataFile->blockSignals(oldState);
}

void setSelectFileLayout(QGridLayout *gridLayoutData, QString label_text, QPushButton *v_selectBtn,
                         QString selectBtn_text, QLineEdit *v_linedit)
{
    gridLayoutData->setContentsMargins(QMargins(2,0,2,0));
    v_selectBtn->setText(selectBtn_text);
    v_selectBtn->setMaximumWidth(100);
    //gridLayoutData->addWidget(new QLabel(label_text),0,0,1,1);
    gridLayoutData->addWidget(v_linedit,0,0,1,2);
    gridLayoutData->addWidget(v_selectBtn,0,2,1,1);
}

void setComboBoxLayout(QString where, QGroupBox *_groupbox, QComboBox *_combobox, QCheckBox *_checkboxVerticalAuthoritative,
                       QComboBox *_combobox2, QLabel *_labelCRSinfo, QString title, bool checkable)
{
    std::string context="";
    if (title.left(12)=="Geodetic CRS")
    {
        context = "Geodetic CRS";
        title = (circe::InterfaceElementText[circe::IE_GEODETIC_CRS]).data();
        _combobox->setMinimumWidth(150);
    }
    else if (title.left(12)=="Vertical CRS")
    {
        context = "Vertical CRS";
        title = (circe::InterfaceElementText[circe::IE_VERTICAL_CRS]).data();
        _combobox->setMinimumWidth(150);
    }
    else if (title.left(23)=="Geodetic transformation")
    {
        title = circe::InterfaceElementText[circe::IE_GEODETIC_TRANSFO].data();
        _combobox->setMinimumWidth(200);
    }
    else if (title.left(23)=="Vertical transformation")
    {
        title = circe::InterfaceElementText[circe::IE_VERTICAL_TRANSFO].data();
        _combobox->setMinimumWidth(200);
    }
    _combobox->adjustSize();
    _groupbox->setTitle(title);
    _groupbox->setCheckable(checkable);
    _groupbox->setChecked(true);
    _groupbox->adjustSize();
    QGridLayout *gridLayout = new QGridLayout();
    _combobox->setCurrentIndex(0);
    gridLayout->addWidget(_combobox,0,0, Qt::AlignLeft);
    if (context=="Geodetic CRS")
    {
        gridLayout->addWidget(_combobox2,0,1, Qt::AlignLeft);
        _combobox2->setMinimumWidth(200);
    }
    else if (context=="Vertical CRS")
    {
        gridLayout->addWidget(_checkboxVerticalAuthoritative,1,0, Qt::AlignLeft);
        _checkboxVerticalAuthoritative->setMinimumWidth(200);
        _checkboxVerticalAuthoritative->setText(circe::InterfaceElementText[circe::IE_AUTHORITATIVE_VERTICAL_FRAMES].data());
    }
    if (_labelCRSinfo!=NULL)
        gridLayout->addWidget(_labelCRSinfo,2,0,1,2, Qt::AlignLeft);
    _groupbox->setLayout(gridLayout);
}

void setDataLayout(QString where, QString what, QGroupBox *_groupbox, QTabWidget *_tabwidget, QComboBox *_comboBoxAngularUnit,
                   QComboBox *_comboBoxFormat, QLineEdit *_lineeditEpoch, QPushButton *_selectBtnDataFile,
                   QLineEdit *_lineditSourceDataFile, QCheckBox *_checkboxPrintCS,
                   QGroupBox *groupboxVerticalCRS, QComboBox *comboboxVerticalFrame,
                   QCheckBox *_checkboxVerticalAuthoritative, QLabel *labelVerticalCRSinfo,
                   QGroupBox *groupboxGeodeticCRS, QComboBox *comboboxGeodeticFrame,
                   QComboBox *comboboxGeodeticCRS, QLabel *labelGeodeticCRSinfo,
                   bool checkable, QToolButton* buttonProcess, QLabel *_labelDataInfo,
                   QString logoFilename, QLabel *_labelLogo)
{
    if (where == "Input")
        _groupbox->setTitle(circe::InterfaceElementText[circe::IE_INPUT].data());
    else if (where == "Output")
        _groupbox->setTitle(circe::InterfaceElementText[circe::IE_OUTPUT].data());
    _groupbox->setContentsMargins(QMargins(2,0,2,0));
    _groupbox->setAlignment(Qt::AlignHCenter);
    _groupbox->setStyleSheet("QGroupBox {font: bold}");

    QGridLayout *gridDataLayout = new QGridLayout();

    _comboBoxFormat->setMinimumWidth(300);
    _lineeditEpoch->setMinimumWidth(100);

    setComboBoxLayout(where, groupboxGeodeticCRS,comboboxGeodeticFrame,_checkboxVerticalAuthoritative,
                      comboboxGeodeticCRS,labelGeodeticCRSinfo,"Geodetic "+what,false);

    gridDataLayout->addWidget(_labelDataInfo,2,0,1,3);
    gridDataLayout->addWidget(_comboBoxFormat,3,0,1,1);
    gridDataLayout->addWidget(_comboBoxAngularUnit,3,1,1,1);
    //_lineeditEpoch->setText(("("+circe::InterfaceElementText[circe::IE_EPOCH]+")").data());
    gridDataLayout->addWidget(_lineeditEpoch,3,2,1,1);

    setComboBoxLayout(where, groupboxVerticalCRS,comboboxVerticalFrame,_checkboxVerticalAuthoritative,
                      NULL,labelVerticalCRSinfo,"Vertical "+what,checkable);
    gridDataLayout->addWidget(groupboxVerticalCRS,4,0,2,2);

    if (_checkboxPrintCS != NULL)
    {
        _checkboxPrintCS->setText(circe::InterfaceElementText[circe::IE_PRINT_CS].data());
        gridDataLayout->addWidget(_checkboxPrintCS,3,1,1,4);
    }

    QGridLayout *gridLayoutData2 = new QGridLayout();
    setSelectFileLayout(gridLayoutData2, circe::InterfaceElementText[circe::IE_DATA_FILE].data(),
                        _selectBtnDataFile, circe::InterfaceElementText[circe::IE_SELECT].data(),
                        _lineditSourceDataFile);
    QWidget *groupbox2 = new QWidget();
    groupbox2->setLayout(gridLayoutData2);
    _tabwidget->addTab(groupbox2,circe::InterfaceElementText[circe::IE_FILE].data());

    // The point data widgets are set up in comboboxSourceFormat_currentIndexChanged
    // and comboboxTargetFormat_currentIndexChanged

//    _tabwidget->setCurrentIndex(CO.dataMode-1);

    if (where == "Output")
    {
        //buttonProcess->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        //buttonProcess->setIcon(QPixmap(":/images/logocouleur.png"));
        //buttonProcess->setIconSize(QSize(100,100));
        buttonProcess->setText(PROCESS_TEXT);
        //https://www.rapidtables.com/web/color/brown-color.html
        buttonProcess->setStyleSheet("QToolButton { background-color : rgb(245,222,179); color : green; font: bold 14px; padding: 5px }");
        gridDataLayout->addWidget(buttonProcess,6,0,1,2);

        _labelLogo->setStyleSheet("background-image:url("+logoFilename+"); background-position: center; background-repeat: no;");
        _labelLogo->setScaledContents( false );
        _labelLogo->setMinimumSize(100,110);
        gridDataLayout->addWidget(_labelLogo,4,2,3,2);

        gridDataLayout->addWidget(_tabwidget,7,0,2,3);
    }
    else
        gridDataLayout->addWidget(_tabwidget,6,0,2,3);

    gridDataLayout->addWidget(groupboxGeodeticCRS,0,0,2,3);

    _groupbox->setLayout(gridDataLayout);
}

void setTransfoLayout(QSplitter *splitter, Qt::Orientation orientation,
                       QGroupBox *groupboxGeodeticCRS, QComboBox *comboboxGeodeticFrame,
                       QGroupBox *groupboxVerticalCRS, QComboBox *comboboxVerticalFrame)
{
    QList<int> li;
    li << 1 << 1;
    splitter->setOrientation(orientation);
    splitter->setSizes(li);
//    setCentralWidget(splitterH1);
    setComboBoxLayout("", groupboxGeodeticCRS,comboboxGeodeticFrame,NULL,NULL,NULL,"Geodetic transformation",false);
    splitter->addWidget(groupboxGeodeticCRS);
    setComboBoxLayout("", groupboxVerticalCRS,comboboxVerticalFrame,NULL,NULL,NULL,"Vertical transformation",false);
    splitter->addWidget(groupboxVerticalCRS);
    splitter->setContentsMargins(QMargins(2,0,2,0));
}

void MainWindow::populateComboBox(QComboBox *_combobox, std::vector<std::string> displayList, int iSel, std::string tmpinfo)
{
    //qDebug("%s",tmpinfo.data());
    //_combobox->blockSignals(true);
    _combobox->clear();
    if(displayList.size()<=0)
        return;
    std::vector<std::string>::const_iterator its;
    for(its=displayList.begin();its<displayList.end();its++)
    {
        //qDebug("%d %s",_combobox->currentIndex(),_combobox->currentText().data());
        if (_combobox->count() == displayList.size())
            break;
        _combobox->addItem(tr(its->data()));
    }
    //_combobox->blockSignals(false);
    _combobox->setCurrentIndex(iSel);
}
/*
void MainWindow::populateComboBox(QComboBox *_combobox, std::vector<std::string> displayList, int iSel, std::string tmpinfo)
{
    qDebug("%s",tmpinfo.data());
    if(displayList.size()<=0)
        return;
    _combobox->clear();
    _combobox->blockSignals(true);
    std::vector<std::string>::const_iterator its;
    for (unsigned int i=0; i<displayList.size(); i++)
    {
        if (i==displayList.size()-1)
            _combobox->blockSignals(false);
        //qDebug("%d %s",_combobox->currentIndex(),_combobox->currentText().data());
        if (size_t(_combobox->count()) == displayList.size())
            break;
        _combobox->addItem(tr(displayList[i].data()));
    }
    _combobox->blockSignals(false);
    _combobox->setCurrentIndex(iSel);
}
*/
void MainWindow::groupboxSourceGeodeticCRS_toggled(bool b)
{
    CO.sourceOptions.useGeodeticCRS = b;
    //if (!b) CO.sourceOptions.geodeticCRSid = "";
    groupboxTargetGeodeticCRS->setChecked(b);
    circe::getGeodeticSubset(&OS, &CO, &GS);
    populateComboBox(comboboxSourceVerticalFrame, OS.sourceSubset.verticalFrameDisplayList, OS.sourceSubset.iVerticalFrame,"groupboxSourceGeodeticCRS_toggled");
    displayCommandLineWhileEditing();
}

void MainWindow::groupboxTargetGeodeticCRS_toggled(bool b)
{
    CO.targetOptions.useGeodeticCRS = b;
    //if (!b) CO.targetOptions.geodeticCRSid = "";
    displayCommandLineWhileEditing();
}

void MainWindow::groupboxSourceVerticalCRS_toggled(bool b)
{
    b = circe::hasVertical(CO.sourceOptions.formatId);
    groupboxSourceVerticalCRS->setChecked(b);
    CO.sourceOptions.useVerticalCRS = b;
    displayCommandLineWhileEditing();
}

void MainWindow::groupboxTargetVerticalCRS_toggled(bool b)
{
    b = circe::hasVertical(CO.targetOptions.formatId);
    groupboxTargetVerticalCRS->setChecked(b);
    CO.targetOptions.useVerticalCRS = b;
    displayCommandLineWhileEditing();
}

void MainWindow::selectBtnSourceDataFile_clicked()
{
    std::string idir = circe::getDirname(CO.sourceOptions.pathname);
    if (idir == "") idir = circe::getDirname(CO.targetOptions.pathname);
    CO.sourceOptions.pathname = QFileDialog::getOpenFileName(0,"Select a file", idir.data(), "").toStdString();
    lineditSourceDataFile.setText(QString(CO.sourceOptions.pathname.data()));
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceDataFile_textChanged(QString s)
{
    CO.sourceOptions.pathname = /*circe::getDirname(CO.sourceOptions.pathname) +*/ s.toStdString();
    displayCommandLineWhileEditing();
}

void MainWindow::selectBtnTargetDataFile_clicked()
{
    std::string idir = circe::getDirname(CO.targetOptions.pathname);
    if (idir == "") idir = circe::getDirname(CO.sourceOptions.pathname);
    CO.targetOptions.pathname = QFileDialog::getSaveFileName(nullptr,"Select a file", idir.data(), "").toStdString();
    lineditTargetDataFile.setText(QString(CO.targetOptions.pathname.data()));
    displayCommandLineWhileEditing();
}
void MainWindow::lineditTargetDataFile_textChanged(QString s)
{
    std::string idir = circe::getDirname(s.toStdString());
    if (idir == "")
    {
        idir = circe::getDirname(CO.targetOptions.pathname);
        if (idir == "") idir = circe::getDirname(CO.sourceOptions.pathname);
        CO.targetOptions.pathname = idir + s.toStdString();
    }
    else
        CO.targetOptions.pathname = s.toStdString();
    displayCommandLineWhileEditing();
}

void MainWindow::populateSourceFormat()
{
    //populateComboBox(comboboxSourceFormat, OS.sourceSubset.dataFormatDisplayList, OS.sourceSubset.iDataFormat);
    if (CO.dataMode == circe::DM_FILE)
        populateComboBox(comboboxSourceFormat, OS.sourceSubset.dataFileFormatDisplayList, OS.sourceSubset.iDataFileFormat,"populateSourceFormat 1");
    else if (CO.dataMode == circe::DM_POINT)
        populateComboBox(comboboxSourceFormat, OS.sourceSubset.dataPointFormatDisplayList, OS.sourceSubset.iDataPointFormat,"populateSourceFormat 2");
    WriteInfo("Input");
}

void MainWindow::populateTargetFormat()
{
    //populateComboBox(comboboxTargetFormat, OS.targetSubset.dataFormatDisplayList, OS.targetSubset.iDataFormat);
    if (CO.dataMode == circe::DM_FILE)
        populateComboBox(comboboxTargetFormat, OS.targetSubset.dataFileFormatDisplayList, OS.targetSubset.iDataFileFormat,"populateTargetFormat 1");
    else if (CO.dataMode == circe::DM_POINT)
        populateComboBox(comboboxTargetFormat, OS.targetSubset.dataPointFormatDisplayList, OS.targetSubset.iDataPointFormat,"populateTargetFormat 2");
    WriteInfo("Output");
}

void MainWindow::makeFormatConsistent(const std::string& frmtId, int *iFormatList,
                                      const std::vector<std::string>& dataFormatList)
{
    if (circe::hasIdentifier(frmtId) && circe::hasVertical(frmtId))
    {
        for (int i=0;i<dataFormatList.size();i++)
            if ( circe::hasIdentifier(dataFormatList[i])
              && circe::hasVertical(dataFormatList[i]))
            {
                *iFormatList=i;
                break;
            }
    }
    else if (circe::hasVertical(frmtId))
    {
        for (int i=0;i<dataFormatList.size();i++)
            if (!(circe::hasIdentifier(dataFormatList[i]))
              && circe::hasVertical(dataFormatList[i]))
            {
                *iFormatList=i;
                break;
            }
    }
    else if (circe::hasIdentifier(frmtId) && circe::hasEllHeight(frmtId))
    {
        for (int i=0;i<dataFormatList.size();i++)
            if ( circe::hasIdentifier(dataFormatList[i])
              && circe::hasEllHeight(dataFormatList[i]))
            {
                *iFormatList=i;
                break;
            }
    }
    else if (circe::hasEllHeight(frmtId))
    {
        for (int i=0;i<dataFormatList.size();i++)
            if (!(circe::hasIdentifier(dataFormatList[i]))
              && circe::hasEllHeight(dataFormatList[i]))
            {
                *iFormatList=i;
                break;
            }
    }
    else if (circe::hasIdentifier(frmtId))
    {
        for (int i=0;i<dataFormatList.size();i++)
            if ( circe::hasIdentifier(dataFormatList[i]))
            {
                *iFormatList=i;
                break;
            }
    }
    else
    {
        for (int i=0;i<dataFormatList.size();i++)
            if (!(circe::hasIdentifier(dataFormatList[i])))
            {
                *iFormatList=i;
                break;
            }
    }
}

void MainWindow::tabwidgetSourceDataFile_currentChanged(int ind)
{
    if (ind<0) return;
    redrawPt4dLayout = false;
    try
    {
        //CO.dataMode = circe::DATA_MODE(ind+1);
        if (CO.dataMode != circe::DATA_MODE(ind+1))
        {
            if (CO.dataMode == circe::DM_FILE)
            {
                //Make the file points available in the POINT GUI
                CO.loadAsVector = 1;
                CO.gridLoading = "BINARY";
                circe::swain s;
                s.fi = new std::ifstream();
                s.fi->open(QString(CO.sourceOptions.pathname.data()).toLocal8Bit());
                circe::processIFStream(CO, &vectorPt4d, &GS, &s);
                CO.loadAsVector = 0;

                //Make the current POINT format consistent with the FILE format
                makeFormatConsistent(CO.sourceOptions.formatId, &(OS.sourceSubset.iDataPointFormat),
                                     OS.sourceSubset.dataPointFormatList);
                //Display first point
                if (vectorPt4d.size()>0)
                    UpdateSourcePointDisplay(&vectorPt4d.at(0));
            }
            else
                makeFormatConsistent(CO.sourceOptions.formatId, &(OS.sourceSubset.iDataFileFormat),
                                     OS.sourceSubset.dataFileFormatList);
            CO.dataMode = circe::DATA_MODE(ind+1);
            //comboboxSourceFormat_currentIndexChanged(comboboxSourceFormat.currentIndex());
            populateSourceFormat();
            populateTargetFormat();
        }
    }
    catch (std::string str)
    {
        labelMessage.setText(str.data());
        labelMessage.setStyleSheet("QLabel { background-color : red; color : black; font: bold 12px;}");
    }
    tabwidgetTargetDataFile->setCurrentIndex(ind);
    displayCommandLineWhileEditing();
    redrawPt4dLayout = true;
}

void MainWindow::tabwidgetTargetDataFile_currentChanged(int ind)
{
    if (ind<0) return;
    redrawPt4dLayout = false;
    //CO.dataMode = circe::DATA_MODE(ind+1);
    if (CO.dataMode != circe::DATA_MODE(ind+1))
    {
        CO.dataMode = circe::DATA_MODE(ind+1);
        if (CO.dataMode == circe::DM_FILE)
            makeFormatConsistent(CO.targetOptions.formatId, &(OS.targetSubset.iDataPointFormat),
                                 OS.targetSubset.dataPointFormatList);
        else
            makeFormatConsistent(CO.targetOptions.formatId, &(OS.targetSubset.iDataFileFormat),
                                 OS.targetSubset.dataFileFormatList);
        populateSourceFormat();
        populateTargetFormat();
        //comboboxTargetFormat_currentIndexChanged(comboboxTargetFormat.currentIndex());
    }
    tabwidgetSourceDataFile->setCurrentIndex(ind);
    displayCommandLineWhileEditing();
    redrawPt4dLayout = true;
}

void MainWindow::comboboxSourceGeodeticFrame_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.sourceOptions.geodeticFrameId != OS.sourceSubset.geodeticFrameIdList[ind])
    {
        CO.sourceOptions.geodeticFrameId = OS.sourceSubset.geodeticFrameIdList[ind];
        if (bUpdateCRSid)
            CO.sourceOptions.geodeticCRSid = "";
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
    populateComboBox(comboboxSourceGeodeticCRS, OS.sourceSubset.geodeticCRSdisplayList, OS.sourceSubset.iGeodeticCRS,"comboboxSourceGeodeticFrame_currentIndexChanged 1");
    //comboboxSourceGeodeticCRS_currentIndexChanged(comboboxSourceGeodeticCRS.currentIndex());
    populateComboBox(comboboxTargetGeodeticFrame, OS.targetSubset.geodeticFrameDisplayList, OS.targetSubset.iGeodeticFrame,"comboboxSourceGeodeticFrame_currentIndexChanged 2");
//    comboboxTargetGeodeticCRS.clear();
    //WriteInfo("Input");
    displayCommandLineWhileEditing();
}
void MainWindow::comboboxSourceVerticalFrame_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.sourceOptions.verticalFrameId != OS.sourceSubset.verticalFrameIdList[ind])
    {
        CO.sourceOptions.verticalFrameId = OS.sourceSubset.verticalFrameIdList[ind];
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
    populateComboBox(comboboxTargetVerticalFrame, OS.targetSubset.verticalFrameDisplayList, OS.targetSubset.iVerticalFrame,"comboboxSourceVerticalFrame_currentIndexChanged");
    displayCommandLineWhileEditing();
    //labelSourceVerticalCRSinfo.setText(getValidityAreaCommandLine(CO.sourceOptions.angularUnit, CO.sourceOptions.verticalCRSid).data());
    displaySourceValidityArea();
}
void MainWindow::comboboxTargetGeodeticFrame_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.targetOptions.geodeticFrameId != OS.targetSubset.geodeticFrameIdList[ind])
    {
        CO.targetOptions.geodeticFrameId = OS.targetSubset.geodeticFrameIdList[ind];
        if (bUpdateCRSid)
            CO.targetOptions.geodeticCRSid = "";
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
//    comboboxTargetFormat_currentIndexChanged(0);
    populateComboBox(comboboxTargetGeodeticCRS, OS.targetSubset.geodeticCRSdisplayList, OS.targetSubset.iGeodeticCRS,"comboboxTargetGeodeticFrame_currentIndexChanged 1");
    //comboboxTargetGeodeticCRS_currentIndexChanged(comboboxTargetGeodeticCRS.currentIndex());
    populateComboBox(comboboxTargetVerticalFrame, OS.targetSubset.verticalFrameDisplayList, OS.targetSubset.iVerticalFrame,"comboboxTargetGeodeticFrame_currentIndexChanged 2");
    populateComboBox(comboboxGeodeticTransfo, OS.geodeticTransfoDisplayList, OS.iGeodeticTransfo,"comboboxTargetGeodeticFrame_currentIndexChanged 3");
    populateComboBox(comboboxVerticalTransfo, OS.verticalTransfoDisplayList, OS.iVerticalTransfo,"comboboxTargetGeodeticFrame_currentIndexChanged 4");
    comboboxGeodeticTransfo->setEnabled(OS.geodeticTransfoDisplayList.size()>1);
    comboboxVerticalTransfo->setEnabled(OS.verticalTransfoDisplayList.size()>1);
    //WriteInfo("Output");
    displayCommandLineWhileEditing();
}
void MainWindow::comboboxTargetVerticalFrame_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.targetOptions.verticalFrameId != OS.targetSubset.verticalFrameIdList[ind])
    {
        CO.targetOptions.verticalFrameId = OS.targetSubset.verticalFrameIdList[ind];
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
    populateComboBox(comboboxVerticalTransfo, OS.verticalTransfoDisplayList, OS.iVerticalTransfo,"comboboxTargetVerticalFrame_currentIndexChanged");
    comboboxVerticalTransfo->setEnabled(OS.verticalTransfoDisplayList.size()>1);
    displayCommandLineWhileEditing();
    //labelTargetVerticalCRSinfo.setText(getValidityAreaCommandLine(CO.targetOptions.angularUnit, CO.targetOptions.verticalCRSid).data());
    displayTargetValidityArea();
}
void MainWindow::comboboxSourceGeodeticCRS_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.sourceOptions.geodeticCRSid != OS.sourceSubset.geodeticCRSidList[ind])
    {
        CO.sourceOptions.geodeticCRSid = OS.sourceSubset.geodeticCRSidList[ind];
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
    populateSourceFormat();
    populateComboBox(comboboxSourceAngularUnit, OS.sourceSubset.dataUnitDisplayList, OS.sourceSubset.iDataUnit,"comboboxSourceGeodeticCRS_currentIndexChanged 1");
    if (OS.sourceSubset.geodeticDimension == circe::SD_4D)
        lineditSourceEpoch.setVisible(true);
    else
    {
        lineditSourceEpoch.setVisible(false);
        lineditSourceEpoch.setText("");
        CO.sourceOptions.epoch = 0;
    }
//    comboboxSourceFormat_currentIndexChanged(OS.sourceSubset.iDataPointFormat);
    populateComboBox(comboboxSourceVerticalFrame, OS.sourceSubset.verticalFrameDisplayList, OS.sourceSubset.iVerticalFrame,"comboboxSourceGeodeticCRS_currentIndexChanged 2");
    displayCommandLineWhileEditing();
    labelSourceGeodeticCRSinfo.setText(getValidityAreaCommandLine(CO.sourceOptions.angularUnit, CO.sourceOptions.geodeticCRSid).data());
    WriteInfo("Input");
}
void MainWindow::comboboxTargetGeodeticCRS_currentIndexChanged(int ind)
{
    if (ind<0) return;
    if (CO.targetOptions.geodeticCRSid != OS.targetSubset.geodeticCRSidList[ind])
    {
        CO.targetOptions.geodeticCRSid = OS.targetSubset.geodeticCRSidList[ind];
        circe::getGeodeticSubset(&OS, &CO, &GS);
    }
    populateTargetFormat();
    populateComboBox(comboboxTargetAngularUnit, OS.targetSubset.dataUnitDisplayList, OS.targetSubset.iDataUnit,"comboboxTargetGeodeticCRS_currentIndexChanged");
    if (OS.targetSubset.geodeticDimension == circe::SD_4D)
        lineditTargetEpoch.setVisible(true);
    else
    {
        lineditTargetEpoch.setVisible(false);
        lineditTargetEpoch.setText("");
        CO.targetOptions.epoch = 0;
    }
    displayCommandLineWhileEditing();
    labelTargetGeodeticCRSinfo.setText(getValidityAreaCommandLine(CO.targetOptions.angularUnit, CO.targetOptions.geodeticCRSid).data());
    WriteInfo("Output");
}

void MainWindow::comboboxSourceFormat_currentIndexChanged(int ind)
{
    if (ind<0) return;
    comboboxSourceAngularUnit->setVisible(CO.sourceOptions.geodeticCRStype == circe::CT_GEOGRAPHIC);
    //CO.sourceOptions.formatId = OS.sourceSubset.dataFormatList[ind];
    if (bUpdateCRSid)
    {
        if (CO.dataMode == circe::DM_FILE)
            CO.sourceOptions.formatId = OS.sourceSubset.dataFileFormatList[ind];
        else if (CO.dataMode == circe::DM_POINT)
            CO.sourceOptions.formatId = OS.sourceSubset.dataPointFormatList[ind];
    }
    //if (CO.sourceOptions.formatId == "GDAL_OGR")
      //  comboboxTargetFormat_currentIndexChanged(ind);
        //CO.targetOptions.formatId = "GDAL_OGR";
    groupboxSourceVerticalCRS->setChecked(circe::hasVertical(CO.sourceOptions.formatId));
    if (!(circe::hasVertical(CO.sourceOptions.formatId)))
    {
        CO.sourceOptions.verticalCRSid = "";
        CO.sourceOptions.verticalFrameId = "";
    }

    bool oldState = comboboxSourceFormat->blockSignals(true);

    circe::getGeodeticSubset(&OS, &CO, &GS);
    populateComboBox(comboboxSourceVerticalFrame, OS.sourceSubset.verticalFrameDisplayList, OS.sourceSubset.iVerticalFrame,"comboboxSourceFormat_currentIndexChanged");

    if (CO.dataMode == circe::DM_FILE
     && circe::setFormatDefaults(&(CO.sourceOptions),&GS))
    {
        circe::getGeodeticSubset(&OS, &CO, &GS);
        comboboxSourceGeodeticCRS->setCurrentText(QString(circe::CRStypeDisplayText[CO.sourceOptions.geodeticCRStype].data()));
        comboboxSourceAngularUnit->setVisible(CO.sourceOptions.geodeticCRStype == circe::CT_GEOGRAPHIC);
    }

    if (redrawPt4dLayout)
    setPt4dLayout("source", tabwidgetSourceDataFile, CO.sourceOptions.formatId, &lineditSourceI, &lineditSourceX,
                  &lineditSourceY, &lineditSourceZ, &lineditSourceL, &lineditSourceP, &lineditSourceH,
                  &lineditSourceE, &lineditSourceN, &lineditSourceV, &lineditSourceC, &lineditSourceS,
                  &lineditSourceD);
    comboboxSourceFormat->setCurrentIndex(ind);
    comboboxSourceFormat->blockSignals(oldState);
    displaySourceValidityArea();
    displayCommandLineWhileEditing();
}

void MainWindow::comboboxSourceAngularUnit_currentIndexChanged(int ind)
{
    if (ind<0) return;
    CO.sourceOptions.angularUnit = OS.sourceSubset.dataUnitList[ind];
    displayCommandLineWhileEditing();
    displaySourceValidityArea();
}


void MainWindow::comboboxTargetFormat_currentIndexChanged(int ind)
{
    if (ind<0) return;
    //CO.targetOptions.formatId = OS.targetSubset.dataFormatList[ind];
    if (bUpdateCRSid)
    {
        if (CO.dataMode == circe::DM_FILE)
            CO.targetOptions.formatId = OS.targetSubset.dataFileFormatList[ind];
        else if (CO.dataMode == circe::DM_POINT)
            CO.targetOptions.formatId = OS.targetSubset.dataPointFormatList[ind];
    }

    if (!(circe::hasVertical(CO.sourceOptions.formatId)))
    {
        CO.sourceOptions.verticalFrameId = "";
        CO.sourceOptions.verticalCRSid = "";
    }
    if (!(circe::hasVertical(CO.targetOptions.formatId)))
    {
        CO.targetOptions.verticalFrameId = "";
        CO.targetOptions.verticalCRSid = "";
    }
    circe::getGeodeticSubset(&OS, &CO, &GS);
    populateComboBox(comboboxTargetVerticalFrame, OS.targetSubset.verticalFrameDisplayList, OS.targetSubset.iVerticalFrame,"comboboxTargetFormat_currentIndexChanged");

    if (CO.targetOptions.geodeticCRStype == circe::CT_GEOGRAPHIC)
    {
        comboboxTargetAngularUnit->setVisible(true);
        checkboxPrintCS_stateChanged(CO.printCS);
    }
    else if (CO.targetOptions.geodeticCRStype == circe::CT_PROJECTED && CO.printCS)
    {
        comboboxTargetAngularUnit->setVisible(true);
        checkboxPrintCS_stateChanged(CO.printCS);
    }
    else
        comboboxTargetAngularUnit->setVisible(false);
    groupboxTargetVerticalCRS->setChecked(circe::hasVertical(CO.targetOptions.formatId));

    bool oldState = comboboxTargetFormat->blockSignals(true);
    if (redrawPt4dLayout)
    setPt4dLayout("target", tabwidgetTargetDataFile, CO.targetOptions.formatId, &lineditTargetI, &lineditTargetX,
                  &lineditTargetY, &lineditTargetZ, &lineditTargetL, &lineditTargetP, &lineditTargetH,
                  &lineditTargetE, &lineditTargetN, &lineditTargetV, &lineditTargetC, &lineditTargetS,
                  &lineditTargetD);
    comboboxTargetFormat->setCurrentIndex(ind);
    comboboxTargetFormat->blockSignals(oldState);
    displayTargetValidityArea();
    displayCommandLineWhileEditing();
}

void MainWindow::checkboxPrintCS_stateChanged(int state)
{
    CO.targetOptions.formatId = circe::rmExtraFormat(CO.targetOptions.formatId);
    if (state)
    {
        if (CO.targetOptions.geodeticCRStype == circe::CT_PROJECTED)
        {
            comboboxTargetAngularUnit->setVisible(true);
            CO.targetOptions.formatId += "CS";
        }
/* Deflection cancelled
             CO.targetOptions.formatId += "CSD";
        }
        else if (CO.targetOptions.geodeticCRStype == circe::CT_GEOGRAPHIC)
        {
            comboboxTargetAngularUnit->setVisible(true);
            CO.targetOptions.formatId += "D";
        }
*/
        CO.printCS = true;
    }
    else
    {
        CO.printCS = false;
        if (CO.targetOptions.geodeticCRStype == circe::CT_PROJECTED)
         comboboxTargetAngularUnit->setVisible(false);
    }

    bool oldState = comboboxTargetFormat->blockSignals(true);
    int ind = comboboxTargetFormat->currentIndex();
    if (redrawPt4dLayout)
    setPt4dLayout("target", tabwidgetTargetDataFile, CO.targetOptions.formatId, &lineditTargetI, &lineditTargetX,
                  &lineditTargetY, &lineditTargetZ, &lineditTargetL, &lineditTargetP, &lineditTargetH,
                  &lineditTargetE, &lineditTargetN, &lineditTargetV, &lineditTargetC, &lineditTargetS,
                  &lineditTargetD);
    comboboxTargetFormat->setCurrentIndex(ind);
    comboboxTargetFormat->blockSignals(oldState);
    WriteInfo("Output");
    displayCommandLineWhileEditing();
}

void MainWindow::comboboxTargetAngularUnit_currentIndexChanged(int ind)
{
    if (ind<0) return;
    CO.targetOptions.angularUnit = OS.targetSubset.dataUnitList[ind];
    displayCommandLineWhileEditing();
    displayTargetValidityArea();
}

void MainWindow::verticalAuthoritative_stateChanged(int state)
{
    checkboxSourceVerticalAuthoritative->setChecked(state);
    CO.sourceOptions.verticalFrameAuthoritative = state;
    comboboxSourceVerticalFrame->setEnabled(!state);
    comboboxSourceVerticalFrame->setVisible(!state);
    checkboxTargetVerticalAuthoritative->setChecked(state);
    CO.targetOptions.verticalFrameAuthoritative = state;
    comboboxTargetVerticalFrame->setEnabled(!state);
    comboboxTargetVerticalFrame->setVisible(!state);
    displayCommandLineWhileEditing();
}

void MainWindow::checkboxSourceVerticalAuthoritative_stateChanged(int state)
{
    verticalAuthoritative_stateChanged(state);
    displaySourceValidityArea();
}

void MainWindow::checkboxTargetVerticalAuthoritative_stateChanged(int state)
{
    verticalAuthoritative_stateChanged(state);
    displayTargetValidityArea();
}

void MainWindow::comboboxVerticalTransfo_currentIndexChanged(int ind)
{
    if (ind<0) return;
    CO.idVerticalTransfoRequired = OS.verticalTransfoIdList[ind];
    displayCommandLineWhileEditing();
}

void MainWindow::comboboxGeodeticTransfo_currentIndexChanged(int ind)
{
    if (ind<0) return;
    CO.idGeodeticTransfoRequired = OS.geodeticTransfoIdList[ind];
    displayCommandLineWhileEditing();
}

void MainWindow::lineditSourceI_textChanged(QString s)
{
    CO.Pt4d.id = s.toStdString();
    displayCommandLineWhileEditing();
    for (unsigned int i=0; i<vectorPt4d.size(); i++)
        if (vectorPt4d[i].id == s.toStdString())
        {
            UpdateSourcePointDisplay(&vectorPt4d[i]);
            break;
        }

}

// dropping
void MainWindow::editCoord(QString s)
{
    std::istringstream ss(s.toStdString());
    for (unsigned int ifrmt=0; ifrmt<CO.sourceOptions.formatId.size(); ifrmt++)
    {
        switch (CO.sourceOptions.formatId[ifrmt])
        {
        case 'I':
            break;
        case 'X':
            ss >> CO.Pt4d.x;
            lineditSourceX.setText(circe::displayElement(CO.Pt4d,'X').data());
            break;
        case 'Y':
            ss >> CO.Pt4d.y;
            lineditSourceY.setText(circe::displayElement(CO.Pt4d,'Y').data());
            break;
        case 'Z':
            ss >> CO.Pt4d.z;
            lineditSourceZ.setText(circe::displayElement(CO.Pt4d,'Z').data());
            break;
        case 'L':
            ss >> CO.Pt4d.l;
            lineditSourceL.setText(circe::displayElement(CO.Pt4d,'L').data());
            break;
        case 'P':
            ss >> CO.Pt4d.p;
            lineditSourceP.setText(circe::displayElement(CO.Pt4d,'P').data());
            break;
        case 'H':
            ss >> CO.Pt4d.h;
            lineditSourceH.setText(circe::displayElement(CO.Pt4d,'H').data());
            break;
        case 'E':
            ss >> CO.Pt4d.e;
            lineditSourceE.setText(circe::displayElement(CO.Pt4d,'E').data());
            break;
        case 'N':
            ss >> CO.Pt4d.n;
            lineditSourceN.setText(circe::displayElement(CO.Pt4d,'N').data());
            break;
        case 'V':
            ss >> CO.Pt4d.w;
            lineditSourceV.setText(circe::displayElement(CO.Pt4d,'V').data());
            break;
        default:
            break;
        }
    }

}

void MainWindow::lineditSourceX_textChanged(QString s)
{
    CO.Pt4d.x = s.toDouble();
    if (CO.Pt4d.x == 0. && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceY_textChanged(QString s)
{
    CO.Pt4d.y = s.toDouble();
    if (CO.Pt4d.y == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceZ_textChanged(QString s)
{
    CO.Pt4d.z = s.toDouble();
    if (CO.Pt4d.z == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceE_textChanged(QString s)
{
    CO.Pt4d.e = s.toDouble();
    if (CO.Pt4d.e == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceN_textChanged(QString s)
{
    CO.Pt4d.n = s.toDouble();
    if (CO.Pt4d.n == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceH_textChanged(QString s)
{
//    //Ceci marche pour séparateur virgule
//    bool ok;
//    QLocale c(QLocale::French);
//    //QLocale c(QLocale::C);
//    CO.Pt4d.h = c.toDouble(s, &ok);
    CO.Pt4d.h = s.toDouble();

    if (CO.Pt4d.h == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceL_textChanged(QString s)
{
    if (CO.sourceOptions.angularUnit == "DMS" && s != "")
        CO.Pt4d.l = circe::readDMS(s.toStdString(), "longitude DMS", false);
    else if (CO.sourceOptions.angularUnit == "DM" && s != "")
        CO.Pt4d.l = circe::readDMS(s.toStdString(), "longitude DM", false);
    else
    {
        CO.Pt4d.l = s.toDouble();
        if (CO.Pt4d.l == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    }
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceP_textChanged(QString s)
{
    if (CO.sourceOptions.angularUnit == "DMS" && s != "")
        CO.Pt4d.p = circe::readDMS(s.toStdString(), "latitude DMS", false);
    else if (CO.sourceOptions.angularUnit == "DM" && s != "")
        CO.Pt4d.p = circe::readDMS(s.toStdString(), "latitude DM", false);
    else
    {
        CO.Pt4d.p = s.toDouble();
        if (CO.Pt4d.p == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    }
    displayCommandLineWhileEditing();
}
void MainWindow::lineditSourceV_textChanged(QString s)
{
    CO.Pt4d.w = s.toDouble();
    if (CO.Pt4d.w == 0 && s != "" && atof(s.toStdString().data()) != 0) editCoord(s);
    displayCommandLineWhileEditing();
}

void MainWindow::lineditSourceEpoch_textChanged(QString s)
{
    CO.sourceOptions.epoch = s.toDouble();
    if (GS.getKeyGeodeticFrame() == "")
        lineditTargetEpoch.setText(s);
    displayCommandLineWhileEditing();
}

void MainWindow::lineditTargetEpoch_textChanged(QString s)
{
    CO.targetOptions.epoch = s.toDouble();
    displayCommandLineWhileEditing();
}


//to be triggered before dragging and dropping coordinates
void MainWindow::menuTool_Clear_triggered()
{
    CO.Pt4d.x = CO.Pt4d.y = CO.Pt4d.z = CO.Pt4d.l = CO.Pt4d.p = CO.Pt4d.h = CO.Pt4d.e = CO.Pt4d.n = CO.Pt4d.w = 0;
    CO.Pt4d.id = "";
    lineditSourceDataFile.setText("");
    lineditTargetDataFile.setText("");
    lineditSourceI.setText("");
    lineditSourceX.setText("");
    lineditSourceY.setText("");
    lineditSourceZ.setText("");
    lineditSourceL.setText("");
    lineditSourceP.setText("");
    lineditSourceH.setText("");
    lineditSourceE.setText("");
    lineditSourceN.setText("");
    lineditSourceV.setText("");
    lineditSourceC.setText("");
    lineditSourceS.setText("");
    lineditSourceD.setText("");
//    lineditSourceEpoch.setText("");
    lineditTargetI.setText("");
    lineditTargetX.setText("");
    lineditTargetY.setText("");
    lineditTargetZ.setText("");
    lineditTargetL.setText("");
    lineditTargetP.setText("");
    lineditTargetH.setText("");
    lineditTargetE.setText("");
    lineditTargetN.setText("");
    lineditTargetV.setText("");
    lineditTargetC.setText("");
    lineditTargetS.setText("");
    lineditTargetD.setText("");
//    lineditTargetEpoch.setText("");
}

void MainWindow::menuTool_Swap_triggered()
{
    try
    {
        if (CO.dataMode ==circe::DM_POINT)
        {
            circe::pt4d p = circe::process_pt4d(circe::writeCommandLine(CO), &GS);
            circe::copyPt4d(&(CO.Pt4d), p);
        }
        circe::swapGeodeticOptions(&(CO.sourceOptions), &(CO.targetOptions));
        circe::writeXMLOptionFile(CO);
        InitWidgetValues(CO.optionFile);
        bUpdateCRSid = true;
        buttonProcess_clicked();
    }
    catch (std::string str)
    {
        labelMessage.setText(str.data());
        labelMessage.setStyleSheet("QLabel { background-color : red; color : black; font: bold 12px;}");
        lineditTargetY.setText("0");
        lineditTargetZ.setText("0");
        lineditTargetX.setText("0");
        lineditTargetL.setText("0");
        lineditTargetP.setText("0");
        lineditTargetH.setText("0");
        lineditTargetE.setText("0");
        lineditTargetN.setText("0");
        lineditTargetV.setText("0");
        lineditTargetC.setText("0");
        lineditTargetS.setText("0");
        lineditTargetD.setText("0");
    }
}

void MainWindow::menuTool_CopyCL_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(circe::writeCommandLine(CO).data());
}

void MainWindow::menuTool_CopyTargetPoint_triggered()
{
    std::string curr_frmt = CO.targetOptions.formatId;
    QClipboard *clipboard = QApplication::clipboard();
    try
    {
        std::ostringstream ss;
        circe::initBasicOstream(&ss);
        unsigned int idt;
        if (circe::hasVertical(curr_frmt))
            CO.targetOptions.formatId = "XYZLPHENV";
        else
            CO.targetOptions.formatId = "XYZLPHEN";
        circe::pt4d p = circe::process_pt4d(circe::writeCommandLine(CO), &GS);
        p.transfoIndex = 0; //don't write transformation precision information
        circe::writeBasicLine(&ss, CO, p, &idt, CO.targetOptions.formatId);
        CO.targetOptions.formatId = curr_frmt;
        clipboard->setText(ss.str().data());
    }
    catch (std::string str)
    {
        CO.targetOptions.formatId = curr_frmt;
        clipboard->setText(str.data());
    }
}

void MainWindow::menuDisplay_Layout_triggered()
{
    if (CO.appStyle == "Horizontal")
        CO.appStyle = "Vertical";
    else
        CO.appStyle = "Horizontal";
    displayMessage(RESTART_TEXT,CO.appTitle.data(),CO.imageFilename);
}

void MainWindow::menuDisplay_Bounding_triggered()
{
    CO.displayBoundingBox = !CO.displayBoundingBox;
    displaySourceValidityArea();
    displayTargetValidityArea();
}

void MainWindow::menuDisplay_CS_triggered()
{
    CO.printCS = !CO.printCS;
    checkboxPrintCS_stateChanged(CO.printCS);
}

void MainWindow::menuDisplay_DMS_triggered()
{
    CO.plainDMS = !CO.plainDMS;
    displayCommandLineWhileEditing();
}

void MainWindow::menuTool_Authoritative_triggered()
{
    CO.noUseVerticalFrameAuthoritative = !CO.noUseVerticalFrameAuthoritative;
    checkboxSourceVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));
    checkboxTargetVerticalAuthoritative->setVisible(!(CO.noUseVerticalFrameAuthoritative));
}

void MainWindow::menuDisplay_scrollArea_triggered()
{
    CO.scrollArea = !CO.scrollArea;
    displayMessage(RESTART_TEXT,CO.appTitle.data(),CO.imageFilename);
}

void MainWindow::menuHelp_About_triggered()
{
    std::string str="";
    str += CO.appTitle+"\n"+"\n";
    str += CO.appVersion+"\n"+"\n";
    str += CO.appDescription+"\n"+"\n";
    str += CO.appCopyright+"\n";
    str += "circelib version " + std::string(CL_VERSION) +"\n";
    str += "Interface version " + std::string(CG_VERSION) +"\n";
    displayMessage(str,CO.appTitle.data(),CO.imageFilename);
}

void MainWindow::menuHelp_Doc_triggered()
{
    QString helpFile = CO.appHelp.data();
    struct stat buffer;
    if (stat (helpFile.toStdString().c_str(), &buffer) != 0)
    {
        displayMessage(std::string(helpFile.toStdString()+circe::ErrorMessageText[circe::EM_NOT_FOUND]).data());
        return;
    }
// Cancelled: there are no more non-ASCII characters in pathes (territory name)
// #ifdef UNIX
//     //xchm (if used) cannot read a file whose path contains non-ASCII characters (territory name)
//     //=>copied and read in .. (e.g. /usr/share/circe/data)
//     std::string systcmd = "cp "+CO.appHelp+" ..";
//     int ret = system(systcmd.data());
//     if (ret != 0)
//         displayMessage(std::string("sh: cp "+CO.appHelp).data());
//     helpFile = circeDataDir+CO.appHelp.data();
// //    systcmd = "xchm ../"+CO.appHelp;
// //    system(systcmd.data());
// //    if (ret != 0)
// //        displayMessage(std::string("sh: xchm ../"+circe::ErrorMessageText[circe::EM_NOT_FOUND]).data());
// //    qDebug("%s returned %d",systcmd.data(),ret);
// #endif

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(helpFile)))
    {
        #ifdef UNIX
        std::string systcmd = "xchm "+CO.appHelp;
        int ret = system(systcmd.data());
        if (ret != 0)
            displayMessage(std::string("sh: xchm ../"+circe::ErrorMessageText[circe::EM_NOT_FOUND]).data());
//        qDebug("%s returned %d",systcmd.data(),ret);
        #endif
        displayMessage(circe::ErrorMessageText[circe::EM_INSTALL_CHM_VIEWER].data());
    }
}

//The user manual may be found in the Application folder (C:\Program Files (x86)\IGN\Circe....)
//or in the application workspace (start in ...) (circe5/data/CirceXXX or circe5/data).
void MainWindow::menuHelp_User_triggered()
{
    QString userManualFile = QCoreApplication::applicationDirPath()+"/"+CO.appUserManual.data();
    struct stat buffer;
    if (stat (userManualFile.toStdString().c_str(), &buffer) != 0)
    {
        userManualFile = circeDataDir+CO.appUserManual.data();
        if (stat (userManualFile.toStdString().c_str(), &buffer) != 0)
        {
            userManualFile = circeDataDir+"../"+CO.appUserManual.data();
            if (stat (userManualFile.toStdString().c_str(), &buffer) != 0)
            {
                displayMessage(std::string(userManualFile.toStdString()+circe::ErrorMessageText[circe::EM_NOT_FOUND]).data());
                return;
            }
        }
    }
    /*
#ifdef UNIX
    std::string systcmd = "evince "+userManualFile.toStdString();
    int ret = system(systcmd.data());
    if (ret != 0)
        displayMessage(std::string("sh: evince"+circe::ErrorMessageText[circe::EM_NOT_FOUND]).data());
    qDebug("%s returned %d",systcmd.data(),ret);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(userManualFile));
#endif
*/
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(userManualFile)))
        displayMessage(circe::ErrorMessageText[circe::EM_INSTALL_PDF_VIEWER].data());
}

void MainWindow::WriteInfo(QString where)
{
    std::string info = circe::InterfaceElementText[circe::IE_COORD_TYPE];
    if (where == "Input")
    {
        if (CO.sourceOptions.geodeticCRStype == circe::CT_GEOGRAPHIC)
            info += ", "+circe::InterfaceElementText[circe::IE_UNIT];
        if (OS.sourceSubset.geodeticDimension == circe::SD_4D)
            info += ", "+circe::InterfaceElementText[circe::IE_EPOCH];
        labelSourceDataInfo.setText(info.data());
    }
    else if (where == "Output")
    {
        if (CO.targetOptions.geodeticCRStype == circe::CT_GEOGRAPHIC)
            info += ", "+circe::InterfaceElementText[circe::IE_UNIT];
        if (CO.targetOptions.geodeticCRStype == circe::CT_PROJECTED && CO.printCS)
            info += ", "+circe::InterfaceElementText[circe::IE_CONV_UNIT];
        if (OS.targetSubset.geodeticDimension == circe::SD_4D)
            info += ", "+circe::InterfaceElementText[circe::IE_EPOCH];
        labelTargetDataInfo.setText(info.data());
    }
}

void MainWindow::comboboxSeparator_currentTextChanged(QString s)
{
    CO.separator = s.toStdString();
    displayCommandLineWhileEditing();
}
