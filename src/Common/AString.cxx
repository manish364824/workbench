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

#include <QTextStream>

#include "AString.h"
#include "CaretLogger.h"
#include <iostream>

using namespace caret;

std::ostream& operator << (std::ostream &lhs, const AString &rhs) 
{ 
    return lhs << rhs.toStdString(); 
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int8_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint8_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}


/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}


/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<float>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<double>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const float* array, const int64_t numberOfElements, const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int8_t* array,
                     const int64_t numberOfElements,
                     const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const uint8_t* array,
                     const int64_t numberOfElements,
                     const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}


/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int32_t* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int64_t* array,
                     const int64_t numberOfElements,
                     const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const double* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert the contents of given string to floats.  Each 
 * piece of text is converted to float.  If a piece of 
 * text does not convert to a float, it is ignored.  This
 * should allow separation with characters other than
 * whitespace.
 *
 * @param s
 *     String convert to floats.
 * @param numbersOut
 *    Vector that will contain the given of this string
 *    as numbers.
 */
void 
AString::toNumbers(const AString& s,
                   std::vector<float>& numbersOut)
{
    AString copy = s;
    QTextStream stream(&copy);
    
    float floatValue;
    while (stream.atEnd() == false) {
        /*
         * Try to read a float value from the current position
         */
        stream >> floatValue;
        
        /*
         * If the text stream could not create a float from
         * the current text position, the corrupt data flag
         * will be set.
         */
        if (stream.status() == QTextStream::ReadCorruptData) {
            /*
             * Reset the status of the string (to OK) and
             * then read one character to remove the character
             * from the stream since it was not the start
             * of a number.
             */
            stream.resetStatus();
            QChar oneChar;
            stream >> oneChar;
        }
        else {
            numbersOut.push_back(floatValue);
        }
    }

//    AString copy = s;
//    QTextStream stream(&copy);
//    
//    AString numberString;
//    bool valid = false;
//    while (stream.atEnd() == false) {
//        stream >> numberString;
//        const float floatValue = numberString.toFloat(&valid);
//        if (valid) {
//            numbersOut.push_back(floatValue);
//        }
//    }
}

/**
 * Convert the contents of given string to ints.  Each 
 * piece of text is converted to int.  If a piece of 
 * text does not convert to an int, it is ignored.  This
 * should allow separation with characters other than
 * whitespace.
 *
 * @param s
 *     String convert to ints.
 * @param numbersOut
 *    Vector that will contain the given of this string
 *    as numbers.
 */
void 
AString::toNumbers(const AString& s,
                   std::vector<int32_t>& numbersOut)
{
    AString copy = s;
    QTextStream stream(&copy);
    
    AString numberString;
    bool valid = false;
    while (stream.atEnd() == false) {
        stream >> numberString;
        const int32_t intValue = numberString.toInt(&valid);
        if (valid) {
            numbersOut.push_back(intValue);
        }
    }
}

/**
 * Convert the string to a boolean value.
 * These case insensitive values are considered true:
 * true, t, 1, 1.0.  All others are considered false.
 *
 * @return 
 *    Boolean value interpreted from contents of 
 *    this string.
 */
bool 
AString::toBool() const
{
    const AString s = this->toLower();
    if ((s == "true")
        || (s == "t") 
        || (s == "1")
        || (s == "1.0")) {
        return true;
    }
    
    return false;
}


/**
 * Convert the boolean value to a string.
 * @param b
 *    The boolean value.
 * @return "true" if true, else "false".
 */
AString 
AString::fromBool(const bool b)
{
    if (b) {
        return "true";
    }
    return "false";
}

/**
 * Convert any URLs in this string to  
 * HTML hyperlinks.
 * "http://www.wustl.edu" becomes "<a href=http://www.wustl.edu>http://wwww.wustl.edu</a>"
 * @param sin
 *    String that may contain URLs.
 * @return
 *    Input string with any URLs replace with hyperlinks.
 */
