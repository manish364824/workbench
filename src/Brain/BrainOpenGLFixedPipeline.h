
#ifndef __BRAIN_OPENGL_FIXED_PIPELINE_H__
#define __BRAIN_OPENGL_FIXED_PIPELINE_H__

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

#include "BrainConstants.h"
#include "BrainOpenGL.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretVolumeExtension.h"
#include "DisplayGroupEnum.h"
#include "FiberOrientationColoringTypeEnum.h"
#include "FiberOrientationSymbolTypeEnum.h"
#include "FiberTrajectoryColorModel.h"
#include "ProjectionViewTypeEnum.h"
#include "SelectionItemDataTypeEnum.h"
#include "StructureEnum.h"
#include "SurfaceNodeColoring.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "WholeBrainVoxelDrawingMode.h"


class QGLWidget;
class QImage;

namespace caret {
    
    class BoundingBox;
    class Brain;
    class BrainOpenGLShapeCone;
    class BrainOpenGLShapeCube;
    class BrainOpenGLShapeCylinder;
    class BrainOpenGLShapeSphere;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class CaretMappableDataFile;
    class ClippingPlaneGroup;
    class FastStatistics;
    class DisplayPropertiesFiberOrientation;
    class FiberOrientation;
    class SelectionItem;
    class SelectionManager;
    class IdentificationWithColor;
    class ImageFile;
    class Plane;
    class Surface;
    class Model;
    class ModelChart;
    class ModelSurface;
    class ModelSurfaceMontage;
    class ModelVolume;
    class ModelWholeBrain;
    class Palette;
    class PaletteColorMapping;
    class PaletteFile;
    class SurfaceFile;
    class SurfaceMontageConfigurationCerebellar;
    class SurfaceMontageConfigurationCerebral;
    class SurfaceMontageConfigurationFlatMaps;
    class VolumeFile;
    class VolumeMappableInterface;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGLFixedPipeline : public BrainOpenGL {
        
    private:
        enum Mode {
            MODE_DRAWING,
            MODE_IDENTIFICATION,
            MODE_PROJECTION
        };
        
        BrainOpenGLFixedPipeline(const BrainOpenGLFixedPipeline&);
        BrainOpenGLFixedPipeline& operator=(const BrainOpenGLFixedPipeline&);
        
    public:
        BrainOpenGLFixedPipeline(BrainOpenGLTextRenderInterface* textRenderer);

        ~BrainOpenGLFixedPipeline();
        
        void drawModels(std::vector<BrainOpenGLViewportContent*>& viewportContents);
        
        void selectModel(BrainOpenGLViewportContent* viewportContent,
                         const int32_t mouseX,
                         const int32_t mouseY,
                         const bool applySelectionBackgroundFiltering);
        
        void projectToModel(BrainOpenGLViewportContent* viewportContent,
                            const int32_t mouseX,
                            const int32_t mouseY,
                            SurfaceProjectedItem& projectionOut);
        
        void initializeOpenGL();
        
        virtual AString getStateOfOpenGL() const;
        
    private:
        class VolumeDrawInfo {
        public:
            VolumeDrawInfo(CaretMappableDataFile* mapFile,
                           VolumeMappableInterface* volumeFile,
                           Brain* brain,
                           PaletteColorMapping* paletteColorMapping,
                           const FastStatistics* statistics,
                           const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode,
                           const int32_t mapIndex,
                           const float opacity);
            Brain* brain;
            CaretMappableDataFile* mapFile;
            VolumeMappableInterface* volumeFile;
            SubvolumeAttributes::VolumeType volumeType;
            PaletteColorMapping* paletteColorMapping;
            WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode;
            const FastStatistics* statistics;
            int32_t mapIndex;
            float opacity;
        };

        struct FiberOrientationDisplayInfo {
            float aboveLimit;
            float belowLimit;
            FiberTrajectoryColorModel::Item* colorSource;
            FiberOrientationColoringTypeEnum::Enum fiberOrientationColorType;
            float fanMultiplier;
            bool isDrawWithMagnitude;
            float minimumMagnitude;
            float magnitudeMultiplier;
            Plane* plane;
            FiberOrientationSymbolTypeEnum::Enum symbolType;
            StructureEnum::Enum structure;
        };
        
        void setFiberOrientationDisplayInfo(const DisplayPropertiesFiberOrientation* dpfo,
                                            const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            Plane* plane,
                                            const StructureEnum::Enum structure,
                                            FiberTrajectoryColorModel::Item* colorSource,
                                            FiberOrientationDisplayInfo& dispInfo);

        void drawModelInternal(Mode mode,
                               BrainOpenGLViewportContent* viewportContent);
        
