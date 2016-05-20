

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

#include <cstdlib>

#include "CaretOpenGLInclude.h"

#define __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H
#include "BrainOpenGLFixedPipeline.h"
#undef __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H

#include <algorithm>
#include <limits>
#include <cmath>

#include <QStringList>
#include <QImage>

#include "AnnotationColorBar.h"
#include "AnnotationManager.h"
#include "AnnotationPointSizeText.h"
#include "Border.h"
#include "BorderFile.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLChartDrawingFixedPipeline.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainOpenGLTextureManager.h"
#include "BrainOpenGLVolumeObliqueSliceDrawing.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrainOpenGLShapeCone.h"
#include "BrainOpenGLShapeCube.h"
#include "BrainOpenGLShapeCylinder.h"
#include "BrainOpenGLShapeRing.h"
#include "BrainOpenGLShapeRingOutline.h"
#include "BrainOpenGLShapeSphere.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretPreferences.h"
#include "ChartableMatrixInterface.h"
#include "ChartableMatrixSeriesInterface.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "ClippingPlaneGroup.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayGroupEnum.h"
#include "DisplayPropertiesAnnotation.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesImages.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventAnnotationColorBarGet.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "EventPaletteGetByName.h"
#include "FastStatistics.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "FiberOrientationTrajectory.h"
#include "FiberTrajectoryMapProperties.h"
#include "FociFile.h"
#include "Focus.h"
#include "GapsAndMargins.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "ImageFile.h"
#include "Matrix4x4.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemVoxel.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
#include "IdentificationWithColor.h"
#include "SelectionManager.h"
#include "MathFunctions.h"
#include "ModelChart.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"
#include "Plane.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceMontageViewport.h"
#include "SurfaceNodeColoring.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "SurfaceSelectionModel.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"
#include "VolumeMappableInterface.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;

/*
 * When true,
 */
static bool drawTabAnnotationsAfterTabContentFlag = true;

/**
 * Constructor.
 *
 * @param parentGLWidget
 *   The optional text renderer is used for text rendering.
 *   This parameter may be NULL in which case no text
 *   rendering is performed.
 */
BrainOpenGLFixedPipeline::BrainOpenGLFixedPipeline(const int32_t windowIndex,
                                                   BrainOpenGLTextRenderInterface* textRenderer)
