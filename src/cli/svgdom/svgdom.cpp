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

/*
 * SvgDom classes are mix of Qt DOM implementation:
 * - only implicitly/explicitly shared classes
 * - classes structure
 * - some functions implementations
 * and TinyXML2:
 * - parser logic
 * - node detecting
 *
 * The main difference that SvgDom:
 * - works only with well-formed SVG files
 * - Qt based
 * - UTF-8 only
 * - written specifically for SVG
 */

#include <QAtomicInt>
#include <QFile>

#include "../enums.h"
#include "../tools.h"
#include "svgparser.h"
#include "svgdom.h"

#define ToDoc(x) static_cast<SvgDocumentPrivate *>(x)
#define ToElem(x) static_cast<SvgElementPrivate *>(x)

// Private classes declarations

using namespace Element;
using namespace Attribute;

SvgNodePrivate* nextNodePrivate(SvgNodePrivate *node, SvgNodePrivate *root);

class SvgNodePrivate
{
public:
    SvgNodePrivate(SvgDocumentPrivate *, SvgNodePrivate *parent = 0);
    virtual ~SvgNodePrivate();

    SvgDocumentPrivate* ownerDocument();
    void setOwnerDocument(SvgDocumentPrivate *doc);

    virtual SvgNodePrivate* insertBefore(SvgNodePrivate *newChild, SvgNodePrivate *refChild);
    virtual void removeChild(SvgNodePrivate *oldChild);
    virtual SvgNodePrivate* appendChild(SvgNodePrivate *newChild);

    // Dynamic cast
    virtual bool isDocument() const    { return false; }
    virtual bool isElement() const     { return false; }
    virtual bool isDeclaration() const { return false; }
    virtual bool isComment() const     { return false; }
    virtual bool isText() const        { return false; }

    virtual void save(QString &s, int depth, int indent) const;

    // vars
    QAtomicInt ref;
    SvgNodePrivate* prev;
    SvgNodePrivate* next;
    SvgNodePrivate* ownerNode; // either the node's parent or the node's owner document
    SvgNodePrivate* first;
    SvgNodePrivate* last;
};

class SvgElementPrivate : public SvgNodePrivate
{
public:
    SvgElementPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &name);
    bool isElement() const { return true; }
    void save(QString &s, int depth, int indent) const;

    // vars
    QString name; // tag name
    SvgAttributeHash attributes; // attributes hash
    // internal
    QList<SvgElement> linkedElemList; // list of elements linked to this element
    Transform bboxTransform; // contains bounding box transform of the element
};

class SvgCommentPrivate : public SvgNodePrivate
{
public:
    SvgCommentPrivate(SvgDocumentPrivate*, SvgNodePrivate *parent, const QString &val);
    bool isComment() const { return true; }
    void save(QString &s, int depth, int indent) const;

    // vars
    QString value;
};

class SvgTextPrivate : public SvgNodePrivate
{
public:
    SvgTextPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &val);
    bool isText() const { return true; }
    void save(QString &s, int depth, int indent) const;

    // vars
    bool isDtd;
    QString value;
};

class SvgDeclarationPrivate : public SvgNodePrivate
{
public:
    SvgDeclarationPrivate(SvgDocumentPrivate *, SvgNodePrivate *parent, const QString &data);
    bool isDeclaration() const { return true; }
    void save(QString &s, int depth, int indent) const;

    // vars
    QString value;
};

class SvgDocumentPrivate : public SvgNodePrivate
{
public:
    SvgDocumentPrivate();
    bool isDocument() const { return true; }

    // vars:
    QString lastError;
    bool blockStringUrls;
    uint genIdCount;
};

SvgNodePrivate::SvgNodePrivate(SvgDocumentPrivate *doc, SvgNodePrivate *par)
{
    ref = 1;
    if (par)
        ownerNode = par;
    else
        setOwnerDocument(doc);
    prev = 0;
    next = 0;
    first = 0;
    last = 0;
}

SvgNodePrivate::~SvgNodePrivate()
{
    SvgNodePrivate* p = first;
    SvgNodePrivate* n;

    while (p) {
        n = p->next;

        // remove all links before node delete or detach
        if (p->isElement()) {
            SvgElementPrivate *e = ToElem(p);
            if (!e->linkedElemList.isEmpty()) {
                e->attributes.clear();
                e->linkedElemList.clear();
            }
        }

        if (!p->ref.deref())
            delete p;
        else
            p->ownerNode = 0;
        p = n;
    }
    first = 0;
    last = 0;
}

