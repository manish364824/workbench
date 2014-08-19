
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

#include <limits>

#include <QBoxLayout>
#include <QColorDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTabWidget>

#define __PREFERENCES_DIALOG__H__DECLARE__
#include "PreferencesDialog.h"
#undef __PREFERENCES_DIALOG__H__DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ImageCaptureMethodEnum.h"
#include "OpenGLDrawingMethodEnum.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::PreferencesDialog 
 * \brief Dialog for display/editing of prefernces.
 * \ingroup GuiQt
 *
 * Presents controls for editing palettes used to color
 * scalar data.
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
PreferencesDialog::PreferencesDialog(QWidget* parent)
: WuQDialogNonModal("Preferences",
                    parent)
{
    setDeleteWhenClosed(false);

    /*
     * No apply button
     */
    setApplyButtonText("");    
    
    /*
     * Used to block signals in all widgets
     */
    m_allWidgets = new WuQWidgetObjectGroup(this);
    
    /*
     * Create the tab widget and all tab content
     */
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createColorsWidget(),
                      "Colors");
    tabWidget->addTab(createMiscellaneousWidget(),
                      "Misc");
    tabWidget->addTab(createOpenGLWidget(),
                      "OpenGL");
    tabWidget->addTab(createVolumeWidget(),
                      "Volume");
    setCentralWidget(tabWidget,
                           WuQDialog::SCROLL_AREA_NEVER);
    
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
PreferencesDialog::~PreferencesDialog()
{
    
}

/**
 * Add a color button and swatch.
 *
 * @param gridLayout
 *     Grid layout for widgets.
 * @param prefColor
 *     Enumerated value for color.
 * @param colorSignalMapper
 *     Signal mapper for buttons.
 */
void
PreferencesDialog::addColorButtonAndSwatch(QGridLayout* gridLayout,
                                           const PREF_COLOR prefColor,
                                           QSignalMapper* colorSignalMapper)
{
    QString buttonText;
    QWidget* colorSwatchWidget = new QWidget();
    
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            buttonText = "All Background";
            m_backgroundColorAllWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            buttonText = "Chart Background";
            m_backgroundColorChartWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            buttonText = "Surface Background";
            m_backgroundColorSurfaceWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            buttonText = "Volume Background";
            m_backgroundColorVolumeWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            buttonText = "All Foreground";
            m_foregroundColorAllWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            buttonText = "Chart Foreground";
            m_foregroundColorChartWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            buttonText = "Surface Foreground";
            m_foregroundColorSurfaceWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            buttonText = "Volume Foreground";
            m_foregroundColorVolumeWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            buttonText = "Chart Grid Lines";
            m_chartMatrixGridLinesColorWidget = colorSwatchWidget;
            break;
        case NUMBER_OF_PREF_COLORS:
            CaretAssert(0);
            break;
    }
    
    buttonText.append("...");
    
    CaretAssert( ! buttonText.isEmpty());
    
    QPushButton* colorPushButton = new QPushButton(buttonText);
    QObject::connect(colorPushButton, SIGNAL(clicked()),
                     colorSignalMapper, SLOT(map()));
    colorSignalMapper->setMapping(colorPushButton,
                                  (int)prefColor);
    
    addWidgetsToLayout(gridLayout,
                       colorPushButton,
                       colorSwatchWidget);
}


/**
 * @return The colors widget.
 */
