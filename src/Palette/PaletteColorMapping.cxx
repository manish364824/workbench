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

#include <cmath>
#include <sstream>

#include "CaretOMP.h"
#include "FastStatistics.h"
#include "MathFunctions.h"
#include "NumericTextFormatting.h"
//because the ROY_BIG_BL palette name is a constant defined in Palette.h
#include "Palette.h"
#define __PALETTE_COLOR_MAPPING_DECLARE__
#include "PaletteColorMapping.h"
#undef __PALETTE_COLOR_MAPPING_DECLARE__
#include "PaletteColorMappingSaxReader.h"
#include "PaletteColorMappingXmlElements.h"
#include "XmlSaxParser.h"
#include "XmlUtilities.h"
#include "XmlWriter.h"
#include <limits>

using namespace caret;


/**
 * Constructor.
 *
 */
PaletteColorMapping::PaletteColorMapping()
    : CaretObject()
{
    this->initializeMembersPaletteColorMapping();
}

/**
 * Destructor
 */
PaletteColorMapping::~PaletteColorMapping()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
PaletteColorMapping::PaletteColorMapping(const PaletteColorMapping& o)
    : CaretObject(o)
{
    this->initializeMembersPaletteColorMapping();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
PaletteColorMapping&
PaletteColorMapping::operator=(const PaletteColorMapping& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Copy the palette color mapping from the given palette
 * color mapping.
 * @param pcm
 *    Color mapping that is copied to this.
 */
void 
PaletteColorMapping::copy(const PaletteColorMapping& pcm)
{
    this->copyHelper(pcm);
    setModified();
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
PaletteColorMapping::copyHelper(const PaletteColorMapping& pcm)
{
    this->autoScalePercentageNegativeMaximum = pcm.autoScalePercentageNegativeMaximum;
    this->autoScalePercentageNegativeMinimum = pcm.autoScalePercentageNegativeMinimum;
    this->autoScalePercentagePositiveMaximum = pcm.autoScalePercentagePositiveMaximum;
    this->autoScalePercentagePositiveMinimum = pcm.autoScalePercentagePositiveMinimum;
    this->autoScaleAbsolutePercentageMaximum = pcm.autoScaleAbsolutePercentageMaximum;
    this->autoScaleAbsolutePercentageMinimum = pcm.autoScaleAbsolutePercentageMinimum;
    this->displayNegativeDataFlag = pcm.displayNegativeDataFlag;
    this->displayPositiveDataFlag = pcm.displayPositiveDataFlag;
    this->displayZeroDataFlag     = pcm.displayZeroDataFlag;
    this->interpolatePaletteFlag  = pcm.interpolatePaletteFlag;
    this->scaleMode = pcm.scaleMode;
    this->selectedPaletteName = pcm.selectedPaletteName;
    this->userScaleNegativeMaximum = pcm.userScaleNegativeMaximum;
    this->userScaleNegativeMinimum = pcm.userScaleNegativeMinimum;
    this->userScalePositiveMaximum = pcm.userScalePositiveMaximum;
    this->userScalePositiveMinimum = pcm.userScalePositiveMinimum;
    this->thresholdType = pcm.thresholdType;
    this->thresholdTest = pcm.thresholdTest;
    this->thresholdNormalMinimum = pcm.thresholdNormalMinimum;
    this->thresholdNormalMaximum = pcm.thresholdNormalMaximum;
    this->thresholdMappedMinimum= pcm.thresholdMappedMinimum;
    this->thresholdMappedMaximum = pcm.thresholdMappedMaximum;
    this->thresholdMappedAverageAreaMinimum = pcm.thresholdMappedAverageAreaMinimum;
    this->thresholdMappedAverageAreaMaximum = pcm.thresholdMappedAverageAreaMaximum;
    this->thresholdDataName = pcm.thresholdDataName;
    this->thresholdShowFailureInGreen = pcm.thresholdShowFailureInGreen;
    this->thresholdRangeMode = pcm.thresholdRangeMode;
    this->thresholdNegMinPosMaxLinked = pcm.thresholdNegMinPosMaxLinked;
    
    this->clearModified();
}

/**
 * Equality operator.
 * @param pcm
 *    Palette color mapping compared to 'this' palette color mapping.
 * @return
 *    True if their members are the same.
 */
bool
PaletteColorMapping::operator==(const PaletteColorMapping& pcm) const
{
    if ((this->autoScalePercentageNegativeMaximum == pcm.autoScalePercentageNegativeMaximum)
        && (this->autoScalePercentageNegativeMinimum == pcm.autoScalePercentageNegativeMinimum)
        && (this->autoScalePercentagePositiveMaximum == pcm.autoScalePercentagePositiveMaximum)
        && (this->autoScalePercentagePositiveMinimum == pcm.autoScalePercentagePositiveMinimum)
        && (this->autoScaleAbsolutePercentageMaximum == pcm.autoScaleAbsolutePercentageMaximum)
        && (this->autoScaleAbsolutePercentageMinimum == pcm.autoScaleAbsolutePercentageMinimum)
        && (this->displayNegativeDataFlag == pcm.displayNegativeDataFlag)
        && (this->displayPositiveDataFlag == pcm.displayPositiveDataFlag)
        && (this->displayZeroDataFlag     == pcm.displayZeroDataFlag)
        && (this->interpolatePaletteFlag  == pcm.interpolatePaletteFlag)
        && (this->scaleMode == pcm.scaleMode)
        && (this->selectedPaletteName == pcm.selectedPaletteName)
        && (this->userScaleNegativeMaximum == pcm.userScaleNegativeMaximum)
        && (this->userScaleNegativeMinimum == pcm.userScaleNegativeMinimum)
        && (this->userScalePositiveMaximum == pcm.userScalePositiveMaximum)
        && (this->userScalePositiveMinimum == pcm.userScalePositiveMinimum)
        && (this->thresholdType == pcm.thresholdType)
        && (this->thresholdTest == pcm.thresholdTest)
        && (this->thresholdNormalMinimum == pcm.thresholdNormalMinimum)
        && (this->thresholdNormalMaximum == pcm.thresholdNormalMaximum)
        && (this->thresholdMappedMinimum== pcm.thresholdMappedMinimum)
        && (this->thresholdMappedMaximum == pcm.thresholdMappedMaximum)
        && (this->thresholdMappedAverageAreaMinimum == pcm.thresholdMappedAverageAreaMinimum)
        && (this->thresholdMappedAverageAreaMaximum == pcm.thresholdMappedAverageAreaMaximum)
        && (this->thresholdDataName == pcm.thresholdDataName)
        && (this->thresholdShowFailureInGreen == pcm.thresholdShowFailureInGreen)
        && (this->thresholdRangeMode == pcm.thresholdRangeMode)
        && (this->thresholdNegMinPosMaxLinked == pcm.thresholdNegMinPosMaxLinked)) {
        return true;
    }
    
    return false;
}


void
PaletteColorMapping::initializeMembersPaletteColorMapping()
{
    this->scaleMode = PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE;
    this->autoScalePercentageNegativeMaximum = 98.0f;
    this->autoScalePercentageNegativeMinimum = 2.0f;
    this->autoScalePercentagePositiveMinimum = 2.0f;
    this->autoScalePercentagePositiveMaximum = 98.0f;
    this->autoScaleAbsolutePercentageMaximum = 98.0f;
    this->autoScaleAbsolutePercentageMinimum = 2.0f;
    this->userScaleNegativeMaximum = -100.0f;
    this->userScaleNegativeMinimum = 0.0f;
    this->userScalePositiveMinimum = 0.0f;
    this->userScalePositiveMaximum = 100.0f;
    this->selectedPaletteName = Palette::ROY_BIG_BL_PALETTE_NAME;
    this->interpolatePaletteFlag = true;
    this->displayPositiveDataFlag = true;
    this->displayZeroDataFlag = false;
    this->displayNegativeDataFlag = true;
    this->thresholdType = PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF;
    this->thresholdTest = PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE;
    this->thresholdNormalMinimum = -1.0f;
    this->thresholdNormalMaximum = 1.0f;
    this->thresholdMappedMinimum = -1.0f;
    this->thresholdMappedMaximum = 1.0f;
    this->thresholdMappedAverageAreaMinimum = -1.0f;
    this->thresholdMappedAverageAreaMaximum =  1.0f;
    this->thresholdDataName = "";
    this->thresholdShowFailureInGreen = false;
    this->thresholdRangeMode = PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP;
    this->thresholdNegMinPosMaxLinked = false;
    this->modifiedFlag = false;
}

/**
 * Write the object as XML.
 * @param xmlWriter - write to this->
 * @throws XmlException - If an error occurs.
 *
 */
void
PaletteColorMapping::writeAsXML(XmlWriter& xmlWriter)
                      
{
    XmlAttributes attributes;
    attributes.addAttribute(
                            PaletteColorMappingXmlElements::XML_ATTRIBUTE_VERSION_NUMBER,
                            PaletteColorMappingXmlElements::XML_VERSION_NUMBER);
    xmlWriter.writeStartElement(
                                PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING,
                                attributes);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_SCALE_MODE,
                                     PaletteScaleModeEnum::toName(this->scaleMode));
    float autoScaleValues[4] = {
        this->autoScalePercentageNegativeMaximum,
        this->autoScalePercentageNegativeMinimum,
        this->autoScalePercentagePositiveMinimum,
        this->autoScalePercentagePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES,
                                     autoScaleValues,
                                     4);
    
    float autoScaleAbsolutePercentageValues[2] = {
        this->autoScaleAbsolutePercentageMinimum,
        this->autoScaleAbsolutePercentageMaximum
    };
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_ABSOLUTE_PERCENTAGE_VALUES,
                                     autoScaleAbsolutePercentageValues,
                                     2);
    
    float userScaleValues[4] = {
        this->userScaleNegativeMaximum,
        this->userScaleNegativeMinimum,
        this->userScalePositiveMinimum,
        this->userScalePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_USER_SCALE_VALUES,
                                     userScaleValues,
                                     4);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_PALETTE_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->selectedPaletteName));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_INTERPOLATE,
                                     this->interpolatePaletteFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_POSITIVE,
                                     this->displayPositiveDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_ZERO,
                                     this->displayZeroDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_NEGATIVE,
                                     this->displayNegativeDataFlag);
    
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TEST,
                                     PaletteThresholdTestEnum::toName(this->thresholdTest));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TYPE,
                                     PaletteThresholdTypeEnum::toName(this->thresholdType));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_FAILURE_IN_GREEN,
                                     this->thresholdShowFailureInGreen);
    float normalValues[2] = {
        this->thresholdNormalMinimum,
        this->thresholdNormalMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NORMAL_VALUES,
                                     normalValues,
                                     2);
    
    float mappedValues[2] = {
        this->thresholdMappedMinimum,
        this->thresholdMappedMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_VALUES,
                                     mappedValues,
                                     2);
    
    float mappedAvgAreaValues[2] = {
        this->thresholdMappedAverageAreaMinimum,
        this->thresholdMappedAverageAreaMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES,
                                     mappedAvgAreaValues,
                                     2);
    
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_DATA_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->thresholdDataName));
    
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_RANGE_MODE,
                                     PaletteThresholdRangeModeEnum::toName(this->thresholdRangeMode));
    
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NEG_MIN_POS_MAX_LINKED,
                                     this->thresholdNegMinPosMaxLinked);
    
    xmlWriter.writeEndElement();
}