inline void SvgNodePrivate::setOwnerDocument(SvgDocumentPrivate *doc)
{
    ownerNode = doc;
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
    if (refChild && refChild->ownerNode != this) {
        qWarning("insertBefore() works only on direct child.");
        return 0;
    }

    // No more errors can occur now, so we take
    // ownership of the node.
    newChild->ref.ref();

    if (newChild->ownerNode)
        newChild->ownerNode->removeChild(newChild);

    newChild->ownerNode = this;

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

void SvgNodePrivate::removeChild(SvgNodePrivate *oldChild)
{
    // Error check
    if (oldChild->ownerNode != this) {
        qWarning("removeChild() works only on direct child.");
        return;
    }

    // Perhaps oldChild was just created with "createElement" or that. In this case
    // its parent is SvgDocument but it is not part of the documents child list.
    if (oldChild->next == 0 && oldChild->prev == 0 && first != oldChild)
        return;

    if (oldChild->next)
        oldChild->next->prev = oldChild->prev;
    if (oldChild->prev)
        oldChild->prev->next = oldChild->next;

    if (last == oldChild)
        last = oldChild->prev;
    if (first == oldChild)
        first = oldChild->next;

    oldChild->ownerNode = 0;
    oldChild->next = 0;
    oldChild->prev = 0;

    // We are no longer interested in the old node
    oldChild->ref.deref();
}

SvgNodePrivate* SvgNodePrivate::appendChild(SvgNodePrivate *newChild)
{
    if (!newChild)
        return 0;

    // Release new node from its current parent
    if (newChild->ownerNode)
        newChild->ownerNode->removeChild(newChild);

    // No more errors can occur now, so we take
    // ownership of the node
    newChild->ref.ref();

    newChild->ownerNode = this;

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
        if (!p)
            return (SvgDocumentPrivate*)p->ownerNode;
        p = p->ownerNode;
    }
    return ToDoc(p);
}

void SvgNodePrivate::save(QString &s, int depth, int indent) const
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

SvgNode SvgNode::parentNode() const
{
    CheckData(SvgNode());
    return impl->ownerNode;
}

SvgNodeList SvgNode::childNodes() const
{
    CheckData(SvgNodeList());
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
    CheckData(SvgNode());
    return impl->first;
}

SvgNode SvgNode::previousSibling() const
{
    CheckData(SvgNode());
    return impl->prev;
}

SvgNode SvgNode::nextSibling() const
{
    CheckData(SvgNode());
    return impl->next;
}

SvgDocument SvgNode::ownerDocument() const
{
    CheckData(SvgDocument());
    return impl->ownerDocument();
}

bool SvgNode::hasNextSibling() const
{
    CheckData(false);
    SvgNodePrivate *p = impl->next;
    while (p) {
        if (p->isElement())
            return true;
        p = p->next;
    }
    return false;
}

SvgNode SvgNode::insertBefore(const SvgNode &newChild, const SvgNode &refChild)
{
    CheckData(SvgNode());
    return impl->insertBefore(newChild.impl, refChild.impl);
}

void SvgNode::removeChild(const SvgNode &oldChild)
{
    CheckData();

    if (oldChild.isNull())
        return;

    impl->removeChild(oldChild.impl);
}

SvgNode SvgNode::appendChild(const SvgNode &newChild)
{
    if (!impl) {
        qWarning("calling appendChild() on a null node does nothing.");
        return SvgNode();
    }
    return impl->appendChild(newChild.impl);
}

