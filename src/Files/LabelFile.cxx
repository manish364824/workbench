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

#include "CaretAssert.h"

#include "GiftiFile.h"
#include "MathFunctions.h"
#include "LabelFile.h"

using namespace caret;

/**
 * Constructor.
 */
LabelFile::LabelFile()
: GiftiTypeFile()
{
    this->initializeMembersLabelFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
LabelFile::LabelFile(const LabelFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperLabelFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the LabelFile parameter.
 */
LabelFile& 
LabelFile::operator=(const LabelFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperLabelFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
LabelFile::~LabelFile()
{
    this->columnDataPointers.clear();
}

/**
 * Clear the surface file.
 */
void 
LabelFile::clear()
{
    GiftiTypeFile::clear();
    this->columnDataPointers.clear();
}

/** 
 * @return Return the GIFTI Label Table. 
 */
GiftiLabelTable* 
LabelFile::getLabelTable() 
{ 
    return this->giftiFile->getLabelTable(); 
}

/** 
 * @return Return the GIFTI Label Table. 
 */
const GiftiLabelTable* 
LabelFile::getLabelTable() const 
{ 
    return this->giftiFile->getLabelTable(); 
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
LabelFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->columnDataPointers.clear();

    this->initializeMembersLabelFile();
    
    this->verifyDataArraysHaveSameNumberOfRows(0, 0);
    
    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfDataArrays; i++) {
        this->columnDataPointers.push_back(this->giftiFile->getDataArray(i)->getDataPointerInt());
    }
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
LabelFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
LabelFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}


/**
 * Initialize members of this class.
 */
void 
LabelFile::initializeMembersLabelFile()
{
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
LabelFile::copyHelperLabelFile(const LabelFile& sf)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Label key at the given node and column indices.
 */
int32_t 
LabelFile::getLabelKey(const int32_t nodeIndex,
                       const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), 
                       "Node Index out of range.");
    
    return this->columnDataPointers[columnIndex][nodeIndex];
}

/**
 * set label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * param labelKey
 *     Label key inserted at the given node and column indices.
 */
void 
LabelFile::setLabelKey(const int32_t nodeIndex,
                       const int32_t columnIndex,
                       const int32_t labelKey)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), "Node Index out of range.");
    
    this->columnDataPointers[columnIndex][nodeIndex] = labelKey;
}

/**
 * Get a pointer to the keys for a label file column.
 * @param columnIndex
 *     Index of the column.
 * @return 
 *     Pointer to keys for the given column.
 */
const int32_t* 
LabelFile::getLabelKeyPointerForColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    return this->columnDataPointers[columnIndex];    
}




