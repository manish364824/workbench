#ifndef __WU_Q_MACRO_GROUP_XML_STREAM_READER_H__
#define __WU_Q_MACRO_GROUP_XML_STREAM_READER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include "WuQMacroCommandTypeEnum.h"
#include "WuQMacroGroupXmlStreamBase.h"
#include "WuQMacroWidgetTypeEnum.h"

class QXmlStreamReader;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroCommandParameter;
    class WuQMacroGroup;
    class WuQMacroMouseEventInfo;
    
    class WuQMacroGroupXmlStreamReader : public WuQMacroGroupXmlStreamBase {

    public:
        WuQMacroGroupXmlStreamReader();
        
        virtual ~WuQMacroGroupXmlStreamReader();
        
        WuQMacroGroupXmlStreamReader(const WuQMacroGroupXmlStreamReader&) = delete;

        WuQMacroGroupXmlStreamReader& operator=(const WuQMacroGroupXmlStreamReader&) = delete;
        
        bool readFromString(const QString& xmlString,
                            WuQMacroGroup* macroGroup,
                            QString& errorMessageOut);

        void readMacroGroup(QXmlStreamReader& xmlReader,
                            WuQMacroGroup* macroGroup);
        
        // ADD_NEW_METHODS_HERE

    private:
        class MacroCommandContent {
        public:
            WuQMacroCommandTypeEnum::Enum m_commandType = WuQMacroCommandTypeEnum::WIDGET;
            QString m_customOperationTypeName;
            WuQMacroMouseEventInfo* m_mouseInfo = NULL; // DO NOT DELETE
            WuQMacroWidgetTypeEnum::Enum m_widgetType = WuQMacroWidgetTypeEnum::INVALID;
            int32_t m_version = - 1;
            QString m_objectName;
            QString m_descriptiveName;
            QString m_toolTip;
            float m_delay = 1.0f;
            std::vector<WuQMacroCommandParameter*> m_parameters; // DO NOT DELETE
        };

        WuQMacroMouseEventInfo* readMacroMouseEventInfo(QXmlStreamReader& xmlReader);
        
        void readVersionOne(QXmlStreamReader& xmlReader,
                            WuQMacroGroup* macroGroup);
        
        WuQMacro* readMacroVersionOne(QXmlStreamReader& xmlReader);
        
        MacroCommandContent* readMacroCommandAttributesVersionOne(QXmlStreamReader& xmlReader);
        
        WuQMacroCommand* readMacroCommandVersionOne(QXmlStreamReader& xmlReader);
        
        WuQMacroCommandParameter* readMacroCommandParameter(QXmlStreamReader& xmlReader);
        
        void addToWarnings(QXmlStreamReader& xmlReader,
                           const QString& warning);
        
        QString m_warningMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_XML_STREAM_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_GROUP_XML_STREAM_READER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_XML_STREAM_READER_H__