bool SvgNode::hasChildren() const
{
    CheckData(false);
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

void SvgNode::save(QString &str, int indent) const
{
    CheckData();
    if (isDocument()) {
        const SvgNodePrivate* n = impl->first;
        while (n) {
            n->save(str, 0, indent);
            n = n->next;
        }
    } else {
        impl->save(str, 1, indent);
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
    CheckData(false);
    return impl->isElement();
}

bool SvgNode::isText() const
{
    CheckData(false);
    return impl->isText();
}

bool SvgNode::isDeclaration() const
{
    CheckData(false);
    return impl->isDeclaration();
}

bool SvgNode::isComment() const
{
    CheckData(false);
    return impl->isComment();
}

bool SvgNode::hasText() const
{
    CheckData(false);
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
    CheckData(SvgElement());
    SvgNodePrivate *p = impl->next;
    while (p) {
        if (p->isElement())
            return SvgElement(((SvgElementPrivate*)p));
        p = p->next;
    }
    return SvgElement();
}

SvgNode SvgNode::nextNode(const SvgNode &root) const
{
    CheckData(SvgNode());
    return nextNodePrivate(impl, root.implementation());
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
        return SvgDocument(static_cast<SvgDocumentPrivate*>(impl));
    return SvgDocument();
}

SvgElement SvgNode::toElement() const
{
    if (impl && impl->isElement())
        return SvgElement(static_cast<SvgElementPrivate*>(impl));
    return SvgElement();
}

SvgDeclaration SvgNode::toDeclaration() const
{
    if (impl && impl->isDeclaration())
        return SvgDeclaration(static_cast<SvgDeclarationPrivate*>(impl));
    return SvgDeclaration();
}

SvgComment SvgNode::toComment() const
{
    if (impl && impl->isComment())
        return SvgComment(static_cast<SvgCommentPrivate*>(impl));
    return SvgComment();
}

SvgText SvgNode::toText() const
{
    if (impl && impl->isText())
        return SvgText(static_cast<SvgTextPrivate*>(impl));
    return SvgText();
}

SvgElementPrivate::SvgElementPrivate(SvgDocumentPrivate *d, SvgNodePrivate *p,
                                     const QString &tagname)
    : SvgNodePrivate(d, p),
      name(tagname)
{
}

void appendAttribute(QString &s, const QString &name, const QString &value)
{
    s += QL1C(' ') + name + QL1S("=\"") + value + QL1C('\"');
}

// TODO: sort attributes
void SvgElementPrivate::save(QString &s, int depth, int indent) const
{
    static const QString startStr  = QL1S("</");
    static const QString endStr    = QL1S("/>");
    static const QString urlPrefix = QL1S("url(#");

    const QString indentStr = QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

    if (this->name != E_tspan)
        s += indentStr;

    s += QL1C('<') + name;

    // save default attributes
    SvgAttributeHash::const_iterator it = attributes.constBegin();
    for (; it != attributes.constEnd(); ++it) {
        const SvgAttribute a = it.value();
        // do not save attributes with empty value
        if (a.isReference()) {
            Q_ASSERT_X(a.referencedElement().isUsed() == true, "link saving",
                       "linked element cannot be unused");

            QString value;
            if (a.id() == AttrId::xlink_href)
                value = QL1C('#') + a.referencedElement().id();
            else
                value = urlPrefix + a.referencedElement().id() + QL1C(')');
            appendAttribute(s, attrIdToStr(a.id()), value);
        } else if (a.isTransform()) {
            QString transformName;
            if (name == E_linearGradient || name == E_radialGradient)
                transformName = A_gradientTransform;
            else if (name == E_pattern)
                transformName = A_patternTransform;
            else
                transformName = A_transform;

            QString ts = a.transform().simplified();
            if (!ts.isEmpty())
                appendAttribute(s, transformName, ts);
        } else if (a.isDefault()) {
            if (!a.value().isEmpty())
                appendAttribute(s, attrIdToStr(a.id()), a.value());
        } else if (a.isExternal()) {
            if (!a.value().isEmpty())
                appendAttribute(s, a.name(), a.value());
            else
                qWarning("attribute '%s' cannot be empty", qPrintable(a.name()));
        } else {
            qWarning("invalid attribute type");
        }
    }

    if (last) {
        s += QL1C('>');
        // -1 disables new lines.
        if (indent != -1 && (first->isElement() && ToElem(first)->name != E_tspan) && name != E_tspan)
            s += QL1C('\n');
        SvgNodePrivate::save(s, depth + 1, indent);
        if (last->isElement())
            s += indentStr;

        s += startStr + name + QL1C('>');
    } else {
        s += endStr;
    }
    if (next) {
        if (next->isElement() && ToElem(next)->name != E_tspan) {
            if (indent != -1)
                s += QL1C('\n');
        }
    } else {
        if (indent != -1)
            s += QL1C('\n');
    }
}

SvgElement::SvgElement(SvgElementPrivate *n) : SvgNode(n) {}

SvgElement& SvgElement::operator= (const SvgElement &x)
{
    return (SvgElement&) SvgNode::operator=(x);
}

SvgElement SvgElement::nextElement(const SvgElement &root, bool ignoreChild) const
{
    CheckData(SvgElement());
    return ToElem(nextElementPrivate(ToElem(impl), root.implementation(), ignoreChild));
}

void SvgElement::setTagName(const QString &name)
{
    if (impl)
        ToElem(impl)->name = name;
}

QString SvgElement::tagName() const
{
    CheckData(QString());
    return ToElem(impl)->name;
}

QString SvgElement::attribute(const uint &attrId, const QString &defValue) const
{
    CheckData(defValue);
    return ToElem(impl)->attributes.value(attrId).value();
}

QString SvgElement::attribute(const QString &name, const QString &defValue) const
{
    return attribute(qHash(name), defValue);
}

SvgAttribute SvgElement::attributeItem(const uint &attrId) const
{
    CheckData(SvgAttribute());
    foreach (const SvgAttribute &a, ToElem(impl)->attributes) {
        if (a.id() == attrId)
            return a;
    }
    return SvgAttribute();
}

void SvgElement::setAttribute(const uint &attrId, const QString &value)
{
    Q_ASSERT(attrId != 0);
    CheckData();

    // string attribute value cannot be an url
    if (ownerDocument().isBlockStringUrls()) {
        if (value.startsWith(QL1S("url(#")))
            qFatal("attribute value cannot contain url");
        else if (attrId == AttrId::xlink_href)
            qFatal("attribute value cannot contain xlink:href");
    }

    if (ToElem(impl)->attributes.contains(attrId)) {
        SvgAttribute attr = ToElem(impl)->attributes.value(attrId);
        attr.setValue(value);
    } else {
        ToElem(impl)->attributes.insert(attrId, SvgAttribute(attrId, value));
    }
}

void SvgElement::setAttribute(const QString &name, const QString &value)
{
    if (value.isEmpty())
        return;
    CheckData();
    if (isDefaultAttribute(name))
        setAttribute(attrStrToId(name), value);

    // string attribute value cannot be an url
    if (ownerDocument().isBlockStringUrls())
        Q_ASSERT(value.startsWith(QL1S("url(#")) == false);

    uint attrId = qHash(name);
    if (ToElem(impl)->attributes.contains(attrId)) {
        SvgAttribute attr = ToElem(impl)->attributes.value(attrId);
        attr.setValue(value);
    } else {
        ToElem(impl)->attributes.insert(attrId, SvgAttribute(name, value));
    }
}

void SvgElement::setAttribute(const SvgAttribute &attr)
{
    CheckData();
    if (attr.isDefault())
        setAttribute(attr.id(), attr.value());
    else if (attr.isExternal())
        setAttribute(attr.name(), attr.value());
    else if (attr.isTransform())
        setTransform(attr.transform());
    else if (attr.isReference())
        setReferenceElement(attr.id(), attr.referencedElement());
    else
        qFatal("invalid attribute");
}

void SvgElement::removeAttribute(uint attrId)
{
    CheckData();
    ToElem(impl)->attributes.remove(attrId);
}

void SvgElement::removeAttribute(const QString &name)
{
    CheckData();
    ToElem(impl)->attributes.remove(qHash(name));
}

bool SvgElement::hasAttribute(uint attrId) const
{
    CheckData(false);
    return ToElem(impl)->attributes.contains(attrId);
}

bool SvgElement::hasAttribute(const QString &name) const
{
    CheckData(false);
    return ToElem(impl)->attributes.contains(qHash(name));
}

bool SvgElement::hasAttributes(const IntList &list) const
{
    foreach (const uint &attrId, list)
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
    CheckData(false);
    return !ToElem(impl)->attributes.isEmpty();
}

QString SvgElement::id() const
{
    CheckData(QString());
    return ToElem(impl)->attributes.value(AttrId::id).value();
}

SvgElement SvgElement::removeChild(const SvgElement &oldChild, bool returnPrevious)
{
    CheckData(SvgElement());

    if (oldChild.isNull())
        return SvgElement();

    SvgElement ret;
    if (returnPrevious)
        ret = prevElement(oldChild);
    impl->removeChild(oldChild.impl);
    return ret;
}

bool SvgElement::hasChildrenElement() const
{
    CheckData(false);
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
    CheckData(0);
    SvgNodePrivate* p = impl->first;
    int count = 0;
    while (p) {
        if (p->isElement())
            count++;
        p = p->next;
    }
    return count;
}

void SvgElement::removeAttributeIf(uint attrId, const QString &value)
{
    CheckData();
    if (attribute(attrId) == value)
        removeAttribute(attrId);
}

void SvgElement::removeAttributeIf(const QString &name, const QString &value)
{
    CheckData();
    if (attribute(name) == value)
        removeAttribute(name);
}

double SvgElement::doubleAttribute(uint attrId)
{
    return toDouble(attribute(attrId));
}

IntList SvgElement::baseAttributesList() const
{
    CheckData(IntList());
    IntList list;
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isDefault())
            list << attr.id();
    }
    return list;
}

