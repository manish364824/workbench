
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WB_MACRO_CUSTOM_OPERATION_ANIMATE_VOLUME_TO_SURFACE_CROSS_FADE_DECLARE__
#include "WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade.h"
#undef __WB_MACRO_CUSTOM_OPERATION_ANIMATE_VOLUME_TO_SURFACE_CROSS_FADE_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "GuiManager.h"
#include "ModelWholeBrain.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade
 * \brief Custom Macro Command for Surface Interpolation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade::WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::ANIMATE_VOLUME_TO_SURFACE_CROSS_FADE)
{
}

/**
 * Destructor.
 */
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade::~WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade()
{
}

/**
 * Get a new instance of the macro command
 *
 * @return
 *     Pointer to command or NULL if not valid
 *     Use getErrorMessage() for error information if NULL returned
 */
WuQMacroCommand*
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade::createCommand()
{
    const int32_t versionOne(1);

    const QString description("Crossfade (blend) from volume to surface:\n"
                              "ALL View must be selected.\n"
                              "(1) The opacity of the surface is set to zero;\n"
                              "(2) The opacity of the volume overlay is set to one\n"
                              "(3) The opacity of the surface increases until it is one\n"
                              "    and simultaneously, the opacity of the volume\n"
                              "    decreases until it reaches zero.\n");
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         WbMacroCustomOperationTypeEnum::toGuiName(getOperationType()),
                                                                         description,
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
//        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
//                              "Fade from Overlay",
//                              (int)2);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)10.0);
    }
    else {
        appendToErrorMessage(errorMessage);
    }
    
    return command;
}

/**
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     the macro executor monitor
 * @param executorOptions
 *     Options for executor
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade::executeCommand(QWidget* parent,
                                                               const WuQMacroExecutorMonitor* executorMonitor,
                                                               const WuQMacroExecutorOptions* executorOptions,
                                                               const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 1)) {
        return false;
    }
    const float durationSeconds(macroCommand->getParameterAtIndex(0)->getValue().toFloat());

    BrainBrowserWindow* bbw = qobject_cast<BrainBrowserWindow*>(parent);
    if (bbw == NULL) {
        appendToErrorMessage("Parent for running surface macro is not a browser window");
        return false;
    }
    
    BrowserTabContent* tabContent = bbw->getBrowserTabContent();
    if (tabContent == NULL) {
        appendToErrorMessage("No tab is selected in browser window");
        return false;
    }
    
    ModelWholeBrain* wholeBrainModel = tabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        appendToErrorMessage("All view must be selected");
        return false;
    }
    
    OverlaySet* overlaySet = tabContent->getOverlaySet();
    CaretAssert(overlaySet);
    
    Overlay* volumeOverlay = overlaySet->getUnderlayContainingVolume();
    if (volumeOverlay == NULL) {
        appendToErrorMessage("An overlay must contain a volume");
        return false;
    }
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }


    bool successFlag = performCrossFade(executorMonitor,
                                        executorOptions,
                                        volumeOverlay,
                                        durationSeconds);
    
    return successFlag;
}

/**
 * Interpolate from starting to ending surface
 *
 * @param executorMonitor
 *     The macro executor's monitor
 * @param executorOptions
 *     The executor options
 * @param volumeOverlay
 *     Overlay that contains the volume
 * @param durationSeconds
 *     Total duration for cross fade
 * @return
 *     True if successful, else false
 */
bool
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade::performCrossFade(const WuQMacroExecutorMonitor* executorMonitor,
                                                                 const WuQMacroExecutorOptions* executorOptions,
                                                                 Overlay* volumeOverlay,
                                                                 const float durationSeconds)
{
    CaretAssert(volumeOverlay);
    
    const float defaultNumberOfSteps(25.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(executorOptions,
                                 defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);

    const float opacityDelta = 1.0 / numberOfSteps;
    float surfaceOpacity(0.0);
    float volumeOpacity(1.0);

    DisplayPropertiesSurface* surfaceProperties = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    CaretAssert(surfaceProperties);
    DisplayPropertiesVolume* volumeProperties = GuiManager::get()->getBrain()->getDisplayPropertiesVolume();
    CaretAssert(volumeProperties);
    
    /*
     * Initialize the opacities
     */

    for (int iStep = 0; iStep < numberOfSteps; iStep++) {
        surfaceProperties->setOpacity(surfaceOpacity);
        volumeProperties->setOpacity(volumeOpacity);
        volumeOverlay->setOpacity(volumeOpacity);
        updateSurfaceColoring();
        updateUserInterface();
        updateGraphics();
        
        surfaceOpacity   += opacityDelta;
        if (surfaceOpacity > 1.0) {
            surfaceOpacity = 1.0;
        }
        volumeOpacity -= opacityDelta;
        if (volumeOpacity < 0.0) {
            volumeOpacity = 0.0;
        }
        
        if (executorMonitor->testForStop()) {
            appendToErrorMessage(executorMonitor->getStoppedByUserMessage());
            return false;
        }
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }
    
    surfaceProperties->setOpacity(1.0);
    volumeProperties->setOpacity(0.0);
    volumeOverlay->setOpacity(0.0);

    updateSurfaceColoring();
    updateUserInterface();
    updateGraphics();

    return true;
}

