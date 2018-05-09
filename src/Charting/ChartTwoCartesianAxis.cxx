
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __CHART_TWO_CARTESIAN_AXIS_DECLARE__
#include "ChartTwoCartesianAxis.h"
#undef __CHART_TWO_CARTESIAN_AXIS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartScaleAutoRanging.h"
#include "EventChartTwoAxisGetDataRange.h"
#include "EventManager.h"
#include "MathFunctions.h"
#include "NumericTextFormatting.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianAxis 
 * \brief Chart Two Cartesian Axis Attributes.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param axisLocation
 *     Location of the axis (left, right, bottom, top)
 * @param parentChartOverlaySet
 *     Parent of this axis.
 */
ChartTwoCartesianAxis::ChartTwoCartesianAxis(const ChartTwoOverlaySet* parentChartOverlaySet,
                                             const ChartAxisLocationEnum::Enum axisLocation)
: CaretObject(),
SceneableInterface(),
m_parentChartOverlaySet(parentChartOverlaySet),
m_axisLocation(axisLocation)
{
    /*
     * Note: Parent chart overlay set is used for sending an event
     * to the parent to get the range of data.  Note that parent
     * chart overlay is in 'Brain' module which we have no access.
     */
    CaretAssert(m_parentChartOverlaySet);
    
    m_showTickmarks = true;
    m_showLabel     = true;
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_displayedByUser",
                          &m_displayedByUser);
    m_sceneAssistant->add("m_userScaleMinimumValue",
                          &m_userScaleMinimumValue);
    m_sceneAssistant->add("m_userScaleMaximumValue",
                          &m_userScaleMaximumValue);
    m_sceneAssistant->add("m_axisLabelsStepValue",
                          &m_axisLabelsStepValue);
    m_sceneAssistant->add("m_userDigitsRightOfDecimal",
                          &m_userDigitsRightOfDecimal);
    
    m_sceneAssistant->add<ChartTwoAxisScaleRangeModeEnum,ChartTwoAxisScaleRangeModeEnum::Enum>("m_scaleRangeMode",
                                                                                               &m_scaleRangeMode);
    m_sceneAssistant->add<CaretUnitsTypeEnum,CaretUnitsTypeEnum::Enum>("m_units",
                                                                       &m_units);
    m_sceneAssistant->add<NumericFormatModeEnum,NumericFormatModeEnum::Enum>("m_userNumericFormat",
                                                                             &m_userNumericFormat);
    m_sceneAssistant->add<ChartTwoNumericSubdivisionsModeEnum,ChartTwoNumericSubdivisionsModeEnum::Enum>("m_numericSubdivsionsMode",
                                                                             &m_numericSubdivsionsMode);
    m_sceneAssistant->add("m_userNumberOfSubdivisions",
                          &m_userNumberOfSubdivisions);
    m_sceneAssistant->add("m_enabledByChart",
                          &m_enabledByChart);
    m_sceneAssistant->add("m_showTickmarks",
                          &m_showTickmarks);
    m_sceneAssistant->add("m_showLabel",
                          &m_showLabel);
    m_sceneAssistant->add("m_titleOverlayIndex",
                          &m_titleOverlayIndex);
    m_sceneAssistant->add("m_labelTextSize",
                          &m_labelTextSize);
    m_sceneAssistant->add("m_numericsTextSize",
                          &m_numericsTextSize);
    m_sceneAssistant->add("m_numericsTextDisplayed",
                          &m_numericsTextDisplayed);
    m_sceneAssistant->add("m_numericsTextRotated",
                          &m_numericsTextRotated);
    m_sceneAssistant->add("m_paddingSize",
                          &m_paddingSize);
}

/**
 * Destructor.
 */