: BrainOpenGL(textRenderer),
m_windowIndex(windowIndex)
{
    CaretAssert((m_windowIndex >= 0)
                && (m_windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    
    this->initializeMembersBrainOpenGL();
    this->colorIdentification   = new IdentificationWithColor();
    m_annotationDrawing.grabNew(new BrainOpenGLAnnotationDrawingFixedPipeline(this));
    m_textureManager.grabNew(new BrainOpenGLTextureManager(m_windowIndex));
                             
    m_shapeSphere = NULL;
    m_shapeCone   = NULL;
    m_shapeCylinder = NULL;
    m_shapeCube   = NULL;
    m_shapeCubeRounded = NULL;
    m_shapeCircleOutline = NULL;
    m_shapeCircleFilled  = NULL;
    this->surfaceNodeColoring = new SurfaceNodeColoring();
    m_brain = NULL;
    m_clippingPlaneGroup = NULL;

    m_tileTabsActiveFlag = false;
    
    setTabViewport(NULL);
}

/**
 * Destructor.
 */
BrainOpenGLFixedPipeline::~BrainOpenGLFixedPipeline()
{
    if (m_shapeSphere != NULL) {
        delete m_shapeSphere;
        m_shapeSphere = NULL;
    }
    if (m_shapeCone != NULL) {
        delete m_shapeCone;
        m_shapeCone = NULL;
    }
    if (m_shapeCylinder != NULL) {
        delete m_shapeCylinder;
        m_shapeCylinder = NULL;
    }
    if (m_shapeCube != NULL) {
        delete m_shapeCube;
        m_shapeCube = NULL;
    }
    if (m_shapeCubeRounded != NULL) {
        delete m_shapeCubeRounded;
        m_shapeCubeRounded = NULL;
    }
    if (m_shapeCircleFilled != NULL) {
        delete m_shapeCircleFilled;
        m_shapeCircleFilled = NULL;
    }
    if (m_shapeCircleOutline != NULL) {
        delete m_shapeCircleOutline;
        m_shapeCircleOutline = NULL;
    }
    if (this->surfaceNodeColoring != NULL) {
        delete this->surfaceNodeColoring;
        this->surfaceNodeColoring = NULL;
    }
    
    for (std::map<float, BrainOpenGLShapeRingOutline*>::iterator iter = m_shapeEllipseOutlines.begin();
         iter != m_shapeEllipseOutlines.end();
         iter++) {
        BrainOpenGLShapeRingOutline* shape = iter->second;
        delete shape;
    }
    m_shapeEllipseOutlines.clear();
    
    delete this->colorIdentification;
    this->colorIdentification = NULL;
}

/**
 * Selection on a model.
 *
 * @param brain
 *    The brain (must be valid!)
 * @param viewportConent
 *    Viewport content in which mouse was clicked
 * @param mouseX
 *    X position of mouse click
 * @param mouseY
 *    Y position of mouse click
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be 
 *    selected.
 */
void 
BrainOpenGLFixedPipeline::selectModel(Brain* brain,
                                      BrainOpenGLViewportContent* viewportContent,
                                      const int32_t mouseX,
                                      const int32_t mouseY,
                                      const bool applySelectionBackgroundFiltering)
{
    m_brain = brain;
    CaretAssert(m_brain);
    
    setTabViewport(viewportContent);
    
    std::vector<BrainOpenGLViewportContent*> viewportContentsVector;
    viewportContentsVector.push_back(viewportContent);
    setAnnotationColorBarsForDrawing(viewportContentsVector);
    
    m_clippingPlaneGroup = NULL;
    
    this->inverseRotationMatrixValid = false;
    
    /*
     * For identification, set the background
     * to white.
     */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    this->mouseX = mouseX;
    this->mouseY = mouseY;
    
    this->colorIdentification->reset();

    this->drawModelInternal(MODE_IDENTIFICATION,
                            viewportContent);

    if (drawTabAnnotationsAfterTabContentFlag) {
        /*
         * Clear depth buffer since tab and window
         * annotations ALWAYS are on top of
         * everything else.
         */
        glClear(GL_DEPTH_BUFFER_BIT);
        drawTabAnnotations(viewportContent); //,
//                           m_tabViewport);
    }
    
    int windowViewport[4];
    viewportContent->getWindowViewport(windowViewport);
    drawWindowAnnotations(windowViewport);
    
    m_brain->getSelectionManager()->filterSelections(applySelectionBackgroundFiltering);
    
    m_brain = NULL;
}

/**
 * Project the given window coordinate to the active models.
 * If the projection is successful, The 'original' XYZ
 * coordinate in 'projectionOut' will be valid.  In addition,
 * the barycentric coordinate may also be valid in 'projectionOut'.
 *
 * @param brain
 *    The brain (must be valid!)
 * @param viewportContent
 *    Viewport content in which mouse was clicked
 * @param mouseX
 *    X position of mouse click
 * @param mouseY
 *    Y position of mouse click
 * @param projectionOut
 *    Contains projection result upon exit.
 */
void 
BrainOpenGLFixedPipeline::projectToModel(Brain* brain,
                                         BrainOpenGLViewportContent* viewportContent,
                                         const int32_t mouseX,
                                         const int32_t mouseY,
                                         SurfaceProjectedItem& projectionOut)
{
    m_brain = brain;
    CaretAssert(m_brain);
    
    setTabViewport(viewportContent);
    
    m_clippingPlaneGroup = NULL;
    
    m_brain->getSelectionManager()->reset();
    
    this->modeProjectionData = &projectionOut;
    this->modeProjectionData->reset();
    this->modeProjectionScreenDepth = std::numeric_limits<double>::max();
    
    /*
     * For projection which uses colors for finding triangles, 
     * set the background to white.
     */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    this->mouseX = mouseX;
    this->mouseY = mouseY;
    
    this->colorIdentification->reset();
    
    this->drawModelInternal(MODE_PROJECTION,
                            viewportContent);
    
    this->modeProjectionData = NULL;
    m_brain = NULL;
}

/**
 * Update the foreground and background colors using the model in 
 * the given viewport content.
 */
void
BrainOpenGLFixedPipeline::updateForegroundAndBackgroundColors(BrainOpenGLViewportContent* vpContent)
{
    /*
     * Default to colors for surface
     */
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->getBackgroundAndForegroundColors()->getColorForegroundSurfaceView(m_foregroundColorByte);
    m_foregroundColorByte[3] = 255;
    prefs->getBackgroundAndForegroundColors()->getColorBackgroundSurfaceView(m_backgroundColorByte);
    m_backgroundColorByte[3] = 255;
    
    if (vpContent != NULL) {
        const BrowserTabContent* btc = vpContent->getBrowserTabContent();
        if (btc != NULL) {
            const Model* model = btc->getModelForDisplay();
            if (model != NULL) {
                switch (model->getModelType()) {
                    case ModelTypeEnum::MODEL_TYPE_CHART:
                        prefs->getBackgroundAndForegroundColors()->getColorForegroundChartView(m_foregroundColorByte);
                        prefs->getBackgroundAndForegroundColors()->getColorBackgroundChartView(m_backgroundColorByte);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_INVALID:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE:
                        prefs->getBackgroundAndForegroundColors()->getColorForegroundSurfaceView(m_foregroundColorByte);
                        prefs->getBackgroundAndForegroundColors()->getColorBackgroundSurfaceView(m_backgroundColorByte);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                        prefs->getBackgroundAndForegroundColors()->getColorForegroundSurfaceView(m_foregroundColorByte);
                        prefs->getBackgroundAndForegroundColors()->getColorBackgroundSurfaceView(m_backgroundColorByte);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                        prefs->getBackgroundAndForegroundColors()->getColorForegroundVolumeView(m_foregroundColorByte);
                        prefs->getBackgroundAndForegroundColors()->getColorBackgroundVolumeView(m_backgroundColorByte);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                        prefs->getBackgroundAndForegroundColors()->getColorForegroundAllView(m_foregroundColorByte);
                        prefs->getBackgroundAndForegroundColors()->getColorBackgroundAllView(m_backgroundColorByte);
                        break;
                }
            }
        }
        m_foregroundColorByte[3] = 255;
        m_backgroundColorByte[3] = 255;
    }
    
    CaretPreferences::byteRgbToFloatRgb(m_backgroundColorByte,
                                        m_backgroundColorFloat);
    m_backgroundColorFloat[3] = 1.0;
    CaretPreferences::byteRgbToFloatRgb(m_foregroundColorByte,
                                        m_foregroundColorFloat);
    m_foregroundColorFloat[3] = 1.0;
}

/**
 * Set the viewport for the tab content that is being drawn.
 * 
 * @param vpContent
 *     The viewport content (may be NULL).
 */
void
BrainOpenGLFixedPipeline::setTabViewport(const BrainOpenGLViewportContent* vpContent)
{
    m_tabViewport[0] = 0;
    m_tabViewport[1] = 0;
    m_tabViewport[2] = 0;
    m_tabViewport[3] = 0;
    
    if (vpContent != NULL) {
        vpContent->getTabViewportBeforeApplyingMargins(m_tabViewport);
    }
}

/**
 * Get colorbars in window space.
 *
 * @param viewportContents
 *     Contents of the viewports.
 * @param windowColorBarsOut
 *     Output with window color bars in the viewports.
 */
void
BrainOpenGLFixedPipeline::setAnnotationColorBarsForDrawing(std::vector<BrainOpenGLViewportContent*>& /*viewportContents*/)
{
    m_annotationColorBarsForDrawing.clear();
    
//    std::vector<int32_t> tabIndices;
//    for (int32_t i = 0; i < static_cast<int32_t>(viewportContents.size()); i++) {
//        /*
//         * Viewport of window.
//         */
//        BrainOpenGLViewportContent* vpContent = viewportContents[i];
//        if (vpContent != NULL) {
//            BrowserTabContent* tabContent = vpContent->getBrowserTabContent();
//            if (tabContent != NULL) {
//                tabIndices.push_back(tabContent->getTabNumber());
//            }
//        }
//    }
    /*
     * When in tile tabs, a tab may have a color bar in window
     * space that appears in a different tab.  So, we need to 
     * get all color bars.  Otherwise, the user will not be able
     * to select a color when it is displayed in a different tab.
     */
    EventAnnotationColorBarGet colorBarEvent;  //tabIndices);
    EventManager::get()->sendEvent(colorBarEvent.getPointer());
    m_annotationColorBarsForDrawing = colorBarEvent.getAnnotationColorBars();
}

/**
 * Draw models in their respective viewports.
 *
 * @param brain
 *    The brain (must be valid!)
 * @param viewportContents
 *    Viewport info for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawModels(Brain* brain,
                                     std::vector<BrainOpenGLViewportContent*>& viewportContents)
{
    m_brain = brain;
    CaretAssert(m_brain);
    
    setTabViewport(NULL);
    
    setAnnotationColorBarsForDrawing(viewportContents);
    
    m_tileTabsActiveFlag = (viewportContents.size() > 1);
    
    this->inverseRotationMatrixValid = false;
    
    m_clippingPlaneGroup = NULL;
    
    this->checkForOpenGLError(NULL, "At beginning of drawModels()");
    
    /*
     * Default the background colors to first model
     * NOTE: If there are no models, the surface background color is used
     */
    if (viewportContents.empty()) {
        updateForegroundAndBackgroundColors(NULL);
    }
    else {
        updateForegroundAndBackgroundColors(viewportContents[0]);
    }
    
    /*
     * Use the background color as the clear color.
     */
    uint8_t clearColorByte[3] = {
        m_backgroundColorByte[0],
        m_backgroundColorByte[1],
        m_backgroundColorByte[2],
    };
    float clearColorFloat[3];
    CaretPreferences::byteRgbToFloatRgb(clearColorByte,
                                        clearColorFloat);
    
    glClearColor(clearColorFloat[0],
                 clearColorFloat[1],
                 clearColorFloat[2],
                 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    this->checkForOpenGLError(NULL, "At middle of drawModels()");
    
    for (int32_t i = 0; i < static_cast<int32_t>(viewportContents.size()); i++) {
        /*
         * Viewport of window.
         */
        BrainOpenGLViewportContent* vpContent = viewportContents[i];
        setTabViewport(vpContent);
        glViewport(m_tabViewport[0], m_tabViewport[1], m_tabViewport[2], m_tabViewport[3]);
        
        /*
         * Update foreground and background colors for model
         */
        updateForegroundAndBackgroundColors(vpContent);
        
        /*
         * If this is NOT the first viewport content,
         * AND the background color for this viewport content is 
         * different that the clear color, THEN
         * draw a rectangle with the background color.
         */
        if (i > 0) {
            if ((m_backgroundColorByte[0] != clearColorByte[0])
                || (m_backgroundColorByte[1] != clearColorByte[1])
                || (m_backgroundColorByte[2] != clearColorByte[2])) {
                GLboolean depthEnabledFlag;
                glGetBooleanv(GL_DEPTH_TEST,
                              &depthEnabledFlag);
                glDisable(GL_DEPTH_TEST);
                
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glColor3ubv(m_backgroundColorByte);
                glBegin(GL_POLYGON);
                glVertex2f(0.0, 0.0);
                glVertex2f(1.0, 0.0);
                glVertex2f(1.0, 1.0);
                glVertex2f(0.0, 1.0);
                glEnd();
                
                if (depthEnabledFlag) {
                    glEnable(GL_DEPTH_TEST);
                }
            }
        }

        drawBackgroundImage(vpContent);
        
        this->drawModelInternal(MODE_DRAWING,
                                vpContent);
        
        /*
         * Draw border in foreground color around tab that is highlighted
         * in Tile Tabs when user selects a tab.
         */
        if (vpContent->isTabHighlighted()) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
//            const float width = windowViewport[2];
//            const float height = windowViewport[3];
            const float width = m_tabViewport[2];
            const float height = m_tabViewport[3];
            glOrtho(0.0, width, 0.0, height, -100.0, 100.0);
            
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            
            glColor3fv(m_foregroundColorFloat);
            
            const float thickness = 10;
            
            /*
             * Left Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(thickness, 0.0, 0.0);
            glVertex3f(thickness, height, 0.0);
            glVertex3f(0.0, height, 0.0);
            glEnd();
            
            /*
             * Right Side
             */
            glBegin(GL_QUADS);
            glVertex3f(width - thickness, 0.0, 0.0);
            glVertex3f(width, 0.0, 0.0);
            glVertex3f(width, height, 0.0);
            glVertex3f(width - thickness, height, 0.0);
            glEnd();
            
            /*
             * Bottom Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(width, 0.0, 0.0);
            glVertex3f(width, thickness, 0.0);
            glVertex3f(0.0, thickness, 0.0);
            glEnd();
            
            /*
             * Top Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, height - thickness, 0.0);
            glVertex3f(width, height - thickness, 0.0);
            glVertex3f(width, height, 0.0);
            glVertex3f(0.0, height, 0.0);
            glEnd();
            
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            
            glMatrixMode(GL_MODELVIEW);
        }
    }
    
    if ( ! viewportContents.empty()) {
        if (drawTabAnnotationsAfterTabContentFlag) {
            /*
             * Clear depth buffer since tab and window
             * annotations ALWAYS are on top of
             * everything else.
             */
            glClear(GL_DEPTH_BUFFER_BIT);
            
            for (int32_t i = 0; i < static_cast<int32_t>(viewportContents.size()); i++) {
                /*
                 * Viewport of window.
                 */
                BrainOpenGLViewportContent* vpContent = viewportContents[i];
                setTabViewport(vpContent);
                
                /*
                 * Update foreground and background colors for model
                 */
                updateForegroundAndBackgroundColors(vpContent);
                
                drawTabAnnotations(vpContent);
                                   //m_tabViewport);
            }
        }
        
        
        /*
         * Draw window viewport annotations
         */
        int windowViewport[4];
        viewportContents[0]->getWindowViewport(windowViewport);
        CaretAssert(m_windowIndex == viewportContents[0]->getWindowIndex());
        drawWindowAnnotations(windowViewport);
    }
    
    this->checkForOpenGLError(NULL, "At end of drawModels()");
    
    m_brain = NULL;
}

/**
 * Draw the tab annotations.
 *
 * @param windowViewport
 *    Viewport (x, y, w, h).
 */
void
BrainOpenGLFixedPipeline::drawTabAnnotations(BrainOpenGLViewportContent* tabContent)
//                                             const int tabViewport[4])
{
    if (tabContent->getBrowserTabContent() == NULL) {
        return;
    }
    
    int tabViewport[4];
    tabContent->getModelViewport(tabViewport);
    CaretAssertMessage(m_brain, "m_brain must NOT be NULL for drawing window annotations.");
    glViewport(tabViewport[0],
               tabViewport[1],
               tabViewport[2],
               tabViewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0.0, tabViewport[2], 0.0, tabViewport[3], -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    CaretAssert(m_windowIndex == tabContent->getWindowIndex());
    this->browserTabContent = tabContent->getBrowserTabContent();
    m_clippingPlaneGroup = const_cast<ClippingPlaneGroup*>(tabContent->getBrowserTabContent()->getClippingPlaneGroup());
    CaretAssert(m_clippingPlaneGroup);
    
    this->windowTabIndex = this->browserTabContent->getTabNumber();
    
    BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                             this->mode,
                                                             BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                             tabViewport,
                                                             m_windowIndex,
                                                             this->windowTabIndex,
                                                             BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::TEXT_HEIGHT_USE_OPENGL_VIEWPORT_HEIGHT,
                                                             BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO);
    m_annotationDrawing->drawAnnotations(&inputs,
                                         AnnotationCoordinateSpaceEnum::TAB,
                                         m_annotationColorBarsForDrawing,
                                         NULL);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw the window annotations.
 *
 * @param windowViewport
 *    Viewport (x, y, w, h).
 */
void
BrainOpenGLFixedPipeline::drawWindowAnnotations(const int windowViewport[4])
{
    CaretAssertMessage(m_brain, "m_brain must NOT be NULL for drawing window annotations.");
    
    /*
     * User may want window annotations only when in tile tabs view.
     */
    BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WindowDrawingMode windowDrawingMode = BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO;
//    bool drawWindowAnnotationsFlag = false;
    if (m_tileTabsActiveFlag) {
//        drawWindowAnnotationsFlag = true;
        windowDrawingMode = BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_YES;
    }
    else {
        const DisplayPropertiesAnnotation* dpa = m_brain->getDisplayPropertiesAnnotation();
        if (dpa->isDisplayWindowAnnotationsInSingleTabViews(m_windowIndex)) {
            windowDrawingMode = BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_YES;
//            drawWindowAnnotationsFlag = true;
        }
    }
    
    glViewport(windowViewport[0],
               windowViewport[1],
               windowViewport[2],
               windowViewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0.0, windowViewport[2], 0.0, windowViewport[3], -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    for (std::vector<AnnotationColorBar*>::iterator iter = m_annotationColorBarsForDrawing.begin();
         iter != m_annotationColorBarsForDrawing.end();
         iter++) {
        AnnotationColorBar* cb = *iter;
        CaretAssert(cb);
        cb->setWindowIndex(m_windowIndex);
    }
    
    /*
     * No valid tab
     */
    this->windowTabIndex = -1;
    
    BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                             this->mode,
                                                             BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                             windowViewport,
                                                             m_windowIndex,
                                                             this->windowTabIndex,
                                                             BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::TEXT_HEIGHT_USE_OPENGL_VIEWPORT_HEIGHT,
                                                             windowDrawingMode);
    
    m_annotationDrawing->drawAnnotations(&inputs,
                                         AnnotationCoordinateSpaceEnum::WINDOW,
                                         m_annotationColorBarsForDrawing,
                                         NULL);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw a model.
 *
 * @param mode
 *    The mode of operations (draw, select, etc).
 * @param viewportContent
 *    Viewport contents for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawModelInternal(Mode mode,
                               BrainOpenGLViewportContent* viewportContent)
{
    ElapsedTimer et;
    et.start();
    
    CaretAssert(m_windowIndex == viewportContent->getWindowIndex());
    this->windowTabIndex = -1;
    
    this->browserTabContent = viewportContent->getBrowserTabContent();
    
    m_mirroredClippingEnabled = false;
    
    Model* model = NULL;
    
    this->mode = mode;
    
    if (this->browserTabContent != NULL) {
        m_clippingPlaneGroup = const_cast<ClippingPlaneGroup*>(this->browserTabContent->getClippingPlaneGroup());
        CaretAssert(m_clippingPlaneGroup);
        
        Model* model = this->browserTabContent->getModelForDisplay();
        this->windowTabIndex = this->browserTabContent->getTabNumber();
        int viewport[4];
        viewportContent->getModelViewport(viewport);
        
        
        this->mode = mode;
        
        this->checkForOpenGLError(model, "At beginning of drawModelInternal()");
        
        if(model != NULL) {
            CaretAssert((this->windowTabIndex >= 0) && (this->windowTabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
            
            bool modelAllowsPalettes = true;
            
            ModelChart* modelChart = dynamic_cast<ModelChart*>(model);
            ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(model);
            ModelSurfaceMontage* surfaceMontageModel = dynamic_cast<ModelSurfaceMontage*>(model);
            ModelVolume* volumeModel = dynamic_cast<ModelVolume*>(model);
            ModelWholeBrain* wholeBrainModel = dynamic_cast<ModelWholeBrain*>(model);
            if (modelChart != NULL) {
                drawChartData(browserTabContent, modelChart, viewport);
                modelAllowsPalettes = true;
            }
            else if (surfaceModel != NULL) {
                m_mirroredClippingEnabled = true;
                this->drawSurfaceModel(surfaceModel, viewport);
            }
            else if (surfaceMontageModel != NULL) {
                m_mirroredClippingEnabled = true;
                this->drawSurfaceMontageModel(browserTabContent,
                                              surfaceMontageModel, 
                                              viewport);
            }
            else if (volumeModel != NULL) {
                this->drawVolumeModel(browserTabContent,
                                           volumeModel, viewport);
            }
            else if (wholeBrainModel != NULL) {
                this->drawWholeBrainModel(browserTabContent,
                                               wholeBrainModel, viewport);
            }
            else {
                modelAllowsPalettes = false;
                CaretAssertMessage(0, "Unknown type of model for drawing");
            }
            
            
            int viewport[4];
            viewportContent->getModelViewport(viewport);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            if (modelAllowsPalettes) {
                this->drawAllPalettes(model->getBrain());
            }
            
            BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                                     this->mode,
                                                                     BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                                     m_tabViewport,
                                                                     m_windowIndex,
                                                                     this->windowTabIndex,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::TEXT_HEIGHT_USE_OPENGL_VIEWPORT_HEIGHT,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO);
            if ( ! drawTabAnnotationsAfterTabContentFlag) {
                m_annotationDrawing->drawAnnotations(&inputs,
                                                     AnnotationCoordinateSpaceEnum::TAB,
                                                     m_annotationColorBarsForDrawing,
                                                     NULL);
            }
        }
    }
    
    glFlush();
    
    this->checkForOpenGLError(model, "At end of drawModelInternal()");
    
    if (model != NULL) {
        CaretLogFine("Time to draw " 
                    + model->getNameForGUI(false)
                    + " was "
                    + AString::number(et.getElapsedTimeSeconds())
                    + " seconds");
    }
}

/**
 * @return Get the texture manager.
 */
BrainOpenGLTextureManager*
BrainOpenGLFixedPipeline::getTextureManager()
{
    BrainOpenGLTextureManager* tm = m_textureManager.getPointer();
    CaretAssert(tm);
    return tm;
}

/**
 * Set the viewport.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjection(const int32_t viewport[4],
                                          const  ProjectionViewTypeEnum::Enum projectionType)
{
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    this->setOrthographicProjection(viewport,
                                    projectionType);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the viewport for a volume.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 * @param volume
 *    Volume for use in setting orthographic projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjectionForWholeBrainVolume(const int32_t viewport[4],
                                                                        const  ProjectionViewTypeEnum::Enum projectionType,
                                                                        const VolumeMappableInterface* volume)
{
    CaretAssert(volume);
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    BoundingBox boundingBox;
    volume->getVoxelSpaceBoundingBox(boundingBox);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setOrthographicProjectionForWithBoundingBox(viewport,
                                                projectionType,
                                                &boundingBox);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the viewport for a surface file.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 * @param surfaceFile
 *    Surface file for use in setting orthographic projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjectionForSurfaceFile(const int32_t viewport[4],
                                                                             const  ProjectionViewTypeEnum::Enum projectionType,
                                                                             const SurfaceFile* surfaceFile)
{
    CaretAssert(surfaceFile);
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setOrthographicProjectionForWithBoundingBox(viewport,
                                            projectionType,
                                            surfaceFile->getBoundingBox());
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Disable clipping planes.
 */
void
BrainOpenGLFixedPipeline::disableClippingPlanes()
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
}

/**
 * Apply the clipping planes for the given data type and structure.
 *
 * @param clippingDataType
 *    Type of data that is being clipped.
 * @param structureIn
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::applyClippingPlanes(const ClippingDataType clippingDataType,
                                              const StructureEnum::Enum structureIn)
{
    disableClippingPlanes();

    StructureEnum::Enum structure = StructureEnum::CORTEX_LEFT;
    if (m_mirroredClippingEnabled) {
        structure = structureIn;
    }
    
    if (browserTabContent == NULL) {
        return;
    }
    
    CaretAssert(m_clippingPlaneGroup);
    
    float rotation[3];
    m_clippingPlaneGroup->getRotationAngles(rotation);

    float panning[3];
    m_clippingPlaneGroup->getTranslationForStructure(structure,
                                                   panning);
    
    float rotationAngles[3];
    m_clippingPlaneGroup->getRotationAngles(rotationAngles);
    
    float thickness[3];
    m_clippingPlaneGroup->getThickness(thickness);
    
    float minX = -thickness[0] / 2.0;
    float maxX =  thickness[0] / 2.0;
    float minY = -thickness[1] / 2.0;
    float maxY =  thickness[1] / 2.0;
    float minZ = -thickness[2] / 2.0;
    float maxZ =  thickness[2] / 2.0;
    
    GLfloat rotMatrix[16];
    m_clippingPlaneGroup->getRotationMatrixForStructure(structure).getMatrixForOpenGL(rotMatrix);

    if (m_clippingPlaneGroup->isDisplayClippingBoxSelected()) {
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(2.0);
        glPushMatrix();
        glTranslatef(panning[0], panning[1], panning[2]);
        
        glMultMatrixf(rotMatrix);
        
        glBegin(GL_LINE_LOOP);
        glVertex3f(minX, minY, minZ);
        glVertex3f(maxX, minY, minZ);
        glVertex3f(maxX, maxY, minZ);
        glVertex3f(minX, maxY, minZ);
        glEnd();
        
        glBegin(GL_LINE_LOOP);
        glVertex3f(minX, minY, maxZ);
        glVertex3f(maxX, minY, maxZ);
        glVertex3f(maxX, maxY, maxZ);
        glVertex3f(minX, maxY, maxZ);
        glEnd();
        
        glBegin(GL_LINES);
        glVertex3f(minX, minY, minZ);
        glVertex3f(minX, minY, maxZ);
        glVertex3f(maxX, minY, minZ);
        glVertex3f(maxX, minY, maxZ);
        glVertex3f(maxX, maxY, minZ);
        glVertex3f(maxX, maxY, maxZ);
        glVertex3f(minX, maxY, minZ);
        glVertex3f(minX, maxY, maxZ);
        glEnd();
        glPopMatrix();
    }
    
    switch (clippingDataType) {
        case CLIPPING_DATA_TYPE_FEATURES:
            if (! m_clippingPlaneGroup->isFeaturesSelected()) {
                return;
            }
            break;
        case CLIPPING_DATA_TYPE_SURFACE:
            if (! m_clippingPlaneGroup->isSurfaceSelected()) {
                return;
            }
            break;
        case CLIPPING_DATA_TYPE_VOLUME:
            if (! m_clippingPlaneGroup->isVolumeSelected()) {
                return;
            }
            break;
    }
    
    std::vector<const Plane*> planes = m_clippingPlaneGroup->getActiveClippingPlanesForStructure(structure);
    const int32_t numPlanes = static_cast<int32_t>(planes.size());
    for (int32_t i = 0; i < numPlanes; i++) {
        const Plane* p = planes[i];
        double a, b, c, d;
        p->getPlane(a, b, c, d);
        const GLdouble abcd[4] = { a, b, c, d };
        glClipPlane(GL_CLIP_PLANE0 + i,
                    abcd);
        glEnable(GL_CLIP_PLANE0 + i);
    }
}

/**
 * Is the coordinate inside the clipping planes?
 *
 * If a clipping plane for an axis is off, the coordinate is considered
 * to be inside the clipping plane.
 *
 * @param structureIn
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 * @param xyz
 *     The coordinate.
 *
 * @return
 *     True if inside the clipping planes, else false.
 */
bool
BrainOpenGLFixedPipeline::isCoordinateInsideClippingPlanesForStructure(const StructureEnum::Enum structureIn,
                                                                       const float xyz[3]) const
{
    CaretAssert(m_clippingPlaneGroup);
    
    StructureEnum::Enum structure = StructureEnum::CORTEX_LEFT;
    if (m_mirroredClippingEnabled) {
        structure = structureIn;
    }
    
    return m_clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(structure, xyz);
}

/**
 * @return Is clipping of features enabled?
 */
bool
BrainOpenGLFixedPipeline::isFeatureClippingEnabled() const
{
    CaretAssert(m_clippingPlaneGroup);
    return m_clippingPlaneGroup->isFeaturesAndAnyAxisSelected();
}

/**
 * Apply the viewing transformations for the content of the browser tab.
 *
 * @param model
 *    Model that is being drawn.
 * @param objectCenterXYZ
 *    If not NULL, contains center of object about
 *    which rotation should take place.
 * @param projectionViewType
 *    Projection view type.
 */
void
BrainOpenGLFixedPipeline::applyViewingTransformations(const Model* model,
                                                      const float objectCenterXYZ[3],
                                                      const ProjectionViewTypeEnum::Enum projectionViewType)
{
    CaretAssert(model);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = 0.0;
    float eyeY = 0.0;
    float eyeZ = 0.0;
    
    float centerX = 0.0;
    float centerY = 0.0;
    float centerZ = 0.0;
    
    float upX = 0.0;
    float upY = 0.0;
    float upZ = 0.0;
    
    bool useGluLookAt = false;
    bool rightCortexFlatFlag = false;
    switch (projectionViewType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            eyeZ = BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance; //5.0;
            upY  = 1.0;
            useGluLookAt = true;
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            eyeZ = BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance; //5.0;
            upY  = 1.0;
            useGluLookAt = true;
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            if (model->getModelType() == ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE) {
                const ModelSurfaceMontage* surfaceMontageModel = dynamic_cast<const ModelSurfaceMontage*>(model);
                CaretAssert(surfaceMontageModel);
                if (surfaceMontageModel != NULL) {
                    if (surfaceMontageModel->getSelectedConfigurationType(this->windowTabIndex) ==  SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION) {
                        rightCortexFlatFlag = true;
                    }
                }
            }
            break;
    }

    if (useGluLookAt) {
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
    }

    
    float translation[3];
    double rotationMatrixElements[16];
    float scaling;
    browserTabContent->getTransformationsForOpenGLDrawing(projectionViewType,
                                                          translation,
                                                          rotationMatrixElements,
                                                          scaling);
    
    glTranslatef(translation[0],
                 translation[1],
                 translation[2]);
    
//    if (rightCortexFlatFlag) {
//        /*
//         * When drawing right flat, the translation is "left translation"
//         * so need to flip sign of X-offset.
//         */
//        float rightFlatOffsetX, rightFlatOffsetY;
//        browserTabContent->getRightCortexFlatMapOffset(rightFlatOffsetX, rightFlatOffsetY);
//        glTranslatef(-rightFlatOffsetX, rightFlatOffsetY, 0.0);
//    }
    
    glMultMatrixd(rotationMatrixElements);
    
    /*
     * Save the inverse rotation matrix which may be used
     * later by some drawing functions.
     */
    Matrix4x4 inverseMatrix;
    inverseMatrix.setMatrixFromOpenGL(rotationMatrixElements);
    this->inverseRotationMatrixValid = inverseMatrix.invert();
    if (this->inverseRotationMatrixValid) {
        inverseMatrix.getMatrixForOpenGL(this->inverseRotationMatrix);
    }
    
    glScalef(scaling,
             scaling,
             scaling);
    
    if (rightCortexFlatFlag) {
        /*
         * When drawing right flat, the translation is "left translation"
         * so need to flip sign of X-offset.
         */
        float rightFlatOffsetX, rightFlatOffsetY;
        browserTabContent->getRightCortexFlatMapOffset(rightFlatOffsetX, rightFlatOffsetY);
        glTranslatef(-rightFlatOffsetX, rightFlatOffsetY, 0.0);
        
        const float rightFlatMapZoomFactor = browserTabContent->getRightCortexFlatMapZoomFactor();
        glScalef(rightFlatMapZoomFactor, rightFlatMapZoomFactor, rightFlatMapZoomFactor);
    }
    
    if (objectCenterXYZ != NULL) {
        /*
         * Place center of surface at origin.
         */
        glTranslatef(-objectCenterXYZ[0], -objectCenterXYZ[1], -objectCenterXYZ[2]);
    }
}

/**
 * For a volume, get translation and scaling so that the volume 'fills' 
 * the window.
 *
 * @param volume
 *   The volume.
 * @param sliceViewPlane
 *   The slice viewing plane.
 * @param orthographicExtent
 *   The orthographic bounds
 * @param translationOut
 *   Output of translation.
 * @param scalingOut
 *   Output of scaling.
 *
 */
void
BrainOpenGLFixedPipeline::getVolumeFitToWindowScalingAndTranslation(const VolumeMappableInterface* volume,
                                                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                    const double orthographicExtent[6],
                                                                    float translationOut[3],
                                                                    float& scalingOut) const
{
    /*
     * Apply some scaling and translation so that the volume slice, by default
     * is not larger than the window in which it is being viewed.
     */
    scalingOut = 1.0;
    translationOut[0] = 0.0;
    translationOut[1] = 0.0;
    translationOut[2] = 0.0;
    
    if (volume != NULL) {
        BoundingBox boundingBox;
        volume->getVoxelSpaceBoundingBox(boundingBox);
        
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volume->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        if ((dimI > 2)
            && (dimJ > 2)
            && (dimK > 2)) {
            
            float volumeCenter[3] = { (boundingBox.getMinX() + boundingBox.getMaxX()) / 2,
                (boundingBox.getMinY() + boundingBox.getMaxY()) / 2,
                (boundingBox.getMinZ() + boundingBox.getMaxZ()) / 2 };
            
            /*
             * Translate so that the center voxel (by dimenisons)
             * is at the center of the screen.
             */
            translationOut[0] = -volumeCenter[0];
            translationOut[1] = -volumeCenter[1];
            translationOut[2] = -volumeCenter[2];
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    translationOut[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    translationOut[1] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    translationOut[0] = 0.0;
                    break;
            }
            
            
            /*
             * Scale so volume fills, but does not extend out of window.
             */
            const float xExtent = (boundingBox.getMaxX() - boundingBox.getMinX()) / 2;
            const float yExtent = (boundingBox.getMaxY() - boundingBox.getMinY()) / 2;
            const float zExtent = (boundingBox.getMaxZ() - boundingBox.getMinZ()) / 2;
            
            const float orthoExtentX = std::min(std::fabs(orthographicExtent[0]),
                                                std::fabs(orthographicExtent[1]));
            const float orthoExtentY = std::min(std::fabs(orthographicExtent[2]),
                                                std::fabs(orthographicExtent[3]));
            
            float temp;
            float scaleWindowX = (orthoExtentX / xExtent);
            temp = (orthoExtentX / yExtent);//parasaggital y is screen x
            if (temp < scaleWindowX) scaleWindowX = temp;
            float scaleWindowY = (orthoExtentY / zExtent);
            temp = (orthoExtentY / yExtent);//axial y is screen y
            if (temp < scaleWindowY) scaleWindowY = temp;
            scalingOut = std::min(scaleWindowX,
                                          scaleWindowY);
            scalingOut *= 0.98;
        }
    }
}

void 
BrainOpenGLFixedPipeline::initializeMembersBrainOpenGL()
{
    this->initializedOpenGLFlag = false;
    this->modeProjectionData = NULL;
}
/**
 * Initialize OpenGL.
 */
void 
BrainOpenGLFixedPipeline::initializeOpenGL()
{
    if (s_staticInitialized == false) {
        s_staticInitialized = true;
        
        BrainOpenGL::initializeOpenGL();
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glFrontFace(GL_CCW);
    
    /*
     * As normal vectors are multiplied by transformation matrices, their 
     * lengths may no longer be one and cause drawing errors.
     * GL_NORMALIZE will rescale normal vectors to one to prevent this problem.
     * GL_RESCALE_NORMAL was added in later versions of OpenGL and
     * is reported to be more efficient.  However, GL_RESCALE_NORMAL 
     * does not seem to work with OpenGL 4.2 on Linux, whereas GL_NORMALIZE
     * seems to work on all operating systems and versions of OpenGL.
     */    
    glEnable(GL_NORMALIZE);
    
    /*
     * Avoid drawing backfacing polygons
     */
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    glShadeModel(GL_SMOOTH);
    
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    float lightColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    
    float materialColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    
    float ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient); 
        
    if (m_shapeSphere == NULL) {
        m_shapeSphere = new BrainOpenGLShapeSphere(5,
                                                   0.5);
    }
    if (m_shapeCone == NULL) {
        m_shapeCone = new BrainOpenGLShapeCone(8);
    }
    
    if (m_shapeCylinder == NULL) {
        m_shapeCylinder = new BrainOpenGLShapeCylinder(8);
    }
    
    if (m_shapeCube == NULL) {
        m_shapeCube = new BrainOpenGLShapeCube(1.0,
                                               BrainOpenGLShapeCube::NORMAL);
    }
    if (m_shapeCubeRounded == NULL) {
        m_shapeCubeRounded = new BrainOpenGLShapeCube(1.0,
                                                      BrainOpenGLShapeCube::ROUNDED);
    }
    if (m_shapeCircleOutline == NULL) {
        m_shapeCircleOutline = new BrainOpenGLShapeRing(20,
                                                     0.9,
                                                     1.0);
    }
    if (m_shapeCircleFilled == NULL) {
        m_shapeCircleFilled = new BrainOpenGLShapeRing(20,
                                                        0.0,
                                                        1.0);
    }
    
    if (this->initializedOpenGLFlag) {
        return;
    }
    this->initializedOpenGLFlag = true;
    
    /*
     * Remaining items need to executed only once.
     */
}

/**
 * Enable lighting based upon the current mode.
 */
void 
BrainOpenGLFixedPipeline::enableLighting()
{
    float lightPosition[] = { 0.0f, 0.0f, 1000.0f, 0.0f };
    switch (this->mode) {
        case MODE_DRAWING:
            glPushMatrix();
            glLoadIdentity();
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
            glEnable(GL_LIGHT0);
            
            /*
             * Light 1 position is opposite of light 0
             */
            lightPosition[0] = -lightPosition[0];
            lightPosition[1] = -lightPosition[1];
            lightPosition[2] = -lightPosition[2];
            glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);      
            glEnable(GL_LIGHT1);
            glPopMatrix();
            
            glEnable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            break;
        case MODE_IDENTIFICATION:
        case MODE_PROJECTION:
            this->disableLighting();
            break;
    }
}

/**
 * Disable lighting.
 */
void 
BrainOpenGLFixedPipeline::disableLighting()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

/**
 * Enable line anti-aliasing (line smoothing) which also required blending.
 */
void 
BrainOpenGLFixedPipeline::enableLineAntiAliasing()
{
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

/**
 * Disable line anti-aliasing (line smoothing) which also required blending.
 */
void 
BrainOpenGLFixedPipeline::disableLineAntiAliasing()
{
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}

/**
 * Draw contents of a surface model.
 * @param surfaceModel
 *    Model that is drawn.
 * @param viewport
 *    Viewport for drawing region.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceModel(ModelSurface* surfaceModel,
                                   const int32_t viewport[4])
{
    Surface* surface = surfaceModel->getSurface();
    float center[3];
    surface->getBoundingBox()->getCenter(center);

    this->setViewportAndOrthographicProjectionForSurfaceFile(viewport,
                                                             browserTabContent->getProjectionViewType(),
                                                             surface);
    
    this->applyViewingTransformations(surfaceModel,
                                      center,
                                      browserTabContent->getProjectionViewType());
    
    const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceModel, 
                                                                                 surface, 
                                                                                 this->windowTabIndex);
    
    this->drawSurface(surface,
                      nodeColoringRGBA,
                      true);
}

/**
 * Draw the surface axes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceAxes()
{
    const float bigNumber = 1000000.0;
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-bigNumber, 0.0, 0.0);
    glVertex3f( bigNumber, 0.0, 0.0);
    glVertex3f(0.0, -bigNumber, 0.0);
    glVertex3f(0.0,  bigNumber, 0.0);
    glVertex3f(0.0, 0.0, -bigNumber);
    glVertex3f(0.0, 0.0, bigNumber);
    glEnd();
    glPopMatrix();
    
}

/**
 * Draw a surface.
 *
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 * @param drawAnnotationsInModelSpaceFlag
 *    If true, draw annotations in model space.
 */
void 
BrainOpenGLFixedPipeline::drawSurface(Surface* surface,
                                      const float* nodeColoringRGBA,
                                      const bool drawAnnotationsInModelSpaceFlag)
{
    const DisplayPropertiesSurface* dps = m_brain->getDisplayPropertiesSurface();
    
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    
    applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_SURFACE,
                        surface->getStructure());
    
    this->enableLighting();
    
    const SurfaceDrawingTypeEnum::Enum drawingType = dps->getSurfaceDrawingType();
    switch (this->mode)  {
        case MODE_DRAWING:
        {
            switch (drawingType) {
                case SurfaceDrawingTypeEnum::DRAW_HIDE:
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_LINKS:
                    /*
                     * Draw first as triangles without coloring which uses
                     * the background color.  This prevents edges on back 
                     * from being seen.
                     */
                    glPolygonOffset(1.0, 1.0);
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    disableLighting();
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               NULL);
                    glDisable(GL_POLYGON_OFFSET_FILL);
                    
                    /*
                     * Now draw as polygon but outline only, do not fill.
                     */
                    enableLighting();
                    setLineWidth(dps->getLinkSize());
                    glPolygonMode(GL_FRONT, GL_LINE);
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               nodeColoringRGBA);
                    glPolygonMode(GL_FRONT, GL_FILL);
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_NODES:
                    this->drawSurfaceNodes(surface,
                                           nodeColoringRGBA);
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_TRIANGLES:
                    /*
                     * Enable alpha blending so that surface transparency
                     * (using first overlay opacity) will function.
                     */
                    GLboolean blendingEnabled = false;
                    glGetBooleanv(GL_BLEND, &blendingEnabled);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               nodeColoringRGBA);
                    if (blendingEnabled == false) {
                        glDisable(GL_BLEND);
                    }
                    break;
            }
            
            this->disableClippingPlanes();
            
            if (dps->isDisplayNormalVectors()) {
                drawSurfaceNormalVectors(surface);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);
            this->drawSurfaceBorderBeingDrawn(surface);

            /*
             * Draw annotations for this surface and maybe draw
             * the model annotations.
             */
            BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                                     this->mode,
                                                                     BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                                     m_tabViewport,
                                                                     m_windowIndex,
                                                                     this->windowTabIndex,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::TEXT_HEIGHT_USE_TAB_VIEWPORT_HEIGHT,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO);
            std::vector<AnnotationColorBar*> emptyColorBars;
            m_annotationDrawing->drawAnnotations(&inputs,
                                                 AnnotationCoordinateSpaceEnum::SURFACE,
                                                 emptyColorBars,
                                                 surface);
            if (drawAnnotationsInModelSpaceFlag) {
                m_annotationDrawing->drawAnnotations(&inputs,
                                                     AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                                     emptyColorBars,
                                                     NULL);
            }
        }
            break;
        case MODE_IDENTIFICATION:
        {
            /*
             * Disable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_FLAT); 
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceNodes(surface,
                                       nodeColoringRGBA);
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }

            this->disableClippingPlanes();
            
            if (dps->isDisplayNormalVectors()) {
                drawSurfaceNormalVectors(surface);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);

            /*
             * Draw annotations for this surface and maybe draw
             * the model annotations.
             */
            BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                                     this->mode,
                                                                     BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                                     m_tabViewport,
                                                                     m_windowIndex,
                                                                     this->windowTabIndex,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::TEXT_HEIGHT_USE_TAB_VIEWPORT_HEIGHT,
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO);
            std::vector<AnnotationColorBar*> emptyColorBars;
            m_annotationDrawing->drawAnnotations(&inputs,
                                                 AnnotationCoordinateSpaceEnum::SURFACE,
                                                 emptyColorBars,
                                                 surface);
            if (drawAnnotationsInModelSpaceFlag) {
                m_annotationDrawing->drawAnnotations(&inputs,
                                                     AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                                     emptyColorBars,
                                                     NULL);
            }
            
            /*
             * Re-enable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_SMOOTH);
        }
            break;
        case MODE_PROJECTION:
            /*
             * Disable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_FLAT);
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }
            /*
             * Re-enable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_SMOOTH);
            break;
    }
    
    this->disableLighting();
    
    this->disableClippingPlanes();
}

/**
 * Draw a surface as individual triangles.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceTriangles(Surface* surface,
                                               const float* nodeColoringRGBA)
{
    const int numTriangles = surface->getNumberOfTriangles();
    
    const int32_t* triangles = surface->getTriangle(0);
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);

    SelectionItemSurfaceTriangle* triangleID = NULL;
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    bool isProjection = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            triangleID = m_brain->getSelectionManager()->getSurfaceTriangleIdentification();
            if (triangleID->isEnabledForSelection()) {
                isSelect = true;
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            isSelect = true;
            isProjection = true;
            break;
    }
    
    if (isSelect) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    uint8_t rgba[4];
    
    glBegin(GL_TRIANGLES);
    for (int32_t i = 0; i < numTriangles; i++) {
        const int32_t i3 = i * 3;
        const int32_t n1 = triangles[i3];
        const int32_t n2 = triangles[i3+1];
        const int32_t n3 = triangles[i3+2];
        
        if (isSelect) {
            this->colorIdentification->addItem(rgba, SelectionItemDataTypeEnum::SURFACE_TRIANGLE, i);
            glColor3ubv(rgba);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
        else {
            glColor4fv(&nodeColoringRGBA[n1*4]);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glColor4fv(&nodeColoringRGBA[n2*4]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glColor4fv(&nodeColoringRGBA[n3*4]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        int32_t triangleIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_TRIANGLE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         triangleIndex,
                                         depth);
        
        
        if (triangleIndex >= 0) {
            bool isTriangleIdAccepted = false;
            if (triangleID != NULL) {
                if (triangleID->isOtherScreenDepthCloserToViewer(depth)) {
                    triangleID->setBrain(surface->getBrainStructure()->getBrain());
                    triangleID->setSurface(surface);
                    triangleID->setTriangleNumber(triangleIndex);
                    const int32_t* triangleNodeIndices = surface->getTriangle(triangleIndex);
                    triangleID->setNearestNode(triangleNodeIndices[0]);
                    triangleID->setScreenDepth(depth);
                    isTriangleIdAccepted = true;
                    CaretLogFine("Selected Triangle: " + triangleID->toString());   
                }
                else {
                    CaretLogFine("Rejecting Selected Triangle but still using: " + triangleID->toString());   
                }
            }
            
            /*
             * Node indices
             */
            const int32_t n1 = triangles[triangleIndex*3];
            const int32_t n2 = triangles[triangleIndex*3 + 1];
            const int32_t n3 = triangles[triangleIndex*3 + 2];
            
            /*
             * Node coordinates
             */
            const float* c1 = &coordinates[n1*3];
            const float* c2 = &coordinates[n2*3];
            const float* c3 = &coordinates[n3*3];
            
            const float average[3] = {
                c1[0] + c2[0] + c3[0],
                c1[1] + c2[1] + c3[1],
                c1[2] + c2[2] + c3[2]
            };
            if (triangleID != NULL) {
                if (isTriangleIdAccepted) {
                    this->setSelectedItemScreenXYZ(triangleID, average);
                }
            }
                   
            GLdouble selectionModelviewMatrix[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, selectionModelviewMatrix);
            
            GLdouble selectionProjectionMatrix[16];
            glGetDoublev(GL_PROJECTION_MATRIX, selectionProjectionMatrix);
            
            GLint selectionViewport[4];
            glGetIntegerv(GL_VIEWPORT, selectionViewport);
            
            /*
             * Window positions of each coordinate
             */
            double dc1[3] = { c1[0], c1[1], c1[2] };
            double dc2[3] = { c2[0], c2[1], c2[2] };
            double dc3[3] = { c3[0], c3[1], c3[2] };
            double wc1[3], wc2[3], wc3[3];
            if (gluProject(dc1[0], 
                           dc1[1], 
                           dc1[2],
                           selectionModelviewMatrix,
                           selectionProjectionMatrix,
                           selectionViewport,
                           &wc1[0],
                           &wc1[1],
                           &wc1[2])
                && gluProject(dc2[0], 
                              dc2[1], 
                              dc2[2],
                              selectionModelviewMatrix,
                              selectionProjectionMatrix,
                              selectionViewport,
                              &wc2[0],
                              &wc2[1],
                              &wc2[2])
                && gluProject(dc3[0], 
                              dc3[1], 
                              dc3[2],
                              selectionModelviewMatrix,
                              selectionProjectionMatrix,
                              selectionViewport,
                              &wc3[0],
                              &wc3[1],
                              &wc3[2])) {
                    const double d1 = MathFunctions::distanceSquared2D(wc1[0], 
                                                                       wc1[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    const double d2 = MathFunctions::distanceSquared2D(wc2[0], 
                                                                       wc2[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    const double d3 = MathFunctions::distanceSquared2D(wc3[0], 
                                                                       wc3[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    if (triangleID != NULL) {
                        if (isTriangleIdAccepted) {
                            triangleID->setNearestNode(n3);
                            triangleID->setNearestNodeScreenXYZ(wc3);
                            triangleID->setNearestNodeModelXYZ(dc3);
                            if ((d1 < d2) && (d1 < d3)) {
                                triangleID->setNearestNode(n1);
                                triangleID->setNearestNodeScreenXYZ(wc1);
                                triangleID->setNearestNodeModelXYZ(dc1);
                            }
                            else if ((d2 < d1) && (d2 < d3)) {
                                triangleID->setNearestNode(n2);
                                triangleID->setNearestNodeScreenXYZ(wc2);
                                triangleID->setNearestNodeModelXYZ(dc2);
                            }
                        }
                    }
                    
                    /*
                     * Getting projected position?
                     */
                    if (isProjection) {
                        /*
                         * Place window coordinates of triangle's nodes
                         * onto the screen by setting Z-coordinate to zero
                         */
                        wc1[2] = 0.0;
                        wc2[2] = 0.0;
                        wc3[2] = 0.0;
                        
                        /*
                         * Area of triangle when projected to display
                         */
                        const double triangleDisplayArea = 
                            MathFunctions::triangleArea(wc1, wc2, wc3);
                        
                        /*
                         * If area of triangle on display is small,
                         * use a coordinate from the triangle
                         */
                        if (triangleDisplayArea < 0.001) {
                            float barycentricAreas[3] = { 1.0, 0.0, 0.0 };
                            int barycentricNodes[3] = { n1, n1, n1 };
                            
                            this->setProjectionModeData(depth, 
                                                        c1, 
                                                        surface->getStructure(), 
                                                        barycentricAreas, 
                                                        barycentricNodes, 
                                                        surface->getNumberOfNodes());
                        }
                        else {
                            /*
                             * Determine position in triangle using barycentric coordinates
                             */
                            double displayXYZ[3] = { 
                                this->mouseX, 
                                this->mouseY, 
                                0.0 
                            };
                            
                            const double areaU = (MathFunctions::triangleArea(displayXYZ, wc2, wc3)
                                                  / triangleDisplayArea);
                            const double areaV = (MathFunctions::triangleArea(displayXYZ, wc3, wc1)
                                                  / triangleDisplayArea);
                            const double areaW = (MathFunctions::triangleArea(displayXYZ, wc1, wc2)
                                                  / triangleDisplayArea);
                            double totalArea = areaU + areaV + areaW;
                            if (totalArea <= 0) {
                                totalArea = 1.0;
                            }
                            if ((areaU < 0.0) || (areaV < 0.0) || (areaW < 0.0)) {
                                CaretLogWarning("Invalid tile area: less than zero when projecting to surface.");
                            }
                            else {
                                /*
                                 * Convert to surface coordinates
                                 */
                                const float projectedXYZ[3] = {
                                    (dc1[0]*areaU + dc2[0]*areaV + dc3[0]*areaW) / totalArea,
                                    (dc1[1]*areaU + dc2[1]*areaV + dc3[1]*areaW) / totalArea,
                                    (dc1[2]*areaU + dc2[2]*areaV + dc3[2]*areaW) / totalArea
                                };
                                
                                const float barycentricAreas[3] = {
                                    areaU,
                                    areaV,
                                    areaW
                                };
                                
                                const int32_t barycentricNodes[3] = {
                                    n1,
                                    n2,
                                    n3
                                };
                            
                                this->setProjectionModeData(depth, 
                                                            projectedXYZ, 
                                                            surface->getStructure(), 
                                                            barycentricAreas, 
                                                            barycentricNodes, 
                                                            surface->getNumberOfNodes());
                            }
                        }
                    }
            }
            CaretLogFine("Selected Triangle: " + QString::number(triangleIndex));
        }
        
    }
}

/**
 * During projection mode, set the projected data.  If the 
 * projection data is already set, it will be overridden
 * if the new data is closer, in screen depth, to the user.
 *
 * @param screenDepth
 *    Screen depth of data.
 * @param xyz
 *    Stereotaxic coordinate of projected position.
 * @param structure
 *    Structure to which data projects.
 * @param barycentricAreas
 *    Barycentric areas of projection, if to surface with valid structure.
 * @param barycentricNodes
 *    Barycentric nodes of projection, if to surface with valid structure
 * @param numberOfNodes
 *    Number of nodes in surface, if to surface with valid structure.
 */
void 
BrainOpenGLFixedPipeline::setProjectionModeData(const float screenDepth,
                                  const float xyz[3],
                                  const StructureEnum::Enum structure,
                                  const float barycentricAreas[3],
                                  const int barycentricNodes[3],
                                  const int numberOfNodes)
{
    CaretAssert(this->modeProjectionData);
    if (screenDepth < this->modeProjectionScreenDepth) {
        this->modeProjectionScreenDepth = screenDepth;
        this->modeProjectionData->setStructure(structure);
        this->modeProjectionData->setStereotaxicXYZ(xyz);
        SurfaceProjectionBarycentric* barycentric =
        this->modeProjectionData->getBarycentricProjection();
        barycentric->setProjectionSurfaceNumberOfNodes(numberOfNodes);
        barycentric->setTriangleAreas(barycentricAreas);
        barycentric->setTriangleNodes(barycentricNodes);
        barycentric->setValid(true);  
        
        CaretLogFiner("Projected to surface " 
                      + StructureEnum::toName(structure)
                      + " with depth "
                      + screenDepth);
    }
}

/**
 * Draw a surface as individual nodes.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceNodes(Surface* surface,
                                           const float* nodeColoringRGBA)
{
    const DisplayPropertiesSurface* dps = m_brain->getDisplayPropertiesSurface();
    
    const int numNodes = surface->getNumberOfNodes();
    
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);
    
    SelectionItemSurfaceNode* nodeID = 
    m_brain->getSelectionManager()->getSurfaceNodeIdentification();
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (nodeID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            break;
    }
    
    uint8_t rgba[4];

    float pointSize = dps->getNodeSize();
    if (isSelect) {
        if (pointSize < 2.0) {
            pointSize = 2.0;
        }
    }
    setPointSize(pointSize);
    
    glBegin(GL_POINTS);
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i3 = i * 3;
        
        if (isSelect) {
            this->colorIdentification->addItem(rgba, SelectionItemDataTypeEnum::SURFACE_NODE, i);
            glColor3ubv(rgba);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
        else {
            glColor4fv(&nodeColoringRGBA[i*4]);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        int nodeIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_NODE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         nodeIndex,
                                         depth);
        if (nodeIndex >= 0) {
            if (nodeID->isOtherScreenDepthCloserToViewer(depth)) {
                nodeID->setBrain(surface->getBrainStructure()->getBrain());
                nodeID->setSurface(surface);
                nodeID->setNodeNumber(nodeIndex);
                nodeID->setScreenDepth(depth);
                this->setSelectedItemScreenXYZ(nodeID, &coordinates[nodeIndex * 3]);
                CaretLogFine("Selected Vertex: " + nodeID->toString());   
            }
            else {
                CaretLogFine("Rejecting Selected Vertex: " + nodeID->toString());
            }
        }
    }
}


/**
 * Draw a surface triangles with vertex arrays.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                               const float* nodeColoringRGBA)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if (nodeColoringRGBA != NULL) {
        glEnableClientState(GL_COLOR_ARRAY);
    }
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, 
                    GL_FLOAT, 
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getCoordinate(0)));
    if (nodeColoringRGBA != NULL) {
        glColorPointer(4,
                       GL_FLOAT,
                       0,
                       reinterpret_cast<const GLvoid*>(nodeColoringRGBA));
    }
    else {
        glColor3fv(m_backgroundColorFloat);
    }
    glNormalPointer(GL_FLOAT,
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getNormalVector(0)));
    
    const int numTriangles = surface->getNumberOfTriangles();
    glDrawElements(GL_TRIANGLES, 
                   (3 * numTriangles), 
                   GL_UNSIGNED_INT,
                   reinterpret_cast<const GLvoid*>(surface->getTriangle(0)));
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Draw a surface's normal vectors.
 * @param surface
 *     Surface on which normal vectors are drawn.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceNormalVectors(const Surface* surface)
{
    disableLighting();
    
    const StructureEnum::Enum structure = surface->getStructure();
    const float length = 10.0;    
    CaretPointer<TopologyHelper> topoHelper = surface->getTopologyHelper();
    setLineWidth(1.0);
    glColor3f(1.0, 0.0, 0.0);
    
    const int32_t numNodes = surface->getNumberOfNodes();
    glBegin(GL_LINES);
    for (int32_t i = 0; i < numNodes; i++) {
        if (topoHelper->getNodeHasNeighbors(i)) {
            const float* xyz = surface->getCoordinate(i);

            if (m_clippingPlaneGroup->isSurfaceSelected()) {
                if ( ! isCoordinateInsideClippingPlanesForStructure(structure, xyz)) {
                    continue;
                }
            }
            
            const float* normal = surface->getNormalVector(i);
            float vector[3] = {
                xyz[0] + length * normal[0],
                xyz[1] + length * normal[1],
                xyz[2] + length * normal[2]
            };
            
            glVertex3fv(xyz);
            glVertex3fv(vector);
        }
    }
    glEnd();
    
    enableLighting();
}

/**
 * Draw attributes for the given surface.
 * @param surface
 *    Surface for which attributes are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceNodeAttributes(Surface* surface)
{
    BrainStructure* brainStructure = surface->getBrainStructure();
    CaretAssert(brainStructure);
    Brain* brain = brainStructure->getBrain();
    CaretAssert(brain);
    const StructureEnum::Enum structure = surface->getStructure();
    
    const int numNodes = surface->getNumberOfNodes();
    
    const float* coordinates = surface->getCoordinate(0);

    IdentificationManager* idManager = brain->getIdentificationManager();
    
    SelectionItemSurfaceNodeIdentificationSymbol* symbolID =
        m_brain->getSelectionManager()->getSurfaceNodeIdentificationSymbol();
    
    const std::vector<IdentifiedItemNode> identifiedNodes = idManager->getNodeIdentifiedItemsForSurface(structure,
                                                                                                        numNodes);
    std::vector<int32_t> identifiedNodeIndices;
    for (std::vector<IdentifiedItemNode>::const_iterator iter = identifiedNodes.begin();
         iter != identifiedNodes.end();
         iter++) {
        const IdentifiedItemNode& nodeID = *iter;
        identifiedNodeIndices.push_back(nodeID.getNodeIndex());
    }
    
    EventNodeIdentificationColorsGetFromCharts colorsFromChartsEvent(structure,
                                                                     this->windowTabIndex,
                                                                     identifiedNodeIndices);
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            EventManager::get()->sendEvent(colorsFromChartsEvent.getPointer());
            break;
        case MODE_IDENTIFICATION:
            if (symbolID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    uint8_t idRGBA[4];
    
    for (std::vector<IdentifiedItemNode>::const_iterator iter = identifiedNodes.begin();
         iter != identifiedNodes.end();
         iter++) {
        const IdentifiedItemNode& nodeID = *iter;
        
        /*
         * Show symbol for node ID?
         */
        if ( ! nodeID.isShowIdentificationSymbol()) {
            continue;
        }
        
        const int32_t nodeIndex = nodeID.getNodeIndex();
        const int32_t i3 = nodeIndex * 3;
        const float* xyz = &coordinates[i3];
        
        if (m_clippingPlaneGroup->isSurfaceSelected()) {
            if ( ! isCoordinateInsideClippingPlanesForStructure(structure, xyz)) {
                continue;
            }
        }
        
        const float symbolDiameter = nodeID.getSymbolSize();
        
        if (isSelect) {
            this->colorIdentification->addItem(idRGBA,
                                               SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL,
                                               nodeIndex);
        }
        else {
            if (structure == nodeID.getStructure()) {
                nodeID.getSymbolRGBA(idRGBA);
                
                colorsFromChartsEvent.applyChartColorToNode(nodeIndex,
                                                            idRGBA);
            }
            else {
                nodeID.getContralateralSymbolRGB(idRGBA);
            }
        }
        idRGBA[3] = 255;
        
        
        glPushMatrix();
        glTranslatef(xyz[0], xyz[1], xyz[2]);
        this->drawSphereWithDiameter(idRGBA,
                                     symbolDiameter);
        glPopMatrix();
    }
    
    if (isSelect) {
        int nodeIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL, 
                                         this->mouseX, 
                                         this->mouseY,
                                         nodeIndex,
                                         depth);
        if (nodeIndex >= 0) {
            if (symbolID->isOtherScreenDepthCloserToViewer(depth)) {
                symbolID->setBrain(surface->getBrainStructure()->getBrain());
                symbolID->setSurface(surface);
                symbolID->setNodeNumber(nodeIndex);
                symbolID->setScreenDepth(depth);
                this->setSelectedItemScreenXYZ(symbolID, &coordinates[nodeIndex * 3]);
                CaretLogFine("Selected Vertex Identification Symbol: " + QString::number(nodeIndex));   
            }
        }
    }
}

