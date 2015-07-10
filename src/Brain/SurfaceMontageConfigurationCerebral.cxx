
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

#define __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__
#include "SurfaceMontageConfigurationCerebral.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventBrainStructureGetAll.h"
#include "EventManager.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationCerebral 
 * \brief Surface montage cerebral cortext configuration.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationCerebral::SurfaceMontageConfigurationCerebral(const int32_t tabIndex)
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_YES)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    SurfaceTypeEnum::getAllEnumsExceptFlat(validSurfaceTypes);
    
    m_leftFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT,
                                                                 validSurfaceTypes);
    m_leftSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT,
                                                                  validSurfaceTypes);
    m_rightFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT,
                                                                  validSurfaceTypes);
    m_rightSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT,
                                                                   validSurfaceTypes);
    m_leftEnabled = true;
    m_rightEnabled = true;
    m_firstSurfaceEnabled = true;
    m_secondSurfaceEnabled = false;
    m_lateralEnabled = true;
    m_medialEnabled  = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_leftFirstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_leftFirstSurfaceSelectionModel);
    m_sceneAssistant->add("m_leftSecondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_leftSecondSurfaceSelectionModel);
    m_sceneAssistant->add("m_rightFirstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_rightFirstSurfaceSelectionModel);
    m_sceneAssistant->add("m_rightSecondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_rightSecondSurfaceSelectionModel);
    
    m_sceneAssistant->add("m_leftEnabled",
                          &m_leftEnabled);
    m_sceneAssistant->add("m_rightEnabled",
                          &m_rightEnabled);
    m_sceneAssistant->add("m_firstSurfaceEnabled",
                          &m_firstSurfaceEnabled);
    m_sceneAssistant->add("m_secondSurfaceEnabled",
                          &m_secondSurfaceEnabled);
    m_sceneAssistant->add("m_lateralEnabled",
                          &m_lateralEnabled);
    m_sceneAssistant->add("m_medialEnabled",
                          &m_medialEnabled);

    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::CORTEX_LEFT);
    supportedStructures.push_back(StructureEnum::CORTEX_RIGHT);
    setupOverlaySet("Cerebral Montage",
                    tabIndex,
                    supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationCerebral::~SurfaceMontageConfigurationCerebral()
{
    delete m_leftFirstSurfaceSelectionModel;
    delete m_leftSecondSurfaceSelectionModel;
    delete m_rightFirstSurfaceSelectionModel;
    delete m_rightSecondSurfaceSelectionModel;
    
    delete m_sceneAssistant;
}

/**
 * Initialize the selected surfaces.
 */
void
SurfaceMontageConfigurationCerebral::initializeSelectedSurfaces()
{
    EventBrainStructureGetAll brainStructureEvent;
    EventManager::get()->sendEvent(brainStructureEvent.getPointer());
    
    Surface* leftAnatSurface = NULL;
    BrainStructure* leftBrainStructure = brainStructureEvent.getBrainStructureByStructure(StructureEnum::CORTEX_LEFT);
    if (leftBrainStructure != NULL) {
        leftAnatSurface = leftBrainStructure->getPrimaryAnatomicalSurface();
    }
    
    Surface* rightAnatSurface = NULL;
    BrainStructure* rightBrainStructure = brainStructureEvent.getBrainStructureByStructure(StructureEnum::CORTEX_RIGHT);
    if (rightBrainStructure != NULL) {
        rightAnatSurface = rightBrainStructure->getPrimaryAnatomicalSurface();
    }
    
    m_leftFirstSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
    if (leftAnatSurface != NULL) {
        m_leftFirstSurfaceSelectionModel->setSurface(leftAnatSurface);
    }
    
    
    m_leftSecondSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                           SurfaceTypeEnum::VERY_INFLATED);
    
    m_rightFirstSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
    if (rightAnatSurface != NULL) {
        m_rightFirstSurfaceSelectionModel->setSurface(rightAnatSurface);
    }
    
    m_rightSecondSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                            SurfaceTypeEnum::VERY_INFLATED);
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationCerebral::isValid()
{
    const bool valid = ((getLeftFirstSurfaceSelectionModel()->getSurface() != NULL)
                        || (getRightFirstSurfaceSelectionModel()->getSurface() != NULL));
    return valid;
}

/**
 * Update the montage viewports using the current selected surfaces and settings.
 *
 * @param surfaceMontageViewports
 *     Will be loaded with the montage viewports.
 */
void
SurfaceMontageConfigurationCerebral::updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
    
    std::vector<SurfaceMontageViewport> leftLateralViewports;
    std::vector<SurfaceMontageViewport> leftMedialViewports;
    std::vector<SurfaceMontageViewport> rightLateralViewports;
    std::vector<SurfaceMontageViewport> rightMedialViewports;
    
    if (m_leftEnabled) {
        if (m_firstSurfaceEnabled) {
            Surface* leftSurface = m_leftFirstSurfaceSelectionModel->getSurface();
            if (leftSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
                    leftLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL);
                    leftMedialViewports.push_back(smv);
                }
            }
        }
        if (m_secondSurfaceEnabled) {
            Surface* leftSurface = m_leftSecondSurfaceSelectionModel->getSurface();
            if (leftSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
                    leftLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL);
                    leftMedialViewports.push_back(smv);
                }
            }
        }
    }
    
    if (m_rightEnabled) {
        if (m_firstSurfaceEnabled) {
            Surface* rightSurface = m_rightFirstSurfaceSelectionModel->getSurface();
            if (rightSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL);
                    rightLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL);
                    rightMedialViewports.push_back(smv);
                }
            }
        }
    }
    if (m_rightEnabled) {
        if (m_secondSurfaceEnabled) {
            Surface* rightSurface = m_rightSecondSurfaceSelectionModel->getSurface();
            if (rightSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL);
                    rightLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL);
                    rightMedialViewports.push_back(smv);
                }
            }
        }
    }

    std::vector<SurfaceMontageViewport> leftViewports;
    leftViewports.insert(leftViewports.end(),
                         leftLateralViewports.begin(),
                         leftLateralViewports.end());
    leftViewports.insert(leftViewports.end(),
                         leftMedialViewports.begin(),
                         leftMedialViewports.end());

    std::vector<SurfaceMontageViewport> rightViewports;
    rightViewports.insert(rightViewports.end(),
                         rightLateralViewports.begin(),
                         rightLateralViewports.end());
    rightViewports.insert(rightViewports.end(),
                         rightMedialViewports.begin(),
                         rightMedialViewports.end());
    
    const int32_t numLeft = static_cast<int32_t>(leftViewports.size());
    const int32_t numRight = static_cast<int32_t>(rightViewports.size());
    const int32_t totalNum = numLeft + numRight;
    
    if (totalNum == 1) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftViewports.begin(),
                                       leftViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightViewports.begin(),
                                       rightViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 1);
        
        surfaceMontageViewports[0].setRowAndColumn(0, 0);
        
    }
    else if (totalNum == 2) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftViewports.begin(),
                                       leftViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightViewports.begin(),
                                       rightViewports.end());
        
        CaretAssert(surfaceMontageViewports.size() == 2);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
        }
        
    }
    else if (totalNum == 4) {
        if (numLeft == 4) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftLateralViewports.begin(),
                                           leftLateralViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftMedialViewports.begin(),
                                           leftMedialViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else if (numRight == 4) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightLateralViewports.begin(),
                                           rightLateralViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightMedialViewports.begin(),
                                           rightMedialViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else if (numLeft == numRight) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftViewports.begin(),
                                           leftViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightViewports.begin(),
                                           rightViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else {
            CaretAssert(0);
        }
    }
    else if (totalNum == 8) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftLateralViewports.begin(),
                                       leftLateralViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftMedialViewports.begin(),
                                       leftMedialViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightLateralViewports.begin(),
                                       rightLateralViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightMedialViewports.begin(),
                                       rightMedialViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 8);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                surfaceMontageViewports[6].setRowAndColumn(0, 3);
                surfaceMontageViewports[7].setRowAndColumn(1, 3);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[5].setRowAndColumn(3, 0);
                surfaceMontageViewports[6].setRowAndColumn(2, 1);
                surfaceMontageViewports[7].setRowAndColumn(3, 1);
                break;
        }
    }
    else if (totalNum > 0) {
        CaretAssert(0);
    }
    
    CaretAssert(totalNum == static_cast<int32_t>(surfaceMontageViewports.size()));
    
