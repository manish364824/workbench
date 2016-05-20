
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_DECLARE__
#include "BackgroundAndForegroundColorsSceneHelper.h"
#undef __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_DECLARE__

#include "BackgroundAndForegroundColors.h"
#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::BackgroundAndForegroundColorsSceneHelper 
 * \brief Helps with saving and restoring background and foreground colors to/from scene.
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
BackgroundAndForegroundColorsSceneHelper::BackgroundAndForegroundColorsSceneHelper(BackgroundAndForegroundColors& colors)
: CaretObject(),
m_colors(colors),
m_wasRestoredFromSceneFlag(false)
{
    
    m_sceneAssistant = new SceneClassAssistant();

    m_sceneAssistant->addArray("m_colorForegroundAll",
                               m_colors.m_colorForegroundAll, 3, 255);
    m_sceneAssistant->addArray("m_colorBackgroundAll",
                               m_colors.m_colorBackgroundAll, 3, 0);
    
    m_sceneAssistant->addArray("m_colorForegroundChart",
                               m_colors.m_colorForegroundChart, 3, 255);
    m_sceneAssistant->addArray("m_colorBackgroundChart",
                               m_colors.m_colorBackgroundChart, 3, 0);
    
    m_sceneAssistant->addArray("m_colorForegroundSurface",
                               m_colors.m_colorForegroundSurface, 3, 255);
    m_sceneAssistant->addArray("m_colorBackgroundSurface",
                               m_colors.m_colorBackgroundSurface, 3, 0);
    
    m_sceneAssistant->addArray("m_colorForegroundVolume",
                               m_colors.m_colorForegroundVolume, 3, 255);
    m_sceneAssistant->addArray("m_colorBackgroundVolume",
                               m_colors.m_colorBackgroundVolume, 3, 0);
    
    m_sceneAssistant->addArray("m_colorChartMatrixGridLines",
                               m_colors.m_colorChartMatrixGridLines, 3, 0);
}

/**
 * Destructor.
 */
BackgroundAndForegroundColorsSceneHelper::~BackgroundAndForegroundColorsSceneHelper()
{
    delete m_sceneAssistant;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BackgroundAndForegroundColorsSceneHelper::toString() const
{
    return "BackgroundAndForegroundColorsSceneHelper";
}

/**
 * @return True if the colors were restored from the scene, else false.
 */
bool
BackgroundAndForegroundColorsSceneHelper::wasRestoredFromScene() const
{
    return m_wasRestoredFromSceneFlag;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
BackgroundAndForegroundColorsSceneHelper::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    m_wasRestoredFromSceneFlag = false;
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BackgroundAndForegroundColorsSceneHelper",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
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
 *     sceneClass from which model specific information is obtained.
 */
void
BackgroundAndForegroundColorsSceneHelper::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    m_wasRestoredFromSceneFlag = false;
    
    if (sceneClass == NULL) {
        /*
         * If not found, assign the default values
         */
        m_colors = BackgroundAndForegroundColors();
        
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_wasRestoredFromSceneFlag = true;
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

