
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

#include <QMessageBox>

#define __USER_INPUT_MODE_VIEW_DECLARE__
#include "UserInputModeView.h"
#undef __USER_INPUT_MODE_VIEW_DECLARE__

#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventUpdateYokedWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MouseEvent.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeView 
 * \brief Processing user input for VIEW mode.
 *
 * Processes user input in VIEW mode which includes
 * viewing transformation of brain models and
 * identification operations.
 */

/**
 * Constructor.
 */
UserInputModeView::UserInputModeView()
: UserInputModeAbstract(UserInputModeAbstract::VIEW)
{
    
}

/**
 * Constructor for subclasses.
 *
 * @param inputMode
 *    Subclass' input mode.
 */
UserInputModeView::UserInputModeView(const UserInputMode inputMode)
: UserInputModeAbstract(inputMode)
{
    
}


/**
 * Destructor.
 */
UserInputModeView::~UserInputModeView()
{
    
}

/**
 * Process identification..
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 * @param mouseClickX
 *     Location of where mouse was clicked.
 * @param mouseClickY
 *     Location of where mouse was clicked.
 */
void
UserInputModeView::processModelViewIdentification(BrainOpenGLViewportContent* viewportContent,
                                           BrainOpenGLWidget* openGLWidget,
                                           const int32_t mouseClickX,
                                           const int32_t mouseClickY)
{
    SelectionManager* selectionManager =
    openGLWidget->performIdentification(mouseClickX,
                                        mouseClickY,
                                        false);
    
    BrowserTabContent* btc = viewportContent->getBrowserTabContent();
    if (btc != NULL) {
       const int32_t tabIndex = btc->getTabNumber();
       GuiManager::get()->processIdentification(tabIndex,
                                                selectionManager,
                                                openGLWidget);    
   }
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void 
UserInputModeView::initialize()
{
    
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeView::finish()
{
    
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeView::update()
{
    
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeView::getCursor() const
{
    
    return CursorEnum::CURSOR_DEFAULT;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserInputModeView::toString() const
{
    return "UserInputModeView";
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftClick(const MouseEvent& mouseEvent)
{
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
    processModelViewIdentification(mouseEvent.getViewportContent(),
                                   mouseEvent.getOpenGLWidget(),
                                   mouseEvent.getX(),
                                   mouseEvent.getY());
}

/**
 * Process a mouse left click with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftClickWithShift(const MouseEvent& mouseEvent)
{
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDrag(const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    browserTabContent->applyMouseRotation(viewportContent,
                                          mouseEvent.getPressedX(),
                                          mouseEvent.getPressedY(),
                                          mouseEvent.getX(),
                                          mouseEvent.getY(),
                                          mouseEvent.getDx(),
                                          mouseEvent.getDy());
    /*
     * Update graphics.
     */
    updateGraphics(mouseEvent);
}

/**
 * Process a mouse left drag with only the alt key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithAlt(const MouseEvent& mouseEvent)
{
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
}

/**
 * Process a mouse left drag with ctrl key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithCtrl(const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    browserTabContent->applyMouseScaling(mouseEvent.getDx(), mouseEvent.getDy());
    updateGraphics(mouseEvent);
}

/**
 * Process a mouse left drag with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithShift(const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    browserTabContent->applyMouseTranslation(viewportContent,
                                             mouseEvent.getPressedX(),
                                             mouseEvent.getPressedY(),
                                             mouseEvent.getDx(),
                                             mouseEvent.getDy());
    updateGraphics(mouseEvent);
}

/**
 * If this windows is yoked, issue an event to update other
 * windows that are using the same yoking.
 */
void
UserInputModeView::updateGraphics(const MouseEvent& mouseEvent)
{
    bool issuedYokeEvent = false;
    if (mouseEvent.getViewportContent() != NULL) {
        BrowserTabContent* browserTabContent = mouseEvent.getViewportContent()->getBrowserTabContent();
        const int32_t browserWindowIndex = mouseEvent.getBrowserWindowIndex();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(browserWindowIndex).getPointer());
        
        if (browserTabContent != NULL) {
            if (browserTabContent->isYoked()) {
                issuedYokeEvent = true;
                EventManager::get()->sendEvent(EventUpdateYokedWindows(browserTabContent->getYokingGroup()).getPointer());
            }
        }
    }
    
    /*
     * If not yoked, just need to update graphics.
     */
    if (issuedYokeEvent == false) {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
    }
}