ChartTwoCartesianAxis::~ChartTwoCartesianAxis()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoCartesianAxis::ChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_axisLocation(obj.m_axisLocation)
{
    this->copyHelperChartTwoCartesianAxis(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoCartesianAxis&
ChartTwoCartesianAxis::operator=(const ChartTwoCartesianAxis& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoCartesianAxis(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoCartesianAxis::copyHelperChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj)
{
    CaretAssert(m_axisLocation == obj.m_axisLocation);
    
    m_titleOverlayIndex         = obj.m_titleOverlayIndex;
    m_displayedByUser           = obj.m_displayedByUser;
    m_userScaleMinimumValue     = obj.m_userScaleMinimumValue;
    m_userScaleMaximumValue     = obj.m_userScaleMaximumValue;
    m_axisLabelsStepValue       = obj.m_axisLabelsStepValue;
    m_userDigitsRightOfDecimal  = obj.m_userDigitsRightOfDecimal;
    m_scaleRangeMode            = obj.m_scaleRangeMode;
    m_units                     = obj.m_units;
    m_userNumericFormat         = obj.m_userNumericFormat;
    m_numericSubdivsionsMode    = obj.m_numericSubdivsionsMode;
    m_userNumberOfSubdivisions  = obj.m_userNumberOfSubdivisions;
    m_enabledByChart            = obj.m_enabledByChart;
    m_showTickmarks             = obj.m_showTickmarks;
    m_showLabel                 = obj.m_showLabel;
    m_labelTextSize             = obj.m_labelTextSize;
    m_numericsTextSize          = obj.m_numericsTextSize;
    m_numericsTextDisplayed     = obj.m_numericsTextDisplayed;
    m_numericsTextRotated       = obj.m_numericsTextRotated;
    m_paddingSize               = obj.m_paddingSize;
    limitUserScaleMinMaxToValidRange();
}

/**
 * @return The axis location (left, right, bottom, top)
 */
ChartAxisLocationEnum::Enum
ChartTwoCartesianAxis::getAxisLocation() const
{
    return m_axisLocation;
}

/**
 * @return display the axis selected by user
 */
bool
ChartTwoCartesianAxis::isDisplayedByUser() const
{
    return m_displayedByUser;
}

/**
 * Set display the axis by user
 * @param displayedByUser
 *    New value for display the axis
 */
void
ChartTwoCartesianAxis::setDisplayedByUser(const bool displayedByUser)
{
    m_displayedByUser = displayedByUser;
}
/**
 * Get the range of data for this axis.
 *
 * @param rangeMinimumOut
 *     Minimum value allowed.
 * @param rangeMaximumOut
 *     Maximum value allowed.
 */
void
ChartTwoCartesianAxis::getDataRange(float& rangeMinimumOut,
                                    float& rangeMaximumOut) const
{
    EventChartTwoAxisGetDataRange rangeEvent(m_parentChartOverlaySet,
                                             m_axisLocation);
    EventManager::get()->sendEvent(rangeEvent.getPointer());
    
    if ( ! rangeEvent.getMinimumAndMaximumValues(rangeMinimumOut,
                                                 rangeMaximumOut)) {
        rangeMinimumOut = 0.0f;
        rangeMaximumOut = 0.0f;
    }
}

/**
 * Limit the user scale min/max values to be within the valid range.
 */
void
ChartTwoCartesianAxis::limitUserScaleMinMaxToValidRange()
{
//    float minimumValue = 0.0f;
//    float maximumValue = 0.0f;
//    getDataRange(minimumValue,
//                 maximumValue);
//    
//    m_userScaleMinimumValue = MathFunctions::limitRange(m_userScaleMinimumValue,
//                                                        minimumValue,
//                                                        maximumValue);
//    m_userScaleMaximumValue = MathFunctions::limitRange(m_userScaleMaximumValue,
//                                                        minimumValue,
//                                                        maximumValue);
}

/**
 * @return User's digits right of decimal point.
 */
int32_t
ChartTwoCartesianAxis::getUserDigitsRightOfDecimal() const
{
    return m_userDigitsRightOfDecimal;
}

/**
 * Set user's digits right of decimal point.
 *
 * @param digitsRightOfDecimal
 *    Numer of digits right of decimal point.
 */
void
ChartTwoCartesianAxis::setUserDigitsRightOfDecimal(const int32_t digitsRightOfDecimal)
{
    m_userDigitsRightOfDecimal = digitsRightOfDecimal;
}


/**
 * @return User's number of subdivisions.
 */
int32_t
ChartTwoCartesianAxis::getUserNumberOfSubdivisions() const
{
    return m_userNumberOfSubdivisions;
}

/**
 * Set User's number of subdivisions
 * @param numberOfSubdivisions
 *    New value for Number of subdivisions.
 */
void
ChartTwoCartesianAxis::setUserNumberOfSubdivisions(const int32_t numberOfSubdivisions)
{
    m_userNumberOfSubdivisions = numberOfSubdivisions;
}

/**
 * @return numeric subdivisions mode
 */
ChartTwoNumericSubdivisionsModeEnum::Enum
ChartTwoCartesianAxis::getNumericSubdivsionsMode() const
{
    return m_numericSubdivsionsMode;
}

/**
 * Set numeric subdivisions mode
 *
 * @param numericSubdivsionsMode
 *     New value for numeric subdivisions mode
 */
void
ChartTwoCartesianAxis::setNumericSubdivsionsMode(const ChartTwoNumericSubdivisionsModeEnum::Enum numericSubdivsionsMode)
{
    m_numericSubdivsionsMode = numericSubdivsionsMode;
}

/**
 * @return User scale's minimum value
 */
float
ChartTwoCartesianAxis::getUserScaleMinimumValue() const
{
    return m_userScaleMinimumValue;
}

/**
 * @return User scale's maximum value
 */
float
ChartTwoCartesianAxis::getUserScaleMaximumValue() const
{
    return m_userScaleMaximumValue;
}

/**
 * Set User scale's maximum value
 * @param userScaleMaximumValue
 *    New value for User scale's maximum value
 */
void
ChartTwoCartesianAxis::setUserScaleMaximumValue(const float userScaleMaximumValue)
{
    m_userScaleMaximumValue = userScaleMaximumValue;
    limitUserScaleMinMaxToValidRange();
}

/**
 * Set User scale's minimum value
 * @param userScaleMinimumValue
 *    New value for User scale's minimum value
 */
void
ChartTwoCartesianAxis::setUserScaleMinimumValue(const float userScaleMinimumValue)
{
    m_userScaleMinimumValue = userScaleMinimumValue;
    limitUserScaleMinMaxToValidRange();
}

/**
 * @return Is the axis enabled because a chart is using it
 */
bool
ChartTwoCartesianAxis::isEnabledByChart() const
{
    return m_enabledByChart;
}

/**
 * Set the axis enabled because a chart is using it
 *
 * @param enabled
 *    New enabled status
 */
void
ChartTwoCartesianAxis::setEnabledByChart(const bool enabled)
{
    m_enabledByChart = enabled;
}

/**
 * @return Show axis tickmarks
 */
bool
ChartTwoCartesianAxis::isShowTickmarks() const
{
    return m_showTickmarks;
}

/**
 * Set Show axis tickmarks
 * @param showTickmarks
 *    New value for Show axis tickmarks
 */
void
ChartTwoCartesianAxis::setShowTickmarks(const bool showTickmarks)
{
    m_showTickmarks = showTickmarks;
}

/**
 * @return show axis label
 */
bool
ChartTwoCartesianAxis::isShowLabel() const
{
    return m_showLabel;
}

/**
 * Set show axis label
 * @param showLabel
 *    New value for show axis label
 */
void
ChartTwoCartesianAxis::setShowLabel(const bool showLabel)
{
    m_showLabel = showLabel;
}

/**
 * @return Scale Range Mode
 */
ChartTwoAxisScaleRangeModeEnum::Enum
ChartTwoCartesianAxis::getScaleRangeMode() const
{
    return m_scaleRangeMode;
}

/**
 * Set Scale Range Mode
 *
 * @param scaleRangeMode
 *    New value for Scale Range Mode
 */
void
ChartTwoCartesianAxis::setScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode)
{
    m_scaleRangeMode = scaleRangeMode;
}

/**
 * @return Axis units
 */
CaretUnitsTypeEnum::Enum
ChartTwoCartesianAxis::getUnits() const
{
    return m_units;
}

/**
 * Set Axis units
 *
 * @param units
 *    New value for Axis units
 */
void
ChartTwoCartesianAxis::setUnits(const CaretUnitsTypeEnum::Enum units)
{
    m_units = units;
}

/**
 * @return User's umeric format mode
 */
NumericFormatModeEnum::Enum
ChartTwoCartesianAxis::getUserNumericFormat() const
{
    return m_userNumericFormat;
}

/**
 * Set User's Numeric format mode
 *
 * @param numericFormat
 *    New value for Numeric format mode
 */
void
ChartTwoCartesianAxis::setUserNumericFormat(const NumericFormatModeEnum::Enum numericFormat)
{
    m_userNumericFormat = numericFormat;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianAxis::toString() const
{
    return "ChartTwoCartesianAxis";
}

/**
 * @return Index of overlay that supplies the label.
 *
 * @param maximumNumberOfOverlays
 *     Maximum number of allowable overlays.
 */
int32_t
ChartTwoCartesianAxis::getLabelOverlayIndex(const int32_t maximumNumberOfOverlays) const
{
    if (m_titleOverlayIndex < 0) {
        m_titleOverlayIndex = 0;
    }
    else if (m_titleOverlayIndex >= maximumNumberOfOverlays) {
        m_titleOverlayIndex = 0;
    }
    
    return m_titleOverlayIndex;
}

/**
 * Set the index of the overlay that supplies the title.
 * 
 * @param labelOverlayIndex
 *     New value for label overlay index.
 */
void
ChartTwoCartesianAxis::setLabelOverlayIndex(const int32_t labelOverlayIndex)
{
    m_titleOverlayIndex = labelOverlayIndex;
}

/**
 * @return size of label text
 */
float
ChartTwoCartesianAxis::getLabelTextSize() const
{
    return m_labelTextSize;
}

/**
 * Set size of label text
 *
 * @param labelTextSize
 *    New value for size of label text
 */
void
ChartTwoCartesianAxis::setLabelTextSize(const float labelTextSize)
{
    m_labelTextSize = labelTextSize;
}

/**
 * @return size of numerics text
 */
float
ChartTwoCartesianAxis::getNumericsTextSize() const
{
    return m_numericsTextSize;
}

/**
 * Set size of numerics text
 *
 * @param numericsTextSize
 *    New value for size of numerics text
 */
void
ChartTwoCartesianAxis::setNumericsTextSize(const float numericsTextSize)
{
    m_numericsTextSize = numericsTextSize;
}

/**
 * @return display numeric text in scale
 */
bool
ChartTwoCartesianAxis::isNumericsTextDisplayed() const
{
    return m_numericsTextDisplayed;
}

/**
 * Set display numeric text in scale
 *
 * @param numericsTextDisplayed
 *    New value for display numeric text in scale
 */
void
ChartTwoCartesianAxis::setNumericsTextDisplayed(const bool numericsTextDisplayed)
{
    m_numericsTextDisplayed = numericsTextDisplayed;
}

/**
 * @return rotate numeric text
 */
bool
ChartTwoCartesianAxis::isNumericsTextRotated() const
{
    return m_numericsTextRotated;
}

/**
 * Set rotate numeric text
 *
 * @param numericsTextRotated
 *    New value for rotate numeric text
 */
void
ChartTwoCartesianAxis::setNumericsTextRotated(const bool numericsTextRotated)
{
    m_numericsTextRotated = numericsTextRotated;
}

/**
 * @return size of padding
 */
float
ChartTwoCartesianAxis::getPaddingSize() const
{
    return m_paddingSize;
}

/**
 * Set size of padding
 *
 * @param paddingSize
 *    New value for size of padding
 */
void
ChartTwoCartesianAxis::setPaddingSize(const float paddingSize)
{
    m_paddingSize = paddingSize;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartTwoCartesianAxis::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianAxis",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartTwoCartesianAxis::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    // may not be in older scenes
    m_displayedByUser       = true;
    m_numericsTextDisplayed = true;
    m_numericsTextRotated   = false;
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Given the bounds of the data, determine the auto range minimum and maximum values.
 *
 * @param minimumValue
 *     Minimum data value
 * @param maximumValue
 *     Maximum data value
 * @param minimumOut
 *     Output minimum value for autoranging.
 * @param maximumOut
 *     Output maximum value for autoranging.
 * @param stepValueOut
 *     Output step value for scale.
 * @param digitsRightOfDecimalOut
 *     Output with digits right of decimal.
 * @return
 *     True if output values are valid, else false.
 */
bool
ChartTwoCartesianAxis::getAutoRangeMinimumAndMaximum(const float minimumValue,
                                                     const float maximumValue,
                                                     float& minimumOut,
                                                     float& maximumOut,
                                                     float& stepValueOut,
                                                     int32_t& digitsRightOfDecimalOut) const
{
    float minValue = minimumValue;
    float maxValue = maximumValue;
    
    if (maxValue > minValue) {
        double scaleStep = 0.0;
        double scaleMin  = 0.0;
        double scaleMax  = 0.0;
        int32_t digitsRightOfDecimal = 0;
        
        ChartScaleAutoRanging::createAutoScale(minValue,
                                               maxValue,
                                               scaleMin,
                                               scaleMax,
                                               scaleStep,
                                               digitsRightOfDecimal);
        minimumOut   = scaleMin;
        maximumOut   = scaleMax;
        stepValueOut = scaleStep;
        digitsRightOfDecimalOut   = digitsRightOfDecimal;
        
        //m_rangeMinimumValue = minimumOut;
        //m_rangeMaximumValue = maximumOut;
        
        return true;
    }
    
    return false;
}

/**
 * Get the axis scale text values and their positions for drawing the scale.
 *
 * @param minimumDataValue
 *     Minimum data value
 * @param maximumDataValue
 *     Maximum data value
 * @param axisLength
 *     Length of axis (no specific unit type is assumed)
 * @param minimumOut
 *     Output minimum value for autoranging.
 * @param maximumOut
 *     Output maximum value for autoranging.
 * @param scaleValuesOffsetInPixelsOut
 *     Output containing offset in pixels for the scale values.
 * @param scaleValuesOut
 *     Output containing text for scale values.
 * @return
 *     True if output data is valid, else false.
 */
bool
ChartTwoCartesianAxis::getScaleValuesAndOffsets(const float minimumDataValue,
                                                const float maximumDataValue,
                                                const float axisLength,
                                                float& minimumOut,
                                                float& maximumOut,
                                                std::vector<float>& scaleValuesOffsetInPixelsOut,
                                                std::vector<AString>& scaleValuesOut) const
{
    float minimumValue = minimumDataValue;
    float maximumValue = maximumDataValue;
    
    minimumOut = 0.0;
    maximumOut = 0.0;
    scaleValuesOffsetInPixelsOut.clear();
    scaleValuesOut.clear();
    
    if (axisLength <= 0.0) {
        CaretAssert(0);
        return false;
    }
    
    float labelsStart = 0.0;
    float labelsEnd   = 0.0;
    float labelsStep  = 1.0;
    int32_t labelsDigitsRightOfDecimal = 0;
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
            switch (m_axisLocation) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    minimumValue = m_userScaleMinimumValue;
                    maximumValue = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    minimumValue = m_userScaleMinimumValue;
                    maximumValue = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    minimumValue = m_userScaleMinimumValue;
                    maximumValue = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    minimumValue = m_userScaleMinimumValue;
                    maximumValue = m_userScaleMaximumValue;
                    break;
            }
            break;
    }

    if ( ! getAutoRangeMinimumAndMaximum(minimumValue,
                                         maximumValue,
                                         labelsStart,
                                         labelsEnd,
                                         labelsStep,
                                         labelsDigitsRightOfDecimal)) {
        return false;
    }
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            m_userScaleMinimumValue = labelsStart;
            m_userScaleMaximumValue = labelsEnd;
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
        {
            const double range = maximumDataValue - minimumDataValue;
            if (range > 0.0) {
                const int32_t numSteps = MathFunctions::round((labelsEnd - labelsStart) / labelsStep);
                if (numSteps > 0) {
                    labelsStart = minimumDataValue;
                    labelsEnd   = maximumDataValue;
                    labelsStep  = range / numSteps;
                    m_userScaleMinimumValue = labelsStart;
                    m_userScaleMaximumValue = labelsEnd;
                }
            }
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
//            labelsStart = m_userScaleMinimumValue;
//            labelsEnd   = m_userScaleMaximumValue;
        {
            const double range = m_userScaleMaximumValue - m_userScaleMinimumValue;
            if (range > 0.0) {
                const int32_t numSteps = MathFunctions::round((labelsEnd - labelsStart) / labelsStep);
                if (numSteps > 0) {
                    labelsStart = m_userScaleMinimumValue;
                    labelsEnd   = m_userScaleMaximumValue;
                    labelsStep  = range / numSteps;
                    m_userScaleMinimumValue = labelsStart;
                    m_userScaleMaximumValue = labelsEnd;
                }
            }
        }
            break;
    }
    
    switch (m_numericSubdivsionsMode) {
        case ChartTwoNumericSubdivisionsModeEnum::AUTO:
            break;
        case ChartTwoNumericSubdivisionsModeEnum::USER:
        {
            const float labelsRange = labelsEnd - labelsStart;
            if (labelsRange <= 0.0) {
                return false;
            }
            const float dividend = (1.0 + m_userNumberOfSubdivisions);
            labelsStep = labelsRange / dividend;
        }
            break;
    }
    
    minimumOut = labelsStart;
    maximumOut = labelsEnd;
    
    /*
     * If the "labels end" or "labels start" value is not valid (infinity or not-a-number) there
     * are invalid values in the data and will cause the labels processing later
     * in this method to fail.  So, alert the user that there is a problem in
     * the data.
     *
     * A set is used to track those models for which the user has
     * already been alerted.  Otherwise, the alert message will be
     * displayed every time this method is called (which is many) and
     * the user will receive endless pop-ups.
     */
    if ( (! MathFunctions::isNumeric(labelsStart))
        || (! MathFunctions::isNumeric(labelsEnd))) {
        const AString msg("Invalid numbers (infinity or not-a-number) found when trying to create chart.  "
                          "Run \"wb_command -file-information\" on files being charted to find the file "
                          "that contains invalid data so that the file can be fixed.");
        CaretLogWarning(msg);
        return false;
    }
    
    float labelsRange = (labelsEnd - labelsStart);
    if (labelsRange <= 0.0) {
        return false;
    }
    
    const float tickLabelsStep = labelsStep;
    if (tickLabelsStep <= 0.0) {
        return false;
    }
    
    const float onePercentRange = labelsRange * 0.01f;
    
    std::vector<float> labelNumericValues;
    
    float labelValue  = labelsStart;
    while (labelValue <= labelsEnd) {
        float labelParametricValue = (labelValue - labelsStart) / labelsRange;
        
        float labelValueForText = labelValue;
        
        if (labelsRange >= 10.0) {
            /*
             * Is this the first label?
             */
            if (labelValue <= labelsStart) {
                /*
                 * Handles case when the minimum DATA value is just a little
                 * bit greater than the minimum value for axis labels such
                 * as in Data-Series data when the minimum data value is "1"
                 * and the minimum axis label value is "0".  Without this
                 * code no value is displayed at the left edge of the axis.
                 */
                if (labelParametricValue < 0.0) {
                    const float nextParametricValue = ((labelValue + tickLabelsStep) - labelsStart) / labelsRange;
                    if (nextParametricValue > 0.05) {
                        labelParametricValue = 0.0;
                        labelValueForText = labelsStart;
                    }
                }
            }
            
            if (labelParametricValue < 0.0) {
                if (labelParametricValue >= -0.01) {
                    labelParametricValue = 0.0;
                }
            }
            
            /*
             * Is this the last label?
             */
            if (labelValue >= labelsEnd) {
                /*
                 * Like above, ensures a value is displayed at the right
                 * edge of the axis.
                 */
                if (labelParametricValue > 1.0) {
                    const float prevParametricValue = ((labelValue - tickLabelsStep) - labelsStart) / labelsRange;
                    if (prevParametricValue < 0.95) {
                        labelParametricValue = 1.0;
                        labelValueForText = labelsEnd;
                    }
                }
            }
            
            if (labelParametricValue > 1.0) {
                if (labelParametricValue < 1.01) {
                    labelParametricValue = 1.0;
                }
            }
        }
        
        if ((labelParametricValue >= 0.0)
            && (labelParametricValue <= 1.0)) {
            const float labelPixelsPosition = axisLength * labelParametricValue;
            labelNumericValues.push_back(labelValueForText);
            scaleValuesOffsetInPixelsOut.push_back(labelPixelsPosition);
        }
        else {
            //            std::cout << "Label value=" << labelValue << " parametric=" << labelParametricValue << " failed." << std::endl;
        }
        
        labelValue  += tickLabelsStep;
        
        /*
         * It is possible that 'labelValue' may be slightly greater than 'labelsEnd'
         * for the last label which results in the last label not displayed.
         * So, if the 'labelValue' is slightly greater than 'labelsEnd', 
         * limit 'labelValue' so that the label at the end of the data range
         * is displayed.
         *
         * Example: labelValue = 73.9500046
         *          labelsEnd  = 73.9499969
         */
        if (labelValue > labelsEnd) {
            const float diff = labelValue - labelsEnd;
            if (diff < onePercentRange) {
                labelValue = labelsEnd;
            }
        }
    }
    
    const int32_t numValues = static_cast<int32_t>(labelNumericValues.size());
    if (numValues > 0) {
        scaleValuesOut.resize(numValues);
        NumericTextFormatting::formatValueRange(m_userNumericFormat,
                                                m_userDigitsRightOfDecimal,
                                                &labelNumericValues[0],
                                                &scaleValuesOut[0],
                                                labelNumericValues.size());
    }
    
    CaretAssert(scaleValuesOffsetInPixelsOut.size() == scaleValuesOut.size());
    return ( ! scaleValuesOut.empty());
}

