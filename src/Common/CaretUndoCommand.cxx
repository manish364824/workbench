
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

/*
 * NOTE: The Qt license is included because the CaretUndoStack API
 * including the method comments is copied from QUndoStack.
 */

/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtOpenGL module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#define __CARET_UNDO_COMMAND_DECLARE__
#include "CaretUndoCommand.h"
#undef __CARET_UNDO_COMMAND_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::CaretUndoCommand 
 * \brief Abstract class for a command that supports 'undo' and 'redo'.
 * \ingroup Common
 *
 * Abstract class for commands that allow 'undo' and 'redo'.
 *
 * Design is based off Qt's QUndoCommand.
 *
 * While using QUndoCommand would be preferred, it cannot be used because:
 * (1) It is located in Qt's GUI module which is not accessible to
 * Caret's "lower level" modules.
 * (2) QUndoCommand is targeted to text documents.
 *
 */

/**
 * Constructor.
 */
CaretUndoCommand::CaretUndoCommand()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
CaretUndoCommand::~CaretUndoCommand()
{
}

/**
 * @return Description of command.
 */
AString
CaretUndoCommand::getDescription() const
{
    return m_description;
}

/**
 * Set description of command.
 * @param descripton
 *    New value for description of command.
 */
void
CaretUndoCommand::setDescription(const AString& description)
{
    m_description = description;
}