/**
 * Returns the XML representation of this object in a String.
 *
 * @return String containing XML.
 * @throws XmlException if an error occurs.
 *
 */
AString
PaletteColorMapping::encodeInXML()
           
{
    std::ostringstream str;
    XmlWriter xmlWriter(str);
    this->writeAsXML(xmlWriter);
    AString s = AString::fromStdString(str.str());
    return s;
}

/**
 * Decode this object from a String containing XML.
 * @param xml - String containing XML.
 * @throws XmlException If an error occurs.
 *
 */
void
PaletteColorMapping::decodeFromStringXML(const AString& xml)
           
{
    PaletteColorMappingSaxReader saxReader(this);
    XmlSaxParser* parser = XmlSaxParser::createXmlParser();
    try {
        parser->parseString(xml, &saxReader);
    }
    catch (XmlSaxParserException& e) {
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        std::ostringstream str;
        str << "Parse Error while reading PaletteColorMapping XML";
        if ((lineNum >= 0) && (colNum >= 0)) {
            str << " line/col ("
            << e.getLineNumber()
            << "/"
            << e.getColumnNumber()
            << ")";
        }
        str << ": "
        << e.whatString().toStdString();
        throw XmlException(AString::fromStdString(str.str()));
    }
    delete parser;
}

/**
 * Get auto scale percentage negative maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentageNegativeMaximum() const
{
    return autoScalePercentageNegativeMaximum;
}

/**
 * Set auto scale percentage negative maximum.
 * @param autoScalePercentageNegativeMaximum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentageNegativeMaximum(const float autoScalePercentageNegativeMaximum)
{
    if (this->autoScalePercentageNegativeMaximum != autoScalePercentageNegativeMaximum) {
        this->autoScalePercentageNegativeMaximum = autoScalePercentageNegativeMaximum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage negative minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentageNegativeMinimum() const
{
    return autoScalePercentageNegativeMinimum;
}

/**
 * Set auto scale percentage negative minimum.
 * @param autoScalePercentageNegativeMinimum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentageNegativeMinimum(const float autoScalePercentageNegativeMinimum)
{
    if (this->autoScalePercentageNegativeMinimum != autoScalePercentageNegativeMinimum) {
        this->autoScalePercentageNegativeMinimum = autoScalePercentageNegativeMinimum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentagePositiveMaximum() const
{
    return autoScalePercentagePositiveMaximum;
}

/**
 * Set auto scale percentage positive maximum.
 * @param autoScalePercentagePositiveMaximum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentagePositiveMaximum(const float autoScalePercentagePositiveMaximum)
{
    if (this->autoScalePercentagePositiveMaximum != autoScalePercentagePositiveMaximum) {
        this->autoScalePercentagePositiveMaximum = autoScalePercentagePositiveMaximum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage positive minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentagePositiveMinimum() const
{
    return autoScalePercentagePositiveMinimum;
}

/**
 * Set auto scale percentage positive minimum.
 * @param autoScalePercentagePositiveMinimum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentagePositiveMinimum(const float autoScalePercentagePositiveMinimum)
{
    if (this->autoScalePercentagePositiveMinimum != autoScalePercentagePositiveMinimum) {
        this->autoScalePercentagePositiveMinimum = autoScalePercentagePositiveMinimum;
        this->setModified();
    }
}

/**
 * @return The auto scale absolute percentage minimum.
 */
