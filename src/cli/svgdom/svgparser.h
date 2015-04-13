#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "../mindef.h"
#include "../transform.h"

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

    bool atEnd() const               { return (str == end || str->isNull()); }
    bool hasError() const            { return m_error != NoError; }
    ErrorType errorId() const        { return m_error; }
    IntHash attributes() const       { return m_attrHash; }
    StringHash attributesExt() const { return m_attrExtHash; }
    QString name() const             { return m_name; }
    QString value() const            { return m_value; }
    Transform transform() const      { return m_transform; }

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
    IntHash m_attrHash;
    StringHash m_attrExtHash;
    Transform m_transform;

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
