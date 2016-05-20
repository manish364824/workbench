
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

#define __CARET_COLOR_ENUM_COMBOBOX_DECLARE__
#include "CaretColorEnumComboBox.h"
#undef __CARET_COLOR_ENUM_COMBOBOX_DECLARE__

#include <QComboBox>

using namespace caret;


    
/**
 * \class caret::CaretColorEnumComboBox 
 * \brief Control for selection of Caret Color enumerated types.
 *
 * Control for selection of Caret Color enumerated types.
 */
/**
 * Constructor.
 * @param parent
 *     Parent object. 
 */
CaretColorEnumComboBox::CaretColorEnumComboBox(QObject* parent)
: WuQWidget(parent)
{
    this->colorComboBox = new QComboBox();
    
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getColorEnums(colors);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int32_t indx = this->colorComboBox->count();
        const AString name = CaretColorEnum::toGuiName(colorEnum);
        this->colorComboBox->addItem(name);
        this->colorComboBox->setItemData(indx, 
                                         CaretColorEnum::toIntegerCode(colorEnum));

        const float* rgb = CaretColorEnum::toRGB(colorEnum);
        QPixmap pm(10, 10);
        pm.fill(QColor::fromRgbF(rgb[0],
                                 rgb[1],
                                 rgb[2]));
        QIcon icon(pm);
        this->colorComboBox->setItemIcon(indx,
                                         icon);
    }
    
    setSelectedColor(CaretColorEnum::BLACK);
    QObject::connect(this->colorComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(colorComboBoxIndexChanged(int)));
}

/**
 * Destructor.
 */
CaretColorEnumComboBox::~CaretColorEnumComboBox()
{
    
}

/**
 * @return The actual widget.
 */
QWidget* 
CaretColorEnumComboBox::getWidget()
{
    return this->colorComboBox;
}

/**
 * @return The selected color.
 */
CaretColorEnum::Enum 
CaretColorEnumComboBox::getSelectedColor()
{
    const int32_t indx = this->colorComboBox->currentIndex();
    const int32_t integerCode = this->colorComboBox->itemData(indx).toInt();
    CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode, NULL);
    return color;
}

/**
 * Set the selected color.
 * @param color
 *   New color for selection.
 */
void 
CaretColorEnumComboBox::setSelectedColor(const CaretColorEnum::Enum color)
{
    const int32_t numColors = static_cast<int32_t>(this->colorComboBox->count());
    for (int32_t i = 0; i < numColors; i++) {
        const int32_t integerCode = this->colorComboBox->itemData(i).toInt();
        CaretColorEnum::Enum c = CaretColorEnum::fromIntegerCode(integerCode, NULL);
        if (c == color) {
            this->colorComboBox->blockSignals(true);
            this->colorComboBox->setCurrentIndex(i);
            this->colorComboBox->blockSignals(false);
            break;
        }
    }
}

/**
 * Called when a color is selected.
 * @param indx
 *   Index of item selected.
 */
void 
CaretColorEnumComboBox::colorComboBoxIndexChanged(int indx)
{
    const int32_t integerCode = this->colorComboBox->itemData(indx).toInt();
    CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode, NULL);
    emit colorSelected(color);
}

