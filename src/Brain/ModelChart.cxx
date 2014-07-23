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

#include <algorithm>
#include <cmath>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartAxis.h"
#include "ChartableBrainordinateInterface.h"
#include "CaretDataFileSelectionModel.h"
#include "ChartableMatrixInterface.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "ModelChart.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 *
 */
ModelChart::ModelChart(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_CHART,
                         brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "Chart View");

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartableMatrixFileSelectionModel[i] = CaretDataFileSelectionModel::newInstanceForChartableMatrixInterface(m_brain);
    }
    initializeCharts();
    
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS);
}

/**
 * Destructor
 */
ModelChart::~ModelChart()
{
    delete m_overlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
    
    removeAllCharts();    

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartableMatrixFileSelectionModel[i];
        m_chartableMatrixFileSelectionModel[i] = NULL;
    }
}

void
ModelChart::initializeCharts()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedChartDataType[i] = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
        
        m_chartModelDataSeries[i] = new ChartModelDataSeries();
        m_chartModelDataSeries[i]->getLeftAxis()->setText("Value");
        m_chartModelDataSeries[i]->getBottomAxis()->setText("Map Index");
        
        m_chartModelTimeSeries[i] = new ChartModelTimeSeries();
        m_chartModelTimeSeries[i]->getLeftAxis()->setText("Activity");
        m_chartModelTimeSeries[i]->getBottomAxis()->setText("Time");
    }    
}

/**
 * Reset this model.
 */
void
ModelChart::reset()
{
    removeAllCharts();
    
    initializeCharts();
}

/**
 * Remove all of the charts.
 */
void
ModelChart::removeAllCharts()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (m_chartModelDataSeries[i] != NULL) {
            delete m_chartModelDataSeries[i];
            m_chartModelDataSeries[i] = NULL;
        }
        
        if (m_chartModelTimeSeries[i] != NULL) {
            delete m_chartModelTimeSeries[i];
            m_chartModelTimeSeries[i] = NULL;
        }
    }

    
    m_dataSeriesChartData.clear();
    m_timeSeriesChartData.clear();
    
    m_previousChartMatrixFiles.clear();
}

/**
 * Load chart data for an average of surface nodes.
 *
 * @param structure
 *     The surface structure
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param nodeIndices
 *     Indices of node.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChart::loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                         const int32_t surfaceNumberOfNodes,
                                         const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    std::map<ChartableBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndChartFilesForChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;
        
        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadAverageBrainordinateChartDataForSurfaceNodes(structure,
                                                                              nodeIndices);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setSurfaceNodeAverage(chartFile->getBrainordinateChartCaretMappableDataFile()->getFileName(),
                                              StructureEnum::toName(structure),
                                              surfaceNumberOfNodes, nodeIndices);
            
            addChartToChartModels(tabIndices,
                                  chartData);
        }
    }    
}

/**
 * Load chart data for voxel at the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChart::loadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    std::map<ChartableBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndChartFilesForChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;
        
        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadBrainordinateChartDataForVoxelAtCoordinate(xyz);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setVolumeVoxel(chartFile->getBrainordinateChartCaretMappableDataFile()->getFileName(),
                                       xyz);
            
            addChartToChartModels(tabIndices,
                                  chartData);
        }
    }
}

/**
 * Add the chart to the given tabs.
 *
 * @param tabIndices
 *    Indices of tabs for chart data
 * @param chartData
 *    Chart data that is added.
 */