float
PaletteColorMapping::getAutoScaleAbsolutePercentageMinimum() const
{
    return this->autoScaleAbsolutePercentageMinimum;
}

/**
 * Set the auto scale absolute percentage minimum.
 *
 * @param autoScaleAbsolutePercentageMinimum
 *     New value for auto scale absolute percentage minimum.
 */
void
PaletteColorMapping::setAutoScaleAbsolutePercentageMinimum(const float autoScaleAbsolutePercentageMinimum)
{
    
    if (this->autoScaleAbsolutePercentageMinimum != autoScaleAbsolutePercentageMinimum) {
        this->autoScaleAbsolutePercentageMinimum = autoScaleAbsolutePercentageMinimum;
        this->setModified();
    }
}

/**
 * @return The auto scale absolute percentage maximum.
 */
float
PaletteColorMapping::getAutoScaleAbsolutePercentageMaximum() const
{
    return this->autoScaleAbsolutePercentageMaximum;
}

/**
 * Set the auto scale absolute percentage maximum.
 *
 * @param autoScaleAbsolutePercentageMaximum
 *     New value for auto scale absolute percentage maximum.
 */
void
PaletteColorMapping::setAutoScaleAbsolutePercentageMaximum(const float autoScaleAbsolutePercentageMaximum)
{
    if (this->autoScaleAbsolutePercentageMaximum != autoScaleAbsolutePercentageMaximum) {
        this->autoScaleAbsolutePercentageMaximum = autoScaleAbsolutePercentageMaximum;
        this->setModified();
    }
}