AString 
AString::convertURLsToHyperlinks() const
{
    std::vector<AString> url;
    std::vector<int> urlStart;
    
    const AString& sin = *this;
    
    if (sin.indexOf("http://") == -1) {
        return sin;
    }
    else {
        //
        // Create a modifiable copy
        //
        AString s(sin);
        
        //
        // loop since there may be more than one URL
        //
        bool done = false;
        int startPos = 0;
        while(done == false) {
            //
            // Find the beginning of the URL 
            //
            const int httpStart = s.indexOf("http://", startPos); 
            
            //
            // Was the start of a URL found
            //
            if (httpStart == -1) {
                done = true;
            }
            else {
                //
                // Find the end of the URL
                //
                int httpEnd = s.indexOfAnyChar(" \t\n\r", httpStart + 1);
                
                //
                // May not find end since end of string
                //
                int httpLength;
                if (httpEnd == -1) {
                    httpLength = s.length() - httpStart;
                }
                else {
                    httpLength = httpEnd - httpStart;
                }
                
                //
                // Get the http URL
                //
                const AString httpString = s.mid(httpStart, httpLength);
                url.push_back(httpString);
                urlStart.push_back(httpStart);
                
                //
                // Prepare for next search
                //
                startPos = httpStart;
                //if (startPos > 0) {
                startPos = startPos + 1;
                //}
            }
        }
        
        if (url.empty() == false) {
            const int startNum = static_cast<int>(url.size()) - 1;
            for (int i = startNum; i >= 0; i--) {
                const int len = url[i].length();
                
                //
                // Create the trailing part of the hyperlink and insert it
                //
                AString trailingHyperLink("\">");
                trailingHyperLink.append(url[i]);
                trailingHyperLink.append("</a>");
                s.insert(urlStart[i] + len, trailingHyperLink);
                
                //
                // Insert the beginning of the hyperlink
                //
                s.insert(urlStart[i], " <a href=\"");
            }
        }
        return s;
    }
}

/**
 * Convert the text string to an HTML page by enclosing text between:
 *    "<html><head></head><body>" and "</body></html>"
 *
 *    Replace some characters with their HTML escaped characters 
 */
AString
AString::convertToHtmlPage() const
{
    return convertToHtmlPageWithCssFontHeight(-1);
}

/**
 * Convert the text string to an HTML page using the given font size
 * by enclosing text between:
 *    "<html><head></head><body><font size="X">" and "</font></body></html>"
 *
 * Note: This uses the font tag's size attribute which is not supported
 * by HTML5.
 *
 * Also replaces some characters with their HTML escaped characters
 *
 * @param fontSize
 *    Size of the font.
 */
AString
AString::convertToHtmlPageWithFontSize(const int fontSize) const
{
    /*
     * If already HTML (assumes "html" is the first six characters),
     * no need to convert.
     */
    if (this->startsWith("<html>",
                         Qt::CaseInsensitive)) {
        return *this;
    }
    
    AString htmlString("<html><head></head><body>");
    
    htmlString.append("<font size=\"" + AString::number(fontSize) + "\">");
    
    htmlString.append(this->replaceHtmlSpecialCharactersWithEscapeCharacters());
    
    htmlString.append("</font>");
    
    htmlString.append("</body></html>");
    
    return htmlString;    
}

/**
 * Convert the text string to an HTML page using the given font height
 * by enclosing text between:
 *    "<html><head></head><body><p style="font-size:Xpx>" and "</p></body></html>"
 *
 * Note: HTML produced by this method and displayed in a QTextBrowser had
 * some problems with cutting and pasting on some Macs not working.
 *
 * Also replaces some characters with their HTML escaped characters
 *
 * @param fontHeight
 *    Height of the font (if negative no font height is applied).
 */
AString
AString::convertToHtmlPageWithCssFontHeight(const int fontHeight) const
{
    /*
     * If already HTML (assumes "html" is the first six characters),
     * no need to convert.
     */
    if (this->startsWith("<html>",
                         Qt::CaseInsensitive)) {
        return *this;
    }
    
    AString htmlString("<html><head></head><body>");
    
    if (fontHeight > 0) {
        htmlString.append("<p style=\"font-size:" + AString::number(fontHeight) + "px\">");
    }
    
    htmlString.append(this->replaceHtmlSpecialCharactersWithEscapeCharacters());
    
    if (fontHeight > 0) {
        htmlString.append("</p>");
    }
    
    htmlString.append("</body></html>");
    
    return htmlString;    
}

/**
 * @return A copy of this string with any HTML special characters replaced
 * by their escape sequences.
 */