QWidget*
PreferencesDialog::createColorsWidget()
{
    QSignalMapper* colorSignalMapper = new QSignalMapper(this);
    
    QGridLayout* gridLayout = new QGridLayout();
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_ALL,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_ALL,
                            colorSignalMapper);
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_CHART,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_CHART,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_CHART_MATRIX_GRID_LINES,
                            colorSignalMapper);
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_SURFACE,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_SURFACE,
                            colorSignalMapper);
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_VOLUME,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_VOLUME,
                            colorSignalMapper);
    
    
    
    QObject::connect(colorSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(colorPushButtonClicked(int)));
    m_allWidgets->add(colorSignalMapper);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the color widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateColorWidget(CaretPreferences* prefs)
{
    for (int32_t i = 0; i < NUMBER_OF_PREF_COLORS; i++) {
        const PREF_COLOR prefColor = (PREF_COLOR)i;
        
        uint8_t rgb[3] = { 0, 0, 0 };
        QWidget* colorSwatchWidget = NULL;
        
        switch (prefColor) {
            case PREF_COLOR_BACKGROUND_ALL:
                prefs->getColorBackgroundAllView(rgb);
                colorSwatchWidget = m_backgroundColorAllWidget;
                break;
            case PREF_COLOR_BACKGROUND_CHART:
                prefs->getColorBackgroundChartView(rgb);
                colorSwatchWidget = m_backgroundColorChartWidget;
                break;
            case PREF_COLOR_BACKGROUND_SURFACE:
                prefs->getColorBackgroundSurfaceView(rgb);
                colorSwatchWidget = m_backgroundColorSurfaceWidget;
                break;
            case PREF_COLOR_BACKGROUND_VOLUME:
                prefs->getColorBackgroundVolumeView(rgb);
                colorSwatchWidget = m_backgroundColorVolumeWidget;
                break;
            case PREF_COLOR_FOREGROUND_ALL:
                prefs->getColorForegroundAllView(rgb);
                colorSwatchWidget = m_foregroundColorAllWidget;
                break;
            case PREF_COLOR_FOREGROUND_CHART:
                prefs->getColorForegroundChartView(rgb);
                colorSwatchWidget = m_foregroundColorChartWidget;
                break;
            case PREF_COLOR_FOREGROUND_SURFACE:
                prefs->getColorForegroundSurfaceView(rgb);
                colorSwatchWidget = m_foregroundColorSurfaceWidget;
                break;
            case PREF_COLOR_FOREGROUND_VOLUME:
                prefs->getColorForegroundVolumeView(rgb);
                colorSwatchWidget = m_foregroundColorVolumeWidget;
                break;
            case PREF_COLOR_CHART_MATRIX_GRID_LINES:
                prefs->getColorChartMatrixGridLines(rgb);
                colorSwatchWidget = m_chartMatrixGridLinesColorWidget;
                break;
            case NUMBER_OF_PREF_COLORS:
                CaretAssert(0);
                break;
        }

        CaretAssert(colorSwatchWidget);
        
        colorSwatchWidget->setStyleSheet("background-color: rgb("
                                         + AString::number(rgb[0])
                                         + ", " + AString::number(rgb[1])
                                         + ", " + AString::number(rgb[2])
                                         + ");");
    }
}

/**
 * @return The miscellaneous widget.
 */