QStringList SvgElement::extAttributesList() const
{
    CheckData(QStringList());
    QStringList list;
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isExternal())
            list << attr.name();
    }
    return list;
}

SvgAttributeList SvgElement::attributesList() const
{
    CheckData(SvgAttributeList());
    return ToElem(impl)->attributes.values();
}

QStringList SvgElement::attributesNamesList() const
{
    CheckData(QStringList());
    QStringList list;
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        list << attr.name();
    }
    return list;
}

SvgElement SvgElement::parentElement() const
{
    return parentNode().toElement();
}

// TODO: get rid of it
SvgElementList SvgElement::childElements() const
{
    CheckData(SvgElementList());
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
    CheckData(SvgElement());
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
    CheckData(IntList());
    IntList list;
    SvgElementPrivate *e = ToElem(impl);
    list.reserve(e->attributes.size());
    foreach (const SvgAttribute &a, e->attributes) {
        if (a.isStyle())
            list << a.id();
    }
    return list;
}

int SvgElement::usesCount() const
{
    CheckData(0);
    return ToElem(impl)->linkedElemList.size();
}

bool SvgElement::hasReference(uint attrId) const
{
    CheckData(false);
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isReference() && attr.id() == attrId)
            return true;
    }
    return false;
}

SvgElement SvgElement::referencedElement(uint attrId) const
{
    CheckData(SvgElement());
    return ToElem(impl)->attributes.value(attrId).referencedElement();
}