/**
 * See if negative data should be displayed.
 * @return true if negative data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayNegativeDataFlag() const
{
    return displayNegativeDataFlag;
}

/**
 * Set negative data should be displayed.
 * @param displayNegativeDataFlag - true to display negative data, else false
 *
 */
void
PaletteColorMapping::setDisplayNegativeDataFlag(const bool displayNegativeDataFlag)
{
    if (this->displayNegativeDataFlag != displayNegativeDataFlag) {
        this->displayNegativeDataFlag = displayNegativeDataFlag;
        this->setModified();
    }
}

/**
 * See if positive data should be displayed.
 * @return true if positive data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayPositiveDataFlag() const
{
    return displayPositiveDataFlag;
}

/**
 * Set positive data should be displayed.
 * @param displayPositiveDataFlag - true to display positive data, else false
 *
 */
void
PaletteColorMapping::setDisplayPositiveDataFlag(const bool displayPositiveDataFlag)
{
    if (this->displayPositiveDataFlag != displayPositiveDataFlag) {
        this->displayPositiveDataFlag = displayPositiveDataFlag;
        this->setModified();
    }
}

/**
 * See if zero data should be displayed.
 * @return true if zero data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayZeroDataFlag() const
{
    return displayZeroDataFlag;
}

/**
 * Set zero data should be displayed.
 * @param displayZeroDataFlag - true to display zero data, else false
 *
 */
void
PaletteColorMapping::setDisplayZeroDataFlag(const bool displayZeroDataFlag)
{
    if (this->displayZeroDataFlag != displayZeroDataFlag) {
        this->displayZeroDataFlag = displayZeroDataFlag;
        this->setModified();
    }
}

/**
 * Interpolate palette colors when displaying data?
 * @return true to interpolate data, else false.
 *
 */
bool
PaletteColorMapping::isInterpolatePaletteFlag() const
{
    return interpolatePaletteFlag;
}

/**
 * Set palette colors should be interpolated when displaying data.
 * @param interpolatePaletteFlag - true to interpolate, else false.
 *
 */
void
PaletteColorMapping::setInterpolatePaletteFlag(const bool interpolatePaletteFlag)
{
    if (this->interpolatePaletteFlag != interpolatePaletteFlag) {
        this->interpolatePaletteFlag = interpolatePaletteFlag;
        this->setModified();
    }
}

/**
 * Get how the data is scaled to the palette.
 * @return  Enumerated type indicating how data is scaled to the palette.
 *
 */
PaletteScaleModeEnum::Enum
PaletteColorMapping::getScaleMode() const
{
    return scaleMode;
}

/**
 * Set how the data is scaled to the palette.
 * @param scaleMode - Enumerated type indicating how data is scaled
 *    to the palette.
 *
 */
void
PaletteColorMapping::setScaleMode(const PaletteScaleModeEnum::Enum scaleMode)
{
    if (this->scaleMode != scaleMode) {
        this->scaleMode = scaleMode;
        this->setModified();
    }
}

/**
 * Get the name of the selected palette.
 * @return Name of the selected palette.
 *
 */
AString
PaletteColorMapping::getSelectedPaletteName() const
{
    return selectedPaletteName;
}

/**
 * Set the name of the selected palette.
 * @param selectedPaletteName - Name of selected palette.
 *
 */
