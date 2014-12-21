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

// Private classes declarations

using namespace Element;
using namespace Attribute;

SvgNodePrivate* nextNodePrivate(SvgNodePrivate *node, SvgNodePrivate *root);

namespace Parser {

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
        m_attrExtHash.clear();
        m_id.clear();
        m_name.clear();
        m_value.clear();

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
                if (*str == '[')
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

    bool atEnd() const               { return (str == end || str->isNull()); }
    bool hasError() const            { return m_error != NoError; }
    ErrorType errorId() const        { return m_error; }
    IntHash attributes() const       { return m_attrHash; }
    StringHash attributesExt() const { return m_attrExtHash; }
    QString id() const               { return m_id; }
    QString name() const             { return m_name; }
    QString value() const            { return m_value; }

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
    QString m_id;

    // parser vars
    bool m_isPrevElemEnded;
    ErrorType m_error;
    QString textBuffer;

    enum EndTagType {
        NotEnd,
        EndType1,
        EndType2
    };

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
            if (isSpace(str->unicode())) {
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
        QString attrValue;
        while (!atEnd()) {
            nameLength = 0;
            skipSpaces();
            // data between ' ' and '=' is attribute name
            while (!atEnd() && *str != QL1C('=')) {
                nameLength++;
                ++str;
            }
            // ignore spaces in attribute name
            attrName = QString(str-nameLength, nameLength).trimmed();

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
            attrValue = QString(str-nameLength, nameLength);
            // skip quote char
            str++;
            skipSpaces();

            // ignore empty attributes
            if (!attrValue.isEmpty()) {
                int aId = attrStrToId(attrName);
                if (aId == AttrId::id)
                    m_id = attrValue;
                if (aId != -1)
                    m_attrHash.insert(aId, attrValue);
                else
                    m_attrExtHash.insert(attrName, attrValue);
            }

            EndTagType endType = isEndTag();
            if (endType != NotEnd) {
                if (endType == EndType2)
                   m_isPrevElemEnded = true;
                break;
            }
        }
    }
    inline EndTagType isEndTag(bool skipTag = true) {
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
};
}

class SvgNodePrivate
{
public:
    SvgNodePrivate(SvgDocumentPrivate *, SvgNodePrivate *parent = 0);
    virtual ~SvgNodePrivate();

    bool hasValue() const { return !value.isEmpty(); }

    SvgDocumentPrivate* ownerDocument();
    void setOwnerDocument(SvgDocumentPrivate *doc);

    virtual SvgNodePrivate* insertBefore(SvgNodePrivate *newChild, SvgNodePrivate *refChild);
    virtual SvgNodePrivate* removeChild(SvgNodePrivate *oldChild);
    virtual SvgNodePrivate* appendChild(SvgNodePrivate *newChild);

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
    SvgElementPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &name);
    SvgElementPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent);
    ~SvgElementPrivate() {}
    virtual bool isElement() const { return true; }
    virtual void save(QTextStream &s, int, int) const;

    // Variables
    QString id;
    IntHash attrs;
    // external attributes
    StringHash attrsExt;
};

class SvgCommentPrivate : public SvgNodePrivate
{
public:
    SvgCommentPrivate(SvgDocumentPrivate*, SvgNodePrivate *parent, const QString &val);
    virtual bool isComment() const { return true; }
    virtual void save(QTextStream &s, int, int) const;
};

class SvgTextPrivate : public SvgNodePrivate
{
public:
    SvgTextPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &val);
    virtual bool isText() const { return true; }
    virtual void save(QTextStream &s, int, int) const;
    // vars
    bool isDtd;
};

class SvgDeclarationPrivate : public SvgNodePrivate
{
public:
    SvgDeclarationPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &data);
    virtual bool isDeclaration() const { return true; }
    virtual void save(QTextStream &s, int, int) const;
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

