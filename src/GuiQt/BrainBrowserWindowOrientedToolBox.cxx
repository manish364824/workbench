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

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QScrollArea>
#include <QToolBox>
#include <QTabWidget>
#include <QTimer>

#include "AnnotationFile.h"
#include "AnnotationSelectionViewController.h"
#include "AnnotationTextSubstitutionViewController.h"
#include "BorderSelectionViewController.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretPreferences.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "ChartTwoOverlaySetViewController.h"
#include "ChartableMatrixInterface.h"
#include "ChartToolBoxViewController.h"
#include "CiftiConnectivityMatrixViewController.h"
#include "DeveloperFlagsEnum.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberOrientationSelectionViewController.h"
#include "FociSelectionViewController.h"
#include "GuiManager.h"
#include "ImageSelectionViewController.h"
#include "LabelSelectionViewController.h"
#include "OverlaySetViewController.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "SessionManager.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineSetViewController.h"
#include "WuQTabWidgetWithSizeHint.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Construct the toolbox.
 * 
 * @param browserWindowIndex
 *    Index of browser window that contains this toolbox.
 * @param title
 *    Title for the toolbox.
 * @param location
 *    Locations allowed for this toolbox.
 */
BrainBrowserWindowOrientedToolBox::BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                                                     const QString& title,
                                                                     const ToolBoxType toolBoxType,
                                                                     QWidget* parent)
:   QDockWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    toggleViewAction()->setText("Toolbox");
    
    m_toolBoxTitle = title;
    setWindowTitle(m_toolBoxTitle);
    
    bool isFeaturesToolBox  = false;
    bool isOverlayToolBox = false;
    Qt::Orientation orientation = Qt::Horizontal;
    AString toolboxTypeName = "";
    switch (toolBoxType) {
        case TOOL_BOX_FEATURES:
            orientation = Qt::Vertical;
            isFeaturesToolBox = true;
            toggleViewAction()->setText("Features Toolbox");
            toolboxTypeName = "Features";
            break;
        case TOOL_BOX_OVERLAYS_HORIZONTAL:
            orientation = Qt::Horizontal;
            isOverlayToolBox = true;
            toolboxTypeName = "OverlayHorizontal";
            break;
        case TOOL_BOX_OVERLAYS_VERTICAL:
            orientation = Qt::Vertical;
            isOverlayToolBox = true;
            toolboxTypeName = "OverlayVertical";
            break;
    }
    
    /*
     * Needed for saving and restoring window state in main window
     */
    CaretAssert(toolboxTypeName.length() > 0);
    setObjectName("BrainBrowserWindowOrientedToolBox_"
                  + toolboxTypeName
                  + "_"
                  + AString::number(browserWindowIndex));
    
    m_annotationTabWidget              = NULL;
    m_annotationViewController         = NULL;
    m_annotationTextSubstitutionViewController = NULL;
    m_borderSelectionViewController    = NULL;
    m_chartOverlaySetViewController    = NULL;
    m_chartToolBoxViewController       = NULL;
    m_connectivityMatrixViewController = NULL;
    m_fiberOrientationViewController   = NULL;
    m_fociSelectionViewController      = NULL;
    m_imageSelectionViewController     = NULL;
    m_labelSelectionViewController     = NULL;
    m_overlaySetViewController         = NULL;
    m_volumeSurfaceOutlineSetViewController = NULL;

    m_tabWidget = new WuQTabWidgetWithSizeHint();
#if QT_VERSION < 0x050600
    /*
     * Versions of Qt prior to 5.7 do not have QMainWindow::resizeDocks().
     * QDockWidget adjusts its size using the size hint from the child widget
     * and we have have a version of QTabWidget that overrides sizeHint.
     * Without this, the horizontal overlay toolbox uses most of the
     * window height (way too tall).
     */
    switch (toolBoxType) {
        case TOOL_BOX_FEATURES:
            break;
        case TOOL_BOX_OVERLAYS_HORIZONTAL:
            m_tabWidget->setSizeHintHeight(100);
            break;
        case TOOL_BOX_OVERLAYS_VERTICAL:
            break;
    }