void
PaletteColorMapping::setSelectedPaletteName(const AString& selectedPaletteName)
{
    if (this->selectedPaletteName != selectedPaletteName) {
        this->selectedPaletteName = selectedPaletteName;
        this->setModified();
    }
}

/**
 * Set the selected palette to PSYCH.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToPsych()
{
    this->setSelectedPaletteName("PSYCH");
}

/**
 * Set the selected palette to PSYCH-NO-NONE
 *
 */
void
PaletteColorMapping::setSelectedPaletteToPsychNoNone()
{
    this->setSelectedPaletteName("PSYCH-NO-NONE");
}

/**
 * Set the selected palette to Orange-Yellow.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToOrangeYellow()
{
    this->setSelectedPaletteName("Orange-Yellow");
}

/**
 * Set the selected palette to Gray Interpolated.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToGrayInterpolated()
{
    this->setSelectedPaletteName("Gray_Interp");
}

/**
 * Get auto user scale negative maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScaleNegativeMaximum() const
{
    return userScaleNegativeMaximum;
}

/**
 * Set user scale negative maximum.
 * @param userScaleNegativeMaximum - new value.
 *
 */
void
PaletteColorMapping::setUserScaleNegativeMaximum(const float userScaleNegativeMaximum)
{
    if (this->userScaleNegativeMaximum != userScaleNegativeMaximum) {
        this->userScaleNegativeMaximum = userScaleNegativeMaximum;
        this->setModified();
    }
}

/**
 * Get auto user scale negative minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScaleNegativeMinimum() const
{
    return userScaleNegativeMinimum;
}

/**
 * Set user scale negative minimum.
 * @param userScaleNegativeMinimum - new value.
 *
 */
void
PaletteColorMapping::setUserScaleNegativeMinimum(const float userScaleNegativeMinimum)
{
    if (this->userScaleNegativeMinimum != userScaleNegativeMinimum) {
        this->userScaleNegativeMinimum = userScaleNegativeMinimum;
        this->setModified();
    }
}

/**
 * Get auto user scale positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScalePositiveMaximum() const
{
    return userScalePositiveMaximum;
}

/**
 * Set user scale positive maximum.
 * @param userScalePositiveMaximum - new value.
 *
 */
void
PaletteColorMapping::setUserScalePositiveMaximum(const float userScalePositiveMaximum)
{
    if (this->userScalePositiveMaximum != userScalePositiveMaximum) {
        this->userScalePositiveMaximum = userScalePositiveMaximum;
        this->setModified();
    }
}

/**
 * Get auto user scale positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScalePositiveMinimum() const
{
    return userScalePositiveMinimum;
}

/**
 * Set user scale positive minimum.
 * @param userScalePositiveMinimum - new value.
 *
 */
void
PaletteColorMapping::setUserScalePositiveMinimum(const float userScalePositiveMinimum)
{
    if (this->userScalePositiveMinimum != userScalePositiveMinimum) {
        this->userScalePositiveMinimum = userScalePositiveMinimum;
        this->setModified();
    }
}

/**
 * Get the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * @return the threshold's value.
 *
 */
float 
PaletteColorMapping::getThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType) const
{
    float value = 0.0;
    
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            value = this->thresholdNormalMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            value = this->thresholdMappedMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            value = this->thresholdMappedAverageAreaMinimum;
            break;            
    }
    
    return value;
}

/**
 * Get the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * @return the threshold's value.
 *
 */
float 
PaletteColorMapping::getThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType) const
{
    float value = 0.0;
    
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            value = this->thresholdNormalMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            value = this->thresholdMappedMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            value = this->thresholdMappedAverageAreaMaximum;
            break;            
    }
    
    return value;
    
}

/**
 * Set the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * param thresholdMinimum the threshold's new value.
 *
 */
void 
PaletteColorMapping::setThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                         const float thresholdMinimum)
{
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            setThresholdNormalMinimum(thresholdMinimum);
            //this->thresholdNormalMinimum = thresholdMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            setThresholdMappedMinimum(thresholdMinimum);
            //this->thresholdMappedMinimum = thresholdMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            setThresholdMappedAverageAreaMinimum(thresholdMinimum);
            //this->thresholdMappedAverageAreaMinimum = thresholdMinimum;
            break;            
    }
}

/**
 * Set the maximum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * param thresholdMaximum the threshold's new value.
 *
 */
void 
PaletteColorMapping::setThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                         const float thresholdMaximum)
{
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            setThresholdNormalMaximum(thresholdMaximum);
            //this->thresholdNormalMaximum = thresholdMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            setThresholdMappedMaximum(thresholdMaximum);
            //this->thresholdMappedMaximum = thresholdMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            setThresholdMappedAverageAreaMaximum(thresholdMaximum);
            //this->thresholdMappedAverageAreaMaximum = thresholdMaximum;
            break;            
    }
}