/**
 * Draw a border on a surface.  The color must be set prior
 * to calling this method.
 *
 * @param borderDrawInfo
 *   Info about border being drawn.
 * @param border
 *   Border that is drawn on the surface.
 * @param borderFileIndex
 *   Index of border file.
 * @param borderIndex
 *   Index of border.
 * @param isSelect
 *   Selection mode is active.
 */
void 
BrainOpenGLFixedPipeline::drawBorder(const BorderDrawInfo& borderDrawInfo)
{
    CaretAssert(borderDrawInfo.surface);
    CaretAssert(borderDrawInfo.border);

    const StructureEnum::Enum surfaceStructure = borderDrawInfo.surface->getStructure();
    const StructureEnum::Enum contralateralSurfaceStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    const int32_t numBorderPoints = borderDrawInfo.border->getNumberOfPoints();
    const bool isHighlightEndPoints = borderDrawInfo.isHighlightEndPoints;
    
    float pointDiameter = 2.0;
    float lineWidth  = 2.0;
    BorderDrawingTypeEnum::Enum drawType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    if (borderDrawInfo.borderFileIndex >= 0) {
        const BrainStructure* bs = borderDrawInfo.surface->getBrainStructure();
        const Brain* brain = bs->getBrain();
        const DisplayPropertiesBorders* dpb = brain->getDisplayPropertiesBorders();
        const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(this->windowTabIndex);
        pointDiameter = dpb->getPointSize(displayGroup,
                                      this->windowTabIndex);
        lineWidth  = dpb->getLineWidth(displayGroup,
                                       this->windowTabIndex);
        drawType  = dpb->getDrawingType(displayGroup,
                                        this->windowTabIndex);
    }
    
    bool drawSphericalPoints = false;
    bool drawSquarePoints = false;
    bool drawLines  = false;
    switch (drawType) {
        case BorderDrawingTypeEnum::DRAW_AS_LINES:
            drawLines = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES:
            drawSphericalPoints = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_SQUARES:
            drawSquarePoints = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_AND_LINES:
            drawLines = true;
            drawSphericalPoints = true;
            break;
    }

    const bool flatSurfaceFlag = (borderDrawInfo.surface->getSurfaceType() == SurfaceTypeEnum::FLAT);
    bool flatSurfaceDrawUnstretchedLinesFlag = false;
    float unstretchedLinesLength = -1.0;
    if (flatSurfaceFlag) {
        if ((borderDrawInfo.anatomicalSurface != NULL)
            && (borderDrawInfo.unstretchedLinesLength > 0.0)) {
            flatSurfaceDrawUnstretchedLinesFlag = true;
            unstretchedLinesLength = borderDrawInfo.unstretchedLinesLength;
        }
    }
    
    const float drawAtDistanceAboveSurface = 0.0;

    std::vector<float> pointXYZ;
    std::vector<float> pointAnatomicalXYZ;
    std::vector<int32_t> pointIndex;
    
    const CaretPointer<TopologyHelper> th = borderDrawInfo.surface->getTopologyHelper();
    const std::vector<int32_t>& nodesBoundaryEdgeCount = th->getNumberOfBoundaryEdgesForAllNodes();
    CaretAssert(static_cast<int32_t>(nodesBoundaryEdgeCount.size()) == borderDrawInfo.surface->getNumberOfNodes());
    
    /*
     * Find points valid for this surface
     */
    for (int32_t i = 0; i < numBorderPoints; i++) {
        const SurfaceProjectedItem* p = borderDrawInfo.border->getPoint(i);
        
        /*
         * If surface structure does not match the point's structure,
         * check to see if contralateral display is enabled and 
         * compare contralateral surface structure to point's structure.
         */
        const StructureEnum::Enum pointStructure = p->getStructure();
        bool structureMatches = true;
        if (surfaceStructure != pointStructure) {
            structureMatches = false;
            if (borderDrawInfo.isContralateralEnabled) {
                if (contralateralSurfaceStructure == pointStructure) {
                    structureMatches = true;
                }
            }
        }
        if (structureMatches == false) {
            continue;
        }
        
        float xyz[3];
        bool isXyzValid = p->getProjectedPositionAboveSurface(*borderDrawInfo.surface, 
                                                                    xyz,
                                                                    drawAtDistanceAboveSurface);
        
        if (isXyzValid) {
            /*
             * On a flat surface, do not draw border points that are attached to all edge nodes
             * as they will likely result in points outside of the flat surface 
             * (near cuts and medial wall)
             */
            if (flatSurfaceDrawUnstretchedLinesFlag){
                if (p->getBarycentricProjection()->isValid()) {
                    const int32_t* baryNodes = p->getBarycentricProjection()->getTriangleNodes();
                    if (baryNodes != NULL) {
                        int32_t edgeNodeCount = 0;
                        if (nodesBoundaryEdgeCount[baryNodes[0]] > 0) edgeNodeCount++;
                        if (nodesBoundaryEdgeCount[baryNodes[1]] > 0) edgeNodeCount++;
                        if (nodesBoundaryEdgeCount[baryNodes[2]] > 0) edgeNodeCount++;
                        if (edgeNodeCount >= 3) {
                            isXyzValid = false;
                        }
                    }
                }
            }
        }
        
        if (isXyzValid) {
            if (flatSurfaceDrawUnstretchedLinesFlag) {
                float anatXYZ[3];
                const bool isAnatXyzValid = p->getProjectedPositionAboveSurface(*borderDrawInfo.anatomicalSurface,
                                                                                anatXYZ,
                                                                                drawAtDistanceAboveSurface);
                if (isAnatXyzValid) {
                    pointXYZ.push_back(xyz[0]);
                    pointXYZ.push_back(xyz[1]);
                    pointXYZ.push_back(xyz[2]);
                    pointAnatomicalXYZ.push_back(anatXYZ[0]);
                    pointAnatomicalXYZ.push_back(anatXYZ[1]);
                    pointAnatomicalXYZ.push_back(anatXYZ[2]);
                    pointIndex.push_back(i);
                    
                }
            }
            else {
                pointXYZ.push_back(xyz[0]);
                pointXYZ.push_back(xyz[1]);
                pointXYZ.push_back(xyz[2]);
                pointIndex.push_back(i);
            }
        }
    }    

    const bool doClipping = isFeatureClippingEnabled();
    
    const int32_t numPointsToDraw = static_cast<int32_t>(pointXYZ.size() / 3);
    if (flatSurfaceDrawUnstretchedLinesFlag) {
        CaretAssert(pointXYZ.size() == pointAnatomicalXYZ.size());
    }
    
    /*
     * Draw points
     */
    if (drawSphericalPoints
        || drawSquarePoints) {
        for (int32_t i = 0; i < numPointsToDraw; i++) {
            const int32_t i3 = i * 3;
            
            const float* xyz = &pointXYZ[i3];
            
            if (doClipping) {
                if ( ! isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                    xyz)) {
                    continue;
                }
            }
            
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            
            if (borderDrawInfo.isSelect) {
                uint8_t idRGBA[4];
                this->colorIdentification->addItem(idRGBA,
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderDrawInfo.borderFileIndex,
                                                   borderDrawInfo.borderIndex,
                                                   pointIndex[i]);
                idRGBA[3] = 255;
                if (drawSphericalPoints) {
                    this->drawSphereWithDiameter(idRGBA,
                                                 pointDiameter);
                }
                else {
                    this->drawSquare(idRGBA,
                                     pointDiameter);
                }
            }
            else {
                float rgba[4] = {
                    borderDrawInfo.rgba[0],
                    borderDrawInfo.rgba[1],
                    borderDrawInfo.rgba[2],
                    borderDrawInfo.rgba[3],
                };
                if (isHighlightEndPoints) {
                    if (i == 0) {
                        rgba[0] = 0.0;
                        rgba[1] = 1.0;
                        rgba[2] = 0.0;
                        rgba[3] = 1.0;
                    }
                    else if (i == (numPointsToDraw - 1)) {
                        rgba[0] = 0.0;
                        rgba[1] = 0.75;
                        rgba[2] = 0.0;
                        rgba[3] = 1.0;
                    }
                }
                if (drawSphericalPoints) {
                    this->drawSphereWithDiameter(rgba,
                                                 pointDiameter);
                }
                else {
                    this->drawSquare(rgba,
                                     pointDiameter);
                }
            }
            
            glPopMatrix();
        }
    }
    
    /*
     * Draw lines
     */
    if (drawLines
        && (numPointsToDraw > 1)) {
        this->setLineWidth(lineWidth);
        
        this->disableLighting();
        
        if (borderDrawInfo.isSelect) {
            /*
             * Start at one, since need two points for each line
             */
            for (int32_t i = 1; i < numPointsToDraw; i++) {
                /*
                 * On a flat surface, do not draw a line segment if it is
                 * from non-consecutive border points.  This occurs when
                 * a border point does not project to the flat surface 
                 * due to a cut or removal of the medial wall.  If helps
                 * prevent long border lines stretching from one edge of the
                 * surface to a far away edge.
                 */
                if (flatSurfaceDrawUnstretchedLinesFlag) {
                    if (pointIndex[i] != (pointIndex[i-1] + 1)) {
                        continue;
                    }
                }
                
                const int32_t i3 = i * 3;
                uint8_t idRGBA[4];
                this->colorIdentification->addItem(idRGBA,
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderDrawInfo.borderFileIndex,
                                                   borderDrawInfo.borderIndex,
                                                   pointIndex[i]);
                glColor3ubv(idRGBA);
                
                CaretAssertVectorIndex(pointXYZ, i3 + 2);
                const float* xyz1 = &pointXYZ[i3 - 3];
                const float* xyz2 = &pointXYZ[i3];
                
                bool drawIt = true;
                if (doClipping) {
                    if (isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                     xyz1)
                        && (isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                         xyz2)) ) {
                        /* nothing */
                    }
                    else {
                        drawIt = false;
                    }
                }
                
                if (drawIt) {
                    if (flatSurfaceDrawUnstretchedLinesFlag) {
                        CaretAssertVectorIndex(pointAnatomicalXYZ, i3 + 2);
                        if (unstretchedBorderLineTest(xyz1,
                                                      xyz2,
                                                      &pointAnatomicalXYZ[i3],
                                                      &pointAnatomicalXYZ[i3-3],
                                                      unstretchedLinesLength)) {
                            drawIt = false;
                        }
                    }
                }
                if (drawIt) {
                    glBegin(GL_LINES);
                    glVertex3fv(xyz1);
                    glVertex3fv(xyz2);
                    glEnd();
                }
            }
        }
        else {
            glColor3fv(borderDrawInfo.rgba);
            
            /*
             * Start at one, since need two points for each line
             */
            glBegin(GL_LINES);
            for (int32_t i = 1; i < numPointsToDraw; i++) {
                /*
                 * On a flat surface, do not draw a line segment if it is
                 * from non-consecutive border points.  This occurs when
                 * a border point does not project to the flat surface
                 * due to a cut or removal of the medial wall.  If helps
                 * prevent long border lines stretching from one edge of the
                 * surface to a far away edge.
                 */
                if (flatSurfaceDrawUnstretchedLinesFlag) {
                    if (pointIndex[i] != (pointIndex[i-1] + 1)) {
                        continue;
                    }
                }
                
                const int32_t i3 = i * 3;
                CaretAssertVectorIndex(pointXYZ, i3 + 2);
                const float* xyz1 = &pointXYZ[i3 - 3];
                const float* xyz2 = &pointXYZ[i3];
                
                bool drawIt = true;
                if (doClipping) {
                    if (isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                     xyz1)
                        && (isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                         xyz2)) ) {
                        /* nothing */
                    }
                    else {
                        drawIt = false;
                    }
                }
                
                
                if (drawIt) {
                    if (flatSurfaceDrawUnstretchedLinesFlag) {
                        CaretAssertVectorIndex(pointAnatomicalXYZ, i3 + 2);
                        if (unstretchedBorderLineTest(xyz1,
                                                      xyz2,
                                                      &pointAnatomicalXYZ[i3],
                                                      &pointAnatomicalXYZ[i3-3],
                                                      unstretchedLinesLength)) {
                            drawIt = false;
                        }
                    }
                }
                
                if (drawIt) {
                    glVertex3fv(xyz1);
                    glVertex3fv(xyz2);
                }
            }
            glEnd();
        }
        
        this->enableLighting();
    }
}

