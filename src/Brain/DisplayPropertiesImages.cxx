
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

#include <algorithm>

#define __DISPLAY_PROPERTIES_IMAGES_DECLARE__
#include "DisplayPropertiesImages.h"
#undef __DISPLAY_PROPERTIES_IMAGES_DECLARE__

#include "Brain.h"
#include "CaretLogger.h"
#include "ImageFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePathName.h"
#include "ScenePathNameArray.h"

using namespace caret;
    
/**
 * \class caret::DisplayPropertiesImages 
 * \brief Contains display properties for images.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DisplayPropertiesImages::DisplayPropertiesImages(Brain* parentBrain)
: DisplayProperties(),
m_parentBrain(parentBrain)
{
    CaretAssert(parentBrain);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        m_imageFileInTab[i] = NULL;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_imageFileInDisplayGroup[i] = NULL;
    }
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                                                m_displayStatusInTab);
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup",
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_displayStatusInDisplayGroup[0]);
}

/**
 * Destructor.
 */
DisplayPropertiesImages::~DisplayPropertiesImages()
{
}

/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesImages::copyDisplayProperties(const int32_t sourceTabIndex,
                                             const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    m_displayStatusInTab[targetTabIndex]    = m_displayStatusInTab[sourceTabIndex];
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesImages::reset()
{
    //    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
    //        m_displayStatus[i] = true;
    //        m_contralateralDisplayStatus[i] = false;
    //        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
    //        m_pasteOntoSurface[i] = false;
    //    }
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesImages::update()
{
    
}

/**
 * @return  Display status of foci.
 * @param displayGroup
 *     Display group.
 */
bool
DisplayPropertiesImages::isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_displayStatusInTab[tabIndex];
    }
    return m_displayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the display status for foci.
 * @param displayGroup
 *     Display group.
 * @param displayStatus
 *    New status.
 */
void
DisplayPropertiesImages::setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex,
                                    const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_displayStatusInTab[tabIndex] = displayStatus;
    }
    else {
        m_displayStatusInDisplayGroup[displayGroup] = displayStatus;
    }
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
DisplayPropertiesImages::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(m_displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void
DisplayPropertiesImages::setDisplayGroupForTab(const int32_t browserTabIndex,
                                             const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * Get the selected image file.
 *
 * @param displayGroup
 *     Display group.
 * @param tabIndex
 *     The tab index.
 * @return
 *     The selected image file.
 */
ImageFile*
DisplayPropertiesImages::getSelectedImageFile(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const
{
    ImageFile* imageFile = NULL;
    
    CaretAssertArrayIndex(m_imageFileInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_imageFileInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        imageFile = m_imageFileInTab[tabIndex];
    }
    else {
        const int32_t displayGroupInt = DisplayGroupEnum::toIntegerCode(displayGroup);
        CaretAssertArrayIndex(m_imageFileInDisplayGroup,
                              DisplayGroupEnum::NUMBER_OF_GROUPS,
                              displayGroupInt);
        imageFile = m_imageFileInDisplayGroup[displayGroupInt];
    }
    
    const std::vector<ImageFile*> allImageFiles = m_parentBrain->getAllImagesFiles();
    
    if (imageFile != NULL) {
        if (std::find(allImageFiles.begin(),
                      allImageFiles.end(),
                      imageFile) == allImageFiles.end()) {
            imageFile = NULL;
        }
    }
    
    if (imageFile == NULL) {
        if ( ! allImageFiles.empty()) {
            imageFile = allImageFiles[0];
        }
        
        if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            CaretAssertArrayIndex(m_imageFileInTab,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            m_imageFileInTab[tabIndex] = imageFile;
        }
        else {
            CaretAssertArrayIndex(m_imageFileInDisplayGroup, DisplayGroupEnum::NUMBER_OF_GROUPS, (int)displayGroup);
            m_imageFileInDisplayGroup[displayGroup] = imageFile;
        }
    }
    
    return imageFile;
}

/**
 * Set the selected image.
 * @param displayGroup
 *     Display group.
 * @param tabIndex
 *     The tab index.
 * @param imageFile
 *     Newly selected image file.
 */
void
DisplayPropertiesImages::setSelectedImageFile(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          ImageFile* imageFile)
{
    CaretAssertArrayIndex(m_imageFileInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_imageFileInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_imageFileInTab[tabIndex] = imageFile;
    }
    else {
        m_imageFileInDisplayGroup[displayGroup] = imageFile;
    }
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
DisplayPropertiesImages::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesImages",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }

    std::vector<AString> pathNamesTabs;
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        const ImageFile* imageFile = getSelectedImageFile(DisplayGroupEnum::DISPLAY_GROUP_TAB, iTab);
        if (imageFile != NULL) {
            pathNamesTabs.push_back(imageFile->getFileName());
        }
        else {
            pathNamesTabs.push_back("");
        }
    }
    ScenePathNameArray* tabArray = new ScenePathNameArray("m_imageFileInTab",
                                                          pathNamesTabs);
    sceneClass->addChild(tabArray);
    
    std::vector<AString> pathNamesDisplayGroup;
    for (int32_t idg = 0; idg < DisplayGroupEnum::NUMBER_OF_GROUPS; idg++) {
        
        DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::fromIntegerCode(idg, NULL);
        const ImageFile* imageFile = getSelectedImageFile(displayGroup, 0);
        if (imageFile != NULL) {
            pathNamesDisplayGroup.push_back(imageFile->getFileName());
        }
        else {
            pathNamesDisplayGroup.push_back("");
        }
    }
    ScenePathNameArray* displayGroupArray = new ScenePathNameArray("m_imageFileInDisplayGroup",
                                                          pathNamesDisplayGroup);
    sceneClass->addChild(displayGroupArray);
    
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
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
DisplayPropertiesImages::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_imageFileInTab[i] = NULL;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_imageFileInDisplayGroup[i] = NULL;
    }
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    const ScenePathNameArray* tabArray = sceneClass->getPathNameArray("m_imageFileInTab");
    if (tabArray != NULL) {
        const int32_t numElements = tabArray->getNumberOfArrayElements();
        const int32_t maxElem = std::min(numElements,
                                         (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
        for (int32_t iTab = 0; iTab < maxElem; iTab++) {
            const ScenePathName* spn = tabArray->getScenePathNameAtIndex(iTab);
            CaretAssert(spn);
            const AString filename = spn->stringValue();
            if ( ! filename.isEmpty()) {
                m_imageFileInTab[iTab] = findImageFile(filename);
            }
        }
    }
    
    
    const ScenePathNameArray* displayGroupArray = sceneClass->getPathNameArray("m_imageFileInDisplayGroup");
    if (displayGroupArray != NULL) {
        const int32_t numElements = displayGroupArray->getNumberOfArrayElements();
        const int32_t maxElem = std::min(numElements,
                                         (int32_t)DisplayGroupEnum::NUMBER_OF_GROUPS);
        for (int32_t idg = 0; idg < maxElem; idg++) {
            const ScenePathName* spn = displayGroupArray->getScenePathNameAtIndex(idg);
            CaretAssert(spn);
            const AString filename = spn->stringValue();
            if ( ! filename.isEmpty()) {
                DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::fromIntegerCode(idg, NULL);
                m_imageFileInDisplayGroup[displayGroup] = findImageFile(filename);
            }
        }
    }
}

/**
 * Find an image file by matching the full path name to the name of the
 * image file name.  If full path name does not match match to just
 * the name of the file without any path.
 *
 * @param imageFiles
 *    All of the image files.
 * @param imageFileName
 *    Name of the image file.
 * @return
 *    Matched image file, otherwise NULL.
 */
ImageFile*
DisplayPropertiesImages::findImageFile(const AString& imageFileName) const
{
    ImageFile* fullPathNameFile = NULL;
    ImageFile* nameOnlyFile = NULL;
    
    FileInformation fileInfo(imageFileName);
    const AString nameNoPath = fileInfo.getFileName();
    
    const std::vector<ImageFile*> allImageFiles = m_parentBrain->getAllImagesFiles();
    for (std::vector<ImageFile*>::const_iterator iter = allImageFiles.begin();
         iter != allImageFiles.end();
         iter++) {
        ImageFile* imageFile = *iter;
        if (imageFile->getFileName() == imageFileName) {
            fullPathNameFile = imageFile;
            break;
        }
        
        if (imageFile->getFileNameNoPath() == nameNoPath) {
            nameOnlyFile = imageFile;
        }
    }
    
    if (fullPathNameFile != NULL) {
        return fullPathNameFile;
    }
    
    if (nameOnlyFile == NULL) {
        CaretLogWarning("Unable to find image file: "
                        + imageFileName);
    }
    
    return nameOnlyFile;
}

