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

#include "CiftiLabelsMap.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"

using namespace caret;

void CiftiLabelsMap::clear()
{
    m_maps.clear();
}

GiftiLabelTable* CiftiLabelsMap::getMapLabelTable(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return &(m_maps[index].m_labelTable);
}

GiftiMetaData* CiftiLabelsMap::getMapMetadata(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return &(m_maps[index].m_metaData);
}

const QString& CiftiLabelsMap::getMapName(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return m_maps[index].m_name;
}

int64_t CiftiLabelsMap::getIndexFromNumberOrName(const QString& numberOrName) const
{
    bool ok = false;
    int64_t ret = numberOrName.toLongLong(&ok) - 1;//quirk: use string "1" as the first index
    if (ok)
    {
        if (ret < 0 || ret >= getLength()) return -1;//if it is a number, do not try to use it as a name, under any circumstances
        return ret;
    } else {
        int64_t length = getLength();
        for (int64_t i = 0; i < length; ++i)
        {
            if (numberOrName == getMapName(i)) return i;
        }
        return -1;
    }
}

void CiftiLabelsMap::setLength(const int64_t& length)
{
    CaretAssert(length > 0);
    m_maps.resize(length);
}

void CiftiLabelsMap::setMapName(const int64_t& index, const QString& mapName) const
{
    CaretAssertVectorIndex(m_maps, index);
    m_maps[index].m_name = mapName;
}

bool CiftiLabelsMap::approximateMatch(const CiftiMappingType& rhs) const
{
    switch (rhs.getType())
    {
        case SCALARS:
        case SERIES://maybe?
        case LABELS:
            return getLength() == rhs.getLength();
        default:
            return false;
    }
}

bool CiftiLabelsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiLabelsMap& myrhs = dynamic_cast<const CiftiLabelsMap&>(rhs);
    return (m_maps == myrhs.m_maps);
}

bool CiftiLabelsMap::LabelMap::operator==(const LabelMap& rhs) const
{
    if (m_name != rhs.m_name) return false;
    if (m_labelTable != rhs.m_labelTable) return false;
    return (m_metaData == rhs.m_metaData);
}

void CiftiLabelsMap::readXML1(QXmlStreamReader& xml)
{
    CaretLogFiner("parsing nonstandard labels mapping type in cifti-1");
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CaretException("unexpected element in labels mapping type: " + xml.name().toString());
                }
                LabelMap tempMap;
                tempMap.readXML1(xml);
                if (xml.hasError()) return;
                m_maps.push_back(tempMap);
                break;
            }
            default:
                break;
        }
    }
}

void CiftiLabelsMap::readXML2(QXmlStreamReader& xml)
{
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CaretException("unexpected element in labels mapping type: " + xml.name().toString());
                }
                LabelMap tempMap;
                tempMap.readXML2(xml);
                if (xml.hasError()) return;
                m_maps.push_back(tempMap);
                break;
            }
            default:
                break;
        }
    }
}

void CiftiLabelsMap::LabelMap::readXML1(QXmlStreamReader& xml)
{
    bool haveName = false, haveTable = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CaretException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML1(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CaretException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readFromQXmlStreamReader(xml);
                    if (xml.hasError()) return;
                    haveTable = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CaretException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CaretException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw CaretException("NamedMap missing required child element MapName");
    }
    if (!haveTable)
    {
        throw CaretException("NamedMap in labels mapping missing required child element LabelTable");
    }
}

void CiftiLabelsMap::LabelMap::readXML2(QXmlStreamReader& xml)
{
    bool haveName = false, haveTable = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CaretException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML2(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CaretException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readFromQXmlStreamReader(xml);
                    if (xml.hasError()) return;
                    haveTable = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CaretException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CaretException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw CaretException("NamedMap missing required child element MapName");
    }
    if (!haveTable)
    {
        throw CaretException("NamedMap in labels mapping missing required child element LabelTable");
    }
}

void CiftiLabelsMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretLogFiner("writing nonstandard labels mapping type in cifti-1");
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_LABELS");
    int64_t numMaps = (int64_t)m_maps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        m_maps[i].m_labelTable.writeAsXML(xml);
        xml.writeEndElement();
    }
}

void CiftiLabelsMap::writeXML2(QXmlStreamWriter& xml) const
{
    int64_t numMaps = (int64_t)m_maps.size();
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_LABELS");
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML2(xml);
        m_maps[i].m_labelTable.writeAsXML(xml);
        xml.writeEndElement();
    }
}
