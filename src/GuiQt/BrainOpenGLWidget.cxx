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

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include <algorithm>
#include <cmath>

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#endif
#include <QOpenGLContext>
#include <QToolTip>
#include <QWheelEvent>

#include "Border.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLShape.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorManager.h"
#include "DummyFontTextRenderer.h"
#include "EventBrainReset.h"
#include "EventImageCapture.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventIdentificationRequest.h"
#include "EventUserInterfaceUpdate.h"
#include "FtglFontTextRenderer.h"
#include "GuiManager.h"
#include "KeyEvent.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "MouseEvent.h"
#include "OffScreenOpenGLRenderer.h"
#include "SelectionManager.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxelEditing.h"
#include "SessionManager.h"
#include "Surface.h"
#include "TileTabsConfiguration.h"
#include "UserInputModeAnnotations.h"
#include "UserInputModeBorders.h"
#include "UserInputModeFoci.h"
#include "UserInputModeImage.h"
#include "UserInputModeView.h"
#include "UserInputModeVolumeEdit.h"
#include "WuQMessageBox.h"

using namespace caret;

/**
 * Constructor.
 * 
 * NOTE: We do not use "sharing" of textures and display lists 
 * between windows.  There are some issues with sharing and
 * recreation of OpenGL contexts such as when renderPixmap()
 * function is used.  
 *
 * @param
 *   The parent widget.
 */
BrainOpenGLWidget::BrainOpenGLWidget(QWidget* parent,
                                     const BrainOpenGLWidget* shareWidget,
                                     const int32_t windowIndex)
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
: QOpenGLWidget(parent),
#else
: QGLWidget(parent,
            shareWidget),
#endif
windowIndex(windowIndex)
{
    this->borderBeingDrawn = new Border();
    
    m_mousePositionValid = false;
    m_mousePositionEvent.grabNew(new MouseEvent(NULL,
                                                NULL,
                                                -1,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                false));
    
    this->userInputAnnotationsModeProcessor = new UserInputModeAnnotations(windowIndex);
    this->userInputBordersModeProcessor = new UserInputModeBorders(this->borderBeingDrawn,
                                                                   windowIndex);
    this->userInputFociModeProcessor = new UserInputModeFoci(windowIndex);
    this->userInputImageModeProcessor = new UserInputModeImage(windowIndex);
    this->userInputVolumeEditModeProcessor = new UserInputModeVolumeEdit(windowIndex);
    this->userInputViewModeProcessor = new UserInputModeView();
    this->selectedUserInputProcessor = this->userInputViewModeProcessor;
    this->selectedUserInputProcessor->initialize();
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    this->mouseNewDraggingStartedFlag = false;
    
    m_newKeyPressStartedFlag = true;
    
    /*
     * Mouse tracking must be on to receive mouse move events
     * when the mouse is NOT down.  When this property is false
     * mouse move events are only received when the at least
     * one mouse button is down.
     */
    setMouseTracking(true);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IDENTIFICATION_REQUEST);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IMAGE_CAPTURE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    m_openGLContextSharingValid = true;
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Default format is set by desktop.cxx call to initializeDefaultGLFormat().
     */
    setFormat(QSurfaceFormat::defaultFormat());
#else
    /*
     * Test to see if OpenGL context sharing is valid.
     * If there is no sharingWidget, then this is the
     * first window opened.
     */
    if (shareWidget != NULL) {
        if ( ! isSharing()) {
            m_openGLContextSharingValid = false;
        }
    }
#endif
    
//    std::cout << "Share widget: " << std::hex << (uint64_t)shareWidget
//    << ", Context Pointer: " << std::hex << (uint64_t)context()
//    << ", Share Group: " << std::hex << (uint64_t)context()->contextHandle()->shareGroup()
//    << ", Sharing enabled: " << (isSharing() ? "Yes" : "No")
//    << std::endl;
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /* Context is not yet valid for QOpenGLWidget */
    m_contextShareGroupPointer = NULL;
#else
    CaretAssert(context());
    CaretAssert(context()->contextHandle());
    m_contextShareGroupPointer = context()->contextHandle()->shareGroup();
    CaretAssert(m_contextShareGroupPointer);
#endif
    
    s_brainOpenGLWidgets.insert(this);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
    makeCurrent();
    
    this->clearDrawingViewportContents();
    
    delete this->userInputViewModeProcessor;
    delete this->userInputAnnotationsModeProcessor;
    delete this->userInputBordersModeProcessor;
    delete this->userInputFociModeProcessor;
    delete this->userInputImageModeProcessor;
    delete this->userInputVolumeEditModeProcessor;
    this->selectedUserInputProcessor = NULL; /* DO NOT DELETE since it does not own the object to which it points */
    
    delete this->borderBeingDrawn;
    
    EventManager::get()->removeAllEventsFromListener(this);
    
    s_brainOpenGLWidgets.erase(this);
    if (s_brainOpenGLWidgets.empty()) {
        if (s_singletonOpenGL != NULL) {
            delete s_singletonOpenGL;
            s_singletonOpenGL = NULL;
        }
    }
}

/**
 * @return True if OpenGL Context Sharing is valid among
 * multiple graphics windows.
 * Note: If there is one window, we declare sharing valid.
 */
bool
BrainOpenGLWidget::isOpenGLContextSharingValid() const
{
    return m_openGLContextSharingValid;
}


/**
 * Initializes graphics.
 */