QWidget*
PreferencesDialog::createMiscellaneousWidget()
{
    /*
     * Logging Level
     */
    m_miscLoggingLevelComboBox = new QComboBox();
    std::vector<LogLevelEnum::Enum> loggingLevels;
    LogLevelEnum::getAllEnums(loggingLevels);
    const int32_t numLogLevels = static_cast<int32_t>(loggingLevels.size());
    for (int32_t i = 0; i < numLogLevels; i++) {
        const LogLevelEnum::Enum logLevel = loggingLevels[i];
        m_miscLoggingLevelComboBox->addItem(LogLevelEnum::toGuiName(logLevel));
        m_miscLoggingLevelComboBox->setItemData(i, LogLevelEnum::toIntegerCode(logLevel));
    }
    QObject::connect(m_miscLoggingLevelComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(miscLoggingLevelComboBoxChanged(int)));
    
    m_allWidgets->add(m_miscLoggingLevelComboBox);
    
    /*
     * Splash Screen
     */
    m_miscSplashScreenShowAtStartupComboBox = new WuQTrueFalseComboBox("On",
                                                                       "Off",
                                                                       this);
    QObject::connect(m_miscSplashScreenShowAtStartupComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(miscSplashScreenShowAtStartupComboBoxChanged(bool)));
    m_allWidgets->add(m_miscSplashScreenShowAtStartupComboBox);
    
    /*
     * Yoking
     */
    m_yokingDefaultComboBox = new WuQTrueFalseComboBox("On",
                                                       "Off",
                                                       this);
    QObject::connect(m_yokingDefaultComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(yokingComboBoxToggled(bool)));
    m_allWidgets->add(m_yokingDefaultComboBox);
    
    /*
     * Developer Menu
     */
    m_miscDevelopMenuEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                                "Off",
                                                                this);
    QObject::connect(m_miscDevelopMenuEnabledComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(miscDevelopMenuEnabledComboBoxChanged(bool)));
    m_allWidgets->add(m_miscDevelopMenuEnabledComboBox);
    
    /*
     * Manage Files View Files Type
     */
    m_miscSpecFileDialogViewFilesTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_miscSpecFileDialogViewFilesTypeEnumComboBox->setup<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>();
    QObject::connect(m_miscSpecFileDialogViewFilesTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(miscSpecFileDialogViewFilesTypeEnumComboBoxItemActivated()));
    m_allWidgets->add(m_miscSpecFileDialogViewFilesTypeEnumComboBox->getWidget());
    
    QGridLayout* gridLayout = new QGridLayout();
    addWidgetToLayout(gridLayout,
                      "Logging Level: ",
                      m_miscLoggingLevelComboBox);
    addWidgetToLayout(gridLayout,
                      "Save/Manage View Files: ",
                      m_miscSpecFileDialogViewFilesTypeEnumComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "New Tabs Yoked to Group A: ",
                      m_yokingDefaultComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Develop Menu in Menu Bar: ",
                      m_miscDevelopMenuEnabledComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Splash Screen at Startup: ",
                      m_miscSplashScreenShowAtStartupComboBox->getWidget());
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the miscellaneous widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateMiscellaneousWidget(CaretPreferences* prefs)
{
    const LogLevelEnum::Enum loggingLevel = prefs->getLoggingLevel();
    int indx = m_miscLoggingLevelComboBox->findData(LogLevelEnum::toIntegerCode(loggingLevel));
    if (indx >= 0) {
        m_miscLoggingLevelComboBox->setCurrentIndex(indx);
    }
    
    m_miscDevelopMenuEnabledComboBox->setStatus(prefs->isDevelopMenuEnabled());
    
    m_miscSplashScreenShowAtStartupComboBox->setStatus(prefs->isSplashScreenEnabled());
    
    m_yokingDefaultComboBox->setStatus(prefs->isYokingDefaultedOn());
    
    m_miscSpecFileDialogViewFilesTypeEnumComboBox->setSelectedItem<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>(prefs->getManageFilesViewFileType());

}

/**
 * @return The OpenGL widget.
 */
