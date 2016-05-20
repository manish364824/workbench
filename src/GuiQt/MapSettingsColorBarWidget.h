#ifndef __MAP_SETTINGS_COLOR_BAR_WIDGET_H__
#define __MAP_SETTINGS_COLOR_BAR_WIDGET_H__

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


#include <QWidget>

class QCheckBox;
class QLabel;
class QSpinBox;

namespace caret {

    class AnnotationColorBar;
    class EnumComboBoxTemplate;
    class PaletteColorMapping;
    
    class Overlay;
    
    class MapSettingsColorBarWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsColorBarWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsColorBarWidget();
        
//        void updateContent(Overlay* overlay);
        
        void updateContent(AnnotationColorBar* annotationColorBar,
                           PaletteColorMapping* paletteColorMapping);

        // ADD_NEW_METHODS_HERE

    private slots:
        void applySelections();
        
        void annotationColorBarPositionModeEnumComboBoxItemActivated();
        
        void annotationCoordinateSpaceEnumComboBoxItemActivated();
        
        void colorBarItemActivated();
        
    private:
        MapSettingsColorBarWidget(const MapSettingsColorBarWidget&);

        MapSettingsColorBarWidget& operator=(const MapSettingsColorBarWidget&);
        
        QWidget* createDataNumericsSection();
        
        QWidget* createLocationPositionSection();
        
        void updateContentPrivate();
        
        void updateColorBarAttributes();
        
        AnnotationColorBar* m_colorBar;
        
        PaletteColorMapping* m_paletteColorMapping;
        
        EnumComboBoxTemplate* m_annotationColorBarPositionModeEnumComboBox;
        
        EnumComboBoxTemplate* m_annotationCoordinateSpaceEnumComboBox;
        
        EnumComboBoxTemplate* m_colorBarDataModeComboBox;
        
        QLabel* m_colorBarNumericFormatModeLabel;
        EnumComboBoxTemplate* m_colorBarNumericFormatModeComboBox;
        
        QLabel* m_colorBarDecimalsLabel;
        QSpinBox* m_colorBarDecimalsSpinBox;
        
        QLabel* m_colorBarNumericSubdivisionsLabel;
        QSpinBox* m_colorBarNumericSubdivisionsSpinBox;
        
        QCheckBox* m_showTickMarksCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_COLOR_BAR_WIDGET_H__
