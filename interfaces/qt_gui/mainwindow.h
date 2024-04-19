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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QSplitter>
#include <QDateTime>
#include <QWidgetAction>
#include "_circe_gui_version_.h"
#include "messageherald.h"
#include "../../circelib/_circelib_version_.h"
#include "../../circelib/geodeticset.h"
#include "../../circelib/parser.h"
#ifdef UI_LANGUAGE_ENGLISH
#include "../../circelib/StringResourcesEN.h"
#elif defined(UI_LANGUAGE_FRENCH)
#include "../../circelib/StringResourcesFR.h"
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::string circeOptionFile, QWidget *parent = 0);
    ~MainWindow();

    circe::GeodeticSet GS;
    circe::circeOptions CO;
    circe::operationSubset OS;
    std::vector<circe::pt4d> vectorPt4d;
    QString circeDataDir;
    bool bUpdateCRSid;
    bool dspSwap;
    circe::MessageHerald mh;

    QPushButton *buttonProcess;
    QLabel *labelLogo;
    QMenu *menuZone, *menuDisplay, *menuTool, *menuHelp, *menuSeparator;
    QAction *menuDisplay_Layout, *menuDisplay_Bounding, *menuDisplay_CS, *menuDisplay_DMS, *menuDisplay_scrollArea;
    QAction *menuTool_CopyCL, *menuTool_CopyRes, *menuTool_Clear, *menuTool_Swap, *menuTool_Authoritative;
    QAction *menuHelp_Doc, *menuHelp_User, *menuHelp_About;
    QWidgetAction *menuTool_Separator;
    QComboBox *comboboxSeparator;
    QGroupBox *groupboxSourceGeodeticCRS;
    QGroupBox *groupboxTargetGeodeticCRS;
    QGroupBox *groupboxSourceData;
    QGroupBox *groupboxTargetData;
    QGroupBox *groupboxSourceVerticalCRS;
    QGroupBox *groupboxTargetVerticalCRS;
    QGroupBox *groupboxGeodeticTransfo;
    QGroupBox *groupboxVerticalTransfo;
    QCheckBox *checkboxSourceVerticalAuthoritative;
    QCheckBox *checkboxTargetVerticalAuthoritative;
    QPushButton *selectBtnSourceDataFile;
    QPushButton *selectBtnTargetDataFile;
    QComboBox *comboboxSourceGeodeticFrame;
    QComboBox *comboboxSourceGeodeticCRS;
    QComboBox *comboboxTargetGeodeticFrame;
    QComboBox *comboboxTargetGeodeticCRS;
    QComboBox *comboboxSourceVerticalFrame;
    QComboBox *comboboxTargetVerticalFrame;
    QComboBox *comboboxGeodeticTransfo;
    QComboBox *comboboxVerticalTransfo;
    QComboBox *comboboxSourceAngularUnit;
    QComboBox *comboboxSourceFormat;
    QComboBox *comboboxTargetAngularUnit;
    QComboBox *comboboxTargetFormat;
    QTabWidget *tabwidgetSourceDataFile;
    QTabWidget *tabwidgetTargetDataFile;
    QLineEdit lineditSourceDataFile;
    QLineEdit lineditTargetDataFile;
    QLineEdit lineditSourceI;
    QLineEdit lineditSourceX;
    QLineEdit lineditSourceY;
    QLineEdit lineditSourceZ;
    QLineEdit lineditSourceL;
    QLineEdit lineditSourceP;
    QLineEdit lineditSourceH;
    QLineEdit lineditSourceE;
    QLineEdit lineditSourceN;
    QLineEdit lineditSourceV;
    QLineEdit lineditSourceC;
    QLineEdit lineditSourceS;
    QLineEdit lineditSourceD;
    QLineEdit lineditTargetI;
    QLineEdit lineditTargetX;
    QLineEdit lineditTargetY;
    QLineEdit lineditTargetZ;
    QLineEdit lineditTargetL;
    QLineEdit lineditTargetP;
    QLineEdit lineditTargetH;
    QLineEdit lineditTargetE;
    QLineEdit lineditTargetN;
    QLineEdit lineditTargetV;
    QLineEdit lineditTargetC;
    QLineEdit lineditTargetS;
    QLineEdit lineditTargetD;
    QLineEdit lineditSourceEpoch;
    QLineEdit lineditTargetEpoch;
    QLabel labelMessage;
    QLabel labelSourceGeodeticCRSinfo;
    QLabel labelSourceVerticalCRSinfo;
    QLabel labelTargetGeodeticCRSinfo;
    QLabel labelTargetVerticalCRSinfo;
    QLabel labelSourceDataInfo;
    QLabel labelTargetDataInfo;

    void editCoord(QString s);
    void InitData(std::string circeOptionFile);
    std::string InitZones();
    void InitWidgetValues(std::string circeOptionFile);
    void populateComboBox(QComboBox *_combobox, std::vector<std::string> displayList, int iSel, std::string tmpinfo);
    void populateSourceFormat();
    void populateTargetFormat();
    void displayCommandLineWhileEditing();
    void verticalAuthoritative_stateChanged(int state);
    std::string getValidityAreaCommandLine(std::string angularUnit, std::string CRSid);
    void displaySourceValidityArea();
    void displayTargetValidityArea();
    void WriteInfo(QString where);
    void UpdateSourcePointDisplay(circe::pt4d *point4d);
    void makeFormatConsistent(const std::string& frmtId, int *iFormatList, const std::vector<std::string>& dataFormatList);
    void setPt4dLayout(QString what, QTabWidget *tabwidgeDataFile, const std::string& lnfrmt, QLineEdit *linedit_I,
                       QLineEdit *linedit_X, QLineEdit *linedit_Y, QLineEdit *linedit_Z, QLineEdit *linedit_L,
                       QLineEdit *linedit_P, QLineEdit *linedit_H, QLineEdit *linedit_E, QLineEdit *linedit_N,
                       QLineEdit *linedit_V, QLineEdit *linedit_C, QLineEdit *linedit_S, QLineEdit *linedit_D);

