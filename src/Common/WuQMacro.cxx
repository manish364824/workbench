
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_DECLARE__
#include "WuQMacro.h"
#undef __WU_Q_MACRO_DECLARE__

#include <QUuid>

#include "CaretAssert.h"
#include "WuQMacroCommand.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroStandardItemTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::WuQMacro 
 * \brief Contains a sequence of WuQMacroCommand's
 * \ingroup WuQMacro
 *
 * Subclasses QStandardItem so that instance can be in a tree widget
 */

/**
 * Constructor.
 */
WuQMacro::WuQMacro()
: QStandardItem(),
TracksModificationInterface()
{
    setFlags(Qt::ItemIsEnabled
             | Qt::ItemIsSelectable);
    m_uniqueIdentifier = QUuid::createUuid().toString();
}

/**
 * Destructor.
 */
WuQMacro::~WuQMacro()
{
    clearCommands();
}

/**
 * Clear (remove) all commands in this macro
 */
void
WuQMacro::clearCommands()
{
    const int32_t numItems = rowCount();
    removeRows(0, numItems);
    CaretAssert(rowCount() == 0);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacro::WuQMacro(const WuQMacro& obj)
: QStandardItem(obj),
TracksModificationInterface()
{
    setFlags(Qt::ItemIsEnabled
             | Qt::ItemIsSelectable);
    m_uniqueIdentifier = QUuid::createUuid().toString();
    
    this->copyHelperWuQMacro(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
WuQMacro&
WuQMacro::operator=(const WuQMacro& obj)
{
    if (this != &obj) {
        QStandardItem::operator=(obj);
        this->copyHelperWuQMacro(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
WuQMacro::copyHelperWuQMacro(const WuQMacro& obj)
{
    /* Note: unique identifier is NOT copied */
    
    clearCommands();
    
    const int32_t numRows = obj.rowCount();
    for (int32_t i = 0; i < numRows; i++) {
        const QStandardItem* item = obj.child(i);
        CaretAssert(item);
        const WuQMacroCommand* command = dynamic_cast<const WuQMacroCommand*>(item);
        CaretAssert(command);
        WuQMacroCommand* commandCopy = new WuQMacroCommand(*command);
        CaretAssert(commandCopy);
        appendMacroCommand(commandCopy);
    }

    setText(obj.text());
    m_description = obj.m_description;
    m_shortCutKey = obj.m_shortCutKey;
}

/**
 * @return The type of this item for Qt
 */
int
WuQMacro::type() const
{
    /*
     * This must be different than the type returned by of macro
     * subclasses of QStandardItem
     */
    return WuQMacroStandardItemTypeEnum::toIntegerCode(WuQMacroStandardItemTypeEnum::MACRO);
}

/**
 * Append a macro command to this macro
 *
 * @param macroCommand
 *     The macro command.
 */
void
WuQMacro::appendMacroCommand(WuQMacroCommand* macroCommand)
{
    CaretAssert(macroCommand);
    const int32_t numCommands = getNumberOfMacroCommands();
    if (numCommands > 0) {
        WuQMacroCommand* lastCommand = getMacroCommandAtIndex(numCommands - 1);
        /*
         * 'Compact' macro commands for mouse events
         */
        if (lastCommand->isMouseEventMatch(macroCommand)) {
            WuQMacroMouseEventInfo* lastMouseInfo = lastCommand->getMouseEventInfo();
            CaretAssert(lastMouseInfo);
            
            const WuQMacroMouseEventInfo* mouseInfo = macroCommand->getMouseEventInfo();
            CaretAssert(mouseInfo);
            const int32_t numXY = mouseInfo->getNumberOfLocalXY();
            for (int32_t i = 0; i < numXY; i++) {
                lastMouseInfo->addLocalXY(mouseInfo->getLocalX(i),
                                          mouseInfo->getLocalY(i));
            }
            
            /*
             * No longer needed since mouse x/y appended to last command
             */
            delete macroCommand;
            macroCommand = NULL;
        }
    }
    
    if (macroCommand != NULL) {
        appendRow(macroCommand);
    }
    
    setModified();
}

/**
 * Insert the given macro command at the given index
 *
 * @param index
 *     Index of where to insert the macro command
 * @param macroCommand
 *     Macro command to insert.
 */
void
WuQMacro::insertMacroCommandAtIndex(const int32_t index,
                                    WuQMacroCommand* macroCommand)
{
    CaretAssert((index >= 0)
                && (index <= getNumberOfMacroCommands()));
    CaretAssert(macroCommand);
    
    insertRow(index,
              macroCommand);
    setModified();
}

/**
 * @return The unique identifier
 */
QString
WuQMacro::getUniqueIdentifier() const
{
    return m_uniqueIdentifier;
}

/**
 * Set unique identifier of macro
 *
 * @param uniqueIdentifier
 *    New unique identifier
 */void
WuQMacro::setUniqueIdentifier(const QString& uniqueIdentifier)
{
    if (uniqueIdentifier.isEmpty()) {
        return;
    }
    if (m_uniqueIdentifier != uniqueIdentifier) {
        m_uniqueIdentifier = uniqueIdentifier;
        setModified();
    }
}

/**
 * @return Name of macro
 */
QString
WuQMacro::getName() const
{
    return text();
}

/**
 * Set name of macro
 *
 * @param name
 *    New name
 */
void
WuQMacro::setName(const QString& name)
{
    if (name != text()) {
        setText(name);
        setModified();
    }
}

/**
 * @return Description of macro
 */
QString
WuQMacro::getDescription() const
{
    return m_description;
}

/**
 * Set description of macro
 *
 * @param description
 *    New description
 */
void
WuQMacro::setDescription(const QString& description)
{
    if (m_description != description) {
        m_description = description;
        setModified();
    }
}

/**
 * @return The short cut key
 */
WuQMacroShortCutKeyEnum::Enum
WuQMacro::getShortCutKey() const
{
    return m_shortCutKey;
}

/**
 * Set the short cut key
 *
 * @param shortCutKey
 *    New short cut key
 */
void
WuQMacro::setShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey)
{
    if (m_shortCutKey != shortCutKey) {
        m_shortCutKey = shortCutKey;
        setModified();
    }
}

/**
 * @return The number of macro commands in this macro
 */
int32_t
WuQMacro::getNumberOfMacroCommands() const
{
    return rowCount();
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
const WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index) const
{
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* item = child(index);
    CaretAssert(item);
    WuQMacroCommand* command = dynamic_cast<WuQMacroCommand*>(item);
    CaretAssert(command);
    return command;
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index)
{
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* item = child(index);
    CaretAssert(item);
    WuQMacroCommand* command = dynamic_cast<WuQMacroCommand*>(item);
    CaretAssert(command);
    return command;
}

/**
 * @return
 *     Index of the given macro command
 * @param macroCommand
 *     The macro command
 */
int32_t
WuQMacro::getIndexOfMacroCommand(const WuQMacroCommand* macroCommand) const
{
    CaretAssert(macroCommand);
    
    const int32_t num = getNumberOfMacroCommands();
    for (int32_t i = 0; i < num; i++) {
        if (getMacroCommandAtIndex(i) == macroCommand) {
            return i;
            break;
        }
    }
    return -1;
}

/**
 * Delete macro command at the given index
 *
 * @param index
 *    Index of macro command
 */
void
WuQMacro::deleteMacroCommandAtIndex(const int32_t index)
{
    CaretAssert((index >= 0)
                && (index < getNumberOfMacroCommands()));
    removeRow(index);
    setModified();
}

/**
 * Delete the given macro command
 
 * @param macroCommand
 */
void
WuQMacro::deleteMacroCommand(WuQMacroCommand* macroCommand)
{
    const int32_t index = getIndexOfMacroCommand(macroCommand);
    if (index >= 0) {
        deleteMacroCommandAtIndex(index);
    }
}

/**
 * Move the given macro command down one position
 *
 * @param macroCommand
 */
void
WuQMacro::moveMacroCommandDown(WuQMacroCommand* macroCommand)
{
    const int32_t index = getIndexOfMacroCommand(macroCommand);
    if ((index >= 0)
        && (index < getNumberOfMacroCommands() - 1)) {
        /* Note that take child removes item but does not remove row */
        QStandardItem* item = takeChild(index);
        removeRow(index);
        CaretAssert(item);
        insertMacroCommandAtIndex(index + 1,
                                  dynamic_cast<WuQMacroCommand*>(item));
    }
}

/**
 * Move the given macro command up one position
 *
 * @param macroCommand
 */
void
WuQMacro::moveMacroCommandUp(WuQMacroCommand* macroCommand)
{
    const int32_t index = getIndexOfMacroCommand(macroCommand);
    if ((index > 0)
        && (index < getNumberOfMacroCommands())) {
        /* Note that take child removes item but does not remove row */
        QStandardItem* item = takeChild(index);
        removeRow(index);
        CaretAssert(item);
        insertMacroCommandAtIndex(index - 1,
                                  dynamic_cast<WuQMacroCommand*>(item));
    }
}

/**
 * @return True if this instance is modified
 */
bool
WuQMacro::isModified() const
{
    if (m_modifiedStatusFlag) {
        return true;
    }
    
    const int32_t numItems = getNumberOfMacroCommands();
    for (int32_t i = 0; i < numItems; i++) {
        if (getMacroCommandAtIndex(i)->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status
 */
void
WuQMacro::clearModified()
{
    m_modifiedStatusFlag = false;
    
    const int32_t numItems = getNumberOfMacroCommands();
    for (int32_t i = 0; i < numItems; i++) {
        getMacroCommandAtIndex(i)->clearModified();
    }
}

/**
 * Set the modification status to modified
 */
void
WuQMacro::setModified()
{
    m_modifiedStatusFlag = true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacro::toString() const
{
    QString s("WuQMacro\n");
    
    s.append("Name=" + text() + "\n");
    s.append("Description=" + m_description + "\n");
    s.append("ShortCutKey=" + WuQMacroShortCutKeyEnum::toGuiName(m_shortCutKey) + "\n");
    s.append("UUID=" + m_uniqueIdentifier + "\n");
    
    const int32_t numItems = getNumberOfMacroCommands();
    for (int32_t i = 0; i < numItems; i++) {
        s.append(getMacroCommandAtIndex(i)->toString());
    }
    
    return s;
}