void
ModelChart::addChartToChartModels(const std::vector<int32_t>& tabIndices,
                                  ChartData* chartData)
{
    CaretAssert(chartData);
    
    const ChartDataTypeEnum::Enum chartDataDataType = chartData->getChartDataType();
    
    switch (chartDataDataType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            CaretAssert(0);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
        {
            ChartDataCartesian* cdc = dynamic_cast<ChartDataCartesian*>(chartData);
            CaretAssert(cdc);
            QSharedPointer<ChartDataCartesian> cdcPtr(cdc);
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                CaretAssertArrayIndex(m_chartModelDataSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
                m_chartModelDataSeries[tabIndex]->addChartData(cdcPtr);
            }
            m_dataSeriesChartData.push_front(cdcPtr.toWeakRef());
        }
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
        {
            ChartDataCartesian* cdc = dynamic_cast<ChartDataCartesian*>(chartData);
            CaretAssert(cdc);
            QSharedPointer<ChartDataCartesian> cdcPtr(cdc);
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                CaretAssertArrayIndex(m_chartModelTimeSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
                m_chartModelTimeSeries[tabIndex]->addChartData(cdcPtr);
            }
            m_timeSeriesChartData.push_front(cdcPtr.toWeakRef());
        }
            break;
    }
}

/**
 * Get tabs and chart files for loading chart data.
 *
 * @param chartFileEnabledTabsOut
 *    Map with first being a chartable file and the second being
 *    tabs for which that chartable file is enabled.
 */
void
ModelChart::getTabsAndChartFilesForChartLoading(std::map<ChartableBrainordinateInterface*, std::vector<int32_t> >& chartFileEnabledTabsOut) const
{
    chartFileEnabledTabsOut.clear();
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
    
    std::vector<ChartableBrainordinateInterface*> chartFiles;
    m_brain->getAllChartableBrainordinateDataFilesWithChartingEnabled(chartFiles);
    
    for (std::vector<ChartableBrainordinateInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableBrainordinateInterface* cf = *iter;
        std::vector<int32_t> chartFileTabIndices;
        
        for (std::vector<int32_t>::iterator tabIter = validTabIndices.begin();
             tabIter != validTabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            if (cf->isBrainordinateChartingEnabled(tabIndex)) {
                chartFileTabIndices.push_back(tabIndex);
            }
        }
        
        if ( ! chartFileTabIndices.empty()) {
            chartFileEnabledTabsOut.insert(std::make_pair(cf, chartFileTabIndices));
        }
    }
}

