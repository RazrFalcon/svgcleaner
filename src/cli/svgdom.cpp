/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
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

/*
 * This classes is a mix of Qt DOM implementation:
 * - Implicitly shared classes
 * - Classes structure
 * - Some functions implementations
 * and TinyXML2:
 * - Parser logic
 * - Node detecting
 *
 * Main difference that SvgDom:
 * - Works only with well-formed SVG files
 * - Qt based
 * - UFT-8 only
 * - Written specifically for SVG
 */

#include <QAtomicInt>
#include <QFile>

#include "tools.h"
#include "svgdom.h"

// Private class declarations

using namespace Element;
using namespace Attribute;

SvgNodePrivate *nextNodePrivate(SvgNodePrivate *node, SvgNodePrivate *root);

namespace Parser {

static const QString ElemEndStr  = QL1S("/>");
static const QChar ElemStartChar = QL1C('<');
static const QChar ElemEndChar   = QL1C('>');
static const QChar SingleQuote   = QL1C('\'');
static const QChar DoubleQuote   = QL1C('\"');
static const QChar Equation      = QL1C('=');

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
        FileOpenError,
        FileEmptyError,
        ParseError
    };

    SvgParser(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QFile::ReadOnly)) {
            m_error = FileOpenError;
            return;
        }
        m_fullStr = QString::fromUtf8(file.readAll());
        if (m_fullStr.size() == 0) {
            m_error = FileEmptyError;
            return;
        }
        // pointer to data start
        str = m_fullStr.constData();
        // pointer to data end
        end = str + m_fullStr.size();

        m_error = NoError;
        m_isPrevElemEnded = false;
    }

    TokenType readNext()
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
        m_id.clear();
        m_name.clear();
        m_value.clear();

        TokenType token = identify(&str);
        if (token == ProcessingInstruction) {
            // store all data between '?'
            QChar prevChar;
            while (!atEnd()) {
                if (*str == ElemEndChar && prevChar == '?') {
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
            static const QString endArr = "-->";
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
                if (*str == ElemEndChar) {
                    str++;
                    break;
                }
                str++;
            }
        } else if (token == Text) {
            clearTextBuffer();
            while (!atEnd()) {
                if (*str == ElemStartChar)
                    break;
                textBuffer += *str;
                str++;
            }
            m_value = textBuffer;
        } else if (token == DTD) {
            // DTD stored as text element
            clearTextBuffer();
            textBuffer += QL1S("<!");
            bool containsEntity = false;
            static const QString entityEnd = QLatin1String("]>");
            while (!atEnd()) {
                if (!containsEntity && *str == ElemEndChar) {
                    // skip '>'
                    str++;
                    break;
                } else if (containsEntity && textBuffer.endsWith(entityEnd))
                    break;
                if (*str == '[')
                    containsEntity = true;
                textBuffer += *str;
                str++;
            }
            if (!containsEntity)
                textBuffer += QL1S(">");
            m_value = textBuffer;
        } else {
            m_error = ParseError;
        }
        return token;
    }

    bool atEnd() const              { return (str == end || str->isNull()); }
    bool hasError() const           { return m_error != NoError; }
    ErrorType errorId() const       { return m_error; }
    StringHash attributes() const   { return m_attrHash; }
    QString id() const              { return m_id; }
    QString name() const            { return m_name; }
    QString value() const           { return m_value; }

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
    StringHash m_attrHash;
    QString m_id;

    // parser vars
    bool m_isPrevElemEnded;
    ErrorType m_error;
    QString textBuffer;
    QString smallTextBuffer;

    enum EndTagType {
        NotEnd,
        EndType1,
        EndType2
    };

    inline void clearSmallTextBuffer()
    {
        smallTextBuffer.clear();
        smallTextBuffer.reserve(50);
    }
    inline void clearTextBuffer()
    {
        textBuffer.clear();
        textBuffer.reserve(300);
    }
    inline void skipSpaces()
    {
        while (!atEnd() && isSpace(str->unicode()))
            str++;
    }
    inline static bool stringEqual(const QChar *p, const QChar *q, int nChar)
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
    inline void parseElement()
    {
        clearSmallTextBuffer();

        bool hasAttributes = true;
        while (!atEnd()) {
            // check is element name ends with end tag
            // namely do not have child elements and attributes
            EndTagType endType = isEndTag();
            if (endType != NotEnd) {
                hasAttributes = false;
                m_name = smallTextBuffer;
                if (endType == EndType2)
                    m_isPrevElemEnded = true;
                break;
            }
            // if char is space than node name is ended
            if (isSpace(str->unicode())) {
                m_name = smallTextBuffer;
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

            smallTextBuffer += *str;
            str++;
        }

        if (!hasAttributes)
            return;

        // parse attributes

        // reserve memory for attributes
        // 6 - is average attributes count
        m_attrHash.reserve(6);
        QChar quote;
        while (!atEnd()) {
            clearSmallTextBuffer();
            skipSpaces();
            // data between ' ' and '=' is attribute name
            while (!atEnd() && *str != Equation) {
                // ignore spaces inside attribute name
                if (!isSpace(str->unicode()))
                    smallTextBuffer += *str;
                ++str;
            }
            // skip '='
            str++;
            clearTextBuffer();

            skipSpaces();

            if (!atEnd() && *str == DoubleQuote) {
                quote = *str;
                str++;
            } else if (!atEnd() && *str == SingleQuote) {
                quote = *str;
                str++;
            }
            // data between '/" and '/" is attribute value
            while (!atEnd() && *str != quote) {
                textBuffer += *str;
                str++;
            }
            // skip quote char
            str++;
            skipSpaces();

            // ignore empty attributes
            if (!textBuffer.isEmpty()) {
                if (smallTextBuffer == Attribute::A_id)
                    m_id = textBuffer;
                m_attrHash.insert(smallTextBuffer, textBuffer);
            }

            EndTagType endType = isEndTag();
            if (endType != NotEnd) {
                if (endType == EndType2)
                   m_isPrevElemEnded = true;
                break;
            }
        }
    }
    inline EndTagType isEndTag() {
        if (*str == ElemEndChar) {
            str++;
            return EndType1;
        } else if (stringEqual(str, ElemEndStr.data(), 2)) {
            str += 2;
            return EndType2;
        }
        return NotEnd;
    }
    // try to detect node type by start char
    TokenType identify(const QChar **p)
    {
        static const QString xmlHeaderStr     = QString("<?");
        static const QString commentHeaderStr = QString("<!--");
        static const QString dtdHeaderStr     = QString("<!");
        static const QString elementEndStr    = QString("</");

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
        else if (**p == ElemStartChar) {
            *p += 1;
            return StartElement;
        }
        else {
            *p = start;
            return Text;
        }
        return Invalid;
    }
};
}

