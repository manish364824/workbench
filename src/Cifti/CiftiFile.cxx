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

#include "CiftiFile.h"

#include "ByteOrderEnum.h"
#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "MultiDimArray.h"
#include "MultiDimIterator.h"
#include "NiftiIO.h"

using namespace std;
using namespace caret;

//private implementation classes
namespace caret
{
    class CiftiOnDiskImpl : public CiftiFile::WriteImplInterface
    {
        mutable NiftiIO m_nifti;//because file objects aren't stateless (current position), so reading "changes" them
        CiftiXML m_xml;//because we need to parse it to set up the dimensions anyway
    public:
        CiftiOnDiskImpl(const QString& filename);//read-only
        CiftiOnDiskImpl(const QString& filename, const CiftiXML& xml, const CiftiVersion& version);//make new empty file with read/write
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const;
        void getColumn(float* dataOut, const int64_t& index) const;
        const CiftiXML& getCiftiXML() const { return m_xml; }
        QString getFilename() const { return m_nifti.getFilename(); }
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);
    };
    
    class CiftiMemoryImpl : public CiftiFile::WriteImplInterface
    {
        MultiDimArray<float> m_array;
    public:
        CiftiMemoryImpl(const CiftiXML& xml);
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const;
        void getColumn(float* dataOut, const int64_t& index) const;
        bool isInMemory() const { return true; }
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);
    };
    
    class CiftiXnatImpl : public CiftiFile::ReadImplInterface
    {
        CiftiXML m_xml;//because we need to parse it to check the dimensions anyway
        CaretHttpRequest m_baseRequest;
        void init(const QString& url);
        void getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) const;
        int64_t getSizeFromReq(CaretHttpRequest& request);
    public:
        CiftiXnatImpl(const QString& url, const QString& user, const QString& pass);
        CiftiXnatImpl(const QString& url);//reuse existing user/pass, or access non-protected url - in the future, maybe only the second use (private http manager)
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const;
        void getColumn(float* dataOut, const int64_t& index) const;
        const CiftiXML& getCiftiXML() const { return m_xml; }
    };
}

CiftiFile::ReadImplInterface::~ReadImplInterface()
{
}

CiftiFile::WriteImplInterface::~WriteImplInterface()
{
}

CiftiFile::CiftiFile(const QString& fileName)
{
    openFile(fileName);
}

void CiftiFile::openFile(const QString& fileName)
{
    m_writingImpl.grabNew(NULL);
    m_readingImpl.grabNew(NULL);//to make sure it closes everything first, even if the open throws
    m_dims.clear();
    CaretPointer<CiftiOnDiskImpl> newRead(new CiftiOnDiskImpl(FileInformation(fileName).getAbsoluteFilePath()));//this constructor opens existing file read-only
    m_readingImpl = newRead;//it should be noted that if the constructor throws (if the file isn't readable), new guarantees the memory allocated for the object will be freed
    m_xml = newRead->getCiftiXML();
    m_dims = m_xml.getDimensions();
}

void CiftiFile::openURL(const QString& url, const QString& user, const QString& pass)
{
    m_writingImpl.grabNew(NULL);
    m_readingImpl.grabNew(NULL);//to make sure it closes everything first, even if the open throws
    m_dims.clear();
    CaretPointer<CiftiXnatImpl> newRead(new CiftiXnatImpl(url, user, pass));
    m_readingImpl = newRead;
    m_xml = newRead->getCiftiXML();
    m_dims = m_xml.getDimensions();
}

void CiftiFile::openURL(const QString& url)
{
    m_writingImpl.grabNew(NULL);
    m_readingImpl.grabNew(NULL);//to make sure it closes everything first, even if the open throws
    m_dims.clear();
    CaretPointer<CiftiXnatImpl> newRead(new CiftiXnatImpl(url));
    m_readingImpl = newRead;
    m_xml = newRead->getCiftiXML();
    m_dims = m_xml.getDimensions();
}

