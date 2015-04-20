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

#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "../mindef.h"
#include "../transform.h"
#include "svgdom.h"

class SvgParser
{
public:
    enum TokenType {
        NoToken,
        Invalid,
        StartElement,
        EndElement,
        Text,
        Comment,
        DTD,
        ProcessingInstruction
    };
    enum ErrorType {
        NoError,
        ParseError
    };

    SvgParser(const QString &text);

    TokenType readNext();

    bool atEnd() const                  { return (str == end || str->isNull()); }
    bool hasError() const               { return m_error != NoError; }
    ErrorType errorId() const           { return m_error; }
    SvgAttributeHash attributes() const { return m_attrHash; }
    QString name() const                { return m_name; }
    QString value() const               { return m_value; }

private:
    // have to store full file content to prevent
    // *str and *end pointing to empty object
    QString m_fullStr;
    // current document pointer
    const QChar *str;
    // end of document
    const QChar *end;

    // svg element data
    QString m_name;
    QString m_value;
    SvgAttributeHash m_attrHash;

    // parser vars
    bool m_isPrevElemEnded;
    ErrorType m_error;
    QString textBuffer;

    enum EndTagType {
        NotEnd,
        EndType1,
        EndType2
    };

    inline void clearTextBuffer();
    inline void skipSpaces();
    inline static bool stringEqual(const QChar *p, const QChar *q, int nChar);
    inline void parseElement();
    inline EndTagType isEndTag(bool skipTag = true);
    // try to detect node type by start char
    TokenType identify(const QChar **p);
};

#endif // SVGPARSER_H
