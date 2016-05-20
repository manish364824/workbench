
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

#define __EVENT_IMAGE_CAPTURE_DECLARE__
#include "EventImageCapture.h"
#undef __EVENT_IMAGE_CAPTURE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventImageCapture 
 * \brief Event for capturing images.
 * \ingroup GuiQt
 */

/**
 * Constructor for capturing image of window without any resizing.
 *
 * @param browserWindowIndex
 *    The browser window index.
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_captureOffsetX(0),
m_captureOffsetY(0),
m_captureWidth(0),
m_captureHeight(0),
m_outputWidth(0),
m_outputHeight(0)
{
}

/**
 * Constructor for capturing image of window with the given sizing.  If
 * the X & Y sizes are both zero, the no image resizing is performed.
 *
 * @param browserWindowIndex
 *    The browser window index.
 * @param captureOffsetX
 *    X-offset for capturing image.
 * @param captureOffsetY
 *    Y-offset for capturing image.
 * @param captureWidth
 *    Width for capturing image.
 * @param captureHeight
 *    Height for capturing image.
 * @param outputWidth
 *    Width of output image.
 * @param outputHeight
 *    Height of output image.
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex,
                                     const int32_t captureOffsetX,
                                     const int32_t captureOffsetY,
                                     const int32_t captureWidth,
                                     const int32_t captureHeight,
                                     const int32_t outputWidth,
                                     const int32_t outputHeight)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_captureOffsetX(captureOffsetX),
m_captureOffsetY(captureOffsetY),
m_captureWidth(captureWidth),
m_captureHeight(captureHeight),
m_outputWidth(outputWidth),
m_outputHeight(outputHeight)
{
    m_backgroundColor[0] = 0;
    m_backgroundColor[1] = 0;
    m_backgroundColor[2] = 0;
}


/**
 * Destructor.
 */
EventImageCapture::~EventImageCapture()
{
}

/**
 * @return The browser window index.
 */
int32_t
EventImageCapture::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The capture X offset
 */
int32_t
EventImageCapture::getCaptureOffsetX() const
{
    return m_captureOffsetX;
}

/**
 * @return The capture Y offset
 */
int32_t
EventImageCapture::getCaptureOffsetY() const
{
    return m_captureOffsetY;
}

/**
 * @return The capture width
 */
int32_t
EventImageCapture::getCaptureWidth() const
{
    return m_captureWidth;
}

/**
 * @return The capture height
 */
int32_t
EventImageCapture::getCaptureHeight() const
{
    return m_captureHeight;
}

/**
 * @return The output image width.
 */
int32_t
EventImageCapture::getOutputWidth() const
{
    return m_outputWidth;
}

/**
 * @return The output image height.
 */
int32_t
EventImageCapture::getOutputHeight() const
{
    return m_outputHeight;
}

/**
 * Get the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::getBackgroundColor(uint8_t backgroundColor[3]) const
{
    backgroundColor[0] = m_backgroundColor[0];
    backgroundColor[1] = m_backgroundColor[1];
    backgroundColor[2] = m_backgroundColor[2];
}

/**
 * Set the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::setBackgroundColor(const uint8_t backgroundColor[3])
{
    m_backgroundColor[0] = backgroundColor[0];
    m_backgroundColor[1] = backgroundColor[1];
    m_backgroundColor[2] = backgroundColor[2];
}

/**
 * @return The captured image.
 */
QImage
EventImageCapture::getImage() const
{
    return m_image;
}

/**
 * Set the captured image.
 */
void
EventImageCapture::setImage(const QImage& image)
{
    m_image = image;
}