/**
 * Load chart data for a surface node.
 *
 * @param structure
 *     The surface structure
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param nodeIndex
 *     Index of node.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChart::loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                        const int32_t surfaceNumberOfNodes,
                                        const int32_t nodeIndex) throw (DataFileException)
{
    std::map<ChartableBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndChartFilesForChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;

        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadBrainordinateChartDataForSurfaceNode(structure,
                                               nodeIndex);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setSurfaceNode(chartFile->getBrainordinateChartCaretMappableDataFile()->getFileName(),
                                       StructureEnum::toName(structure),
                                       surfaceNumberOfNodes,
                                       nodeIndex);
            
            addChartToChartModels(tabIndices,
                                  chartData);
        }
    }
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelChart::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS) {
        EventNodeIdentificationColorsGetFromCharts* nodeChartID =
           dynamic_cast<EventNodeIdentificationColorsGetFromCharts*>(event);
        CaretAssert(nodeChartID);
        
        EventBrowserTabGetAll allTabsEvent;
        EventManager::get()->sendEvent(allTabsEvent.getPointer());
        std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
        
        
        const AString structureName = nodeChartID->getStructureName();
        
        std::vector<ChartDataCartesian*> cartesianChartData;
        
        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator dsIter = m_dataSeriesChartData.begin();
             dsIter != m_dataSeriesChartData.end();
             dsIter++) {
            QSharedPointer<ChartDataCartesian> spCart = dsIter->toStrongRef();
            if ( ! spCart.isNull()) {
                cartesianChartData.push_back(spCart.data());
            }
        }
        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator tsIter = m_timeSeriesChartData.begin();
             tsIter != m_timeSeriesChartData.end();
             tsIter++) {
            QSharedPointer<ChartDataCartesian> spCart = tsIter->toStrongRef();
            if ( ! spCart.isNull()) {
                cartesianChartData.push_back(spCart.data());
            }
        }
        
        
        /*
         * Iterate over node indices for which colors are desired.
         */
        const std::vector<int32_t> nodeIndices = nodeChartID->getNodeIndices();
        for (std::vector<int32_t>::const_iterator nodeIter = nodeIndices.begin();
             nodeIter != nodeIndices.end();
             nodeIter++) {
            const int32_t nodeIndex = *nodeIter;
            
            /*
             * Iterate over the data in the cartesian chart
             */
            bool foundNodeFlag = false;
            for (std::vector<ChartDataCartesian*>::iterator cdIter = cartesianChartData.begin();
                 cdIter != cartesianChartData.end();
                 cdIter++) {
                const ChartDataCartesian* cdc = *cdIter;
                const ChartDataSource* cds = cdc->getChartDataSource();
                if (cds->isSurfaceNodeSourceOfData(structureName, nodeIndex)) {
                    /*
                     * Found node index so add its color to the event
                     */
                    foundNodeFlag = true;
                    const CaretColorEnum::Enum color = cdc->getColor();
                    const float* rgb = CaretColorEnum::toRGB(color);
                    nodeChartID->addNode(nodeIndex,
                                         rgb);
                    break;
                }
            }
        }

        nodeChartID->setEventProcessed();
    }
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelChart::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Chart";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelChart::getNameForBrowserTab() const
{
    AString name = "Chart";
    return name;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelChart::getOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelChart::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Initilize the overlays for this model.
 */
void 
ModelChart::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void 
ModelChart::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    std::set<AString> validChartDataIDs;
    saveChartModelsToScene(sceneAttributes,
                           sceneClass,
                           tabIndices,
                           validChartDataIDs);
    
    sceneClass->addEnumeratedTypeArrayForTabIndices<ChartDataTypeEnum, ChartDataTypeEnum::Enum>("m_selectedChartDataType",
                                                                                                m_selectedChartDataType,
                                                                                                tabIndices);

    /*
     * Save matrix chart models to scene.
     */
    SceneObjectMapIntegerKey* matrixSceneMap = new SceneObjectMapIntegerKey("chartableMatrixFileSelectionModelMap",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    
    std::vector<SceneClass*> matrixSelectionVector;
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        matrixSceneMap->addClass(tabIndex, m_chartableMatrixFileSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                                      "m_chartableMatrixFileSelectionModel"));
    }
    sceneClass->addChild(matrixSceneMap);
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
ModelChart::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    reset();
    
    /*
     * Restore the chart models
     */
    restoreChartModelsFromScene(sceneAttributes,
                                sceneClass);
    
    sceneClass->getEnumerateTypeArrayForTabIndices<ChartDataTypeEnum, ChartDataTypeEnum::Enum>("m_selectedChartDataType",
                                                                                               m_selectedChartDataType);

    /*
     * Restore matrix chart models from scene.
     */
    const SceneObjectMapIntegerKey* matrixSceneMap = sceneClass->getMapIntegerKey("chartableMatrixFileSelectionModelMap");
    if (matrixSceneMap != NULL) {
        const std::vector<int32_t> tabIndices = matrixSceneMap->getKeys();
        for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
             tabIter != tabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            const SceneClass* sceneClass = matrixSceneMap->classValue(tabIndex);
            m_chartableMatrixFileSelectionModel[tabIndex]->restoreFromScene(sceneAttributes,
                                                                            sceneClass);
        }
    }
}