        void initializeMembersBrainOpenGL();
        
        void drawChartData(BrowserTabContent* browserTabContent,
                            ModelChart* chartData,
                            const int32_t viewport[4]);
        
        void drawSurfaceModel(ModelSurface* surfaceModel,
                                   const int32_t viewport[4]);
        
        void drawSurface(Surface* surface,
                         const float* nodeColoringRGBA);
        
        void drawSurfaceNodes(Surface* surface,
                              const float* nodeColoringRGBA);
        
        void drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                  const float* nodeColoringRGBA);
        
        void drawSurfaceTriangles(Surface* surface,
                                  const float* nodeColoringRGBA);
        
        void drawSurfaceNodeAttributes(Surface* surface);
        
        void drawSurfaceBorderBeingDrawn(const Surface* surface);
        
        void drawSurfaceBorders(Surface* surface);
        
        struct BorderDrawInfo {
            Surface* anatomicalSurface;
            Surface* surface;
            Border* border;
            int32_t borderFileIndex;
            int32_t borderIndex;
            float rgba[4];
            bool isSelect;
            bool isContralateralEnabled;
            bool isHighlightEndPoints;
            float unstretchedLinesLength;
        };
        
        void drawBorder(const BorderDrawInfo& borderDrawInfo);
        
        bool unstretchedBorderLineTest(const float p1[3],
                                       const float p2[3],
                                       const float anat1[3],
                                       const float anat2[3],
                                       const float unstretchedLinesFactor) const;
        
        void drawSurfaceFoci(Surface* surface);
        
        void drawSurfaceNormalVectors(const Surface* surface);
        
        void drawSurfaceFiberOrientations(const StructureEnum::Enum structure);
        
        void drawFiberOrientations(const Plane* plane,
                                   const StructureEnum::Enum structure);
        
        void addFiberOrientationForDrawing(const FiberOrientationDisplayInfo* fodi,
                                           const FiberOrientation* fiberOrientation);
        
        void sortFiberOrientationsByDepth();
        
        void drawAllFiberOrientations(const FiberOrientationDisplayInfo* fodi,
                                      const bool isSortFibers);
        
        void drawSurfaceFiberTrajectories(const StructureEnum::Enum structure);
        
        void drawFiberTrajectories(const Plane* plane,
                                   const StructureEnum::Enum structure);
        
        void drawVolumeModel(BrowserTabContent* browserTabContent,
                                  ModelVolume* volumeModel,
                                  const int32_t viewport[4]);
        
        void drawVolumeAxesCrosshairs(
                                  const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                  const float voxelXYZ[3]);
        
        void drawVolumeAxesLabels(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const int32_t viewport[4]);

        void drawVolumeVoxelsAsCubesWholeBrain(std::vector<VolumeDrawInfo>& volumeDrawInfoIn);
        
        void drawVolumeOrthogonalSliceWholeBrain(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       std::vector<VolumeDrawInfo>& volumeDrawInfoIn);
        
        void drawVolumeOrthogonalSliceVolumeViewer(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       std::vector<VolumeDrawInfo>& volumeDrawInfo);
        
        void drawVolumeSurfaceOutlines(Brain* brain,
                                       Model* modelDisplayModel,
                                       BrowserTabContent* browserTabContent,
                                       const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       VolumeMappableInterface* underlayVolume);
        
        void drawVolumeFoci(Brain* brain,
                            ModelVolume* modelVolume,
                            BrowserTabContent* browserTabContent,
                            const VolumeSliceViewPlaneEnum::Enum slicePlane,
                            const int64_t sliceIndex,
                            VolumeMappableInterface* underlayVolume);
        
        void drawVolumeFibers(Brain* brain,
                              ModelVolume* modelVolume,
                              BrowserTabContent* browserTabContent,
                              const VolumeSliceViewPlaneEnum::Enum slicePlane,
                              const int64_t sliceIndex,
                              VolumeMappableInterface* underlayVolume);
        
        void setupVolumeDrawInfo(BrowserTabContent* browserTabContent,
                                 Brain* brain,
                                 std::vector<VolumeDrawInfo>& volumeDrawInfoOut);
        
        void drawWholeBrainModel(BrowserTabContent* browserTabContent,
                                      ModelWholeBrain* wholeBrainModel,
                                      const int32_t viewport[4]);
        
        void drawSurfaceMontageModel(BrowserTabContent* browserTabContent,
                                     ModelSurfaceMontage* surfaceMontageModel,
                                     const int32_t viewport[4]);
        
        
        void setOrthographicProjection(const int32_t viewport[4],
                                       const ProjectionViewTypeEnum::Enum projectionType);
        