void 
BrainOpenGLWidget::initializeGL()
{
    if (s_singletonOpenGL == NULL) {
        /*
         * OpenGL drawing will take ownership of the text renderer
         * and handle deletion of the text renderer.
         */
        BrainOpenGLTextRenderInterface* textRenderer = new FtglFontTextRenderer();
        if (! textRenderer->isValid()) {
            delete textRenderer;
            textRenderer = NULL;
            CaretLogWarning("Unable to create FTGL Font Renderer.\n"
                            "No text will be available in graphics window.");
        }
        if (textRenderer == NULL) {
            textRenderer = new DummyFontTextRenderer();
        }
        CaretAssert(textRenderer);
        
        s_singletonOpenGL = new BrainOpenGLFixedPipeline(textRenderer);
    }
    
    s_singletonOpenGL->initializeOpenGL();
    
    this->lastMouseX = 0;
    this->lastMouseY = 0;
    this->isMousePressedNearToolBox = false;

    this->setFocusPolicy(Qt::StrongFocus);
    

    CaretLogConfig(getOpenGLInformation());
    
    if (s_defaultGLFormatInitialized == false) {
        CaretLogSevere("PROGRAM ERROR: The default OpenGL has not been set.\n"
                       "Need to call BrainOpenGLWidget::initializeDefaultGLFormat() prior to "
                       "instantiating an instance of this class.");
    }
}

/**
 * @return Information about OpenGL.
 */
