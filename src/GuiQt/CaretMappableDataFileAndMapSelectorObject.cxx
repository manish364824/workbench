
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


#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_DECLARE__
#include "CaretMappableDataFileAndMapSelectorObject.h"
#undef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_DECLARE__

#include <QComboBox>
#include <QSpinBox>

#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "GuiManager.h"
#include "WuQEventBlockingFilter.h"

using namespace caret;



/**
 * \class caret::CaretMappableDataFileAndMapSelectorObject
 * \brief Widgets for selecting a map file and map index.
 * \ingroup GuiQt
 */


/**
 * Constructor that creates a selection object.  User will need to
 * insert a model into an instance created with this constructor.
 * This constructor is used when the selection model is stored
 * in another object.
 *
 * @param options
 *    Options for this instance.
 * @param parent
 *    Parent of this instance.
 */
CaretMappableDataFileAndMapSelectorObject::CaretMappableDataFileAndMapSelectorObject(const Options options,
                                                                                     QObject* parent)
: QObject(parent)
{
    m_model = NULL;
    
    initializeConstruction(options);
    
    m_needToDestroyModelFlag = false;
}

/**
 * Constructor that creates a selection object with a model for the selected
 * data file type.
 *
 * @param dataFileType
 *    Type 
 * @param options
 *    Options for this instance.
 * @param parent
 *    Parent of this instance.
 */
CaretMappableDataFileAndMapSelectorObject::CaretMappableDataFileAndMapSelectorObject(const DataFileTypeEnum::Enum dataFileType,
                                                                                     const Options options,
                                                                                     QObject* parent)
: QObject(parent)
{
    
    m_model = new CaretMappableDataFileAndMapSelectionModel(GuiManager::get()->getBrain(),
                                                            dataFileType);
    
    initializeConstruction(options);
    
    m_needToDestroyModelFlag = true;
}

/**
 * Destructor.
 */
CaretMappableDataFileAndMapSelectorObject::~CaretMappableDataFileAndMapSelectorObject()
{
    if (m_needToDestroyModelFlag) {
        delete m_model;
        m_model = NULL;
    }
}

/**
 * Assist with construction.
 *
 * @param options
 *    Options for this instance.
 */
void
CaretMappableDataFileAndMapSelectorObject::initializeConstruction(const Options options)
{
    m_mapFileComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_mapFileComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(mapFileComboBoxFileSelected(CaretDataFile*)));
    
    m_mapIndexSpinBox = NULL;
    if (options & OPTION_SHOW_MAP_INDEX_SPIN_BOX) {
        m_mapIndexSpinBox = new QSpinBox();
        m_mapIndexSpinBox->setMinimum(1);
        m_mapIndexSpinBox->setSingleStep(1);
        QObject::connect(m_mapIndexSpinBox, SIGNAL(valueChanged(int)),
                         this, SLOT(mapIndexSpinBoxValuesChanged(int)));
    }
    
    m_mapNameComboBox = new QComboBox();
    QObject::connect(m_mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxActivated(int)));
    WuQEventBlockingFilter::blockMouseWheelEventInMacComboBox(m_mapNameComboBox);
}

/**
 * Update the model in this file and map selector.
 *
 * @param model
 *    The model.
 */
void
CaretMappableDataFileAndMapSelectorObject::updateFileAndMapSelector(CaretMappableDataFileAndMapSelectionModel* model)
{
    CaretAssert(model);
    m_model = model;

    m_mapFileComboBox->updateComboBox(model->getCaretDataFileSelectionModel());
    updateContent();
}

/**
 * @return Model in this file and map selector.
 */
CaretMappableDataFileAndMapSelectionModel*
CaretMappableDataFileAndMapSelectorObject::getModel()
{
    return m_model;
}

/**
 * Get the widgets for this file and map selector.
 *
 * @param mapFileComboBox
 *    Combo box for file selection.
 * @param mapIndexSpinBox
 *    Spin box for map index selection.
 * @param mapNameComboBox
 *    Combo box for map name selection.
 */
void
CaretMappableDataFileAndMapSelectorObject::getWidgetsForAddingToLayout(QWidget* &mapFileComboBox,
                                                                       QWidget* &mapIndexSpinBox,
                                                                       QWidget* &mapNameComboBox)
{
    mapFileComboBox = m_mapFileComboBox->getWidget();
    mapIndexSpinBox = m_mapIndexSpinBox;
    mapNameComboBox = m_mapNameComboBox;
}

/**
 * Update the content of this object.
 */
void
CaretMappableDataFileAndMapSelectorObject::updateContent()
{
    bool validFlag = false;
    
    if (m_model != NULL) {
        CaretMappableDataFile* mapFile = m_model->getSelectedFile();
        if (mapFile != NULL) {
            const int32_t numMaps  = mapFile->getNumberOfMaps();
            const int32_t mapIndex = m_model->getSelectedMapIndex();
            if ((mapIndex >= 0)
                && (mapIndex < numMaps)) {
                validFlag = true;
                
                if (m_mapIndexSpinBox != NULL) {
                    m_mapIndexSpinBox->blockSignals(true);
                    m_mapIndexSpinBox->setMaximum(numMaps);
                    m_mapIndexSpinBox->blockSignals(false);
                }
                
                m_mapNameComboBox->clear();
                for (int32_t i = 0; i < numMaps; i++) {
                    m_mapNameComboBox->addItem(mapFile->getMapName(i));
                }

                if (m_mapIndexSpinBox != NULL) {
                    /*
                     * Note: Indices are zero to num-maps minus 1
                     * but show 1 to num-maps
                     */
                    m_mapIndexSpinBox->blockSignals(true);
                    m_mapIndexSpinBox->setValue(mapIndex + 1);
                    m_mapIndexSpinBox->blockSignals(false);
                }
                m_mapNameComboBox->setCurrentIndex(mapIndex);
            }
        }
    }
    
    if (m_mapIndexSpinBox != NULL) {
        m_mapIndexSpinBox->setEnabled(validFlag);
    }
    m_mapNameComboBox->setEnabled(validFlag);
}

/**
 * Gets called when a file is selected by the user.
 *
 * @param caretDataFile
 *    File that is selected.
 */
void
CaretMappableDataFileAndMapSelectorObject::mapFileComboBoxFileSelected(CaretDataFile* /*caretDataFile*/)
{
    updateContent();
    
    emit selectionWasPerformed();
}

/**
 * Gets called when a file is selected by the user.
 *
 * @param mapIndex
 *    Index of the map.
 */
void
CaretMappableDataFileAndMapSelectorObject::mapIndexSpinBoxValuesChanged(int mapIndex)
{
    if (m_model != NULL) {
        /*
         * Note: Indices are zero to num-maps minus 1
         * but show 1 to num-maps
         */
        int32_t zeroToOneMapIndex = mapIndex - 1;
        m_model->setSelectedMapIndex(zeroToOneMapIndex);
        updateContent();
        
        emit selectionWasPerformed();
    }
}

/**
 * Gets called when a file is selected by the user.
 *
 * @param mapIndex
 *    Index of the map.
 */
void
CaretMappableDataFileAndMapSelectorObject::mapNameComboBoxActivated(int mapIndex)
{
    if (m_model != NULL) {
        m_model->setSelectedMapIndex(mapIndex);
        updateContent();
        
        emit selectionWasPerformed();
    }
    
}