/**
 * Save chart models to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void
ModelChart::saveChartModelsToScene(const SceneAttributes* sceneAttributes,
                            SceneClass* sceneClass,
                            const std::vector<int32_t>& tabIndices,
                            std::set<AString>& validChartDataIDsOut)
{
    validChartDataIDsOut.clear();
    
    std::set<ChartData*> chartDataForSavingToSceneSet;
    
    /*
     * Save chart models to scene.
     */
    std::vector<SceneClass*> chartModelVector;
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        ChartModel* chartModel = NULL;
        switch (getSelectedChartDataType(tabIndex)) {
            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                chartModel = getSelectedDataSeriesChartModel(tabIndex);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                chartModel = getSelectedTimeSeriesChartModel(tabIndex);
                break;
        }
        
        if (chartModel != NULL) {
            SceneClass* chartModelClass = chartModel->saveToScene(sceneAttributes,
                                                                  "chartModel");
            if (chartModelClass == NULL) {
                continue;
            }
            
            SceneClass* chartClassContainer = new SceneClass("chartClassContainer",
                                                             "ChartClassContainer",
                                                             1);
            chartClassContainer->addInteger("tabIndex", tabIndex);
            chartClassContainer->addEnumeratedType<ChartDataTypeEnum,ChartDataTypeEnum::Enum>("chartDataType",
                                                                                              chartModel->getChartDataType());
            chartClassContainer->addClass(chartModelClass);
            
            chartModelVector.push_back(chartClassContainer);
            
            /*
             * Add chart data that is in models saved to scene.
             *
             */
            std::vector<ChartData*> chartDatasInModel = chartModel->getAllChartDatas();
            chartDataForSavingToSceneSet.insert(chartDatasInModel.begin(),
                                                chartDatasInModel.end());
        }
    }

    if ( ! chartModelVector.empty()) {
        SceneClassArray* modelArray = new SceneClassArray("chartModelArray",
                                                      chartModelVector);
        sceneClass->addChild(modelArray);
    }

    if ( ! chartDataForSavingToSceneSet.empty()) {
        std::vector<SceneClass*> chartDataClassVector;
        for (std::set<ChartData*>::iterator cdIter = chartDataForSavingToSceneSet.begin();
             cdIter != chartDataForSavingToSceneSet.end();
             cdIter++) {
            ChartData* chartData = *cdIter;
            SceneClass* chartDataClass = chartData->saveToScene(sceneAttributes,
                                                                "chartData");
            
            SceneClass* chartDataContainer = new SceneClass("chartDataContainer",
                                                            "ChartDataContainer",
                                                            1);
            chartDataContainer->addEnumeratedType<ChartDataTypeEnum, ChartDataTypeEnum::Enum>("chartDataType",
                                                                                              chartData->getChartDataType());
            chartDataContainer->addClass(chartDataClass);
            
            chartDataClassVector.push_back(chartDataContainer);
        }
        
        if ( ! chartDataClassVector.empty()) {
            SceneClassArray* dataArray = new SceneClassArray("chartDataArray",
                                                             chartDataClassVector);
            sceneClass->addChild(dataArray);
        }
    }
}

