#ifndef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
#define __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_

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


#include "BrainOpenGLTextAttributes.h"
#include "BrainOpenGLTextRenderInterface.h"

class QGLWidget;

class QFont;

namespace caret {

    class BrainOpenGLWidgetTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        BrainOpenGLWidgetTextRenderer(QGLWidget* glWidget);
        
        virtual ~BrainOpenGLWidgetTextRenderer();
        
        bool isValid() const;
        
        void drawTextAtViewportCoords(const int viewport[4],
                                    const double windowX,
                                    const double windowY,
                                    const QString& text,
                                    const BrainOpenGLTextAttributes& textAttributes);
        
        void drawTextAtModelCoords(const double modelX,
                                   const double modelY,
                                   const double modelZ,
                                   const QString& text,
                                   const BrainOpenGLTextAttributes& textAttributes);
        
        void getTextBoundsInPixels(double& widthOut,
                                   double& heightOut,
                                   const QString& text,
                                   const BrainOpenGLTextAttributes& textAttributes);

        virtual AString getName() const;
        
    private:
        BrainOpenGLWidgetTextRenderer(const BrainOpenGLWidgetTextRenderer&);

        BrainOpenGLWidgetTextRenderer& operator=(const BrainOpenGLWidgetTextRenderer&);
        
    private:
        class FontData {
        public:
            FontData();

            ~FontData();
            
            void initialize(const AString& fontName,
                            const bool boldFlag);
            
            QFont* m_font;
            bool m_fontValid;
        };
        
        void drawHorizontalTextAtWindowCoords(const int viewport[4],
                                              const double windowX,
                                              const double windowY,
                                              const QString& text,
                                              const BrainOpenGLTextAttributes&  textAttributes);
        
        void drawVerticalTextAtWindowCoords(const int viewport[4],
                                            const double windowX,
                                            const double windowY,
                                            const QString& text,
                                            const BrainOpenGLTextAttributes&  textAttributes);
        
        FontData m_normalFont;
        
        FontData m_boldFont;
        
        QFont* findFont(const bool boldFlag,
                        const int fontHeight);
        
        QGLWidget* m_glWidget;
    };
    
#ifdef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
