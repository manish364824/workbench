#ifndef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__
#define __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__

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

#include "stdint.h"
#include <vector>



namespace caret {

    class BrainOpenGLPrimitiveDrawing {
        
    public:
        static void drawQuads(const std::vector<float>& coordinates,
                       const std::vector<float>& normals,
                       const std::vector<uint8_t>& rgbaColors);
        
    private:
        BrainOpenGLPrimitiveDrawing();
        
        ~BrainOpenGLPrimitiveDrawing();
        
        BrainOpenGLPrimitiveDrawing(const BrainOpenGLPrimitiveDrawing&);

        BrainOpenGLPrimitiveDrawing& operator=(const BrainOpenGLPrimitiveDrawing&);
        
        static void drawQuadsImmediateMode(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                    const std::vector<uint8_t>& rgbaColors,
                                           const int64_t coordinateOffset,
                                           const int64_t coordinateCount);
        
        static void drawQuadsVertexArrays(const std::vector<float>& coordinates,
                                   const std::vector<float>& normals,
                                          const std::vector<uint8_t>& rgbaColors,
                                          const int64_t coordinateOffset,
                                          const int64_t coordinateCount);
        
        static void drawQuadsVertexBuffers(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                           const std::vector<uint8_t>& rgbaColors,
                                           const int64_t coordinateOffset,
                                           const int64_t coordinateCount);
        
    };
    
#ifdef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__
