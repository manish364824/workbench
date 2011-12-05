/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "PaletteScalarAndColor.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param scalar     - the scalar value
 * @param colorIndex - the scalar's color index
 *
 */
PaletteScalarAndColor::PaletteScalarAndColor(
                   const float scalar,
                   const AString& colorName)
    : CaretObject()
{
    this->initializeMembersPaletteScalarAndColor();
    this->scalar = scalar;
    this->colorName = colorName;
}

/**
 * Destructor
 */
PaletteScalarAndColor::~PaletteScalarAndColor()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
PaletteScalarAndColor::PaletteScalarAndColor(const PaletteScalarAndColor& o)
    : CaretObject(o), TracksModificationInterface()
{
    this->initializeMembersPaletteScalarAndColor();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
PaletteScalarAndColor&
PaletteScalarAndColor::operator=(const PaletteScalarAndColor& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
PaletteScalarAndColor::copyHelper(const PaletteScalarAndColor& o)
{
    this->scalar = o.scalar;
    this->colorName = o.colorName;
    this->rgba[0] = o.rgba[0];
    this->rgba[1] = o.rgba[1];
    this->rgba[2] = o.rgba[2];
    this->rgba[3] = o.rgba[3];
    this->clearModified();
}

void
PaletteScalarAndColor::initializeMembersPaletteScalarAndColor()
{
    this->modifiedFlag = false;
    this->scalar = 0.0f;
    this->colorName = "";
    this->rgba[0] = 1.0f;
    this->rgba[1] = 1.0f;
    this->rgba[2] = 1.0f;
    this->rgba[3] = 1.0f;
}
/**
 * Get the scalar.
 * 
 * @return  The scalar.
 *
 */
float
PaletteScalarAndColor::getScalar() const
{
    return this->scalar;
}

/**
 * Set the scalar.
 * 
 * @param scalar - new value for scalar.
 *
 */
void
PaletteScalarAndColor::setScalar(const float scalar)
{
    if (this->scalar != scalar) {
        this->scalar = scalar;
        this->setModified();
    }
}

/**
 * Get the name of the color.
 * @return
 *   Name of the color assigned to the scalar.
 */
AString 
PaletteScalarAndColor::getColorName() const 
{ 
    return this->colorName; 
}

/**
 * Set the name of the color for this scalar.
 * @param colorName
 *    New name for color.
 */
void 
PaletteScalarAndColor::setColorName(const AString& colorName)
{
    if (this->colorName != colorName) {
        this->colorName = colorName;
        this->setModified();
    }
}

/**
 * Get the RGBA components of the color. 
 * @return
 *    float array with red, green, blue, alpha
 * components ranging 0 to 1.
 */
const float* 
PaletteScalarAndColor::getColor() const 
{ 
    return rgba; 
}

/**
 * Get the RGBA components of the color. 
 * @param rgbaOut
 *    float array with red, green, blue, alpha
 * components ranging 0 to 1 are loaded into.
 */
void 
PaletteScalarAndColor::getColor(float rgbaOut[4]) const
{
    rgbaOut[0] = this->rgba[0];
    rgbaOut[1] = this->rgba[1];
    rgbaOut[2] = this->rgba[2];
    rgbaOut[3] = this->rgba[3];
}

/**
 * Set the color's RGBA components.
 * @param rgba
 *    New components for RGBA color.
 */
void 
PaletteScalarAndColor::setColor(const float rgba[4])
{
    if ((this->rgba[0] != rgba[0]) 
        || (this->rgba[1] != rgba[1])
        || (this->rgba[2] != rgba[2])
        || (this->rgba[3] != rgba[3])) {
        this->rgba[0] = rgba[0];
        this->rgba[1] = rgba[1];
        this->rgba[2] = rgba[2];
        this->rgba[3] = rgba[3];
        this->setModified();
    }
}


/**
 * Get string representation for debugging.
 * 
 * @return A string.
 *
 */
AString
PaletteScalarAndColor::toString() const
{
    AString s = 
    "[colorName="
    + this->colorName
    + ", scale="
    + AString::number(this->scalar)
    + ", rgba="
    + AString::fromNumbers(rgba, 4, ",")
    + "]";
    return s;
}

/**
 * Set this object has been modified.
 *
 */
void
PaletteScalarAndColor::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
PaletteScalarAndColor::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
PaletteScalarAndColor::isModified() const
{
    return this->modifiedFlag;
}

/**
 * @return Is the color the 'none' color meaning
 * that no coloring is applied?
 */
bool 
PaletteScalarAndColor::isNoneColor() const
{
    const bool isNone = (this->colorName == "none");
    return isNone;
}