inline void SvgNodePrivate::setOwnerDocument(SvgDocumentPrivate *doc)
{
    ownerNode = doc;
    hasParent = false;
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

SvgNodePrivate* SvgNodePrivate::insertBefore(SvgNodePrivate *newChild, SvgNodePrivate *refChild)
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

SvgNodePrivate* SvgNodePrivate::removeChild(SvgNodePrivate *oldChild)
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

SvgNodePrivate* SvgNodePrivate::appendChild(SvgNodePrivate *newChild)
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

void SvgNodePrivate::save(QTextStream &s, int depth, int indent) const
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

bool SvgNode::operator== (const SvgNode &n) const
{
    return (impl == n.impl);
}

bool SvgNode::operator!= (const SvgNode &n) const
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
    return impl->name;
}

SvgNode SvgNode::parentNode() const
{
    if (!impl)
        return SvgNode();
    return impl->parent();
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
    return impl->first;
}

SvgNode SvgNode::lastChild() const
{
    if (!impl)
        return SvgNode();
    return impl->last;
}

SvgNode SvgNode::previousSibling() const
{
    if (!impl)
        return SvgNode();
    return impl->prev;
}

SvgNode SvgNode::nextSibling() const
{
    if (!impl)
        return SvgNode();
    return impl->next;
}

SvgDocument SvgNode::ownerDocument() const
{
    if (!impl)
        return SvgDocument();
    return impl->ownerDocument();
}

bool SvgNode::hasNextSibling() const
{
    if (!impl)
        return false;
    SvgNodePrivate *p = impl->next;
    // FIXME: ignore isElement check, use it only in hasNextSiblingElement
    while (p) {
        if (p->isElement())
            return true;
        p = p->next;
    }
    return false;
}

SvgNode SvgNode::insertBefore(const SvgNode &newChild, const SvgNode &refChild)
{
    if (!impl)
        return SvgNode();
    return impl->insertBefore(newChild.impl, refChild.impl);
}

SvgNode SvgNode::removeChild(const SvgNode &oldChild)
{
    if (!impl)
        return SvgNode();

    if (oldChild.isNull())
        return SvgNode();

    return impl->removeChild(oldChild.impl);
}

SvgNode SvgNode::appendChild(const SvgNode &newChild)
{
    if (!impl) {
        qWarning("Calling appendChild() on a null node does nothing.");
        return SvgNode();
    }
    return impl->appendChild(newChild.impl);
}

