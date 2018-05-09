#ifndef __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_
#define __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_

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


#include "CaretObject.h"

namespace caret {
    class AnnotationText;

    class BrainOpenGLTextRenderInterface : public CaretObject {
        
    protected:
        BrainOpenGLTextRenderInterface();
        
    public:
        virtual ~BrainOpenGLTextRenderInterface();
        
        /**
         * Draw annnotation text at the given viewport coordinates using
         * the the annotations attributes for the style of text.
         *
         * Depth testing is DISABLED when drawing text with this method.
         *
         * @param viewportX
         *     Viewport X-coordinate.
         * @param viewportY
         *     Viewport Y-coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const AnnotationText& annotationText) = 0;
        
        /**
         * Draw annnotation text at the given viewport coordinates using
         * the the annotations attributes for the style of text.
         *
         * Depth testing is ENABLED when drawing text with this method.
         *
         * @param viewportX
         *     Viewport X-coordinate.
         * @param viewportY
         *     Viewport Y-coordinate.
         * @param viewportZ
         *     Viewport Z-coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText) = 0;
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * Depth testing is ENABLED when drawing text with this method.
         *
         * @param modelX
         *     Model X-coordinate.
         * @param modelY
         *     Model Y-coordinate.
         * @param modelZ
         *     Model Z-coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        virtual void drawTextAtModelCoords(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const AnnotationText& annotationText) = 0;
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * Depth testing is ENABLED when drawing text with this method.
         *
         * @param modelXYZ
         *     Model XYZ coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        void drawTextAtModelCoords(const double modelXYZ[3],
                                   const AnnotationText& annotationText) {
            drawTextAtModelCoords(modelXYZ[0], modelXYZ[1], modelXYZ[2], annotationText);
        }
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * Depth testing is ENABLED when drawing text with this method.
         *
         * @param modelXYZ
         *     Model XYZ coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        void drawTextAtModelCoords(const float modelXYZ[3],
                                   const AnnotationText& annotationText) {
            drawTextAtModelCoords(modelXYZ[0], modelXYZ[1], modelXYZ[2], annotationText);
        }
        
        /**
         * Get the estimated width and height of text (in pixels) using the given text
         * attributes.
         *
         * See http://ftgl.sourceforge.net/docs/html/metrics.png
         *
         * @param annotationText
         *   Text for width and height estimation.
         * @param viewportWidth
         *    Width of the viewport needed for percentage height text.
         * @param viewportHeight
         *    Height of the viewport needed for percentage height text.
         * @param widthOut
         *    Estimated width of text.
         * @param heightOut
         *    Estimated height of text.
         */
        virtual void getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                const double viewportWidth,
                                                const double viewportHeight,
                                                double& widthOut,
                                                double& heightOut) = 0;
        
        /**
         * Get the bounds of text (in pixels) using the given text
         * attributes.
         *
         * See http://ftgl.sourceforge.net/docs/html/metrics.png
         *
         * @param annotationText
         *   Text that is to be drawn.
         * @param viewportX
         *    Viewport X-coordinate.
         * @param viewportY
         *    Viewport Y-coordinate.
         * @param viewportZ
         *    Viewport Z-coordinate.
         * @param viewportWidth
         *    Width of the viewport needed for percentage height text.
         * @param viewportHeight
         *    Height of the viewport needed for percentage height text.
         * @param bottomLeftOut
         *    The bottom left corner of the text bounds.
         * @param bottomRightOut
         *    The bottom right corner of the text bounds.
         * @param topRightOut
         *    The top right corner of the text bounds.
         * @param topLeftOut
         *    The top left corner of the text bounds.
         */
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
                                                      const double viewportWidth,
                                                      const double viewportHeight,
                                                      double bottomLeftOut[3],
                                                      double bottomRightOut[3],
                                                      double topRightOut[3],
                                                      double topLeftOut[3]) = 0;
        
        /**
         * Get the bounds of text (in pixels) using the given text
         * attributes.
         *
         * See http://ftgl.sourceforge.net/docs/html/metrics.png
         *
         * @param annotationText
         *   Text that is to be drawn.
         * @param viewportX
         *    Viewport X-coordinate.
         * @param viewportY
         *    Viewport Y-coordinate.
         * @param viewportZ
         *    Viewport Z-coordinate.
         * @param viewportWidth
         *    Width of the viewport needed for percentage height text.
         * @param viewportHeight
         *    Height of the viewport needed for percentage height text.
         * @param bottomLeftOut
         *    The bottom left corner of the text bounds.
         * @param bottomRightOut
         *    The bottom right corner of the text bounds.
         * @param topRightOut
         *    The top right corner of the text bounds.
         * @param topLeftOut
         *    The top left corner of the text bounds.
         */
        void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                              const float viewportX,
                                              const float viewportY,
                                              const float viewportZ,
                                              const float viewportWidth,
                                              const float viewportHeight,
                                              float bottomLeftOut[3],
                                              float bottomRightOut[3],
                                              float topRightOut[3],
                                              float topLeftOut[3]);
        
        static float pointSizeToPixels(const float pointSize);
        
        static float pixelsToPointSize(const float pixels);
        
        static float getPixelsPerInch();
        
        static void setPixelsPerInch(const float pixelsPerInch);

        /**
         * @return The font system is valid.
         */
        virtual bool isValid() const = 0;

        /**
         * @return Name of the text renderer.
         */
        virtual AString getName() const = 0;
        
    private:
        BrainOpenGLTextRenderInterface(const BrainOpenGLTextRenderInterface&);

        BrainOpenGLTextRenderInterface& operator=(const BrainOpenGLTextRenderInterface&);
        
    private:
        static float s_pixelsPerInch;
    };
    
#ifdef __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__
    float BrainOpenGLTextRenderInterface::s_pixelsPerInch = 72.0;
#endif // __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_