        void setOrthographicProjectionForWithBoundingBox(const int32_t viewport[4],
                                                         const ProjectionViewTypeEnum::Enum projectionType,
                                                          const BoundingBox* boundingBox);
        
        void setOrthographicProjectionWithHeight(const int32_t viewport[4],
                                                 const ProjectionViewTypeEnum::Enum projectionType,
                                                 const float halfWindowHeight);
        
        void checkForOpenGLError(const Model* modelModel,
                                 const AString& msg);
        
        void enableLighting();
        
        void disableLighting();
        
        void enableLineAntiAliasing();
        void disableLineAntiAliasing();
        
        void getIndexFromColorSelection(const SelectionItemDataTypeEnum::Enum dataType,
                                           const int32_t x,
                                           const int32_t y,
                                           int32_t& indexOut,
                                           float& depthOut);
        
        void getIndexFromColorSelection(const SelectionItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y,
                                        int32_t& index1Out,
                                        int32_t& index2Out,
                                        float& depthOut);
        
        void getIndexFromColorSelection(const SelectionItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y,
                                        int32_t& index1Out,
                                        int32_t& index2Out,
                                        int32_t& index3Out,
                                        float& depthOut);
        
        void setSelectedItemScreenXYZ(SelectionItem* item,
                                        const float itemXYZ[3]);

        void setViewportAndOrthographicProjection(const int32_t viewport[4],
                                                  const  ProjectionViewTypeEnum::Enum projectionType);
        
        void setViewportAndOrthographicProjectionForWholeBrainVolume(const int32_t viewport[4],
                                                           const  ProjectionViewTypeEnum::Enum projectionType,
                                                           const VolumeMappableInterface* volume);
        
        void setViewportAndOrthographicProjectionForSurfaceFile(const int32_t viewport[4],
                                                                const  ProjectionViewTypeEnum::Enum projectionType,
                                                                const SurfaceFile* surfaceFile);
        
        void applyViewingTransformations(const float objectCenterXYZ[3],
                                         const ProjectionViewTypeEnum::Enum projectionViewType);
        
        void applyViewingTransformationsVolumeSlice(const ModelVolume* modelVolume,
                                         const int32_t tabIndex,
                                         const VolumeSliceViewPlaneEnum::Enum viewPlane);
        
        void drawSurfaceAxes();
        
        void drawSphereWithDiameter(const uint8_t rgba[4],
                                    const double diameter);
        
        void drawSphereWithDiameter(const float rgba[4],
                                    const double diameter);
        
        void drawSquare(const float rgba[4],
                        const float size);
        
        void drawSquare(const uint8_t rgba[4],
                        const float size);
        
        void drawCube(const float rgba[4],
                      const double cubeSize);
        
        void drawCuboid(const uint8_t rgba[4],
                        const double sizeX,
                        const double sizeY,
                        const double sizeZ);
        
        void drawRoundedCube(const float rgba[4],
                             const double cubeSize);
        
        void drawRoundedCuboid(const uint8_t rgba[4],
                               const double sizeX,
                        const double sizeY,
                        const double sizeZ);
        
        void drawCylinder(const float rgba[4],
                          const float bottomXYZ[3],
                          const float topXYZ[3],
                          const float radius);
        
        void drawEllipticalCone(const float rgba[4],
                                const float baseXYZ[3],
                                const float apexXYZ[3],
                                const float baseRadiusScaling,
                                const float baseMajorAngle,
                                const float baseMinorAngle,
                                const float baseRotationAngle,
                                const bool backwardsFlag);
        
        void drawTextWindowCoords(const int windowX,
                                  const int windowY,
                                  const QString& text,
                                  const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                  const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY,
                                  const BrainOpenGLTextRenderInterface::TextStyle textStyle,
                                  const int fontHeight);
        
        void drawTextWindowCoordsWithBackground(const int windowX,
                                      const int windowY,
                                      const QString& text,
                                      const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                      const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY,
                                      const BrainOpenGLTextRenderInterface::TextStyle textStyle,
                                      const int fontHeight);
        
        void drawTextModelCoords(const double modelX,
                                 const double modelY,
                                 const double modelZ,
                                 const QString& text);
        
        void drawTextModelCoords(const double modelXYZ[3],
                                 const QString& text);
        
        void drawAllPalettes(Brain* brain);
        
        void drawPalette(const Palette* palette,
                         const PaletteColorMapping* paletteColorMapping,
                         const FastStatistics* statistics,
                         const int paletteDrawingIndex);
        
        void drawBackgroundImage(BrainOpenGLViewportContent* vpContent);
        
        void drawImage(const int viewport[4],
                       const ImageFile* imageFile);
        
