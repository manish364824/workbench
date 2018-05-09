
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

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "OverlayViewController.h"
#undef __OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "AnnotationColorBar.h"
#include "CaretMappableDataFile.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "Overlay.h"
#include "UsernamePasswordWidget.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;
    
/**
 * \class caret::OverlayViewController 
 * \brief View Controller for an overlay.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window in which this view controller resides.
 * @param showTopHorizontalLine
 *    If true, display a horizontal line above the controls.
 * @param parent
 *    The parent widget.
 */
OverlayViewController::OverlayViewController(const Qt::Orientation orientation,
                                             QGridLayout* gridLayout,
                                             const int32_t browserWindowIndex,
                                             const int32_t overlayIndex,
                                             QObject* parent)
: QObject(parent),
  browserWindowIndex(browserWindowIndex),
  m_overlayIndex(overlayIndex)
{
    this->overlay = NULL;
    m_constructionReloadFileAction = NULL;
    
    int minComboBoxWidth = 200;
    int maxComboBoxWidth = 100000; //400;
    if (orientation == Qt::Horizontal) {
        minComboBoxWidth = 50;
        maxComboBoxWidth = 100000;
    }
    const QComboBox::SizeAdjustPolicy comboSizePolicy = QComboBox::AdjustToContentsOnFirstShow; //QComboBox::AdjustToContents;

    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    this->enabledCheckBox = new QCheckBox(checkboxText);
    QObject::connect(this->enabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(enabledCheckBoxClicked(bool)));
    this->enabledCheckBox->setToolTip("Enables display of this overlay");
    
    /*
     * File Selection Check Box
     */
    this->fileComboBox = WuQFactory::newComboBox();
    this->fileComboBox->setMinimumWidth(minComboBoxWidth);
    this->fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    this->fileComboBox->setToolTip("Selects file for this overlay");
    this->fileComboBox->setSizeAdjustPolicy(comboSizePolicy);
    
    /*
     * Map Index Spin Box
     */
    m_mapIndexSpinBox = WuQFactory::newSpinBox();
    QObject::connect(m_mapIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(mapIndexSpinBoxValueChanged(int)));
    m_mapIndexSpinBox->setToolTip("Select map by its index");
    
    /*
     * Map Name Combo Box
     */
    this->mapNameComboBox = WuQFactory::newComboBox();
    this->mapNameComboBox->setMinimumWidth(minComboBoxWidth);
    this->mapNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxSelected(int)));
    this->mapNameComboBox->setToolTip("Select map by its name");
    this->mapNameComboBox->setSizeAdjustPolicy(comboSizePolicy);
    
    /*
     * Opacity double spin box
     */
    this->opacityDoubleSpinBox = WuQFactory::newDoubleSpinBox();
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.10);
    this->opacityDoubleSpinBox->setDecimals(1);
    this->opacityDoubleSpinBox->setFixedWidth(50);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityDoubleSpinBoxValueChanged(double)));
    this->opacityDoubleSpinBox->setToolTip("Opacity (0.0=transparent, 1.0=opaque)");
    
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    this->colorBarAction = WuQtUtilities::createAction("CB", 
                                                       "Display color bar for this overlay", 
                                                       this, 
                                                       this, 
                                                       SLOT(colorBarActionTriggered(bool)));
    this->colorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        this->colorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(this->colorBarAction);
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);

    this->settingsAction = WuQtUtilities::createAction("S",
                                                          "Edit settings for this map and overlay", 
                                                          this, 
                                                          this, 
                                                          SLOT(settingsActionTriggered()));
    if (settingsIconValid) {
        this->settingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(this->settingsAction);
    
    /*
     * Construction Tool Button
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                           constructionIcon);
    this->constructionAction = WuQtUtilities::createAction("M", 
                                                           "Add/Move/Remove Overlays", 
                                                           this);
    if (constructionIconValid) {
        this->constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton);
    this->constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(this->constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    const AString yokeToolTip =
    ("Select a yoking group.\n"
     "\n"
     "When files with more than one map are yoked,\n"
     "the seleted maps are synchronized by map index.\n"
     "\n"
     "If the SAME FILE is in yoked in multiple overlays,\n"
     "the overlay enabled statuses are synchronized.\n");
    
    /*
     * Yoking Group
     */
    m_mapYokingGroupComboBox = new MapYokingGroupComboBox(this);
    m_mapYokingGroupComboBox->getWidget()->setStatusTip("Synchronize enabled status and map indices)");
    m_mapYokingGroupComboBox->getWidget()->setToolTip("Yoke to Overlay Mapped Files");
#ifdef CARET_OS_MACOSX
    m_mapYokingGroupComboBox->getWidget()->setFixedWidth(m_mapYokingGroupComboBox->getWidget()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
    QObject::connect(m_mapYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokingGroupActivated()));

    /*
     * Use layout group so that items can be shown/hidden
     */
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout, this);
    
    if (orientation == Qt::Horizontal) {
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(this->m_mapYokingGroupComboBox->getWidget(),
                                         row, 6,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(m_mapIndexSpinBox,
                                         row, 7);
        this->gridLayoutGroup->addWidget(this->mapNameComboBox,
                                         row, 8);
        
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        QLabel* fileLabel = new QLabel("File");
        QLabel* mapLabel = new QLabel("Map");
        
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(fileLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5, 1, 2);
        
        row++;
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 0,
                                         1, 2,
                                         Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(this->m_mapYokingGroupComboBox->getWidget(),
                                         row, 2,
                                         1, 2);
        this->gridLayoutGroup->addWidget(mapLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(m_mapIndexSpinBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(this->mapNameComboBox,
                                         row, 6);
        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
    }
}

/**
 * Destructor.
 */
OverlayViewController::~OverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
OverlayViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/*
 * If this overlay ins an overlay settings editor, update its content
 */
void
OverlayViewController::updateOverlaySettingsEditor()
{
    if (overlay == NULL) {
        return;
    }

    CaretMappableDataFile* mapFile = NULL;
    int32_t mapIndex = -1;
    overlay->getSelectionData(mapFile,
                              mapIndex);
    
    if ((mapFile != NULL)
        && (mapIndex >= 0)) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_OVERLAY_MAP_CHANGED,
                                                     this->browserWindowIndex,
                                                     this->overlay,
                                                     mapFile,
                                                     mapIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::fileComboBoxSelected(int indx)
{
    if (overlay == NULL) {
        return;
    }
    
    void* pointer = this->fileComboBox->itemData(indx).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, 0);
    
    validateYokingSelection();
    
    //validateYokingSelection(overlay->getYokingGroup());
    // not needed with call to validateYokingSelection: this->updateViewController(this->overlay);
    
    // called inside validateYokingSelection();  this->updateUserInterfaceAndGraphicsWindow();

    updateOverlaySettingsEditor();
    updateViewController(this->overlay);
    
    if (file != NULL) {
        if (file->isVolumeMappable()) {
            /*
             * Need to update slice indices/coords in toolbar.
             */
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(browserWindowIndex).addToolBar().getPointer());
        }
    }
    
    updateGraphicsWindow();
}

/**
 * Called when a selection is made from the map index spin box.
 * @parm indx
 *    Index of selection.
 */
void
OverlayViewController::mapIndexSpinBoxValueChanged(int indx)
{
    if (overlay == NULL)
    {
        //TSC: not sure how to put the displayed integer back to 0 where it starts when opening without data files
        return;
    }
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    /*
     * Overlay indices range [0, N-1] but spin box shows [1, N].
     */
    const int overlayIndex = indx - 1;
    
    overlay->setSelectionData(file, overlayIndex);
    
    const MapYokingGroupEnum::Enum mapYoking = overlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               file,
                                               overlayIndex,
                                               overlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map name combo box.
     */
    mapNameComboBox->blockSignals(true);
    if ((overlayIndex >= 0)
        && (overlayIndex < mapNameComboBox->count())) {        
        mapNameComboBox->setCurrentIndex(overlayIndex);
    }
    mapNameComboBox->blockSignals(false);
    
    this->updateUserInterface();
    this->updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when a selection is made from the map name combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::mapNameComboBoxSelected(int indx)
{
    if (overlay == NULL) {
        return;
    }
    
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    overlay->setSelectionData(file, indx);
    
    const MapYokingGroupEnum::Enum mapYoking = overlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               file,
                                               indx,
                                               overlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map index spin box.
     * Note that the map index spin box ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    m_mapIndexSpinBox->setValue(indx + 1);
    m_mapIndexSpinBox->blockSignals(false);
    
    this->updateUserInterface();
    this->updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when enabled checkbox state is changed
 * @parm checked
 *    Checked status
 */
void 
OverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (overlay == NULL) {
        return;
    }
    overlay->setEnabled(checked);
    
    const MapYokingGroupEnum::Enum mapYoking = overlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        CaretMappableDataFile* myFile = NULL;
        int32_t myIndex = -1;
        this->overlay->getSelectionData(myFile,
                                        myIndex);
        
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               myFile,
                                               myIndex,
                                               overlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    this->updateUserInterface();

    this->updateGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void 
OverlayViewController::colorBarActionTriggered(bool status)
{
    if (overlay == NULL) {
        return;
    }

    this->overlay->getColorBar()->setDisplayed(status);
    
    this->updateGraphicsWindow();
}

/**
 * Called when opacity value is changed.
 * @param value
 *    New value.
 */
void 
OverlayViewController::opacityDoubleSpinBoxValueChanged(double value)
{
    if (overlay == NULL) {
        return;
    }
    
    this->overlay->setOpacity(value);
    
    this->updateGraphicsWindow();
}

/**
 * Validate yoking when there are changes made to the overlay.
 */
void
OverlayViewController::validateYokingSelection()
{
    m_mapYokingGroupComboBox->validateYokingChange(this->overlay);
    updateViewController(this->overlay);
    updateGraphicsWindow();
    //EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the yoking group is changed.
 */
void
OverlayViewController::yokingGroupActivated()
{
    MapYokingGroupEnum::Enum yokingGroup = m_mapYokingGroupComboBox->getMapYokingGroup();
   
    /*
     * Has yoking group changed?
     * TSC: overlay can be null when opened without loaded files
     */
    if (overlay != NULL && yokingGroup != overlay->getMapYokingGroup()) {
        validateYokingSelection();
    }
}


/**
 * Called when the settings action is selected.
 */
void 
OverlayViewController::settingsActionTriggered()
{
    if (overlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    this->overlay->getSelectionData(mapFile, 
                                    mapIndex);
    if (mapFile != NULL) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
                                                     this->browserWindowIndex,
                                                     this->overlay,
                                                     mapFile,
                                                     mapIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void 
OverlayViewController::updateViewController(Overlay* overlay)
{
    this->overlay = overlay;

    this->fileComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    //AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    if (this->overlay != NULL) {
        this->overlay->getSelectionData(dataFiles, 
                                  selectedFile, 
                                  //selectedMapUniqueID,
                                  selectedMapIndex);
    }
    
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(dataFiles,
                                                                            displayNames);
    CaretAssert(dataFiles.size() == displayNames.size());

    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CaretMappableDataFile* dataFile = dataFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toOverlayTypeName(dataFile->getDataFileType());
        CaretAssertVectorIndex(displayNames, i);
        this->fileComboBox->addItem(displayNames[i],
                                    qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the map selection combo box
     */
    int32_t numberOfMaps = 0;
    this->mapNameComboBox->blockSignals(true);
    this->mapNameComboBox->clear();
    if (selectedFile != NULL) {
        numberOfMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numberOfMaps; i++) {
            this->mapNameComboBox->addItem(selectedFile->getMapName(i));
        }
        this->mapNameComboBox->setCurrentIndex(selectedMapIndex);
    }
    this->mapNameComboBox->blockSignals(false);
    
    /*
     * Load the map index spin box that ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    m_mapIndexSpinBox->setRange(1, numberOfMaps);
    if (selectedFile != NULL) {
        m_mapIndexSpinBox->setValue(selectedMapIndex + 1);
    }
    m_mapIndexSpinBox->blockSignals(false);

    /*
     * Update enable check box
     */
    Qt::CheckState checkState = Qt::Unchecked;
    if (this->overlay != NULL) {
        if (this->overlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    this->enabledCheckBox->setCheckState(checkState);
    
    m_mapYokingGroupComboBox->setMapYokingGroup(overlay->getMapYokingGroup());
    
    this->colorBarAction->blockSignals(true);
    this->colorBarAction->setChecked(overlay->getColorBar()->isDisplayed());
    this->colorBarAction->blockSignals(false);
    
    this->opacityDoubleSpinBox->blockSignals(true);
    this->opacityDoubleSpinBox->setValue(overlay->getOpacity());
    this->opacityDoubleSpinBox->blockSignals(false);

    const bool haveFile = (selectedFile != NULL);
    bool haveMultipleMaps = false;
    bool dataIsMappedWithPalette = false;
    bool dataIsMappedWithLabelTable = false;
    bool dataIsMappedWithRGBA = false;
    bool haveOpacity = false;
    if (haveFile) {
        dataIsMappedWithPalette = selectedFile->isMappedWithPalette();
        dataIsMappedWithLabelTable = selectedFile->isMappedWithLabelTable();
        dataIsMappedWithRGBA    = selectedFile->isMappedWithRGBA();
        haveMultipleMaps = (selectedFile->getNumberOfMaps() > 1);
        haveOpacity = (dataIsMappedWithLabelTable
                       || dataIsMappedWithPalette
                       || dataIsMappedWithRGBA);
    }
    
    /**
     * Yoking is enabled when either:
     * (1) The file maps to both surface and volumes
     * (2) The file has multiple maps.
     */
    bool haveYoking = false;
    if (haveFile) {
        if (selectedFile->isSurfaceMappable()
            && selectedFile->isVolumeMappable()) {
            haveYoking = true;
        }
        if (haveMultipleMaps) {
            haveYoking = true;
        }
    }
    
    /*
     * Update tooltips with full path to file and name of map
     * as names may be too long to fit into combo boxes
     */
    AString fileComboBoxToolTip("Select file for this overlay");
    AString nameComboBoxToolTip("Select map by its name");
    if (selectedFile != NULL) {
        FileInformation fileInfo(selectedFile->getFileName());
        fileComboBoxToolTip.append(":\n"
                                   + fileInfo.getFileName()
                                   + "\n"
                                   + fileInfo.getPathName()
                                   + "\n\n"
                                   + "Copy File Name/Path to Clipboard with Construction Menu");
        
        nameComboBoxToolTip.append(":\n"
                                   + this->mapNameComboBox->currentText());
    }
    this->fileComboBox->setToolTip(fileComboBoxToolTip);
    this->mapNameComboBox->setToolTip(nameComboBoxToolTip);

    /*
     * Make sure items are enabled at the appropriate time
     */
    this->fileComboBox->setEnabled(haveFile);
    this->mapNameComboBox->setEnabled(haveFile);
    this->m_mapIndexSpinBox->setEnabled(haveMultipleMaps);
    this->enabledCheckBox->setEnabled(haveFile);
    this->constructionAction->setEnabled(true);
    this->opacityDoubleSpinBox->setEnabled(haveOpacity);
    this->m_mapYokingGroupComboBox->getWidget()->setEnabled(haveYoking);
    this->colorBarAction->setEnabled(dataIsMappedWithPalette);
    this->settingsAction->setEnabled(true);
}

/**
 * Update graphics and GUI after selections made
 */
void 
OverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateUserInterface();
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
OverlayViewController::updateUserInterface()
{
    if (this->overlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    }
}

/**
 * Update graphics after selections made
 */
void
OverlayViewController::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    if (this->overlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
    }
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 */
QMenu* 
OverlayViewController::createConstructionMenu(QWidget* parent)
{
    QMenu* menu = new QMenu(parent);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(menuConstructionAboutToShow()));
    
    menu->addAction("Add Overlay Above", 
                    this, 
                    SLOT(menuAddOverlayAboveTriggered()));
    
    menu->addAction("Add Overlay Below", 
                    this, 
                    SLOT(menuAddOverlayBelowTriggered()));
    
    menu->addSeparator();
    
    menu->addAction("Move Overlay Up", 
                    this, 
                    SLOT(menuMoveOverlayUpTriggered()));
    
    menu->addAction("Move Overlay Down", 
                    this, 
                    SLOT(menuMoveOverlayDownTriggered()));
    
    menu->addSeparator();
    
    menu->addAction("Remove This Overlay", 
                    this, 
                    SLOT(menuRemoveOverlayTriggered()));
    
    menu->addSeparator();
    
    m_constructionReloadFileAction = menu->addAction("Reload Selected File",
                                                     this,
                                                     SLOT(menuReloadFileTriggered()));
    
    menu->addSeparator();
    
    m_copyPathAndFileNameToClipboardAction = menu->addAction("Copy Path and File Name to Clipboard",
                                                             this,
                                                             SLOT(menuCopyFileNameToClipBoard()));
    
    menu->addAction("Copy Map Name to Clipboard",
                    this,
                    SLOT(menuCopyMapNameToClipBoard()));
    
    return menu;
    
}

/**
 * Called when construction menu is about to be displayed.
 */
void
OverlayViewController::menuConstructionAboutToShow()
{
    if (this->overlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        this->overlay->getSelectionData(caretDataFile,
                                        mapIndex);
    
        QString menuText = "Reload Selected File";
        if (caretDataFile != NULL) {
            if (caretDataFile->isModified()) {
                QString suffix = " (MODIFIED)";
                if (caretDataFile->isModifiedPaletteColorMapping()) {
                    if ( ! caretDataFile->isModifiedExcludingPaletteColorMapping()) {
                        suffix = " (MODIFIED PALETTE)";
                    }
                }
                menuText += suffix;
            }
            
            const bool notDynConnFileFlag = (caretDataFile->getDataFileType() != DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC);
            m_constructionReloadFileAction->setEnabled(notDynConnFileFlag);
            m_copyPathAndFileNameToClipboardAction->setEnabled(notDynConnFileFlag);
        }
        
        m_constructionReloadFileAction->setText(menuText);
    }
}

/**
 * Add an overlay above this overlay.
 */
void 
OverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_overlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void 
OverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_overlayIndex);
}

/**
 * Remove this overlay.
 */
void 
OverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
OverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
OverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_overlayIndex);
}

/**
 * Copy the file name to the clip board.
 */
void
OverlayViewController::menuCopyFileNameToClipBoard()
{
    if (this->overlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        this->overlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            QApplication::clipboard()->setText(caretDataFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Copy the map name to the clip board.
 */
void
OverlayViewController::menuCopyMapNameToClipBoard()
{
    if (this->overlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        this->overlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            if ((mapIndex >= 0)
                && (mapIndex < caretDataFile->getNumberOfMaps())) {
                QApplication::clipboard()->setText(caretDataFile->getMapName(mapIndex).trimmed(),
                                                   QClipboard::Clipboard);
            }
        }
    }
}

/**
 * Reload the file in the overlay.
 */
void OverlayViewController::menuReloadFileTriggered()
{
    if (this->overlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        this->overlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            AString username;
            AString password;
            
            if (DataFile::isFileOnNetwork(caretDataFile->getFileName())) {
                const QString msg("This file is on the network.  "
                                  "If accessing the file requires a username and "
                                  "password, enter it here.  Otherwise, remove any "
                                  "text from the username and password fields.");
                
                
                if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(m_constructionToolButton,
                                                                           "Username and Password",
                                                                           msg,
                                                                           username,
                                                                           password)) {
                    /* nothing */
                }
                else {
                    return;
                }
            }
            
            EventDataFileReload reloadEvent(GuiManager::get()->getBrain(),
                                            caretDataFile);
            reloadEvent.setUsernameAndPassword(username,
                                               password);
            EventManager::get()->sendEvent(reloadEvent.getPointer());
            
            if (reloadEvent.isError()) {
                WuQMessageBox::errorOk(m_constructionToolButton,
                                       reloadEvent.getErrorMessage());
            }
            
            updateOverlaySettingsEditor();
            
            updateUserInterfaceAndGraphicsWindow();
        }
    }
}