/**
 * Get mapped average area minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaMinimum() const
{
    return thresholdMappedAverageAreaMinimum;
}

/**
 * Set the mapped average area minimum threshold.
 *
 * @param thresholdMappedAverageAreaNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaMinimum(const float thresholdMappedAverageAreaMinimum)
{
    if (this->thresholdMappedAverageAreaMinimum != thresholdMappedAverageAreaMinimum) {
        this->thresholdMappedAverageAreaMinimum = thresholdMappedAverageAreaMinimum;
        this->setModified();
    }
}

/**
 * Get mapped average area maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaMaximum() const
{
    return thresholdMappedAverageAreaMaximum;
}

/**
 * Set the mapped average area maximum threshold.
 *
 * @param thresholdMappedAverageAreaPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaMaximum(const float thresholdMappedAverageAreaMaximum)
{
    if (this->thresholdMappedAverageAreaMaximum != thresholdMappedAverageAreaMaximum) {
        this->thresholdMappedAverageAreaMaximum = thresholdMappedAverageAreaMaximum;
        this->setModified();
    }
}

/**
 * Get mapped minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedMinimum() const
{
    return thresholdMappedMinimum;
}

/**
 * Set the mapped minimum threshold.
 *
 * @param thresholdMappedNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedMinimum(const float thresholdMappedMinimum)
{
    if (this->thresholdMappedMinimum != thresholdMappedMinimum) {
        this->thresholdMappedMinimum = thresholdMappedMinimum;
        this->setModified();
    }
}

/**
 * Get mapped maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedMaximum() const
{
    return thresholdMappedMaximum;
}

/**
 * Set the mapped maximum threshold.
 *
 * @param thresholdMappedPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedMaximum(const float thresholdMappedMaximum)
{
    if (this->thresholdMappedMaximum != thresholdMappedMaximum) {
        this->thresholdMappedMaximum = thresholdMappedMaximum;
        this->setModified();
    }
}

/**
 * Get normal minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalMinimum() const
{
    return thresholdNormalMinimum;
}

/**
 * Set the normal minimum threshold.
 *
 * @param thresholdNormalNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalMinimum(const float thresholdNormalMinimum)
{
    if (this->thresholdNormalMinimum != thresholdNormalMinimum) {
        this->thresholdNormalMinimum = thresholdNormalMinimum;
        this->setModified();
    }
}

/**
 * Get normal maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalMaximum() const
{
    return thresholdNormalMaximum;
}

/**
 * Set the normal maximum threshold.
 *
 * @param thresholdNormalPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalMaximum(const float thresholdNormalMaximum)
{
    if (this->thresholdNormalMaximum != thresholdNormalMaximum) {
        this->thresholdNormalMaximum = thresholdNormalMaximum;
        this->setModified();
    }
}

/**
 * Get the threshold test.
 * @return Threshold test.
 *
 */
PaletteThresholdTestEnum::Enum
PaletteColorMapping::getThresholdTest() const
{
    return thresholdTest;
}

/**
 * Set the threshold test.
 * @param thresholdTest - The threshold test.
 *
 */
void
PaletteColorMapping::setThresholdTest(const PaletteThresholdTestEnum::Enum thresholdTest)
{
    if (this->thresholdTest != thresholdTest) {
        this->thresholdTest = thresholdTest;
        this->setModified();
    }
}

/**
 * Get the threshold type.
 * @return  Threshold type.
 *
 */
PaletteThresholdTypeEnum::Enum
PaletteColorMapping::getThresholdType() const
{
    return thresholdType;
}

/**
 * Set the threshold type.
 *
 * @param thresholdType - The threshold type.
 */
void
PaletteColorMapping::setThresholdType(const PaletteThresholdTypeEnum::Enum thresholdType)
{
    if (this->thresholdType != thresholdType) {
        this->thresholdType = thresholdType;
        this->setModified();
    }
}

/**
 * @return The threshold range mode.
 */
PaletteThresholdRangeModeEnum::Enum
PaletteColorMapping::getThresholdRangeMode() const
{
    return this->thresholdRangeMode;
}

/**
 * Set the threshold range mode.
 *
 * @param rangeMode
 *    New value for range mode.
 */
void
PaletteColorMapping::setThresholdRangeMode(const PaletteThresholdRangeModeEnum::Enum rangeMode)
{
    if (this->thresholdRangeMode != rangeMode) {
        this->thresholdRangeMode = rangeMode;
        setModified();
    }
}

/**
 * Get the name of the threshold data which may be the name of a
 * functional volume or a metric column.
 * @return  Name of data used as a threshold.
 *
 */
AString
PaletteColorMapping::getThresholdDataName() const
{
    return thresholdDataName;
}

/**
 * Set the name of the threshold data which may be the name of a
 * functional volume or a metric column.
 * @param thresholdDataName - name of data used as a threshold.
 *
 */
void
PaletteColorMapping::setThresholdDataName(const AString& thresholdDataName)
{
    if (this->thresholdDataName != thresholdDataName) {
        this->thresholdDataName = thresholdDataName;
        this->setModified();
    }
}

