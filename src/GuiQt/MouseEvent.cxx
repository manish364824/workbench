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

#include <QMouseEvent>
#include <QWheelEvent>

#include "MouseEvent.h"

using namespace caret;

/**
 * \class caret::MouseEvent
 * \brief Event issued when mouse is moved or buttons are pressed.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param viewportContent
 *    Content of viewport.
 * @param openGLWidget
 *    OpenGL Widget in which mouse activity took place.
 * @param browserWindowIndex
 *    Index of the browser winddow in which mouse activity took place.
 * @param x
 *    Current mouse X-coordinate (left == 0)
 * @param y
 *    Current mouse Y-coordinate (bottom == 0)
 * @param dx
 *    Change in mouse X-coordinate since last mouse event
 * @param dy
 *    Change in mouse Y-coordinate since last mouse event
 * @param mousePressX
 *    X-coordinate of mouse when button was pressed
 * @param mousePressY
 *    Y-coordinate of mouse when button was pressed
 * @param firstDraggingFlag
 *    Should be true the first time in in a mouse dragging operation.
 */
MouseEvent::MouseEvent(BrainOpenGLViewportContent* viewportContent,
                       BrainOpenGLWidget* openGLWidget,
                       const int32_t browserWindowIndex,
                       const int32_t x,
                       const int32_t y,
                       const int32_t dx,
                       const int32_t dy,
                       const int32_t mousePressX,
                       const int32_t mousePressY,
                       const bool firstDraggingFlag)
: CaretObject()
{
    initializeMembersMouseEvent();

    m_viewportContent = viewportContent;
    m_openGLWidget    = openGLWidget;
    m_browserWindowIndex = browserWindowIndex;
    m_x = x;
    m_y = y;
    m_dx = dx;
    m_dy = dy;
    m_pressX = mousePressX;
    m_pressY = mousePressY;
    m_firstDraggingFlag = firstDraggingFlag;
}

/**
 * Destructor
 */
MouseEvent::~MouseEvent()
{
}

/**
 * Initialize all members.
 */
void
MouseEvent::initializeMembersMouseEvent()
{
    m_viewportContent = NULL;
    m_openGLWidget = NULL;
    m_browserWindowIndex = -1;
    m_x = 0;
    m_y = 0;
    m_dx = 0;
    m_dy = 0;
    m_pressX = 0;
    m_pressY = 0;
    m_wheelRotation = 0;
}

/**
 * @return The viewport content in which the mouse was pressed.
 */
BrainOpenGLViewportContent*
MouseEvent::getViewportContent() const
{
    return m_viewportContent;
}

/**
 * @return The OpenGL Widget in which the mouse event occurred.
 */
BrainOpenGLWidget*
MouseEvent::getOpenGLWidget() const
{
    return m_openGLWidget;
}

/**
 * Get a string showing the contents of this mouse event.
 * @return String describing the mouse status.
 */
AString
MouseEvent::toString() const
{
    const AString msg = 
      ", x=" + AString::number(m_x)
    + ", y=" + AString::number(m_y)
    + ", dx=" + AString::number(m_dx)
    + ", dy=" + AString::number(m_dy);
    + ", pressX=" + AString::number(m_pressX)
    + ", pressY=" + AString::number(m_pressY);
    + ", wheelRotation=" + AString::number(m_wheelRotation);
    
    return msg;
}

/**
 * @return Index of the browser window in which the
 * event took place.
 */
int32_t 
MouseEvent::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * Get the change in the X-coordinate.
 * @return change in the X-coordinate.
 *
 */
int32_t
MouseEvent::getDx() const
{
    return m_dx;
}

/**
 * Get the change in the Y-coordinate.
 * @return change in the Y-coordinate.
 *
 */
int32_t
MouseEvent::getDy() const
{
    return m_dy;
}

/**
 * Get the X-coordinate of the mouse.
 * @return The X-coordinate.
 *
 */
int32_t
MouseEvent::getX() const
{
    return m_x;
}

/**
 * Get the Y-coordinate of the mouse.
 * Origin is at the BOTTOM of the widget !!!
 * @return The Y-coordinate.
 *
 */
int32_t
MouseEvent::getY() const
{
    return m_y;
}

/**
 * Get the X-coordinate of where the mouse was pressed.
 * @return The X-coordinate.
 *
 */
int32_t
MouseEvent::getPressedX() const
{
    return m_pressX;
}

/**
 * Get the Y-coordinate of where the mouse was pressed.
 * Origin is at the BOTTOM of the widget !!!
 * @return The Y-coordinate.
 *
 */
int32_t
MouseEvent::getPressedY() const
{
    return m_pressY;
}

/**
 * Get the amount of rotation in the mouse wheel.
 * @return  Amount mouse wheel rotated.
 *
 */
int32_t
MouseEvent::getWheelRotation() const
{
    return m_wheelRotation;
}

/**
 * @return Is this the first in a sequence of mouse dragging?
 *
 * A mouse drag is the sequence:
 * (1) User presses the mouse
 * (2) One or more calls are made to the input receivers mouseLeftDrag() method
 * (3) User releases the mouse ending the dragging.
 *
 * This method returns true for the first call made to mouseLeftDrag() in
 * step 2 and false in all other calls to mouseLeftDrag().
 */
bool
MouseEvent::isFirstDragging() const
{
    return m_firstDraggingFlag;
}