/**
 * Restore the chart models from the scene.
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
ModelChart::restoreChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                 const SceneClass* sceneClass)
{
    /*
     * Restore the chart models
     */
    const SceneClassArray* chartModelArray = sceneClass->getClassArray("chartModelArray");
    if (chartModelArray != NULL) {
        const int numElements = chartModelArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numElements; i++) {
            const SceneClass* chartClassContainer = chartModelArray->getClassAtIndex(i);
            if (chartClassContainer != NULL) {
                const int32_t tabIndex = chartClassContainer->getIntegerValue("tabIndex", -1);
                const ChartDataTypeEnum::Enum chartDataType =  chartClassContainer->getEnumeratedTypeValue<ChartDataTypeEnum, ChartDataTypeEnum::Enum>("chartDataType",
                                                                                                        ChartDataTypeEnum::CHART_DATA_TYPE_INVALID);
                const SceneClass* chartModelClass = chartClassContainer->getClass("chartModel");
                
                if ((tabIndex >= 0)
                    && (chartDataType != ChartDataTypeEnum::CHART_DATA_TYPE_INVALID)
                    && (chartModelClass != NULL)) {
                    CaretAssertArrayIndex(m_chartModelDataSeries,
                                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                          tabIndex);
                    
                    switch (chartDataType) {
                        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                            CaretAssert(0);
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                            m_chartModelDataSeries[tabIndex]->restoreFromScene(sceneAttributes,
                                                                               chartModelClass);
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                            m_chartModelTimeSeries[tabIndex]->restoreFromScene(sceneAttributes,
                                                                               chartModelClass);
                            break;
                    }
                }
            }
        }
    }
    
    /*
     * Restore the chart data
     */
    std::vector<QSharedPointer<ChartData> > restoredChartData;
    const SceneClassArray* chartDataArray = sceneClass->getClassArray("chartDataArray");
    if (chartDataArray != NULL) {
        const int numElements = chartDataArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numElements; i++) {
            const SceneClass* chartDataContainer = chartDataArray->getClassAtIndex(i);
            if (chartDataContainer != NULL) {
                const ChartDataTypeEnum::Enum chartDataType = chartDataContainer->getEnumeratedTypeValue<ChartDataTypeEnum, ChartDataTypeEnum::Enum>("chartDataType",
                                                                                                                                                       ChartDataTypeEnum::CHART_DATA_TYPE_INVALID);
                const SceneClass* chartDataClass = chartDataContainer->getClass("chartData");
                if ((chartDataType != ChartDataTypeEnum::CHART_DATA_TYPE_INVALID)
                    && (chartDataClass != NULL)) {
                    ChartData* chartData = ChartData::newChartDataForChartDataType(chartDataType);
                    chartData->restoreFromScene(sceneAttributes, chartDataClass);
                    
                    restoredChartData.push_back(QSharedPointer<ChartData>(chartData));
                }
                
            }
        }
    }
    
    /*
     * Have chart models restore pointers to chart data
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartModelDataSeries[i]->restoreChartDataFromScene(restoredChartData);
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartModelTimeSeries[i]->restoreChartDataFromScene(restoredChartData);
    }
    
    
    for (std::vector<QSharedPointer<ChartData> >::iterator rcdIter = restoredChartData.begin();
         rcdIter != restoredChartData.end();
         rcdIter++) {
        QSharedPointer<ChartData> chartPointer = *rcdIter;
        
        switch (chartPointer->getChartDataType()) {
            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                CaretAssert(0);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            {
                QSharedPointer<ChartDataCartesian> cartChartPointer = chartPointer.dynamicCast<ChartDataCartesian>();
                CaretAssert( ! cartChartPointer.isNull());
                m_dataSeriesChartData.push_back(cartChartPointer);
            }
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                CaretAssert(0);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            {
                QSharedPointer<ChartDataCartesian> cartChartPointer = chartPointer.dynamicCast<ChartDataCartesian>();
                CaretAssert( ! cartChartPointer.isNull());
                m_timeSeriesChartData.push_back(cartChartPointer);
            }
                break;
        }
    }
}


/**
 * Get a text description of the window's content.
 *
 * @param tabIndex
 *    Index of the tab for content description.
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ModelChart::getDescriptionOfContent(const int32_t tabIndex,
                                    PlainTextStringBuilder& descriptionOut) const
{
    ChartModel* chartModel = NULL;
    switch (getSelectedChartDataType(tabIndex)) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            chartModel = const_cast<ChartModelDataSeries*>(getSelectedDataSeriesChartModel(tabIndex));
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            chartModel = const_cast<ChartModelTimeSeries*>(getSelectedTimeSeriesChartModel(tabIndex));
            break;
    }

    const ChartModel* chartModelConst = chartModel;
    
    if (chartModel != NULL) {
        descriptionOut.addLine("Chart Type: "
                               + ChartDataTypeEnum::toGuiName(chartModel->getChartDataType()));

        descriptionOut.pushIndentation();
        
        const std::vector<const ChartData*> cdVec = chartModelConst->getAllChartDatas();
        for (std::vector<const ChartData*>::const_iterator iter = cdVec.begin();
             iter != cdVec.end();
             iter++) {
            const ChartData* cd = *iter;
            if (cd->isSelected(tabIndex)) {
                descriptionOut.addLine(cd->getChartDataSource()->getDescription());
            }
        }
        
        if (chartModel->isAverageChartDisplaySupported()) {
            if (chartModel->isAverageChartDisplaySelected()) {
                descriptionOut.addLine("Average Chart Displayed");
            }
        }
        
        descriptionOut.popIndentation();
    }
    else {
        descriptionOut.addLine("No charts to display");
    }
}

/**
 * Copy the tab content from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelChart::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    m_overlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    
    m_selectedChartDataType[destinationTabIndex] = m_selectedChartDataType[sourceTabIndex];
    *m_chartModelDataSeries[destinationTabIndex] = *m_chartModelDataSeries[sourceTabIndex];
    *m_chartModelTimeSeries[destinationTabIndex] = *m_chartModelTimeSeries[sourceTabIndex];
    m_chartableMatrixFileSelectionModel[destinationTabIndex]->setSelectedFile(m_chartableMatrixFileSelectionModel[sourceTabIndex]->getSelectedFile());
}

/**
 * Set the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @param dataType
 *    Type of data for chart.
 */
