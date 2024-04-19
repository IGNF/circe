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
* \file operation.cpp
* \brief Implementation of the Operation class.
*/

#include "operation.h"

namespace circe
{

/**
* \brief Constructor.
*/
Operation::Operation()
{
}

/**
* \brief Copy constructor.
*/
Operation::Operation(const Operation &B) : CirceBoundedObject(B) 
{
    Status = B.Status;
    OperationApplication = B.OperationApplication;
    Reversibility = B.Reversibility;
    SourceCRSid = B.SourceCRSid;
    TargetCRSid = B.TargetCRSid;
    InitParamMethodId = B.InitParamMethodId;
    ApplicationMethodId = B.ApplicationMethodId;
    ApplicationInvMethodId = B.ApplicationInvMethodId;
}

/**
* \brief Empties values of the members of an Operation instance.
*/
void Operation::reset()
{
    CirceBoundedObject::reset();
    Status = S_UNDEF;
    OperationApplication = Reversibility = OA_UNDEF;
    SourceCRSid = TargetCRSid = InitParamMethodId = ApplicationMethodId = ApplicationInvMethodId = "";
}

/**
* \brief Summarizes the reversibility status in a boolean.
* \return true if the instance is reversible.
*/
bool Operation::IsReversible()
{
    if (getOperationReversibility()==OA_REVERSE_SAME_METHOD
     || getOperationReversibility()==OA_REVERSE_SAME_PARAMETERS)
        return true;
    else
        return false;
}

/**
* \brief Initializes STATUS Status.
* \param[in] _value: A std::string that should fit with a StatusText value.
*/
void Operation::setStatus(std::string _value)
{
    for(int i = 0; i<S_COUNT; i++) if (_value == StatusText[i])
    {
        Status = (STATUS)i;
        break;
    }

}

/**
* \brief Initializes OPERATION_APPLICATION Reversibility.
* \param[in] _value: A std::string that should fit with a OperationApplicationText value.
*/
void Operation::setReversibility(std::string _value)
{
    for(int i = 0; i<OA_COUNT; i++) if (_value == OperationApplicationText[i])
    {
        Reversibility = (OPERATION_APPLICATION)i;
        break;
    }

}

/**
* \brief Initializes OPERATION_APPLICATION OperationApplication.
* \param[in] _value: A std::string that should fit with a OperationApplicationText value.
*/
void Operation::setOperationApplication(std::string _value)
{
    for(int i = 0; i<OA_COUNT; i++) if (_value == OperationApplicationText[i])
    {
        OperationApplication = (OPERATION_APPLICATION)i;
        break;
    }

}

} //namespace circe