void CiftiFile::setWritingFile(const QString& fileName)
{
    m_writingFile = FileInformation(fileName).getAbsoluteFilePath();//always resolve paths as soon as they enter CiftiFile, in case some clown changes directory before writing data
    m_writingImpl.grabNew(NULL);//prevent writing to previous writing implementation, let the next set...() set up for writing
}

void CiftiFile::writeFile(const QString& fileName, const CiftiVersion& writingVersion)
{
    if (m_readingImpl == NULL || m_dims.empty()) throw DataFileException("writeFile called on uninitialized CiftiFile");
    FileInformation myInfo(fileName);
    QString canonicalFilename = myInfo.getCanonicalFilePath();//NOTE: returns EMPTY STRING for nonexistant file
    CaretPointer<ReadImplInterface> tempRead = m_readingImpl;
    const CiftiOnDiskImpl* testImpl = dynamic_cast<CiftiOnDiskImpl*>(m_readingImpl.getPointer());
    bool collision = false;
    if (testImpl != NULL && canonicalFilename != "" && FileInformation(testImpl->getFilename()).getCanonicalFilePath() == canonicalFilename)
    {//empty string test is so that we don't say collision if both are nonexistant - could happen if file is removed/unlinked while reading on some filesystems
        if (m_writingVersion == writingVersion) return;//don't need to copy to itself
        collision = true;//we need to copy to memory temporarily
        CaretPointer<WriteImplInterface> tempMemory(new CiftiMemoryImpl(m_xml));//because tempRead is a ReadImpl, can't be used to copy to
        copyImplData(m_readingImpl, tempMemory, m_dims);
        tempRead = tempMemory;//set it to read from the memory rather than m_readingImpl
    }
    CaretPointer<WriteImplInterface> tempWrite(new CiftiOnDiskImpl(myInfo.getAbsoluteFilePath(), m_xml, writingVersion));//NOTE: this makes m_readingImpl/m_writingImpl unusable if collision is true!
    copyImplData(tempRead, tempWrite, m_dims);
    if (collision)//if we rewrote the file, we need the handle to the new file, the old one has the wrong version and vox_offset in it
    {
        m_writingVersion = writingVersion;//also record the current version number
        if (m_writingImpl != NULL)//NULL can happen if setWritingFile is called with a name other than the current file, then writeFile is called with the same name as current file but different version
        {//or, if it is in read-only on-disk mode
            m_writingImpl = tempWrite;//replace the now-unusable old file implementation
        }
        m_readingImpl = tempWrite;//replace the now-unusable old file implementation
    }
}

void CiftiFile::writeFile(const QString& fileName)
{
    writeFile(fileName, m_writingVersion);//let the more complex case handle the simple one too, will always return early on collision
}

void CiftiFile::convertToInMemory()
{
    if (isInMemory()) return;
    if (m_readingImpl == NULL || m_dims.empty())//not set up yet
    {
        m_writingFile = "";//make sure it doesn't do on-disk when set...() is called
        return;
    }
    CaretPointer<WriteImplInterface> tempWrite(new CiftiMemoryImpl(m_xml));//if we get an error while reading, free the memory immediately, and don't leave m_readingImpl and m_writingImpl pointing to different things
    copyImplData(m_readingImpl, tempWrite, m_dims);
    m_writingImpl = tempWrite;
    m_readingImpl = tempWrite;
}

bool CiftiFile::isInMemory() const
{
    if (m_readingImpl == NULL)
    {
        return (m_writingFile == "");//return what it would be if verifyImpl() was called
    } else {
        return m_readingImpl->isInMemory();
    }
}