void
ModelChart::setSelectedChartDataType(const int32_t tabIndex,
                              const ChartDataTypeEnum::Enum dataType)
{
    CaretAssertArrayIndex(m_selectedChartDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_selectedChartDataType[tabIndex] = dataType;
}

/**
 * Get the valid chart data types based upon the currently loaded files.
 * 
 * @param validChartDataTypesOut
 *    Output containing valid chart data types.
 */
void
ModelChart::getValidChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& validChartDataTypesOut) const
{
    validChartDataTypesOut.clear();
    
    bool haveDataSeries = false;
    bool haveMatrix     = false;
    bool haveTimeSeries = false;
    
    std::vector<ChartableBrainordinateInterface*> allBrainordinateChartableFiles;
    m_brain->getAllChartableBrainordinateDataFiles(allBrainordinateChartableFiles);

    for (std::vector<ChartableBrainordinateInterface*>::iterator fileIter = allBrainordinateChartableFiles.begin();
         fileIter != allBrainordinateChartableFiles.end();
         fileIter++) {
        ChartableBrainordinateInterface* chartFile = *fileIter;
        
        std::vector<ChartDataTypeEnum::Enum> chartDataTypes;
        chartFile->getSupportedBrainordinateChartDataTypes(chartDataTypes);
        
        for (std::vector<ChartDataTypeEnum::Enum>::iterator typeIter = chartDataTypes.begin();
             typeIter != chartDataTypes.end();
             typeIter++) {
            const ChartDataTypeEnum::Enum cdt = *typeIter;
            switch (cdt) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    haveDataSeries = true;
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    haveTimeSeries = true;
                    break;
            }
        }
    }
    
    std::vector<ChartableMatrixInterface*> allMatrixChartableFiles;
    m_brain->getAllChartableMatrixDataFiles(allMatrixChartableFiles);
    
    for (std::vector<ChartableMatrixInterface*>::iterator fileIter = allMatrixChartableFiles.begin();
         fileIter != allMatrixChartableFiles.end();
         fileIter++) {
        ChartableMatrixInterface* chartFile = *fileIter;
        
        std::vector<ChartDataTypeEnum::Enum> chartDataTypes;
        chartFile->getSupportedMatrixChartDataTypes(chartDataTypes);
        
        for (std::vector<ChartDataTypeEnum::Enum>::iterator typeIter = chartDataTypes.begin();
             typeIter != chartDataTypes.end();
             typeIter++) {
            const ChartDataTypeEnum::Enum cdt = *typeIter;
            switch (cdt) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    haveMatrix = true;
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    break;
            }
        }
    }
    
    if (haveDataSeries) {
        validChartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES);
    }
    if (haveMatrix) {
        validChartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX);
    }
    if (haveTimeSeries) {
        validChartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES);
    }
}


/**
 * Get the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart type in the given tab.
 */
