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
* \file messageBearer.h
* \brief Definition of the MessageBearer class.
*/

#ifndef MESSAGEBEARER_H
#define MESSAGEBEARER_H

namespace circe
{

/**
* \brief This class manages MessageBearer.
*/

class MessageBearer
{
private:
    std::string currentMessage;
public:
    virtual void messageEdit(std::string message) = 0;
    void setCurrentMessage(std::string message) {currentMessage=message;}
    std::string getCurrentMessage() {return currentMessage;}
};

} //namespace circe

#endif // MESSAGEBEARER_H