AString
AString::replaceHtmlSpecialCharactersWithEscapeCharacters() const
{
    AString htmlString;
    
    const int64_t length = this->count();
    for (int64_t i = 0; i < length; i++) {
        const QChar ch = this->at(i);
        switch (ch.toAscii()) {
            case '&':
                htmlString.append("&amp;");
                break;
            case '<':
                htmlString.append("&lt;");
                break;
            case '>':
                htmlString.append("&gt;");
                break;
            case '\'':
                htmlString.append("&apos;");
                break;
            case '\"':
                htmlString.append("&quot;");
                break;
            case ' ':
                htmlString.append("&nbsp;");
                break;
            case '\n':
                htmlString.append("<br>");
                break;
            default:
                htmlString.append(ch);
                break;
        }
    }
    
    return htmlString;
}

/**
 * Returns the index position of any character in
 * 'str' in this string.
 * @param str  Characters that are searched
 *    for in this string.
 * @param from String position (default is first character).
 */
int32_t
AString::indexOfAnyChar(const AString& str,
                        int from) const
{
    const AString& s = *this;
    const int len = s.length();
    if (from < 0)//use the same "from" logic as Qt
    {
        from += len;//-2 starts at second to last character
        if (from < 0) from = 0;//can't start before the beginning
    }
    const int len2 = str.length();
    for (int i = from; i < len; i++) {
        for (int j = 0; j < len2; j++) {
            if (s[i] == str[j]) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * @return The index position of the first character that is NOT 
 * the character 'ch'.  Returns -1 if all characters in the 
 * string are 'ch'.
 */
int32_t
AString::indexNotOf(const QChar& ch) const
{
    const int32_t len = length();
    for (int32_t i = 0; i < len; i++) {
        if (at(i) != ch) {
            return i;
        }
    }
    return -1;
}

/**
 * Return a 'C' char array containing the value
 * of the string.  This method is necessary since
 * on an instance of Ubuntu Linux, an invalid ASCII
 * character is found on the end of data returned 
 * by toLocal8Bit().constData();.  This method
 * will replace any non-ascii characters with "_".
 * All trailing non ASCII characters will be removed.
 * 
 * @return
 *    Char array of ASCII characters with a string
 *    terminator '\0' at the end.  Caller MUST
 *    free memory by running delete[] on the 
 *    returned array.
 */
char* 
AString::toCharArray() const 
{
    /*
     * Convert to a byte array
     */
    QByteArray byteArray = this->toLocal8Bit();
    const int32_t numBytes = byteArray.length();
    if (numBytes > 0) {
        char* charOut = new char[numBytes + 1];//are there any byteArrays that don't already come with a line terminator?
        
        int32_t lastAsciiChar = -1;
        for (int32_t i = 0; i < numBytes; i++) {
            char c = byteArray.at(i);
            if ((c == 10) || ((c >= 32) && (c <= 126))) {
                charOut[i] = c;
                lastAsciiChar = i;
            }
            else if((c == 0) && i == (numBytes-1)) {
                charOut[i] = c;
            }
            else
            {
                charOut[i] = '_';
            }
        }
        charOut[lastAsciiChar + 1] = '\0';
        
        return charOut;
    }
    
    char* s = new char[1];
    s[0] = 0;
    return s;
}

/**
 * If this string is not empty append a newline. 
 * Next, append the given string.
 */
void
AString::appendWithNewLine(const AString& str)
{
    if (isEmpty() == false) {
        append("\n");
    }
    append(str);
}

/**
 * Count the number of matching characters with the other string
 * starting at the end of the strings.
 * 
 * Example: this="someText"  rhs="moreText" => result=4
 * @rhs 
 *    The other string.
 * @return
 *    Number of character that match from the end of this and rhs.
 */
int64_t
AString::countMatchingCharactersFromEnd(const AString& rhs) const
{
    int64_t matchCount = 0;
    
    const int64_t myLength  = length();
    const int64_t rhsLength = rhs.length();
    const int64_t minLength  = std::min(myLength,
                                       rhsLength);
    
    for (int32_t offset = 1; offset <= minLength; offset++) {
        if (at(myLength - offset) == rhs.at(rhsLength - offset)) {
            matchCount++;
        }
        else {
            break;
        }
    }

    return matchCount;
}