QString
BrainOpenGLWidget::getOpenGLInformation()
{
    AString info;
#if QT_VERSION >= 0x050000
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    info += ("Rendering with Qt5 QOpenGLWidget.\n");
#else
    info += ("Rendering with Qt5 QGLWidget (deprecated).\n");
#endif
    info += ("\n");
#endif
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    QSurfaceFormat format = this->format();
    AString swapInfo("Swap Behavior: ");
    switch (format.swapBehavior()) {
        case QSurfaceFormat::DefaultSwapBehavior:
            swapInfo += ("DefaultSwapBehavior");
            break;
        case QSurfaceFormat::SingleBuffer:
            swapInfo += ("SingleBuffer");
            break;
        case QSurfaceFormat::DoubleBuffer:
            swapInfo += ("DoubleBuffer");
            break;
        case QSurfaceFormat::TripleBuffer:
            swapInfo += ("TripleBuffer");
            break;
    }

    info += ("QOpenGLWidget Context:"
             "\n   Alpha: " + AString::fromBool(format.hasAlpha())
             + "\n   Alpha size: " + AString::number(format.alphaBufferSize())
             + "\n   Depth size: " + AString::number(format.depthBufferSize())
             + "\n   Red size: " + AString::number(format.redBufferSize())
             + "\n   Green size: " + AString::number(format.greenBufferSize())
             + "\n   Blue size: " + AString::number(format.blueBufferSize())
             + "\n   Samples size: " + AString::number(format.samples())
             + "\n   Stencil size: " + AString::number(format.stencilBufferSize())
             + "\n   " + swapInfo
             + "\n   Swap Interval: " + AString::number(format.swapInterval())
             + "\n   Stereo: " + AString::fromBool(format.stereo())
             + "\n   Major Version: " + AString::number(format.majorVersion())
             + "\n   Minor Version: " + AString::number(format.minorVersion()));
    
    switch (format.profile()) {
        case QSurfaceFormat::NoProfile:
            info += ("\nOpenGL NoProfile - OpenGL version is lower than 3.2. For 3.2 and newer this is same as CoreProfile.");
            break;
        case QSurfaceFormat::CoreProfile:
            info += ("\nOpenGL CoreProfile - Functionality deprecated in OpenGL version 3.0 is not available.");
            break;
        case QSurfaceFormat::CompatibilityProfile:
            info += ("\nOpenGL CompatibilityProfile - Functionality from earlier OpenGL versions is available.");
            break;
    }
    
#else
    QGLFormat format = this->format();
    info += ("QGLWidget Context:"
             "\n   Accum: " + AString::fromBool(format.accum())
             + "\n   Accum size: " + AString::number(format.accumBufferSize())
             + "\n   Alpha: " + AString::fromBool(format.alpha())
             + "\n   Alpha size: " + AString::number(format.alphaBufferSize())
             + "\n   Depth: " + AString::fromBool(format.depth())
             + "\n   Depth size: " + AString::number(format.depthBufferSize())
             + "\n   Direct Rendering: " + AString::fromBool(format.directRendering())
             + "\n   Red size: " + AString::number(format.redBufferSize())
             + "\n   Green size: " + AString::number(format.greenBufferSize())
             + "\n   Blue size: " + AString::number(format.blueBufferSize())
             + "\n   Double Buffer: " + AString::fromBool(format.doubleBuffer())
             + "\n   RGBA: " + AString::fromBool(format.rgba())
             + "\n   Samples: " + AString::fromBool(format.sampleBuffers())
             + "\n   Samples size: " + AString::number(format.samples())
             + "\n   Stencil: " + AString::fromBool(format.stencil())
             + "\n   Stencil size: " + AString::number(format.stencilBufferSize())
             + "\n   Swap Interval: " + AString::number(format.swapInterval())
             + "\n   Stereo: " + AString::fromBool(format.stereo())
             + "\n   Major Version: " + AString::number(format.majorVersion())
             + "\n   Minor Version: " + AString::number(format.minorVersion()));
#endif
    
    info += ("\n\n" + s_singletonOpenGL->getOpenGLInformation());
    
#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    int32_t numDisplayLists = 0;
    for (GLuint iList = 1; iList < 1000; iList++) {
        if (glIsList(iList)) {
            numDisplayLists++;
        }
    }
    info += ("\nAt least "
             + AString::number(numDisplayLists)
             + " display lists are allocated in first OpenGL context.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    info += "\n";
    
    return info;
}

/**
 * Called when widget is resized.
 */
void 
BrainOpenGLWidget::resizeGL(int w, int h)
{
    this->windowWidth[this->windowIndex] = w;
    this->windowHeight[this->windowIndex] = h;
}

/**
 * @return Pointer to the border that is being drawn.
 */
Border* 
BrainOpenGLWidget::getBorderBeingDrawn()
{
    return this->borderBeingDrawn;
}

/**
 * Clear the contents for drawing into the viewports.
 */
void 
BrainOpenGLWidget::clearDrawingViewportContents()
{
    m_windowContent.clear();
}

/**
 * Update the cursor from the active user input processor.
 */
void
BrainOpenGLWidget::updateCursor()
{
    /*
     * Set the cursor to that requested by the user input processor
     */
    CursorEnum::Enum cursor = this->selectedUserInputProcessor->getCursor();
    
    GuiManager::get()->getCursorManager()->setCursorForWidget(this,
                                                              cursor);
}

/**
 * Perform experimental off screen image capture.
 */
QImage
BrainOpenGLWidget::performOffScreenImageCapture(const int32_t imageWidth,
                                                const int32_t imageHeight)
{
    makeCurrent();
    
    QImage image;
    
    OffScreenOpenGLRenderer offscreen(this,
                                      imageWidth,
                                      imageHeight);
    if (offscreen.isError()) {
        WuQMessageBox::errorOk(this,
                               offscreen.getErrorMessage());
        doneCurrent();
        return image;
    }

    const int32_t viewport[4] = { 0, 0, imageWidth, imageHeight };
    
    BrainOpenGLWindowContent windowContent;
    getDrawingWindowContent(viewport,
                            windowContent);
    
    s_singletonOpenGL->drawModels(this->windowIndex,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  windowContent.getAllTabViewports());
    
    if (offscreen.isError()) {
        WuQMessageBox::errorOk(this,
                               offscreen.getErrorMessage());
        doneCurrent();
        return image;
    }
    
    return offscreen.getImage();
}

/**
 * Get the content for drawing the window.
 * 
 * @param windowViewport
 *     Viewport of the window.
 * @param windowContent
 *     Window content that is updated with window and its tabs.
 */
void
BrainOpenGLWidget::getDrawingWindowContent(const int32_t windowViewportIn[4],
                                           BrainOpenGLWindowContent& windowContent) const
{
    windowContent.clear();
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    
    /*
     * When restoring scene, the first browser window may get closed.
     * This occurs when the user creates a scene that does not contain
     * the first browser window.  The close() method in QWindow may not execute
     * destruction of the window so this widget may still be valid but
     * GuiManager no longer has a pointer to the window since it is being closed.
     * In this case, the window will be NULL.
     *
     * This problem is related to the moving window properties to the SessionManager,
     * in Feb 2018.
     */
    if (bbw == NULL) {
        return;
    }
    
    EventBrowserWindowDrawingContent getModelEvent(this->windowIndex);
    EventManager::get()->sendEvent(getModelEvent.getPointer());
    
    if (getModelEvent.isError()) {
        return;
    }
    
    int32_t windowViewport[4] = {
        windowViewportIn[0],
        windowViewportIn[1],
        windowViewportIn[2],
        windowViewportIn[3]
    };
    
    if (bbw->isWindowAspectRatioLocked()) {
        const float aspectRatio = bbw->getAspectRatio();
        if (aspectRatio > 0.0) {
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(windowViewport,
                                                                     aspectRatio);
        }
    }
    
    /*
     * Highlighting of border points
     */
    s_singletonOpenGL->setDrawHighlightedEndPoints(false);
    if (this->selectedUserInputProcessor == this->userInputBordersModeProcessor) {
        s_singletonOpenGL->setDrawHighlightedEndPoints(this->userInputBordersModeProcessor->isHighlightBorderEndPoints());
    }
    
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    const int32_t numberOfTabs = getModelEvent.getNumberOfBrowserTabs();
    std::vector<BrowserTabContent*> allTabs;
    for (int32_t i = 0; i < numberOfTabs; i++) {
        allTabs.push_back(getModelEvent.getBrowserTab(i));
    }
    
    BrowserWindowContent* browserWindowContent = getModelEvent.getBrowserWindowContent();
    CaretAssert(browserWindowContent);

    if (browserWindowContent->isTileTabsEnabled()
        && (numberOfTabs > 1)) {
        const int32_t windowWidth  = windowViewport[2];
        const int32_t windowHeight = windowViewport[3];
        
        std::vector<int32_t> rowHeights;
        std::vector<int32_t> columnsWidths;
        
        /*
         * Determine if default configuration for tiles
         */
        TileTabsConfiguration* tileTabsConfiguration = browserWindowContent->getSelectedTileTabsConfiguration();
        CaretAssert(tileTabsConfiguration);
        
        /*
         * Get the sizes of the tab tiles from the tile tabs configuration
         */
        if (tileTabsConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                             windowHeight,
                                                                             numberOfTabs,
                                                                             browserWindowContent->getTileTabsConfigurationMode(),
                                                                             rowHeights,
                                                                             columnsWidths)) {
            
            /*
             * Create the viewport drawing contents for all tabs
             */
            
            std::vector<BrainOpenGLViewportContent*> tabViewportContent = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabs,
                                                                                                                                       browserWindowContent,
                                                                                                                                       gapsAndMargins,
                                                                                                                                       windowViewport,
                                                                                                                                       getModelEvent.getTabIndexForTileTabsHighlighting());
            for (auto tabvp : tabViewportContent) {
                windowContent.addTabViewport(tabvp);
            }
        }
        else {
            CaretLogSevere("Tile Tabs Row/Column sizing failed !!!");
        }
    }
    else if (numberOfTabs >= 1) {
        BrainOpenGLViewportContent* vc = BrainOpenGLViewportContent::createViewportForSingleTab(allTabs,
                                                                                                getModelEvent.getSelectedBrowserTabContent(),
                                                                                                gapsAndMargins,
                                                                                                this->windowIndex,
                                                                                                windowViewport);
        windowContent.addTabViewport(vc);
    }
    
    std::vector<BrowserTabContent*> emptyTabVector;
    BrainOpenGLViewportContent* windowViewportContent = BrainOpenGLViewportContent::createViewportForSingleTab(emptyTabVector,
                                                                                                               NULL,
                                                                                                               gapsAndMargins,
                                                                                                               windowIndex,
                                                                                                               windowViewport);
    windowContent.setWindowViewport(windowViewportContent);
}