QWidget*
PreferencesDialog::createOpenGLWidget()
{
    /*
     * Image Capture Method
     */
    m_openGLImageCaptureMethodEnumComboBox = new EnumComboBoxTemplate(this);
    m_openGLImageCaptureMethodEnumComboBox->setup<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>();
    QObject::connect(m_openGLImageCaptureMethodEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(openGLImageCaptureMethodEnumComboBoxItemActivated()));
    const AString captureMethodToolTip = ("Sometimes, the default image capture method fails to "
                                          "function correctly and the captured image does not match "
                                          "the content of the graphics window.  If this occurs, "
                                          "try changing the Capture Method to Grab Frame Buffer.");
    WuQtUtilities::setWordWrappedToolTip(m_openGLImageCaptureMethodEnumComboBox->getComboBox(),
                                         captureMethodToolTip);
    m_allWidgets->add(m_openGLImageCaptureMethodEnumComboBox->getWidget());
    
    /*
     * OpenGL Drawing Method
     */
    m_openGLDrawingMethodEnumComboBox = new EnumComboBoxTemplate(this);
    m_openGLDrawingMethodEnumComboBox->setup<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>();
    QObject::connect(m_openGLDrawingMethodEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(openGLDrawingMethodEnumComboBoxItemActivated()));
    m_allWidgets->add(m_openGLDrawingMethodEnumComboBox->getWidget());
    
    
    QGridLayout* gridLayout = new QGridLayout();
    addWidgetToLayout(gridLayout,
                      "Image Capture Method: ",
                      m_openGLImageCaptureMethodEnumComboBox->getWidget());
    QLabel* vertexBuffersLabel = addWidgetToLayout(gridLayout,
                                                         "OpenGL Vertex Buffers: ",
                                                         m_openGLDrawingMethodEnumComboBox->getWidget());
    
    /*
     * HIDE THE VERTEX BUFFERS OPTION
     */
    vertexBuffersLabel->setHidden(true);
    m_openGLDrawingMethodEnumComboBox->getWidget()->setHidden(true);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the OpenGL widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateOpenGLWidget(CaretPreferences* prefs)
{
    const ImageCaptureMethodEnum::Enum captureMethod = prefs->getImageCaptureMethod();
    m_openGLImageCaptureMethodEnumComboBox->setSelectedItem<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>(captureMethod);
    
    const OpenGLDrawingMethodEnum::Enum drawingMethod = prefs->getOpenDrawingMethod();
    m_openGLDrawingMethodEnumComboBox->setSelectedItem<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>(drawingMethod);
}

/**
 * @return The volume widget.
 */
QWidget*
PreferencesDialog::createVolumeWidget()
{
    /*
     * Crosshairs On/Off
     */
    m_volumeAxesCrosshairsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesCrosshairsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesCrosshairsComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesCrosshairsComboBox);
    
    /*
     * Axes Labels On/Off
     */
    m_volumeAxesLabelsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesLabelsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesLabelsComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesLabelsComboBox);
    
    /*
     * Identification On/Off
     */
    m_volumeIdentificationComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeIdentificationComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeIdentificationComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeIdentificationComboBox);

    /*
     * Montage Coordinates On/Off
     */
    m_volumeAxesMontageCoordinatesComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesMontageCoordinatesComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesMontageCoordinatesComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesMontageCoordinatesComboBox);
    
    /*
     * Montage Slice Gap
     */
    m_volumeMontageGapSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(0,
                                                                                  100000,
                                                                                  1,
                                                                                  this,
                                                                                  SLOT(volumeMontageGapValueChanged(int)));
    m_allWidgets->add(m_volumeMontageGapSpinBox);
    
    /*
     * Montage Slice Coordinate Precision
     */
    m_volumeMontageCoordinatePrecisionSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(0,
                                                                                                  5,
                                                                                                  1,
                                                                                                  this,
                                                                                                  SLOT(volumeMontageCoordinatePrecisionChanged(int)));
    m_allWidgets->add(m_volumeMontageCoordinatePrecisionSpinBox);
    
    m_allWidgets->add(m_volumeAxesCrosshairsComboBox);
    m_allWidgets->add(m_volumeAxesLabelsComboBox);
    m_allWidgets->add(m_volumeAxesMontageCoordinatesComboBox);
    m_allWidgets->add(m_volumeMontageGapSpinBox);
    m_allWidgets->add(m_volumeMontageCoordinatePrecisionSpinBox);
    
    QGridLayout* gridLayout = new QGridLayout();
    
    addWidgetToLayout(gridLayout,
                      "Volume Axes Crosshairs: ",
                      m_volumeAxesCrosshairsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Axes Labels: ",
                      m_volumeAxesLabelsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Identification For New Tabs: ",
                      m_volumeIdentificationComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Montage Slice Coord: ",
                      m_volumeAxesMontageCoordinatesComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Montage Gap: ",
                      m_volumeMontageGapSpinBox);
    addWidgetToLayout(gridLayout,
                      "Volume Montage Precision: ",
                      m_volumeMontageCoordinatePrecisionSpinBox);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the Volume widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateVolumeWidget(CaretPreferences* prefs)
{
    m_volumeAxesCrosshairsComboBox->setStatus(prefs->isVolumeAxesCrosshairsDisplayed());
    m_volumeAxesLabelsComboBox->setStatus(prefs->isVolumeAxesLabelsDisplayed());
    m_volumeAxesMontageCoordinatesComboBox->setStatus(prefs->isVolumeMontageAxesCoordinatesDisplayed());
    m_volumeIdentificationComboBox->setStatus(prefs->isVolumeIdentificationDefaultedOn());
    m_volumeMontageGapSpinBox->setValue(prefs->getVolumeMontageGap());
    m_volumeMontageCoordinatePrecisionSpinBox->setValue(prefs->getVolumeMontageCoordinatePrecision());
}

