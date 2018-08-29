#ifndef __PALETTECOLORMAPPING_H__
#define __PALETTECOLORMAPPING_H__

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

#include <set>

#include <AString.h>

#include "CaretObject.h"

#include "AnnotationColorBarNumericText.h"
#include "CaretColorEnum.h"
#include "NumericFormatModeEnum.h"
#include "PaletteColorBarValuesModeEnum.h"
#include "PaletteEnums.h"
#include "PaletteHistogramRangeModeEnum.h"
#include "PaletteInvertModeEnum.h"
#include "PaletteModifiedStatusEnum.h"
#include "PaletteThresholdOutlineDrawingModeEnum.h"
#include "PaletteThresholdRangeModeEnum.h"
#include "XmlException.h"

namespace caret {

    class AnnotationColorBar;
    class FastStatistics;
    class Palette;
    class XmlWriter;
    
    /**
     * Controls color mapping using a palette.
     */
    class PaletteColorMapping : public CaretObject {
        
    public:        
        PaletteColorMapping();
        
        PaletteColorMapping(const PaletteColorMapping& o);
        
        PaletteColorMapping& operator=(const PaletteColorMapping& o);
        
        bool operator==(const PaletteColorMapping& pcm) const;
        
        bool operator!=(const PaletteColorMapping& pcm) const { return !((*this) == pcm); }
        
        virtual ~PaletteColorMapping();
        
        void copy(const PaletteColorMapping& pcm,
                  const bool copyHistogramAttributesFlag);
        
    private:
        void copyHelper(const PaletteColorMapping& o,
                        const bool copyHistogramAttributesFlag);
        
        void initializeMembersPaletteColorMapping();
        
    public:
        void setupAnnotationColorBar(const FastStatistics* statistics,
                                     AnnotationColorBar* colorBar);

        void setupAnnotationColorBarNumericText(const FastStatistics* statistics,
                                                AnnotationColorBar* colorBar);
        
        void writeAsXML(XmlWriter& xmlWriter);
        
        AString encodeInXML();
        
        void decodeFromStringXML(const AString& xml);
        
        float getAutoScalePercentageNegativeMaximum() const;
        
        void setAutoScalePercentageNegativeMaximum(const float autoScalePercentageNegativeMaximum);
        
        float getAutoScalePercentageNegativeMinimum() const;
        
        void setAutoScalePercentageNegativeMinimum(const float autoScalePercentageNegativeMinimum);
        
        float getAutoScalePercentagePositiveMaximum() const;
        
        void setAutoScalePercentagePositiveMaximum(const float autoScalePercentagePositiveMaximum);
        
        float getAutoScalePercentagePositiveMinimum() const;
        
        void setAutoScalePercentagePositiveMinimum(const float autoScalePercentagePositiveMinimum);
        
        float getAutoScaleAbsolutePercentageMinimum() const;
        
        void setAutoScaleAbsolutePercentageMinimum(const float autoScaleAbsolutePercentageMinimum);
        
        float getAutoScaleAbsolutePercentageMaximum() const;
        
        void setAutoScaleAbsolutePercentageMaximum(const float autoScaleAbsolutePercentageMaximum);
        
        bool isDisplayNegativeDataFlag() const;
        
        void setDisplayNegativeDataFlag(const bool displayNegativeDataFlag);
        
        bool isDisplayPositiveDataFlag() const;
        
        void setDisplayPositiveDataFlag(const bool displayPositiveDataFlag);
        
        bool isDisplayZeroDataFlag() const;
        
        void setDisplayZeroDataFlag(const bool displayZeroDataFlag);
        
        bool isInterpolatePaletteFlag() const;
        
        void setInterpolatePaletteFlag(const bool interpolatePaletteFlag);
        
        PaletteInvertModeEnum::Enum getInvertedMode() const;
        
        void setInvertedMode(const PaletteInvertModeEnum::Enum invertedMode);
        
        PaletteScaleModeEnum::Enum getScaleMode() const;
        
        void setScaleMode(const PaletteScaleModeEnum::Enum scaleMode);
        
        AString getSelectedPaletteName() const;
        
        void setSelectedPaletteName(const AString& selectedPaletteName);
        
        const Palette* getPalette() const;
        
        void setSelectedPaletteToPsych();
        
        void setSelectedPaletteToPsychNoNone();
        
        void setSelectedPaletteToOrangeYellow();
        
        void setSelectedPaletteToGrayInterpolated();
        
        float getUserScaleNegativeMaximum() const;
        
        void setUserScaleNegativeMaximum(const float userScaleNegativeMaximum);
        