class SvgNodePrivate
{
public:
    SvgNodePrivate(SvgDocumentPrivate*, SvgNodePrivate* parent = 0);
    virtual ~SvgNodePrivate();

    bool hasValue() const { return !value.isEmpty(); }

    SvgDocumentPrivate* ownerDocument();
    void setOwnerDocument(SvgDocumentPrivate* doc);

    virtual SvgNodePrivate* insertBefore(SvgNodePrivate* newChild, SvgNodePrivate* refChild);
    virtual SvgNodePrivate* removeChild(SvgNodePrivate* oldChild);
    virtual SvgNodePrivate* appendChild(SvgNodePrivate* newChild);

    virtual void clear();

    inline SvgNodePrivate* parent() const { return hasParent ? ownerNode : 0; }
    inline void setParent(SvgNodePrivate *p) { ownerNode = p; hasParent = true; }

    void setNoParent() {
        ownerNode = hasParent ? (SvgNodePrivate*)ownerDocument() : 0;
        hasParent = false;
    }

    // Dynamic cast
    virtual bool isDocument() const    { return false; }
    virtual bool isElement() const     { return false; }
    virtual bool isDeclaration() const { return false; }
    virtual bool isComment() const     { return false; }
    virtual bool isText() const        { return false; }

    virtual void save(QTextStream&, int, int) const;

    // Variables
    QAtomicInt ref;
    SvgNodePrivate* prev;
    SvgNodePrivate* next;
    SvgNodePrivate* ownerNode; // either the node's parent or the node's owner document
    SvgNodePrivate* first;
    SvgNodePrivate* last;

    QString name;
    QString value;
    bool createdWithDom1Interface : 1;
    bool hasParent                : 1;
};

class SvgElementPrivate : public SvgNodePrivate
{
public:
    SvgElementPrivate(SvgDocumentPrivate*, SvgNodePrivate* parent, const QString& name);
    SvgElementPrivate(SvgDocumentPrivate*, SvgNodePrivate* parent);
    ~SvgElementPrivate() {}
    virtual bool isElement() const { return true; }
    virtual void save(QTextStream& s, int, int) const;

    // Variables
    QString id;
    StringHash attrs;
};

class SvgCommentPrivate : public SvgNodePrivate
{
public:
    SvgCommentPrivate(SvgDocumentPrivate*, SvgNodePrivate* parent, const QString& val);
    virtual bool isComment() const { return true; }
    virtual void save(QTextStream& s, int, int) const;
};

class SvgTextPrivate : public SvgNodePrivate
{
public:
    SvgTextPrivate(SvgDocumentPrivate*, SvgNodePrivate* parent, const QString& val);
    virtual bool isText() const { return true; }
    virtual void save(QTextStream& s, int, int) const;
    // vars
    bool isDtd;
};

