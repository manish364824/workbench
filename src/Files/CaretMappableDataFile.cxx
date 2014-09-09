
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

#define __CARET_MAPPABLE_DATA_FILE_DECLARE__
#include "CaretMappableDataFile.h"
#undef __CARET_MAPPABLE_DATA_FILE_DECLARE__

#include <limits>

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "DataFileContentInformation.h"
#include "FastStatistics.h"
#include "GiftiLabelTable.h"
#include "Histogram.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneAttributes.h"
#include "StringTableModel.h"

using namespace caret;


    
/**
 * Constructor.
 */
CaretMappableDataFile::CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    
}

/**
 * Destructor.
 */
CaretMappableDataFile::~CaretMappableDataFile()
{
    
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is copied.
 */
CaretMappableDataFile::CaretMappableDataFile(const CaretMappableDataFile& cmdf)
: CaretDataFile(cmdf)
{
    this->copyCaretMappableDataFile(cmdf);
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is assigned to this.
 * @return
 *   Reference to this instance.
 */
CaretMappableDataFile& 
CaretMappableDataFile::operator=(const CaretMappableDataFile& cmdf)
{
    if (this != &cmdf) {
        CaretDataFile::operator=(cmdf);
        this->copyCaretMappableDataFile(cmdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 */
void 
CaretMappableDataFile::copyCaretMappableDataFile(const CaretMappableDataFile&)
{
    
}

// note: method is documented in header file
bool
CaretMappableDataFile::hasMapAttributes() const
{
    return true;
}

/**
 * Is this file able to map to the given structure?  Some data files, such
 * as CIFTI files, are able to map to multiple surface structure.  The default
 * implementation of this method simply compares the given structure to
 * getStructure() and returns true if they are the same value, else false.
 *
 * @param structure
 *   Structure for testing mappability status.
 * @return True if this file is able to map to the given structure, else false.
 */
bool
CaretMappableDataFile::isMappableToSurfaceStructure(const StructureEnum::Enum structure) const
{
    if (getStructure() == StructureEnum::ALL) {
        return true;
    }
    
    if (structure == getStructure()) {
        return true;
    }
    
    return false;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromNameOrNumber(const AString& mapName) const
{
    bool ok = false;
    int32_t ret = mapName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)
    {
        if (ret < 0 || ret >= getNumberOfMaps())
        {
            ret = -1;
        }
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        ret = getMapIndexFromName(mapName);
    }
    return ret;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromName(const AString& mapName) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (mapName == getMapName(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (uniqueID == getMapUniqueID(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
void
CaretMappableDataFile::updateScalarColoringForAllMaps(const PaletteFile* paletteFile)
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        updateScalarColoringForMap(iMap,
                             paletteFile);
    }
}

// note: method is documented in header file
NiftiTimeUnitsEnum::Enum
CaretMappableDataFile::getMapIntervalUnits() const
{
    return NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
}

// note: method is documented in header file
void
CaretMappableDataFile::getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                        float& mapIntervalStepValueOut) const
{
    firstMapUnitsValueOut   = 1.0;
    mapIntervalStepValueOut = 1.0;
}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
CaretMappableDataFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    dataRangeMaximumOut = std::numeric_limits<float>::max();
    dataRangeMinimumOut = -dataRangeMaximumOut;
    
    return false;
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CaretMappableDataFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                           SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    if (isMappedWithPalette()) {
        if (sceneAttributes->isModifiedPaletteSettingsSavedToScene()) {
            std::vector<SceneClass*> pcmClassVector;
            
            const int32_t numMaps = getNumberOfMaps();
            for (int32_t i = 0; i < numMaps; i++) {
                const PaletteColorMapping* pcmConst = getMapPaletteColorMapping(i);
                if (pcmConst->isModified()) {
                    PaletteColorMapping* pcm = const_cast<PaletteColorMapping*>(pcmConst);
                    
                    try {
                        const AString xml = pcm->encodeInXML();

                        
                        SceneClass* pcmClass = new SceneClass("savedPaletteColorMapping",
                                                              "SavedPaletteColorMapping",
                                                              1);
                        pcmClass->addString("mapName",
                                            getMapName(i));
                        pcmClass->addInteger("mapIndex", i);
                        pcmClass->addInteger("mapCount", numMaps);
                        pcmClass->addString("mapColorMapping", xml);
                        
                        pcmClassVector.push_back(pcmClass);
                    }
                    catch (const XmlException& e) {
                        sceneAttributes->addToErrorMessage("Failed to encode palette color mapping for file: "
                                                           + getFileNameNoPath()
                                                           + "  Map Name: "
                                                           + getMapName(i)
                                                           + "  Map Index: "
                                                           + AString::number(i)
                                                           + ".  "
                                                           + e.whatString());
                    }
                }
            }
            
            if ( ! pcmClassVector.empty()) {
                SceneClassArray* pcmArray = new SceneClassArray("savedPaletteColorMappingArray",
                                                                pcmClassVector);
                sceneClass->addChild(pcmArray);
            }
        }
    }
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CaretMappableDataFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    if (isMappedWithPalette()) {
        const int32_t numMaps = getNumberOfMaps();
        const SceneClassArray* pcmArray = sceneClass->getClassArray("savedPaletteColorMappingArray");
        if (pcmArray != NULL) {
            const int32_t numElements = pcmArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numElements; i++) {
                const SceneClass* pcmClass = pcmArray->getClassAtIndex(i);
                
                const AString mapName = pcmClass->getStringValue("mapName");
                const int32_t mapIndex = pcmClass->getIntegerValue("mapIndex", -1);
                const int32_t mapCount = pcmClass->getIntegerValue("mapCount", -1);
                const AString pcmString = pcmClass->getStringValue("mapColorMapping");
                
                int32_t restoreMapIndex = -1;
                
                /*
                 * Try to find map that has the saved name AND index.
                 */
                if (restoreMapIndex < 0) {
                    if ((mapIndex >= 0)
                        && (mapIndex < numMaps)) {
                        if (getMapName(mapIndex) == mapName) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                }
                
                /*
                 * If map count has not changed, give preference to
                 * map index over map name
                 */
                if (mapCount == numMaps) {
                    /*
                     * Try to find map that has the saved map index.
                     */
                    if (restoreMapIndex < 0) {
                        if ((mapIndex >= 0)
                            && (mapIndex < numMaps)) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                    
                    /*
                     * Try to find map that has the saved map name.
                     */
                    if (restoreMapIndex < 0) {
                        if ( ! mapName.isEmpty()) {
                            restoreMapIndex = getMapIndexFromName(mapName);
                        }
                    }
                    
                }
                else {
                    /*
                     * Try to find map that has the saved map name.
                     */
                    if (restoreMapIndex < 0) {
                        if ( ! mapName.isEmpty()) {
                            restoreMapIndex = getMapIndexFromName(mapName);
                        }
                    }
                    
                    /*
                     * Try to find map that has the saved map index.
                     */
                    if (restoreMapIndex < 0) {
                        if ((mapIndex >= 0)
                            && (mapIndex < numMaps)) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                }
                
                
                if (restoreMapIndex >= 0) {
                    try {
                        PaletteColorMapping pcm;
                        pcm.decodeFromStringXML(pcmString);
                        
                        PaletteColorMapping* pcmMap = getMapPaletteColorMapping(restoreMapIndex);
                        pcmMap->copy(pcm);
                        pcmMap->clearModified();
                    }
                    catch (const XmlException& e) {
                        sceneAttributes->addToErrorMessage("Failed to decode palette color mapping for file: "
                                                           + getFileNameNoPath()
                                                           + "  Map Name: "
                                                           + getMapName(i)
                                                           + "  Map Index: "
                                                           + AString::number(i)
                                                           + ".  "
                                                           + e.whatString());
                    }
                }
                else {
                    const AString msg = ("Unable to find map for restoring palette settings for file: "
                                         + getFileNameNoPath()
                                         + "  Map Name: "
                                         + mapName
                                         + "  Map Index: "
                                         + AString::number(mapIndex));
                    sceneAttributes->addToErrorMessage(msg);
                }
            }
        }
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CaretMappableDataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Maps to Surface",
                                        isSurfaceMappable());
    dataFileInformation.addNameAndValue("Maps to Volume",
                                        isVolumeMappable());
    dataFileInformation.addNameAndValue("Maps with LabelTable",
                                        isMappedWithLabelTable());
    dataFileInformation.addNameAndValue("Maps with Palette",
                                        isMappedWithPalette());
    
    if (isMappedWithPalette()) {
        NiftiTimeUnitsEnum::Enum timeUnits = getMapIntervalUnits();
        switch (timeUnits) {
            case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                break;
        }
        dataFileInformation.addNameAndValue("Map Interval Units", NiftiTimeUnitsEnum::toName(timeUnits));
        if (timeUnits != NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN) {
            float mapIntervalStart, mapIntervalStep;
            getMapIntervalStartAndStep(mapIntervalStart, mapIntervalStep);
            dataFileInformation.addNameAndValue("Map Interval Start", mapIntervalStart);
            dataFileInformation.addNameAndValue("Map Interval Step", mapIntervalStep);
        }
    }
    
    bool showMapFlag = ((isMappedWithLabelTable() || isMappedWithPalette())
                              && (isSurfaceMappable() || isVolumeMappable()));
    
    /*
     * Do not show maps on CIFTI connectivity matrix data because
     * they do not have maps, they have a matrix.
     */
    const bool ciftiMatrixFlag = (dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(this)
                                  != NULL);
    if (ciftiMatrixFlag) {
        showMapFlag = false;
    }
    
    /*
     * Did user override display of map information?
     */
    if ( ! dataFileInformation.isOptionFlag(DataFileContentInformation::OPTION_SHOW_MAP_INFORMATION)) {
        showMapFlag = false;
    }
    
    if (showMapFlag) {
        const int32_t numMaps = getNumberOfMaps();
        if (isSurfaceMappable()) {
            dataFileInformation.addNameAndValue("Number of Maps",
                                                numMaps);
        }
        
        if (numMaps > 0) {
            int columnCount = 0;
            const int COL_INDEX   = columnCount++;
            
            int32_t COL_MIN     = -1;
            int32_t COL_MAX     = -1;
            int32_t COL_MEAN    = -1;
            int32_t COL_DEV     = -1;
            int32_t COL_PCT_POS = -1;
            int32_t COL_PCT_NEG = -1;
            int32_t COL_INF_NAN = -1;
            
            if (isMappedWithPalette()) {
                COL_MIN = columnCount++;
                COL_MAX = columnCount++;
                COL_MEAN = columnCount++;
                COL_DEV = columnCount++;
                COL_PCT_POS = columnCount++;
                COL_PCT_NEG = columnCount++;
                COL_INF_NAN = columnCount++;
            }
            const int COL_NAME = columnCount++;
            
            /*
             * Include a row for the column titles
             */
            const int32_t tableRowCount = numMaps + 1;
            StringTableModel stringTable(tableRowCount,
                                         columnCount);
            
            stringTable.setElement(0, COL_INDEX, "Map");
            if (COL_MIN >= 0) {
                stringTable.setElement(0, COL_MIN, "Minimum");
            }
            if (COL_MAX >= 0) {
                stringTable.setElement(0, COL_MAX, "Maximum");
            }
            if (COL_MEAN >= 0) {
                stringTable.setElement(0, COL_MEAN, "Mean");
            }
            if (COL_DEV >= 0) {
                stringTable.setElement(0, COL_DEV, "Sample Dev");
            }
            if (COL_PCT_POS >= 0) {
                stringTable.setElement(0, COL_PCT_POS, "% Positive");
            }
            if (COL_PCT_NEG >= 0) {
                stringTable.setElement(0, COL_PCT_NEG, "% Negative");
            }
            if (COL_INF_NAN >= 0) {
                stringTable.setElement(0, COL_INF_NAN, "Inf/NaN");
            }
            
            stringTable.setElement(0, COL_NAME, "Map Name");
            stringTable.setColumnAlignment(COL_NAME, StringTableModel::ALIGN_LEFT);
            
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                
                const int32_t tableRow = mapIndex + 1;
                
                CaretAssert(COL_INDEX >= 0);
                CaretAssert(COL_NAME >= 0);
                stringTable.setElement(tableRow, COL_INDEX, (mapIndex + 1));
                stringTable.setElement(tableRow, COL_NAME, getMapName(mapIndex));
                
                const FastStatistics* stats = const_cast<CaretMappableDataFile*>(this)->getMapFastStatistics(mapIndex);
                if (isMappedWithPalette()
                    && (stats != NULL)) {
                    
                    const Histogram* histogram = getMapHistogram(mapIndex);
                    int64_t posCount = 0;
                    int64_t zeroCount = 0;
                    int64_t negCount = 0;
                    int64_t infCount = 0;
                    int64_t negInfCount = 0;
                    int64_t nanCount = 0;
                    histogram->getCounts(posCount,
                                         zeroCount,
                                         negCount,
                                         infCount,
                                         negInfCount,
                                         nanCount);
                    const int64_t numInfinityAndNotANumber = (infCount
                                                + negInfCount
                                                + nanCount);
                    const double totalCount = (posCount
                                                + zeroCount
                                                + negCount
                                                + numInfinityAndNotANumber);
                    const double pctPositive = (posCount / totalCount) * 100.0;
                    const double pctNegative = (negCount / totalCount) * 100.0;
                    
                    if (COL_MIN >= 0) {
                        stringTable.setElement(tableRow, COL_MIN, stats->getMin());
                    }
                    
                    if (COL_MAX >= 0) {
                        stringTable.setElement(tableRow, COL_MAX, stats->getMax());
                    }
                    
                    if (COL_MEAN >= 0) {
                        stringTable.setElement(tableRow, COL_MEAN, stats->getMean());
                    }
                    
                    if (COL_DEV >= 0) {
                        stringTable.setElement(tableRow, COL_DEV, stats->getSampleStdDev());
                    }
                    if (COL_PCT_POS >= 0) {
                        stringTable.setElement(tableRow, COL_PCT_POS, pctPositive);
                    }
                    
                    if (COL_PCT_NEG >= 0) {
                        stringTable.setElement(tableRow, COL_PCT_NEG, pctNegative);
                    }
                    
                    if (COL_INF_NAN >= 0) {
                        stringTable.setElement(tableRow, COL_INF_NAN, numInfinityAndNotANumber);
                    }
                }
            }
            
            dataFileInformation.addText("\n"
                                        + stringTable.getInString()
                                        + "\n");
            
        }
    }
    
    if (showMapFlag) {
        if (isMappedWithLabelTable()) {
            /*
             * Show label table for each map.
             * However, some files contain only a single label table used
             * for all maps and this condition is detected if the first
             * two label tables use the same pointer.
             */
            const int32_t numMaps = getNumberOfMaps();
            bool haveLabelTableForEachMap = false;
            if (numMaps > 1) {
                if (getMapLabelTable(0) != getMapLabelTable(1)) {
                    haveLabelTableForEachMap = true;
                }
            }
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                const AString labelTableName = ("Label table for "
                                                + (haveLabelTableForEachMap
                                                   ? ("map " + AString::number(mapIndex + 1) + ": " + getMapName(mapIndex))
                                                   : ("ALL maps"))
                                                + "\n");
                
                dataFileInformation.addText(labelTableName
                                            + getMapLabelTable(mapIndex)->toFormattedString("    ")
                                            + "\n");
                
                if ( ! haveLabelTableForEachMap) {
                    break;
                }
            }
        }
    }
}

/**
 * @return True if any of the maps in this file contain a
 * color mapping that possesses a modified status.
 */
bool
CaretMappableDataFile::isModifiedPaletteColorMapping() const
{
    if (isMappedWithPalette()) {
        const int32_t numMaps = getNumberOfMaps();
        
        for (int32_t i = 0; i < numMaps; i++) {
            if (getMapPaletteColorMapping(i)->isModified()) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @return True if the file is modified in any way EXCEPT for
 * the palette color mapping.  Also see isModified().
 */
bool
CaretMappableDataFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    return false;
}


/**
 * @return True if the file is modified in any way including
 * the palette color mapping.
 *
 * NOTE: While this method overrides that in the super class,
 * it is NOT virtual here.  Thus subclasses cannot override
 * this method and instead, subclasses should overrride
 * isModifiedExcludingPaletteColorMapping().
 */
bool
CaretMappableDataFile::isModified() const
{
    if (isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    
    if (isModifiedPaletteColorMapping()) {
        return true;
    }
    
    return false;
}

/**
 * Create cartesian chart data from the given data.
 *
 * @param
 *     Data for the Y-axis.
 * @return 
 *     Pointer to the ChartDataCartesian instance.
 */
ChartDataCartesian*
CaretMappableDataFile::helpCreateCartesianChartData(const std::vector<float>& data) throw (DataFileException)
{
    const int64_t numData = static_cast<int64_t>(data.size());
    
    /*
     * Some files may have time data but initially assume data-series
     */
    bool timeSeriesFlag = false;
    
    float convertTimeToSeconds = 1.0;
    switch (getMapIntervalUnits()) {
        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
            timeSeriesFlag = true;
            convertTimeToSeconds = 1000.0;
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
            convertTimeToSeconds = 1.0;
            timeSeriesFlag = true;
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
            convertTimeToSeconds = 1000000.0;
            timeSeriesFlag = true;
            break;
    }
    
    ChartDataCartesian* chartData = NULL;
    
    if (timeSeriesFlag) {
        chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
    }
    else {
        chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
    }

    if (chartData != NULL) {
        float timeStart = 0.0;
        float timeStep  = 1.0;
        if (timeSeriesFlag) {
            getMapIntervalStartAndStep(timeStart,
                                       timeStep);
            timeStart *= convertTimeToSeconds;
            timeStep  *= convertTimeToSeconds;
            chartData->setTimeStartInSecondsAxisX(timeStart);
            chartData->setTimeStepInSecondsAxisX(timeStep);
        }
        
        for (int64_t i = 0; i < numData; i++) {
            float xValue = i;
            
            if (timeSeriesFlag) {
                /*
                 * X-Value is "time"
                 */
                xValue = timeStart + (i * timeStep);
            }
            else {
                /*
                 * X-Value is the map index and map indices start at one
                 */
                xValue = i + 1;
            }
            
            chartData->addPoint(xValue,
                                data[i]);
        }
    }
    
    return chartData;
}

/**
 * Helper for getting chart data types supported by files that create
 * charts from brainordinates (multi-map files).
 * The chart data types are a function of the map interval units.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CaretMappableDataFile::helpGetSupportedBrainordinateChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    switch (getMapIntervalUnits()) {
        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
            CaretLogSevere("Units - HZ not supported");
            CaretAssertMessage(0, "Units - HZ not supported");
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
            chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
            CaretLogSevere("Units - PPM not supported");
            CaretAssertMessage(0, "Units - PPM not supported");
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
            chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
            chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
            chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES);
            break;
    }
}