void CiftiFile::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool& tolerateShortRead) const
{
    if (m_dims.empty()) throw DataFileException("getRow called on uninitialized CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    m_readingImpl->getRow(dataOut, indexSelect, tolerateShortRead);
}

void CiftiFile::getColumn(float* dataOut, const int64_t& index) const
{
    if (m_dims.empty()) throw DataFileException("getColumn called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw DataFileException("getColumn called on non-2D CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    m_readingImpl->getColumn(dataOut, index);
}

void CiftiFile::setCiftiXML(const CiftiXML& xml, const bool useOldMetadata, const CiftiVersion& writingVersion)
{
    if (xml.getNumberOfDimensions() == 0) throw DataFileException("setCiftiXML called with 0-dimensional CiftiXML");
    m_writingVersion = writingVersion;
    if (useOldMetadata)
    {
        const GiftiMetaData* oldmd = m_xml.getFileMetaData();
        if (oldmd != NULL)
        {
            GiftiMetaData newmd = *oldmd;//make a copy
            oldmd = NULL;//don't leave a potentially dangling pointer around
            m_xml = xml;//because this will result in a new pointer for the metadata
            GiftiMetaData* changemd = m_xml.getFileMetaData();
            if (changemd != NULL)
            {
                *changemd = newmd;
            }
        } else {
            m_xml = xml;
        }
    } else {
        m_xml = xml;
    }
    m_dims = m_xml.getDimensions();
    m_readingImpl.grabNew(NULL);//drop old implementation, as it is now invalid due to XML (and therefore matrix size) change
    m_writingImpl.grabNew(NULL);
}

void CiftiFile::setCiftiXML(const CiftiXMLOld& xml, const bool useOldMetadata, const CiftiVersion& writingVersion)
{
    QString xmlText;
    xml.writeXML(xmlText);
    CiftiXML tempXML;//so that we can use the same code path
    tempXML.readXML(xmlText);
    if (tempXML.getDimensionLength(CiftiXML::ALONG_ROW) < 1)
    {
        CiftiSeriesMap& tempMap = tempXML.getSeriesMap(CiftiXML::ALONG_ROW);
        tempMap.setLength(xml.getDimensionLength(CiftiXMLOld::ALONG_ROW));
    }
    if (tempXML.getDimensionLength(CiftiXML::ALONG_COLUMN) < 1)
    {
        CiftiSeriesMap& tempMap = tempXML.getSeriesMap(CiftiXML::ALONG_COLUMN);
        tempMap.setLength(xml.getDimensionLength(CiftiXMLOld::ALONG_COLUMN));
    }
    setCiftiXML(tempXML, useOldMetadata, writingVersion);
}

void CiftiFile::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    verifyWriteImpl();
    m_writingImpl->setRow(dataIn, indexSelect);
}

void CiftiFile::setColumn(const float* dataIn, const int64_t& index)
{
    verifyWriteImpl();
    if (m_dims.size() != 2) throw DataFileException("setColumn called on non-2D CiftiFile");
    m_writingImpl->setColumn(dataIn, index);
}

//compatibility with old interface
void CiftiFile::getRow(float* dataOut, const int64_t& index, const bool& tolerateShortRead) const
{
    if (m_dims.empty()) throw DataFileException("getRow called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw DataFileException("getRow with single index called on non-2D CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    vector<int64_t> tempvec(1, index);//could use a member if we need more speed
    m_readingImpl->getRow(dataOut, tempvec, tolerateShortRead);
}

void CiftiFile::getRow(float* dataOut, const int64_t& index) const
{
    getRow(dataOut, index, false);//once CiftiInterface is gone, we can collapse this into a default value
}

int64_t CiftiFile::getNumberOfRows() const
{
    if (m_dims.empty()) throw DataFileException("getNumberOfRows called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw DataFileException("getNumberOfRows called on non-2D CiftiFile");
    return m_dims[1];//length of a column
}

int64_t CiftiFile::getNumberOfColumns() const
{
    if (m_dims.empty()) throw DataFileException("getNumberOfRows called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw DataFileException("getNumberOfRows called on non-2D CiftiFile");
    return m_dims[0];//length of a row
}

void CiftiFile::setRow(const float* dataIn, const int64_t& index)
{
    verifyWriteImpl();
    if (m_dims.size() != 2) throw DataFileException("setRow with single index called on non-2D CiftiFile");
    vector<int64_t> tempvec(1, index);//could use a member if we need more speed
    m_writingImpl->setRow(dataIn, tempvec);
}
//*///end old compatibility functions

void CiftiFile::verifyWriteImpl()
{//this is where the magic happens - we want to emulate being a simple in-memory file, but actually be reading/writing on-disk when possible
    if (m_writingImpl != NULL) return;
    CaretAssert(!m_dims.empty());//if the xml hasn't been set, then we can't do anything meaningful
    if (m_dims.empty()) throw DataFileException("setRow or setColumn attempted on uninitialized CiftiFile");
    if (m_writingFile == "")
    {
        if (m_readingImpl != NULL)
        {
            convertToInMemory();
        } else {
            m_writingImpl.grabNew(new CiftiMemoryImpl(m_xml));
        }
    } else {
        if (m_readingImpl != NULL)
        {
            CiftiOnDiskImpl* testImpl = dynamic_cast<CiftiOnDiskImpl*>(m_readingImpl.getPointer());
            if (testImpl != NULL)
            {
                QString canonicalCurrent = FileInformation(testImpl->getFilename()).getCanonicalFilePath();//returns "" if nonexistant, if unlinked while open
                if (canonicalCurrent != "" && canonicalCurrent == FileInformation(m_writingFile).getCanonicalFilePath())//these were already absolute
                {
                    convertToInMemory();//save existing data in memory before we clobber file
                }
            }
        }
        m_writingImpl.grabNew(new CiftiOnDiskImpl(m_writingFile, m_xml, m_writingVersion));//this constructor makes new file for writing
        if (m_readingImpl != NULL)
        {
            copyImplData(m_readingImpl, m_writingImpl, m_dims);
        }
    }
    m_readingImpl = m_writingImpl;//read-only implementations are set up in specialized functions
}

void CiftiFile::copyImplData(const ReadImplInterface* from, WriteImplInterface* to, const vector<int64_t>& dims)
{
    vector<int64_t> iterateDims(dims.begin() + 1, dims.end());
    vector<float> scratchRow(dims[0]);
    for (MultiDimIterator<int64_t> iter(iterateDims); !iter.atEnd(); ++iter)
    {
        from->getRow(scratchRow.data(), *iter, false);
        to->setRow(scratchRow.data(), *iter);
    }
}

CiftiMemoryImpl::CiftiMemoryImpl(const CiftiXML& xml)
{
    CaretAssert(xml.getNumberOfDimensions() != 0);
    m_array.resize(xml.getDimensions());
}

void CiftiMemoryImpl::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool&) const
{
    const float* ref = m_array.get(1, indexSelect);
    int64_t rowSize = m_array.getDimensions()[0];//we don't accept 0-D CiftiXML, so this will always work
    for (int64_t i = 0; i < rowSize; ++i)
    {
        dataOut[i] = ref[i];
    }
}

void CiftiMemoryImpl::getColumn(float* dataOut, const int64_t& index) const
{
    CaretAssert(m_array.getDimensions().size() == 2);//otherwise, CiftiFile shouldn't have called this
    const float* ref = m_array.get(2, vector<int64_t>());//empty vector is intentional, only 2 dimensions exist, so no more to select from
    int64_t rowSize = m_array.getDimensions()[0];
    int64_t colSize = m_array.getDimensions()[1];
    CaretAssert(index >= 0 && index < rowSize);//because we are doing the indexing math manually for speed
    for (int64_t i = 0; i < colSize; ++i)
    {
        dataOut[i] = ref[index + rowSize * i];
    }
}

void CiftiMemoryImpl::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    float* ref = m_array.get(1, indexSelect);
    int64_t rowSize = m_array.getDimensions()[0];//we don't accept 0-D CiftiXML, so this will always work
    for (int64_t i = 0; i < rowSize; ++i)
    {
        ref[i] = dataIn[i];
    }
}

void CiftiMemoryImpl::setColumn(const float* dataIn, const int64_t& index)
{
    CaretAssert(m_array.getDimensions().size() == 2);//otherwise, CiftiFile shouldn't have called this
    float* ref = m_array.get(2, vector<int64_t>());//empty vector is intentional, only 2 dimensions exist, so no more to select from
    int64_t rowSize = m_array.getDimensions()[0];
    int64_t colSize = m_array.getDimensions()[1];
    CaretAssert(index >= 0 && index < rowSize);//because we are doing the indexing math manually for speed
    for (int64_t i = 0; i < colSize; ++i)
    {
        ref[index + rowSize * i] = dataIn[i];
    }
}

CiftiOnDiskImpl::CiftiOnDiskImpl(const QString& filename)
{//opens existing file for reading
    m_nifti.openRead(filename);//read-only, so we don't need write permission to read a cifti file
    const NiftiHeader& myHeader = m_nifti.getHeader();
    int numExts = (int)myHeader.m_extensions.size(), whichExt = -1;
    for (int i = 0; i < numExts; ++i)
    {
        if (myHeader.m_extensions[i]->m_ecode == NIFTI_ECODE_CIFTI)
        {
            whichExt = i;
            break;
        }
    }
    if (whichExt == -1) throw DataFileException("no cifti extension found in file '" + filename + "'");
    m_xml.readXML(QByteArray(myHeader.m_extensions[whichExt]->m_bytes.data(), myHeader.m_extensions[whichExt]->m_bytes.size()));//CiftiXML should be under 2GB
    vector<int64_t> dimCheck = m_nifti.getDimensions();
    if (dimCheck.size() < 5) throw DataFileException("invalid dimensions in cifti file '" + filename + "'");
    for (int i = 0; i < 4; ++i)
    {
        if (dimCheck[i] != 1) throw DataFileException("non-singular dimension #" + QString::number(i + 1) + " in cifti file '" + filename + "'");
    }
    if (m_xml.getParsedVersion().hasReversedFirstDims())
    {
        while (dimCheck.size() < 6) dimCheck.push_back(1);//just in case
        int64_t temp = dimCheck[4];//note: nifti dim[5] is the 5th dimension, index 4 in this vector
        dimCheck[4] = dimCheck[5];
        dimCheck[5] = temp;
        m_nifti.overrideDimensions(dimCheck);
    }
    if (m_xml.getNumberOfDimensions() + 4 != (int)dimCheck.size()) throw DataFileException("XML does not match number of nifti dimensions in file " + filename + "'");
    for (int i = 4; i < (int)dimCheck.size(); ++i)
    {
        if (m_xml.getDimensionLength(i - 4) < 1)//CiftiXML will only let this happen with cifti-1
        {
            m_xml.getSeriesMap(i - 4).setLength(dimCheck[i]);//and only in a series map
        } else {
            if (m_xml.getDimensionLength(i - 4) != dimCheck[i])
            {
                throw DataFileException("xml and nifti header disagree on matrix dimensions");
            }
        }
    }
}

CiftiOnDiskImpl::CiftiOnDiskImpl(const QString& filename, const CiftiXML& xml, const CiftiVersion& version)
{//starts writing new file
    NiftiHeader outHeader;
    outHeader.setDataType(NIFTI_TYPE_FLOAT32);//actually redundant currently, default is float32
    char intentName[16];
    int32_t intentCode = xml.getIntentInfo(version, intentName);
    outHeader.setIntent(intentCode, intentName);
    QByteArray xmlBytes = xml.writeXMLToQByteArray(version);
    CaretPointer<NiftiExtension> outExtension(new NiftiExtension());
    outExtension->m_ecode = NIFTI_ECODE_CIFTI;
    int numBytes = xmlBytes.size();
    outExtension->m_bytes.resize(numBytes);
    for (int i = 0; i < numBytes; ++i)
    {
        outExtension->m_bytes[i] = xmlBytes[i];
    }
    outHeader.m_extensions.push_back(outExtension);
    vector<int64_t> matrixDims = xml.getDimensions();
    vector<int64_t> niftiDims(4, 1);//the reserved space and time dims
    niftiDims.insert(niftiDims.end(), matrixDims.begin(), matrixDims.end());
    if (version.hasReversedFirstDims())
    {
        vector<int64_t> headerDims = niftiDims;
        while (headerDims.size() < 6) headerDims.push_back(1);//just in case
        int64_t temp = headerDims[4];
        headerDims[4] = headerDims[5];
        headerDims[5] = temp;
        outHeader.setDimensions(headerDims);//give the header the reversed dimensions
        m_nifti.writeNew(filename, outHeader, 2, true);
        m_nifti.overrideDimensions(niftiDims);//and then tell the nifti reader to use the correct dimensions
    } else {
        outHeader.setDimensions(niftiDims);
        m_nifti.writeNew(filename, outHeader, 2, true);
    }
    m_xml = xml;
}

void CiftiOnDiskImpl::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool& tolerateShortRead) const
{
    m_nifti.readData(dataOut, 5, indexSelect, tolerateShortRead);//5 means 4 reserved (space and time) plus the first cifti dimension
}

void CiftiOnDiskImpl::getColumn(float* dataOut, const int64_t& index) const
{
    CaretAssert(m_xml.getNumberOfDimensions() == 2);//otherwise this shouldn't be called
    CaretAssert(index >= 0 && index < m_xml.getDimensionLength(CiftiXML::ALONG_ROW));
    CaretLogFine("getColumn called on CiftiOnDiskImpl, this will be slow");//generate logging messages at a low priority
    vector<int64_t> indexSelect(2);
    indexSelect[0] = index;
    int64_t colLength = m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int64_t i = 0; i < colLength; ++i)//assume if they really want getColumn on disk, they don't want their pagecache obliterated, so read it 1 element at a time
    {
        indexSelect[1] = i;
        m_nifti.readData(dataOut + i, 4, indexSelect);//4 means just the 4 reserved dimensions, so 1 element of the matrix
    }
}