class SvgDeclarationPrivate : public SvgNodePrivate
{
public:
    SvgDeclarationPrivate(SvgDocumentPrivate*, SvgNodePrivate* parent, const QString& data);
    virtual bool isDeclaration() const { return true; }
    virtual void save(QTextStream& s, int, int) const;
};

class SvgDocumentPrivate : public SvgNodePrivate
{
public:
    SvgDocumentPrivate();
    ~SvgDocumentPrivate() {}
    bool isDocument() const { return true; }
    void clear();

    // vars:
    QString lastError;
};

inline void SvgNodePrivate::setOwnerDocument(SvgDocumentPrivate *doc)
{
    ownerNode = doc;
    hasParent = false;
}

SvgNodePrivate::SvgNodePrivate(SvgDocumentPrivate *doc, SvgNodePrivate *par)
{
    ref = 1;
    if (par)
        setParent(par);
    else
        setOwnerDocument(doc);
    prev = 0;
    next = 0;
    first = 0;
    last = 0;
    createdWithDom1Interface = true;
}

SvgNodePrivate::~SvgNodePrivate()
{
    SvgNodePrivate* p = first;
    SvgNodePrivate* n;

    while (p) {
        n = p->next;
        if (!p->ref.deref())
            delete p;
        else
            p->setNoParent();
        p = n;
    }
    first = 0;
    last = 0;
}

void SvgNodePrivate::clear()
{
    SvgNodePrivate* p = first;
    SvgNodePrivate* n;

    while (p) {
        n = p->next;
        if (!p->ref.deref())
            delete p;
        p = n;
    }
    first = 0;
    last = 0;
}

SvgNodePrivate* SvgNodePrivate::insertBefore(SvgNodePrivate* newChild, SvgNodePrivate* refChild)
{
    // Error check
    if (!newChild)
        return 0;

    // Error check
    if (newChild == refChild)
        return 0;

    // Error check
    if (refChild && refChild->parent() != this)
        return 0;

    // No more errors can occur now, so we take
    // ownership of the node.
    newChild->ref.ref();

    if (newChild->parent())
        newChild->parent()->removeChild(newChild);

    newChild->setParent(this);

    if (!refChild) {
        if (first)
            first->prev = newChild;
        newChild->next = first;
        if (!last)
            last = newChild;
        first = newChild;
        return newChild;
    }

    if (refChild->prev == 0) {
        if (first)
            first->prev = newChild;
        newChild->next = first;
        if (!last)
            last = newChild;
        first = newChild;
        return newChild;
    }

    newChild->next = refChild;
    newChild->prev = refChild->prev;
    refChild->prev->next = newChild;
    refChild->prev = newChild;

    return newChild;
}

SvgNodePrivate* SvgNodePrivate::removeChild(SvgNodePrivate* oldChild)
{
    // Error check
    if (oldChild->parent() != this)
        return 0;

    // Perhaps oldChild was just created with "createElement" or that. In this case
    // its parent is SvgDocument but it is not part of the documents child list.
    if (oldChild->next == 0 && oldChild->prev == 0 && first != oldChild)
        return 0;

    if (oldChild->next)
        oldChild->next->prev = oldChild->prev;
    if (oldChild->prev)
        oldChild->prev->next = oldChild->next;

    if (last == oldChild)
        last = oldChild->prev;
    if (first == oldChild)
        first = oldChild->next;

    oldChild->setNoParent();
    oldChild->next = 0;
    oldChild->prev = 0;

    // We are no longer interested in the old node
    oldChild->ref.deref();

    return oldChild;
}

SvgNodePrivate* SvgNodePrivate::appendChild(SvgNodePrivate* newChild)
{
    if (!newChild)
        return 0;

    // Release new node from its current parent
    if (newChild->parent())
        newChild->parent()->removeChild(newChild);

    // No more errors can occur now, so we take
    // ownership of the node
    newChild->ref.ref();

    newChild->setParent(this);

    // Insert at the end
    if (last)
        last->next = newChild;
    newChild->prev = last;
    if (!first)
        first = newChild;
    last = newChild;
    return newChild;
}

SvgDocumentPrivate* SvgNodePrivate::ownerDocument()
{
    SvgNodePrivate* p = this;
    while (p && !p->isDocument()) {
        if (!p->hasParent)
            return (SvgDocumentPrivate*)p->ownerNode;
        p = p->parent();
    }

    return static_cast<SvgDocumentPrivate *>(p);
}

void SvgNodePrivate::save(QTextStream& s, int depth, int indent) const
{
    const SvgNodePrivate* n = first;
    while (n) {
        n->save(s, depth, indent);
        n = n->next;
    }
}

SvgNode::SvgNode()
{
    impl = 0;
}

SvgNode::SvgNode(const SvgNode &n)
{
    impl = n.impl;
    if (impl)
        impl->ref.ref();
}

SvgNode::SvgNode(SvgNodePrivate *n)
{
    impl = n;
    if (impl)
        impl->ref.ref();
}

