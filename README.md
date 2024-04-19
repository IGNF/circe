# IGNF / Circé 5

## PURPOSE OF THE SOFTWARE

Circé is a computer program whose purpose is the transformation of 4D+1 point 
coordinates between Coordinate Reference Systems.

**WHAT CIRCE IS**

Here are the basics of Circe software.

Circe operates conversions and transformations on single point coordinates, 
possibly read in files.

Circe deals with 4D+1 Coordinate Reference Systems, including:

* *2D geodetic CRS* : LP or EN and so-called reprojections.

* *3D geodetic CRS* : ENH, LPH, XYZ and 3-D transformations.

* *4D geodetic CRS* : 3D coordinates at epochs, and velocity models.

* *vertical CRS* : MSL heights and grids of geoid.

Circe has a rough looking Graphic User Interface, but is user-friendly as it 
helps the user choosing meaningful operations on Coordinate Reference Systems.

It can also be run in command-line mode, possibly with the help of the GUI.

XML and easy to handle text data files are used to initialize geodetic data. 
These files are built on a geographical area consistency basis.


## PURPOSE OF THE GIT PROJECT

This Git project mainly aims at providing the free library *circelib* (standard C ++) 
to any developer that needs functions to operate on Coordinate Reference Systems. 
Contributions are also appreciated; [please read here how to contribute or 
send questions to Circé team.](https://devsgn.ign.fr/circeV5/circe_public/blob/master/CONTRIBUTING.md)

For convenience for the developer, it also provides the source code of the 
graphical and command line interfaces (GUI & CLI) corresponding to the 
versions of Circé 5.4 distributed on the IGN website:
https://geodesie.ign.fr/index.php?page=circe.

Finally, the *assets* directory contains data for the eight French public 
service geographic areas, possibly not the most recent.

All this allows the developer to compile and run Circe immediately.

The **most recent public service data**, as well as the **user manual** and 
**geodetic documentations by area**, can be retrieved by installing Circe 5.4 or later version 
under Linux or under Windows ; the setups are available at the url mentioned above.

## LANGUAGE AND COMPILATION

The free library *circelib* and the command line interface are in standard C ++.

The GUI is in Qt C ++, developed and compiled under Qt Creator 4.11 based on Qt 5.14.

The *qt_cmd* and *qt_gui* directories contain the Qt project files (*.pro*) which allow you to open, compile and run the projects.

## EXECUTION

The files of a typical Circé installation are divided into two directories, the directory of executable files and that of data files by territory (*assets*).

To start the GUI from Qt Creator, the working directory (not containing spaces or special characters) must be that of the data files by territory; it may be one of the sub-directories, containing the data of a single territory, and in this case the XML file of options (circe_options_XXX .xml) must be passed as an argument.

For the command line, the name of the geodetic metadata file (of a single territory) is passed as an option with a full path, so there is no need to set a particular working directory.

## INCLUDING CIRCE IN YOUR PROCESS

Circe can be used at several levels :

* Use the Graphic User Interface for a full interactive use; Please note that 
this a good introduction to the CLI mode as the command-line options are 
written at the bottom of the GUI while they are input.

* Use the Command-line Interface in a command shell or in a script.

* Include the standard C++ source code files, possibly as an external library, 
in your development project. 
Please note the preferred way of parsing and validating the options 
(see *MainWindow::buttonProcess_clicked()* in the GUI project) :

```
circe::process(circe::writeCommandLine(CO), &GS, &s);
```
The *writeCommandLine* function is used to display the command-line options at 
the bottom of the GUI; you can compose your CL options the way you want and 
insert it as *process* function argument.

## CODE DOCUMENTATION

Please open the following page as a web page to get access to the software 
documentation:

<a href="https://devsgn.ign.fr/circeV5/circe_public/blob/master/html/index.html" target="_blank">index.html</a>