/**
 * Paints the graphics.
 */
void
BrainOpenGLWidget::paintGL()
{
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    if (m_contextShareGroupPointer == NULL) {
        m_contextShareGroupPointer = context()->shareGroup();
        CaretAssert(m_contextShareGroupPointer);
    }
#endif
    
    updateCursor();
    
    this->clearDrawingViewportContents();
    
    const int windowViewport[4] = {
        0,
        0,
        this->windowWidth[this->windowIndex],
        this->windowHeight[this->windowIndex]
    };
    
    getDrawingWindowContent(windowViewport,
                            m_windowContent);
    
    if (this->selectedUserInputProcessor == userInputBordersModeProcessor) {
        s_singletonOpenGL->setBorderBeingDrawn(this->borderBeingDrawn);
    }
    else {
        s_singletonOpenGL->setBorderBeingDrawn(NULL);
    }
    s_singletonOpenGL->drawModels(this->windowIndex,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  m_windowContent.getAllTabViewports());
    
    /*
     * Issue browser window redrawn event
     */
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    if (bbw != NULL) {
        EventManager::get()->sendEvent(EventBrowserWindowGraphicsRedrawn(bbw).getPointer());
    }
}

/**
 * Override of event handling.
 */
bool
BrainOpenGLWidget::event(QEvent* event)
{
    const bool toolTipsEnabled = false;
    if (toolTipsEnabled) {
        if (event->type() == QEvent::ToolTip) {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            CaretAssert(helpEvent);
            
            QPoint globalXY = helpEvent->globalPos();
            QPoint xy = helpEvent->pos();
            
            static int counter = 0;
            
            std::cout
            << "Displaying tooltip "
            << counter++
            << " at global ("
            << globalXY.x()
            << ", "
            << globalXY.y()
            << ") at pos ("
            << xy.x()
            << ", "
            << xy.y()
            << ")"
            << std::endl;
            
            QToolTip::showText(globalXY,
                               "This is the tooltip " + AString::number(counter));
            
            return true;
        }
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    return QOpenGLWidget::event(event);
#else
    return QGLWidget::event(event);
#endif
}


/**
 * Receive Content Menu events from Qt.
 * @param contextMenuEvent
 *    The context menu event.
 */
void 
BrainOpenGLWidget::contextMenuEvent(QContextMenuEvent* contextMenuEvent)
{
    const int mouseX = contextMenuEvent->x();
    const int mouseY = this->height() - contextMenuEvent->y();
    
    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                               mouseY);
    if (viewportContent != NULL) {
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              mouseX,
                              mouseY,
                              0,
                              0,
                              mouseX,
                              mouseY,
                              false);
        
        this->selectedUserInputProcessor->showContextMenu(mouseEvent,
                                                          contextMenuEvent->globalPos(),
                                                          this);
    }
}

/**
 * Receive Mouse Wheel events from Qt.  A wheel event is that same
 * as CTRL-LEFT-DRAG.  The wheel's change in value is reported as
 * change in Y.  Change in X is reported as zero.
 *
 * @param we
 *   The wheel event.
 */
void 
BrainOpenGLWidget::wheelEvent(QWheelEvent* we)
{
    const int wheelX = we->x();
    const int wheelY = this->windowHeight[this->windowIndex] - we->y();
    int delta = we->delta();
    delta = MathFunctions::limitRange(delta, -2, 2);
    
    /*
     * Use location of mouse press so that the model
     * being manipulated does not change if mouse moves
     * out of its viewport without releasing the mouse
     * button.
     */
    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(wheelX,
                                                                               wheelY);
    if (viewportContent != NULL) {
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              wheelX,
                              wheelY,
                              0,
                              delta,
                              0,
                              0,
                              this->mouseNewDraggingStartedFlag);
        this->selectedUserInputProcessor->mouseLeftDragWithCtrl(mouseEvent);
    }
    
    we->accept();
}

/*
 * A mouse event that is the middle mouse button but with no keys pressed
 * is reported as a SHIFT-LEFT-DRAG and the mouse event is changed.
 *
 * @param mouseButtons
 *     Button state when event was generated
 * @param button
 *     Button that caused the event.
 * @param keyModifiers
 *     Keys that are down, may be modified.
 * @param isMouseMoving
 *     True if mouse is moving, else false.
 */
void
BrainOpenGLWidget::checkForMiddleMouseButton(Qt::MouseButtons& mouseButtons,
                                             Qt::MouseButton& button,
                                             Qt::KeyboardModifiers& keyModifiers,
                                             const bool isMouseMoving)
{
    if (isMouseMoving) {
        if (button == Qt::NoButton) {
            if (mouseButtons == Qt::MiddleButton) {
                if (keyModifiers == Qt::NoModifier) {
                    mouseButtons = Qt::LeftButton;
                    button = Qt::NoButton;
                    keyModifiers = Qt::ShiftModifier;
                }
            }
        }
    }
    else {
        if (button == Qt::MiddleButton) {
            if (keyModifiers == Qt::NoModifier) {
                button = Qt::LeftButton;
                keyModifiers = Qt::ShiftModifier;
            }
        }
    }
}

/**
 * Receive key press events from Qt.
 * @param e
 *    The key event.
 */
void
BrainOpenGLWidget::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyModifiers = e->modifiers();
    const bool shiftKeyDownFlag = ((keyModifiers & Qt::ShiftModifier) != 0);
    
    KeyEvent keyEvent(this,
                      this->windowIndex,
                      e->key(),
                      m_newKeyPressStartedFlag,
                      shiftKeyDownFlag);
    
    this->selectedUserInputProcessor->keyPressEvent(keyEvent);
    
    e->accept();
    
    m_newKeyPressStartedFlag = false;
}

/**
 * Receive key release events from Qt.
 * @param e
 *    The key event.
 */
void
BrainOpenGLWidget::keyReleaseEvent(QKeyEvent* e)
{
    m_newKeyPressStartedFlag = true;
    
    e->accept();
}

