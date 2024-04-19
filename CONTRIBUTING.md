## WHEN YOU'RE SIGNED OUT ...

First of all, you can send an email to _geodesie(AT)ign.fr_ for any topic 
related to geodesy, Coordinate Reference Systems and Circe software operation.

Questions regarding Circe source code must be sent to _circe(AT)ign.fr_:

* Bug reporting. Please include the following information if relevant:
>* Sample data to reproduce the issue.
>* Screenshot.
>* Your operating system and version.
>* The exact version of Circé. Did this work in a previous version ?


* Suggesting a new functionality. 

New functionalities will be considered as long 
as they respect the Circe basics described in [README](https://devsgn.ign.fr/circeV5/circe_public/blob/master/README.md).


## WHEN YOU'RE SIGNED IN ...

When you get [there](https://devsgn.ign.fr/users/sign_in) and choose the 
*Register* tab, you can create an account and log in.

Please send a mail to _circe(AT)ign.fr_ if you face a problem in creating your 
account or in using this Gitlab project.

When you're signed in, you can **create issues** and **fork the master branch**.

The preferred way of contribution to the source code development is as follows:

* Fork the project.
* Clone the forked project to your local system.
* Create a branch in which to place your changes.
* Make your changes to the new branch.
* Commit the changes to the branch.
* Submit them through a merge request.

Source code enhancement and new functionalities will be considered as long 
as they respect the Circe basics described in [README](https://devsgn.ign.fr/circeV5/circe_public/blob/master/README.md).


## CODE ENHANCEMENT SUGGESTION

Circe is not a GIS but should process more complex data formats like shapefiles 
for the user's convenience. Their implentation should use existing libraries 
as gdal; Circe includes rough demonstration functions using an old version of gdal (v1).

You must uncomment the following lines in the .pro files to build Circe against gdal:

```
#win32:DEFINES += WITH_COMPLEX_FORMATS
#unix:DEFINES += WITH_COMPLEX_FORMATS
#win32:CONFIG += WITH_COMPLEX_FORMATS
#unix:CONFIG += WITH_COMPLEX_FORMATS
```

Circe then needs a gdal library and header (.h) files; search the *.pro* files 
for INCLUDEPATH and LIBS and have them consistent with your gdal v1 installation; 
or have your gdal installation consistent with them.


