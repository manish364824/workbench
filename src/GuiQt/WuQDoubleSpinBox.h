#ifndef __WU_Q_DOUBLE_SPIN_BOX_H__
#define __WU_Q_DOUBLE_SPIN_BOX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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



#include <memory>

#include "WuQWidget.h"

class QDoubleSpinBox;

namespace caret {

    class WuQDoubleSpinBox : public WuQWidget {
        
        Q_OBJECT

    public:
        /**
         * Mode for decimals (digits right of decimal)
         */
        enum class DecimalsMode {
            /**
             * Number of decimals is calculated from the range of data
             */
            AUTO,
            /**
             * Number of decimals is fixed by calling setDecimals(int) 
             * (Normal spin box functionality)
             */
            FIXED
        };
        
        /**
         * Mode for single step (increment/decrement) arrows
         */
        enum class SingleStepMode {
            /**
             * Step by a fixed amount (normal spin box functionality)
             */
            FIXED,
            /**
             * Step by a percentage of data range
             */
            PERCENTAGE
        };
        
        /**
         * Mode for data range
         */
        enum class RangeMode {
            /**
             * Normal spin box behavior, values limited inclusively to minimum and maximum
             */
            INCLUSIVE,
            /**
             * May exceed minimum and maximum values.
             * Formatted for specified minimum and maximum.
             */
            EXCEEDABLE
        };
        
        WuQDoubleSpinBox(QObject* parent);
        
        virtual ~WuQDoubleSpinBox();

        virtual QWidget* getWidget();
        
        //void copySettings(const WuQDoubleSpinBox* copyFromSpinBox);
        
        QString	cleanText() const;
        
        int	decimals() const;
        
        double maximum() const;
        
        double minimum() const;
        
        QString	prefix() const;
        
        DecimalsMode decimalsMode() const;
        
        void setDecimals(int prec);
        
        void setDecimalsModeAuto();
        
        void setPrefix(const QString &prefix);
        
        void setRange(double minimum, double maximum);
        
        void setRangeExceedable(double minimum,
                                double maximum,
                                double exceedAmount);
        
        void setRangeExceedable(double minimum,
                                double maximum);
        
        void setRangePercentage(const double minimumPercentage,
                                const double maximumPercentage);
        
        void setSingleStep(double value);
        
        void setSingleStepPercentage(double percentage);
        
        void setSuffix(const QString &suffix);
        
        double singleStep() const;
        
        SingleStepMode singleStepMode() const;
        
        double singleStepPercentage() const;
        
        QString	suffix() const;
        
        virtual QString	textFromValue(double value) const;
        
        double value() const;
        
        virtual double	valueFromText(const QString &text) const;
        
        virtual void setToolTip(const QString& tooltip);
        
        // ADD_NEW_METHODS_HERE

    public slots:
        void setValue(double val);
        
    signals:
        /**
         * This signal is emitted when the value changes.
         *
         * Example of signal connection:
         *     QObject::connect(m_spinBox,  static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
         *     this, &WuQDoubleSpinBox::valueChanged);
         *
         * @param d
         *     New value.
         */
        void valueChanged(double d);
        
    private slots:
        void valueChangedPrivate(double d);
        
    private:
        WuQDoubleSpinBox(const WuQDoubleSpinBox&);

        WuQDoubleSpinBox& operator=(const WuQDoubleSpinBox&);
        
        void updateSingleStepPercentage();
        
        void updateDecimalsForAutoMode();
        
        int32_t computeDigitsRightOfDecimal(const double dataRange);
        
        void testDigitsRightOfDecimal();
        
        double computeExceedRange(const double minValue,
                                  const double maxValue);
        
        QString doubleToString(const double value) const;
        
        void testExceedRange();
        
        void setRangeExceedable(const double dataMinimum,
                                const double dataMaximum,
                                const double spinBoxMinimum,
                                const double spinBoxMaximum);
        
        double makePowerOfTen(const double value) const;
        
        void setDataRangeToolTip() const;
        
        QDoubleSpinBox* m_spinBox;
        
        RangeMode m_rangeMode = RangeMode::INCLUSIVE;
        
        DecimalsMode m_decimalsMode = DecimalsMode::FIXED;
        
        SingleStepMode m_singleStepMode = SingleStepMode::FIXED;
        
        double m_singleStepPercentage = 1.0;
        
        double m_minimumValue = 0.0;
        
        double m_maximumValue = 99.0;
        
        bool m_blockValueUpdateFlag = false;
        
        QString m_userToolTip;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_DOUBLE_SPIN_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_DOUBLE_SPIN_BOX_DECLARE__

} // namespace
#endif  //__WU_Q_DOUBLE_SPIN_BOX_H__
