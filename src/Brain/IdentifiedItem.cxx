
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __IDENTIFIED_ITEM_DECLARE__
#include "IdentifiedItem.h"
#undef __IDENTIFIED_ITEM_DECLARE__

#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItem
 * \brief Describes an identified item.
 */


/**
 * Constructor.
 *
 */
IdentifiedItem::IdentifiedItem()
: CaretObject()
{
    initializeMembers();
}

/**
 * Constructor.
 *
 * @param text
 *    Text describing the identified item.
 */
IdentifiedItem::IdentifiedItem(const AString& text)
: CaretObject()
{
    initializeMembers();
    m_text = text;
}

/**
 * Destructor.
 */
IdentifiedItem::~IdentifiedItem()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItem::IdentifiedItem(const IdentifiedItem& obj)
: CaretObject(obj), SceneableInterface()
{
    this->initializeMembers();
    
    this->copyHelperIdentifiedItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItem&
IdentifiedItem::operator=(const IdentifiedItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperIdentifiedItem(obj);
    }
    return *this;    
}

/**
 * Initialize a new instance of this class.
 */
void
IdentifiedItem::initializeMembers()
{
    m_text.clear();
    m_showIdentificationSymbol = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_text", &m_text);
    m_sceneAssistant->add("m_showIdentificationSymbol",
                          &m_showIdentificationSymbol);
}
                          

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItem::copyHelperIdentifiedItem(const IdentifiedItem& obj)
{
    m_text = obj.m_text;
    m_showIdentificationSymbol = obj.m_showIdentificationSymbol;
}

/**
 * @return Is this item valid?  Typically only used when restoring
 * from scene.
 */
bool
IdentifiedItem::isValid() const
{
    if (m_text.isEmpty() == false) {
        return true;
    }
    return false;
}

/**
 * Append text to this item's text.
 */
void
IdentifiedItem::appendText(const AString& text)
{
    m_text += text;
}

/**
 * Clear the text for this item.
 */
void
IdentifiedItem::clearText()
{
    m_text = "";
}

/**
 * @return The text describing the identified item.
 */
AString
IdentifiedItem::getText() const
{
    return m_text;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentifiedItem::toString() const
{
    return ("m_text=" + m_text);
}

/**
 * @return Is the identification symbol displayed for this item?
 */
bool
IdentifiedItem::isShowIdentificationSymbol() const
{
    return m_showIdentificationSymbol;
}

/**
 * Set show identification symbol for this item.
 * By default the symbol is on but sub-classes
 * can call this method to change the status.
 *
 * @param showSymbol
 *     New status for showing identification symbol.
 */
void
IdentifiedItem::setShowIdentificationSymbol(const bool showSymbol)
{
    m_showIdentificationSymbol = showSymbol;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
IdentifiedItem::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItem",
                                            1);
    
    saveMembers(sceneAttributes, sceneClass);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
IdentifiedItem::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    restoreMembers(sceneAttributes, sceneClass);
}

/**
 * Restore members (protected function for derived classes).
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.
 */
void
IdentifiedItem::restoreMembers(const SceneAttributes* sceneAttributes,
                    const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Save members (protected function for derived classes).
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.
 */
void
IdentifiedItem::saveMembers(const SceneAttributes* sceneAttributes,
                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes, sceneClass);
}
