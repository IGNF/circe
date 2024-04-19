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
* \file operation.h
* \brief Definition of the Operation class.
*/

#ifndef OPERATION_H
#define OPERATION_H

//#include "circe.h"
#include "circeboundedobject.h"

namespace circe
{

/**
* \brief This class manages operation data (that Transformation and Conversion share).
*/
class Operation: public CirceBoundedObject
{
private:
    STATUS Status;
    OPERATION_APPLICATION OperationApplication;
    OPERATION_APPLICATION Reversibility;
    std::string SourceCRSid;
    std::string TargetCRSid;
    std::string InitParamMethodId;
    std::string ApplicationMethodId;
    std::string ApplicationInvMethodId;
public:
    Operation();
    Operation(const Operation &B) ; // copy constructor
    void reset();
    OPERATION_APPLICATION getOperationApplication() const {return OperationApplication;}
    OPERATION_APPLICATION getOperationReversibility() const {return Reversibility;}
    std::string getInitParamMethodId() {return InitParamMethodId;}
    STATUS getStatus() const {return Status;}
    void setStatus(std::string _value);
    void setReversibility(std::string _value);
    void setOperationApplication(std::string _value);
    void setOperationApplication(OPERATION_APPLICATION _value) {OperationApplication=_value;}
    bool IsReversible();
};

} //namespace circe

#endif // OPERATION_H