/**
 * Determine if the ratio if border length over anatomical border length
 * is greater than the unstretched lines factor.
 *
 * @param p1
 *    Position of border point in surface.
 * @param p1
 *    Position of next border point in surface.
 * @param anat1
 *    Position of border point in anatomical surface.
 * @param anat2
 *    Position of next border point in anatomical surface.
 * @param unstretchedLinesFactor
 *    The unstretched lines factor.
 * @return
 *    True if the border is too long and should NOT be drawn, else false.
 */
bool
BrainOpenGLFixedPipeline::unstretchedBorderLineTest(const float p1[3],
                                                    const float p2[3],
                                                    const float anat1[3],
                                                    const float anat2[3],
                                                    const float unstretchedLinesFactor) const
{
    const float dist = MathFunctions::distance3D(p1, p2);
    const float anatDist = MathFunctions::distance3D(anat1,
                                                     anat2);
    
    if (anatDist > 0.0) {
        const float ratio = dist / anatDist;
        if (ratio > unstretchedLinesFactor) {
            return true;
        }
    }
    
    return false;
}


/**
 * Set the OpenGL line width.  Value is clamped
 * to minimum and maximum values to prevent
 * OpenGL error caused by invalid line width.
 */
void 
BrainOpenGLFixedPipeline::setLineWidth(const float lineWidth)
{
    if (lineWidth > s_maxLineWidth) {
        glLineWidth(s_maxLineWidth);
    }
    else if (lineWidth < s_minLineWidth) {
        glLineWidth(s_minLineWidth);
    }
    else {
        glLineWidth(lineWidth);
    }
}

/**
 * Set the OpenGL point size.  Value is clamped
 * to minimum and maximum values to prevent
 * OpenGL error caused by invalid point size.
 */
void
BrainOpenGLFixedPipeline::setPointSize(const float pointSize)
{
    if (pointSize > s_maxPointSize) {
        glPointSize(s_maxPointSize);
    }
    else if (pointSize < s_minPointSize) {
        glPointSize(s_minPointSize);
    }
    else {
        glPointSize(pointSize);
    }
}


/**
 * Draw foci on a surface.
 * @param surface
 *   Surface on which foci are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceFoci(Surface* surface)
{
    SelectionItemFocusSurface* idFocus = m_brain->getSelectionManager()->getSurfaceFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    Brain* brain = surface->getBrainStructure()->getBrain();
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(this->windowTabIndex);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           this->windowTabIndex) == false) {
        return;
    }
    const float focusDiameter = fociDisplayProperties->getFociSize(displayGroup,
                                                                 this->windowTabIndex);
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                               this->windowTabIndex);
    
    const StructureEnum::Enum surfaceStructure = surface->getStructure();
    const StructureEnum::Enum surfaceContralateralStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    
    const bool doClipping = isFeatureClippingEnabled();
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  this->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    const CaretColorEnum::Enum caretColor = fociDisplayProperties->getStandardColorType(displayGroup,
                                                                                           this->windowTabIndex);
    float caretColorRGBA[4];
    CaretColorEnum::toRGBFloat(caretColor, caretColorRGBA);
    
    const bool isPasteOntoSurface = fociDisplayProperties->isPasteOntoSurface(displayGroup,
                                                                              this->windowTabIndex);
    
    const bool isContralateralEnabled = fociDisplayProperties->isContralateralDisplayed(displayGroup,
                                                                                        this->windowTabIndex);
    const int32_t numFociFiles = brain->getNumberOfFociFiles();
    for (int32_t i = 0; i < numFociFiles; i++) {
        FociFile* fociFile = brain->getFociFile(i);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              this->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            
            const GroupAndNameHierarchyItem* nameItem = focus->getGroupNameSelectionItem();
            if (nameItem != NULL) {
                if (nameItem->isSelected(displayGroup,
                                         this->windowTabIndex) == false) {
                    continue;
                }
            }
            
            switch (fociColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (focus->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(focus->getClassName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setClassRgba(rgba);
                        }
                        else {
                            focus->setClassRgba(rgba);
                        }
                    }
                    focus->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_STANDARD_COLOR:
                    rgba[0] = caretColorRGBA[0];
                    rgba[1] = caretColorRGBA[1];
                    rgba[2] = caretColorRGBA[2];
                    rgba[3] = caretColorRGBA[3];
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (focus->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(focus->getName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setNameRgba(rgba);
                        }
                        else {
                            focus->setNameRgba(rgba);
                        }
                    }
                    focus->getNameRgba(rgba);
                    break;
            }
            
            glColor3fv(rgba);
            
            
            const int32_t numProjections = focus->getNumberOfProjections();
            for (int32_t k = 0; k < numProjections; k++) {
                const SurfaceProjectedItem* spi = focus->getProjection(k);
                float xyz[3];
                if (spi->getProjectedPosition(*surface,
                                              xyz,
                                              isPasteOntoSurface)) {
                    const StructureEnum::Enum focusStructure = spi->getStructure();
                    bool drawIt = false;
                    if (focusStructure == surfaceStructure) {
                        drawIt = true;
                    }
                    else if (focusStructure == StructureEnum::INVALID) {
                        drawIt = true;
                    }
                    else if (isContralateralEnabled) {
                        if (focusStructure == surfaceContralateralStructure) {
                            drawIt = true;
                        }
                    }

                    if (doClipping) {
                        if ( ! isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                            xyz)) {
                            drawIt = false;
                        }
                    }
                    
                    if (drawIt) {
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        
                        if (isSelect) {
                            uint8_t idRGBA[4];
                            this->colorIdentification->addItem(idRGBA,
                                                               SelectionItemDataTypeEnum::FOCUS_SURFACE, 
                                                               i, // file index
                                                               j, // focus index
                                                               k);// projection index
                            idRGBA[3] = 255;
                            if (drawAsSpheres) {
                                this->drawSphereWithDiameter(idRGBA,
                                                             focusDiameter);
                            }
                            else {
                                this->drawSquare(idRGBA,
                                                 focusDiameter);
                            }
                        }
                        else {
                            if (drawAsSpheres) {
                                this->drawSphereWithDiameter(rgba,
                                                             focusDiameter);
                            }
                            else {
                                this->drawSquare(rgba,
                                                 focusDiameter);
                            }
                        }
                        
                        glPopMatrix();
                    }
                }                
            }
        }
    }
    
    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_SURFACE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         fociFileIndex,
                                         focusIndex,
                                         focusProjectionIndex,
                                         depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setSurface(surface);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getProjectedPosition(*surface,
                                            xyz,
                                            false);
                this->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Focus Identification Symbol: " + QString::number(focusIndex));   
            }
        }
    }
}


/**
 * Draw borders on a surface.
 * @param surface
 *   Surface on which borders are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceBorders(Surface* surface)
{
    SelectionItemBorderSurface* idBorder = m_brain->getSelectionManager()->getSurfaceBorderIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (idBorder->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    

    Brain* brain = surface->getBrainStructure()->getBrain();
    const DisplayPropertiesBorders* borderDisplayProperties = brain->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum displayGroup = borderDisplayProperties->getDisplayGroupForTab(this->windowTabIndex);
    if (borderDisplayProperties->isDisplayed(displayGroup,
                                             this->windowTabIndex) == false) {
        return;
    }
    
    float unstretchedLinesLength = -1.0;
    if (borderDisplayProperties->isUnstretchedLinesEnabled(displayGroup,
                                                           this->windowTabIndex)) {
        unstretchedLinesLength = borderDisplayProperties->getUnstretchedLinesLength(displayGroup,
                                                                                    this->windowTabIndex);
    }
    
    const FeatureColoringTypeEnum::Enum borderColoringType = borderDisplayProperties->getColoringType(displayGroup,
                                                                                                      this->windowTabIndex);
    const CaretColorEnum::Enum caretColor = borderDisplayProperties->getStandardColorType(displayGroup,
                                                                                       this->windowTabIndex);
    float caretColorRGBA[4];
    CaretColorEnum::toRGBFloat(caretColor, caretColorRGBA);
    const bool isContralateralEnabled = borderDisplayProperties->isContralateralDisplayed(displayGroup,
                                                                                          this->windowTabIndex);
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t i = 0; i < numBorderFiles; i++) {
        BorderFile* borderFile = brain->getBorderFile(i);

        const GroupAndNameHierarchyModel* classAndNameSelection = borderFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              this->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = borderFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable  = borderFile->getNameColorTable();
        const int32_t numBorders = borderFile->getNumberOfBorders();
        
        for (int32_t j = 0; j < numBorders; j++) {
            Border* border = borderFile->getBorder(j);
            if (borderFile->isBorderDisplayed(displayGroup,
                                              this->windowTabIndex,
                                              border) == false) {
                continue;
            }
            
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (borderColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (border->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(border->getClassName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            border->setClassRgba(rgba);
                        }
                        else {
                            border->setClassRgba(rgba);
                        }
                    }
                    border->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_STANDARD_COLOR:
                    rgba[0] = caretColorRGBA[0];
                    rgba[1] = caretColorRGBA[1];
                    rgba[2] = caretColorRGBA[2];
                    rgba[3] = caretColorRGBA[3];
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (border->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(border->getName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            border->setNameRgba(rgba);
                        }
                        else {
                            border->setNameRgba(rgba);
                        }
                    }
                    border->getNameRgba(rgba);
                    break;
            }
            glColor3fv(rgba);
            
            BorderDrawInfo borderDrawInfo;
            borderDrawInfo.surface = surface;
            borderDrawInfo.border = border;
            borderDrawInfo.rgba[0] = rgba[0];
            borderDrawInfo.rgba[1] = rgba[1];
            borderDrawInfo.rgba[2] = rgba[2];
            borderDrawInfo.rgba[3] = rgba[3];
            borderDrawInfo.borderFileIndex = i;
            borderDrawInfo.borderIndex = j;
            borderDrawInfo.isSelect = isSelect;
            borderDrawInfo.isContralateralEnabled = isContralateralEnabled;
            borderDrawInfo.isHighlightEndPoints = m_drawHighlightedEndPoints;
            
            borderDrawInfo.anatomicalSurface = NULL;
            borderDrawInfo.unstretchedLinesLength = unstretchedLinesLength;
            
            BrainStructure* bs = brain->getBrainStructure(border->getStructure(),
                                                          false);
            if (bs != NULL) {
                borderDrawInfo.anatomicalSurface = bs->getPrimaryAnatomicalSurface();
            }
            
            this->drawBorder(borderDrawInfo);
        }
    }
    
    if (isSelect) {
        int32_t borderFileIndex = -1;
        int32_t borderIndex = -1;
        int32_t borderPointIndex;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         borderFileIndex,
                                         borderIndex,
                                         borderPointIndex,
                                         depth);
        if (borderFileIndex >= 0) {
            if (idBorder->isOtherScreenDepthCloserToViewer(depth)) {
                Border* border = brain->getBorderFile(borderFileIndex)->getBorder(borderIndex);
                idBorder->setBrain(brain);
                idBorder->setBorder(border);
                idBorder->setBorderFile(brain->getBorderFile(borderFileIndex));
                idBorder->setBorderIndex(borderIndex);
                idBorder->setBorderPointIndex(borderPointIndex);
                idBorder->setSurface(surface);
                idBorder->setScreenDepth(depth);
                float xyz[3];
                border->getPoint(borderPointIndex)->getProjectedPosition(*surface,
                                                                         xyz,
                                                                         false);
                this->setSelectedItemScreenXYZ(idBorder, xyz);
                CaretLogFine("Selected Border Identification Symbol: " + QString::number(borderIndex));   
            }
        }
    }
}

/**
 * Draw the border that is begin drawn.
 * @param surface
 *    Surface on which border is being drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceBorderBeingDrawn(const Surface* surface)
{
    glColor3f(1.0, 0.0, 0.0);
    
    if (this->borderBeingDrawn != NULL) {
        BorderDrawInfo borderDrawInfo;
        borderDrawInfo.surface = const_cast<Surface*>(surface);
        borderDrawInfo.border = this->borderBeingDrawn;
        borderDrawInfo.rgba[0] = 1.0;
        borderDrawInfo.rgba[1] = 0.0;
        borderDrawInfo.rgba[2] = 0.0;
        borderDrawInfo.rgba[3] = 1.0;
        borderDrawInfo.borderFileIndex = -1;
        borderDrawInfo.borderIndex = -1;
        borderDrawInfo.isSelect = false;
        borderDrawInfo.isContralateralEnabled = false;
        borderDrawInfo.isHighlightEndPoints = false;
        borderDrawInfo.anatomicalSurface = NULL;
        borderDrawInfo.unstretchedLinesLength = -1.0;
        
        this->drawBorder(borderDrawInfo);
    }
}


/**
 * Setup volume drawing information for an overlay set.
 *
 * @param browserTabContent
 *    Content in the browser tab.
 * @param paletteFile
 *    File from which palette is obtained.
 * @param volumeDrawInfoOut
 *    Output containing information for volume drawing.
 */
void 
BrainOpenGLFixedPipeline::setupVolumeDrawInfo(BrowserTabContent* browserTabContent,
                                 Brain* brain,
                                 std::vector<VolumeDrawInfo>& volumeDrawInfoOut)
{
    volumeDrawInfoOut.clear();
    
    PaletteFile* paletteFile = brain->getPaletteFile();
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t iOver = (numberOfOverlays - 1); iOver >= 0; iOver--) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile,
                                      mapIndex);
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        float opacity = overlay->getOpacity();
                        if (volumeDrawInfoOut.empty()) {
                            opacity = 1.0;
                        }
                        
                        WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode = overlay->getWholeBrainVoxelDrawingMode();
                        
                        if (mapFile->isMappedWithPalette()) {
                            FastStatistics* statistics = NULL;
                            switch (mapFile->getPaletteNormalizationMode()) {
                                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                                    statistics = const_cast<FastStatistics*>(mapFile->getFileFastStatistics());
                                    break;
                                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                                    statistics = const_cast<FastStatistics*>(mapFile->getMapFastStatistics(mapIndex));
                                    break;
                            }
                            //CaretAssert(statistics);
                            
                            PaletteColorMapping* paletteColorMapping = mapFile->getMapPaletteColorMapping(mapIndex);
                            Palette* palette = paletteFile->getPaletteByName(paletteColorMapping->getSelectedPaletteName());
                            if (palette != NULL) {
                                /*
                                 * Statistics may be NULL for a dense connectome file
                                 * that does not have any data loaded by user
                                 * clicking on surface/volume.
                                 */
                                if (statistics != NULL) {
                                    bool useIt = true;
                                    
                                    if (volumeDrawInfoOut.empty() == false) {
                                        /*
                                         * If previous volume is the same as this
                                         * volume, there is no need to draw it twice.
                                         */
                                        const VolumeDrawInfo& vdi = volumeDrawInfoOut[volumeDrawInfoOut.size() - 1];
                                        if ((vdi.volumeFile == vf)
                                            && (opacity >= 1.0)
                                            && (mapIndex == vdi.mapIndex)
                                            && (*paletteColorMapping == *vdi.paletteColorMapping)) {
                                            useIt = false;
                                        }
                                    }
                                    if (useIt) {
                                        VolumeDrawInfo vdi(mapFile,
                                                           vf,
                                                           brain,
                                                           paletteColorMapping,
                                                           statistics,
                                                           wholeBrainVoxelDrawingMode,
                                                           mapIndex,
                                                           opacity);
                                        volumeDrawInfoOut.push_back(vdi);
                                    }
                                }
                            }
                            else {
                                CaretLogWarning("No valid palette for drawing volume file: "
                                                + mapFile->getFileNameNoPath());
                            }
                        }
                        else {
                            VolumeDrawInfo vdi(mapFile,
                                               vf,
                                               brain,
                                               NULL,
                                               NULL,
                                               wholeBrainVoxelDrawingMode,
                                               mapIndex,
                                               opacity);
                            volumeDrawInfoOut.push_back(vdi);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Draw the volume slices.
 * @param browserTabContent
 *    Content of the window.
 * @param volumeModel
 *    Model for slices.
 * @param viewport
 *    Region of drawing.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeModel(BrowserTabContent* browserTabContent,
                                  ModelVolume* volumeModel,
                                  const int32_t viewport[4])
{
    /*
     * Determine volumes that are to be drawn
     */
    const int32_t tabNumber = browserTabContent->getTabNumber();
    volumeModel->updateModel(tabNumber);
    Brain* brain = volumeModel->getBrain();
    std::vector<VolumeDrawInfo> volumeDrawInfo;
    this->setupVolumeDrawInfo(browserTabContent,
                              brain,
                              volumeDrawInfo);
    
    VolumeSliceDrawingTypeEnum::Enum sliceDrawingType = browserTabContent->getSliceDrawingType();
    VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = browserTabContent->getSliceProjectionType();

    
    /*
     * There is/was a flaw in volume drawing in that it does not "center"
     * correctly when the voxel corresponding to the coordinate (0, 0, 0)
     * is not within the volume.  It seems to be fixed for orthogonal
     * drawing but oblique drawing probably needs a new algorithm to 
     * fix the problem.
     */
    bool useNewDrawingFlag = false;
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            useNewDrawingFlag = true;
            break;
    }
    
    if (useNewDrawingFlag) {
        BrainOpenGLVolumeSliceDrawing volumeSliceDrawing;
        volumeSliceDrawing.draw(this,
                                browserTabContent,
                                volumeDrawInfo,
                                sliceDrawingType,
                                sliceProjectionType,
                                viewport);
    }
    else {
        BrainOpenGLVolumeObliqueSliceDrawing obliqueVolumeSliceDrawing;
        obliqueVolumeSliceDrawing.draw(this,
                                   browserTabContent,
                                   volumeDrawInfo,
                                   sliceDrawingType,
                                   sliceProjectionType,
                                   viewport);
    }
}


/**
 * Draw volumes a voxel cubes for whole brain view.
 *
 * @param volumeDrawInfoIn
 *    Describes volumes that are drawn.
 */