/**
 * Add a label in the left column and the widget in the right column.
 *
 * @param gridLayout
 *    The grid layout to which the widgets are added.
 * @param labelText
 *    Text for label.
 * @param widget
 *    Widget for right column.
 * @return
 *    The label that corresponds to the widget.
 */
QLabel*
PreferencesDialog::addWidgetToLayout(QGridLayout* gridLayout,
                                     const QString& labelText,
                                     QWidget* widget)
{
    QLabel* label = new QLabel(labelText);
    label->setAlignment(Qt::AlignRight);
    addWidgetsToLayout(gridLayout,
                             label,
                             widget);
    
    return label;
}

/**
 * Add widgets to the layout.  If rightWidget is NULL,
 * leftItem spans both columns.
 *
 * @param leftWidget
 *    Widget for left column.
 * @param rightWidget
 *    Widget for right column.
 */
void 
PreferencesDialog::addWidgetsToLayout(QGridLayout* gridLayout,
                                      QWidget* leftWidget,
                                      QWidget* rightWidget)
{
    int row = gridLayout->rowCount();
    if (rightWidget != NULL) {
        gridLayout->addWidget(leftWidget, row, 0);
        gridLayout->addWidget(rightWidget, row, 1);
    }
    else {
        gridLayout->addWidget(leftWidget, row, 0, 1, 2, Qt::AlignLeft);
    }
}

/**
 * May be called to update the dialog's content.
 */
void 
PreferencesDialog::updateDialog()
{
    m_allWidgets->blockAllSignals(true);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    updateColorWidget(prefs);
    updateMiscellaneousWidget(prefs);
    updateOpenGLWidget(prefs);
    updateVolumeWidget(prefs);
    
    m_allWidgets->blockAllSignals(false);
}