#endif
    
    m_annotationTabIndex = -1;
    m_borderTabIndex = -1;
    m_chartOverlayTabIndex = -1;
    m_chartTabIndex = -1;
    m_connectivityTabIndex = -1;
    m_fiberOrientationTabIndex = -1;
    m_fociTabIndex = -1;
    m_imageTabIndex = -1;
    m_labelTabIndex = -1;
    m_overlayTabIndex = -1;
    m_volumeSurfaceOutlineTabIndex = -1;
    
    if (isOverlayToolBox) {
        m_overlaySetViewController = new OverlaySetViewController(orientation,
                                                                      browserWindowIndex,
                                                                      this);  
        m_overlayTabIndex = addToTabWidget(m_overlaySetViewController,
                       "Layers");
    }
    if (isOverlayToolBox) {
        m_chartOverlaySetViewController = new ChartTwoOverlaySetViewController(orientation,
                                                                         browserWindowIndex,
                                                                         this);
        m_chartOverlayTabIndex = addToTabWidget(m_chartOverlaySetViewController,
                                                "Chart Layers");
    }
    if (isOverlayToolBox) {
        m_chartToolBoxViewController = new ChartToolBoxViewController(orientation,
                                                                      browserWindowIndex,
                                                                      this);
        m_chartTabIndex = addToTabWidget(m_chartToolBoxViewController,
                                         "Charting");
    }
    if (isOverlayToolBox) {
        m_connectivityMatrixViewController = new CiftiConnectivityMatrixViewController(orientation,
                                                                                       this);
        m_connectivityTabIndex = addToTabWidget(m_connectivityMatrixViewController,
                             "Connectivity");
    }
    if (isFeaturesToolBox) {
        m_annotationViewController = new AnnotationSelectionViewController(browserWindowIndex,
                                                                           this);
        m_annotationTextSubstitutionViewController = new AnnotationTextSubstitutionViewController(browserWindowIndex,
                                                                                                  this);
        
        m_annotationTabWidget = new QTabWidget();
        m_annotationTabWidget->addTab(m_annotationViewController, "Annotations");
        m_annotationTabWidget->addTab(m_annotationTextSubstitutionViewController, "Substitutions");
        m_annotationTabIndex = addToTabWidget(m_annotationTabWidget,
                                              "Annot");
    }
    if (isFeaturesToolBox) {
        m_borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                                this);
        m_borderTabIndex = addToTabWidget(m_borderSelectionViewController,
                             "Borders");
    }
    
    if (isFeaturesToolBox) {
        m_fiberOrientationViewController = new FiberOrientationSelectionViewController(browserWindowIndex,
                                                                                       this);
        m_fiberOrientationTabIndex = addToTabWidget(m_fiberOrientationViewController,
                       "Fibers");
    }
    
    if (isFeaturesToolBox) {
        m_fociSelectionViewController = new FociSelectionViewController(browserWindowIndex,
                                                                                this);
        m_fociTabIndex = addToTabWidget(m_fociSelectionViewController,
                             "Foci");
    }
    
    if (isFeaturesToolBox) {
        m_imageSelectionViewController = new ImageSelectionViewController(browserWindowIndex,
                                                                          this);
        m_imageTabIndex = addToTabWidget(m_imageSelectionViewController,
                                         "Images");
    }
    
    if (isFeaturesToolBox) {
        m_labelSelectionViewController = new LabelSelectionViewController(browserWindowIndex,
                                                                          this);
        m_labelTabIndex = addToTabWidget(m_labelSelectionViewController,
                       "Labels");
    }
    
    if (isOverlayToolBox) {
        m_volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                                m_browserWindowIndex);
        m_volumeSurfaceOutlineTabIndex = addToTabWidget(m_volumeSurfaceOutlineSetViewController,
                             "Vol/Surf Outline");
    }
        
    setWidget(m_tabWidget);

    if (orientation == Qt::Horizontal) {
        setMinimumHeight(200);
        setMaximumHeight(800);
        //setSizeHintHeight(200);
    }
    else {
        if (isOverlayToolBox) {
            setMinimumWidth(300);
            setMaximumWidth(800);
        }
        else {
            setMinimumWidth(200);
            setMaximumWidth(800);
        }
    }

    QObject::connect(this, SIGNAL(topLevelChanged(bool)),
                     this, SLOT(floatingStatusChanged(bool)));

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Place widget into a scroll area and then into the tab widget.
 * @param page
 *    Widget that is added.
 * @param label
 *    Name corresponding to widget's tab.
 */