ChartDataTypeEnum::Enum
ModelChart::getSelectedChartDataType(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectedChartDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    ChartDataTypeEnum::Enum chartDataType = m_selectedChartDataType[tabIndex];
    
    /*
     * Verify that the selected chart data type is valid.
     */
    std::vector<ChartDataTypeEnum::Enum> validChartDataTypes;
    getValidChartDataTypes(validChartDataTypes);
    if (std::find(validChartDataTypes.begin(),
                  validChartDataTypes.end(),
                  chartDataType) == validChartDataTypes.end()) {
        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
    }
    
    /*
     * If selected chart data type is invalid, find a valid chart type,
     * preferably one that contains data.
     */
    if (chartDataType == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
        if ( ! validChartDataTypes.empty()) {
            /*
             * Will become the the first valid chart data type that contains
             * data (if there is one)
             */
            ChartDataTypeEnum::Enum chartDataTypeWithValidData = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
            
            /*
             * Loop through all chart types (some or all valid charts 
             * types may not contain data until the user commands loading of data)
             */
            std::vector<ChartDataTypeEnum::Enum> allChartDataTypes;
            ChartDataTypeEnum::getAllEnums(allChartDataTypes);
            for (std::vector<ChartDataTypeEnum::Enum>::iterator iter = allChartDataTypes.begin();
                 iter != allChartDataTypes.end();
                 iter++) {
                const ChartDataTypeEnum::Enum cdt = *iter;
                if (std::find(validChartDataTypes.begin(),
                              validChartDataTypes.end(),
                              cdt) != validChartDataTypes.end()) {
                    if (chartDataType == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                        chartDataType = cdt;
                    }
                    
                    switch (cdt) {
                        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                            if (m_chartModelDataSeries[tabIndex]->getNumberOfChartData()) {
                                if (chartDataTypeWithValidData == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                    chartDataTypeWithValidData = ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES;
                                }
                            }
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                            if (chartDataTypeWithValidData == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                chartDataTypeWithValidData = ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX;
                            }
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                            if (m_chartModelTimeSeries[tabIndex]->getNumberOfChartData()) {
                                if (chartDataTypeWithValidData == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                    chartDataTypeWithValidData = ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES;
                                }
                            }
                            break;
                    }
                }
            }
            
            if (chartDataTypeWithValidData != ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                chartDataType = chartDataTypeWithValidData;
            }
            else if (chartDataType == ChartDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                chartDataType = validChartDataTypes[0];
            }
        }
    }
    
    /*
     * Selected type may have changed due to loaded files changing
     */
    m_selectedChartDataType[tabIndex] = chartDataType;
    
    return chartDataType;
}

/**
 * Get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModel(const int32_t tabIndex)
{
    const ChartModelDataSeries* model = getSelectedDataSeriesChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModelDataSeries* nonConstModel = const_cast<ChartModelDataSeries*>(model);
    return nonConstModel;
}

/**
 * Get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
const ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModel(const int32_t tabIndex) const
{
    return getSelectedDataSeriesChartModelHelper(tabIndex);
}

/**
 * Helper to get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
const ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModelHelper(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartModelDataSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartModelDataSeries[tabIndex];
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModel(const int32_t tabIndex)
{
    const ChartModelTimeSeries* model = getSelectedTimeSeriesChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModelTimeSeries* nonConstModel = const_cast<ChartModelTimeSeries*>(model);
    return nonConstModel;
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModel(const int32_t tabIndex) const
{
    return getSelectedTimeSeriesChartModelHelper(tabIndex);
}

/**
 * Helper to get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModelHelper(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartModelTimeSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartModelTimeSeries[tabIndex];
}

/**
 * Get the chartable matrix file selection model for the given tab.
 * 
 * @param tabIndex
 *     Index of the tab.
 * @return 
 *     Chartable file selection model for the tab.
 */
CaretDataFileSelectionModel*
ModelChart::getChartableMatrixFileSelectionModel(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartableMatrixFileSelectionModel,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    return m_chartableMatrixFileSelectionModel[tabIndex];
}