SvgNode& SvgNode::operator=(const SvgNode &n)
{
    if (n.impl)
        n.impl->ref.ref();
    if (impl && !impl->ref.deref())
        delete impl;
    impl = n.impl;
    return *this;
}

bool SvgNode::operator== (const SvgNode& n) const
{
    return (impl == n.impl);
}

bool SvgNode::operator!= (const SvgNode& n) const
{
    return (impl != n.impl);
}

SvgNode::~SvgNode()
{
    if (impl && !impl->ref.deref())
        delete impl;
}

QString SvgNode::nodeName() const
{
    if (!impl)
        return QString();
    return ((SvgNodePrivate*)impl)->name;
}

SvgNode SvgNode::parentNode() const
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->parent());
}

SvgNodeList SvgNode::childNodes() const
{
    if (!impl)
        return SvgNodeList();
    SvgNodeList list;
    SvgNodePrivate* p = impl->first;
    while (p) {
        list.append(p);
        p = p->next;
    }
    return list;
}

SvgNode SvgNode::firstChild() const
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->first);
}

SvgNode SvgNode::lastChild() const
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->last);
}

SvgNode SvgNode::previousSibling() const
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->prev);
}

SvgNode SvgNode::nextSibling() const
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->next);
}

SvgDocument SvgNode::ownerDocument() const
{
    if (!impl)
        return SvgDocument();
    return SvgDocument(((SvgNodePrivate*)impl)->ownerDocument());
}

bool SvgNode::hasNextSibling() const
{
    if (!impl)
        return false;
    for (SvgNode sib = nextSibling(); !sib.isNull(); sib = sib.nextSibling()) {
        if (sib.isElement())
            return true;
    }
    return false;
}

SvgNode SvgNode::insertBefore(const SvgNode& newChild, const SvgNode& refChild)
{
    if (!impl)
        return SvgNode();
    return SvgNode(((SvgNodePrivate*)impl)->insertBefore(newChild.impl, refChild.impl));
}

SvgNode SvgNode::removeChild(const SvgNode& oldChild)
{
    if (!impl)
        return SvgNode();

    if (oldChild.isNull())
        return SvgNode();

    return SvgNode(((SvgNodePrivate*)impl)->removeChild(oldChild.impl));
}

SvgNode SvgNode::appendChild(const SvgNode& newChild)
{
    if (!impl) {
        qWarning("Calling appendChild() on a null node does nothing.");
        return SvgNode();
    }
    return SvgNode(((SvgNodePrivate*)impl)->appendChild(newChild.impl));
}

bool SvgNode::hasChildren() const
{
    if (!impl)
        return false;
    return (((SvgNodePrivate*)impl)->first != 0);
}

bool SvgNode::isNull() const
{
    return (impl == 0);
}

void SvgNode::clear()
{
    if (impl && !impl->ref.deref())
        delete impl;
    impl = 0;
}

void SvgNode::save(QTextStream& str, int indent) const
{
    if (!impl)
        return;

    if (isDocument()) {
        const SvgNodePrivate* n = impl->first;
        while (n) {
            n->save(str, 0, indent);
            n = n->next;
        }
//        static_cast<const SvgDocumentPrivate *>(impl)->saveDocument(str, indent);
    } else
        ((SvgNodePrivate*)impl)->save(str, 1, indent);
}

bool SvgNode::isDocument() const
{
    if (impl)
        return impl->isDocument();
    return false;
}

bool SvgNode::isElement() const
{
    if (impl)
        return impl->isElement();
    return false;
}

bool SvgNode::isText() const
{
    if (impl)
        return impl->isText();
    return false;
}

bool SvgNode::isDeclaration() const
{
    if (impl)
        return impl->isDeclaration();
    return false;
}

bool SvgNode::isComment() const
{
    if (impl)
        return impl->isComment();
    return false;
}

bool SvgNode::hasText() const
{
    if (!impl)
        return false;
    if (impl->first)
        return impl->first->isText();
    return false;
}

SvgElement SvgNode::firstChildElement() const
{
    for (SvgNode child = firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isElement())
            return child.toElement();
    }
    return SvgElement();
}

SvgElement SvgNode::nextSiblingElement() const
{
    for (SvgNode sib = nextSibling(); !sib.isNull(); sib = sib.nextSibling()) {
        if (sib.isElement())
            return sib.toElement();
    }
    return SvgElement();
}

SvgElement SvgNode::previousSiblingElement() const
{
    for (SvgNode sib = previousSibling(); !sib.isNull(); sib = sib.previousSibling()) {
        if (sib.isElement())
            return sib.toElement();
    }
    return SvgElement();
}

SvgDocument SvgNode::toDocument() const
{
    if (impl && impl->isDocument())
        return SvgDocument(((SvgDocumentPrivate*)impl));
    return SvgDocument();
}