        void setProjectionModeData(const float screenDepth,
                                          const float xyz[3],
                                          const StructureEnum::Enum structure,
                                          const float barycentricAreas[3],
                                          const int barycentricNodes[3],
                                          const int numberOfNodes);
        
        void setLineWidth(const float lineWidth);
        
        void setPointSize(const float pointSize);
        
        enum ClippingDataType {
            CLIPPING_DATA_TYPE_FEATURES,
            CLIPPING_DATA_TYPE_SURFACE,
            CLIPPING_DATA_TYPE_VOLUME
        };
        
        void applyClippingPlanes(const ClippingDataType clippingDataType,
                                 const StructureEnum::Enum structureIn);
        
        void disableClippingPlanes();
        
        bool isCoordinateInsideClippingPlanesForStructure(const StructureEnum::Enum structureIn,
                                                          const float xyz[3]) const;
        
        bool isFeatureClippingEnabled() const;
        
        void getVolumeFitToWindowScalingAndTranslation(const VolumeMappableInterface* volume,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const double orthographicExtent[6],
                                                       float translationOut[3],
                                                       float& scalingOut) const;
        
        Plane getPlaneForVolumeSliceIndex(const VolumeMappableInterface* volumeMappable,
                                          const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                          const int64_t sliceIndex) const;
        
        void updateForegroundAndBackgroundColors(BrainOpenGLViewportContent* vpContent);
        
        /** Indicates OpenGL has been initialized */
        bool initializedOpenGLFlag;
        
        /** Content of browser tab being drawn */
        BrowserTabContent* browserTabContent;
        
        /** Source brain of content being drawn DOES NOT get deleted! */
        Brain* m_brain;
        
        /** Index of window tab */
        int32_t windowTabIndex;
        
        /** mode of operation draw/select/etc*/
        Mode mode;
        
        /** Identification manager */
        SelectionManager* selectionManager;
        
        int32_t mouseX;
        int32_t mouseY;
        
        /** Clipping plane group active in browser tab */
        ClippingPlaneGroup* m_clippingPlaneGroup;
        
        /** 
         * When mirrored clipping is enabled, the clipping region is
         * 'mirror flipped' for right structures and clipping performed
         * separately for left and right structures.  Otherwise, one
         * clipping is used for all data.
         *
         *   Model      Enabled
         *   -----      -------
         *   All        NO
         *   Montage    YES
         *   Surface    YES
         *   Volume     NO
         */
        bool m_mirroredClippingEnabled;
        
        /** Identify using color */
        IdentificationWithColor* colorIdentification;

        SurfaceProjectedItem* modeProjectionData;
        
        /** Screen depth when projecting to surface mode */
        double modeProjectionScreenDepth;
        
        /** Performs node coloring */
        SurfaceNodeColoring* surfaceNodeColoring;
         
        /** Sphere symbol */
        BrainOpenGLShapeSphere* m_shapeSphere;
        
        /** Cone symbol */
        BrainOpenGLShapeCone* m_shapeCone;
        
        /** Cube symbol */
        BrainOpenGLShapeCube* m_shapeCube;
        
        /** Rounded Cube symbol */
        BrainOpenGLShapeCube* m_shapeCubeRounded;
        
        /** Cylinder symbol */
        BrainOpenGLShapeCylinder* m_shapeCylinder;
        
        std::list<FiberOrientation*> m_fiberOrientationsForDrawing;
        
        double inverseRotationMatrix[16];
        bool inverseRotationMatrixValid;
        
        double orthographicLeft; 
        double orthographicRight;
        double orthographicBottom;
        double orthographicTop;
        double orthographicFar; 
        double orthographicNear;
        
        static bool s_staticInitialized;
        
        static float COLOR_RED[3];
        static float COLOR_GREEN[3];
        static float COLOR_BLUE[3];
        
        friend class BrainOpenGLChartDrawingFixedPipeline;
        friend class BrainOpenGLVolumeObliqueSliceDrawing;
        friend class BrainOpenGLVolumeSliceDrawing;
        friend class OldBrainOpenGLVolumeSliceDrawing;
    };

#ifdef __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H
    bool BrainOpenGLFixedPipeline::s_staticInitialized = false;
    float BrainOpenGLFixedPipeline::COLOR_RED[3]   = { 1.0, 0.0, 0.0 };
    float BrainOpenGLFixedPipeline::COLOR_GREEN[3] = { 0.0, 1.0, 0.0 };
    float BrainOpenGLFixedPipeline::COLOR_BLUE[3]  = { 0.0, 0.0, 1.0 };
#endif //__BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_FIXED_PIPELINE_H__