void
BrainOpenGLFixedPipeline::drawVolumeVoxelsAsCubesWholeBrain(std::vector<VolumeDrawInfo>& volumeDrawInfoIn)
{
    /*
     * Filter volumes for drawing and only draw those volumes that
     * are to be drawn as 3D Voxel Cubes.
     */
    std::vector<VolumeDrawInfo> volumeDrawInfo;
    for (std::vector<VolumeDrawInfo>::iterator iter = volumeDrawInfoIn.begin();
         iter != volumeDrawInfoIn.end();
         iter++) {
        bool useIt = false;
        VolumeDrawInfo& vdi = *iter;
        switch (vdi.wholeBrainVoxelDrawingMode) {
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                useIt = true;
                break;
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                break;
        }
        if (useIt) {
            volumeDrawInfo.push_back(vdi);
        }
    }
    
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
    if (numberOfVolumesToDraw <= 0) {
        return;
    }
    
    SelectionItemVoxel* voxelID =
    m_brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    /*
     * When selecting turn on lighting and shading since
     * colors are used for identification.
     */
    if (isSelect) {
        this->disableLighting();
        glShadeModel(GL_FLAT);
    }
    else {
        this->enableLighting();
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
    }
    
    glEnable(GL_CULL_FACE);
    
    const bool doClipping = isFeatureClippingEnabled();
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(this->windowTabIndex);
    
    /*
     * For identification, five items per voxel
     * 1) volume index
     * 2) map index
     * 3) index I
     * 4) index J
     * 5) index K
     */
    const int32_t idPerVoxelCount = 5;
    std::vector<int32_t> identificationIndices;
    if (isSelect) {
        identificationIndices.reserve(10000 * idPerVoxelCount);
    }

    PaletteFile* paletteFile = m_brain->getPaletteFile();
    
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
        if (volInfo.opacity < 1.0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else {
            glDisable(GL_BLEND);
        }
        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        
        float originX, originY, originZ;
        float x1, y1, z1;
        float lastX, lastY, lastZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        volumeFile->indexToSpace(dimI - 1, dimJ - 1, dimK - 1, lastX, lastY, lastZ);
        const float dx = x1 - originX;
        const float dy = y1 - originY;
        const float dz = z1 - originZ;
        
        /*
         * Cube size for voxel drawing.  Some volumes may have a right to left
         * orientation in which case dx may be negative.
         */
        const float cubeSizeDX = std::fabs(dx);
        const float cubeSizeDY = std::fabs(dy);
        const float cubeSizeDZ = std::fabs(dz);
        
        std::vector<float> labelMapData;
        const CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<const CiftiBrainordinateLabelFile*>(volumeFile);
        if (ciftiLabelFile != NULL) {
            ciftiLabelFile->getMapData(volInfo.mapIndex,
                                       labelMapData);
        }
        
        uint8_t rgba[4];
        for (int64_t iVoxel = 0; iVoxel < dimI; iVoxel++) {
            for (int64_t jVoxel = 0; jVoxel < dimJ; jVoxel++) {
                for (int64_t kVoxel = 0; kVoxel < dimK; kVoxel++) {
                    if (ciftiLabelFile != NULL) {
                        ciftiLabelFile->getVoxelColorInMapForLabelData(paletteFile,
                                                                       labelMapData,
                                                                       iVoxel,
                                                                       jVoxel,
                                                                       kVoxel,
                                                                       volInfo.mapIndex,
                                                                       displayGroup,
                                                                       this->windowTabIndex,
                                                                       rgba);
                    }
                    else {
                        volumeFile->getVoxelColorInMap(paletteFile,
                                                       iVoxel,
                                                       jVoxel,
                                                       kVoxel,
                                                       volInfo.mapIndex,
                                                       displayGroup,
                                                       this->windowTabIndex,
                                                       rgba);
                    }
                    if (rgba[3] > 0) {
                        if (volInfo.opacity < 1.0) {
                            rgba[3] *= volInfo.opacity;
                        }
                        if (rgba[3] > 0) {
                            if (isSelect) {
                                const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                                this->colorIdentification->addItem(rgba,
                                                                   SelectionItemDataTypeEnum::VOXEL,
                                                                   idIndex);
                                identificationIndices.push_back(iVol);
                                identificationIndices.push_back(volInfo.mapIndex);
                                identificationIndices.push_back(iVoxel);
                                identificationIndices.push_back(jVoxel);
                                identificationIndices.push_back(kVoxel);
                            }
                            
                            const float x = iVoxel * dx + originX;
                            const float y = jVoxel * dy + originY;
                            const float z = kVoxel * dz + originZ;
                            
                            bool drawIt = true;
                            if (doClipping) {
                                const float xyz[3] = { x, y, z };
                                if ( ! isCoordinateInsideClippingPlanesForStructure(StructureEnum::ALL,
                                                                                    xyz)) {
                                    drawIt = false;
                                }
                            }
                            
                            if (drawIt) {
                                glPushMatrix();
                                glTranslatef(x, y, z);
                                switch (volInfo.wholeBrainVoxelDrawingMode) {
                                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
                                        drawCuboid(rgba, cubeSizeDX, cubeSizeDY, cubeSizeDZ);
                                        break;
                                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                                        drawRoundedCuboid(rgba, cubeSizeDX, cubeSizeDY, cubeSizeDZ);
                                        break;
                                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                                        break;
                                }
                                glPopMatrix();
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                         this->mouseX,
                                         this->mouseY,
                                         identifiedItemIndex,
                                         depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * idPerVoxelCount;
            const int32_t volDrawInfoIndex = identificationIndices[idIndex];
            CaretAssertVectorIndex(volumeDrawInfo, volDrawInfoIndex);
            VolumeMappableInterface* vf = volumeDrawInfo[volDrawInfoIndex].volumeFile;
            const int64_t voxelIndices[3] = {
                identificationIndices[idIndex + 2],
                identificationIndices[idIndex + 3],
                identificationIndices[idIndex + 4]
            };
            
            if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                voxelID->setVoxelIdentification(m_brain,
                                                vf,
                                                voxelIndices,
                                                depth);
                
                float voxelCoordinates[3];
                vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                                 voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
                
                this->setSelectedItemScreenXYZ(voxelID,
                                               voxelCoordinates);
                CaretLogFine("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
            }
        }
    }
    
    this->disableLighting();
    glShadeModel(GL_SMOOTH);
    glDisable(GL_BLEND);
}

void
BrainOpenGLFixedPipeline::setFiberOrientationDisplayInfo(const DisplayPropertiesFiberOrientation* dpfo,
                                                         const DisplayGroupEnum::Enum displayGroup,
                                                         const int32_t tabIndex,
                                                         Plane* plane,
                                                         const StructureEnum::Enum structure,
                                                         FiberTrajectoryColorModel::Item* colorSource,
                                    FiberOrientationDisplayInfo& dispInfo)
{
    dispInfo.aboveLimit = dpfo->getAboveLimit(displayGroup, tabIndex);
    dispInfo.belowLimit = dpfo->getBelowLimit(displayGroup, tabIndex);
    dispInfo.colorSource = colorSource;
    dispInfo.fiberOrientationColorType = dpfo->getColoringType(displayGroup, tabIndex);
    dispInfo.fanMultiplier = dpfo->getFanMultiplier(displayGroup, tabIndex);
    dispInfo.isDrawWithMagnitude = dpfo->isDrawWithMagnitude(displayGroup, tabIndex);
    dispInfo.minimumMagnitude = dpfo->getMinimumMagnitude(displayGroup, tabIndex);
    dispInfo.magnitudeMultiplier = dpfo->getLengthMultiplier(displayGroup, tabIndex);
    dispInfo.plane = plane;
    dispInfo.structure = structure;
    dispInfo.symbolType = dpfo->getSymbolType(displayGroup, tabIndex);
}

/**
 * Draw fibers for a surface or a volume.
 *
 * @param plane
 *    If not NULL, it is the plane of the volume slice being drawn and
 *    only fibers within the above and below limits from the plane will
 *    be drawn.
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawFiberOrientations(const Plane* plane,
                                                const StructureEnum::Enum structure)
{
    const DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->windowTabIndex);
    if (dpfo->isDisplayed(displayGroup, this->windowTabIndex) == false) {
        return;
    }
    const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup, this->windowTabIndex);
    
    
    /*
     * Save status of clipping and disable clipping.
     * For fibers, the entire fiber symbol is displayed if its
     * origin is within the clipping planes which is tested below.
     */
    GLboolean clipPlanesEnabled[6] = {
        glIsEnabled(GL_CLIP_PLANE0),
        glIsEnabled(GL_CLIP_PLANE1),
        glIsEnabled(GL_CLIP_PLANE2),
        glIsEnabled(GL_CLIP_PLANE3),
        glIsEnabled(GL_CLIP_PLANE4),
        glIsEnabled(GL_CLIP_PLANE5)
    };
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    
    /*
     * Fans use lighting but NOT on a volume slice
     */
    disableLighting();
    switch (symbolType) {
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
            if (plane == NULL) {
                enableLighting();
            }
            break;
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
            break;
    }

    
    /*
     * Default constructor is color by fiber orientation settings XYZ/123 as RGB
     */
    FiberTrajectoryColorModel::Item colorUseFiber;
    
    FiberOrientationDisplayInfo fiberOrientDispInfo;
    setFiberOrientationDisplayInfo(dpfo,
                                   displayGroup,
                                   this->windowTabIndex,
                                   const_cast<Plane*>(plane),
                                   structure,
                                   &colorUseFiber,
                                   fiberOrientDispInfo);
    /*
     * Draw the vectors from each of the connectivity files
     */
    const int32_t numFiberOrienationFiles = m_brain->getNumberOfConnectivityFiberOrientationFiles();
    for (int32_t iFile = 0; iFile < numFiberOrienationFiles; iFile++) {
        CiftiFiberOrientationFile* cfof = m_brain->getConnectivityFiberOrientationFile(iFile);
        if (cfof->isDisplayed(displayGroup,
                              this->windowTabIndex)) {
            /*
             * Draw each of the fiber orientations which may contain multiple fibers
             */
            const int64_t numberOfFiberOrientations = cfof->getNumberOfFiberOrientations();
            for (int64_t i = 0; i < numberOfFiberOrientations; i++) {
                const FiberOrientation* fiberOrientation = cfof->getFiberOrientations(i);
                if (fiberOrientation->m_valid == false) {
                    continue;
                }
                
                for (int32_t ifi = 0; ifi < fiberOrientation->m_numberOfFibers; ifi++) {
                    fiberOrientation->m_fibers[ifi]->m_opacityForDrawing = 1.0;
                }
                
                addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                              fiberOrientation);
            }
        }
    }
    
    drawAllFiberOrientations(&fiberOrientDispInfo,
                             false);
    
    /*
     * Restore status of clipping planes enabled
     */
    if (clipPlanesEnabled[0]) glEnable(GL_CLIP_PLANE0);
    if (clipPlanesEnabled[1]) glEnable(GL_CLIP_PLANE1);
    if (clipPlanesEnabled[2]) glEnable(GL_CLIP_PLANE2);
    if (clipPlanesEnabled[3]) glEnable(GL_CLIP_PLANE3);
    if (clipPlanesEnabled[4]) glEnable(GL_CLIP_PLANE4);
    if (clipPlanesEnabled[5]) glEnable(GL_CLIP_PLANE5);
}

/**
 * Add fiber orientation for drawing.  Note that for alpha blending to
 * work correctly, the fibers must be sorted by depth and drawn from 
 * furthest to nearest.  Some tests will be performed to determine if
 * the fiber should be drawn prior to adding the fiber to the list
 * of fibers that will be drawn.
 *
 * @param fodi
 *    Parameters controlling the drawing of fiber orientations.
 * @param fiberOrientation
 *    The fiber orientation that will be drawn.
 */
void
BrainOpenGLFixedPipeline::addFiberOrientationForDrawing(const FiberOrientationDisplayInfo* fodi,
                                                        const FiberOrientation* fiberOrientation)
{
    /*
     * Test location of fiber orientation for drawing
     */
    if (fodi->plane != NULL) {
        const float distToPlane = fodi->plane->signedDistanceToPlane(fiberOrientation->m_xyz);
        if (distToPlane > fodi->aboveLimit) {
            return;
        }
        if (distToPlane < fodi->belowLimit) {
            return;
        }
    }
    if (isFeatureClippingEnabled()) {
        if ( ! isCoordinateInsideClippingPlanesForStructure(fodi->structure,
                                                            fiberOrientation->m_xyz)) {
            return;
        }
    }
    
    m_fiberOrientationsForDrawing.push_back(const_cast<FiberOrientation*>(fiberOrientation));
}

/*
 * For comparison when sorting that results in furthest fibers drawn first.
 */
static bool
fiberDepthCompare(FiberOrientation* &f1,
                              FiberOrientation* &f2)
{
    return (f1->m_drawingDepth > f2->m_drawingDepth);
}

/**
 * Sort the fiber orientations by depth.
 */
void
BrainOpenGLFixedPipeline::sortFiberOrientationsByDepth()
{
    ElapsedTimer timer;
    timer.start();
    
    /*
     * Create transforms model coordinate to a screen coordinate.
     */
    GLdouble modelMatrixOpenGL[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrixOpenGL);
    
    GLdouble projectionMatrixOpenGL[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrixOpenGL);
    
    Matrix4x4 modelMatrix;
    modelMatrix.setMatrixFromOpenGL(modelMatrixOpenGL);
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.setMatrixFromOpenGL(projectionMatrixOpenGL);
    
    Matrix4x4 modelToScreenMatrix;
    modelToScreenMatrix.setMatrix(projectionMatrix);
    modelToScreenMatrix.premultiply(modelMatrix);
    
    const float m0 = modelToScreenMatrix.getMatrixElement(2, 0);
    const float m1 = modelToScreenMatrix.getMatrixElement(2, 1);
    const float m2 = modelToScreenMatrix.getMatrixElement(2, 2);
    const float m3 = modelToScreenMatrix.getMatrixElement(2, 3);
    
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;
        
        const float rawDepth =(m0 * fiberOrientation->m_xyz[0]
                            + m1 * fiberOrientation->m_xyz[1]
                            + m2 * fiberOrientation->m_xyz[2]
                            + m3);
        const float screenDepth = ((rawDepth + 1.0) / 2.0);

        fiberOrientation->m_drawingDepth = screenDepth;
        
    }
    
    m_fiberOrientationsForDrawing.sort(fiberDepthCompare);
}

/**
 * Draw all of the fiber orienations.
 *
 * @param fodi
 *    Parameters controlling the drawing of fiber orientations. 
 */
void
BrainOpenGLFixedPipeline::drawAllFiberOrientations(const FiberOrientationDisplayInfo* fodi,
                                                   const bool isSortFibers)
{
    if (isSortFibers) {
        sortFiberOrientationsByDepth();
    }
    
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;

        /*
         * Draw each of the fibers
         */
        const int64_t numberOfFibers = fiberOrientation->m_numberOfFibers;
        for (int64_t j = 0; j < numberOfFibers; j++) {
            const Fiber* fiber = fiberOrientation->m_fibers[j];
            
            /*
             * Apply display properties
             */
            bool drawIt = true;
            if (fiber->m_meanF < fodi->minimumMagnitude) {
                drawIt = false;
            }
            
            if (drawIt) {
                float alpha = 1.0;
                if (j < 3) {
                    alpha = fiber->m_opacityForDrawing;
                    CaretAssertMessage(((alpha >= 0.0) && (alpha <= 1.0)),
                                       ("Value=" + AString::number(alpha)));
                    if (alpha <= 0.0) {
                        continue;
                    }
                }
                
                /*
                 * Length of vector
                 */
                float vectorLength = fodi->magnitudeMultiplier;
                if (fodi->isDrawWithMagnitude) {
                    vectorLength *= fiber->m_meanF;
                }
                
                /*
                 * Vector with magnitude
                 */
                const float magnitudeVector[3] = {
                    fiber->m_directionUnitVector[0] * vectorLength,
                    fiber->m_directionUnitVector[1] * vectorLength,
                    fiber->m_directionUnitVector[2] * vectorLength
                };
                
                const float halfMagnitudeVector[3] = {
                    magnitudeVector[0] * 0.5,
                    magnitudeVector[1] * 0.5,
                    magnitudeVector[2] * 0.5,
                };
                
                /*
                 * Start of vector
                 */
                float startXYZ[3] = {
                    fiberOrientation->m_xyz[0],
                    fiberOrientation->m_xyz[1],
                    fiberOrientation->m_xyz[2]
                };
                
                /*
                 * When drawing lines, start of vector is offset by
                 * have of the vector length since the vector is
                 * bi-directional.
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                        startXYZ[0] -= halfMagnitudeVector[0];
                        startXYZ[1] -= halfMagnitudeVector[1];
                        startXYZ[2] -= halfMagnitudeVector[2];
                        break;
                }
                
                
                /*
                 * End of vector
                 */
                float endXYZ[3] = { 0.0, 0.0, 0.0 };
                
                /*
                 * When drawing lines, end point is the start
                 * plus the vector with magnitude.
                 *
                 * When drawing fans, there are two endpoints
                 * with the fans starting in the middle.
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                        endXYZ[0] = startXYZ[0] + halfMagnitudeVector[0];
                        endXYZ[1] = startXYZ[1] + halfMagnitudeVector[1];
                        endXYZ[2] = startXYZ[2] + halfMagnitudeVector[2];
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                        endXYZ[0] = startXYZ[0] + magnitudeVector[0];
                        endXYZ[1] = startXYZ[1] + magnitudeVector[1];
                        endXYZ[2] = startXYZ[2] + magnitudeVector[2];
                        break;
                }
                
                float fiberRGBA[4] = { 0.0, 0.0, 0.0, 0.0 };
                
                /*
                 * Color of fiber
                 */
                switch (fodi->colorSource->getItemType()) {
                    case FiberTrajectoryColorModel::Item::ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
                        switch (fodi->fiberOrientationColorType) {
                            case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                            {
                                const int32_t indx = j % 3;
                                switch (indx) {
                                    case 0: // use RED
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_RED[0],
                                                  BrainOpenGLFixedPipeline::COLOR_RED[1],
                                                  BrainOpenGLFixedPipeline::COLOR_RED[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_RED[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_RED[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_RED[2];
                                        fiberRGBA[3] = alpha;
                                        break;
                                    case 1: // use BLUE
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_BLUE[0],
                                                  BrainOpenGLFixedPipeline::COLOR_BLUE[1],
                                                  BrainOpenGLFixedPipeline::COLOR_BLUE[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_BLUE[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_BLUE[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_BLUE[2];
                                        fiberRGBA[3] = alpha;
                                        break;
                                    case 2: // use GREEN
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_GREEN[0],
                                                  BrainOpenGLFixedPipeline::COLOR_GREEN[1],
                                                  BrainOpenGLFixedPipeline::COLOR_GREEN[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_GREEN[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_GREEN[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_GREEN[2];
                                        fiberRGBA[3] = alpha;
                                        break;
                                }
                            }
                                break;
                            case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                                CaretAssert((fiber->m_directionUnitVectorRGB[0] >= 0.0) && (fiber->m_directionUnitVectorRGB[0] <= 1.0));
                                CaretAssert((fiber->m_directionUnitVectorRGB[1] >= 0.0) && (fiber->m_directionUnitVectorRGB[1] <= 1.0));
                                CaretAssert((fiber->m_directionUnitVectorRGB[2] >= 0.0) && (fiber->m_directionUnitVectorRGB[2] <= 1.0));
                                CaretAssert((alpha >= 0.0) && (alpha <= 1.0));
                                glColor4f(fiber->m_directionUnitVectorRGB[0],
                                          fiber->m_directionUnitVectorRGB[1],
                                          fiber->m_directionUnitVectorRGB[2],
                                          alpha);
                                fiberRGBA[0] = fiber->m_directionUnitVectorRGB[0];
                                fiberRGBA[1] = fiber->m_directionUnitVectorRGB[1];
                                fiberRGBA[2] = fiber->m_directionUnitVectorRGB[2];
                                fiberRGBA[3] = alpha;
                                break;
                        }
                        break;
                    case FiberTrajectoryColorModel::Item::ITEM_TYPE_CARET_COLOR:
                    {
                        const CaretColorEnum::Enum caretColor = fodi->colorSource->getCaretColor();
                        const float* rgb = CaretColorEnum::toRGB(caretColor);
                        glColor4f(rgb[0],
                                  rgb[1],
                                  rgb[2],
                                  alpha);
                        fiberRGBA[0] = rgb[0];
                        fiberRGBA[1] = rgb[1];
                        fiberRGBA[2] = rgb[2];
                        fiberRGBA[3] = alpha;
                    }
                        break;
                }
                
                /*
                 * Draw the fiber
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                    {
                        /*
                         * Draw the cones
                         */
                        const float radiansToDegrees = 180.0 / M_PI;
                        const float majorAxis = std::min((vectorLength
                                                          * std::tan(fiber->m_fanningMajorAxisAngle)
                                                          * fodi->fanMultiplier),
                                                         vectorLength);
                        const float minorAxis = std::min((vectorLength
                                                          * std::tan(fiber->m_fanningMinorAxisAngle)
                                                          * fodi->fanMultiplier),
                                                         vectorLength);
                        
                        /*
                         * First cone
                         */
                        glPushMatrix();
                        glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                        glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glRotatef(-fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                        glRotatef(-fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glScalef(majorAxis * 2.0,
                                 minorAxis * 2.0,
                                 vectorLength);
                        m_shapeCone->draw(fiberRGBA);
                        glPopMatrix();
                        
                        /*
                         * Second cone but pointing in opposite direction
                         */
                        glPushMatrix();
                        glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                        glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glRotatef(180.0 -fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                        glRotatef(fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glScalef(majorAxis * 2.0,
                                 minorAxis * 2.0,
                                 vectorLength);
                        m_shapeCone->draw(fiberRGBA);
                        glPopMatrix();
                        
                    }
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                    {
                        const float radius = 2.0;
                        setLineWidth(radius);
                        glBegin(GL_LINES);
                        glVertex3fv(startXYZ);
                        glVertex3fv(endXYZ);
                        glEnd();
                    }
                        break;
                }
            }
        }
    }
    
    /*
     * Now clear the list of fiber orientations for drawing.
     */
    m_fiberOrientationsForDrawing.clear();
}

/**
 * Draw fiber trajectories on a surface.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberTrajectories(const StructureEnum::Enum structure)
{
    drawFiberTrajectories(NULL,
                          structure);
}

/**
 * Draw the fiber trajectories.
 * @param plane
 *    If a volume it is non-NULL and contains the plane of the slice.
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawFiberTrajectories(const Plane* plane,
                                                const StructureEnum::Enum structure)
{
    /*
     * Save status of clipping and disable clipping.
     * For fibers, the entire fiber symbol is displayed if its
     * origin is within the clipping planes which is tested below.
     */
    GLboolean clipPlanesEnabled[6] = {
        glIsEnabled(GL_CLIP_PLANE0),
        glIsEnabled(GL_CLIP_PLANE1),
        glIsEnabled(GL_CLIP_PLANE2),
        glIsEnabled(GL_CLIP_PLANE3),
        glIsEnabled(GL_CLIP_PLANE4),
        glIsEnabled(GL_CLIP_PLANE5)
    };
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    CaretAssert(this->browserTabContent);
    OverlaySet* overlaySet = this->browserTabContent->getOverlaySet();
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t iOver = 0; iOver < numberOfDisplayedOverlays; iOver++) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled() == false) {
            continue;
        }
        
        CaretMappableDataFile* caretMappableDataFile = NULL;
        int32_t mapIndex = -1;
        overlay->getSelectionData(caretMappableDataFile,
                                  mapIndex);
        if (caretMappableDataFile == NULL) {
            continue;
        }
        CiftiFiberTrajectoryFile* trajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(caretMappableDataFile);
        if (trajFile == NULL) {
            continue;
        }
        
        FiberTrajectoryMapProperties* ftmp = trajFile->getFiberTrajectoryMapProperties();
        
        const float proportionMinimumOpacity = ftmp->getProportionMinimumOpacity();
        const float proportionMaximumOpacity = ftmp->getProportionMaximumOpacity();
        const float proportionRangeOpacity = proportionMaximumOpacity - proportionMinimumOpacity;
        
        const float countMinimumOpacity = ftmp->getCountMinimumOpacity();
        const float countMaximumOpacity = ftmp->getCountMaximumOpacity();
        const float countRangeOpacity = countMaximumOpacity - countMinimumOpacity;
        
        const float distanceMinimumOpacity = ftmp->getDistanceMinimumOpacity();
        const float distanceMaximumOpacity = ftmp->getDistanceMaximumOpacity();
        const float distanceRangeOpacity = distanceMaximumOpacity - distanceMinimumOpacity;
        
        const FiberTrajectoryDisplayModeEnum::Enum displayMode = ftmp->getDisplayMode();
        float streamlineThreshold = std::numeric_limits<float>::max();
        switch (displayMode) {
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE:
                streamlineThreshold = ftmp->getCountStreamline();
                if (countRangeOpacity <= 0.0) {
                    continue;
                }
                break;
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED:
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
                streamlineThreshold = ftmp->getDistanceStreamline();
                if (distanceRangeOpacity <= 0.0) {
                    continue;
                }
                break;
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
                streamlineThreshold = ftmp->getProportionStreamline();
                if (proportionRangeOpacity <= 0.0) {
                    continue;
                }
                break;
        }
        
        DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
        const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->windowTabIndex);
        const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup,
                                                                                    this->windowTabIndex);
        FiberOrientationDisplayInfo fiberOrientDispInfo;
        setFiberOrientationDisplayInfo(dpfo,
                                       displayGroup,
                                       this->windowTabIndex,
                                       const_cast<Plane*>(plane),
                                       structure,
                                       ftmp->getFiberTrajectoryColorModel()->getSelectedItem(),
                                       fiberOrientDispInfo);
        
        /*
         * Fans use lighting but NOT on a volume slice
         */
        disableLighting();
        switch (symbolType) {
            case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                if (plane == NULL) {
                    enableLighting();
                }
                break;
            case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                break;
        }
        
        
        
        const std::vector<FiberOrientationTrajectory*>& trajectories = trajFile->getLoadedFiberOrientationTrajectories();
        const int64_t numTraj = static_cast<int64_t>(trajectories.size());
        for (int64_t iTraj = 0; iTraj < numTraj; iTraj++) {
            const FiberOrientationTrajectory* fiberTraj = trajectories[iTraj];
            const FiberOrientation* orientation = fiberTraj->getFiberOrientation();
            
            const float fiberFractionTotalCount = fiberTraj->getFiberFractionTotalCount();
            
            const std::vector<float>& fiberFractions = fiberTraj->getFiberFractions();
            if (fiberFractions.size() != 3) {
                CaretLogFinest("Fiber Trajectory index="
                                + AString::number(iTraj)
                                + " has "
                                + AString::number(fiberFractions.size())
                                + " fibers != 3 from file "
                                + trajFile->getFileNameNoPath());
                
                continue;
            }
            else if (fiberFractionTotalCount < streamlineThreshold) {
                continue;
            }
            float fiberOpacities[3] = { 0.0, 0.0, 0.0 };
            const float fiberCounts[3] = {
                fiberFractions[0] * fiberFractionTotalCount,
                fiberFractions[1] * fiberFractionTotalCount,
                fiberFractions[2] * fiberFractionTotalCount
            };
            
            const float fiberFractionDistance = fiberTraj->getFiberFractionDistance();
            
            /*
             * Set opacities for each fiber using mapping of minimum and
             * maximum opacities
             */
            switch (displayMode) {
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE:
                    fiberOpacities[0] = (fiberCounts[0]
                                         - countMinimumOpacity) / countRangeOpacity;
                    fiberOpacities[1] = (fiberCounts[1]
                                         - countMinimumOpacity) / countRangeOpacity;
                    fiberOpacities[2] = (fiberCounts[2]
                                         - countMinimumOpacity) / countRangeOpacity;
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED:
                    fiberOpacities[0] = ((fiberCounts[0] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
                {
                    const float distanceLog = std::log(fiberFractionDistance);
                    fiberOpacities[0] = ((fiberCounts[0] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                }
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
                    fiberOpacities[0] = (fiberFractions[0]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[1] = (fiberFractions[1]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[2] = (fiberFractions[2]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    break;
            }
            int32_t drawCount = 3;
            for (int32_t i = 0; i < 3; i++) {
                if (fiberOpacities[i] > 1.0) {
                    fiberOpacities[i] = 1.0;
                }
                else if (fiberOpacities[i] <= 0.0) {
                    fiberOpacities[i] = 0.0;
                    drawCount--;
                }
            }
            if (drawCount > 0) {
                orientation->m_fibers[0]->m_opacityForDrawing = fiberOpacities[0];
                orientation->m_fibers[1]->m_opacityForDrawing = fiberOpacities[1];
                orientation->m_fibers[2]->m_opacityForDrawing = fiberOpacities[2];
                
                addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                              orientation);
            }
        }
        
        drawAllFiberOrientations(&fiberOrientDispInfo,
                                 true);
    }
    
    glDisable(GL_BLEND);
    
    /*
     * Restore status of clipping planes enabled
     */
    if (clipPlanesEnabled[0]) glEnable(GL_CLIP_PLANE0);
    if (clipPlanesEnabled[1]) glEnable(GL_CLIP_PLANE1);
    if (clipPlanesEnabled[2]) glEnable(GL_CLIP_PLANE2);
    if (clipPlanesEnabled[3]) glEnable(GL_CLIP_PLANE3);
    if (clipPlanesEnabled[4]) glEnable(GL_CLIP_PLANE4);
    if (clipPlanesEnabled[5]) glEnable(GL_CLIP_PLANE5);
}


/**
 * Draw a cone with an elliptical shape.
 * @param rgba
 *    Color of cone.
 * @param baseXYZ
 *    Location of the base (flat wide) part of the cone
 * @param apexXYZ
 *    Location of the pointed end of the cone
 * @param baseRadiusScaling
 *    Scale the base radius by this amount
 * @param baseMajorAngle
 *    Angle for the major axis of the ellipse (units = Radians)
 *    Valid range is [0, Pi/2]
 * @param baseMinorAngle
 *    Angle for the minor axis of the ellipse (units = Radians)
 *    Valid range is [0, Pi/2]
 * @param baseRotationAngle  (units = Radians)
 *    Rotation of major axis from 'up'
 * @param backwardsFlag
 *    If true, draw the cone backwards (rotated 180 degrees).
 */
void
BrainOpenGLFixedPipeline::drawEllipticalCone(const float rgba[4],
                                             const float baseXYZ[3],
                                             const float apexXYZ[3],
                                             const float baseRadiusScaling,
                                             const float baseMajorAngleIn,
                                             const float baseMinorAngleIn,
                                             const float baseRotationAngle,
                                             const bool backwardsFlag)
{
    float x1 = apexXYZ[0];
    float y1 = apexXYZ[1];
    float z1 = apexXYZ[2];
    float vx = baseXYZ[0] - x1;
    float vy = baseXYZ[1] - y1;
    float vz = baseXYZ[2] - z1;
    
    float z = (float)std::sqrt( vx*vx + vy*vy + vz*vz );
    double ax = 0.0f;
    
    const float maxAngle = M_PI_2 * 0.95;
    float baseMajorAngle = baseMajorAngleIn;
    if (baseMajorAngle > maxAngle) {
        baseMajorAngle = maxAngle;
    }
    float baseMinorAngle = baseMinorAngleIn;
    if (baseMinorAngle > maxAngle) {
        baseMinorAngle = maxAngle;
    }
    
    const float maxWidth = z;
    const float majorAxis = std::min(z * std::tan(baseMajorAngle) * baseRadiusScaling,
                                       maxWidth);
    const float minorAxis = std::min(z * std::tan(baseMinorAngle) * baseRadiusScaling,
                                       maxWidth);
    
    double zero = 1.0e-3;
    
    if (std::abs(vz) < zero) {
        ax = 57.2957795*std::acos( vx/z ); // rotation angle in x-y plane
        if ( vx <= 0.0f ) ax = -ax;
    }
    else {
        ax = 57.2957795*std::acos( vz/z ); // rotation angle
        if ( vz <= 0.0f ) ax = -ax;
    }
    
    glPushMatrix();
    glTranslatef( x1, y1, z1 );
    
    float rx = -vy*vz;
    float ry = vx*vz;
    
    if ((std::abs(vx) < zero) && (std::fabs(vz) < zero)) {
        if (vy > 0) {
            ax = 90;
        }
    }
    
    if (std::abs(vz) < zero)  {
        glRotated(90.0, 0.0, 1.0, 0.0); // Rotate & align with x axis
        glRotated(ax, -1.0, 0.0, 0.0); // Rotate to point 2 in x-y plane
    }
    else {
        glRotated(ax, rx, ry, 0.0); // Rotate about rotation vector
    }
    
    glPushMatrix();
    
    if (backwardsFlag) {
        glRotatef(180.0,
                  0.0,
                  1.0,
                  0.0);
        glRotatef(MathFunctions::toDegrees(-baseRotationAngle), 0.0, 0.0, 1.0);
    }
    else {
        /*
         * Rotate around Z-axis using the base rotation angle
         */
        glRotatef(MathFunctions::toDegrees(baseRotationAngle), 0.0, 0.0, 1.0);
    }

    /*
     * Draw the cone
     */
    glScalef(majorAxis * 2.0,
             minorAxis * 2.0,
             z);
    m_shapeCone->draw(rgba);
    glPopMatrix();
    glPopMatrix();
}

/**
 * Draw a cone with an elliptical shape.
 * @param rgba
 *    Color of cone.
 * @param bottomXYZ
 *    Location of the bottom of the cylinder.
 * @param topXYZ
 *    Location of the top of the cylinder.
 * @param radius
 *    Radius of the cylinder.
 */
void
BrainOpenGLFixedPipeline::drawCylinder(const float rgba[4],
                                       const float bottomXYZ[3],
                                       const float topXYZ[3],
                                       const float radius)
{
    float x1 = topXYZ[0];
    float y1 = topXYZ[1];
    float z1 = topXYZ[2];
    float vx = bottomXYZ[0] - x1;
    float vy = bottomXYZ[1] - y1;
    float vz = bottomXYZ[2] - z1;
    
    float z = (float)std::sqrt( vx*vx + vy*vy + vz*vz );
    double ax = 0.0f;
    
    double zero = 1.0e-3;
    
    if (std::abs(vz) < zero) {
        ax = 57.2957795*std::acos( vx/z ); // rotation angle in x-y plane
        if ( vx <= 0.0f ) ax = -ax;
    }
    else {
        ax = 57.2957795*std::acos( vz/z ); // rotation angle
        if ( vz <= 0.0f ) ax = -ax;
    }
    
    glPushMatrix();
    glTranslatef( x1, y1, z1 );
    
    float rx = -vy*vz;
    float ry = vx*vz;
    
    if ((std::abs(vx) < zero) && (std::fabs(vz) < zero)) {
        if (vy > 0) {
            ax = 90;
        }
    }
    
    if (std::abs(vz) < zero)  {
        glRotated(90.0, 0.0, 1.0, 0.0); // Rotate & align with x axis
        glRotated(ax, -1.0, 0.0, 0.0); // Rotate to point 2 in x-y plane
    }
    else {
        glRotated(ax, rx, ry, 0.0); // Rotate about rotation vector
    }
    
    glPushMatrix();
    
    /*
     * Draw the cone
     */
    glScalef(radius * 2.0,
             radius * 2.0,
             z);
    m_shapeCylinder->draw(rgba);
    glPopMatrix();
    glPopMatrix();    
}

/**
 * Given the full size of a viewport, the gap percentage, and the number 
 * of subviewports, compute the subviewport size and gap in pixels.
 *
 * @param viewportSize
 *     Full size of the viewport.
 * @param gapPercentage
 *     Percentage of viewport used for gap between adjacent subviewports
 * @param gapOverride
 *     If greater than zero, use this value as the gap and ignore the gap percentage.
 * @param numberOfSubViewports
 *     Number of subviewports
 * @param subViewportSizeOut
 *     Output containing the size for a subviewport
 * @param gapOut
 *     Output for the size of the gap between adjacent subviewports.
 */
void
BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(const int32_t viewportSize,
                                                       const float gapPercentage,
                                                       const int32_t gapOverride,
                                                       const int32_t numberOfSubViewports,
                                                       int32_t& subViewportSizeOut,
                                                       int32_t& gapOut)
{
    subViewportSizeOut  = viewportSize;
    gapOut     = 0;
    
    if (numberOfSubViewports > 1) {
        subViewportSizeOut = viewportSize / numberOfSubViewports;
        gapOut = static_cast<int32_t>(std::floor(static_cast<double>(viewportSize * (gapPercentage / 100.0))));
        if (gapOverride > 0) {
            gapOut = gapOverride;
        }
        const double gapSum = gapOut * (numberOfSubViewports - 1);
        const int32_t subtractFromViewport = std::ceil(gapSum / numberOfSubViewports);
        subViewportSizeOut -= subtractFromViewport;
        
        int32_t checkValue = (subViewportSizeOut * numberOfSubViewports) + (gapOut * (numberOfSubViewports - 1));
        
        if (checkValue < viewportSize) {
            const bool makeViewportsLargerFlag = true;
            if (makeViewportsLargerFlag) {
                /*
                 * Since viewports are integer values, may be able to add more to gaps
                 */
                const int32_t subViewportSizePlusOne = subViewportSizeOut + 1;
                const int32_t checkValueOne = (subViewportSizePlusOne * numberOfSubViewports) + (gapOut * (numberOfSubViewports - 1));
                if (checkValueOne <= viewportSize) {
                    subViewportSizeOut = subViewportSizePlusOne;
                    checkValue = checkValueOne;
                    
                    const int32_t subViewportSizePlusTwo = subViewportSizeOut + 1;
                    const int32_t checkValueTwo = (subViewportSizePlusTwo * numberOfSubViewports) + (gapOut * (numberOfSubViewports - 1));
                    if (checkValueTwo <= viewportSize) {
                        subViewportSizeOut = subViewportSizePlusTwo;
                        checkValue = checkValueTwo;
                    }
                }
            }
            else {
                /*
                 * Since viewports are integer values, may be able to add more to gaps
                 */
                const int32_t gapPlusOne = gapOut + 1;
                const int32_t checkValueOne = (subViewportSizeOut * numberOfSubViewports) + (gapPlusOne * (numberOfSubViewports - 1));
                if (checkValueOne <= viewportSize) {
                    gapOut = gapPlusOne;
                    checkValue = checkValueOne;
                    
                    const int32_t gapPlusTwo = gapOut + 1;
                    const int32_t checkValueTwo = (subViewportSizeOut * numberOfSubViewports) + (gapPlusTwo * (numberOfSubViewports - 1));
                    if (checkValueTwo <= viewportSize) {
                        gapOut = gapPlusTwo;
                        checkValue = checkValueTwo;
                    }
                }
            }
        }
        
        CaretAssert(checkValue <= viewportSize);
    }
}

/**
 * Draw fiber orientations on surface models.
 *
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberOrientations(const StructureEnum::Enum structure)
{
    drawFiberOrientations(NULL,
                          structure);
}

/**
 * Draw the surface montage model.
 * @param browserTabContent
 *   Content of the window.
 * @param surfaceMontageModel
 *   The surface montage displayed in the window.
 * @param viewport
 *   Region for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceMontageModel(BrowserTabContent* browserTabContent,
                                                  ModelSurfaceMontage* surfaceMontageModel,
                                                  const int32_t viewport[4])
{
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    std::vector<SurfaceMontageViewport*> montageViewports;
    surfaceMontageModel->getSurfaceMontageViewportsForDrawing(tabIndex,
                                                              montageViewports);
    if (montageViewports.empty()) {
        return;
    }
    
    GLint savedVP[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedVP);
    
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    SurfaceMontageViewport::getNumberOfRowsAndColumns(montageViewports,
                                                      numberOfRows,
                                                      numberOfColumns);
    
    const GapsAndMargins* gapsAndMargins = m_brain->getGapsAndMargins();
    
    int32_t subViewportHeight = 0;
    int32_t verticalGap       = 0;
    createSubViewportSizeAndGaps(viewport[3],
                                 gapsAndMargins->getSurfaceMontageVerticalGapForWindow(m_windowIndex),
                                 -1,
                                 numberOfRows,
                                 subViewportHeight,
                                 verticalGap);
    
    int32_t subViewportWidth = 0;
    int32_t horizontalGap    = 0;
    createSubViewportSizeAndGaps(viewport[2],
                                 gapsAndMargins->getSurfaceMontageHorizontalGapForWindow(m_windowIndex),
                                 -1,
                                 numberOfColumns,
                                 subViewportWidth,
                                 horizontalGap);
    
    const int32_t numberOfViewports = static_cast<int32_t>(montageViewports.size());
    for (int32_t ivp = 0; ivp < numberOfViewports; ivp++) {
        SurfaceMontageViewport* mvp = montageViewports[ivp];
        const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel,
                                                                                     mvp->getSurface(),
                                                                                     this->windowTabIndex);
        float center[3];
        mvp->getSurface()->getBoundingBox()->getCenter(center);
        
        const int32_t rowFromTop    = mvp->getRow();
        const int32_t rowFromBottom = (numberOfRows - rowFromTop - 1);
        const int32_t column = mvp->getColumn();
        
        const int32_t surfaceViewport[4] = {
            (viewport[0] + (column * (subViewportWidth + horizontalGap))),
            (viewport[1] + (rowFromBottom * (subViewportHeight + verticalGap))),
            subViewportWidth,
            subViewportHeight
        };
        mvp->setViewport(surfaceViewport);
        
        this->setViewportAndOrthographicProjectionForSurfaceFile(surfaceViewport,
                                                                 mvp->getProjectionViewType(),
                                                                 mvp->getSurface());
        
        this->applyViewingTransformations(surfaceMontageModel,
                                          center,
                                          mvp->getProjectionViewType());
        
        this->drawSurface(mvp->getSurface(),
                          nodeColoringRGBA,
                          true);
    }
    
    glViewport(savedVP[0],
               savedVP[1],
               savedVP[2],
               savedVP[3]);
}

/**
 * Draw the whole brain.
 * @param browserTabContent
 *    Content of the window.
 * @param wholeBrainModel
 *    Model for whole brain.
 * @param viewport
 *    Region for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawWholeBrainModel(BrowserTabContent* browserTabContent,
                                      ModelWholeBrain* wholeBrainModel,
                                      const int32_t viewport[4])
{
    const int32_t tabNumberIndex = browserTabContent->getTabNumber();
    
    Surface* leftSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                    tabNumberIndex);
    Surface* rightSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                     tabNumberIndex);
    /*
     * Center using volume, if it is available
     * Otherwise, see if surface is available, but a surface is offset
     * from center so override the X-coordinate to zero.
     */
    float center[3] = { 0.0, 0.0, 0.0 };
    VolumeMappableInterface* underlayVolumeFile = wholeBrainModel->getUnderlayVolumeFile(tabNumberIndex);
    if (underlayVolumeFile != NULL) {
        BoundingBox volumeBoundingBox;
        underlayVolumeFile->getVoxelSpaceBoundingBox(volumeBoundingBox);
        volumeBoundingBox.getCenter(center);
    }
    if (leftSurface != NULL) {
        leftSurface->getBoundingBox()->getCenter(center);
        center[0] = 0.0;
    }
    else {
        if (rightSurface != NULL) {
            rightSurface->getBoundingBox()->getCenter(center);
            center[0] = 0.0;
        }
    }

    /*
     * Use a surface (if available) to set the orthographic projection size
     */
    Surface* anySurface = NULL;
    if (leftSurface != NULL) {
        anySurface = leftSurface;
    }
    else if (rightSurface != NULL) {
        anySurface = rightSurface;
    }
    
    if (anySurface != NULL) {
        this->setViewportAndOrthographicProjectionForSurfaceFile(viewport,
                                                                 ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL,
                                                                 anySurface);
    }
    else if (underlayVolumeFile != NULL) {
        this->setViewportAndOrthographicProjectionForWholeBrainVolume(viewport,
                                                            ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL,
                                                            underlayVolumeFile);
    }
    else {
        this->setViewportAndOrthographicProjection(viewport,
                                                   ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    }
    this->applyViewingTransformations(wholeBrainModel,
                                      center,
                                      ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    
    const SurfaceTypeEnum::Enum surfaceType = wholeBrainModel->getSelectedSurfaceType(tabNumberIndex);
    
    /*
     * Need depth testing for drawing slices
     */
    glEnable(GL_DEPTH_TEST);

    /*
     * Determine volumes that are to be drawn  
     */
    if (underlayVolumeFile != NULL) {
        std::vector<VolumeDrawInfo> volumeDrawInfo;
        this->setupVolumeDrawInfo(browserTabContent,
                                  m_brain,
                                  volumeDrawInfo);
        if (volumeDrawInfo.empty() == false) {
            /*
             * Voxels as 3D
             */
            drawVolumeVoxelsAsCubesWholeBrain(volumeDrawInfo);

            /*
             * Filter volumes for drawing and only draw those volumes that
             * are to be drawn as 2D volume slices.
             */
            std::vector<VolumeDrawInfo> twoDimSliceDrawVolumeDrawInfo;
            for (std::vector<VolumeDrawInfo>::iterator iter = volumeDrawInfo.begin();
                 iter != volumeDrawInfo.end();
                 iter++) {
                bool useIt = false;
                VolumeDrawInfo& vdi = *iter;
                switch (vdi.wholeBrainVoxelDrawingMode) {
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                        break;
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                        useIt = true;
                        break;
                }
                if (useIt) {
                    twoDimSliceDrawVolumeDrawInfo.push_back(vdi);
                }
            }
            
            if ( ! twoDimSliceDrawVolumeDrawInfo.empty()) {
                /*
                 * Check for oblique slice drawing
                 */
                VolumeSliceDrawingTypeEnum::Enum sliceDrawingType = browserTabContent->getSliceDrawingType();
                VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = browserTabContent->getSliceProjectionType();
                
                BrainOpenGLVolumeSliceDrawing volumeSliceDrawing;
                volumeSliceDrawing.draw(this,
                                        browserTabContent,
                                        twoDimSliceDrawVolumeDrawInfo, //volumeDrawInfo,
                                        sliceDrawingType,
                                        sliceProjectionType,
                                        viewport);
            }
        }
    }
    
    drawSurfaceFiberOrientations(StructureEnum::ALL);
    drawSurfaceFiberTrajectories(StructureEnum::ALL);

    /*
     * Draw surfaces last so that opacity works.
     */
    std::vector<Surface*> surfacesToDraw;
    const int32_t numberOfBrainStructures = m_brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
        BrainStructure* brainStructure = m_brain->getBrainStructure(i);
        const StructureEnum::Enum structure = brainStructure->getStructure();
        Surface* surface = wholeBrainModel->getSelectedSurface(structure,
                                                               tabNumberIndex);
        if (surface != NULL) {
            bool drawIt = false;
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    drawIt = browserTabContent->isWholeBrainLeftEnabled();
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    drawIt = browserTabContent->isWholeBrainRightEnabled();
                    break;
                case StructureEnum::CEREBELLUM:
                    drawIt = browserTabContent->isWholeBrainCerebellumEnabled();
                    break;
                default:
                    CaretLogWarning("programmer-issure: Surface type not left/right/cerebellum");
                    break;
            }
            
            if (drawIt) {
                surfacesToDraw.push_back(surface);
            }
        }
    }

    const int32_t numSurfaceToDraw = static_cast<int32_t>(surfacesToDraw.size());
    for (int32_t i = 0; i < numSurfaceToDraw; i++) {
        CaretAssertVectorIndex(surfacesToDraw, i);
        Surface* surface = surfacesToDraw[i];
        CaretAssert(surface);
        
        float dx = 0.0;
        float dy = 0.0;
        float dz = 0.0;
        switch (surface->getStructure()) {
            case StructureEnum::CORTEX_LEFT:
                dx = -browserTabContent->getWholeBrainLeftRightSeparation();
                if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                    && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                    dx -= surface->getBoundingBox()->getMaxX();
                }
                break;
            case StructureEnum::CORTEX_RIGHT:
                dx = browserTabContent->getWholeBrainLeftRightSeparation();
                if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                    && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                    dx -= surface->getBoundingBox()->getMinX();
                }
                break;
            case StructureEnum::CEREBELLUM:
                dz = browserTabContent->getWholeBrainCerebellumSeparation();
                break;
            default:
                CaretLogWarning("programmer-issure: Surface type not left/right/cerebellum");
                break;
        }
        
        if (surface != NULL) {
            /*
             * Draw the model annotations when the last surface is drawn.
             */
            const bool drawModelSpaceAnnotationsFlag = (i == (numSurfaceToDraw - 1));
            
            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(wholeBrainModel,
                                                                                         surface,
                                                                                         this->windowTabIndex);
            
            glPushMatrix();
            glTranslatef(dx, dy, dz);
            this->drawSurface(surface,
                              nodeColoringRGBA,
                              drawModelSpaceAnnotationsFlag);
            glPopMatrix();
        }
    }
//    /*
//     * Draw surfaces last so that opacity works.
//     */
//    const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
//    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
//        BrainStructure* brainStructure = brain->getBrainStructure(i);
//        const StructureEnum::Enum structure = brainStructure->getStructure();
//        Surface* surface = wholeBrainModel->getSelectedSurface(structure,
//                                                                    tabNumberIndex);
//        if (surface != NULL) {
//            float dx = 0.0;
//            float dy = 0.0;
//            float dz = 0.0;
//            
//            bool drawIt = false;
//            switch (structure) {
//                case StructureEnum::CORTEX_LEFT:
//                    drawIt = browserTabContent->isWholeBrainLeftEnabled();
//                    dx = -browserTabContent->getWholeBrainLeftRightSeparation();
//                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
//                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
//                        dx -= surface->getBoundingBox()->getMaxX();
//                    }
//                    break;
//                case StructureEnum::CORTEX_RIGHT:
//                    drawIt = browserTabContent->isWholeBrainRightEnabled();
//                    dx = browserTabContent->getWholeBrainLeftRightSeparation();
//                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
//                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
//                        dx -= surface->getBoundingBox()->getMinX();
//                    }
//                    break;
//                case StructureEnum::CEREBELLUM:
//                    drawIt = browserTabContent->isWholeBrainCerebellumEnabled();
//                    dz = browserTabContent->getWholeBrainCerebellumSeparation();
//                    break;
//                default:
//                    CaretLogWarning("programmer-issure: Surface type not left/right/cerebellum");
//                    break;
//            }
//            
//            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(wholeBrainModel,
//                                                                                         surface,
//                                                                                         this->windowTabIndex);
//            
//            if (drawIt) {
//                glPushMatrix();
//                glTranslatef(dx, dy, dz);
//                this->drawSurface(surface,
//                                  nodeColoringRGBA);
//                glPopMatrix();
//            }
//        }
//    }
}

/**
 * Draw a chart model.
 *
 * @param browserTabContent
 *    Content of browser tab.
 * @param chartModel
 *    The chart model.
 * @param viewport
 *    The viewport (x, y, width, height)
 */
void
BrainOpenGLFixedPipeline::drawChartData(BrowserTabContent* browserTabContent,
                    ModelChart* chartModel,
                    const int32_t viewport[4])
{
    CaretAssert(browserTabContent);
    CaretAssert(chartModel);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ChartModelCartesian* cartesianChart = NULL;
    ChartableMatrixInterface* matrixChartFile = NULL;
    const ChartDataTypeEnum::Enum chartDataType = chartModel->getSelectedChartDataType(tabIndex);

    SelectionItemDataTypeEnum::Enum selectionItemDataType = SelectionItemDataTypeEnum::INVALID;
    int32_t scalarDataSeriesMapIndex = -1;
    
    switch (chartDataType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            cartesianChart = chartModel->getSelectedDataSeriesChartModel(tabIndex);
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_DATA_SERIES;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            cartesianChart = chartModel->getSelectedFrequencySeriesChartModel(tabIndex);
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_FREQUENCY_SERIES;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            cartesianChart = chartModel->getSelectedTimeSeriesChartModel(tabIndex);
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_TIME_SERIES;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
        {
            CaretDataFileSelectionModel* matrixFileSelector = chartModel->getChartableMatrixParcelFileSelectionModel(tabIndex);
            matrixChartFile = matrixFileSelector->getSelectedFileOfType<ChartableMatrixInterface>();
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_MATRIX;
        }
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
        {
            CaretDataFileSelectionModel* fileModel = chartModel->getChartableMatrixSeriesFileSelectionModel(tabIndex);
            CaretDataFile* caretFile = fileModel->getSelectedFile();
            if (caretFile != NULL) {
                ChartableMatrixSeriesInterface* matrixSeriesFile = dynamic_cast<ChartableMatrixSeriesInterface*>(caretFile);
                if (matrixSeriesFile != NULL) {
                    matrixChartFile = matrixSeriesFile;
                    selectionItemDataType = SelectionItemDataTypeEnum::CHART_MATRIX;
                    scalarDataSeriesMapIndex = matrixSeriesFile->getSelectedMapIndex(tabIndex);
                }
                
            }
        }
            break;
    }
    
    if (cartesianChart != NULL) {
        BrainOpenGLChartDrawingFixedPipeline chartDrawing;
        chartDrawing.drawCartesianChart(m_brain,
                                        this,
                                        viewport,
                                        getTextRenderer(),
                                        cartesianChart,
                                        selectionItemDataType,
                                        this->windowTabIndex);
    }
    else if (matrixChartFile != NULL) {
        BrainOpenGLChartDrawingFixedPipeline chartDrawing;
        chartDrawing.drawMatrixChart(m_brain,
                                     this,
                                     viewport,
                                     getTextRenderer(),
                                     matrixChartFile,
                                     scalarDataSeriesMapIndex,
                                     selectionItemDataType,
                                     this->windowTabIndex);
    }
}

/**
 * Setup the orthographic projection.
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjection(const int32_t viewport[4],
                                                    const ProjectionViewTypeEnum::Enum projectionType)
{
    setOrthographicProjectionWithHeight(viewport,
                                        projectionType,
                                        getModelViewingHalfWindowHeight());
}

/**
 * Setup the orthographic projection for the given surface file.
 *
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 * @param boundingBox
 *    The bounding box used for maximum spatial extent.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjectionForWithBoundingBox(const int32_t viewport[4],
                                                 const ProjectionViewTypeEnum::Enum projectionType,
                                                 const BoundingBox* boundingBox)
{
    CaretAssert(boundingBox);
    
    /*
     * For a cortical surface, this largest dimension is the Y-Axis.
     * This worked correctly when the default view was dorsal with
     * the anterior pole at the top of the display and the posterior
     * pole at the bottom of the display.
     */
    float modelHalfHeight = std::max(std::max(boundingBox->getDifferenceX(),
                                              boundingBox->getDifferenceY()),
                                              boundingBox->getDifferenceZ()) / 2.0;
    float modelHalfWidth = modelHalfHeight;
    
    float windowHorizontalSize = boundingBox->getDifferenceY();
    float windowVerticalSize   = boundingBox->getDifferenceZ();
    
    switch (projectionType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            windowHorizontalSize = boundingBox->getDifferenceX();
            windowVerticalSize   = boundingBox->getDifferenceY();
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            break;
    }
    
    /*
     * The default view was changed to a lateral view and the above
     * code results in problems during some window resize operations.
     * But, the Z-difference of a flat surface is zero.
     *
     * See also BrowserTabContent::restoreFromScene() that tries to make
     * old scenes compatible with this new scaling.
     */
    //const float zDiff = boundingBox->getDifferenceZ();
    if (windowVerticalSize != 0.0) {
        modelHalfHeight = windowVerticalSize / 2.0;
        
//        const float yDiff = boundingBox->getDifferenceY();
        if ((windowHorizontalSize > 0.0)
            && (viewport[2] > 0.0)) {
            /*
             * Note Z is vertical, Y is horizontal when viewed
             */
            const float surfaceAspectRatio  = windowVerticalSize / windowHorizontalSize;
            const float viewportAspectRatio = (static_cast<float>(viewport[3])
                                               / static_cast<float>(viewport[2]));
            
            if (viewportAspectRatio > surfaceAspectRatio) {
                modelHalfWidth  = windowHorizontalSize / 2.0;
                modelHalfHeight = modelHalfWidth * viewportAspectRatio;
            }
        }
    }
    
    const float orthoHeight = modelHalfHeight * 1.02;
    const float orthoWidth  = modelHalfWidth  * 1.02;
    
    const bool setWidthFromHeightFlag = true;
    if (setWidthFromHeightFlag) {
        setOrthographicProjectionWithHeight(viewport,
                                            projectionType,
                                            orthoHeight);
    }
    else {
        setOrthographicProjectionWithWidth(viewport,
                                           projectionType,
                                           orthoWidth);
    }
}

/**
 * Setup the orthographic projection with the given window height.
 *
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 * @param halfWindowHeight
 *    Half of window height for model.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjectionWithHeight(const int32_t viewport[4],
                                                              const ProjectionViewTypeEnum::Enum projectionType,
                                                              const float halfWindowHeight)
{
    double width = viewport[2];
    double height = viewport[3];
    double aspectRatio = (width / height);
    this->orthographicRight  =    halfWindowHeight * aspectRatio;
    this->orthographicLeft   =   -halfWindowHeight * aspectRatio;
    this->orthographicTop    =    halfWindowHeight;
    this->orthographicBottom =   -halfWindowHeight;
    this->orthographicNear   = -1000.0; //-500.0; //-10000.0;
    this->orthographicFar    =  1000.0; //500.0; // 10000.0;
    
    switch (projectionType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            glOrtho(this->orthographicRight, this->orthographicLeft,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicFar, this->orthographicNear);
            break;
    }
}

/**
 * Setup the orthographic projection with the given window width.
 *
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 * @param halfWindowWidth
 *    Half of window width for model.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjectionWithWidth(const int32_t viewport[4],
                                                              const ProjectionViewTypeEnum::Enum projectionType,
                                                              const float halfWindowWidth)
{
    double width = viewport[2];
    double height = viewport[3];
    double aspectRatio = (width / height);
    this->orthographicRight  =    halfWindowWidth;  //  halfWindowHeight * aspectRatio;
    this->orthographicLeft   =   -halfWindowWidth;  // -halfWindowHeight * aspectRatio;
    this->orthographicTop    =    halfWindowWidth / aspectRatio;  // halfWindowHeight;
    this->orthographicBottom =   -halfWindowWidth / aspectRatio;  //-halfWindowHeight;
    this->orthographicNear   = -1000.0; //-500.0; //-10000.0;
    this->orthographicFar    =  1000.0; //500.0; // 10000.0;
    
    switch (projectionType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            glOrtho(this->orthographicRight, this->orthographicLeft,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicFar, this->orthographicNear);
            break;
    }
}

/**
 * check for an OpenGL Error.
 */
void
BrainOpenGLFixedPipeline::checkForOpenGLError(const Model* model,
                                      const AString& msgIn)
{
    BrainOpenGL::testForOpenGLError(msgIn,
                                    model,
                                    this->m_windowIndex,
                                    this->windowTabIndex);
    
//    GLenum errorCode = glGetError();
//    if (errorCode != GL_NO_ERROR) {
//        AString msg;
//        if (msgIn.isEmpty() == false) {
//            msg += (msgIn + "\n");
//        }
//        msg += ("OpenGL Error: " + AString((char*)gluErrorString(errorCode)) + "\n");
//        msg += ("OpenGL Version: " + AString((char*)glGetString(GL_VERSION)) + "\n");
//        msg += ("OpenGL Vendor:  " + AString((char*)glGetString(GL_VENDOR)) + "\n");
//        if (model != NULL) {
//            msg += ("While drawing brain model " + model->getNameForGUI(true) + "\n");
//        }
//        msg += ("In tab number " + AString::number(this->windowTabIndex) + "\n");
//        
//        GLint maxNameStackDepth, maxModelStackDepth, maxProjStackDepth;
//        glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,
//                      &maxProjStackDepth);
//        glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,
//                      &maxModelStackDepth);
//        glGetIntegerv(GL_MAX_NAME_STACK_DEPTH,
//                      &maxNameStackDepth);
//        
//        GLint nameStackDepth, modelStackDepth, projStackDepth;
//        glGetIntegerv(GL_PROJECTION_STACK_DEPTH,
//                      &projStackDepth);
//        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,
//                      &modelStackDepth);
//        glGetIntegerv(GL_NAME_STACK_DEPTH,
//                      &nameStackDepth);
//        
//        msg += ("Projection Matrix Stack Depth "
//                + AString::number(projStackDepth)
//                + "  Max Depth "
//                + AString::number(maxProjStackDepth)
//                + "\n");
//        msg += ("Model Matrix Stack Depth "
//                + AString::number(modelStackDepth)
//                + "  Max Depth "
//                + AString::number(maxModelStackDepth)
//                + "\n");
//        msg += ("Name Matrix Stack Depth "
//                + AString::number(nameStackDepth)
//                + "  Max Depth "
//                + AString::number(maxNameStackDepth)
//                + "\n");
//        SystemBacktrace myBacktrace;
//        SystemUtilities::getBackTrace(myBacktrace);
//        msg += ("Backtrace:\n"
//                + myBacktrace.toSymbolString()
//                + "\n");
//        CaretLogSevere(msg);
//    }
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut
 *    Index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y,
                                        int32_t& indexOut,
                                        float& depthOut)
{
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                    (int)y,
                    1,
                    1,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    pixels);
    
    indexOut = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &indexOut);
    
    if (indexOut >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                        1,
                        1,
                        GL_DEPTH_COMPONENT,
                        GL_FLOAT,
                        &depthOut);
    }
    this->colorIdentification->reset();
    
    glPopClientAttrib();
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut1
 *    First index of selected item.
 * @param indexOut2
 *    Second index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                                     const int32_t x,
                                                     const int32_t y,
                                                     int32_t& index1Out,
                                                     int32_t& index2Out,
                                                     float& depthOut)
{
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                 (int)y,
                 1,
                 1,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    index1Out = -1;
    index2Out = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &index1Out, &index2Out);
    
    if (index1Out >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &depthOut);
    }
    this->colorIdentification->reset();
    
    glPopClientAttrib();
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut1
 *    First index of selected item.
 * @param indexOut2
 *    Second index of selected item.
 * @param indexOut3
 *    Third index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                                     const int32_t x,
                                                     const int32_t y,
                                                     int32_t& index1Out,
                                                     int32_t& index2Out,
                                                     int32_t& index3Out,
                                                     float& depthOut)
{
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                 (int)y,
                 1,
                 1,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    index1Out = -1;
    index2Out = -1;
    index3Out = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &index1Out, &index2Out, &index3Out);
    
    if (index1Out >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &depthOut);
    }
    this->colorIdentification->reset();
    
    glPopClientAttrib();
}

/**
 * Set the selected item's screen coordinates.
 * @param item
 *    Item that has screen coordinates set.
 * @param itemXYZ
 *    Model's coordinate.
 */
void 
BrainOpenGLFixedPipeline::setSelectedItemScreenXYZ(SelectionItem* item,
                                        const float itemXYZ[3])
{
    GLdouble selectionModelviewMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, selectionModelviewMatrix);
    
    GLdouble selectionProjectionMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, selectionProjectionMatrix);
    
    GLint selectionViewport[4];
    glGetIntegerv(GL_VIEWPORT, selectionViewport);
    
    const double modelXYZ[3] = {
        itemXYZ[0],
        itemXYZ[1],
        itemXYZ[2]
    };
    
    double windowPos[3];
    if (gluProject(modelXYZ[0], 
                   modelXYZ[1], 
                   modelXYZ[2],
                   selectionModelviewMatrix,
                   selectionProjectionMatrix,
                   selectionViewport,
                   &windowPos[0],
                   &windowPos[1],
                   &windowPos[2])) {
        item->setScreenXYZ(windowPos);
        item->setModelXYZ(modelXYZ);
    }
}

