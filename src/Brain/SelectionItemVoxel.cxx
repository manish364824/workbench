
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

#define __SELECTION_ITEM_VOXEL_DECLARE__
#include "SelectionItemVoxel.h"
#undef __SELECTION_ITEM_VOXEL_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class SelectionItemVoxel
 * \brief Selected voxel.
 *
 * Information about an selected voxel.
 */


/**
 * Constructor.
 */
SelectionItemVoxel::SelectionItemVoxel()
: SelectionItem(SelectionItemDataTypeEnum::VOXEL)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}

/**
 * Constructor for child classes.
 *
 * @param itemDataType
 *     The selection item data type for child class.
 */
SelectionItemVoxel::SelectionItemVoxel(const SelectionItemDataTypeEnum::Enum itemDataType)
: SelectionItem(itemDataType)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}


/**
 * Destructor.
 */
SelectionItemVoxel::~SelectionItemVoxel()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemVoxel::SelectionItemVoxel(const SelectionItemVoxel& obj)
: SelectionItem(obj)
{
    copyHelperSelectionItemVoxel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
SelectionItemVoxel&
SelectionItemVoxel::operator=(const SelectionItemVoxel& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        copyHelperSelectionItemVoxel(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItemVoxel::copyHelperSelectionItemVoxel(const SelectionItemVoxel& idItem)
{
    m_volumeFile  = idItem.m_volumeFile;
    m_voxelIJK[0] = idItem.m_voxelIJK[0];
    m_voxelIJK[1] = idItem.m_voxelIJK[1];
    m_voxelIJK[2] = idItem.m_voxelIJK[2];
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemVoxel::reset()
{
    SelectionItem::reset();
    
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemVoxel::resetPrivate()
{
    m_volumeFile = NULL;
    m_voxelIJK[0] = -1;
    m_voxelIJK[1] = -1;
    m_voxelIJK[2] = -1;
}


/**
 * @return The volume file.
 */
const VolumeMappableInterface* 
SelectionItemVoxel::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * Get the voxel indices.
 * @param voxelIJK
 *    Output containing voxel indices.
 */
void 
SelectionItemVoxel::getVoxelIJK(int64_t voxelIJK[3]) const
{
    voxelIJK[0] = m_voxelIJK[0];
    voxelIJK[1] = m_voxelIJK[1];
    voxelIJK[2] = m_voxelIJK[2];
}

/**
 * Set the volume file.
 *
 * @param brain
 *    Brain containing the volume.
 * @param volumeFile
 *    New value for volume file.
 * @param voxelIJK
 *    New value for voxel indices.
 * @param screenDepth
 *    The screen depth.
 */
void 
SelectionItemVoxel::setVoxelIdentification(Brain* brain,
                                           VolumeMappableInterface* volumeFile,
                                           const int64_t voxelIJK[3],
                                           const double screenDepth)
{
    setBrain(brain);
    m_volumeFile = volumeFile;
    m_voxelIJK[0] = voxelIJK[0];
    m_voxelIJK[1] = voxelIJK[1];
    m_voxelIJK[2] = voxelIJK[2];
    setScreenDepth(screenDepth);
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemVoxel::isValid() const
{
    return (m_volumeFile != NULL);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemVoxel::toString() const
{
    AString text = SelectionItem::toString();
    AString name = "INVALID";
    if (m_volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(m_volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    text += ("Volume: " + name);
    text += ("Voxel: " 
             + AString::number(m_voxelIJK[0]) + ", "
             + AString::number(m_voxelIJK[1]) + ", "
             + AString::number(m_voxelIJK[2]) + "\n");
    
    return text;
}