/**
 * Receive mouse press events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mousePressEvent(QMouseEvent* me)
{
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    /*
     * When the mouse is dragged, a mouse input receiver may want to
     * know that a new dragging has started.
     */
    this->mouseNewDraggingStartedFlag = true;
    
    this->isMousePressedNearToolBox = false;
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = this->windowHeight[this->windowIndex] - me->y();

        this->mousePressX = mouseX;
        this->mousePressY = mouseY;
        
        this->lastMouseX = mouseX;
        this->lastMouseY = mouseY;

        this->mouseMovementMinimumX = mouseX;
        this->mouseMovementMaximumX = mouseX;
        this->mouseMovementMinimumY = mouseY;
        this->mouseMovementMaximumY = mouseY;
        
        /*
         * The user may intend to increase the size of a toolbox
         * but instead misses the edge of the toolbox when trying
         * to drag the toolbox and make it larger.  So, indicate
         * when the user is very close to the edge of the graphics
         * window.
         */
        const int nearToolBoxDistance = 5;
        if ((mouseX < nearToolBoxDistance) 
            || (mouseX > (this->windowWidth[this->windowIndex] - 5))
            || (mouseY < nearToolBoxDistance) 
            || (mouseY > (this->windowHeight[this->windowIndex] - 5))) {
            this->isMousePressedNearToolBox = true;
        }

        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                   mouseY);
        if (viewportContent != NULL) {
            MouseEvent mouseEvent(viewportContent,
                                  this,
                                  this->windowIndex,
                                  mouseX,
                                  mouseY,
                                  0,
                                  0,
                                  this->mousePressX,
                                  this->mousePressY,
                                  this->mouseNewDraggingStartedFlag);
            
            if (keyModifiers == Qt::NoModifier) {
                this->selectedUserInputProcessor->mouseLeftPress(mouseEvent);
            }
            else if (keyModifiers == Qt::ShiftModifier) {
                /* not implemented  this->selectedUserInputProcessor->mouseLeftPressWithShift(mouseEvent); */
            }
        }
    }
    else {
        this->mousePressX = -10000;
        this->mousePressY = -10000;
    }
    
    me->accept();
}

/**
 * Receive mouse button release events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mouseReleaseEvent(QMouseEvent* me)
{
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();

    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = this->windowHeight[this->windowIndex] - me->y();
        
        this->mouseMovementMinimumX = std::min(this->mouseMovementMinimumX, mouseX);
        this->mouseMovementMaximumX = std::max(this->mouseMovementMaximumX, mouseX);
        this->mouseMovementMinimumY = std::min(this->mouseMovementMinimumY, mouseY);
        this->mouseMovementMaximumY = std::max(this->mouseMovementMaximumY, mouseY);
        
        const int dx = this->mouseMovementMaximumX - this->mouseMovementMinimumX;
        const int dy = this->mouseMovementMaximumY - this->mouseMovementMinimumY;
        const int absDX = (dx >= 0) ? dx : -dx;
        const int absDY = (dy >= 0) ? dy : -dy;

        {
            /*
             * Mouse button RELEASE event
             */
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                       mouseY);
            if (viewportContent != NULL) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      0,
                                      0,
                                      this->mousePressX,
                                      this->mousePressY,
                                      this->mouseNewDraggingStartedFlag);
                this->selectedUserInputProcessor->mouseLeftRelease(mouseEvent);
            }
        }
        
        /*
         * Use location of mouse press so that the model
         * being manipulated does not change if mouse moves
         * out of its viewport without releasing the mouse
         * button.
         */
        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
                                                                                   this->mousePressY);
        if (viewportContent != NULL) {
            if ((absDX <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE)
                && (absDY <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE)) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      dx,
                                      dy,
                                      this->mousePressX,
                                      this->mousePressY,
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    this->selectedUserInputProcessor->mouseLeftClick(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    this->selectedUserInputProcessor->mouseLeftClickWithShift(mouseEvent);
                }
                else if (keyModifiers == (Qt::ShiftModifier
                                          | Qt::ControlModifier)) {
                    this->selectedUserInputProcessor->mouseLeftClickWithCtrlShift(mouseEvent);
                }
            }
        }
    }
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    this->isMousePressedNearToolBox = false;
    
    me->accept();
}

/**
 * Receive mouse button double click events from Qt.
 * @param me
 *    The mouse event.
 */
void
BrainOpenGLWidget::mouseDoubleClickEvent(QMouseEvent* me)
{
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    if (button == Qt::LeftButton) {
        if (keyModifiers == Qt::NoModifier) {
            const int mouseX = me->x();
            const int mouseY = this->windowHeight[this->windowIndex] - me->y();
            
            /*
             * Use location of mouse press so that the model
             * being manipulated does not change if mouse moves
             * out of its viewport without releasing the mouse
             * button.
             */
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                       mouseY);
            if (viewportContent != NULL) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      0,
                                      0,
                                      this->mousePressX,
                                      this->mousePressY,
                                      this->mouseNewDraggingStartedFlag);
                
                this->selectedUserInputProcessor->mouseLeftDoubleClick(mouseEvent);
            }
        }
    }
    
    this->isMousePressedNearToolBox = false;
    
    me->accept();
}

void
BrainOpenGLWidget::enterEvent(QEvent* /*e*/)
{
    m_mousePositionValid = true;
}

void
BrainOpenGLWidget::leaveEvent(QEvent* /*e*/)
{
    m_mousePositionValid = false;
    
    this->selectedUserInputProcessor->setMousePosition(m_mousePositionEvent,
                                                       m_mousePositionValid);
}