        float getUserScaleNegativeMinimum() const;
        
        void setUserScaleNegativeMinimum(const float userScaleNegativeMinimum);
        
        float getUserScalePositiveMaximum() const;
        
        void setUserScalePositiveMaximum(const float userScalePositiveMaximum);
        
        float getUserScalePositiveMinimum() const;
        
        void setUserScalePositiveMinimum(const float userScalePositiveMinimum);
        
        float getThresholdMappedAverageAreaMinimum() const;
        
        void setThresholdMappedAverageAreaMinimum(const float thresholdMappedAverageAreaMinimum);
        
        float getThresholdMappedAverageAreaMaximum() const;
        
        void setThresholdMappedAverageAreaMaximum(const float thresholdMappedAverageAreaPositive);
        
        float getThresholdMappedMinimum() const;
        
        void setThresholdMappedMinimum(const float thresholdMappedMinimum);
        
        float getThresholdMappedMaximum() const;
        
        void setThresholdMappedMaximum(const float thresholdMappedPositive);
        
        float getThresholdNormalMinimum() const;
        
        void setThresholdNormalMinimum(const float thresholdNormalMinimum);
        
        float getThresholdNormalMaximum() const;
        
        void setThresholdNormalMaximum(const float thresholdNormalPositive);
        
        float getThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType) const;
        