public slots:
    void groupboxSourceGeodeticCRS_toggled(bool b);
    void groupboxTargetGeodeticCRS_toggled(bool b);
    void groupboxSourceVerticalCRS_toggled(bool b);
    void groupboxTargetVerticalCRS_toggled(bool b);
    void selectBtnSourceDataFile_clicked();
    void selectBtnTargetDataFile_clicked();
    void tabwidgetSourceDataFile_currentChanged(int ind);
    void tabwidgetTargetDataFile_currentChanged(int ind);
    void comboboxSourceGeodeticFrame_currentIndexChanged(int ind);
    void comboboxSourceVerticalFrame_currentIndexChanged(int ind);
    void comboboxTargetGeodeticFrame_currentIndexChanged(int ind);
    void comboboxTargetVerticalFrame_currentIndexChanged(int ind);
    void comboboxSourceGeodeticCRS_currentIndexChanged(int ind);
    void comboboxTargetGeodeticCRS_currentIndexChanged(int ind);
    void comboboxSourceFormat_currentIndexChanged(int ind);
    void comboboxTargetFormat_currentIndexChanged(int ind);
    void checkboxSourceVerticalAuthoritative_stateChanged(int state);
    void checkboxTargetVerticalAuthoritative_stateChanged(int state);
    void comboboxSourceAngularUnit_currentIndexChanged(int ind);
    void comboboxTargetAngularUnit_currentIndexChanged(int ind);
    void checkboxPrintCS_stateChanged(int state);
    void comboboxVerticalTransfo_currentIndexChanged(int ind);
    void comboboxGeodeticTransfo_currentIndexChanged(int ind);
    void lineditSourceI_textChanged(QString s);
    void lineditSourceX_textChanged(QString s);
    void lineditSourceY_textChanged(QString s);
    void lineditSourceZ_textChanged(QString s);
    void lineditSourceE_textChanged(QString s);
    void lineditSourceN_textChanged(QString s);
    void lineditSourceH_textChanged(QString s);
    void lineditSourceL_textChanged(QString s);
    void lineditSourceP_textChanged(QString s);
    void lineditSourceV_textChanged(QString s);
    void lineditSourceEpoch_textChanged(QString s);
    void lineditTargetEpoch_textChanged(QString s);
    void lineditSourceDataFile_textChanged(QString s);
    void lineditTargetDataFile_textChanged(QString s);
    void buttonProcess_clicked();
    void menuZone_triggered();
    void menuDisplay_Layout_triggered();
    void menuDisplay_Bounding_triggered();
    void menuDisplay_CS_triggered();
    void menuDisplay_DMS_triggered();
    void menuDisplay_scrollArea_triggered();
    void menuTool_CopyCL_triggered();
    void menuTool_Clear_triggered();
    void menuTool_Swap_triggered();
    void menuTool_CopyTargetPoint_triggered();
    void menuTool_Authoritative_triggered();
    void menuHelp_Doc_triggered();
    void menuHelp_User_triggered();
    void menuHelp_About_triggered();
    void comboboxSeparator_currentTextChanged(QString s);

    void displayMessageFromCircelib(const QString &str);

private:
    Ui::MainWindow *ui;
    bool redrawPt4dLayout;
};

const QDateTime qt0(QDate(2000, 1, 1));

void displayMessage(std::string Message, std::string Caption, std::string imageFilename);
void displayMessage(const char* Message);
void setSelectFileLayout(QGridLayout *gridLayoutData, QString label_text, QPushButton *v_selectBtn,
                         QString selectBtn_text, QLineEdit *v_linedit);
void setComboBoxLayout(QString where, QGroupBox *_groupbox, QComboBox *_combobox, QCheckBox *_checkboxVerticalAuthoritative, QComboBox *_combobox2, QLabel *_labelCRSinfo, QString title, bool checkable);
void setTransfoLayout(QSplitter *splitter, Qt::Orientation orientation,
                     QGroupBox *groupboxGeodeticCRS, QComboBox *comboboxGeodeticFrame,
                     QGroupBox *groupboxVerticalCRS, QComboBox *comboboxVerticalFrame);
void setDataLayout(QString where, QString what, QGroupBox *_groupbox, QTabWidget *_tabwidget, QComboBox *_comboBoxAngularUnit,
                   QComboBox *_comboBoxFormat, QLineEdit *_lineeditEpoch, QPushButton *_selectBtnDataFile,
                   QLineEdit *_lineditSourceDataFile, QCheckBox *_checkboxPrintCS,
                   QGroupBox *groupboxVerticalCRS, QComboBox *comboboxVerticalFrame,
                   QCheckBox *_checkboxVerticalAuthoritative, QLabel *labelVerticalCRSinfo,
                   QGroupBox *groupboxGeodeticCRS, QComboBox *comboboxGeodeticFrame,
                   QComboBox *comboboxGeodeticCRS, QLabel *labelGeodeticCRSinfo,
                   bool checkable, QToolButton *buttonProcess, QLabel *_labelDataInfo,
                   QString logoFilename, QLabel *_labelLogo);

#endif // MAINWINDOW_H