/**
 * Get the viewport content at the given location.
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
const BrainOpenGLViewportContent*
BrainOpenGLWidget::getViewportContentAtXY(const int x,
                                          const int y)
{
    const BrainOpenGLViewportContent* tabViewportContent = m_windowContent.getTabViewportWithLockAspectXY(x, y);
    if (tabViewportContent != NULL) {
        return tabViewportContent;
    }
    
    /*
     * If not in a tab, then use the window viewport information.
     * This allows selection of annotations in window space that are not
     * within a tab (tab may be small in height due to lock aspect).
     */
    return m_windowContent.getWindowViewport();
}

/**
 * Get all viewport content.  If tile tabs is ON, the output will contain 
 * viewport content for all tabs.  Otherwise, the output will contain viewport
 * content for only the selected tab.
 *
 * @return Viewport content.
 */
std::vector<const BrainOpenGLViewportContent*>
BrainOpenGLWidget::getViewportContent() const
{
    std::vector<const BrainOpenGLViewportContent*> contentOut = m_windowContent.getAllTabViewports();
    
    return contentOut;
}

/**
 * Perform identification on all items EXCEPT voxel editing.
 *
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be
 *    selected.
 * @return
 *    SelectionManager providing identification information.
 */
SelectionManager* 
BrainOpenGLWidget::performIdentification(const int x,
                                         const int y,
                                         const bool applySelectionBackgroundFiltering)
{
    const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);

    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(true);
    idManager->getVoxelEditingIdentification()->setEnabledForSelection(false);
    
    if (idViewport != NULL) {
        s_singletonOpenGL->selectModel(this->windowIndex,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x, 
                                  y,
                                  applySelectionBackgroundFiltering);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so 
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaint();
    this->doneCurrent();
#else
    this->repaint();
#endif
    
    return idManager;
}

/**
 * Perform identification of only annotations.  Identification of other
 * data types is off.
 *
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @return
 *    A pointer to the annotation selection item.  Its
 *    "isValid()" method may be queried to determine
 *    if the selected annotation is valid.
 */
SelectionItemAnnotation*
BrainOpenGLWidget::performIdentificationAnnotations(const int x,
                                                    const int y)
{
    const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(false);
    SelectionItemAnnotation* annotationID = idManager->getAnnotationIdentification();
    annotationID->setEnabledForSelection(true);
    
    if (idViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
         int vp[4];
         idViewport->getViewport(vp);
         const int idX = x - vp[0];
         const int idY = y - vp[1];
         */
        s_singletonOpenGL->selectModel(this->windowIndex,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x,
                                  y,
                                  true);
    }

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaint();
    this->doneCurrent();
#else
    this->repaint();
#endif
    
    return annotationID;
}

/**
 * Perform identification of only voxel editing.  Identification of other
 * data types is off.
 *
 * @param editingVolumeFile
 *    Volume file that is being edited.
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @return
 *    SelectionManager providing identification information.
 */
