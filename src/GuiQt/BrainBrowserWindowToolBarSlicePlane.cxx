
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__
#include "BrainBrowserWindowToolBarSlicePlane.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QToolButton>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarSlicePlane 
 * \brief Toolbar component for volume slice plane and projection selection.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarSlicePlane::BrainBrowserWindowToolBarSlicePlane(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QIcon parasagittalIcon;
    const bool parasagittalIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-parasagittal.png",
                            parasagittalIcon);
    
    QIcon coronalIcon;
    const bool coronalIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-coronal.png",
                            coronalIcon);
    
    QIcon axialIcon;
    const bool axialIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-axial.png",
                            axialIcon);
    
    m_volumePlaneParasagittalToolButtonAction =
    WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::PARASAGITTAL),
                                "View the PARASAGITTAL slice",
                                this);
    m_volumePlaneParasagittalToolButtonAction->setCheckable(true);
    if (parasagittalIconValid) {
        m_volumePlaneParasagittalToolButtonAction->setIcon(parasagittalIcon);
    }
    
    m_volumePlaneCoronalToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::CORONAL),
                                                                           "View the CORONAL slice",
                                                                           this);
    m_volumePlaneCoronalToolButtonAction->setCheckable(true);
    if (coronalIconValid) {
        m_volumePlaneCoronalToolButtonAction->setIcon(coronalIcon);
    }
    
    m_volumePlaneAxialToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::AXIAL),
                                                                         "View the AXIAL slice",
                                                                         this);
    m_volumePlaneAxialToolButtonAction->setCheckable(true);
    if (axialIconValid) {
        m_volumePlaneAxialToolButtonAction->setIcon(axialIcon);
    }
    
    m_volumePlaneAllToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::ALL),
                                                                       "View the PARASAGITTAL, CORONAL, and AXIAL slices",
                                                                       this);
    m_volumePlaneAllToolButtonAction->setCheckable(true);
    
    
    m_volumePlaneActionGroup = new QActionGroup(this);
    m_volumePlaneActionGroup->addAction(m_volumePlaneParasagittalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneCoronalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAxialToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAllToolButtonAction);
    m_volumePlaneActionGroup->setExclusive(true);
    QObject::connect(m_volumePlaneActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneActionGroupTriggered(QAction*)));
    
    
    m_volumePlaneResetToolButtonAction = WuQtUtilities::createAction("Reset",
                                                                         "Reset to remove panning, zooming, and/or oblique rotation",
                                                                         this,
                                                                         this,
                                                                         SLOT(volumePlaneResetToolButtonTriggered(bool)));
    
    
    QToolButton* volumePlaneParasagittalToolButton = new QToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(m_volumePlaneParasagittalToolButtonAction);
    
    QToolButton* volumePlaneCoronalToolButton = new QToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(m_volumePlaneCoronalToolButtonAction);
    
    QToolButton* volumePlaneAxialToolButton = new QToolButton();
    volumePlaneAxialToolButton->setDefaultAction(m_volumePlaneAxialToolButtonAction);
    
    QToolButton* volumePlaneAllToolButton = new QToolButton();
    volumePlaneAllToolButton->setDefaultAction(m_volumePlaneAllToolButtonAction);
    
    QToolButton* volumePlaneResetToolButton = new QToolButton();
    volumePlaneResetToolButton->setDefaultAction(m_volumePlaneResetToolButtonAction);
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton,
                                      volumePlaneAllToolButton);
    WuQtUtilities::matchWidgetWidths(volumePlaneParasagittalToolButton,
                                     volumePlaneCoronalToolButton,
                                     volumePlaneAxialToolButton,
                                     volumePlaneAllToolButton);
    
    QToolButton* slicePlaneCustomToolButton = new QToolButton();
    slicePlaneCustomToolButton->setDefaultAction(m_parentToolBar->customViewAction);
    slicePlaneCustomToolButton->setSizePolicy(QSizePolicy::Minimum,
                                              QSizePolicy::Fixed);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(volumePlaneParasagittalToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneCoronalToolButton,      rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumePlaneAxialToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneAllToolButton,   rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumePlaneResetToolButton, rowIndex, 0, 1, 2, Qt::AlignHCenter);
    rowIndex++;
    gridLayout->addWidget(slicePlaneCustomToolButton, rowIndex, 0, 1, 2, Qt::AlignHCenter);
    
    m_volumePlaneWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumePlaneWidgetGroup->add(m_volumePlaneActionGroup);
    m_volumePlaneWidgetGroup->add(m_volumePlaneResetToolButtonAction);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSlicePlane::~BrainBrowserWindowToolBarSlicePlane()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSlicePlane::updateContent(BrowserTabContent* browserTabContent)
{
    m_volumePlaneWidgetGroup->blockAllSignals(true);
    
    switch (browserTabContent->getSliceViewPlane()) {
        case VolumeSliceViewPlaneEnum::ALL:
            m_volumePlaneAllToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_volumePlaneAxialToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_volumePlaneCoronalToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_volumePlaneParasagittalToolButtonAction->setChecked(true);
            break;
    }
    
    m_volumePlaneWidgetGroup->blockAllSignals(false);
}

/**
 * Called when volume slice plane button is clicked.
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneActionGroupTriggered(QAction* action)
{
    VolumeSliceViewPlaneEnum::Enum plane = VolumeSliceViewPlaneEnum::AXIAL;
    
    if (action == m_volumePlaneAllToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::ALL;
    }
    else if (action == m_volumePlaneAxialToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::AXIAL;
        
    }
    else if (action == m_volumePlaneCoronalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::CORONAL;
        
    }
    else if (action == m_volumePlaneParasagittalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::PARASAGITTAL;
    }
    else {
        CaretLogSevere("Invalid volume plane action: " + action->text());
    }
    
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    
    btc->setSliceViewPlane(plane);
    
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindow();
    updateOtherYokedWindows();
}

/**
 * Called when volume reset slice view button is pressed.
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->resetView();
    
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindow();
    updateOtherYokedWindows();
}