SvgElement SvgNode::toElement() const
{
    if (impl && impl->isElement())
        return SvgElement(((SvgElementPrivate*)impl));
    return SvgElement();
}

SvgDeclaration SvgNode::toDeclaration() const
{
    if (impl && impl->isDeclaration())
        return SvgDeclaration(((SvgDeclarationPrivate*)impl));
    return SvgDeclaration();
}

SvgComment SvgNode::toComment() const
{
    if (impl && impl->isComment())
        return SvgComment(((SvgCommentPrivate*)impl));
    return SvgComment();
}

SvgText SvgNode::toText() const
{
    if (impl && impl->isText())
        return SvgText(((SvgTextPrivate*)impl));
    return SvgText();
}

SvgElementPrivate::SvgElementPrivate(SvgDocumentPrivate* d, SvgNodePrivate* p,
                                          const QString& tagname)
    : SvgNodePrivate(d, p)
{
    name = tagname;
}

SvgElementPrivate::SvgElementPrivate(SvgDocumentPrivate* d, SvgNodePrivate* p)
    : SvgNodePrivate(d, p)
{
    createdWithDom1Interface = false;
}

void SvgElementPrivate::save(QTextStream& s, int depth, int indent) const
{
    static const QString tspanElem = QL1S("tspan");
    static const QString startStr  = QL1S("</");
    static const QString endStr    = QL1S("/>");
    static const QString startChar = QL1S("<");
    static const QString endChar   = QL1S(">");
    static const QString startAttr = QL1S("=\"");
    static const QString quoteChar = QL1S("\"");
    static const QChar spaceChar   = QL1C(' ');

    if (!(prev && prev->hasValue()) && !hasValue() && name != tspanElem)
        s << QString(indent < 1 ? 0 : depth * indent, spaceChar);

    s << startChar << name;

    if (!attrs.isEmpty()) {
        // save attributes
        StringHash::const_iterator it = attrs.constBegin();
        for (; it != attrs.constEnd(); ++it) {
            // do not save attributes with empty value
            if (!it.value().isEmpty()) {
                s << spaceChar;
                s << it.key() << startAttr << it.value() << quoteChar;
            }
        }
    }

    if (last) {
        // has child nodes
        if (first->hasValue()) {
            s << endChar;
        } else {
            s << endChar;
            // -1 disables new lines.
            if (indent != -1 && first->name != tspanElem && name != tspanElem)
                s << endl;
        }
        SvgNodePrivate::save(s, depth + 1, indent);
        if (!last->hasValue())
            s << QString(indent < 1 ? 0 : depth * indent, spaceChar);

        s << startStr << name << endChar;
    } else {
        if (hasValue()) {
            s << endChar;
            s << value;
            s << startStr << name << endChar;
        } else {
            s << endStr;
        }
    }
    if (next) {
        if (next->name != tspanElem && !next->hasValue()) {
            if (indent != -1)
                s << endl;
        }
    } else {
        if (indent != -1)
            s << endl;
    }
}

#define IMPL ((SvgElementPrivate*)impl)

SvgElement::SvgElement(SvgElementPrivate* n)
    : SvgNode(n)
{
}

SvgElement& SvgElement::operator= (const SvgElement& x)
{
    return (SvgElement&) SvgNode::operator=(x);
}

void SvgElement::setTagName(const QString& name)
{
    if (impl)
        impl->name = name;
}

QString SvgElement::tagName() const
{
    if (!impl)
        return QString();
    return impl->name;
}

StringHash SvgElement::attributes() const
{
    if (!impl)
        return StringHash();
    return IMPL->attrs;
}

QString SvgElement::attribute(const QString& name, const QString& defValue) const
{
    if (!impl)
        return defValue;
    if (!IMPL->attrs.contains(name))
        return defValue;
    return IMPL->attrs.value(name);
}

void SvgElement::setAttribute(const QString& name, const QString& value)
{
    if (!impl)
        return;
    if (name == Attribute::A_id)
        IMPL->id = value;
    IMPL->attrs.insert(name, value);
}

void SvgElement::setAttributeHash(const StringHash &hash)
{
    if (!impl)
        return;
    IMPL->attrs = hash;
}

void SvgElement::removeAttribute(const QString& name)
{
    if (!impl)
        return;
    if (name == Attribute::A_id)
        IMPL->id.clear();
    IMPL->attrs.remove(name);
}

bool SvgElement::hasAttribute(const QString& name) const
{
    if (!impl)
        return false;
    return IMPL->attrs.contains(name);
}

bool SvgElement::hasAttributes(const QStringList &list) const
{
    foreach (const QString &attrName, list)
        if (hasAttribute(attrName))
            return true;
    return false;
}

bool SvgElement::hasAttributes() const
{
    if (!impl)
        return false;
    return IMPL->attrs.size() > 0;
}

QString SvgElement::xlinkId() const
{
    return attribute(Attribute::A_xlink_href).remove(0,1);
}

