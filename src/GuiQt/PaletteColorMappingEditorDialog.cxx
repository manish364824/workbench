
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

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#define __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__
#include "PaletteColorMappingEditorDialog.h"
#undef __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "MapSettingsPaletteColorMappingWidget.h"

using namespace caret;


    
/**
 * \class caret::PaletteColorMappingEditorDialog 
 * \brief Dialog for editing color palettes
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
PaletteColorMappingEditorDialog::PaletteColorMappingEditorDialog(QWidget* parent)
: WuQDialogNonModal("Palette Color Mapping Editor",
                    parent)
{
    QLabel* fileLabel = new QLabel("Map File: ");
    m_fileNameValueLabel = new QLabel("");
    
    QLabel* mapLabel = new QLabel("Map Name: ");
    m_mapNameValueLabel = new QLabel("");
    
    m_paletteColorMappingEditor = new MapSettingsPaletteColorMappingWidget(this);
    
    QGridLayout* namesLayout = new QGridLayout();
    namesLayout->setColumnStretch(0, 0);
    namesLayout->setColumnStretch(1, 100);
    int namesRow = 0;
    namesLayout->addWidget(fileLabel, namesRow, 0);
    namesLayout->addWidget(m_fileNameValueLabel, namesRow, 1, Qt::AlignLeft);
    namesRow++;
    namesLayout->addWidget(mapLabel, namesRow, 0);
    namesLayout->addWidget(m_mapNameValueLabel, namesRow, 1, Qt::AlignLeft);
    
    /*
     * For now, hide map name labels
     */
    mapLabel->hide();
    m_mapNameValueLabel->hide();
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addLayout(namesLayout);
    layout->addWidget(m_paletteColorMappingEditor);

    setCentralWidget(w,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    disableAutoDefaultForAllPushButtons();
    
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_DELETE);
}

/**
 * Destructor.
 */
PaletteColorMappingEditorDialog::~PaletteColorMappingEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
PaletteColorMappingEditorDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_DELETE) {
        updateDialog();
    }
}

/**
 * May be called to update the dialog's content.
 *
 * @param mapFile
 *    Map file whose map is having its palette color mapping edited
 * @param mapIndex
 *    Index of the map whose color mapping is being edited.
 */
void
PaletteColorMappingEditorDialog::updateDialogContent(CaretMappableDataFile* mapFile,
                                                     const int32_t mapIndex)
{
    m_mapFile = mapFile;
    m_mapIndex = mapIndex;
    
    bool enableEditor = false;
    if (m_mapFile != NULL) {
        if ((m_mapIndex >= 0)
            && (m_mapIndex < m_mapFile->getNumberOfMaps())) {
            enableEditor = true;
        }
    }
    
    if (enableEditor) {
        m_fileNameValueLabel->setText(m_mapFile->getFileNameNoPath());
        m_mapNameValueLabel->setText(m_mapFile->getMapName(m_mapIndex));
        m_paletteColorMappingEditor->setEnabled(true);
        m_paletteColorMappingEditor->updateEditor(m_mapFile,
                                                  m_mapIndex);
    }
    else {
        m_fileNameValueLabel->setText("");
        m_mapNameValueLabel->setText("");
        m_paletteColorMappingEditor->setEnabled(false);
    }
}

/**
 * May be called to update the dialog.
 */
void
PaletteColorMappingEditorDialog::updateDialog()
{
    if (m_mapFile != NULL) {
        /*
         * Validate map file to prevent crash
         */
        EventCaretMappableDataFilesGet getMapFilesEvent;
        EventManager::get()->sendEvent(getMapFilesEvent.getPointer());
        
        std::vector<CaretMappableDataFile*> allMapFiles;
        getMapFilesEvent.getAllFiles(allMapFiles);
        
        if (std::find(allMapFiles.begin(),
                      allMapFiles.end(),
                      m_mapFile) != allMapFiles.end()) {
            if (m_mapIndex < 0) {
                m_mapIndex = 0;
            }
            if (m_mapFile->getNumberOfMaps() > 0) {
                if (m_mapIndex >= m_mapFile->getNumberOfMaps()) {
                    m_mapIndex = m_mapFile->getNumberOfMaps() - 1;
                }
            }
            else {
                m_mapFile = NULL;
                m_mapIndex = -1;
            }
        }
        else {
            m_mapFile  = NULL;
            m_mapIndex = -1;
        }
        
        updateDialogContent(m_mapFile,
                            m_mapIndex);
    }
    
    if (m_mapFile == NULL) {
        close();
    }
}

