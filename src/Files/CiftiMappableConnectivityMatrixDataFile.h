#ifndef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__
#define __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__

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

#include <set>

#include "BrainConstants.h"
#include "ChartMatrixLoadingDimensionEnum.h"
#include "CiftiMappableDataFile.h"
#include "VoxelIJK.h"

namespace caret {

    class ConnectivityDataLoaded;
    class SceneClassAssistant;
    
    class CiftiMappableConnectivityMatrixDataFile :
    public CiftiMappableDataFile
    {
    protected:
        CiftiMappableConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType);
        
    public:
        virtual ~CiftiMappableConnectivityMatrixDataFile();
        
        bool isMapDataLoadingEnabled(const int32_t mapIndex) const;
        
        void setMapDataLoadingEnabled(const int32_t mapIndex,
                                      const bool enabled);
        
        virtual void loadMapDataForSurfaceNode(const int32_t mapIndex,
                                                  const int32_t surfaceNumberOfNodes,
                                                  const StructureEnum::Enum structure,
                                                  const int32_t nodeIndex,
                                                  int64_t& rowIndexOut,
                                                  int64_t& columnIndexOut);
        
        virtual void loadMapAverageDataForSurfaceNodes(const int32_t mapIndex,
                                                       const int32_t surfaceNumberOfNodes,
                                                       const StructureEnum::Enum structure,
                                                       const std::vector<int32_t>& nodeIndices);
        
        virtual void loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                     const float xyz[3],
                                                     int64_t& rowIndexOut,
                                                     int64_t& columnIndexOut);

        virtual bool loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                       const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices);

        void loadDataForRowIndex(const int64_t rowIndex);
        
        void loadDataForColumnIndex(const int64_t rowIndex);
                
        virtual void clear();
        
        virtual bool isEmpty() const;

		virtual AString getMapName(const int32_t mapIndex) const;

        AString getRowName(const int32_t rowIndex) const;
        
        AString getColumnName(const int32_t rowIndex) const;

        AString getRowLoadedText() const;

		//int64_t getRowLoadedIndex() const;

        virtual void getMapData(const int32_t mapIndex, std::vector<float>& dataOut) const;

//        bool loadMapData(const int32_t rowIndex);
        
        const ConnectivityDataLoaded* getConnectivityDataLoaded() const;
        
        bool getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut,
                                                    const StructureEnum::Enum &structure) const;
        
        ChartMatrixLoadingDimensionEnum::Enum getChartMatrixLoadingDimension() const;
        
    private:
        CiftiMappableConnectivityMatrixDataFile(const CiftiMappableConnectivityMatrixDataFile&);

        CiftiMappableConnectivityMatrixDataFile& operator=(const CiftiMappableConnectivityMatrixDataFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void resetLoadedRowDataToEmpty();
        
        void setChartMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum matrixLoadingType);
        
    private:
        void setLoadedRowDataToAllZeros();
        
        void clearPrivate();
        
//        int64_t getRowIndexForNodeWhenLoading(const StructureEnum::Enum structure,
//                                              const int64_t surfaceNumberOfNodes,
//                                              const int64_t nodeIndex);
        
        void getRowColumnIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                                 const int64_t surfaceNumberOfNodes,
                                                 const int64_t nodeIndex,
                                                 int64_t& rowIndexOut,
                                                 int64_t& columnIndexOut);
        
        void getRowColumnIndexForVoxelAtCoordinateWhenLoading(const float xyz[3],
                                                                 int64_t& rowIndexOut,
                                                                 int64_t& columnIndexOut);
        
        void getRowColumnIndexForVoxelIndexWhenLoading(const int64_t ijk[3],
                                                          int64_t& rowIndexOut,
                                                          int64_t& columnIndexOut);
        
//        int64_t getRowIndexForVoxelAtCoordinateWhenLoading(const float xyz[3]);
//        
//        int64_t getRowIndexForVoxelIndexWhenLoading(const int64_t ijk[3]);
        
        int32_t getCifitDirectionForLoadingRowOrColumn();
        
        // ADD_NEW_MEMBERS_HERE
        
        SceneClassAssistant* m_sceneAssistant;
        
        bool m_dataLoadingEnabled;
        
        std::vector<float> m_loadedRowData;
        
        AString m_rowLoadedTextForMapName;

        AString m_rowLoadedText;
        
        ConnectivityDataLoaded* m_connectivityDataLoaded;
        
        /*
         * This is really a member of parcel file since it the parcel
         * file is the only file that can load by row or column.
         * However, because of scenes, the chart loading dimension needs
         * to be restored in this class.
         */
        ChartMatrixLoadingDimensionEnum::Enum m_chartLoadingDimension;
        
        friend class CiftiBrainordinateScalarFile;

    };
    
#ifdef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__
