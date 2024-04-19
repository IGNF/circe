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
* \file StringResourcesFR.h
* \brief Definition of string resources in french.
*/

#ifndef STRINGRESOURCESFR_H
#define STRINGRESOURCESFR_H
#include "StringResources.h"
namespace circe
{

#define OPTION_FILE_PREFIX "circe_options"
#define OPTION_FILE_PREFIX_W L"circe_options"

#define HELP_HEADER L"\
Options de Circe\n\
****************\n\
\n\
Une ligne de commande de Circe comporte une option ou plus.\n\
Une option suit une des syntaxes suivantes :\n\
	- un nom d'option long;\n\
	- un nom d'option long suivi de '=' avec un argument sans espace;\n\
	- un nom d'option court;\n\
	- un nom d'option court avec un argument avec ou sans espace.\n\
RECOMMANDATION: Utilisez les noms d'option longs pour conserver votre ligne de commande sur le long terme.\n\
\n\
nom court  nom long                 arg?  description\n\
---------  --------                 ----  -----------";

#define HELP_SYNTAX L"\
Syntaxe\n\
-------\n\
Afficher l'aide\n\
circe --help\n\
\n\
Afficher la liste des SRC disponibles\n\
circe --metadataFile=metadataFilePathName --getCRSlist\n\
\n\
Afficher la zone de validite d'un SRC\n\
circe --metadataFile=metadataFilePathName --getValidityArea --sourceCRS=SourceGeodeticCRSid.SourceVerticalCRSid [--sourceFormat=..sourceAngularUnit]\n\
\n\
Lancer les tests\n\
circe --runTest --sourcePathname=testFilePathName\n\
\n\
Faire une operation sur un point\n\
circe --metadataFile=metadataFilePathName --sourceCRS=SourceGeodeticCRSid.SourceVerticalCRSid --sourceFormat=sourceFormatId.sourceLinearUnit.sourceAngularUnit --targetCRS=TargetGeodeticCRSid.TargetVerticalCRSid --targetFormat=targetFormatId.targetLinearUnit.targetAngularUnit coord1 coord2 [coord3]\n\
\n\
Faire une operation sur un fichier\n\
circe --metadataFile = metadataFilePathName --sourceCRS = SourceGeodeticCRSid.SourceVerticalCRSid --sourceFormat = sourceFormatId.sourceLinearUnit.sourceAngularUnit --targetCRS = TargetGeodeticCRSid.TargetVerticalCRSid --targetFormat = targetFormatId.targetLinearUnit.targetAngularUnit --sourcePathname = sourcePathname --targetPathname = targetPathname\n\
\n\
Exemple\n\
-------\n\
Le fichier source datafile.txt contient des points exprimes dans le SRC projete de code NTFLAMB3 (NTF Lambert 3). Leur format est IEN (id est nord). \
Ils seront transformes dans le SRC geographique de code WGS84G dans le format ILP (id longitude latitude) et ecrits dans le fichier outfile.txt. \
Les codes et autres metadonnees sont definis dans le fichier IGNF.xml. \
La ligne de commande est :\n\
circe --sourceCRS=NTFLAMB3. --sourceFormat=IEN.METERS. --targetCRS=WGS84G. --targetFormat=ILP.METERS.DEGREES --metadataFile=data/IGNF.xml --sourcePathname=./datafile.txt --targetPathname=./outfile.txt\n\
"

#define PROCESS_TEXT "Calculer"
#define RESTART_TEXT "Redémarrez Circe pour prendre en compte la modification."

static const std::string ErrorMessageText[EM_COUNT] =
{
    "",
    "non initialisé",
    "Impossible d'ouvrir ",
    " imprévu",
    " introuvable",
    " invalide",
    " incohérent",
    "mauvais numéro de version",
    "Pas de couche",
    "L'extension du fichier cible ne correspond pas à la liste des pilotes",
    "Echec de la création du fichier cible",
    "Echec de la création de la couche",
    "Echec de la création du champ Name",
    "Echec de la création d'un élément dans la couche",
    "Installez un visionneur de PDF",
    "Installez un visionneur de CHM"
};

static const std::string OperationMessageText[OM_COUNT] =
{
    "",
    "SRC géodésique source introuvable.",
    "SRC vertical source introuvable.",
    "SRC géodésique cible introuvable.",
    "SRC vertical cible introuvable.",
    "Le point est en-dehors du SRC géodésique source",
    "Le point est en-dehors du SRC géodésique cible dans le repère géodésique source",
    "Le point est en-dehors du SRC géodésique cible dans le repère géodésique cible",
    "Une transformation est demandée mais pas trouvée",
    "Transformations verticales ambiguës avec les deux repères géodésiques source et cible (peut-être faut-il supprimer l'information sur les altitudes)",
    "Les repères sources ne sont pas initialisés",
    "Les repères cibles ne sont pas initialisés",
    "Transformation géodésique introuvable pour ces SRC",
    "Transformation géodésique introuvable pour ce point",
    "Transformation verticale introuvable pour ces SRC",
    "Transformation verticale introuvable pour ce point",
    "L'interpolation a échoué",
    "Le type du SRC source et l'unité de mesure sont incompatibles.",
    "Le point est en-dehors du domaine de définition du SRC.",
    "L'époque source n'est pas valide ou le repère source n'est pas 4D.",
    "L'époque cible n'est pas valide ou le repère cible n'est pas 4D.",
    "Points traités avec succès ",
    "L'époque source a été modifiée à sa valeur prédéfinie.",
    "L'époque cible a été modifiée à sa valeur prédéfinie."
};

static const std::string GridFormatInfoText[GF_COUNT] =
{
    "UNDEFINED",
    "Grilles ascii distribuées avec Circé. Leur dimension n'est pas prédéterminée. En pratique, il y a des grilles 1D (conversion verticale) et 3D (changement de système géodésique).",
    "Grilles binaires distribuées avec Circé (Version binaire des .tac).",
    "Grilles ascii 1D Gravsoft dont l'en-tête comprend dans cet ordre : latitude min, latitude max, longitude min, longitude max, pas en latitude, pas en longitude.",
    "Grilles ascii 1D spécifiques au logiciel Surfer.",
    "Grille NTV2 binaire little endian (pour processeurs à architecture petit-boutiste comme les x86 des PC). Les grilles NTV2 sont des grilles 2D de changement de système géodésique. Elles ont 4 valeurs à chaque node : les translations en latitude et longitude suivies de leurs précisions.",
    "Grille NTV2 binaire big endian (pour processeurs à architecture gros-boutiste). Voir NTV2 binaire little endian.",
    "Grille NTV2 ascii pour contrôle des NTV2 binaires.",
    "Grille ESRI dont l'en-tête comprend 6 rubriques : ncols, nrows, xllcorner, yllcorner, cellsize, NODATA_value.",
    "La grille de géoïde EGM2008 est publiée par le USNGA avec des logiciels de lecture/écriture de sous-grilles comme gridget_1min. Les sous-grilles doivent être renommées en .egm pour être prises en compte par Cooriste.",
    "Grille DIS : un format de distribution de la BDAlti.",
    "Format des grilles de conversion altimétrique implémenté dans PROJ.4.",
    "Format des grilles de conversion altimétrique implémenté dans PROJ.4."
};

static const std::string ArgumentValidationText[AV_COUNT] =
{
    "",
    "Erreur inconnue",
	"Fichier de métadonnées mal ou non initialisé.",
    "Fichier de limites mal ou non initialisé.",
	"Argument non valide",
    "Option ou argument non valide",
    "Nom d'option long non valide",
    "Option non valide sans argument",
    "Nom d'option court non valide",
    "L'id du SRC vertical devrait être défini.",
    "L'id du SRC géodésique devrait être défini.",
    "Format géodésique invalide (doit contenir XYZ/EN/NE/LP/PL).",
    "L'extension du fichier de métadonnées n'est pas valide (doit être txt ou xml).",
    "Coordonnées en entrée invalides.",
    "Valeur DMS invalide",
    "Valeur DM invalide"
};

static const std::wstring OptionsHelpText[OH_COUNT] =
{
   L"",
   L"Affiche cette aide.",
   L"Give a short usage message",
   L"Argument : Chemin complet du fichier de métadonnées.",
   L"Argument : Precision d'affichage des coordonnées en metres",
   L"Argument : id des reperes sources au format GeodeticFrameid.VerticalFrameid",
   L"Argument : id des reperes cibles au format GeodeticFrameid.VerticalFrameid",
   L"Argument : id des SRC sources au format GeodeticCRSid.VerticalCRSid",
   L"Argument : id des SRC cibles au format GeodeticCRSid.VerticalCRSid",
   L"Argument : id des transfos requises au format idGeodeticTransfo.idVerticalTransfo",
   L"Suivi des valeurs des transformations",
   L"Argument : format et unite source au format FormatId.LinearUnit.AngularUnit",
   L"Argument : format et unite cible au format FormatId.LinearUnit.AngularUnit",
   L"Argument : Epoque source en annee decimale",
   L"Argument : Epoque cible en annee decimale",
   L"Argument : Chemin complet du fichier source",
   L"Argument : Chemin complet du fichier cible",
   L"Lancer les tests",
   L"Traiter les SRC multiples",
   L"Silencieux",
   L"Verbeux",
   L"Traiter en tant que vecteur (pour test seulement)",
   L"Charger en tant que vecteur",
   L"Donner la liste des SRC",
   L"Donner la liste des zones",
   L"Donner le domaine de definition d'un SRC dans une certaine unite (degres par defaut)",
   L"Donner l'autorite d'un SRC",
   L"Argument : code pour le chargement des grilles",
   L"Argument : Chemin complet du fichier log",
   L"Argument : Format de sortie (format par defaut ou JSON)",
   L"Argument : Fichier de limites sans chemin",
   L"Obtenir une liste coherente d'unites au format id:text,id:text,",
   L"Obtenir une liste coherente de formats de fichiers au format id:text,id:text,",
   L"Obtenir une liste coherente de formats de points au format id:text,id:text,",
   L"Obtenir une liste coherente de transformations geodesiques au format id:text,id:text,",
   L"Obtenir une liste coherente de transformations verticales au format id:text,id:text,",
   L"Obtenir une liste coherente de reperes geodesiques sources au format id:text,id:text,",
   L"Obtenir une liste coherente de reperes geodesiques cibles au format id:text,id:text,",
   L"Obtenir une liste coherente de SRC geodesiques sources au format id:text,id:text,",
   L"Obtenir une liste coherente de SRC geodesiques cibles au format id:text,id:text,",
   L"Obtenir une liste coherente de SRC verticaux sources au format id:text,id:text,",
   L"Obtenir une liste coherente de SRC verticaux cibles au format id:text,id:text,",
   L"Ecrire les DMS en clair, pas sous forme decimale",
   L"Ne pas afficher le facteur d'échelle en PPM",
   L"Séparateur de champs"
};

// for user-friendly HMI purposes, all and only angular units between MU_UNDEF and MU_METER
static const std::string MeasureUnitDisplayText[MU_COUNT] =
{
    "",
    "Radians",
    "Secondes",
    "Degrés",
    "DMS",
    "DM",
    "Grades",
    "Mètres",
    "Pouces",
    "ppm",
    "°/Ma",
	"mas/an"
};

static const std::string CRStypeDisplayText[CT_COUNT] =
{
    "",
    "Géocentriques",
    "Géographiques",
    "Projetées",
    "Verticales"
};

static const std::string DimensionText[SD_COUNT] =
{
    "UNDEFINED",
    "1D",
    "2D",
    "3D",
    "4D"
};

static const std::string DataValidationText[DV_COUNT] =
{
    "",
    "mauvais format",
    "Coordonnées invalides",
    "Impossible d'ouvrir le fichier en sortie.",
    "fin du fichier",
    "commentaire",
    "erreur au chargement",
    "balise non trouvée"
};

//for user-friendly HMI only (unused in API or command-line mode)
static const std::string uiFormatText[UF_COUNT] =
{
    "",
    "XYZ",
    "Id XYZ",
    "",
    "Longitude Latitude",
    "Longitude Latitude Altitude",
    "Id Longitude Latitude",
    "Id Longitude Latitude Altitude",
    "",
    "Latitude Longitude",
    "Latitude Longitude Altitude",
    "Id Latitude Longitude",
    "Id Latitude Longitude Altitude",
    "",
    "Longitude Latitude Hauteur",
    "Id Longitude Latitude Hauteur",
    "",
    "Latitude Longitude Hauteur",
    "Id Latitude Longitude Hauteur",
    "",
    "Est Nord",
    "Est Nord Altitude",
    "Id Est Nord",
    "Id Est Nord Altitude",
    "",
    "Nord Est",
    "Nord Est Altitude",
    "Id Nord Est",
    "Id Nord Est Altitude",
    "",
    "Est Nord Hauteur",
    "Id Est Nord Hauteur",
    "",
    "Nord Est Hauteur",
    "Id Nord Est Hauteur",
    "",
#ifdef WITH_GDAL
    "shapefile",
#endif
    "NMEA points",
    "KML points",
    "GPX points"
};

static const std::string InterfaceElementText[IE_END] =
{
    "",
    "SRC géodésique",
    "SRC vertical (format avec altitude)",
    "Données",
    "Format",
    "Transformation géodésique",
    "Transformation verticale",
    "Fichier",
    "Point",
    "Affichage",
    "Aide",
    "Documentation",
    "A propos",
    "Effacer les coordonnées",
    "Disposition horizontale / verticale",
    "Afficher les emprises géographiques",
    "Époque des coordonnées",
    "Afficher plus de résultats",
    "Référentiels verticaux par défaut",
    "Fichier de données",
    "Sélectionner",
    "Convergence du méridien",
    "Altération linéaire (ppm)",
    "Module linéaire",
    "Dév. de la verticale",
    "Unité de la convergence",
    "Unité",
    "Type de coordonnées",
    "Entrée",
    "Sortie",
    "DMS en clair",
    "Pleine valeur du facteur d'échelle",
    "Zone de défilement",
    "Copier les options de la ligne de commande",
    "Zone",
    "Outils",
    "Intervertir source/cible",
    "Utiliser les référentiels verticaux par défaut",
    "Manuel utilisateur",
    "Copier point cible",
    "Séparateur de champ de fichier"
};

static const std::string FieldSeparatorText[FS_END] =
{
    "Espace",
    "Virgule",
    "Point-virgule",
    "Tabulation"
};

static const std::string DataModeText[DM_COUNT] =
{
    "",
    "fichier",
    "point",
    "liste"
};

static const std::string MultipleCRSprocessingText[MP_COUNT] =
{
	"",
	"test",
	"calculation"
};

static const std::string CRSdescriptionElements[CD_COUNT] =
{
	"",
	"CODE",
	"SOURCE",
	"CIBLE",
	"REPERE GÉODÉSIQUE",
	"REPERE VERTICAL",
	"COORDONNÉES",
	"ELLIPSOÏDE",
	"PROJECTION",
    "UNITÉ",
    "GRILLE VERTICALE",
    "ÉPOQUE"
};

}
#endif // STRINGRESOURCESFR_H
