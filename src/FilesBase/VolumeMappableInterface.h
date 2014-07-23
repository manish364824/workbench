#ifndef __VOLUME_MAPPABLE_INTERFACE_H__
#define __VOLUME_MAPPABLE_INTERFACE_H__

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

#include "DisplayGroupEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {
    
    class BoundingBox;
    class PaletteFile;
    
    /**
     * \class caret::VolumeMappableInterface
     * \brief Interface for data that is mapped to volumes
     * \ingroup FilesBase
     *
     * Defines an interface for data files that are drawn with voxel data.
     */
    class VolumeMappableInterface {
        
    protected:
        VolumeMappableInterface() { }
        
        virtual ~VolumeMappableInterface() { }
        
    private:
        VolumeMappableInterface(const VolumeMappableInterface&);
        
        VolumeMappableInterface& operator=(const VolumeMappableInterface&);
        
    public:
        
        /**
         * Get the dimensions of the volume.
         *
         * @param dimOut1
         *     First dimension (i) out.
         * @param dimOut2
         *     Second dimension (j) out.
         * @param dimOut3
         *     Third dimension (k) out.
         * @param dimTimeOut
         *     Time dimensions out (number of maps)
         * @param numComponents
         *     Number of components per voxel.
         */
        virtual void getDimensions(int64_t& dimOut1,
                                   int64_t& dimOut2,
                                   int64_t& dimOut3,
                                   int64_t& dimTimeOut,
                                   int64_t& numComponents) const = 0;
        
        /**
         * Get the dimensions of the volume.
         *
         * @param dimsOut
         *     Will contain 5 elements: (0) X-dimension, (1) Y-dimension
         * (2) Z-dimension, (3) time, (4) components.
         */
        virtual void getDimensions(std::vector<int64_t>& dimsOut) const = 0;

        /**
         * Does this volume have these spatial dimensions?
         *
         * @param dim1
         *     First dimension.
         * @param dim2
         *     Second dimension.
         * @param dim3
         *     Third dimension.
         * @return
         *     True if this volume's spatial dimensions match the
         *     given dimensions, else false.
         */
        bool matchesDimensions(const int64_t dim1,
                               const int64_t dim2,
                               const int64_t dim3) const;
        
        /**
         * @return The number of componenents per voxel.
         */
        virtual const int64_t& getNumberOfComponents() const = 0;
        
        /**
         * Get the value of the voxel containing the given coordinate.
         *
         * @param coordinateIn
         *    The 3D coordinate
         * @param validOut
         *    If not NULL, will indicate if the coordinate (and hence the 
         *    returned value) is valid.
         * @param mapIndex
         *    Index of map.
         * @param component
         *    Voxel component.
         * @return
         *    Value of voxel containing the given coordinate.
         */
        virtual float getVoxelValue(const float* coordinateIn,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const = 0;
        
        /**
         * Get the value of the voxel containing the given coordinate.
         *
         * @param coordinateX
         *    The X coordinate
         * @param coordinateY
         *    The Y coordinate
         * @param coordinateZ
         *    The Z coordinate
         * @param validOut
         *    If not NULL, will indicate if the coordinate (and hence the
         *    returned value) is valid.
         * @param mapIndex
         *    Index of map.
         * @param component
         *    Voxel component.
         * @return
         *    Value of voxel containing the given coordinate.
         */
        virtual float getVoxelValue(const float coordinateX,
                                    const float coordinateY,
                                    const float coordinateZ,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut1
         *     Output first (x) coordinate.
         * @param coordOut2
         *     Output first (y) coordinate.
         * @param coordOut3
         *     Output first (z) coordinate.
         */
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float& coordOut1,
                                  float& coordOut2,
                                  float& coordOut3) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut
         *     Output XYZ coordinates.
         */
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float* coordOut) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn
         *     IJK indices
         * @param coordOut
         *     Output XYZ coordinates.
         */
        virtual void indexToSpace(const int64_t* indexIn,
                                  float* coordOut) const = 0;
        
        /**
         * Convert a coordinate to indices.  Note that output indices
         * MAY NOT BE WITHING THE VALID VOXEL DIMENSIONS.
         *
         * @param coordIn1
         *     First (x) input coordinate.
         * @param coordIn2
         *     Second (y) input coordinate.
         * @param coordIn3
         *     Third (z) input coordinate.
         * @param indexOut1
         *     First output index (i).
         * @param indexOut2
         *     First output index (j).
         * @param indexOut3
         *     First output index (k).
         */
        virtual void enclosingVoxel(const float& coordIn1,
                                    const float& coordIn2,
                                    const float& coordIn3,
                                    int64_t& indexOut1,
                                    int64_t& indexOut2,
                                    int64_t& indexOut3) const = 0;
        
        /**
         * Determine in the given voxel indices are valid (within the volume).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut1
         *     Output first (x) coordinate.
         * @param brickIndex
         *     Time/map index (default 0).
         * @param component
         *     Voxel component (default 0).
         */
        virtual bool indexValid(const int64_t& indexIn1,
                                const int64_t& indexIn2,
                                const int64_t& indexIn3,
                                const int64_t brickIndex = 0,
                                const int64_t component = 0) const = 0;
        
        /**
         * Get a bounding box for the voxel coordinate ranges.
         *
         * @param boundingBoxOut
         *    The output bounding box.
         */
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const = 0;
        
        /**
         * Get the voxel spacing for each of the spatial dimensions.
         *
         * @param spacingOut1
         *    Spacing for the first dimension (typically X).
         * @param spacingOut2
         *    Spacing for the first dimension (typically Y).
         * @param spacingOut3
         *    Spacing for the first dimension (typically Z).
         */
        void getVoxelSpacing(float& spacingOut1,
                             float& spacingOut2,
                             float& spacingOut3) const;
        
        /**
         * Get the voxel colors for a slice in the map.
         *
         * @param paletteFile
         *    The palette file.
         * @param mapIndex
         *    Index of the map.
         * @param slicePlane
         *    The slice plane.
         * @param sliceIndex
         *    Index of the slice.
         * @param displayGroup
         *    The selected display group.
         * @param tabIndex
         *    Index of selected tab.
         * @param rgbaOut
         *    Output containing the rgba values (must have been allocated
         *    by caller to sufficient count of elements in the slice).
         */
        virtual void getVoxelColorsForSliceInMap(const PaletteFile* paletteFile,
                                                 const int32_t mapIndex,
                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                 const int64_t sliceIndex,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 uint8_t* rgbaOut) const = 0;
        
        /**
         * Get the voxel coloring for the voxel at the given indices.
         *
         * @param paletteFile
         *    The palette file.
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param brickIndex
         *     Time/map index.
         * @param displayGroup
         *    The selected display group.
         * @param tabIndex
         *    Index of selected tab.
         * @param rgbaOut
         *     Output containing RGBA values for voxel at the given indices.
         */
        virtual void getVoxelColorInMap(const PaletteFile* paletteFile,
                                        const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t brickIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        uint8_t rgbaOut[4]) const = 0;
    };
    
#ifdef __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    
} // namespace
#endif  //__VOLUME_MAPPABLE_INTERFACE_H__