int 
BrainBrowserWindowOrientedToolBox::addToTabWidget(QWidget* page,
                                                  const QString& label)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    int indx = m_tabWidget->addTab(scrollArea,
                                       label);
    return indx;
}


/**
 * Called when floating status changes.
 * @param status
 *   New floating status.
 */
void 
BrainBrowserWindowOrientedToolBox::floatingStatusChanged(bool /*status*/)
{
    QString title = m_toolBoxTitle;
    setWindowTitle(title);
}

/**
 * Set the size hint width (non-positive uses default size hint for width)
 *
 * @param width
 *     Width for size hint.
 */
void
BrainBrowserWindowOrientedToolBox::setSizeHintWidth(const int width)
{
    m_sizeHintWidth = width;
    
    if (m_sizeHintWidth > 0) {
        const int32_t childSize = std::max(m_sizeHintWidth - 50, 50);
        m_tabWidget->setSizeHintWidth(childSize);
    }
}

/**
 * Set the size hint height (non-positive uses default size hint for height)
 *
 * @param height
 *     Height for size hint.
 */
void
BrainBrowserWindowOrientedToolBox::setSizeHintHeight(const int height)
{
    m_sizeHintHeight = height;
    
    if (m_sizeHintHeight > 0) {
        const int32_t childSize = std::max(m_sizeHintHeight - 50, 50);
        m_tabWidget->setSizeHintHeight(childSize);
    }
}

/**
 * @return Recommended size for this widget.
 */
