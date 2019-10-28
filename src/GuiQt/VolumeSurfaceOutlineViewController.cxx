
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

#include <stdint.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__

#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "VolumeSurfaceOutlineColorOrTabViewController.h"
#include "VolumeSurfaceOutlineModel.h"
#include "WuQDoubleSpinBox.h"
#include "WuQFactory.h"
#include "WuQGridLayoutGroup.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineViewController 
 * \brief View controller for volume surface outline
 *
 */
/**
 * Constructor.
 *
 * @param orientation
 *     Orientation for controller
 * @param gridLayout
 *     Layout for widgets
 * @param objectNamePrefix
 *     Object name prefix for macros
 * @param descriptivePrefix
 *     Descriptive name prefix for macros
 */
VolumeSurfaceOutlineViewController::VolumeSurfaceOutlineViewController(const Qt::Orientation orientation,
                                                                       QGridLayout* gridLayout,
                                                                       const QString& objectNamePrefix,
                                                                       const QString& descriptivePrefix,
                                                                       QObject* parent)
: QObject(parent)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    this->outlineModel = NULL;
    
    this->enabledCheckBox = new QCheckBox(" ");
    QObject::connect(this->enabledCheckBox, &QCheckBox::clicked,
                     this, &VolumeSurfaceOutlineViewController::enabledCheckBoxChecked);
    this->enabledCheckBox->setToolTip("Enables display of this volume surface outline");
    this->enabledCheckBox->setObjectName(objectNamePrefix
                                         + ":Enable");
    macroManager->addMacroSupportToObject(this->enabledCheckBox,
                                          "Enable volume surface outline for " + descriptivePrefix);
    
    this->surfaceSelectionViewController = new SurfaceSelectionViewController(this,
                                                                              (objectNamePrefix
                                                                               + ":Surface"),
                                                                              "Select volume surface outline surface for " + descriptivePrefix);
    QObject::connect(this->surfaceSelectionViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(surfaceSelected(Surface*)));
    this->surfaceSelectionViewController->getWidget()->setToolTip("Select surface drawn as outline over volume slices");
    
    this->colorOrTabSelectionControl = new VolumeSurfaceOutlineColorOrTabViewController(this);
    QObject::connect(this->colorOrTabSelectionControl, SIGNAL(modelSelected(VolumeSurfaceOutlineColorOrTabModel::Item*)),
                     this, SLOT(colorTabSelected(VolumeSurfaceOutlineColorOrTabModel::Item*)));
    this->colorOrTabSelectionControl->getWidget()->setToolTip("Select coloring for surface outline.\n"
                                                              "If tab, coloring assigned to selected surface\n"
                                                              "in the selected tab is used.\n");
    this->colorOrTabSelectionControl->getWidget()->setObjectName(objectNamePrefix
                                                                 + ":ColorSource");
    macroManager->addMacroSupportToObject(this->colorOrTabSelectionControl->getWidget(),
                                          "Set surface outline color for " + descriptivePrefix);
    
    this->thicknessSpinBox = new WuQDoubleSpinBox(this);
    this->thicknessSpinBox->setRange(0.0, 100.0);
    this->thicknessSpinBox->setSingleStep(0.10);
    this->thicknessSpinBox->setSuffix("%");
    QObject::connect(this->thicknessSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &VolumeSurfaceOutlineViewController::thicknessSpinBoxValueChanged);
    this->thicknessSpinBox->getWidget()->setToolTip("Thickness of surface outline as percentage of viewport height");
    this->thicknessSpinBox->getWidget()->setObjectName(objectNamePrefix
                                          + ":Thickness");
    macroManager->addMacroSupportToObject(this->thicknessSpinBox->getWidget(),
                                          "Set thickness for volume surface outline for " + descriptivePrefix);
    
    
    if (orientation == Qt::Horizontal) {
        this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                       this);
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox, row, 0);
        this->gridLayoutGroup->addWidget(this->colorOrTabSelectionControl->getWidget(), row, 1);        
        this->gridLayoutGroup->addWidget(this->thicknessSpinBox->getWidget(), row, 2);
        this->gridLayoutGroup->addWidget(this->surfaceSelectionViewController->getWidget(), row, 3);
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                       this);
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox, row, 0, 2, 1, Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(this->surfaceSelectionViewController->getWidget(), row, 1, 1, 2);
        row++;
        this->gridLayoutGroup->addWidget(this->colorOrTabSelectionControl->getWidget(), row, 1);        
        this->gridLayoutGroup->addWidget(this->thicknessSpinBox->getWidget(), row, 2, Qt::AlignLeft);
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget, row, 0, 1, -1);
    }
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineViewController::~VolumeSurfaceOutlineViewController()
{
}

/**
 * Set the visibility of widgets in this view controller.
 */
void 
VolumeSurfaceOutlineViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/**
 * Called when a surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void 
VolumeSurfaceOutlineViewController::surfaceSelected(Surface* surface)
{
    if (this->outlineModel != NULL) {
        this->outlineModel->getSurfaceSelectionModel()->setSurface(surface);
    }
    
    this->updateGraphics();
}

/**
 * Called when a color/tab is selected.
 * @param colorTab
 *    Value that was selected.
 */
void 
VolumeSurfaceOutlineViewController::colorTabSelected(VolumeSurfaceOutlineColorOrTabModel::Item* /*colorTab*/)
{
    this->updateGraphics();
}

/**
 * Called when enabled checkbox is selected.
 * @param checked
 *    New state of checkbox.
 */
void 
VolumeSurfaceOutlineViewController::enabledCheckBoxChecked(bool checked)
{
    if (this->outlineModel != NULL) {
        this->outlineModel->setDisplayed(checked);
    }
    this->updateGraphics();
}

/**
 * Called when thickness value is changed.
 * @param value
 *    Value that was selected.
 */
void 
VolumeSurfaceOutlineViewController::thicknessSpinBoxValueChanged(double value)
{
    if (this->outlineModel != NULL) {
        this->outlineModel->setThicknessPercentageViewportHeight(value);
    }
    this->updateGraphics();
}

/**
 * Update this view controller.
 * @param outlineModel
 *    Outline model for use in this view controller.
 */
void 
VolumeSurfaceOutlineViewController::updateViewController(VolumeSurfaceOutlineModel* outlineModel)
{
    this->outlineModel = outlineModel;
    
    if (this->outlineModel != NULL) {
        this->enabledCheckBox->setChecked(this->outlineModel->isDisplayed());
        
        this->thicknessSpinBox->blockSignals(true);
        float thickness = outlineModel->getThicknessPercentageViewportHeight();
        if (thickness < 0.0f) {
            /* old scenes will have negative for mm thickness */
            thickness = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT;
        }
        this->thicknessSpinBox->setValue(thickness);
        this->thicknessSpinBox->blockSignals(false);
        this->surfaceSelectionViewController->updateControl(outlineModel->getSurfaceSelectionModel());
        this->colorOrTabSelectionControl->updateViewController(outlineModel->getColorOrTabModel());
    }
}

/**
 * Update the graphics.
 */
void 
VolumeSurfaceOutlineViewController::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