void SvgElement::setReferenceElement(uint attrId, const SvgElement &elem)
{
    CheckData();

    if (elem.isNull()) {
        referencedElement(attrId).removeLinkedElement(id());
        ToElem(impl)->attributes.remove(attrId);
        return;
    }

    ToElem(impl)->attributes.insert(attrId, SvgAttribute(attrId, elem));
    if (!elem.isNull())
        referencedElement(attrId).appendLinkedElement(attrId, *this);
}

IntList SvgElement::referencedAttributes() const
{
    CheckData(IntList());
    IntList list;
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isReference())
            list << attr.id();
    }
    return list;
}

uint SvgElement::referenceAttribute(const SvgElement &elem) const
{
    CheckData(0);
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isReference() && attr.referencedElement() == elem)
            return attr.id();
    }
    return 0;
}

void SvgElement::removeReferenceElement(uint attrId)
{
    setReferenceElement(attrId, SvgElement());
}

void SvgElement::appendLinkedElement(const uint attrId, const SvgElement &elem)
{
    CheckData();
    ToElem(impl)->linkedElemList.insert(attrId, elem);
}

void SvgElement::removeLinkedElement(const QString &id)
{
    CheckData();

    uint listSize = ToElem(impl)->linkedElemList.size();
    for (uint i = 0; i < listSize; ++i) {
        if (ToElem(impl)->linkedElemList.at(i).id() == id) {
            ToElem(impl)->linkedElemList.removeAt(i);
            break;
        }
    }
}

SvgElementList SvgElement::linkedElements() const
{
    return ToElem(impl)->linkedElemList;
}

bool SvgElement::hasParentAttribute(uint attrId, bool includeCurrentElem)
{
    CheckData(false);
    SvgNodePrivate *parent = 0;
    if (includeCurrentElem)
        parent = impl;
    else
        parent = impl->ownerNode;

    while (parent) {
        if (parent->isElement()) {
            SvgElementPrivate *e = ((SvgElementPrivate *)parent);
            if (e->attributes.contains(attrId))
                return true;
        }
        parent = parent->ownerNode;
    }
    return false;
}

QString SvgElement::parentAttribute(uint attrId, bool includeCurrentElem)
{
    CheckData(QString());
    SvgNodePrivate *parent = 0;
    if (includeCurrentElem)
        parent = impl;
    else
        parent = impl->ownerNode;

    while (parent) {
        if (parent->isElement()) {
            SvgElementPrivate *e = ((SvgElementPrivate *)parent);
            if (e->attributes.contains(attrId)) {
                return e->attributes.value(attrId).value();
            }
        }
        parent = parent->ownerNode;
    }
    return QString();
}

bool SvgElement::hasReferencedDefs() const
{
    CheckData(false);
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isReference())
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
    CheckData(false);

    int attrCount = attributesCount();
    if (attrCount == 0)
        return false;

    IntList list = ToElem(impl)->attributes.keys();
    foreach (const uint &attrId, ignoreList)
        list.removeOne(attrId);

    if (list.isEmpty())
        return false;

    if (Keys::get().flag(Key::RemoveUnreferencedIds)) {
        if (list.size() == 1 && hasAttribute(AttrId::id))
            return false;
    }
    return true;
}

bool SvgElement::isUsed() const
{
    CheckData(false);
    return usesCount() > 0;
}

bool SvgElement::hasTextChild() const
{
    CheckData(false);
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
    CheckData(0);
    return ToElem(impl)->attributes.size();
}

QString SvgElement::defIdFromAttribute(const uint &attrId) const
{
    CheckData(QString());
    if (ownerDocument().isBlockStringUrls())
        return ToElem(impl)->attributes.value(attrId).referencedElement().id();

    QString id = attribute(attrId);
    if (!id.startsWith(UrlPrefix))
        return QString();
    return id.mid(5, id.size()-6);
}

SvgAttributeHash SvgElement::styleHash() const
{
    CheckData(SvgAttributeHash());
    SvgAttributeHash hash;
    foreach (const SvgAttribute &attr, ToElem(impl)->attributes) {
        if (attr.isStyle())
            hash.insert(attr.id(), attr);
    }
    return hash;
}

void SvgElement::removeAttributes(const QStringList &list)
{
    foreach (const QString &text, list)
        removeAttribute(text);
}

bool SvgElement::hasTransform() const
{
    CheckData(false);
    return ToElem(impl)->attributes.value(AttrId::transform).transform().isValid();
}

void SvgElement::setTransform(const Transform &atransform, bool fromParent)
{
    Transform ts;
    if (hasTransform()) {
        if (fromParent) {
            ts = atransform;
            ts.append(transform());
        } else {
            ts = transform();
            ts.append(atransform);
        }
    } else {
        ts = atransform;
    }

    if (!ts.isValid()) {
        removeTransform();
        return;
    }

    ToElem(impl)->attributes.insert(AttrId::transform, SvgAttribute(ts));
}