bool SvgNode::hasChildren() const
{
    if (!impl)
        return false;
    return (impl->first != 0);
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

void SvgNode::save(QTextStream &str, int indent) const
{
    if (!impl)
        return;

    if (isDocument()) {
        const SvgNodePrivate* n = impl->first;
        while (n) {
            n->save(str, 0, indent);
            n = n->next;
        }
    } else {
        ((SvgNodePrivate*)impl)->save(str, 1, indent);
    }
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
    if (!impl)
        return SvgElement();
    SvgNodePrivate *p = impl->next;
    while (p) {
        if (p->isElement())
            return SvgElement(((SvgElementPrivate*)p));
        p = p->next;
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

SvgElementPrivate::SvgElementPrivate(SvgDocumentPrivate *d, SvgNodePrivate *p,
                                     const QString &tagname)
    : SvgNodePrivate(d, p)
{
    name = tagname;
}

SvgElementPrivate::SvgElementPrivate(SvgDocumentPrivate *d, SvgNodePrivate *p)
    : SvgNodePrivate(d, p)
{
    createdWithDom1Interface = false;
}

void SvgElementPrivate::save(QTextStream &s, int depth, int indent) const
{
    static const QString tspanElem = QL1S("tspan");
    static const QString startStr  = QL1S("</");
    static const QString endStr    = QL1S("/>");
    static const QString startAttr = QL1S("=\"");

    if (this->name != tspanElem)
        s << QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

    s << QL1C('<') << name;

    if (!attrs.isEmpty() || !attrsExt.isEmpty()) {
        // save default attributes
        IntHash::const_iterator it = attrs.constBegin();
        for (; it != attrs.constEnd(); ++it) {
            // do not save attributes with empty value
            if (!it.value().isEmpty()) {
                s << QL1C(' ');
                Q_ASSERT(attrIdToStr(it.key()).isEmpty() == false);
                Q_ASSERT(it.value().isEmpty() == false);
                s << attrIdToStr(it.key()) << startAttr << it.value() << QL1C('\"');
            }
        }
        // save custom attributes
        StringHash::const_iterator it2 = attrsExt.constBegin();
        for (; it2 != attrsExt.constEnd(); ++it2) {
            // do not save attributes with empty value
            if (!it2.value().isEmpty()) {
                s << QL1C(' ');
                Q_ASSERT(it2.key().isEmpty() == false);
                Q_ASSERT(it2.value().isEmpty() == false);
                s << it2.key() << startAttr << it2.value() << QL1C('\"');
            }
        }

//        // sorted
//        StringMap map;
//        foreach (const int &attrId, attrs.keys())
//            map.insert(attrIdToStr(attrId), attrs.value(attrId));
//        foreach (const QString &attrName, attrsExt.keys())
//            map.insert(attrName, attrsExt.value(attrName));

//        StringMap::const_iterator it3 = map.constBegin();
//        for (; it3 != map.constEnd(); ++it3) {
//            if (!it3.value().isEmpty()) {
//                s << spaceChar;
//                s << it3.key() << startAttr << it3.value() << quoteChar;
//            }
//        }
    }

    if (last) {
        // has child nodes
        if (first->hasValue()) {
            s << QL1C('>');
        } else {
            s << QL1C('>');
            // -1 disables new lines.
            if (indent != -1 && first->name != tspanElem && name != tspanElem)
                s << endl;
        }
        SvgNodePrivate::save(s, depth + 1, indent);
        if (!last->hasValue())
            s << QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

        s << startStr << name << QL1C('>');
    } else {
        if (hasValue()) {
            s << QL1C('>');
            s << value;
            s << startStr << name << QL1C('>');
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

SvgElement::SvgElement(SvgElementPrivate *n) : SvgNode(n) {}

SvgElement& SvgElement::operator= (const SvgElement &x)
{
    return (SvgElement&) SvgNode::operator=(x);
}

void SvgElement::setTagName(const QString &name)
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

QString SvgElement::attribute(const int &attrId, const QString &defValue) const
{
    if (!impl)
        return defValue;
    if (!IMPL->attrs.contains(attrId))
        return defValue;
    return IMPL->attrs.value(attrId);
}

QString SvgElement::attribute(const QString &name, const QString &defValue) const
{
    if (!impl)
        return defValue;
    int aId = attrStrToId(name);
    if (!IMPL->attrs.contains(aId)) {
        if (!IMPL->attrsExt.contains(name))
            return defValue;
        else
            return IMPL->attrsExt.value(name);
    }
    return IMPL->attrs.value(aId);
}

QString SvgElement::extAttribute(const QString &name, const QString &defValue) const
{
    if (!impl)
        return defValue;
    if (!IMPL->attrsExt.contains(name))
        return defValue;
    return IMPL->attrsExt.value(name);
}

void SvgElement::setAttribute(const int &attrId, const QString &value)
{
    Q_ASSERT(attrId != -1);
    if (!impl)
        return;
    if (attrId == AttrId::id)
        IMPL->id = value;
    IMPL->attrs.insert(attrId, value);
}

void SvgElement::setAttribute(const QString &name, const QString &value)
{
    if (!impl)
        return;
    if (name == Attribute::A_id)
        IMPL->id = value;
    if (isDefaultAttribute(name))
        IMPL->attrs.insert(attrStrToId(name), value);
    else
        IMPL->attrsExt.insert(name, value);
}

void SvgElement::setAttributeHash(const IntHash &baseHash, const StringHash &extHash)
{
    if (!impl)
        return;
    IMPL->attrs = baseHash;
    IMPL->attrsExt = extHash;
}

void SvgElement::removeAttribute(int attrId)
{
    if (!impl)
        return;
    if (attrId == AttrId::id)
        IMPL->id.clear();
    IMPL->attrs.remove(attrId);
}

void SvgElement::removeAttribute(const QVariant &name)
{
    if (!impl)
        return;

    int id = -1;
    if (name.type() == QVariant::String) {
        if (!isDefaultAttribute(name.toString())) {
            IMPL->attrsExt.remove(name.toString());
            return;
        } else {
            id = attrStrToId(name.toString());
        }
    }

    if (id == AttrId::id)
        IMPL->id.clear();
    IMPL->attrs.remove(id);
}

bool SvgElement::hasAttribute(int attrId) const
{
    if (!impl)
        return false;
    return IMPL->attrs.contains(attrId);
}

bool SvgElement::hasAttribute(const QString &name) const
{
    if (!impl)
        return false;
    if (isDefaultAttribute(name))
        return IMPL->attrs.contains(attrStrToId(name));
    else
        return IMPL->attrsExt.contains(name);
}

bool SvgElement::hasExtAttribute(const QString &name) const
{
    if (!impl)
        return false;
    return IMPL->attrsExt.contains(name);
}

bool SvgElement::hasAttributes(const IntList &list) const
{
    foreach (const int &attrId, list)
        if (hasAttribute(attrId))
            return true;
    return false;
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
    return IMPL->attrs.size() > 0 || IMPL->attrsExt.size() > 0;
}

QString SvgElement::xlinkId() const
{
    return attribute(AttrId::xlink_href).remove(0,1);
}

QString SvgElement::id() const
{
    if (!impl)
        return QString();
    return IMPL->id;
}

SvgElement SvgElement::removeChild(const SvgElement &oldChild, bool returnPreviousElement)
{
    if (!impl)
        return SvgElement();

    if (oldChild.isNull())
        return SvgElement();

    SvgElement ret;
    if (returnPreviousElement) {
        ret = prevElement(oldChild);
        ((SvgNodePrivate*)impl)->removeChild(oldChild.impl);
    } else {
        ret = SvgNode(((SvgNodePrivate*)impl)->removeChild(oldChild.impl)).toElement();
    }

    return ret;
}

bool SvgElement::hasChildrenElement() const
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
        if (p->isElement())
            count++;
        p = p->next;
    }
    return count;
}

// TODO: remove
StringHash SvgElement::attributesHash(bool ignoreId) const
{
    if (!impl)
        return StringHash();

    StringHash hash = IMPL->attrsExt;
    foreach (const int &id, IMPL->attrs.keys())
        hash.insert(attrIdToStr(id), IMPL->attrs.value(id));
    if (ignoreId)
        hash.remove(Attribute::A_id);
    return hash;
}

void SvgElement::removeAttributeIf(int attrId, const QString &value)
{
    if (!impl)
        return;
    if (IMPL->attrs.value(attrId) == value)
        IMPL->attrs.remove(attrId);
}

void SvgElement::removeAttributeIf(const QString &name, const QString &value)
{
    if (!impl)
        return;
    if (isDefaultAttribute(name)) {
        int id = attrStrToId(name);
        if (IMPL->attrs.value(id) == value)
            IMPL->attrs.remove(id);
    } else {
        if (IMPL->attrsExt.value(name) == value)
            IMPL->attrsExt.remove(name);
    }
}

double SvgElement::doubleAttribute(int attrId)
{
    return toDouble(attribute(attrId));
}

QStringList SvgElement::attributesList() const
{
    if (!impl)
        return QStringList();
    QStringList list;
    foreach (const int &id, IMPL->attrs.keys())
        list << attrIdToStr(id);
    list << IMPL->attrsExt.keys();
    return list;
}

IntList SvgElement::baseAttributesList() const
{
    if (!impl)
        return IntList();
    IntList list;
    foreach (const int &id, IMPL->attrs.keys())
        list << id;
    return list;
}

QStringList SvgElement::extAttributesList() const
{
    if (!impl)
        return QStringList();
    QStringList list;
    foreach (const QString &name, IMPL->attrsExt.keys())
        list << name;
    return list;
}

SvgElement SvgElement::parentElement() const
{
    return parentNode().toElement();
}

// TODO: get rid of it
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
            return SvgElement(((SvgElementPrivate*)p));
        p = p->next;
    }
    return SvgElement();
}

IntList SvgElement::styleAttributesList() const
{
    if (!impl)
        return IntList();
    IntList list;
    list.reserve(attributesCount());
    foreach (const int &id, IMPL->attrs.keys()) {
        if (Properties::presentationAttributesIds.contains(id))
            list << id;
    }
    return list;
}

bool SvgElement::hasLinkedDef() const
{
    if (!impl)
        return false;
    // TODO: is 'filter' needed?
    static const IntList illegalAttrList = IntList()
        << AttrId::clip_path << AttrId::mask << AttrId::filter;
    foreach (const int &id, illegalAttrList) {
        if (hasAttribute(id))
            return true;
    }
    static const IntList illegalStyleAttrList = IntList()
        << AttrId::fill << AttrId::stroke;
    foreach (const int &id, illegalStyleAttrList) {
        if (attribute(id).startsWith(UrlPrefix))
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

bool SvgElement::hasImportantAttrs(const IntList &ignoreList)
{
    int attrCount = attributesCount();
    if (attrCount == 0)
        return false;
    QStringList list = attributesList();
    foreach (const int &attrId, ignoreList)
        list.removeOne(attrIdToStr(attrId));
    if (list.isEmpty())
        return false;
    if (Keys::get().flag(Key::RemoveUnreferencedIds)) {
        if (list.size() == 1 && hasAttribute(AttrId::id))
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
    return hasAttribute(AttrId::used_element);
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
    return IMPL->attrs.size() + IMPL->attrsExt.size();
}

QString SvgElement::defIdFromAttribute(const int &attrId) const
{
    QString id = attribute(attrId);
    if (!id.startsWith(UrlPrefix))
        return QString();
    return id.mid(5, id.size()-6);
}

IntHash SvgElement::styleHash() const
{
    if (!impl)
        return IntHash();
    IntHash hash;
    foreach (const int &id, IMPL->attrs.keys()) {
        if (Properties::presentationAttributesIds.contains(id))
            hash.insert(id, IMPL->attrs.value(id));
    }
    return hash;
}

void SvgElement::removeAttributes(const QStringList &list)
{
    foreach (const QString &text, list)
        removeAttribute(text);
}

void SvgElement::setStylesFromHash(const IntHash &hash)
{
    foreach (const int &attrId, hash.keys())
        setAttribute(attrId, hash.value(attrId));
}

void SvgElement::setTransform(const QString &transform, bool fromParent)
{
    if (hasAttribute(AttrId::transform)) {
        if (fromParent) {
            Transform ts(transform + " " + attribute(AttrId::transform));
            setAttribute(AttrId::transform, ts.simplified());
        } else {
            Transform ts(attribute(AttrId::transform) + " " + transform);
            setAttribute(AttrId::transform, ts.simplified());
        }
    } else {
        setAttribute(AttrId::transform, transform);
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

SvgCommentPrivate::SvgCommentPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent,
                                     const QString &val)
    : SvgNodePrivate(d, parent)
{
    value = val;
}

void SvgCommentPrivate::save(QTextStream &s, int depth, int indent) const
{
    if (indent != -1)
        s << endl;

    // We don't output whitespace if we would pollute a text node.
    if (!(prev && prev->isElement() && !prev->value.isEmpty()))
        s << QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

    s << "<!--" << value;
    if (value.endsWith(QL1C('-')))
        s << ' '; // Ensures that XML comment doesn't end with --->
    s << "-->";

    if (!(next && !next->value.isEmpty()))
        s << endl;
}

SvgComment::SvgComment(SvgCommentPrivate *n) : SvgNode(n) {}

SvgComment& SvgComment::operator= (const SvgComment &x)
{
    return (SvgComment&) SvgNode::operator=(x);
}

SvgTextPrivate::SvgTextPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent, const QString &val)
    : SvgNodePrivate(d, parent)
{
    value = val;
    isDtd = false;
}

void SvgTextPrivate::save(QTextStream &s, int /*depth*/, int indent) const
{
    if (isDtd && indent != -1)
        s << endl;
    s << value;
    if (isDtd && indent != -1)
        s << endl;
}

SvgText::SvgText(SvgTextPrivate *n) : SvgNode(n) {}

SvgText& SvgText::operator= (const SvgText &x)
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

SvgDeclarationPrivate::SvgDeclarationPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent,
                                             const QString &data)
    : SvgNodePrivate(d, parent)
{
    value = data;
}

void SvgDeclarationPrivate::save(QTextStream &s, int, int indent) const
{
    s << "<?" << value << "?>";
    if (indent != -1)
        s << endl;
}

SvgDeclaration& SvgDeclaration::operator= (const SvgDeclaration &x)
{
    return (SvgDeclaration&) SvgNode::operator=(x);
}

QString SvgDeclaration::data() const
{
    if (!impl)
        return QString();
    return impl->value;
}

void SvgDeclaration::setData(const QString &d)
{
    if (!impl)
        return;
    impl->value = d;
}

SvgDeclaration::SvgDeclaration(SvgDeclarationPrivate *n) : SvgNode(n) {}
SvgDocumentPrivate::SvgDocumentPrivate() : SvgNodePrivate(0) {}

void SvgDocumentPrivate::clear()
{
    SvgNodePrivate::clear();
}

#define IMPL ((SvgDocumentPrivate*)impl)

SvgDocument::SvgDocument(SvgDocumentPrivate *x)
    : SvgNode(x)
{
}

SvgDocument& SvgDocument::operator= (const SvgDocument &x)
{
    return (SvgDocument&) SvgNode::operator=(x);
}

bool SvgDocument::loadFile(const QString &filePath)
{
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
            elem.setAttribute(AttrId::id, reader.id());
            elem.setAttributeHash(reader.attributes(), reader.attributesExt());
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

SvgElement SvgDocument::createElement(const QString &tagName)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgElementPrivate *e = new SvgElementPrivate(IMPL, 0, tagName);
    e->ref.deref();
    return e;
}

SvgComment SvgDocument::createComment(const QString &value)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgCommentPrivate *c = new SvgCommentPrivate(IMPL, 0, value);
    c->ref.deref();
    return c;
}

SvgDeclaration SvgDocument::createDeclaration(const QString &data)
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

void SvgDocument::calcElemAttrCount(const QString &text)
{
    quint32 elemCount = 0;
    quint32 attrCount = 0;

    SvgElement elem = documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        elemCount++;
        attrCount += elem.attributesCount();
        nextElement(elem, root);
    }
    if (!Keys::get().flag(Key::ShortOutput)) {
        qDebug("The %s number of elements is: %u",   qPrintable(text), elemCount);
        qDebug("The %s number of attributes is: %u", qPrintable(text), attrCount);
    } else {
        qDebug("%u", elemCount);
        qDebug("%u", attrCount);
    }
}

#undef IMPL

void nextElement(SvgElement &elem, const SvgElement &root, bool ignoreChild)
{
    if (elem.hasChildrenElement() && !ignoreChild) {
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
    SvgElement pElem = elem.parentElement();
    while (pElem.isNull())
        pElem = pElem.parentElement();
    return pElem;
}
