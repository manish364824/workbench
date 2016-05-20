#ifndef __ANNOTATION_FONT_WIDGET_H__
#define __ANNOTATION_FONT_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <stdint.h>

#include <vector>
#include <QWidget>

#include "AnnotationWidgetParentEnum.h"
#include "CaretColorEnum.h"

class QToolButton;

namespace caret {

    class Annotation;
    class AnnotationFontAttributesInterface;
    class AnnotationText;
    class CaretColorEnumMenu;
    class EnumComboBoxTemplate;
    class WuQSpecialIncrementDoubleSpinBox;
    
    class AnnotationFontWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationFontWidget(const AnnotationWidgetParentEnum::Enum parentWidgetType,
                             const int32_t browserWindowIndex,
                             QWidget* parent = 0);
        
        virtual ~AnnotationFontWidget();

        void updateContent(std::vector<AnnotationFontAttributesInterface*>& annotations);

        // ADD_NEW_METHODS_HERE
        
    private slots:
        void textColorSelected(const CaretColorEnum::Enum);
        
        void fontBoldChanged();

        void fontItalicChanged();
        
        void fontNameChanged();
        
        void fontSizeChanged();
        
        void fontUnderlineChanged();
        
    private:
        AnnotationFontWidget(const AnnotationFontWidget&);

        AnnotationFontWidget& operator=(const AnnotationFontWidget&);
        
        void updateFontSizeSpinBox(const float value,
                                   const bool haveMultipleValuesFlag);
        
        void updateTextColorButton();
        
        float getSurfaceMontageRowCount() const;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        EnumComboBoxTemplate* m_fontNameComboBox;

        WuQSpecialIncrementDoubleSpinBox* m_fontSizeSpinBox;
        
        QToolButton* m_textColorToolButton;
        
        QAction* m_textColorAction;
        
        CaretColorEnumMenu* m_textColorMenu;
        
        QAction* m_boldFontAction;
        
        QAction* m_italicFontAction;
        
        QAction* m_underlineFontAction;
        
        /** Contains annotations */
        std::vector<Annotation*> m_annotations;
        
        /** Contains annotations with type as font style */
        std::vector<AnnotationFontAttributesInterface*> m_annotationsFontStyle;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FONT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FONT_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_FONT_WIDGET_H__