QString SvgElement::id() const
{
    if (!impl)
        return QString();
    return IMPL->id;
}

bool SvgElement::hasChildElement() const
{
    if (!impl)
        return false;
    SvgNodePrivate* p = impl->first;
    while (p) {
        if (p->isElement())
            return true;
        p = p->next;
    }
    return false;
}

int SvgElement::childElementCount() const
{
    if (!impl)
        return 0;
    SvgNodePrivate* p = impl->first;
    int count = 0;
    while (p) {
        count++;
        p = p->next;
    }
    return count;
}

StringHash SvgElement::attributesHash(bool ignoreId) const
{
    if (!impl)
        return StringHash();
    StringHash map = IMPL->attrs;
    if (ignoreId)
        map.remove(Attribute::A_id);
    return map;
}

void SvgElement::removeAttributeIf(const QString &name, const QString &value)
{
    if (!impl)
        return;
    if (IMPL->attrs.value(name) == value)
        IMPL->attrs.remove(name);
}

double SvgElement::doubleAttribute(const QString &name)
{
    return attribute(name).toDouble();
}

QStringList SvgElement::attributesList() const
{
    if (!impl)
        return QStringList();
    return ((SvgElementPrivate*)impl)->attrs.keys();
}

SvgElement SvgElement::parentElement() const
{
    return parentNode().toElement();
}

SvgElementList SvgElement::childElements() const
{
    if (!impl)
        return SvgElementList();
    SvgNodePrivate* p = impl->first;
    SvgElementList list;
    list.reserve(childElementCount());
    while (p) {
        if (p->isElement())
            list.append(SvgNode(p).toElement());
        p = p->next;
    }
    return list;
}

SvgElement SvgElement::firstChildElement() const
{
    if (!impl)
        return SvgElement();
    SvgNodePrivate* p = impl->first;
    while (p) {
        if (p->isElement())
            return SvgNode(p).toElement();
        p = p->next;
    }
    return SvgElement();
}

QStringList SvgElement::styleAttributesList() const
{
    if (!impl)
        return QStringList();
    QStringList list;
    list.reserve(attributesCount());
    foreach (const QString &key, IMPL->attrs.keys()) {
        if (Properties::presentationAttributes.contains(key))
            list << key;
    }
    return list;
}

QStringList SvgElement::attributesListBySet(const StringSet &set) const
{
    if (!impl)
        return QStringList();
    QStringList list;
    foreach (const QString &key, IMPL->attrs.keys()) {
        if (set.contains(key))
            list << key;
    }
    return list;
}

bool SvgElement::hasLinkedDef() const
{
    if (!impl)
        return false;
    // TODO: is filter needed?
    static const QStringList illegalAttrList = QStringList()
        << A_clip_path << A_mask << A_filter;
    foreach (const QString attrName, illegalAttrList) {
        if (hasAttribute(attrName))
            return true;
    }
    static const QStringList illegalStyleAttrList = QStringList()
        << A_fill << A_stroke;
    foreach (const QString attrName, illegalStyleAttrList) {
        if (attribute(attrName).startsWith(UrlPrefix))
            return true;
    }
    return false;
}

bool SvgElement::isContainer() const
{
    static const StringSet containers = StringSet()
        << E_a << E_defs << E_glyph << E_g << E_marker << E_missing_glyph
        << E_svg << E_switch << E_symbol;
    return containers.contains(tagName());
}

bool SvgElement::isGroup() const
{
    return (tagName() == E_g);
}

bool SvgElement::hasImportantAttrs()
{
    int attrCount = attributesCount();
    if (attrCount == 0)
        return false;
    if (Keys::get().flag(Key::RemoveUnreferencedIds)) {
        if (attrCount == 1 && hasAttribute(A_id))
            return false;
    }
    return true;
}

bool SvgElement::hasLinkedStyle()
{
    if (!impl)
        return false;
    static QStringList illegalStyleAttrList;
    if (illegalStyleAttrList.isEmpty())
        illegalStyleAttrList << A_fill << A_stroke;
    foreach (const QString attrName, illegalStyleAttrList) {
        if (attribute(attrName).startsWith(UrlPrefix))
            return true;
    }
    return false;
}

bool SvgElement::isUsed() const
{
    return hasAttribute(CleanerAttr::UsedElement);
}

bool SvgElement::hasChildWithTagName(const QString &name) const
{
    if (!impl)
        return false;

    element_loop(*this) {
        if (elem.tagName() == name)
            return true;
        nextElement(elem, root);
    }
    return false;
}

bool SvgElement::hasTextChild() const
{
    if (!impl)
        return false;

    SvgNodePrivate *p = impl->first;
    while (p) {
        if (p->isText())
            return true;
        p = nextNodePrivate(p, impl);
    }
    return false;
}