SelectionManager*
BrainOpenGLWidget::performIdentificationVoxelEditing(VolumeFile* editingVolumeFile,
                                                     const int x,
                                                     const int y)
{
    const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(false);
    SelectionItemVoxelEditing* idVoxelEdit = idManager->getVoxelEditingIdentification();
    idVoxelEdit->setEnabledForSelection(true);
    idVoxelEdit->setVolumeFileForEditing(editingVolumeFile);
    
    if (idViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
         int vp[4];
         idViewport->getViewport(vp);
         const int idX = x - vp[0];
         const int idY = y - vp[1];
         */
        s_singletonOpenGL->selectModel(this->windowIndex,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x,
                                  y,
                                  true);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaint();
    this->doneCurrent();
#else
    this->repaint();
#endif
    
    return idManager;
}

/**
 * Project the given item to a model.
 *
 * @param x
 *    X-coordinate for projection.
 * @param y
 *    Y-coordinate for projection.
 * @param projectionOut
 *    Projection updated for the given x and y coordinates.
 */
void
BrainOpenGLWidget::performProjection(const int x,
                                     const int y,
                                     SurfaceProjectedItem& projectionOut)
{
    const BrainOpenGLViewportContent* projectionViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing projection");
    
    if (projectionViewport != NULL) {
        s_singletonOpenGL->projectToModel(this->windowIndex,
                                     GuiManager::get()->getBrain(),
                                     m_contextShareGroupPointer,
                                     projectionViewport,
                                     x,
                                     y,
                                     projectionOut);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaint();
    this->doneCurrent();
#endif
}

/**
 * Receive mouse move events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mouseMoveEvent(QMouseEvent* me)
{    
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              true);
    
    const int mouseX = me->x();
    const int mouseY = this->windowHeight[this->windowIndex] - me->y();
    
    if (button == Qt::NoButton) {
        
        if (mouseButtons == Qt::LeftButton) {
            
            this->mouseMovementMinimumX = std::min(this->mouseMovementMinimumX, mouseX);
            this->mouseMovementMaximumX = std::max(this->mouseMovementMaximumX, mouseX);
            this->mouseMovementMinimumY = std::min(this->mouseMovementMinimumY, mouseY);
            this->mouseMovementMaximumY = std::max(this->mouseMovementMaximumY, mouseY);
            
            const int dx = mouseX - this->lastMouseX;
            const int dy = mouseY - this->lastMouseY;
            const int absDX = (dx >= 0) ? dx : -dx;
            const int absDY = (dy >= 0) ? dy : -dy;
            
            if ((absDX > 0) 
                || (absDY > 0)) { 
                /*
                 * Use location of mouse press so that the model
                 * being manipulated does not change if mouse moves
                 * out of its viewport without releasing the mouse
                 * button.
                 */
                const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
                                                                                           this->mousePressY);
                if (viewportContent != NULL) {
                    MouseEvent mouseEvent(viewportContent,
                                          this,
                                          this->windowIndex,
                                          mouseX,
                                          mouseY,
                                          dx,
                                          dy,
                                          this->mousePressX,
                                          this->mousePressY,
                                          this->mouseNewDraggingStartedFlag);
                    
                    if (keyModifiers == Qt::NoModifier) {
                        this->selectedUserInputProcessor->mouseLeftDrag(mouseEvent);
                    }
                    else if (keyModifiers == Qt::ControlModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithCtrl(mouseEvent);
                    }
                    else if (keyModifiers == Qt::ShiftModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithShift(mouseEvent);
                    }
                    else if (keyModifiers == Qt::AltModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithAlt(mouseEvent);
                    }
                    else if (keyModifiers == (Qt::ShiftModifier
                                              | Qt::ControlModifier)) {
                        this->selectedUserInputProcessor->mouseLeftDragWithCtrlShift(mouseEvent);
                    }
                    
                    this->mouseNewDraggingStartedFlag = false;
                }
            }
            
            this->lastMouseX = mouseX;
            this->lastMouseY = mouseY;
        }
        else if (mouseButtons == Qt::NoButton) {
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                       mouseY);
            if (viewportContent != NULL) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      0,
                                      0,
                                      this->mousePressX,
                                      this->mousePressY,
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    this->selectedUserInputProcessor->mouseMove(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    this->selectedUserInputProcessor->mouseMoveWithShift(mouseEvent);
                }
            }
            
        }
    }
    
    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                               mouseY);
    if (viewportContent != NULL) {
        m_mousePositionEvent.grabNew(new MouseEvent(viewportContent,
                                                    this,
                                                    this->windowIndex,
                                                    mouseX,
                                                    mouseY,
                                                    0,
                                                    0,
                                                    this->mousePressX,
                                                    this->mousePressY,
                                                    this->mouseNewDraggingStartedFlag));
        
        this->selectedUserInputProcessor->setMousePosition(m_mousePositionEvent,
                                                           m_mousePositionValid);
    }
    else {
        
    }
    
    me->accept();
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BrainOpenGLWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainResetEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainResetEvent);
        
        this->borderBeingDrawn->clear();
        
        brainResetEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        EventGraphicsUpdateAllWindows* updateAllEvent =
            dynamic_cast<EventGraphicsUpdateAllWindows*>(event);
        CaretAssert(updateAllEvent);
        
        updateAllEvent->setEventProcessed();
        
        if (updateAllEvent->isRepaint()) {
            this->repaint();
        }
        else {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            this->update();
#else
            this->updateGL();
#endif
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {
        EventGraphicsUpdateOneWindow* updateOneEvent =
        dynamic_cast<EventGraphicsUpdateOneWindow*>(event);
        CaretAssert(updateOneEvent);
        
        if (updateOneEvent->getWindowIndex() == this->windowIndex) {
            updateOneEvent->setEventProcessed();
            
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            this->update();
#else
            this->updateGL();
#endif
        }
        else {
            /*
             * If a window is yoked, update its graphics.
             */
            EventBrowserWindowDrawingContent getModelEvent(this->windowIndex);
            EventManager::get()->sendEvent(getModelEvent.getPointer());
            
            if (getModelEvent.isError()) {
                return;
            }
            
            bool needUpdate = false;
            if (needUpdate) {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
                this->update();
#else
                this->updateGL();
#endif
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE) {
        EventGetOrSetUserInputModeProcessor* inputModeEvent =
        dynamic_cast<EventGetOrSetUserInputModeProcessor*>(event);
        CaretAssert(inputModeEvent);
        
        if (inputModeEvent->getWindowIndex() == this->windowIndex) {
            if (inputModeEvent->isGetUserInputMode()) {
                inputModeEvent->setUserInputProcessor(this->selectedUserInputProcessor);
            }
            else if (inputModeEvent->isSetUserInputMode()) {
                UserInputModeAbstract* newUserInputProcessor = NULL;
                switch (inputModeEvent->getUserInputMode()) {
                    case UserInputModeAbstract::INVALID:
                        CaretAssertMessage(0, "INVALID is NOT allowed for user input mode");
                        break;
                    case UserInputModeAbstract::ANNOTATIONS:
                        newUserInputProcessor = this->userInputAnnotationsModeProcessor;
                        break;
                    case UserInputModeAbstract::BORDERS:
                        newUserInputProcessor = this->userInputBordersModeProcessor;
                        break;
                    case UserInputModeAbstract::FOCI:
                        newUserInputProcessor = this->userInputFociModeProcessor;
                        break;
                    case UserInputModeAbstract::IMAGE:
                        newUserInputProcessor = this->userInputImageModeProcessor;
                        break;
                    case UserInputModeAbstract::VOLUME_EDIT:
                        newUserInputProcessor = this->userInputVolumeEditModeProcessor;
                        break;
                    case UserInputModeAbstract::VIEW:
                        newUserInputProcessor = this->userInputViewModeProcessor;
                        break;
                }
                
                if (newUserInputProcessor != NULL) {
                    if (newUserInputProcessor != this->selectedUserInputProcessor) {
                        this->selectedUserInputProcessor->finish();
                        this->selectedUserInputProcessor = newUserInputProcessor;
                        this->selectedUserInputProcessor->initialize();
                    }
                }
            }
            inputModeEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IMAGE_CAPTURE) {
        EventImageCapture* imageCaptureEvent = dynamic_cast<EventImageCapture*>(event);
        CaretAssert(imageCaptureEvent);
        
        if (imageCaptureEvent->getBrowserWindowIndex() == this->windowIndex) {
            captureImage(imageCaptureEvent);
            imageCaptureEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_REQUEST) {
        EventIdentificationRequest* idRequestEvent = dynamic_cast<EventIdentificationRequest*>(event);
        CaretAssert(idRequestEvent);
        
        if (idRequestEvent->getWindowIndex() == this->windowIndex) {
            SelectionManager* sm = performIdentification(idRequestEvent->getWindowX(),
                                                         idRequestEvent->getWindowY(),
                                                         false);
            idRequestEvent->setSelectionManager(sm);
            idRequestEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* guiUpdateEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(guiUpdateEvent);
        guiUpdateEvent->setEventProcessed();
        
        this->selectedUserInputProcessor->update();
    }
    else {
        
    }
}

/**
 * Capture an image using the parameters from the event.
 *
 * @param imageCaptureEvent
 *    The image capture event.
 */
void
BrainOpenGLWidget::captureImage(EventImageCapture* imageCaptureEvent)
{
    const int oldSizeX = this->windowWidth[this->windowIndex];
    const int oldSizeY = this->windowHeight[this->windowIndex];

    /*
     * Note that a size of zero indicates capture graphics in its
     * current size.
     */
    int captureOffsetX    = imageCaptureEvent->getCaptureOffsetX();
    int captureOffsetY    = imageCaptureEvent->getCaptureOffsetY();
    int captureWidth      = imageCaptureEvent->getCaptureWidth();
    int captureHeight     = imageCaptureEvent->getCaptureHeight();
    int outputImageWidth  = imageCaptureEvent->getOutputWidth();
    int outputImageHeight = imageCaptureEvent->getOutputHeight();
    
    if ((outputImageWidth <= 0)
        || (outputImageHeight <= 0)) {
        BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
        
        if (browserWindow != NULL) {
            int32_t widgetWidth(0), widgetHeight(0);
            browserWindow->getGraphicsWidgetSize(captureOffsetX,
                                                 captureOffsetY,
                                                 captureWidth,
                                                 captureHeight,
                                                 widgetWidth,
                                                 widgetHeight,
                                                 true); /* true => apply lock aspect ratio */
            outputImageWidth  = captureWidth;
            outputImageHeight = captureHeight;
        }
    }
    
    /*
     * Disable tab highlighting when capturing images
     */
    BrainOpenGL::setAllowTabHighlighting(false);
    
    /*
     * Force immediate mode since problems with display lists
     * in image capture.
     */
    BrainOpenGLShape::setImmediateModeOverride(true);
    
    QImage image;
    
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const ImageCaptureMethodEnum::Enum imageCaptureMethod = prefs->getImageCaptureMethod();
    
    switch (imageCaptureMethod) {
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_GRAB_FRAME_BUFFER:
        {
            /*
             * Grab frame buffer seems to have a bug in that it grabs
             * the previous buffer on Mac so repaint to ensure frame
             * buffer is updated.  (repaint() updates immediately,
             * update() is a scheduled update).
             */
            repaint();
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            image = grabFramebuffer();
#else
            image = grabFrameBuffer();
#endif
            if ((captureWidth > 0)
                && (captureHeight > 0)) {
                image = image.copy(captureOffsetX,
                                   captureOffsetY,
                                   captureWidth,
                                   captureHeight);
            }
            
            if ((outputImageWidth != image.width())
                || (outputImageHeight != image.height())) {
                image = image.scaled(outputImageWidth,
                                     outputImageHeight,
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
            }
            
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP:
        {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            WuQMessageBox::errorOk(this, "Render PixMap not implemented with Qt5");
#else
            /*
             * When the user chooses to exclude regions
             * caused by locking of tab/window aspect ratio,
             * the pixmap is rendered to the output width and
             * height and in the correct aspect ratio so when
             * the rendering takes place, there is no empty 
             * region caused by aspect locking that needs to 
             * be excluded.
             */
            
            QPixmap pixmap = this->renderPixmap(outputImageWidth,
                                                outputImageHeight,
                                                true);
            image = pixmap.toImage();
#endif
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_OFFSCREEN_FRAME_BUFFER:
        {
            image = performOffScreenImageCapture(outputImageWidth,
                                                 outputImageHeight);
        }
            break;
    }
    
    if ((image.size().width() <= 0)
        || (image.size().height() <= 0)) {
        imageCaptureEvent->setErrorMessage("Image capture appears to have failed (invalid size).");
    }
    else {
        imageCaptureEvent->setImage(image);
        
        uint8_t backgroundColor[3];
        s_singletonOpenGL->getBackgroundColor(backgroundColor);
        imageCaptureEvent->setBackgroundColor(backgroundColor);
    }
    
    BrainOpenGLShape::setImmediateModeOverride(false);
    BrainOpenGL::setAllowTabHighlighting(true);
    
    this->resizeGL(oldSizeX, oldSizeY);
}


/**
 * Initialize the OpenGL format.  This must be called
 * prior to initializing an instance of this class so
 * that the OpenGL is setup properly.
 */
void
BrainOpenGLWidget::initializeDefaultGLFormat()
{
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    QSurfaceFormat glfmt;
    glfmt.setAlphaBufferSize(8);
    glfmt.setBlueBufferSize(8);
    glfmt.setDepthBufferSize(24);
    glfmt.setGreenBufferSize(8);
    glfmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    glfmt.setRedBufferSize(8);
    glfmt.setRenderableType(QSurfaceFormat::OpenGL);
    glfmt.setSamples(6);
    glfmt.setStereo(false);
    glfmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    
    glfmt.setMajorVersion(2);
    glfmt.setMinorVersion(1);
    QSurfaceFormat::setDefaultFormat(glfmt);
#else
    QGLFormat glfmt;
    glfmt.setAccum(false);
    glfmt.setAlpha(true);
    glfmt.setAlphaBufferSize(8);
    glfmt.setDepth(true);
    glfmt.setDepthBufferSize(24);
    glfmt.setDirectRendering(true);
    glfmt.setDoubleBuffer(true);
    glfmt.setOverlay(false);
    glfmt.setProfile(QGLFormat::CompatibilityProfile);
    glfmt.setVersion(2, 1);
    glfmt.setSampleBuffers(true);
    glfmt.setStencil(false);
    glfmt.setStereo(false);
    
    glfmt.setRgba(true);
    glfmt.setRedBufferSize(8);
    glfmt.setGreenBufferSize(8);
    glfmt.setBlueBufferSize(8);
    QGLFormat::setDefaultFormat(glfmt);
#endif
    
    s_defaultGLFormatInitialized = true;
}