void CiftiOnDiskImpl::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    m_nifti.writeData(dataIn, 5, indexSelect);
}

void CiftiOnDiskImpl::setColumn(const float* dataIn, const int64_t& index)
{
    CaretAssert(m_xml.getNumberOfDimensions() == 2);//otherwise this shouldn't be called
    CaretAssert(index >= 0 && index < m_xml.getDimensionLength(CiftiXML::ALONG_ROW));
    CaretLogFine("getColumn called on CiftiOnDiskImpl, this will be slow");//generate logging messages at a low priority
    vector<int64_t> indexSelect(2);
    indexSelect[0] = index;
    int64_t colLength = m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int64_t i = 0; i < colLength; ++i)//don't do RMW, so write it 1 element at a time
    {
        indexSelect[1] = i;
        m_nifti.writeData(dataIn + i, 4, indexSelect);//4 means just the 4 reserved dimensions, so 1 element of the matrix
    }
}

CiftiXnatImpl::CiftiXnatImpl(const QString& url, const QString& user, const QString& pass)
{
    CaretHttpManager::setAuthentication(url, user, pass);
    init(url);
}

CiftiXnatImpl::CiftiXnatImpl(const QString& url)
{
    init(url);
}

void CiftiXnatImpl::init(const QString& url)
{
    m_baseRequest.m_url = url;
    m_baseRequest.m_method = CaretHttpManager::POST;
    int32_t start = url.indexOf('?');
    bool foundSearchID = false;
    bool foundResource = false;
    while ((!foundSearchID) && (!foundResource))
    {
        if (start == -1)
        {
            throw DataFileException("Error: searchID not found in URL string");
        }
        if (url.mid(start + 1, 9) == "searchID=")
        {
            foundSearchID = true;
        } else if (url.mid(start + 1, 9) == "resource=") {
            foundResource = true;
        }
        start = url.indexOf('&', start + 1);
    }
    m_baseRequest.m_queries.push_back(make_pair(AString("type"), AString("dconn")));
    CaretHttpRequest metadata = m_baseRequest;
    if (foundResource)
    {
        metadata.m_queries.push_back(make_pair(AString("metadata"), AString("true")));
    } else {
        metadata.m_queries.push_back(make_pair(AString("metadata"), AString("")));
    }
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(metadata, myResponse);
    if (!myResponse.m_ok)
    {
        throw DataFileException("Error opening URL, response code: " + AString::number(myResponse.m_responseCode));
    }
    myResponse.m_body.push_back('\0');//null terminate it so we can construct an AString easily - CaretHttpManager is nice and pre-reserves this room for this purpose
    AString theBody(myResponse.m_body.data());
    m_xml.readXML(theBody);
    if (m_xml.getNumberOfDimensions() != 2)
    {
        throw DataFileException("only 2D cifti are supported via URL at this time");
    }
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SERIES && m_xml.getDimensionLength(CiftiXML::ALONG_ROW) < 1)
    {
        CaretHttpRequest rowRequest = m_baseRequest;
        rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString("0")));
        m_xml.getSeriesMap(CiftiXML::ALONG_COLUMN).setLength(getSizeFromReq(rowRequest));//number of timepoints along a row is the number of columns
    }
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES && m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN) < 1)
    {
        CaretHttpRequest columnRequest = m_baseRequest;
        columnRequest.m_queries.push_back(make_pair(AString("column-index"), AString("0")));
        m_xml.getSeriesMap(CiftiXML::ALONG_ROW).setLength(getSizeFromReq(columnRequest));//see above
    }
    CaretLogFine("Connected URL: "
                   + url
                   + "\nRow/Column length:"
                   + QString::number(m_xml.getDimensionLength(CiftiXML::ALONG_ROW))
                   + "/"
                   + QString::number(m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN)));
}