int SvgElement::attributesCount() const
{
    if (!impl)
        return 0;
    return IMPL->attrs.size();
}

QString SvgElement::defIdFromAttribute(const QString &name)
{
    QString id = attribute(name);
    if (!id.startsWith(UrlPrefix))
        return QString();
    return id.mid(5, id.size()-6);
}

StringMap SvgElement::styleMap() const
{
    if (!impl)
        return StringMap();
    StringMap hash;
    foreach (const QString &key, IMPL->attrs.keys()) {
        if (Properties::presentationAttributes.contains(key))
            hash.insert(key, IMPL->attrs.value(key));
    }
    return hash;
}

StringHash SvgElement::styleHash() const
{
    if (!impl)
        return StringHash();
    StringHash hash;
    foreach (const QString &key, IMPL->attrs.keys()) {
        if (Properties::presentationAttributes.contains(key))
            hash.insert(key, IMPL->attrs.value(key));
    }
    return hash;
}

void SvgElement::removeAttributes(const QStringList &list)
{
    foreach (const QString &text, list)
        removeAttribute(text);
}

void SvgElement::setStylesFromHash(const StringHash &hash)
{
    foreach (const QString &attr, hash.keys())
        setAttribute(attr, hash.value(attr));
}

void SvgElement::setTransform(const QString &transform, bool fromParent)
{
    if (hasAttribute(A_transform)) {
        if (fromParent) {
            Transform ts(transform + " " + attribute(A_transform));
            setAttribute(A_transform, ts.simplified());
        } else {
            Transform ts(attribute(A_transform) + " " + transform);
            setAttribute(A_transform, ts.simplified());
        }
    } else {
        setAttribute(A_transform, transform);
    }
}

QString SvgElement::text() const
{
    if (!impl)
        return QString();
    SvgNodePrivate *first = IMPL->first;
    if (first != 0) {
        SvgNodePrivate* p = first;
        QString str;
        while (p) {
            if (p->isText())
                str += p->value;
            p = p->next;
        }
        return str;
    }
    return "";
}

#undef IMPL

SvgCommentPrivate::SvgCommentPrivate(SvgDocumentPrivate* d, SvgNodePrivate* parent, const QString& val)
    : SvgNodePrivate(d, parent)
{
    value = val;
}

void SvgCommentPrivate::save(QTextStream& s, int depth, int indent) const
{
    // We don't output whitespace if we would pollute a text node.
    if (!(prev && !prev->value.isEmpty()))
        s << QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

    if (indent != -1)
        s << endl;

    s << "<!--" << value;
    if (value.endsWith(QL1C('-')))
        s << ' '; // Ensures that XML comment doesn't end with --->
    s << "-->";

    if (!(next && !next->value.isEmpty()))
        s << endl;
}

SvgComment::SvgComment(SvgCommentPrivate* n)
    : SvgNode(n)
{
}

SvgComment& SvgComment::operator= (const SvgComment& x)
{
    return (SvgComment&) SvgNode::operator=(x);
}

SvgTextPrivate::SvgTextPrivate(SvgDocumentPrivate* d, SvgNodePrivate* parent, const QString& val)
    : SvgNodePrivate(d, parent)
{
    value = val;
    isDtd = false;
}

void SvgTextPrivate::save(QTextStream& s, int /*depth*/, int indent) const
{
    if (isDtd && indent != -1)
        s << endl;
    s << value;
    if (isDtd && indent != -1)
        s << endl;
}

SvgText::SvgText(SvgTextPrivate* n)
    : SvgNode(n)
{
}

SvgText& SvgText::operator= (const SvgText& x)
{
    return (SvgText&) SvgNode::operator=(x);
}

QString SvgText::text() const
{
    if (!impl)
        return QString();
    return ((SvgTextPrivate*)impl)->value;
}

void SvgText::setDtd(bool flag)
{
    if (!impl)
        return;
    ((SvgTextPrivate*)impl)->isDtd = flag;
}

SvgDeclarationPrivate::SvgDeclarationPrivate(SvgDocumentPrivate* d,
    SvgNodePrivate* parent, const QString& data)
    : SvgNodePrivate(d, parent)
{
    value = data;
}

void SvgDeclarationPrivate::save(QTextStream& s, int, int indent) const
{
    s << "<?" << value << "?>";
    if (indent != -1)
        s << endl;
}

SvgDeclaration& SvgDeclaration::operator= (const SvgDeclaration& x)
{
    return (SvgDeclaration&) SvgNode::operator=(x);
}

QString SvgDeclaration::data() const
{
    if (!impl)
        return QString();
    return impl->value;
}

void SvgDeclaration::setData(const QString& d)
{
    if (!impl)
        return;
    impl->value = d;
}

SvgDeclaration::SvgDeclaration(SvgDeclarationPrivate *n)
    : SvgNode(n)
{
}

SvgDocumentPrivate::SvgDocumentPrivate()
    : SvgNodePrivate(0)
{
}

