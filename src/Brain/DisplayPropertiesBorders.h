#ifndef __DISPLAY_PROPERTIES_BORDERS__H_
#define __DISPLAY_PROPERTIES_BORDERS__H_

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

#include "BrainConstants.h"
#include "BorderDrawingTypeEnum.h"
#include "CaretColorEnum.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"
#include "FeatureColoringTypeEnum.h"

namespace caret {

    class DisplayPropertiesBorders : public DisplayProperties {
        
    public:
        DisplayPropertiesBorders();
        
        virtual ~DisplayPropertiesBorders();

        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayStatus);
        
        bool isContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const;
        
        void setContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool contralateralDisplayStatus);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                             const DisplayGroupEnum::Enum displayGroup);
        
        float getPointSize(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const;
        
        void setPointSize(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const float pointSize);
        
        float getLineWidth(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const;
        
        void setLineWidth(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const float lineWidth);
        
        BorderDrawingTypeEnum::Enum getDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                                   const int32_t tabIndex) const;
        
        void setDrawingType(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const BorderDrawingTypeEnum::Enum drawingType);
        
        FeatureColoringTypeEnum::Enum getColoringType(const DisplayGroupEnum::Enum displayGroup,
                                                   const int32_t tabIndex) const;
        
        void setColoringType(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const FeatureColoringTypeEnum::Enum drawingType);
        
        CaretColorEnum::Enum getStandardColorType(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex) const;
        
        void setStandardColorType(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const CaretColorEnum::Enum caretColor);
        
        bool isUnstretchedLinesEnabled(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const;
        
        void setUnstretchedLinesEnabled(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool unstretchedLinesEnabled);
        
        float getUnstretchedLinesLength(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const;
        
        void setUnstretchedLinesLength(const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        const float unstretchedLinesLength);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        DisplayPropertiesBorders(const DisplayPropertiesBorders&);

        DisplayPropertiesBorders& operator=(const DisplayPropertiesBorders&);
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_contralateralDisplayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_contralateralDisplayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_pointSizeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_pointSizeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_lineWidthInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_lineWidthInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        BorderDrawingTypeEnum::Enum m_drawingTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        BorderDrawingTypeEnum::Enum m_drawingTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        FeatureColoringTypeEnum::Enum m_coloringTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        FeatureColoringTypeEnum::Enum m_coloringTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CaretColorEnum::Enum m_standardColorTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        CaretColorEnum::Enum m_standardColorTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_unstretchedLinesStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_unstretchedLinesStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_unstretchedLinesLengthInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_unstretchedLinesLengthInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };
    
#ifdef __DISPLAY_PROPERTIES_BORDERS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_BORDERS_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_BORDERS__H_