        float getThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType) const;
        
        void setThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                 const float thresholdMinimum);
        
        void setThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                 const float thresholdMaximum);
        
        PaletteThresholdTestEnum::Enum getThresholdTest() const;
        
        void setThresholdTest(const PaletteThresholdTestEnum::Enum thresholdTest);
        
        PaletteThresholdTypeEnum::Enum getThresholdType() const;
        
        void setThresholdType(const PaletteThresholdTypeEnum::Enum thresholdType);
        
        PaletteThresholdRangeModeEnum::Enum getThresholdRangeMode() const;
        
        void setThresholdRangeMode(const PaletteThresholdRangeModeEnum::Enum rangeMode);
        
        AString getThresholdDataName() const;
        
        void setThresholdDataName(const AString& thresholdDataName);
        
        bool isShowThresholdFailureInGreen() const;
        
        void setShowThresholdFailureInGreen(const bool showInGreenFlag);
        
        bool isThresholdNegMinPosMaxLinked() const;
        
        void setThresholdNegMinPosMaxLinked(const bool linked);
        
        PaletteHistogramRangeModeEnum::Enum getHistogramRangeMode() const;
        
        void setHistogramRangeMode(const PaletteHistogramRangeModeEnum::Enum histogramRangeMode);
        
        bool isHistogramBarsVisible() const;
        
        void setHistogramBarsVisible(const bool histogramBarsVisible);
        
        bool isHistogramEnvelopeVisible() const;
        
        void setHistogramEnvelopeVisible(const bool histogramEnvelopeVisible);
        
        CaretColorEnum::Enum getHistogramBarsColor() const;
        
        void setHistogramBarsColor(const CaretColorEnum::Enum histogramBarsColor);
        
        CaretColorEnum::Enum getHistogramEnvelopeColor() const;
        
        void setHistogramEnvelopeColor(const CaretColorEnum::Enum histogramEnvelopeColor);
        
        float getHistogramEnvelopeLineWidthPercentage() const;
        
        void setHistogramEnvelopeLineWidthPercentage(const float lineWidthPercentage);
        
        int32_t getHistogramNumberOfBuckets() const;
        
        void setHistogramNumberOfBuckets(const int32_t histogramNumberOfBuckets);
    
        NumericFormatModeEnum::Enum getColorBarNumericFormatMode() const;
        
        int32_t getColorBarPrecisionDigits() const;
        
        int32_t getColorBarNumericSubdivisionCount() const;
        
        void setColorBarNumericFormatMode(const NumericFormatModeEnum::Enum colorBarNumericFormatMode);
        
        void setColorBarPrecisionDigits(const int32_t colorBarPrecisionDigits);
        
        void setColorBarNumericSubdivisionCount(const int32_t colroBarNumericSubdivisionCount);
        
        PaletteColorBarValuesModeEnum::Enum getColorBarValuesMode() const;
        
        void setColorBarValuesMode(const PaletteColorBarValuesModeEnum::Enum colorBarValuesMode);
        
        bool isColorBarShowTickMarksSelected() const;
        
        void setColorBarShowTickMarksSelected(const bool selected);
        
        PaletteThresholdOutlineDrawingModeEnum::Enum getThresholdOutlineDrawingMode() const;
        
        void setThresholdOutlineDrawingMode(const PaletteThresholdOutlineDrawingModeEnum::Enum drawingMode);
        
        CaretColorEnum::Enum getThresholdOutlineDrawingColor() const;
        
        void setThresholdOutlineDrawingColor(const CaretColorEnum::Enum color);

        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
        void setSceneModified();
        
        PaletteModifiedStatusEnum::Enum getModifiedStatus() const;
        
        void mapDataToPaletteNormalizedValues(const FastStatistics* statistics,
                                              const float* dataValues,
                                              float* normalizedValuesOut,
                                              const int64_t numberOfData) const;
        
        void getPaletteColorBarScaleText(const FastStatistics* statistics,
                                         std::vector<AnnotationColorBarNumericText*>& colorBarNumericTextOut) const;
        
        /** A positive value near zero - may be zero! */
        static const float SMALL_POSITIVE;
        
        /** A negative value near zero - may be zero! */
        static const float SMALL_NEGATIVE;
        
        /** TSC: the excluded zone of normalization is a separate issue to zero detection in the data
          * specifically, it is a HACK, in order for palettes to be able to specify a special color for data that is 0, which is not involved in color interpolation */
        static const float PALETTE_ZERO_COLOR_ZONE;
    private:
        bool isZeroNumericText(const AString& numericText) const;
        
        PaletteScaleModeEnum::Enum scaleMode;
        
        float autoScalePercentageNegativeMaximum;
        
        float autoScalePercentageNegativeMinimum;
        
        float autoScalePercentagePositiveMinimum;
        
        float autoScalePercentagePositiveMaximum;
        
        float autoScaleAbsolutePercentageMinimum;
        
        float autoScaleAbsolutePercentageMaximum;
        
        float userScaleNegativeMaximum;
        
        float userScaleNegativeMinimum;
        
        float userScalePositiveMinimum;
        
        float userScalePositiveMaximum;
        
        AString selectedPaletteName;
        
        bool interpolatePaletteFlag;
        
        PaletteInvertModeEnum::Enum invertedMode = PaletteInvertModeEnum::OFF;
        
        bool displayPositiveDataFlag;
        
        bool displayZeroDataFlag;
        
        bool displayNegativeDataFlag;
        
        PaletteThresholdTypeEnum::Enum thresholdType;
        
        PaletteThresholdTestEnum::Enum thresholdTest;
        
        PaletteThresholdRangeModeEnum::Enum thresholdRangeMode;
        
        float thresholdNormalMinimum;
        
        float thresholdNormalMaximum;
        
        float thresholdMappedMinimum;
        
        float thresholdMappedMaximum;
        
        float thresholdMappedAverageAreaMinimum;
        
        float thresholdMappedAverageAreaMaximum;
        
        AString thresholdDataName;
        
        bool thresholdShowFailureInGreen;
        
        bool thresholdNegMinPosMaxLinked;
        
        PaletteHistogramRangeModeEnum::Enum histogramRangeMode;
        
        bool histogramBarsVisible;

        bool histogramEnvelopeVisible;
        
        CaretColorEnum::Enum histogramBarsColor;
        
        CaretColorEnum::Enum histogramEnvelopeColor;
        
        float histogramEnvelopeLineWidthPercentage;
        
        int32_t histogramNumberOfBuckets;
        
        NumericFormatModeEnum::Enum colorBarNumericFormatMode;
        
        int32_t colorBarPrecisionDigits;
        
        int32_t colorBarNumericSubdivisionCount;
        
        PaletteColorBarValuesModeEnum::Enum colorBarValuesMode;
    
        bool colorBarShowTickMarksSelected;
        
        PaletteThresholdOutlineDrawingModeEnum::Enum thresholdOutlineDrawingMode;
        
        CaretColorEnum::Enum thresholdOutlineDrawingColor;
        
        /**Tracks modification, DO NOT copy */
        PaletteModifiedStatusEnum::Enum modifiedStatus;
        
        /** keeps missing palettes from being logged more than once */
        static std::set<AString> s_missingPaletteNames;
    };

#ifdef __PALETTE_COLOR_MAPPING_DECLARE__
    const float PaletteColorMapping::SMALL_POSITIVE = 0.0;
    const float PaletteColorMapping::SMALL_NEGATIVE = 0.0;
    const float PaletteColorMapping::PALETTE_ZERO_COLOR_ZONE = 0.00001f;
    std::set<AString> PaletteColorMapping::s_missingPaletteNames;
#endif // __PALETTE_COLOR_MAPPING_DECLARE__
    
} // namespace

#endif // __PALETTECOLORMAPPING_H__
