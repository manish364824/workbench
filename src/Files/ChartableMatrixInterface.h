#ifndef __CHARTABLE_MATRIX_INTERFACE_H__
#define __CHARTABLE_MATRIX_INTERFACE_H__

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

#include "CaretColorEnum.h"
#include "ChartDataTypeEnum.h"
#include "CiftiParcelColoringModeEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class ChartMatrixDisplayProperties;
    
    class ChartableMatrixInterface {
        
    protected:
        ChartableMatrixInterface() { }
        
        virtual ~ChartableMatrixInterface() { }
        
    public:
        /**
         * Get the matrix RGBA coloring for this matrix data creator.
         *
         * @param numberOfRowsOut
         *    Number of rows in the coloring matrix.
         * @param numberOfColumnsOut
         *    Number of rows in the coloring matrix.
         * @param rgbaOut
         *    RGBA coloring output with number of elements
         *    (numberOfRowsOut * numberOfColumnsOut * 4).
         * @return
         *    True if data output data is valid, else false.
         */
        virtual bool getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                       int32_t& numberOfColumnsOut,
                                       std::vector<float>& rgbaOut) const = 0;
        
        /**
         * Get the value, row name, and column name for a cell in the matrix.
         *
         * @param rowIndex
         *     The row index.
         * @param columnIndex
         *     The column index.
         * @param cellValueOut
         *     Output containing value in the cell.
         * @param rowNameOut
         *     Name of row corresponding to row index.
         * @param columnNameOut
         *     Name of column corresponding to column index.
         * @return
         *     True if the output values are valid (valid row/column indices).
         */
        virtual bool getMatrixCellAttributes(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             float& cellValueOut,
                                             AString& rowNameOut,
                                             AString& columnNameOut) const = 0;

        /**
         * @return The CaretMappableDataFile that implements this interface (const methdod).
         */
        virtual const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const = 0;
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) = 0;
        
        /**
         * @return Coloring mode for selected parcel.
         */
        virtual CiftiParcelColoringModeEnum::Enum getSelectedParcelColoringMode() const = 0;
        
        /**
         * Set the coloring mode for selected parcel.
         *
         * @param coloringMode
         *    New value for coloring mode.
         */
        virtual void setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode) = 0;
        
        /**
         * @return Color for selected parcel.
         */
        virtual CaretColorEnum::Enum getSelectedParcelColor() const = 0;
        
        /**
         * Set color for selected parcel.
         *
         * @param color
         *    New color for selected parcel.
         */
        virtual void setSelectedParcelColor(const CaretColorEnum::Enum color) = 0;
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getMatrixChartCaretMappableDataFile();
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getMatrixChartCaretMappableDataFile() const;
        
        /**
         * @return Is charting enabled for this file in the given tab?
         */
        virtual bool isMatrixChartingEnabled(const int32_t tabIndex) const = 0;
        
        /**
         * @return Return true if the file's current state supports
         * charting data, else false.  Typically a brainordinate file
         * is chartable if it contains more than one map.
         */
        virtual bool isMatrixChartingSupported() const = 0;
        
        /**
         * Set charting enabled for this file in the given tab
         *
         * @param enabled
         *    New status for charting enabled.
         */
        virtual void setMatrixChartingEnabled(const int32_t tabIndex,
                                                     const bool enabled) = 0;
        
        /**
         * Get chart data types supported by the file.
         *
         * @param chartDataTypesOut
         *    Chart types supported by this file.
         */
        virtual void getSupportedMatrixChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const = 0;
        
        bool isMatrixChartDataTypeSupported(const ChartDataTypeEnum::Enum chartDataType) const;

        // ADD_NEW_METHODS_HERE

    private:
        ChartableMatrixInterface(const ChartableMatrixInterface&);

        ChartableMatrixInterface& operator=(const ChartableMatrixInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_MATRIX_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_MATRIX_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_MATRIX_INTERFACE_H__