/**
 * Draw sphere.
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the sphere.
 */
void 
BrainOpenGLFixedPipeline::drawSphereWithDiameter(const float rgba[4],
                                                 const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, diameter);
    m_shapeSphere->draw(rgba);
    glPopMatrix();
}

/**
 * Draw sphere.
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the sphere.
 */
void
BrainOpenGLFixedPipeline::drawSphereWithDiameter(const uint8_t rgba[4],
                                                 const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, diameter);
    m_shapeSphere->draw(rgba);
    glPopMatrix();
}

/**
 * Draw cube.
 *
 * @param rgba
 *    Color for drawing.
 * @param cubeSize
 *    Size of the cube (distance from one face to its opposite face).
 */
void
BrainOpenGLFixedPipeline::drawCube(const float rgba[4],
                                   const double cubeSize)
{
    glPushMatrix();
    glScaled(cubeSize, cubeSize, cubeSize);
    m_shapeCube->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a cuboid (3D Box)
 *
 * @param rgba
 *    Color for drawing.
 * @param sizeX
 *    X-Size of the cube (distance from -X face to its +X face).
 * @param sizeY
 *    Y-Size of the cube (distance from -Y face to its +Y face).
 * @param sizeZ
 *    Z-Size of the cube (distance from -Z face to its +X face).
 */
void
BrainOpenGLFixedPipeline::drawCuboid(const uint8_t rgba[4],
                                     const double sizeX,
                                     const double sizeY,
                                     const double sizeZ)
{
    glPushMatrix();
    glScaled(sizeX, sizeY, sizeZ);
    m_shapeCube->draw(rgba);
    glPopMatrix();
}

/**
 * Draw cube.
 *
 * @param rgba
 *    Color for drawing.
 * @param cubeSize
 *    Size of the cube (distance from one face to its opposite face).
 */
void
BrainOpenGLFixedPipeline::drawRoundedCube(const float rgba[4],
                                          const double cubeSize)
{
    glPushMatrix();
    glScaled(cubeSize, cubeSize, cubeSize);
    m_shapeCubeRounded->draw(rgba);
    glPopMatrix();
}

/**
 * Draw an outline circle.
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the circle.
 */
void
BrainOpenGLFixedPipeline::drawCircleOutline(const uint8_t rgba[4],
                                            const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, 1.0);
    m_shapeCircleOutline->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a filled circle.
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the circle.
 */
void
BrainOpenGLFixedPipeline::drawCircleFilled(const uint8_t rgba[4],
                                           const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, 1.0);
    m_shapeCircleFilled->draw(rgba);
    glPopMatrix();
}