void SvgElement::removeTransform()
{
    CheckData();
    ToElem(impl)->attributes.remove(AttrId::transform);
}

Transform SvgElement::transform() const
{
    CheckData(Transform());
    return ToElem(impl)->attributes.value(AttrId::transform).transform();
}

void SvgElement::setBBoxTransform(const Transform &transform)
{
    ToElem(impl)->bboxTransform = transform;
}

Transform SvgElement::bboxTransform() const
{
    return ToElem(impl)->bboxTransform;
}

QString SvgElement::text() const
{
    CheckData(QString());
    SvgNodePrivate *first = ToElem(impl)->first;
    if (first != 0) {
        SvgNodePrivate* p = first;
        QString str;
        while (p) {
            if (p->isText())
                str += ((SvgTextPrivate*)p)->value;
            p = p->next;
        }
        return str;
    }
    return QString();
}

SvgNodePrivate* _nextSiblingElement(SvgNodePrivate *node)
{
    SvgNodePrivate *p = node->next;
    while (p) {
        if (p->isElement())
            return p;
        p = p->next;
    }
    return 0;
}

SvgNodePrivate* SvgElement::nextElementPrivate(SvgNodePrivate *node, SvgNodePrivate *root,
                                               bool ignoreChild)
{
    SvgNodePrivate *bn = node;

    if (node->first != 0 && !ignoreChild) {
        node = node->first;
        while (node) {
            if (node->isElement()) {
                break;
            }
            node = node->next;
        }
        if (node)
            return node;
        else
            node = bn;
    }

    if (_nextSiblingElement(node) == 0) {
        if (node->ownerNode == root) {
            node = 0;
            return node;
        }
        while (node->ownerNode != 0 && _nextSiblingElement(node->ownerNode) == 0) {
            node = node->ownerNode;
        }
        if (node != root && node->ownerNode != root) {
            if (node->ownerNode)
                node = _nextSiblingElement(node->ownerNode);
            else
                node = 0;
            return node;
        }
    } else if (node == root) {
        return 0;
    }

    node = _nextSiblingElement(node);
    return node;
}

SvgCommentPrivate::SvgCommentPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent,
                                     const QString &val)
    : SvgNodePrivate(d, parent),
      value(val)
{
}

void SvgCommentPrivate::save(QString &s, int depth, int indent) const
{
    if (indent != -1)
        s += QL1C('\n');

    // We don't output whitespace if we would pollute a text node.
    if (!(prev && prev->isElement()/* && !prev->value.isEmpty()*/))
        s += QString(indent < 1 ? 0 : depth * indent, QL1C(' '));

    s += QL1S("<!--") + value;
    if (value.endsWith(QL1C('-')))
        s += QL1C(' '); // Ensures that XML comment doesn't end with --->
    s += QL1S("-->");

    if (!(next && !next->isElement() /* && !next->value.isEmpty()*/))
        s += QL1C('\n');
}

SvgComment::SvgComment(SvgCommentPrivate *n) : SvgNode(n) {}

SvgComment& SvgComment::operator= (const SvgComment &x)
{
    return (SvgComment&) SvgNode::operator=(x);
}

QString SvgComment::data() const
{
    CheckData(QString());
    return ((SvgCommentPrivate*)impl)->value;
}

SvgTextPrivate::SvgTextPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent, const QString &val)
    : SvgNodePrivate(d, parent),
      isDtd(false),
      value(val)
{
}

void SvgTextPrivate::save(QString &s, int /*depth*/, int indent) const
{
    s += value;
    if (isDtd && indent != -1)
        s += QL1C('\n');
}

SvgText::SvgText(SvgTextPrivate *n) : SvgNode(n) {}

SvgText& SvgText::operator= (const SvgText &x)
{
    return (SvgText&) SvgNode::operator=(x);
}

QString SvgText::text() const
{
    CheckData(QString());
    return ((SvgTextPrivate*)impl)->value;
}

void SvgText::setDtd(bool flag)
{
    CheckData();
    ((SvgTextPrivate*)impl)->isDtd = flag;
}

SvgDeclarationPrivate::SvgDeclarationPrivate(SvgDocumentPrivate *d, SvgNodePrivate *parent,
                                             const QString &data)
    : SvgNodePrivate(d, parent),
      value(data)
{
}

void SvgDeclarationPrivate::save(QString &s, int, int indent) const
{
    s += QL1S("<?") + value + QL1S("?>");
    if (indent != -1)
        s += QL1C('\n');
}

SvgDeclaration& SvgDeclaration::operator= (const SvgDeclaration &x)
{
    return (SvgDeclaration&) SvgNode::operator=(x);
}

QString SvgDeclaration::data() const
{
    CheckData(QString());
    return ((SvgDeclarationPrivate*)impl)->value;
}