void CiftiXnatImpl::getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) const
{
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(request, myResponse);
    if (!myResponse.m_ok)
    {
        throw DataFileException("Error getting row, response code: " + AString::number(myResponse.m_responseCode));
    }
    if (myResponse.m_body.size() % 4 != 0)//expect a multiple of 4 bytes
    {
        throw DataFileException("Bad reply, number of bytes is not a multiple of 4");
    }
    int32_t numItems = *((int32_t*)myResponse.m_body.data());
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swap(numItems);
    }
    if (numItems * 4 + 4 != (int64_t)myResponse.m_body.size())
    {
        throw DataFileException("Bad reply, number of items does not match length of reply");
    }
    if (dataSize != numItems)
    {
        throw DataFileException("Bad reply, number of items does not match header");
    }
    float* myPointer = (float*)(myResponse.m_body.data() + 4);//skip the first element (which is an int32)
    for (int i = 0; i < numItems; ++i)
    {
        data[i] = myPointer[i];
    }
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapArray(data, numItems);
    }
}

int64_t CiftiXnatImpl::getSizeFromReq(CaretHttpRequest& request)
{
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(request, myResponse);
    if (!myResponse.m_ok)
    {
        throw DataFileException("Error getting row, response code: " + AString::number(myResponse.m_responseCode));
    }
    if (myResponse.m_body.size() % 4 != 0)//expect a multiple of 4 bytes
    {
        throw DataFileException("Bad reply, number of bytes is not a multiple of 4");
    }
    int32_t numItems = *((int32_t*)myResponse.m_body.data());
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swap(numItems);
    }
    if (numItems * 4 + 4 != (int64_t)myResponse.m_body.size())
    {
        throw DataFileException("Bad reply, number of items does not match length of reply");
    }
    return numItems;
}

void CiftiXnatImpl::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool&) const
{
    CaretAssert(indexSelect.size() == 1);
    CaretHttpRequest rowRequest = m_baseRequest;
    rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString::number(indexSelect[0])));
    getReqAsFloats(dataOut, m_xml.getDimensionLength(CiftiXML::ALONG_ROW), rowRequest);
}

void CiftiXnatImpl::getColumn(float* dataOut, const int64_t& index) const
{
    CaretHttpRequest columnRequest = m_baseRequest;
    columnRequest.m_queries.push_back(make_pair(AString("column-index"), AString::number(index)));
    getReqAsFloats(dataOut, m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN), columnRequest);
}
