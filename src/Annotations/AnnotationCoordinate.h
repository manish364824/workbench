#ifndef __ANNOTATION_COORDINATE_H__
#define __ANNOTATION_COORDINATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "AnnotationSurfaceOffsetVectorTypeEnum.h"
#include "CaretObjectTracksModification.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"


namespace caret {
    class SceneClassAssistant;

    class AnnotationCoordinate : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        AnnotationCoordinate();
        
        virtual ~AnnotationCoordinate();
        
        AnnotationCoordinate(const AnnotationCoordinate& obj);

        AnnotationCoordinate& operator=(const AnnotationCoordinate& obj);
        
        const float* getXYZ() const;
        
        void getXYZ(float xyzOut[3]) const;
        
        void setXYZ(const float xyz[3]);
        
        void setXYZ(const double xyz[3]);
        
        void setXYZ(const float x,
                    const float y,
                    const float z);
        
        void setXYZFromViewportXYZ(const float viewportWidth,
                                   const float viewportHeight,
                                   const float viewportX,
                                   const float viewportY);
        
        void getViewportXY(const float viewportWidth,
                           const float viewportHeight,
                           float& viewportXOut,
                           float& viewportYOut) const;
        
        void getSurfaceSpace(StructureEnum::Enum& structureOut,
                             int32_t& surfaceNumberOfNodesOut,
                             int32_t& surfaceNodeIndexOut) const;
        
        void getSurfaceSpace(StructureEnum::Enum& structureOut,
                             int32_t& surfaceNumberOfNodesOut,
                             int32_t& surfaceNodeIndexOut,
                             float& surfaceOffsetLengthOut,
                             AnnotationSurfaceOffsetVectorTypeEnum::Enum& surfaceOffsetVectorTypeOut) const;
        
        void setSurfaceSpace(const StructureEnum::Enum structure,
                             const int32_t surfaceNumberOfNodes,
                             const int32_t surfaceNodeIndex);
        
        void setSurfaceSpace(const StructureEnum::Enum structure,
                             const int32_t surfaceNumberOfNodes,
                             const int32_t surfaceNodeIndex,
                             const float surfaceOffsetLength,
                             const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType);
        
        float getSurfaceOffsetLength() const;
        
        StructureEnum::Enum getSurfaceStructure() const;
        
        AnnotationSurfaceOffsetVectorTypeEnum::Enum getSurfaceOffsetVectorType() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
        static float getDefaultSurfaceOffsetLength();

        
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotationCoordinate(const AnnotationCoordinate& obj);

        void initializeAnnotationCoordinateMembers();
        
        SceneClassAssistant* m_sceneAssistant;

        float m_xyz[3];
        
        int32_t m_surfaceSpaceNodeIndex;
        
        int32_t m_surfaceSpaceNumberOfNodes;
        
        StructureEnum::Enum m_surfaceSpaceStructure;
        
        float m_surfaceOffsetLength;
        
        AnnotationSurfaceOffsetVectorTypeEnum::Enum m_surfaceOffsetVectorType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COORDINATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COORDINATE_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATE_H__