/**
 * Display non-zero data that fails the threshold test in green?
 * @return true if displayed, else false.
 *
 */
bool
PaletteColorMapping::isShowThresholdFailureInGreen() const
{
    return this->thresholdShowFailureInGreen;
}

/**
 * Set display non-zero data that fails the threshold test in green?
 * @param showInGreenFlag - true if displayed, else false.
 *
 */
void
PaletteColorMapping::setShowThresholdFailureInGreen(const bool showInGreenFlag)
{
    if (this->thresholdShowFailureInGreen != showInGreenFlag) {
        this->thresholdShowFailureInGreen = showInGreenFlag;
        setModified();
    }
}

/**
 * Set this object has been modified.
 *
 */
void
PaletteColorMapping::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
PaletteColorMapping::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object
 * or any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
PaletteColorMapping::isModified() const
{
    return this->modifiedFlag;
}

/**
 * Map data values to palette normalized values using the 
 * settings in this palette color mapping.
 *
 * @param statistics
 *    Statistics containing min.max values.
 * @param data 
 *    The data values.
 * @param normalizedValuesOut
 *    Result of mapping data values to palette normalized 
 *    values which range [-1.0, 1.0].  This array MUST contain
 *    the same number of values as 'data'.
 * @param numberOfData  
 *    Number of values in both data and normalizedValuesOut.
 */