/**
 * Draw an outline ellipse.
 *
 * @param rgba
 *    Color for drawing.
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param lineThickness
 *    Thickness of the line.
 */
void
BrainOpenGLFixedPipeline::drawEllipseOutline(const uint8_t rgba[4],
                                             const double majorAxis,
                                             const double minorAxis,
                                             const double lineThickness)
{
    glPushMatrix();
    glScaled(majorAxis, minorAxis, 1.0);
    
    BrainOpenGLShapeRingOutline* ringOutline = NULL;
    
    std::map<float, BrainOpenGLShapeRingOutline*>::iterator iter = m_shapeEllipseOutlines.find(lineThickness);
    if (iter != m_shapeEllipseOutlines.end()) {
        ringOutline = iter->second;
    }
    else {
        ringOutline = new BrainOpenGLShapeRingOutline(20,
                                                      1.0,
                                                      lineThickness);
        m_shapeEllipseOutlines.insert(std::make_pair(lineThickness,
                                                     ringOutline));
    }

    CaretAssert(ringOutline);

    ringOutline->draw(rgba);
    
    glPopMatrix();
}

/**
 * Draw an outline ellipse.
 *
 * @param rgba
 *    Color for drawing.
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 */
void
BrainOpenGLFixedPipeline::drawEllipseFilled(const uint8_t rgba[4],
                                            const double majorAxis,
                                            const double minorAxis)
{
    glPushMatrix();
    glScaled(majorAxis, minorAxis, 1.0);
    m_shapeCircleFilled->draw(rgba);
    glPopMatrix();
}


/**
 * Draw a cuboid (3D Box)
 *
 * @param rgba
 *    Color for drawing.
 * @param sizeX
 *    X-Size of the cube (distance from -X face to its +X face).
 * @param sizeY
 *    Y-Size of the cube (distance from -Y face to its +Y face).
 * @param sizeZ
 *    Z-Size of the cube (distance from -Z face to its +X face).
 */