void SvgDeclaration::setData(const QString &d)
{
    CheckData();
    ((SvgDeclarationPrivate*)impl)->value = d;
}

SvgDeclaration::SvgDeclaration(SvgDeclarationPrivate *n) : SvgNode(n) {}

SvgDocumentPrivate::SvgDocumentPrivate() : SvgNodePrivate(0)
{
    blockStringUrls = false;
    genIdCount = 0;
}

SvgDocument::SvgDocument(SvgDocumentPrivate *x) : SvgNode(x) {}

SvgDocument& SvgDocument::operator= (const SvgDocument &x)
{
    return (SvgDocument&) SvgNode::operator=(x);
}

bool SvgDocument::loadFile(const QString &filePath)
{
    if (!impl)
        impl = new SvgDocumentPrivate();

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        ToDoc(impl)->lastError = QL1S("could not open svg file");
        return false;
    }

    // TODO: create elements inside parser
    return fromString(QString::fromUtf8(file.readAll()));
}

bool SvgDocument::fromString(const QString &text)
{
    if (!impl)
        impl = new SvgDocumentPrivate();

    SvgNode node(impl);
    SvgParser reader(text);
    while (!reader.atEnd() && !reader.hasError()) {
        SvgParser::TokenType token = reader.readNext();
        if (token == SvgParser::ProcessingInstruction) {
            node.appendChild(createDeclaration(reader.value()));
        } else if (token == SvgParser::StartElement) {
            SvgElement elem = createElement(reader.name());
            node.appendChild(elem);
            foreach (int attrId, reader.attributes().keys())
                elem.setAttribute(attrId, reader.attributes().value(attrId));
            foreach (const QString &name, reader.attributesExt().keys())
                elem.setAttribute(name, reader.attributesExt().value(name));
            elem.setTransform(reader.transform());
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
        if (reader.errorId() == SvgParser::ParseError)
            ToDoc(impl)->lastError = QL1S("could not parse svg file");
        return false;
    }

    return true;
}

const QString SvgDocument::toString(int indent) const
{
    QString str;
    save(str, indent);
    return str;
}

QString SvgDocument::lastError() const
{
    CheckData(QString());
    return ToDoc(impl)->lastError;
}

SvgElement SvgDocument::documentElement() const
{
    CheckData(SvgElement());
    SvgNodePrivate *p = impl->first;
    while (p && !p->isElement())
        p = p->next;
    return static_cast<SvgElementPrivate *>(p);
}

void SvgDocument::setBlockStringUrls(bool flag)
{
    CheckData();
    ToDoc(impl)->blockStringUrls = flag;
}

bool SvgDocument::isBlockStringUrls() const
{
    CheckData(false);
    return ToDoc(impl)->blockStringUrls;
}

QString SvgDocument::takeFreeId()
{
    int id = ToDoc(impl)->genIdCount;
    ToDoc(impl)->genIdCount++;
    return QString::number(id);
}

SvgElement SvgDocument::createElement(const QString &tagName)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgElementPrivate *e = new SvgElementPrivate(ToDoc(impl), 0, tagName);
    e->ref.deref();
    return e;
}

SvgComment SvgDocument::createComment(const QString &value)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgCommentPrivate *c = new SvgCommentPrivate(ToDoc(impl), 0, value);
    c->ref.deref();
    return c;
}

SvgDeclaration SvgDocument::createDeclaration(const QString &data)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgDeclarationPrivate *p = new SvgDeclarationPrivate(ToDoc(impl), 0, data);
    p->ref.deref();
    return p;
}

// TODO: store comments and declaration as text
SvgText SvgDocument::createText(const QString &text)
{
    if (!impl)
        impl = new SvgDocumentPrivate();
    SvgTextPrivate *p = new SvgTextPrivate(ToDoc(impl), 0, text);
    p->ref.deref();
    return p;
}

void SvgDocument::calcElemAttrCount(const QString &text)
{
    quint32 elemCount = 0;
    quint32 attrCount = 0;

    element_loop (documentElement()) {
        elemCount++;
        attrCount += elem.attributesCount();
    }
    if (!Keys::get().flag(Key::ShortOutput)) {
        qDebug("The %s number of elements is: %u",   qPrintable(text), elemCount);
        qDebug("The %s number of attributes is: %u", qPrintable(text), attrCount);
    } else {
        qDebug("%u", elemCount);
        qDebug("%u", attrCount);
    }
}

void nextElement(SvgElement &elem, const SvgElement &root, bool ignoreChild)
{
    elem = elem.nextElement(root, ignoreChild);
}

SvgNodePrivate* nextNodePrivate(SvgNodePrivate *node, SvgNodePrivate *root)
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

SvgElement prevSiblingElement(const SvgElement &elem)
{
    SvgElement e = elem;
    if (!e.previousSiblingElement().isNull())
        e = e.previousSiblingElement();
    else
        e = e.parentElement().firstChildElement();
    return e;
}

