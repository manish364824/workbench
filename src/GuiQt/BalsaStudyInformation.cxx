
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __BALSA_STUDY_INFORMATION_DECLARE__
#include "BalsaStudyInformation.h"
#undef __BALSA_STUDY_INFORMATION_DECLARE__

#include <QJsonObject>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BalsaStudyInformation 
 * \brief Information about a BALSA Study
 * \ingroup GuiQt
 */

/**
 * Constructor with invalid ID and title.
 */
BalsaStudyInformation::BalsaStudyInformation()
: CaretObject()
{
    
}

/**
 * Constructor with ID and title.
 *
 * @param studyID
 *     The Study Identifier
 * @param studyTitle
 *     The Study Title
 */
BalsaStudyInformation::BalsaStudyInformation(const AString& studyID,
                                             const AString& studyTitle)
: CaretObject(),
m_studyID(studyID),
m_studyTitle(studyTitle),
m_editableStatus(false)
{
}

/**
 * Constructor that creates BALSA study information from a JSON object.
 *
 * @param jsonObject
 *     The JSON object.
 */
BalsaStudyInformation::BalsaStudyInformation(const QJsonObject& jsonObject)
{
    QJsonObject::const_iterator idIter = jsonObject.find("id");
    QJsonObject::const_iterator titleIter = jsonObject.find("title");
    if ((idIter != jsonObject.end())
        && (titleIter != jsonObject.end())) {
        m_studyID    = (*idIter).toString();
        m_studyTitle = (*titleIter).toString();

        QJsonObject::const_iterator statusIter = jsonObject.find("status");
        if (statusIter != jsonObject.end()) {
            QJsonObject statusObject = (*statusIter).toObject();
            QJsonObject::const_iterator nameIter = statusObject.find("name");
            if (nameIter != jsonObject.end()) {
                AString editText = (*nameIter).toString().toUpper().trimmed();
                m_editableStatus = (editText == "EDITABLE");
            }
        }
    }
}

/**
 * Destructor.
 */
BalsaStudyInformation::~BalsaStudyInformation()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BalsaStudyInformation::BalsaStudyInformation(const BalsaStudyInformation& obj)
: CaretObject(obj)
{
    this->copyHelperBalsaStudyInformation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BalsaStudyInformation&
BalsaStudyInformation::operator=(const BalsaStudyInformation& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBalsaStudyInformation(obj);
    }
    return *this;    
}

/**
 * Comparison operator using study title.
 *
 * @param obj
 *     Other study information.
 * @return
 *     True if "this" is less than obj.
 */
bool
BalsaStudyInformation::operator<(const BalsaStudyInformation& obj) const
{
    return m_studyTitle < obj.m_studyTitle;
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BalsaStudyInformation::copyHelperBalsaStudyInformation(const BalsaStudyInformation& obj)
{
    m_studyID    = obj.m_studyID;
    m_studyTitle = obj.m_studyTitle;
    m_editableStatus = obj.m_editableStatus;
}

/**
 * @return True if ID is not empty (title may be empty).
 */
bool
BalsaStudyInformation::isEmpty() const
{
    return m_studyID.isEmpty();
}

/**
 * @return study identifier generated by BALSA
 */
AString
BalsaStudyInformation::getStudyID() const
{
    return m_studyID;
}

/**
 * Set study identifier generated by BALSA
 * @param studyID
 *    New value for study identifier generated by BALSA
 */
void
BalsaStudyInformation::setStudyID(const AString& studyID)
{
    m_studyID = studyID;
}

/**
 * @return study title
 */
AString
BalsaStudyInformation::getStudyTitle() const
{
    return m_studyTitle;
}

/**
 * Set study title
 * @param studyTitle
 *    New value for study title
 */
void
BalsaStudyInformation::setStudyTitle(const AString& studyTitle)
{
    m_studyTitle = studyTitle;
}

/**
 * @return True is the study is editable.
 */
bool
BalsaStudyInformation::isEditable() const
{
    return m_editableStatus;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BalsaStudyInformation::toString() const
{
    return "BalsaStudyInformation";
}