void SvgDocumentPrivate::clear()
{
    SvgNodePrivate::clear();
}

#define IMPL ((SvgDocumentPrivate*)impl)

SvgDocument::SvgDocument(SvgDocumentPrivate *x)
    : SvgNode(x)
{
}

SvgDocument& SvgDocument::operator= (const SvgDocument& x)
{
    return (SvgDocument&) SvgNode::operator=(x);
}

bool SvgDocument::loadFile(const QString &filePath)
{
    clear();

    if (!impl)
        impl = new SvgDocumentPrivate();

    using namespace Parser;

    SvgNode node(impl);
    SvgParser reader(filePath);
    while (!reader.atEnd() && !reader.hasError()) {
        SvgParser::TokenType token = reader.readNext();
        if (token == SvgParser::ProcessingInstruction) {
            node.appendChild(createDeclaration(reader.value()));
        } else if (token == SvgParser::StartElement) {
            SvgElement elem = createElement(reader.name());
            node.appendChild(elem);
            elem.setAttribute(Attribute::A_id, reader.id());
            elem.setAttributeHash(reader.attributes());
            node = elem;
        } else if (token == SvgParser::Text) {
            if (node == documentElement())
                continue;
            node.appendChild(createText(reader.value()));
        } else if (token == SvgParser::EndElement) {
            if (!node.isNull() && node != documentElement())
                node = node.parentNode();
        } else if (token == SvgParser::Comment) {
            node.appendChild(createComment(reader.value()));
        } else if (token == SvgParser::DTD) {
            SvgText text = createText(reader.value());
            text.setDtd(true);
            node.appendChild(text);
        }
    }
    if (reader.hasError()) {
        if (reader.errorId() == SvgParser::FileOpenError)
            IMPL->lastError = "Error: could not open svg file";
        else if (reader.errorId() == SvgParser::ParseError)
            IMPL->lastError = "Error: could not parse svg file";
        return false;
    }
    return true;
}

QString SvgDocument::toString(int indent) const
{
    QString str;
    QTextStream s(&str, QIODevice::WriteOnly);
    save(s, indent);
    return str;
}

QString SvgDocument::lastError() const
{
    if (!impl)
        return QString();
    return IMPL->lastError;
}

SvgElement SvgDocument::documentElement() const
{
    if (!impl)
        return SvgElement();
    SvgNodePrivate *p = impl->first;
    while (p && !p->isElement())
        p = p->next;
    return static_cast<SvgElementPrivate *>(p);
}

SvgElement SvgDocument::createElement(const QString& tagName)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgElementPrivate *e = new SvgElementPrivate(IMPL, 0, tagName);
    e->ref.deref();
    return e;
}

SvgComment SvgDocument::createComment(const QString& value)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgCommentPrivate *c = new SvgCommentPrivate(IMPL, 0, value);
    c->ref.deref();
    return c;
}

SvgDeclaration SvgDocument::createDeclaration(const QString& data)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgDeclarationPrivate *p = new SvgDeclarationPrivate(IMPL, 0, data);
    p->ref.deref();
    return p;
}

SvgText SvgDocument::createText(const QString &text)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgTextPrivate *p = new SvgTextPrivate(IMPL, 0, text);
    p->ref.deref();
    return p;
}

#undef IMPL

inline void nextElement(SvgElement &elem, const SvgElement &root, bool ignoreChild)
{
    if (elem.hasChildElement() && !ignoreChild) {
        elem = elem.firstChildElement();
        return;
    }
    if (!elem.hasNextSibling()) {
        if (elem.parentNode() == root) {
            elem = SvgElement();
            return;
        }
        while (!elem.parentNode().isNull() && !elem.parentNode().hasNextSibling()) {
            elem = elem.parentElement();
        }
        if (elem != root && elem.parentNode() != root) {
            elem = elem.parentNode().nextSiblingElement();
            return;
        }
    }
    elem = elem.nextSiblingElement();
}

inline SvgNodePrivate* nextNodePrivate(SvgNodePrivate *node, SvgNodePrivate *root)
{
    if (node->first != 0) {
        node = node->first;
        return node;
    }
    if (node->next == 0) {
        if (node->ownerNode == root) {
            node = 0;
            return node;
        }
        while (node->ownerNode != 0 && node->ownerNode->next == 0) {
            node = node->ownerNode;
        }
        if (node != root && node->ownerNode != root) {
            if (node->ownerNode)
                node = node->ownerNode->next;
            else
                node = 0;
            return node;
        }
    }
    node = node->next;
    return node;
}

SvgElement prevElement(const SvgElement &elem)
{
    if (!elem.previousSiblingElement().isNull())
        return elem.previousSiblingElement();
    SvgElement pElem = elem;
    while (pElem.isNull())
        pElem = pElem.parentElement();
    return pElem;
}