SvgAttribute::SvgAttribute()
{
    impl = 0;
}

SvgAttribute::SvgAttribute(uint aid, const SvgElement &elem)
{
    Q_ASSERT(aid != 0);

    ReferenceSvgAttributeData *p = new ReferenceSvgAttributeData;
    p->id = aid;
    p->linked = elem;
    if (p->id == AttrId::xlink_href)
        p->value = QL1S("#") + elem.id();
    else
        p->value = UrlPrefix + QL1S("#") + elem.id() + QL1S(")");

    if (Properties::presentationAttributesIds.contains(aid))
        p->isStyle = true;

    impl = p;
}

SvgAttribute::SvgAttribute(uint aid, const QString &avalue)
{
    Q_ASSERT(aid != 0);

    DefaultSvgAttributeData *p = new DefaultSvgAttributeData;
    p->id = aid;
    p->value = avalue;
    if (Properties::presentationAttributesIds.contains(aid))
        p->isStyle = true;
    impl = p;
}

SvgAttribute::SvgAttribute(const Transform &ts)
{
    Q_ASSERT(ts.isValid() == true);

    TransformSvgAttributeData *p = new TransformSvgAttributeData;
    p->id = AttrId::transform;
    p->transform = ts;
    impl = p;
}

SvgAttribute::SvgAttribute(const QString &aname, const QString &avalue)
{
    ExternalSvgAttributeData *p = new ExternalSvgAttributeData;
    p->name = aname;
    p->value = avalue;
    impl = p;
}

SvgAttribute::SvgAttribute(const SvgAttribute &a): impl(a.impl) {}

bool SvgAttribute::operator==(const SvgAttribute &t) const
{
    // compare pointers
    if (impl == t.impl)
        return true;

    return false;
}

bool SvgAttribute::operator!=(const SvgAttribute &t) const
{
    return !(*this == t);
}

bool SvgAttribute::isNull() const
{
    return !impl;
}

uint SvgAttribute::id() const
{
    if (impl)
        return impl->id;
    return 0;
}

QString SvgAttribute::name() const
{
    CheckData(QString());

    if (impl->type != SvgAttributeData::External)
        return attrIdToStr(impl->id);
    else
        return static_cast<ExternalSvgAttributeData*>(impl.data())->name;
    return QString();
}

SvgAttribute::SvgAttribute(SvgAttributeData *t) { impl = t; }

QString SvgAttribute::value() const
{
    CheckData(QString());

    if (impl->type != SvgAttributeData::TTransform) {
        if (isReference() && static_cast<ReferenceSvgAttributeData*>(impl.data())->linked.isNull())
            return QString();
        return impl->value;
    }
    qFatal("cannot get string value from transform attribute");
    return QString();
}

void SvgAttribute::setValue(const QString &text)
{
    CheckData();

    if (impl->type == SvgAttributeData::Default)
        impl->value = text;
    else if (impl->type == SvgAttributeData::External)
        impl->value = text;
    else if (impl->type == SvgAttributeData::Reference) {
        SvgAttributeData *p = new SvgAttributeData;
        p->type = SvgAttributeData::Default;
        p->id = id();
        p->value = text;
        if (Properties::presentationAttributesIds.contains(id()))
            p->isStyle = true;
        impl = p;
    } else if (impl->type == SvgAttributeData::TTransform) {
        qFatal("transform attribute cannot be init by string");
    } else {
        qFatal("current attribute is invalid");
    }
}

SvgElement SvgAttribute::referencedElement() const
{
    CheckData(SvgElement());

    if (impl->type == SvgAttributeData::Default)
        return SvgElement();
    if (impl->type == SvgAttributeData::Reference)
        return static_cast<ReferenceSvgAttributeData*>(impl.data())->linked;

    qFatal("current attribute is not link");
    return SvgElement();
}

Transform SvgAttribute::transform() const
{
    CheckData(Transform());
    if (impl->type == SvgAttributeData::TTransform)
        return static_cast<TransformSvgAttributeData*>(impl.data())->transform;
    qFatal("current attribute is not transform");
    return Transform();
}

bool SvgAttribute::isDefault() const
{
    CheckData(false);
    return impl->type == SvgAttributeData::Default;
}

bool SvgAttribute::isExternal() const
{
    CheckData(false);
    return impl->type == SvgAttributeData::External;
}

bool SvgAttribute::isTransform() const
{
    CheckData(false);
    return impl->type == SvgAttributeData::TTransform;
}

bool SvgAttribute::isStyle() const
{
    CheckData(false);

    if (impl->type == SvgAttributeData::Default)
        return impl->isStyle;
    if (impl->type == SvgAttributeData::Reference)
        return impl->isStyle;
    return false;
}

bool SvgAttribute::isReference() const
{
    CheckData(false);
    return impl->type == SvgAttributeData::Reference;
}