//    std::cout << "Orientation: " << SurfaceMontageLayoutOrientationEnum::toName(getLayoutOrientation()) << std::endl;
//    for (int32_t i = 0; i < numViewports; i++) {
//        const SurfaceMontageViewport& svp = surfaceMontageViewports[i];
//        std::cout << qPrintable("("
//                                + AString::number(svp.getRow())
//                                + ","
//                                + AString::number(svp.getColumn())
//                                + ") "
//                                + ProjectionViewTypeEnum::toName(svp.getProjectionViewType()))
//        << std::endl;
//    }
//    std::cout << std::endl;
}

/**
 * Save members to the given scene class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 * @param sceneClass
 *     sceneClass to which information is added.
 */
void
SurfaceMontageConfigurationCerebral::saveMembersToScene(const SceneAttributes* sceneAttributes,
                                                          SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which information is restored.
 */
void
SurfaceMontageConfigurationCerebral::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                               const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Is left  enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isLeftEnabled() const
{
    return m_leftEnabled;
}

/**
 * Set left enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setLeftEnabled(const bool enabled)
{
    m_leftEnabled = enabled;
}

/**
 * @return Is right enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isRightEnabled() const
{
    return m_rightEnabled;
}

/**
 * Set right enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setRightEnabled(const bool enabled)
{
    m_rightEnabled = enabled;
}

/**
 * @return Is lateral enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isLateralEnabled() const
{
    return m_lateralEnabled;
}

/**
 * Set lateral enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setLateralEnabled(const bool enabled)
{
    m_lateralEnabled = enabled;
}

/**
 * @return Is medial enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isMedialEnabled() const
{
    return m_medialEnabled;
}

/**
 * Set medial enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setMedialEnabled(const bool enabled)
{
    m_medialEnabled = enabled;
}

/**
 * @return Is  enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isFirstSurfaceEnabled() const
{
    return m_firstSurfaceEnabled;
}

/**
 * Set first surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setFirstSurfaceEnabled(const bool enabled)
{
    m_firstSurfaceEnabled = enabled;
}

/**
 * @return Is first surfce enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isSecondSurfaceEnabled() const
{
    return m_secondSurfaceEnabled;
}

/**
 * Set second surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setSecondSurfaceEnabled(const bool enabled)
{
    m_secondSurfaceEnabled = enabled;
}

/**
 * @return the left first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftFirstSurfaceSelectionModel()
{
    return m_leftFirstSurfaceSelectionModel;
}

/**
 * @return the left first surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftFirstSurfaceSelectionModel() const
{
    return m_leftFirstSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftSecondSurfaceSelectionModel()
{
    return m_leftSecondSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftSecondSurfaceSelectionModel() const
{
    return m_leftSecondSurfaceSelectionModel;
}

/**
 * @return the right first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightFirstSurfaceSelectionModel()
{
    return m_rightFirstSurfaceSelectionModel;
}

/**
 * @return the right first surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightFirstSurfaceSelectionModel() const
{
    return m_rightFirstSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightSecondSurfaceSelectionModel()
{
    return m_rightSecondSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightSecondSurfaceSelectionModel() const
{
    return m_rightSecondSurfaceSelectionModel;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SurfaceMontageConfigurationCerebral::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the instance's content.
 *
 * @param descriptionOut
 *    Description of the instance's content.
 */
void
SurfaceMontageConfigurationCerebral::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    AString msg;
    
    descriptionOut.addLine("Cerebral Montage: ");
    
    descriptionOut.pushIndentation();
    
    if (isLeftEnabled()) {
        if (isFirstSurfaceEnabled()) {
            const Surface* firstLeftSurface = getLeftFirstSurfaceSelectionModel()->getSurface();
            if (firstLeftSurface != NULL) {
                descriptionOut.addLine("Left Surface:");
                descriptionOut.pushIndentation();
                firstLeftSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
        
        if (isSecondSurfaceEnabled()) {
            const Surface* secondLeftSurface = getLeftSecondSurfaceSelectionModel()->getSurface();
            if (secondLeftSurface != NULL) {
                descriptionOut.addLine("Left Surface:");
                descriptionOut.pushIndentation();
                secondLeftSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
    }
    
    if (isRightEnabled()) {
        if (isFirstSurfaceEnabled()) {
            const Surface* firstRightSurface = getRightFirstSurfaceSelectionModel()->getSurface();
            if (firstRightSurface != NULL) {
                descriptionOut.addLine("Right Surface:");
                descriptionOut.pushIndentation();
                firstRightSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
        if (isSecondSurfaceEnabled()) {
            const Surface* secondRightSurface = getRightSecondSurfaceSelectionModel()->getSurface();
            if (secondRightSurface != NULL) {
                descriptionOut.addLine("Right Surface:");
                descriptionOut.pushIndentation();
                secondRightSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
    }
    
    AString viewsMsg = "Selected Views: ";
    
    if (isLateralEnabled()) {
        viewsMsg += " Lateral";
    }
    if (isMedialEnabled()) {
        viewsMsg += " Medial";
    }
    
    descriptionOut.addLine(viewsMsg);
    
    descriptionOut.popIndentation();
}

/**
 * Get all surfaces displayed in this configuration.
 *
 * @param surfaceOut
 *    Will contain all displayed surfaces upon exit.
 */
void
SurfaceMontageConfigurationCerebral::getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    if (isLeftEnabled()) {
        Surface* firstLeftSurface = NULL;;
        if (isFirstSurfaceEnabled()) {
            firstLeftSurface = const_cast<Surface*>(getLeftFirstSurfaceSelectionModel()->getSurface());
            if (firstLeftSurface != NULL) {
                surfacesOut.push_back(const_cast<Surface*>(firstLeftSurface));
            }
        }
        
        if (isSecondSurfaceEnabled()) {
            const Surface* secondLeftSurface = getLeftSecondSurfaceSelectionModel()->getSurface();
            if (secondLeftSurface != NULL) {
                if (secondLeftSurface != firstLeftSurface) {
                    surfacesOut.push_back(const_cast<Surface*>(secondLeftSurface));
                }
            }
        }
    }
    
    if (isRightEnabled()) {
        Surface* firstRightSurface = NULL;
        if (isFirstSurfaceEnabled()) {
            firstRightSurface = const_cast<Surface*>(getRightFirstSurfaceSelectionModel()->getSurface());
            if (firstRightSurface != NULL) {
                surfacesOut.push_back(const_cast<Surface*>(firstRightSurface));
            }
        }
        if (isSecondSurfaceEnabled()) {
            const Surface* secondRightSurface = getRightSecondSurfaceSelectionModel()->getSurface();
            if (secondRightSurface != NULL) {
                if (secondRightSurface != firstRightSurface) {
                    surfacesOut.push_back(const_cast<Surface*>(secondRightSurface));
                }
            }
        }
    }
}

/**
 * Copy the given configuration to this configurtion.
 *
 * @param configuration.
 *    Configuration that is copied.
 */
void
SurfaceMontageConfigurationCerebral::copyConfiguration(SurfaceMontageConfigurationAbstract* configuration)
{
    SurfaceMontageConfigurationAbstract::copyConfiguration(configuration);
    
    SurfaceMontageConfigurationCerebral* cerebralConfiguration = dynamic_cast<SurfaceMontageConfigurationCerebral*>(configuration);
    CaretAssert(cerebralConfiguration);

    m_leftFirstSurfaceSelectionModel->setSurface(cerebralConfiguration->m_leftFirstSurfaceSelectionModel->getSurface());
    
    m_leftSecondSurfaceSelectionModel->setSurface(cerebralConfiguration->m_leftSecondSurfaceSelectionModel->getSurface());
    
    m_rightFirstSurfaceSelectionModel->setSurface(cerebralConfiguration->m_rightFirstSurfaceSelectionModel->getSurface());
    
    m_rightSecondSurfaceSelectionModel->setSurface(cerebralConfiguration->m_rightSecondSurfaceSelectionModel->getSurface());
    
    m_leftEnabled = cerebralConfiguration->m_leftEnabled;
    
    m_rightEnabled = cerebralConfiguration->m_rightEnabled;
    
    m_firstSurfaceEnabled = cerebralConfiguration->m_firstSurfaceEnabled;
    
    m_secondSurfaceEnabled = cerebralConfiguration->m_secondSurfaceEnabled;
    
    m_lateralEnabled = cerebralConfiguration->m_lateralEnabled;
    
    m_medialEnabled = cerebralConfiguration->m_medialEnabled;
}


