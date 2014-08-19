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

#include "EventOverlaySettingsEditorDialogRequest.h"

using namespace caret;

/**
 * \class caret::EventOverlaySettingsEditorDialogRequest
 * \brief Event for creating overlay settings editor dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window.
 * @param overlay
 *    Overlay for map editor.
 * @param mapFile
 *    Caret Mappable Data File.
 * @param mapIndex
 *    Map index in mapFile.
 */
EventOverlaySettingsEditorDialogRequest::EventOverlaySettingsEditorDialogRequest(const int32_t browserWindowIndex,
                                                                         Overlay* overlay,
                                                               CaretMappableDataFile* mapFile,
                                                               const int32_t mapIndex)
: Event(EventTypeEnum::EVENT_OVERLAY_SETTINGS_EDITOR_SHOW)
{
    m_browserWindowIndex = browserWindowIndex;
    m_overlay = overlay;
    m_mapFile = mapFile;
    m_mapIndex = mapIndex;
}

/*
 * Destructor.
 */
EventOverlaySettingsEditorDialogRequest::~EventOverlaySettingsEditorDialogRequest()
{
    
}