void
BrainOpenGLFixedPipeline::drawRoundedCuboid(const uint8_t rgba[4],
                                            const double sizeX,
                                     const double sizeY,
                                     const double sizeZ)
{
    glPushMatrix();
    glScaled(sizeX, sizeY, sizeZ);
    m_shapeCubeRounded->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param rgba
 *     RGBA coloring ranging 0.0 to 1.0.
 * @param size
 *     Size of square.
 */
void 
BrainOpenGLFixedPipeline::drawSquare(const float rgba[4],
                                     const float size)
{
    if (this->inverseRotationMatrixValid) {
        glColor4fv(rgba);
        
        /*
         * Remove any rotation 
         */
        glMultMatrixd(this->inverseRotationMatrix);

        glScalef(size, size, size);
        
        /*
         * Draw both front and back side since in some instances,
         * such as surface montage, we are viweing from the far
         * side (from back of monitor)
         */
        glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glEnd();
    }
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param rgba
 *     RGBA coloring ranging 0 to 255.
 * @param size
 *     Size of square.
 */
void
BrainOpenGLFixedPipeline::drawSquare(const uint8_t rgba[4],
                                     const float size)
{
    if (this->inverseRotationMatrixValid) {
        glColor4ubv(rgba);
        
        /*
         * Remove any rotation
         */
        glMultMatrixd(this->inverseRotationMatrix);
        
        glScalef(size, size, size);
        
        /*
         * Draw both front and back side since in some instances,
         * such as surface montage, we are viweing from the far
         * side (from back of monitor)
         */
        glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glEnd();
    }
}

/**
 * Draw the user's selected image over the background
 *
 * vpContent
 *    Viewport content which image is displayed.
 */
void
BrainOpenGLFixedPipeline::drawBackgroundImage(BrainOpenGLViewportContent* vpContent)
{
    BrowserTabContent* btc = vpContent->getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    
    DisplayPropertiesImages* dpi = m_brain->getDisplayPropertiesImages();
    const int32_t tabIndex = btc->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpi->getDisplayGroupForTab(tabIndex);
    if (dpi->isDisplayed(displayGroup,
                         tabIndex)) {
        int modelViewport[4];
        vpContent->getModelViewport(modelViewport);
        
        ImageFile* imageFile = dpi->getSelectedImageFile(displayGroup,
                                                               tabIndex);
        if (imageFile != NULL) {
            drawImage(modelViewport,
                      imageFile);
        }
    }
}

/**
 * Draw the given image in the given viewport.
 *
 * @param viewport
 *    The viewport dimensions.
 * @param image
 *    The QImage that is drawn.
 */
void
BrainOpenGLFixedPipeline::drawImage(const int viewport[4],
                                    const ImageFile* imageFile)
{
    const int32_t originalImageWidth  = imageFile->getWidth();
    const int32_t originalImageHeight = imageFile->getHeight();
    const int32_t imageSize = originalImageWidth * originalImageHeight;
    if (imageSize <= 0) {
        return;
    }
    
    /*
     * Normalized width/height
     * 
     * > 1.0 ===> viewport dimension larger than image dimension
     * < 1.0 ===> viewport dimension smaller than image dimension
     */
    const int32_t viewportWidth  = viewport[2];
    const int32_t viewportHeight = viewport[3];
    const float widthNormalized  = static_cast<float>(viewportWidth)  / static_cast<float>(originalImageWidth);
    const float heightNormalized = static_cast<float>(viewportHeight) / static_cast<float>(originalImageHeight);
    
    /*
     * Scale image so that it fills window in one dimension and other
     * image dimension is less than or equal to the window dimension.
     */
    float imageScale = 0.0;
    if (widthNormalized < heightNormalized) {
        imageScale = widthNormalized;
    }
    else {
        imageScale = heightNormalized;
    }
    
    std::vector<uint8_t> imageBytesRGBA;
    int32_t imageWidth  = originalImageWidth;
    int32_t imageHeight = originalImageHeight;
    if (imageScale > 0.0) {
        imageWidth  = originalImageWidth  * imageScale;
        imageHeight = originalImageHeight * imageScale;
        imageFile->getImageResizedBytes(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                        imageWidth,
                                        imageHeight,
                                        imageBytesRGBA);
    }
    else {
        int32_t dummyWidth  = 0;
        int32_t dummyHeight = 0;
        imageFile->getImageBytesRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                     imageBytesRGBA,
                                     dummyWidth,
                                     dummyHeight);
    }
    
    const int32_t correctNumberOfBytes = imageWidth * imageHeight * 4;
    if (static_cast<int32_t>(imageBytesRGBA.size()) != correctNumberOfBytes) {
        CaretLogSevere("Image size is incorrect.  Number of bytes is "
                       + QString::number(imageBytesRGBA.size())
                       + " but should be "
                       + QString::number(correctNumberOfBytes));
    }

    /*
     * Center image in the window
     */
    const int32_t xHalfMargin = (viewportWidth - imageWidth) / 2.0;
    const int32_t xPos = std::max(xHalfMargin, 0);
    
    const int32_t yHalfMargin = (viewportHeight - imageHeight) / 2.0;
    const int32_t yPos = std::max(yHalfMargin, 0);
    
    /*
     * Reset orthographic projection to viewport size
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    const double nearClip = -1000.0;
    const double farClip  =  1000.0;
    glOrtho(0, viewportWidth,
            0, viewportHeight,
            nearClip, farClip);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    //
    // Draw image near far clipping plane
    //
    const float imageZ = 10.0 - farClip;
    glRasterPos3f(xPos, yPos, imageZ); //-500.0); // set Z so image behind surface
    
    glDrawPixels(imageWidth, imageHeight,
                 GL_RGBA, GL_UNSIGNED_BYTE,
                 (GLvoid*)&imageBytesRGBA[0]);
    
    glPopClientAttrib();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED for this method
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
BrainOpenGLFixedPipeline::drawTextAtViewportCoords(const double viewportX,
                                                   const double viewportY,
                                                   const AnnotationText& annotationText)
{
    if (getTextRenderer() != NULL) {
        getTextRenderer()->drawTextAtViewportCoords(viewportX,
                                                     viewportY,
                                                     annotationText);
    }
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED for this method
 *
 * @param modelX
 *     Model X-coordinate.
 * @param modelY
 *     Model Y-coordinate.
 * @param modelZ
 *     Model Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
BrainOpenGLFixedPipeline::drawTextAtModelCoords(const double modelX,
                                                const double modelY,
                                                const double modelZ,
                                                const AnnotationText& annotationText)
{
    if (getTextRenderer() != NULL) {
        getTextRenderer()->drawTextAtModelCoords(modelX,
                                                  modelY,
                                                  modelZ,
                                                  annotationText);
    }
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
 *
 * @param modelXYZ
 *     Model XYZ coordinates.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
BrainOpenGLFixedPipeline::drawTextAtModelCoords(const double modelXYZ[3],
                                                const AnnotationText& annotationText)
{
    if (getTextRenderer() != NULL) {
        getTextRenderer()->drawTextAtModelCoords(modelXYZ,
                                                  annotationText);
    }
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
 *
 * @param modelXYZ
 *     Model XYZ coordinates.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
BrainOpenGLFixedPipeline::drawTextAtModelCoords(const float modelXYZ[3],
                                                const AnnotationText& annotationText)
{
    if (getTextRenderer() != NULL) {
        getTextRenderer()->drawTextAtModelCoords(modelXYZ,
                                                  annotationText);
    }
}

/**
 * Draw the palettes showing how scalars are mapped
 * to colors.
 * @param brain
 *    Brain containing model being drawn.
 * @param viewport
 *    Viewport for the model.
 */
void
BrainOpenGLFixedPipeline::drawAllPalettes(Brain* brain)
{
    const bool useTheNewestPaletteDrawingFlag = true;
    if (useTheNewestPaletteDrawingFlag) {
        return;
    }
    
    /*
     * Turn off depth testing
     */
    glDisable(GL_DEPTH_TEST);
    

    /*
     * Save the projection matrix, model matrix, and viewport.
     */
    glMatrixMode(GL_PROJECTION);
    GLfloat savedProjectionMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, 
                savedProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
    GLfloat savedModelviewMatrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, 
                savedModelviewMatrix);
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, 
                  savedViewport);
    
    CaretAssert(brain);
    
    /*
     * Check for a 'selection' type mode
     */
    bool selectFlag = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            selectFlag = true;
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    if (selectFlag) {
        return;
    }
    
    this->disableLighting();
    
    PaletteFile* paletteFile = brain->getPaletteFile();
    CaretAssert(paletteFile);
    
    std::vector<CaretMappableDataFile*> mapFiles;
    std::vector<int32_t> mapIndices;
    this->browserTabContent->getDisplayedPaletteMapFiles(mapFiles,
                                                         mapIndices);
    
    /*
     * Each map file has a palette drawn to represent the
     * datas mapping to colors.
     */
    const int32_t numMapFiles = static_cast<int32_t>(mapFiles.size());
    for (int32_t i = 0; i < numMapFiles; i++) {
        const int mapIndex = mapIndices[i];
        const PaletteColorMapping* pcm = mapFiles[i]->getMapPaletteColorMapping(mapIndex);
        if (pcm != NULL) {
            const AString paletteName = pcm->getSelectedPaletteName();
            const Palette* palette = paletteFile->getPaletteByName(paletteName);
            if (palette != NULL) {
                FastStatistics* statistics = NULL;
                switch (mapFiles[i]->getPaletteNormalizationMode()) {
                    case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                        statistics = const_cast<FastStatistics*>(mapFiles[i]->getFileFastStatistics());
                        break;
                    case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                        statistics = const_cast<FastStatistics*>(mapFiles[i]->getMapFastStatistics(mapIndex));
                        break;
                }
                if (statistics != NULL) {
                    this->drawPalette(palette,
                                      pcm,
                                      statistics,
                                      i);
                }
            }
            else {
                CaretLogWarning("Palette named "
                                + paletteName
                                + " not found in palette file.");
            }
        }
    }
    
    /*
     * Restore the projection matrix, model matrix, and viewport.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(savedProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(savedModelviewMatrix);
    glViewport(savedViewport[0],
               savedViewport[1],
               savedViewport[2],
               savedViewport[3]);
}

///**
// * Draw a palette.
// * @param palette
// *    Palette that is drawn.
// * @param paletteColorMapping
// *    Controls mapping of data to colors.
// * @param statistics
// *    Statistics describing the data that is mapped to the palette.
// * @param paletteDrawingIndex
// *    Counts number of palettes being drawn for the Y-position
// */
//void 
//BrainOpenGLFixedPipeline::drawPalette(const Palette* palette,
//                                      const PaletteColorMapping* paletteColorMapping,
//                                      const FastStatistics* statistics,
//                                      const int paletteDrawingIndex)
//{
//    /*
//     * Save viewport.
//     */
//    GLint modelViewport[4];
//    glGetIntegerv(GL_VIEWPORT, modelViewport);
//    
//    /*
//     * Create a viewport for drawing the palettes in the 
//     * lower left corner of the window.  Try to use
//     * 25% of the display's width.
//     */
//    const GLint colorbarViewportWidth = std::max(static_cast<GLint>(modelViewport[2] * 0.25), 
//                                                 (GLint)120);
//    const GLint colorbarViewportHeight = 35;    
//    const GLint colorbarViewportX = modelViewport[0] + 10;
//    const GLint colorbarVerticalSpacing = 10;
//    GLint colorbarViewportY = (modelViewport[1]
//                               + colorbarVerticalSpacing
//                               + (paletteDrawingIndex * colorbarViewportHeight));
//    
//    glViewport(colorbarViewportX, 
//               colorbarViewportY, 
//               colorbarViewportWidth, 
//               colorbarViewportHeight);
//    
//    CaretLogFine("Palette " + palette->getName() + " Viewport: ("
//                   + AString::number(colorbarViewportX) + ", "
//                   + AString::number(colorbarViewportY) + ", "
//                   + AString::number(colorbarViewportWidth) + ", "
//                   + AString::number(colorbarViewportHeight)
//                   + ")\n Model Viewport: "
//                   + AString::fromNumbers(modelViewport, 4, ", "));
//    /*
//     * Types of values for display
//     */
//    const bool isPositiveDisplayed = paletteColorMapping->isDisplayPositiveDataFlag();
//    const bool isNegativeDisplayed = paletteColorMapping->isDisplayNegativeDataFlag();
//    const bool isZeroDisplayed     = paletteColorMapping->isDisplayZeroDataFlag();
//    const bool isPositiveOnly = (isPositiveDisplayed && (isNegativeDisplayed == false));
//    const bool isNegativeOnly = ((isPositiveDisplayed == false) && isNegativeDisplayed);
//   
//    /*
//     * Create an orthographic projection that ranges in X:
//     *   (-1, 1) If negative and positive displayed
//     *   (-1, 0) If positive is NOT displayed
//     *   (0, 1)  If negative is NOT displayed
//     */
//    const GLdouble halfHeight = static_cast<GLdouble>(colorbarViewportHeight / 2);
//    const GLdouble orthoHeight = halfHeight;
//    GLdouble orthoLeft = -1.0;
//    GLdouble orthoRight = 1.0;    
//    if (isPositiveOnly) {
//        orthoLeft = 0.0;
//    }
//    else if (isNegativeOnly) {
//        orthoRight = 0.0;
//    }
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(orthoLeft,  orthoRight, 
//            -orthoHeight, orthoHeight, 
//            -1.0, 1.0);
//    
//    glMatrixMode (GL_MODELVIEW);
//    glLoadIdentity();
//
//    /*
//     * A little extra so viewport gets filled
//     */
//    const GLdouble orthoLeftWithExtra = orthoLeft - 0.10;
//    const GLdouble orthoRightWithExtra = orthoRight + 0.10;
//    
//    /*
//     * Fill the palette viewport with the background color
//     * Add a little to left and right so viewport is filled (excess will get clipped)
//     */
//    glColor3fv(m_backgroundColorFloat);
//    glRectf(orthoLeftWithExtra, -orthoHeight, orthoRightWithExtra, orthoHeight);
//    
//    /*
//     * Always interpolate if the palette has only two colors
//     */
//    bool interpolateColor = paletteColorMapping->isInterpolatePaletteFlag();
//    if (palette->getNumberOfScalarsAndColors() <= 2) {
//        interpolateColor = true;
//    }
//    
//    /*
//     * Draw the colorbar starting with the color assigned
//     * to the negative end of the palette.
//     * Colorbar scalars range from -1 to 1.
//     */
//    const int iStart = palette->getNumberOfScalarsAndColors() - 1;
//    const int iEnd = 1;
//    const int iStep = -1;
//    for (int i = iStart; i >= iEnd; i += iStep) {
//        /*
//         * palette data for 'left' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* sc = palette->getScalarAndColor(i);
//        float scalar = sc->getScalar();
//        float rgba[4];
//        sc->getColor(rgba);
//        
//        /*
//         * palette data for 'right' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* nextSC = palette->getScalarAndColor(i - 1);
//        float nextScalar = nextSC->getScalar();
//        float nextRGBA[4];
//        nextSC->getColor(nextRGBA);
//        const bool isNoneColorFlag = nextSC->isNoneColor();
//        
//        /*
//         * Exclude negative regions if not displayed.
//         */
//        if (isNegativeDisplayed == false) {
//            if (nextScalar < 0.0) {
//                continue;
//            }
//            else if (scalar < 0.0) {
//                scalar = 0.0;
//            }
//        }
//        
//        /*
//         * Exclude positive regions if not displayed.
//         */
//        if (isPositiveDisplayed == false) {
//            if (scalar > 0.0) {
//                continue;
//            }
//            else if (nextScalar > 0.0) {
//                nextScalar = 0.0;
//            }
//        }
//        
//        /*
//         * Normally, the first entry has a scalar value of -1.
//         * If it does not, use the first color draw from 
//         * -1 to the first scalar value.
//         */
//        if (i == iStart) {
//            if (sc->isNoneColor() == false) {
//                if (scalar > -1.0) {
//                    const float xStart = -1.0;
//                    const float xEnd   = scalar;
//                    glColor3fv(rgba);
//                    glBegin(GL_POLYGON);
//                    glVertex3f(xStart, 0.0, 0.0);
//                    glVertex3f(xStart, -halfHeight, 0.0);
//                    glVertex3f(xEnd, -halfHeight, 0.0);
//                    glVertex3f(xEnd, 0.0, 0.0);
//                    glEnd();
//                }
//            }
//        }
//        
//        /*
//         * If the 'next' color is none, drawing
//         * is skipped to let the background show
//         * throw the 'none' region of the palette.
//         */ 
//        if (isNoneColorFlag == false) {
//            /*
//             * left and right region of an entry in the palette
//             */
//            const float xStart = scalar;
//            const float xEnd   = nextScalar;
//            
//            /*
//             * Unless interpolating, use the 'next' color.
//             */
//            float* startRGBA = nextRGBA;
//            float* endRGBA   = nextRGBA;
//            if (interpolateColor) {
//                startRGBA = rgba;
//            }
//            
//            /*
//             * Draw the region in the palette.
//             */
//            glBegin(GL_POLYGON);
//            glColor3fv(startRGBA);
//            glVertex3f(xStart, 0.0, 0.0);
//            glVertex3f(xStart, -halfHeight, 0.0);
//            glColor3fv(endRGBA);
//            glVertex3f(xEnd, -halfHeight, 0.0);
//            glVertex3f(xEnd, 0.0, 0.0);
//            glEnd();
//            
//            /*
//             * The last scalar value is normally 1.0.  If the last
//             * scalar is less than 1.0, then fill in the rest of 
//             * the palette from the last scalar to 1.0.
//             */
//            if (i == iEnd) {
//                if (nextScalar < 1.0) {
//                    const float xStart = nextScalar;
//                    const float xEnd   = 1.0;
//                    glColor3fv(nextRGBA);
//                    glBegin(GL_POLYGON);
//                    glVertex3f(xStart, 0.0, 0.0);
//                    glVertex3f(xStart, -halfHeight, 0.0);
//                    glVertex3f(xEnd, -halfHeight, 0.0);
//                    glVertex3f(xEnd, 0.0, 0.0);
//                    glEnd();
//                }
//            }
//        }
//    }
//    
//    /*
//     * Draw over thresholded regions with background color
//     */
//    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
//    if (thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
//        const float minMaxThresholds[2] = {
//            paletteColorMapping->getThresholdMinimum(thresholdType),
//            paletteColorMapping->getThresholdMaximum(thresholdType)
//        };
//        float normalizedThresholds[2];
//        
//        paletteColorMapping->mapDataToPaletteNormalizedValues(statistics,
//                                                              minMaxThresholds,
//                                                              normalizedThresholds,
//                                                              2);
//        
//        switch (paletteColorMapping->getThresholdTest()) {
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
//                glColor3fv(m_backgroundColorFloat);
//                glRectf(orthoLeftWithExtra, -orthoHeight, normalizedThresholds[0], orthoHeight);
//                glRectf(normalizedThresholds[1], -orthoHeight, orthoRightWithExtra, orthoHeight);
//                break;
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
//                glColor3fv(m_backgroundColorFloat);
//                glRectf(normalizedThresholds[0], -orthoHeight, normalizedThresholds[1], orthoHeight);
//                break;
//        }
//    }
//    
//    /*
//     * If zeros are not displayed, draw a line in the 
//     * background color at zero in the palette.
//     */
//    if (isZeroDisplayed == false) {
//        this->setLineWidth(1.0);
//        glColor3fv(m_backgroundColorFloat);
//        glBegin(GL_LINES);
//        glVertex2f(0.0, -halfHeight);
//        glVertex2f(0.0, 0.0);
//        glEnd();
//    }
//    
//    AString textLeft;
//    AString textCenter;
//    AString textRight;
//    const bool useNewFormattingFlag = true;
//    if (useNewFormattingFlag) {
//        /*
//         * NEW FORMATTING !!!!!
//         */
//        paletteColorMapping->getPaletteColorBarScaleText(statistics,
//                                                         textLeft,
//                                                         textCenter,
//                                                         textRight);
//        
//        std::vector<std::pair<float, AString> > normalizedPositionAndText;
//        paletteColorMapping->getPaletteColorBarScaleText(statistics,
//                                                         normalizedPositionAndText);
//    }
//    else {
//        float minMax[4] = { -1.0, 0.0, 0.0, 1.0 };
//        switch (paletteColorMapping->getScaleMode()) {
//            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
//            {
//                float dummy;
//                statistics->getNonzeroRanges(minMax[0], dummy, dummy, minMax[3]);
//            }
//                break;
//            case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
//            {
//                const float maxPct = paletteColorMapping->getAutoScaleAbsolutePercentageMaximum();
//                const float minPct = paletteColorMapping->getAutoScaleAbsolutePercentageMinimum();
//                
//                minMax[0] = -statistics->getApproxAbsolutePercentile(maxPct);
//                minMax[1] = -statistics->getApproxAbsolutePercentile(minPct);
//                minMax[2] =  statistics->getApproxAbsolutePercentile(minPct);
//                minMax[3] =  statistics->getApproxAbsolutePercentile(maxPct);
//            }
//                break;
//            case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
//            {
//                const float negMaxPct = paletteColorMapping->getAutoScalePercentageNegativeMaximum();
//                const float negMinPct = paletteColorMapping->getAutoScalePercentageNegativeMinimum();
//                const float posMinPct = paletteColorMapping->getAutoScalePercentagePositiveMinimum();
//                const float posMaxPct = paletteColorMapping->getAutoScalePercentagePositiveMaximum();
//                
//                minMax[0] = statistics->getApproxNegativePercentile(negMaxPct);
//                minMax[1] = statistics->getApproxNegativePercentile(negMinPct);
//                minMax[2] = statistics->getApproxPositivePercentile(posMinPct);
//                minMax[3] = statistics->getApproxPositivePercentile(posMaxPct);
//            }
//                break;
//            case PaletteScaleModeEnum::MODE_USER_SCALE:
//                minMax[0] = paletteColorMapping->getUserScaleNegativeMaximum();
//                minMax[1] = paletteColorMapping->getUserScaleNegativeMinimum();
//                minMax[2] = paletteColorMapping->getUserScalePositiveMinimum();
//                minMax[3] = paletteColorMapping->getUserScalePositiveMaximum();
//                break;
//        }
//        textLeft = AString::number(minMax[0], 'f', 1);
//        AString textCenterNeg = AString::number(minMax[1], 'f', 1);
//        if (textCenterNeg == "-0.0") {
//            textCenterNeg = "0.0";
//        }
//        AString textCenterPos = AString::number(minMax[2], 'f', 1);
//        textCenter = textCenterPos;
//        if (isNegativeDisplayed && isPositiveDisplayed) {
//            if (textCenterNeg != textCenterPos) {
//                textCenter = textCenterNeg + "/" + textCenterPos;
//            }
//        }
//        else if (isNegativeDisplayed) {
//            textCenter = textCenterNeg;
//        }
//        else if (isPositiveDisplayed) {
//            textCenter = textCenterPos;
//        }
//        textRight = AString::number(minMax[3], 'f', 1);
//    }
//    
//    /*
//     * Reset to the models viewport for drawing text.
//     */
//    glViewport(modelViewport[0], 
//               modelViewport[1], 
//               modelViewport[2], 
//               modelViewport[3]);
//    
//    /*
//     * Account for margin around colorbar when calculating text locations
//     */
//    int textCenterX = colorbarViewportX - modelViewport[0] + (colorbarViewportWidth / 2);
//    const int textLeftX   = colorbarViewportX - modelViewport[0];
//    const int textRightX  = (colorbarViewportX  - modelViewport[0] + colorbarViewportWidth);
//    if (isPositiveOnly) {
//        textCenterX = textLeftX;
//    }
//    else if (isNegativeOnly) {
//        textCenterX = textRightX;
//    }
//    
//    const int textY = 2 + colorbarViewportY  - modelViewport[1] + (colorbarViewportHeight / 2);
//    if (isNegativeDisplayed) {
//        AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
//        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
//        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
//        annotationText.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE12);
//        annotationText.setTextColor(CaretColorEnum::CUSTOM);
//        annotationText.setCustomLineColor(m_foregroundColorFloat);
//        annotationText.setText(textLeft);
//        this->drawTextAtViewportCoords(textLeftX,
//                                       textY,
//                                       annotationText);
//    }
//    
//    if (isNegativeDisplayed
//        || isZeroDisplayed
//        || isPositiveDisplayed) {
//
//        AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
//        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
//        if (isNegativeOnly) {
//            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
//        }
//        else if (isPositiveOnly) {
//            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
//        }
//        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
//        annotationText.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE12);
//        annotationText.setTextColor(CaretColorEnum::CUSTOM);
//        annotationText.setCustomLineColor(m_foregroundColorFloat);
//        annotationText.setText(textCenter);
//        this->drawTextAtViewportCoords(textCenterX,
//                                       textY,
//                                       annotationText);
//    }
//    
//    if (isPositiveDisplayed) {
//        AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
//        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
//        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
//        annotationText.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE12);
//        annotationText.setTextColor(CaretColorEnum::CUSTOM);
//        annotationText.setCustomLineColor(m_foregroundColorFloat);
//        annotationText.setText(textRight);
//        this->drawTextAtViewportCoords(textRightX,
//                                       textY,
//                                       annotationText);
//    }
//    
//    return;
//}

/**
 * Draw a palette.
 * @param palette
 *    Palette that is drawn.
 * @param paletteColorMapping
 *    Controls mapping of data to colors.
 * @param statistics
 *    Statistics describing the data that is mapped to the palette.
 * @param paletteDrawingIndex
 *    Counts number of palettes being drawn for the Y-position
 */
void
BrainOpenGLFixedPipeline::drawPalette(const Palette* palette,
                                      const PaletteColorMapping* paletteColorMapping,
                                      const FastStatistics* statistics,
                                      const int paletteDrawingIndex)
{
    /*
     * Save viewport.
     */
    GLint modelViewport[4];
    glGetIntegerv(GL_VIEWPORT, modelViewport);
    
    /*
     * Create a viewport for drawing the palettes in the
     * lower left corner of the window.  Try to use
     * 25% of the display's width.
     */
    const GLint colorbarViewportWidth = std::max(static_cast<GLint>(modelViewport[2] * 0.25),
                                                 (GLint)120);
    const GLint colorbarViewportHeight = 35;
    const GLint colorbarViewportX = modelViewport[0] + 10;
    const GLint colorbarVerticalSpacing = 10;
    GLint colorbarViewportY = (modelViewport[1]
                               + colorbarVerticalSpacing
                               + (paletteDrawingIndex * colorbarViewportHeight));
    
    glViewport(colorbarViewportX,
               colorbarViewportY,
               colorbarViewportWidth,
               colorbarViewportHeight);
    
    CaretLogFine("Palette " + palette->getName() + " Viewport: ("
                 + AString::number(colorbarViewportX) + ", "
                 + AString::number(colorbarViewportY) + ", "
                 + AString::number(colorbarViewportWidth) + ", "
                 + AString::number(colorbarViewportHeight)
                 + ")\n Model Viewport: "
                 + AString::fromNumbers(modelViewport, 4, ", "));
    /*
     * Types of values for display
     */
    const bool isPositiveDisplayed = paletteColorMapping->isDisplayPositiveDataFlag();
    const bool isNegativeDisplayed = paletteColorMapping->isDisplayNegativeDataFlag();
    const bool isZeroDisplayed     = paletteColorMapping->isDisplayZeroDataFlag();
    const bool isPositiveOnly = (isPositiveDisplayed && (isNegativeDisplayed == false));
    const bool isNegativeOnly = ((isPositiveDisplayed == false) && isNegativeDisplayed);
    
    /*
     * Create an orthographic projection that ranges in X:
     *   (-1, 1) If negative and positive displayed
     *   (-1, 0) If positive is NOT displayed
     *   (0, 1)  If negative is NOT displayed
     */
    const GLdouble halfHeight = static_cast<GLdouble>(colorbarViewportHeight / 2);
    const GLdouble orthoHeight = halfHeight;
    GLdouble orthoLeft = -1.0;
    GLdouble orthoRight = 1.0;
    if (isPositiveOnly) {
        orthoLeft = 0.0;
    }
    else if (isNegativeOnly) {
        orthoRight = 0.0;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(orthoLeft,  orthoRight,
            -orthoHeight, orthoHeight,
            -1.0, 1.0);
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * A little extra so viewport gets filled
     */
    const GLdouble orthoLeftWithExtra = orthoLeft - 0.10;
    const GLdouble orthoRightWithExtra = orthoRight + 0.10;
    
    /*
     * Fill the palette viewport with the background color
     * Add a little to left and right so viewport is filled (excess will get clipped)
     */
    glColor3fv(m_backgroundColorFloat);
    glRectf(orthoLeftWithExtra, -orthoHeight, orthoRightWithExtra, orthoHeight);
    
    /*
     * Always interpolate if the palette has only two colors
     */
    bool interpolateColor = paletteColorMapping->isInterpolatePaletteFlag();
    if (palette->getNumberOfScalarsAndColors() <= 2) {
        interpolateColor = true;
    }
    
    /*
     * Draw the colorbar starting with the color assigned
     * to the negative end of the palette.
     * Colorbar scalars range from -1 to 1.
     */
    const int iStart = palette->getNumberOfScalarsAndColors() - 1;
    const int iEnd = 1;
    const int iStep = -1;
    for (int i = iStart; i >= iEnd; i += iStep) {
        /*
         * palette data for 'left' side of a color in the palette.
         */
        const PaletteScalarAndColor* sc = palette->getScalarAndColor(i);
        float scalar = sc->getScalar();
        float rgba[4];
        sc->getColor(rgba);
        
        /*
         * palette data for 'right' side of a color in the palette.
         */
        const PaletteScalarAndColor* nextSC = palette->getScalarAndColor(i - 1);
        float nextScalar = nextSC->getScalar();
        float nextRGBA[4];
        nextSC->getColor(nextRGBA);
        const bool isNoneColorFlag = nextSC->isNoneColor();
        
        /*
         * Exclude negative regions if not displayed.
         */
        if (isNegativeDisplayed == false) {
            if (nextScalar < 0.0) {
                continue;
            }
            else if (scalar < 0.0) {
                scalar = 0.0;
            }
        }
        
        /*
         * Exclude positive regions if not displayed.
         */
        if (isPositiveDisplayed == false) {
            if (scalar > 0.0) {
                continue;
            }
            else if (nextScalar > 0.0) {
                nextScalar = 0.0;
            }
        }
        
        /*
         * Normally, the first entry has a scalar value of -1.
         * If it does not, use the first color draw from
         * -1 to the first scalar value.
         */
        if (i == iStart) {
            if (sc->isNoneColor() == false) {
                if (scalar > -1.0) {
                    const float xStart = -1.0;
                    const float xEnd   = scalar;
                    glColor3fv(rgba);
                    glBegin(GL_POLYGON);
                    glVertex3f(xStart, 0.0, 0.0);
                    glVertex3f(xStart, -halfHeight, 0.0);
                    glVertex3f(xEnd, -halfHeight, 0.0);
                    glVertex3f(xEnd, 0.0, 0.0);
                    glEnd();
                }
            }
        }
        
        /*
         * If the 'next' color is none, drawing
         * is skipped to let the background show
         * throw the 'none' region of the palette.
         */
        if (isNoneColorFlag == false) {
            /*
             * left and right region of an entry in the palette
             */
            const float xStart = scalar;
            const float xEnd   = nextScalar;
            
            /*
             * Unless interpolating, use the 'next' color.
             */
            float* startRGBA = nextRGBA;
            float* endRGBA   = nextRGBA;
            if (interpolateColor) {
                startRGBA = rgba;
            }
            
            /*
             * Draw the region in the palette.
             */
            glBegin(GL_POLYGON);
            glColor3fv(startRGBA);
            glVertex3f(xStart, 0.0, 0.0);
            glVertex3f(xStart, -halfHeight, 0.0);
            glColor3fv(endRGBA);
            glVertex3f(xEnd, -halfHeight, 0.0);
            glVertex3f(xEnd, 0.0, 0.0);
            glEnd();
            
            /*
             * The last scalar value is normally 1.0.  If the last
             * scalar is less than 1.0, then fill in the rest of
             * the palette from the last scalar to 1.0.
             */
            if (i == iEnd) {
                if (nextScalar < 1.0) {
                    const float xStart = nextScalar;
                    const float xEnd   = 1.0;
                    glColor3fv(nextRGBA);
                    glBegin(GL_POLYGON);
                    glVertex3f(xStart, 0.0, 0.0);
                    glVertex3f(xStart, -halfHeight, 0.0);
                    glVertex3f(xEnd, -halfHeight, 0.0);
                    glVertex3f(xEnd, 0.0, 0.0);
                    glEnd();
                }
            }
        }
    }
    
    /*
     * Draw over thresholded regions with background color
     */
    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
    if (thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
        const float minMaxThresholds[2] = {
            paletteColorMapping->getThresholdMinimum(thresholdType),
            paletteColorMapping->getThresholdMaximum(thresholdType)
        };
        float normalizedThresholds[2];
        
        paletteColorMapping->mapDataToPaletteNormalizedValues(statistics,
                                                              minMaxThresholds,
                                                              normalizedThresholds,
                                                              2);
        
        switch (paletteColorMapping->getThresholdTest()) {
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                glColor3fv(m_backgroundColorFloat);
                glRectf(orthoLeftWithExtra, -orthoHeight, normalizedThresholds[0], orthoHeight);
                glRectf(normalizedThresholds[1], -orthoHeight, orthoRightWithExtra, orthoHeight);
                break;
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                glColor3fv(m_backgroundColorFloat);
                glRectf(normalizedThresholds[0], -orthoHeight, normalizedThresholds[1], orthoHeight);
                break;
        }
    }
    
    /*
     * If zeros are not displayed, draw a line in the
     * background color at zero in the palette.
     */
    if (isZeroDisplayed == false) {
        this->setLineWidth(1.0);
        glColor3fv(m_backgroundColorFloat);
        glBegin(GL_LINES);
        glVertex2f(0.0, -halfHeight);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
    
    /*
     * Reset to the models viewport for drawing text.
     */
    glViewport(modelViewport[0],
               modelViewport[1],
               modelViewport[2],
               modelViewport[3]);

    /*
     * Get the numeric text values for display above the color bar
     */
    std::vector<std::pair<float, AString> > normalizedPositionAndText;
    paletteColorMapping->getPaletteColorBarScaleText(statistics,
                                                     normalizedPositionAndText);
    
    const int textViewportY = 2 + colorbarViewportY  - modelViewport[1] + (colorbarViewportHeight / 2);
    const int32_t numericTextCount = static_cast<int32_t>(normalizedPositionAndText.size());
    for (int32_t iText = 0; iText < numericTextCount; iText++) {
        CaretAssertVectorIndex(normalizedPositionAndText, iText);
        
        const float textNormalizedX = normalizedPositionAndText[iText].first;
        const float textViewportX = ((colorbarViewportX - modelViewport[0])
                                     + static_cast<int32_t>(textNormalizedX * colorbarViewportWidth));
        
        AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        if (iText == 0) {
            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
        }
        else if (iText == (numericTextCount - 1)) {
            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        }
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        annotationText.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE14);
        annotationText.setTextColor(CaretColorEnum::CUSTOM);
        annotationText.setCustomTextColor(m_foregroundColorFloat);
        annotationText.setText(normalizedPositionAndText[iText].second);
        this->drawTextAtViewportCoords(textViewportX,
                                       textViewportY,
                                       annotationText);
    }
}

/**
 * @return A string containing the state of OpenGL (depth testing, lighting, etc.)
 */
AString
BrainOpenGLFixedPipeline::getStateOfOpenGL() const
{
    AString s = BrainOpenGL::getStateOfOpenGL();
    
    s.appendWithNewLine("Fixed Pipeline State:");
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_BLEND", GL_BLEND));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE0", GL_CLIP_PLANE0));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE1", GL_CLIP_PLANE1));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE2", GL_CLIP_PLANE2));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE3", GL_CLIP_PLANE3));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE4", GL_CLIP_PLANE4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE5", GL_CLIP_PLANE5));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_COLOR_MATERIAL", GL_COLOR_MATERIAL));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CULL_FACE", GL_CULL_FACE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_DEPTH_TEST", GL_DEPTH_TEST));
    s.appendWithNewLine("   " + getOpenGLBooleanAsText("GL_LIGHT_MODEL_LOCAL_VIEWER", GL_LIGHT_MODEL_LOCAL_VIEWER));
    s.appendWithNewLine("   " + getOpenGLBooleanAsText("GL_LIGHT_MODEL_TWO_SIDE", GL_LIGHT_MODEL_TWO_SIDE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHTING", GL_LIGHTING));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT0", GL_LIGHT0));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT1", GL_LIGHT1));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT2", GL_LIGHT2));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT3", GL_LIGHT3));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT4", GL_LIGHT4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT5", GL_LIGHT5));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT0, GL_DIFFUSE", GL_LIGHT0, GL_DIFFUSE, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT0, GL_POSITION", GL_LIGHT0, GL_POSITION, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT1, GL_DIFFUSE", GL_LIGHT1, GL_DIFFUSE, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT1, GL_POSITION", GL_LIGHT1, GL_POSITION, 4));
    s.appendWithNewLine("   " + getOpenGLFloatAsText("GL_LIGHT_MODEL_AMBIENT", GL_LIGHT_MODEL_AMBIENT, 4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LINE_SMOOTH", GL_LINE_SMOOTH));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_NORMALIZE", GL_NORMALIZE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_POLYGON_OFFSET_FILL", GL_POLYGON_OFFSET_FILL));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_POLYGON_SMOOTH", GL_POLYGON_SMOOTH));
    
    GLint frontFace;
    glGetIntegerv(GL_FRONT_FACE, &frontFace);
    AString frontFaceValue;
    switch (frontFace) {
        case GL_CW:
            frontFaceValue = "GL_CW";
            break;
        case GL_CCW:
            frontFaceValue = "GL_CCW";
            break;
        default:
            frontFaceValue = "INVALID";
            break;
    }
    s.appendWithNewLine("   Front Face " + frontFaceValue);
    
    GLint cullFace;
    glGetIntegerv(GL_FRONT_FACE, &cullFace);
    AString cullFaceValue;
    switch (cullFace) {
        case GL_FRONT:
            cullFaceValue = "GL_FRONT";
            break;
        case GL_BACK:
            cullFaceValue = "GL_BACK";
            break;
        case GL_FRONT_AND_BACK:
            cullFaceValue = "GL_FRONT_AND_BACK";
            break;
        default:
            cullFaceValue = "INVALID";
            break;
    }
    s.appendWithNewLine("   Cull Face " + cullFaceValue);
    
    return s;
}



//============================================================================
/**
 * Constructor.
 */
BrainOpenGLFixedPipeline::VolumeDrawInfo::VolumeDrawInfo(CaretMappableDataFile* mapFile,
                                                         VolumeMappableInterface* volumeFile,
                                                         Brain* brain,
                                                         PaletteColorMapping* paletteColorMapping,
                                                         const FastStatistics* statistics,
                                                         const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode,
                                                         const int32_t mapIndex,
                                                         const float opacity) 
: statistics(statistics) {
    this->mapFile = mapFile;
    this->volumeFile = volumeFile;
    this->brain = brain;
    this->paletteColorMapping = paletteColorMapping;
    this->wholeBrainVoxelDrawingMode = wholeBrainVoxelDrawingMode;
    this->mapIndex = mapIndex;
    this->opacity    = opacity;
}