QSize
BrainBrowserWindowOrientedToolBox::sizeHint() const
{
    QSize sh = QDockWidget::sizeHint();
    return sh;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
BrainBrowserWindowOrientedToolBox::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainBrowserWindowOrientedToolBox",
                                            1);
    
    AString tabName;
    const int tabIndex = m_tabWidget->currentIndex();
    if ((tabIndex >= 0) 
        && tabIndex < m_tabWidget->count()) {
        tabName = m_tabWidget->tabText(tabIndex);
    }
    sceneClass->addString("selectedTabName",
                          tabName);
    
    if (m_chartToolBoxViewController != NULL) {
        sceneClass->addClass(m_chartToolBoxViewController->saveToScene(sceneAttributes,
                                                                       "m_chartToolBoxViewController"));
    }
    
    /*
     * Save current widget size
     */
    QWidget* childWidget = m_tabWidget->currentWidget();
    if (childWidget != NULL) {
        SceneWindowGeometry swg(childWidget,
                                this);
        sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                             "childWidget"));
    }
    
    /*
     * Save the toolbox
     */
    SceneWindowGeometry swg(this,
                            GuiManager::get()->getBrowserWindowByWindowIndex(this->m_browserWindowIndex));
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
    /**
     * Save the size when visible BUT NOT floating
     */
    if (isFloating() == false) {
        sceneClass->addInteger("toolboxWidth", width());
        sceneClass->addInteger("toolboxHeight", height());
    }
    
    /*
     * Save controllers in the toolbox
     */
    if (m_annotationTabWidget) {
        sceneClass->addInteger("annotationSubTabWidgetIndex",
                               m_annotationTabWidget->currentIndex());
    }
    if (m_annotationViewController != NULL) {
        sceneClass->addClass(m_annotationViewController->saveToScene(sceneAttributes,
                                                                     "m_annotationViewController"));
    }
    if (m_annotationTextSubstitutionViewController != NULL) {
        sceneClass->addClass(m_annotationTextSubstitutionViewController->saveToScene(sceneAttributes,
                                                                                     "m_annotationTextSubstitutionViewController"));
    }
    if (m_borderSelectionViewController != NULL) {
        sceneClass->addClass(m_borderSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_borderSelectionViewController"));
    }
    if (m_fiberOrientationViewController != NULL) {
        sceneClass->addClass(m_fiberOrientationViewController->saveToScene(sceneAttributes,
                                                     "m_fiberOrientationViewController"));
    }
    if (m_fociSelectionViewController != NULL) {
        sceneClass->addClass(m_fociSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_fociSelectionViewController"));
    }
    if (m_imageSelectionViewController != NULL) {
        sceneClass->addClass(m_imageSelectionViewController->saveToScene(sceneAttributes,
                                                                         "m_imageSelectionViewController"));
    }
    if (m_labelSelectionViewController != NULL) {
        sceneClass->addClass(m_labelSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_labelSelectionViewController"));
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
BrainBrowserWindowOrientedToolBox::restoreFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const AString tabName = sceneClass->getStringValue("selectedTabName",
                                                       "");
    for (int32_t i = 0; i < m_tabWidget->count(); i++) {
        if (m_tabWidget->tabText(i) == tabName) {
            m_tabWidget->setCurrentIndex(i);
            break;
        }
    }
    
    if (m_chartToolBoxViewController != NULL) {
        m_chartToolBoxViewController->restoreFromScene(sceneAttributes,
                                                       sceneClass->getClass("m_chartToolBoxViewController"));
    }

    /*
     * Restore controllers in the toolbox
     */
    if (m_annotationTabWidget != NULL) {
        const int32_t subTabIndex = sceneClass->getIntegerValue("annotationSubTabWidgetIndex", 0);
        if (subTabIndex >= 0) {
            m_annotationTabWidget->setCurrentIndex(subTabIndex);
        }
    }
    if (m_annotationViewController != NULL) {
        m_annotationViewController->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_annotationViewController"));
    }
    if (m_annotationTextSubstitutionViewController != NULL) {
        m_annotationTextSubstitutionViewController->restoreFromScene(sceneAttributes, sceneClass->getClass("m_annotationTextSubstitutionViewController"));
    }
    if (m_borderSelectionViewController != NULL) {
        m_borderSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_borderSelectionViewController"));
    }
    if (m_fiberOrientationViewController != NULL) {
        m_fiberOrientationViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_fiberOrientationViewController"));
    }
    if (m_fociSelectionViewController != NULL) {
        m_fociSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_fociSelectionViewController"));
    }
    if (m_imageSelectionViewController != NULL) {
        m_imageSelectionViewController->restoreFromScene(sceneAttributes,
                                                         sceneClass->getClass("m_imageSelectionViewController"));
    }
    if (m_labelSelectionViewController != NULL) {
        m_labelSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_labelSelectionViewController"));
    }
    
    /*
     * Restore current widget size
     */
    QWidget* childWidget = m_tabWidget->currentWidget();
    QSize childMinSize;
    QSize childSize;
    if (childWidget != NULL) {
        childMinSize = childWidget->minimumSize();
        SceneWindowGeometry swg(childWidget,
                                this);
        swg.restoreFromScene(sceneAttributes,
                             sceneClass->getClass("childWidget"));
        childSize = childWidget->size();
    }
    
    if (isFloating() && isVisible()) {
        SceneWindowGeometry swg(this,
                                GuiManager::get()->getBrowserWindowByWindowIndex(this->m_browserWindowIndex));
        swg.restoreFromScene(sceneAttributes,
                             sceneClass->getClass("geometry"));
    }
    else {
        const SceneClass* geometryClass = sceneClass->getClass("geometry");
        if (geometryClass != NULL) {
            /* is restored only when floating and visible. */
            geometryClass->setDescendantsRestored(true);
        }
#if QT_VERSION < 0x050000
        /*
         * Do this for Qt4 only.  Qt5 restores size in BrainBrowserWindow.
         *
         * From http://stackoverflow.com/questions/2722939/c-resize-a-docked-qt-qdockwidget-programmatically
         *
         * Set the minimum and maximum sizes and restore them later.
         * Trying to restore them immediately does not work.  So, as
         * explained in the link above, set the minimum and maximum
         * sizes to that the toolbox is the correct size and then use
         * a timer to restore the correct values for the minimum and
         * maximum sizes after a little delay.
         */
        const int w = sceneClass->getIntegerValue("toolboxWidth", -1);
        const int h = sceneClass->getIntegerValue("toolboxHeight", -1);
        if ((w > 0) && (h > 0)) {
            m_minimumSizeAfterSceneRestored = minimumSize();
            m_maximumSizeAfterSceneRestored = maximumSize();
            
            setMaximumWidth(w);
            setMaximumHeight(h);
            setMinimumWidth(w);
            setMinimumHeight(h);

            QTimer::singleShot(1000,  // 1000 ms => 1 second
                               this,
                               SLOT(restoreMinimumAndMaximumSizesAfterSceneRestored()));
        }
#endif
    }
}

/**
 * This slot is called when restoring a scene
 */
void
BrainBrowserWindowOrientedToolBox::restoreMinimumAndMaximumSizesAfterSceneRestored()
{
    setMinimumSize(m_minimumSizeAfterSceneRestored);
    setMaximumSize(m_maximumSizeAfterSceneRestored);
}


/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
BrainBrowserWindowOrientedToolBox::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();

        Brain* brain = GuiManager::get()->getBrain();
        
        /*
         * Determine types of data this is loaded
         */
        bool haveAnnotation = ( ! brain->getSceneAnnotationFile()->isEmpty());
        bool haveBorders    = false;
        bool haveConnFiles  = false;
        bool haveFibers     = false;
        bool haveFoci       = false;
        bool haveImages     = false;
        bool haveLabels     = false;
        bool haveSurfaces   = false;
        bool haveVolumes    = false;
        
        std::vector<CaretDataFile*> allDataFiles;
        brain->getAllDataFiles(allDataFiles);
        for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
             iter != allDataFiles.end();
             iter++) {
            const CaretDataFile* caretDataFile = *iter;
            
            const DataFileTypeEnum::Enum dataFileType = caretDataFile->getDataFileType();
            switch (dataFileType) {
                case DataFileTypeEnum::ANNOTATION:
                    haveAnnotation = true;
                    break;
                case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                    haveAnnotation = true;
                    break;
                case DataFileTypeEnum::BORDER:
                    haveBorders = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    haveLabels = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                    haveFibers = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                    haveConnFiles = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                    haveLabels = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                    break;
                case DataFileTypeEnum::FOCI:
                    haveFoci = true;
                    break;
                case DataFileTypeEnum::IMAGE:
                    haveImages = true;
                    break;
                case DataFileTypeEnum::LABEL:
                    haveLabels = true;
                    break;
                case DataFileTypeEnum::METRIC:
                    break;
                case DataFileTypeEnum::PALETTE:
                    break;
                case DataFileTypeEnum::RGBA:
                    break;
                case DataFileTypeEnum::SCENE:
                    break;
                case DataFileTypeEnum::SPECIFICATION:
                    break;
                case DataFileTypeEnum::SURFACE:
                    haveSurfaces = true;
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    break;
                case DataFileTypeEnum::VOLUME:
                {
                    haveVolumes = true;
                    
                    const VolumeFile* vf = dynamic_cast<const VolumeFile*>(caretDataFile);
                    CaretAssert(vf);
                    if (vf->isMappedWithLabelTable()) {
                        haveLabels = true;
                    }
                }
                    break;
            }
        }
        
        /*
         * Enable surface volume outline only if have both surfaces and volumes
         * loaded and model being viewed is allows drawing of volume surface
         * outline.
         */
        int defaultTabIndex = -1;
        bool enableLayers = true;
        bool enableVolumeSurfaceOutline = false;
        bool enableChartOne = false;
        bool enableChartTwo = false;
        EventBrowserWindowDrawingContent browserContentEvent(m_browserWindowIndex);
        EventManager::get()->sendEvent(browserContentEvent.getPointer());
        BrowserTabContent* windowContent = browserContentEvent.getSelectedBrowserTabContent();
            if (windowContent != NULL) {
                switch (windowContent->getSelectedModelType()) {
                    case ModelTypeEnum::MODEL_TYPE_INVALID:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE:
                        defaultTabIndex = m_overlayTabIndex;
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                        defaultTabIndex = m_overlayTabIndex;
                        break;
                    case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                        defaultTabIndex = m_overlayTabIndex;
                        enableVolumeSurfaceOutline = (haveSurfaces
                                                      & haveVolumes);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                        defaultTabIndex = m_overlayTabIndex;
                        enableVolumeSurfaceOutline = (haveSurfaces
                                                      & haveVolumes);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_CHART:
                        defaultTabIndex = m_chartTabIndex;
                        enableChartOne = true;
                        enableLayers = false;
                        enableVolumeSurfaceOutline = false;
                        haveBorders = false;
                        haveFibers  = false;
                        haveFoci    = false;
                        haveLabels  = false;
                        break;
                    case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                        defaultTabIndex = m_chartOverlayTabIndex;
                        enableChartTwo = true;
                        enableLayers = false;
                        enableVolumeSurfaceOutline = false;
                        haveBorders = false;
                        haveFibers  = false;
                        haveFoci    = false;
                        haveLabels  = false;
                        break;
                }
            }

        /*
         * Get the selected tab BEFORE enabling/disabling tabs.
         * Otherwise, the enabling/disabling of tabs may cause the selection
         * to change
         */
        const int32_t tabIndex = m_tabWidget->currentIndex();

        
        /*
         * Enable/disable Tabs based upon data that is loaded
         * NOTE: Order is important so that overlay tab is 
         * automatically selected.
         */
        if (m_chartTabIndex >= 0) m_tabWidget->setTabEnabled(m_chartTabIndex, enableChartOne);
        if (m_chartOverlayTabIndex >= 0) m_tabWidget->setTabEnabled(m_chartOverlayTabIndex, enableChartTwo);
        if (m_connectivityTabIndex >= 0) m_tabWidget->setTabEnabled(m_connectivityTabIndex, haveConnFiles);
        if (m_volumeSurfaceOutlineTabIndex >= 0) m_tabWidget->setTabEnabled(m_volumeSurfaceOutlineTabIndex, enableVolumeSurfaceOutline);
        
        if (m_annotationTabIndex >= 0) m_tabWidget->setTabEnabled(m_annotationTabIndex, haveAnnotation);
        if (m_borderTabIndex >= 0) m_tabWidget->setTabEnabled(m_borderTabIndex, haveBorders);
        if (m_fiberOrientationTabIndex >= 0) m_tabWidget->setTabEnabled(m_fiberOrientationTabIndex, haveFibers);
        if (m_fociTabIndex >= 0) m_tabWidget->setTabEnabled(m_fociTabIndex, haveFoci);
        if (m_imageTabIndex >= 0) m_tabWidget->setTabEnabled(m_imageTabIndex, haveImages);
        if (m_labelTabIndex >= 0) m_tabWidget->setTabEnabled(m_labelTabIndex, haveLabels);
        
        if (m_overlayTabIndex >= 0) m_tabWidget->setTabEnabled(m_overlayTabIndex, enableLayers);
        
        /*
         * Switch selected tab if it is not valid
         */
        bool tabIndexValid = false;
        if ((tabIndex >= 0)
            && (tabIndex < m_tabWidget->count())) {
            if (m_tabWidget->isTabEnabled(tabIndex)) {
                tabIndexValid = true;
            }
        }
        
        if ( ! tabIndexValid) {
            if (m_tabWidget->isTabEnabled(defaultTabIndex)) {
                m_tabWidget->setCurrentIndex(defaultTabIndex);
            }
            else {
                for (int i = 0; i < m_tabWidget->count(); ++i) {
                    if (m_tabWidget->isTabEnabled(i)) {
                        m_tabWidget->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
    }
    else {
    }
}