void
PreferencesDialog::updateColorWithDialog(const PREF_COLOR prefColor)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    uint8_t rgb[3];
    AString prefColorName;
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            prefs->getColorBackgroundAllView(rgb);
            prefColorName = "Background - All";
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            prefs->getColorBackgroundChartView(rgb);
            prefColorName = "Background - Chart";
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            prefs->getColorBackgroundSurfaceView(rgb);
            prefColorName = "Background - Surface";
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            prefs->getColorBackgroundVolumeView(rgb);
            prefColorName = "Background - Volume";
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            prefs->getColorForegroundAllView(rgb);
            prefColorName = "Foreground - All";
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            prefs->getColorForegroundChartView(rgb);
            prefColorName = "Foreground - Chart";
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            prefs->getColorForegroundSurfaceView(rgb);
            prefColorName = "Foreground - Surface";
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            prefs->getColorForegroundVolumeView(rgb);
            prefColorName = "Foreground - Volume";
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            prefs->getColorChartMatrixGridLines(rgb);
            prefColorName = "Chart Matrix Grid Lines";
            break;
        case NUMBER_OF_PREF_COLORS:
            CaretAssert(0);
            break;
    }
    
    const QColor initialColor(rgb[0],
                              rgb[1],
                              rgb[2]);
    
    QColorDialog colorDialog(this);
    colorDialog.setCurrentColor(initialColor);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog);
    colorDialog.setWindowTitle(prefColorName);
    
    if (colorDialog.exec() == QColorDialog::Accepted) {
        const QColor newColor = colorDialog.currentColor();
        rgb[0] = newColor.red();
        rgb[1] = newColor.green();
        rgb[2] = newColor.blue();
        
        switch (prefColor) {
            case PREF_COLOR_BACKGROUND_ALL:
                prefs->setColorBackgroundAllView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_CHART:
                prefs->setColorBackgroundChartView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_SURFACE:
                prefs->setColorBackgroundSurfaceView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_VOLUME:
                prefs->setColorBackgroundVolumeView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_ALL:
                prefs->setColorForegroundAllView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_CHART:
                prefs->setColorForegroundChartView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_SURFACE:
                prefs->setColorForegroundSurfaceView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_VOLUME:
                prefs->setColorForegroundVolumeView(rgb);
                break;
            case PREF_COLOR_CHART_MATRIX_GRID_LINES:
                prefs->setColorChartMatrixGridLines(rgb);
                break;
            case NUMBER_OF_PREF_COLORS:
                CaretAssert(0);
                break;
        }
        
        updateColorWidget(prefs);
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when a color button is clicked.
 *
 * @param enumIndex
 *     color enum integer value indicating button that was clicked.
 */
void
PreferencesDialog::colorPushButtonClicked(int enumIndex)
{
    const PREF_COLOR prefColor = (PREF_COLOR)enumIndex;
    updateColorWithDialog(prefColor);
}

/**
 * Called when the logging level is changed.
 * @param int indx
 *   New index of logging level combo box.
 */
void 
PreferencesDialog::miscLoggingLevelComboBoxChanged(int indx)
{    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const int32_t logLevelIntegerCode = m_miscLoggingLevelComboBox->itemData(indx).toInt();
    prefs->setLoggingLevel(LogLevelEnum::fromIntegerCode(logLevelIntegerCode, NULL));
}

/**
 * Called when the apply button is pressed.
 */
void PreferencesDialog::applyButtonClicked()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the OpenGL drawing method is changed.
 */
void
PreferencesDialog::openGLDrawingMethodEnumComboBoxItemActivated()
{
    const OpenGLDrawingMethodEnum::Enum drawingMethod = m_openGLDrawingMethodEnumComboBox->getSelectedItem<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setOpenGLDrawingMethod(drawingMethod);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the image capture method is changed.
 */
void
PreferencesDialog::openGLImageCaptureMethodEnumComboBoxItemActivated()
{
    const ImageCaptureMethodEnum::Enum imageCaptureMethod = m_openGLImageCaptureMethodEnumComboBox->getSelectedItem<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setImageCaptureMethod(imageCaptureMethod);
}

/**
 * Called when volume crosshairs is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::volumeAxesCrosshairsComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAxesCrosshairsDisplayed(value);    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when volume labels is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::volumeAxesLabelsComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAxesLabelsDisplayed(value);    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when volume labels is toggled.
 * @param value
 *    New value.
 */
void
PreferencesDialog::volumeAxesMontageCoordinatesComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageAxesCoordinatesDisplayed(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when volume montage gap value is changed.
 */
void
PreferencesDialog::volumeMontageGapValueChanged(int value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageGap(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when volume montage coordinate precision value is changed.
 */
void
PreferencesDialog::volumeMontageCoordinatePrecisionChanged(int value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageCoordinatePrecision(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when volume identification value is changed.
 */
void
PreferencesDialog::volumeIdentificationComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeIdentificationDefaultedOn(value);
}

/**
 * Called when yoking default value is changed.
 */
void
PreferencesDialog::yokingComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setYokingDefaultedOn(value);
}

/**
 * Called when show splash screen option changed.
 * @param value
 *   New value.
 */
void PreferencesDialog::miscSplashScreenShowAtStartupComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setSplashScreenEnabled(value);
}

/**
 * Called when show develop menu option changed.
 * @param value
 *   New value.
 */
void
PreferencesDialog::miscDevelopMenuEnabledComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setDevelopMenuEnabled(value);
    
    const AString msg = ("The Develop menu will "
                         + QString((value ? "appear" : " not appear"))
                         + " in newly opened windows.");
    WuQMessageBox::informationOk(m_miscDevelopMenuEnabledComboBox->getWidget(),
                                 msg);
}

/**
 * Gets called when view files type is changed.
 */
void
PreferencesDialog::miscSpecFileDialogViewFilesTypeEnumComboBoxItemActivated()
{
    const SpecFileDialogViewFilesTypeEnum::Enum viewFilesType = m_miscSpecFileDialogViewFilesTypeEnumComboBox->getSelectedItem<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setManageFilesViewFileType(viewFilesType);
}


