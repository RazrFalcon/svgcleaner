/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2015 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include "../enums.h"
#include "../tools.h"
#include "../stringwalker.h"

#include "svgparser.h"

SvgParser::SvgParser(const QString &text)
{
    m_fullStr = text;
    // pointer to data start
    str = m_fullStr.constData();
    // pointer to data end
    end = str + m_fullStr.size();

    m_error = NoError;
    m_isPrevElemEnded = false;
}

SvgParser::TokenType SvgParser::readNext()
{
    if (m_error != NoError)
        return Invalid;

    // if last element do not have child elements
    // set end tag
    if (m_isPrevElemEnded) {
        m_isPrevElemEnded = false;
        return EndElement;
    }

    // end of file
    if (atEnd())
        return NoToken;

    m_attrHash.clear();
    m_attrExtHash.clear();
    m_name.clear();
    m_value.clear();
    m_transform.clear();

    TokenType token = identify(&str);
    if (token == ProcessingInstruction) {
        // store all data between '?'
        QChar prevChar;
        while (!atEnd()) {
            if (*str == QL1C('>') && prevChar == QL1C('?')) {
                str++;
                break;
            }
            if (!prevChar.isNull())
                m_value += prevChar;
            prevChar = *str;
            str++;
        }
    } else if (token == Comment) {
        // store all data between '<-- ' and  ' -->'
        clearTextBuffer();
        static const QString endArr = QL1S("-->");
        while (!atEnd()) {
            if (textBuffer.endsWith(endArr))
                break;
            textBuffer += *str;
            str++;
        }
        m_value = textBuffer.mid(0, textBuffer.size()-3);
    } else if (token == StartElement) {
        parseElement();
    } else if (token == EndElement) {
        while (!atEnd()) {
            if (*str == QL1C('>')) {
                str++;
                break;
            }
            str++;
        }
    } else if (token == Text) {
        int length = 0;
        while (!atEnd()) {
            if (*str == QL1C('<'))
                break;
            length++;
            str++;
        }
        m_value = QString(str-length, length);
    } else if (token == DTD) {
        // DTD stored as text element
        clearTextBuffer();
        textBuffer += QL1S("<!");
        bool containsEntity = false;
        static const QString entityEnd = QLatin1String("]>");
        while (!atEnd()) {
            if (!containsEntity && *str == QL1C('>')) {
                // skip '>'
                str++;
                break;
            } else if (containsEntity && textBuffer.endsWith(entityEnd))
                break;
            if (*str == QL1C('['))
                containsEntity = true;
            textBuffer += *str;
            str++;
        }
        if (!containsEntity)
            textBuffer += QL1C('>');
        m_value = textBuffer;
    } else {
        m_error = ParseError;
    }
    return token;
}

void SvgParser::clearTextBuffer()
{
    textBuffer.clear();
    textBuffer.reserve(300);
}

void SvgParser::skipSpaces()
{
    while (!atEnd() && StringWalker::isSpace(str->unicode()))
        str++;
}

bool SvgParser::stringEqual(const QChar *p, const QChar *q, int nChar)
{
    if (p == q)
        return true;
    int n = 0;
    while (*p == *q && n < nChar) {
        ++p;
        ++q;
        ++n;
    }
    if ((n == nChar) || (*p == 0 && *q == 0))
        return true;
    return false;
}

void SvgParser::parseElement()
{
    bool hasAttributes = true;
    int nameLength = 0;
    while (!atEnd()) {
        // check is element name ends with end tag
        // namely do not have child elements and attributes
        EndTagType endType = isEndTag(false);
        if (endType != NotEnd) {
            hasAttributes = false;
            m_name = QString(str-nameLength, nameLength);
            if (endType == EndType1)
                str++;
            else if (endType == EndType2) {
                str += 2;
                m_isPrevElemEnded = true;
            }
            break;
        }
        // if char is space than node name is ended
        if (StringWalker::isSpace(str->unicode())) {
            m_name = QString(str-nameLength, nameLength);

            // check is element has end char after spaces
            // and not attributes
            skipSpaces();
            endType = isEndTag();
            if (endType != NotEnd) {
                if (endType == EndType2)
                    m_isPrevElemEnded = true;
                hasAttributes = false;
            }
            break;
        }
        nameLength++;
        str++;
    }

    if (!hasAttributes)
        return;

    // parse attributes

    // reserve memory for attributes
    // 6 - is average attributes count
    m_attrHash.reserve(6);
    QChar quote;
    QString attrName;
    while (!atEnd()) {
        nameLength = 0;
        skipSpaces();
        // data between ' ' and '=' is attribute name
        while (!atEnd() && *str != QL1C('=')) {
            nameLength++;
            ++str;
        }
        // ignore spaces in attribute name
        attrName.clear();

        uint attrId = hash(str-nameLength, nameLength);
        if (!isDefaultAttribute(attrId))
            attrName = QString(str-nameLength, nameLength);

        // skip '='
        str++;

        skipSpaces();

        if (!atEnd() && (*str == QL1C('\"') || *str == QL1C('\''))) {
            quote = *str;
            str++;
        }
        // data between quotes is attribute value
        nameLength = 0;
        while (!atEnd() && *str != quote) {
            nameLength++;
            str++;
        }

        // ignore empty attributes
        if (nameLength > 0) {
            if (   attrId == AttrId::transform
                || attrId == AttrId::gradientTransform
                || attrId == AttrId::patternTransform) {
                m_transform = Transform(str-nameLength, nameLength);
            } else {
                QString attrValue = QString(str-nameLength, nameLength);
                if (attrName.isEmpty()) {
                    m_attrHash.insert(attrId, attrValue);
                } else {
                    m_attrExtHash.insert(attrName, attrValue);
                }
            }
        }

        // skip quote char
        str++;
        skipSpaces();

        EndTagType endType = isEndTag();
        if (endType != NotEnd) {
            if (endType == EndType2)
                m_isPrevElemEnded = true;
            break;
        }
    }
}

SvgParser::EndTagType SvgParser::isEndTag(bool skipTag) {
    static const QString elemEndStr = QL1S("/>");
    if (*str == QL1C('>')) {
        if (skipTag)
            str++;
        return EndType1;
    } else if (stringEqual(str, elemEndStr.data(), 2)) {
        if (skipTag)
            str += 2;
        return EndType2;
    }
    return NotEnd;
}

SvgParser::TokenType SvgParser::identify(const QChar **p)
{
    static const QString xmlHeaderStr     = QL1S("<?");
    static const QString commentHeaderStr = QL1S("<!--");
    static const QString dtdHeaderStr     = QL1S("<!");
    static const QString elementEndStr    = QL1S("</");

    const QChar* start = *p;

    skipSpaces();

    // magic numbers it's string search length
    if (stringEqual(*p, xmlHeaderStr.data(), 2)) {
        *p += 2;
        return ProcessingInstruction;
    }
    else if (stringEqual(*p, commentHeaderStr.data(), 4)) {
        *p += 4;
        return Comment;
    }
    else if (stringEqual(*p, dtdHeaderStr.data(), 2)) {
        *p += 2;
        return DTD;
    }
    else if (stringEqual(*p, elementEndStr.data(), 2)) {
        *p += 2;
        return EndElement;
    }
    else if (**p == QL1C('<')) {
        *p += 1;
        return StartElement;
    }
    else {
        *p = start;
        return Text;
    }
    return Invalid;
}