void 
PaletteColorMapping::mapDataToPaletteNormalizedValues(const FastStatistics* statistics,
                                                      const float* dataValues,
                                                      float* normalizedValuesOut,
                                                      const int64_t numberOfData) const
{
    if (numberOfData <= 0) {
        return;
    }

    /*
     * Minimum and maximum values used when mapping scalar into color palette.
     */
    float mappingMostNegative  = 0.0;
    float mappingLeastNegative = 0.0;
    float mappingLeastPositive  = 0.0;
    float mappingMostPositive  = 0.0;
    switch (this->getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
            statistics->getNonzeroRanges(mappingMostNegative, mappingLeastNegative, mappingLeastPositive, mappingMostPositive);
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
        {
            const float mostPercentage  = this->getAutoScaleAbsolutePercentageMaximum();
            const float leastPercentage = this->getAutoScaleAbsolutePercentageMinimum();
            mappingMostNegative  = -statistics->getApproxAbsolutePercentile(mostPercentage);
            mappingLeastNegative = -statistics->getApproxAbsolutePercentile(leastPercentage);
            mappingLeastPositive =  statistics->getApproxAbsolutePercentile(leastPercentage);
            mappingMostPositive  =  statistics->getApproxAbsolutePercentile(mostPercentage);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float mostNegativePercentage  = this->getAutoScalePercentageNegativeMaximum();
            const float leastNegativePercentage = this->getAutoScalePercentageNegativeMinimum();
            const float leastPositivePercentage = this->getAutoScalePercentagePositiveMinimum();
            const float mostPositivePercentage  = this->getAutoScalePercentagePositiveMaximum();
            mappingMostNegative  = statistics->getApproxNegativePercentile(mostNegativePercentage);
            mappingLeastNegative = statistics->getApproxNegativePercentile(leastNegativePercentage);
            mappingLeastPositive = statistics->getApproxPositivePercentile(leastPositivePercentage);
            mappingMostPositive  = statistics->getApproxPositivePercentile(mostPositivePercentage);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            mappingMostNegative  = this->getUserScaleNegativeMaximum();
            mappingLeastNegative = this->getUserScaleNegativeMinimum();
            mappingLeastPositive = this->getUserScalePositiveMinimum();
            mappingMostPositive  = this->getUserScalePositiveMaximum();
            break;
    }
    float mappingPositiveDenominator = std::fabs(mappingMostPositive - mappingLeastPositive) * (1.0f - SMALL_POSITIVE);//if the "zero" color is extended to more than exact zeros, this correction prevents normalization from returning something greater than 1
    if (mappingPositiveDenominator == 0.0) {
        mappingPositiveDenominator = 1.0;
    }
    float mappingNegativeDenominator = std::fabs(mappingMostNegative - mappingLeastNegative) * (1.0f + SMALL_NEGATIVE);//ditto, but SMALL_NEGATIVE is negative
    if (mappingNegativeDenominator == 0.0) {
        mappingNegativeDenominator = 1.0;
    }
    
    for (int64_t i = 0; i < numberOfData; i++) {
        float scalar    = dataValues[i];
        
        /*
         * Color scalar using palette
         */
        float normalized = 0.0f;
        if (scalar > 0.0) {
            if (scalar >= mappingMostPositive) {
                normalized = 1.0f;
            }
            else if (scalar >= mappingLeastPositive) {
                float numerator = scalar - mappingLeastPositive;
                normalized = numerator / mappingPositiveDenominator + SMALL_POSITIVE; // JWH 24 April 2015   0.00001f;//don't return less than 0.00001f if input is positive
            }
            else {
                normalized = SMALL_POSITIVE;  // JWH 24 April 2015  0.00001f;
            }
        }
        else if (scalar < 0.0) {
            if (scalar <= mappingMostNegative) {
                normalized = -1.0f;
            }
            else if (scalar <= mappingLeastNegative) {
                float numerator = scalar - mappingLeastNegative;
                normalized = numerator / mappingNegativeDenominator + SMALL_NEGATIVE;  // JWH 24 April 2015  - 0.00001f;
            }
            else {
                normalized = SMALL_NEGATIVE;   // JWH 24 April 2015   -0.00001f;
            }
        }
        normalizedValuesOut[i] = normalized;
    }
}

/**
 * Get the text characters for drawing the scale above the palette
 * color bar.
 *
 * @param statistics
 *     Statistics for the data.
 * @param minimumValueTextOut
 *     Text for the minimum value.
 * @param zeroValueTextOut
 *     Text for the zero value(s)
 * @param maximumValueTextOut
 *     Text for the maximum value.
 *
 */
void
PaletteColorMapping::getPaletteColorBarScaleText(const FastStatistics* statistics,
                                                 AString& minimumValueTextOut,
                                                 AString& zeroValueTextOut,
                                                 AString& maximumValueTextOut) const
{
    minimumValueTextOut = "";
    zeroValueTextOut    = "";
    maximumValueTextOut = "";
    
    float minMax[4] = { -1.0, 0.0, 0.0, 1.0 };
    switch (getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
        {
            float dummy;
            statistics->getNonzeroRanges(minMax[0], dummy, dummy, minMax[3]);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
        {
            const float maxPct = getAutoScaleAbsolutePercentageMaximum();
            const float minPct = getAutoScaleAbsolutePercentageMinimum();
            
            minMax[0] = -statistics->getApproxAbsolutePercentile(maxPct);
            minMax[1] = -statistics->getApproxAbsolutePercentile(minPct);
            minMax[2] =  statistics->getApproxAbsolutePercentile(minPct);
            minMax[3] =  statistics->getApproxAbsolutePercentile(maxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float negMaxPct = getAutoScalePercentageNegativeMaximum();
            const float negMinPct = getAutoScalePercentageNegativeMinimum();
            const float posMinPct = getAutoScalePercentagePositiveMinimum();
            const float posMaxPct = getAutoScalePercentagePositiveMaximum();
            
            minMax[0] = statistics->getApproxNegativePercentile(negMaxPct);
            minMax[1] = statistics->getApproxNegativePercentile(negMinPct);
            minMax[2] = statistics->getApproxPositivePercentile(posMinPct);
            minMax[3] = statistics->getApproxPositivePercentile(posMaxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            minMax[0] = getUserScaleNegativeMaximum();
            minMax[1] = getUserScaleNegativeMinimum();
            minMax[2] = getUserScalePositiveMinimum();
            minMax[3] = getUserScalePositiveMaximum();
            break;
    }
    
    AString minMaxValueText[4];
//    NumericTextFormatting::formatValueRange(minMax,
//                                            minMaxValueText,
//                                            4);
    
    NumericTextFormatting::formatValueRangeNegativeAndPositive(minMax,
                                                               minMaxValueText);
    
    /*
     * Types of values for display
     */
    const bool isPositiveDisplayed = isDisplayPositiveDataFlag();
    const bool isNegativeDisplayed = isDisplayNegativeDataFlag();
    
    minimumValueTextOut = minMaxValueText[0];
    AString textCenterNeg = minMaxValueText[1];
    AString textCenterPos = minMaxValueText[2];
    AString textCenter = textCenterPos;
    if (isNegativeDisplayed && isPositiveDisplayed) {
        if (textCenterNeg != textCenterPos) {
            zeroValueTextOut = textCenterNeg + "/" + textCenterPos;
        }
        else {
            zeroValueTextOut = textCenterPos;
        }
    }
    else if (isNegativeDisplayed) {
        zeroValueTextOut = textCenterNeg;
    }
    else if (isPositiveDisplayed) {
        zeroValueTextOut = textCenterPos;
    }
    maximumValueTextOut = minMaxValueText[3];
}

/**
 * @return True if thresholding is linked meaning
 * that the high threshold is restricted to a positive value
 * the low threshold = -high.
 *
 * This is just a status and it is up to the user of this class
 * to properly set the threshold values.
 */
bool
PaletteColorMapping::isThresholdNegMinPosMaxLinked() const
{
    return this->thresholdNegMinPosMaxLinked;
}

/**
 * Set thresholding is linked meaning
 * that the high threshold is restricted to a positive value
 * the low threshold = -high.
 *
 * This is just a status and it is up to the user of this class
 * to properly set the threshold values.
 *
 * @param linked
 *    New status of low/high linked thresholding.
 */
void
PaletteColorMapping::setThresholdNegMinPosMaxLinked(const bool linked)
{
    if (this->thresholdNegMinPosMaxLinked != linked) {
        this->thresholdNegMinPosMaxLinked = linked;
        setModified();
    }
}


