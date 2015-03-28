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

#ifndef SVGDOM_H
#define SVGDOM_H

#include "enums.h"
#include "transform.h"

class SvgDocumentPrivate;
class SvgNodePrivate;
class SvgElementPrivate;
class SvgDeclarationPrivate;
class SvgTextPrivate;
class SvgCommentPrivate;

class SvgDocument;
class SvgNode;
class SvgElement;
class SvgDeclaration;
class SvgText;
class SvgComment;
class SvgAttribute;

typedef QHash<QString,QString> StringHash;
typedef QMap<QString,QString> StringMap;
typedef QSet<QString> StringSet;
typedef QList<SvgElement> SvgElementList;
typedef QList<SvgNode> SvgNodeList;
typedef QList<uint> IntList;
typedef QList<SvgAttribute> SvgAttributeList;
typedef QHash<uint,SvgAttribute> SvgAttributeHash;

#define element_loop(_x) \
    for (SvgElement elem = _x, root = elem; !elem.isNull(); nextElement(elem, root))

#define loop_children(_x) \
    for (SvgElement elem = _x.firstChildElement(); !elem.isNull(); elem = elem.nextSiblingElement())

#define loop_children_arg(variable, container) \
    for (SvgElement variable = container.firstChildElement(); !variable.isNull(); \
            variable = variable.nextSiblingElement())

class SvgNode
{
public:
    SvgNode();
    SvgNode(const SvgNode &);
    SvgNode &operator= (const SvgNode &);
    bool operator== (const SvgNode &) const;
    bool operator!= (const SvgNode &) const;
    ~SvgNode();

    SvgNode insertBefore(const SvgNode &newChild, const SvgNode &refChild);
    void removeChild(const SvgNode &oldChild);
    SvgNode appendChild(const SvgNode &newChild);
    bool hasChildren() const;

    SvgNode parentNode() const;
    SvgNodeList childNodes() const;
    SvgNode firstChild() const;
    SvgNode previousSibling() const;
    SvgNode nextSibling() const;
    SvgDocument ownerDocument() const;    
    bool hasNextSibling() const;

    bool isDocument() const;
    bool isElement() const;
    bool isText() const;
    bool isDeclaration() const;
    bool isComment() const;
    bool hasText() const;
    bool isNull() const;
    void clear();

    SvgDocument toDocument() const;
    SvgElement toElement() const;
    SvgDeclaration toDeclaration() const;
    SvgComment toComment() const;
    SvgText toText() const;

    SvgElement firstChildElement() const;
    SvgElement previousSiblingElement() const;
    SvgElement nextSiblingElement() const;
    SvgNode nextNode(const SvgNode &root) const;

protected:
    SvgNodePrivate* impl;
    SvgNode(SvgNodePrivate*);

    SvgNodePrivate* implementation() const { return impl; }

private:
    friend class SvgDocument;
    friend class SvgElement;

    void save(QString &str, int indent) const;
};

class SvgDocument : public SvgNode
{
public:
    SvgDocument() { impl = 0; }
    SvgDocument(const SvgDocument& x) : SvgNode(x) {}
    SvgDocument& operator= (const SvgDocument&);

    SvgElement createElement(const QString &tagName);
    SvgComment createComment(const QString &data);
    SvgDeclaration createDeclaration(const QString &data);
    SvgText createText(const QString &text);

    void calcElemAttrCount(const QString &text);

    SvgElement documentElement() const;

    void setBlockStringUrls(bool flag);
    bool isBlockStringUrls() const;

    QString takeFreeId();

    bool loadFile(const QString &filePath);
    bool fromString(const QString &text);
    const QString toString(int indent = 1) const;
    QString lastError() const;

private:
    SvgDocument(SvgDocumentPrivate *x);

    friend class SvgNode;
};

class SvgElement : public SvgNode
{
public:
    SvgElement() : SvgNode() {}
    SvgElement(const SvgElement &x) : SvgNode(x) {}
    SvgElement& operator= (const SvgElement &);

    // navigation
    SvgElement parentElement() const;
    SvgElementList childElements() const;
    SvgElement firstChildElement() const;
    SvgElement nextElement(const SvgElement &root, bool ignoreChild = false) const;

    // attributes processing
    QString attribute(const uint &attrId, const QString &defValue = QString()) const;
    QString attribute(const QString &name, const QString &defValue = QString()) const;
    SvgAttribute attributeItem(const uint &attrId) const;
    void setAttribute(const uint &attrId, const QString &value);
    void setAttribute(const QString &name, const QString& value);
    void setAttribute(const SvgAttribute &attr);
    void removeAttribute(uint attrId);
    void removeAttribute(const QString &name);
    void removeAttributes(const QStringList &list);
    bool hasAttribute(uint attrId) const;
    bool hasAttribute(const QString &name) const;
    bool hasAttributes(const IntList &list) const;
    bool hasAttributes(const QStringList &list) const;
    bool hasAttributes() const;
    QString id() const;
    SvgElement removeChild(const SvgElement &oldChild, bool returnPrevious = false);
    bool hasChildrenElement() const;
    int childElementCount() const;
    void removeAttributeIf(uint attrId, const QString &value);
    void removeAttributeIf(const QString &name, const QString &value);
    double doubleAttribute(uint attrId);
    IntList baseAttributesList() const;
    QStringList extAttributesList() const;
    SvgAttributeList attributesList() const;
    QStringList attributesNamesList() const;
    IntList styleAttributesList() const;
    QString tagName() const;
    void setTagName(const QString &name);
    bool hasParentAttribute(uint attrId);
    QString parentAttribute(uint attrId, bool includeCurrentElem = false);
    int attributesCount() const;
    QString defIdFromAttribute(const uint &attrId) const;
    bool hasImportantAttrs(const IntList &ignoreList = IntList());
    SvgAttributeHash styleHash() const;

    // referenced elements processing
    bool hasReference(uint attrId) const;
    SvgElement referencedElement(uint attrId) const;
    void setReferenceElement(uint attrId, const SvgElement &elem);
    IntList referencedAttributes() const;
    uint referenceAttribute(const SvgElement &elem) const;
    bool hasReferencedDefs() const;
    void removeReferenceElement(uint attrId);

    // linked elements processing
    void appendLinkedElement(const uint attrId, const SvgElement &elem);
    void removeLinkedElement(const QString &id);
    SvgElementList linkedElements() const;
    bool isUsed() const;
    int usesCount() const;

    bool isContainer() const;
    bool isGroup() const;
    bool hasTextChild() const;
    QString text() const;

    // transform elements processing
    bool hasTransform() const;
    void setTransform(const Transform &atransform, bool fromParent = false);
    void removeTransform();
    Transform transform() const;

    void setBBoxTransform(const Transform &transform);
    Transform bboxTransform() const;

private:
    SvgElement(SvgElementPrivate*);
    static SvgNodePrivate* nextElementPrivate(SvgNodePrivate *node, SvgNodePrivate *root,
                                              bool ignoreChild = false);

    friend class SvgDocument;
    friend class SvgNode;
};

class SvgAttributeData : public QSharedData
{
public:
    SvgAttributeData() : QSharedData(), id(0), isStyle(false), type(None) {}

    enum Types { None, Default, External, TTransform, Reference };

    uint id;
    QString value;
    bool isStyle;
    Types type;
};

class DefaultSvgAttributeData : public SvgAttributeData
{
public:
    DefaultSvgAttributeData() : SvgAttributeData() { type = Default; }
};

class ExternalSvgAttributeData : public SvgAttributeData
{
public:
    ExternalSvgAttributeData() : SvgAttributeData() { type = External; }

    QString name;
};

class TransformSvgAttributeData : public SvgAttributeData
{
public:
    TransformSvgAttributeData() : SvgAttributeData() { type = TTransform; }

    Transform transform;
};

class ReferenceSvgAttributeData : public SvgAttributeData
{
public:
    ReferenceSvgAttributeData() : SvgAttributeData() { type = Reference; }

    SvgElement linked;
};

class SvgAttribute
{
public:
    SvgAttribute();
    SvgAttribute(uint aid, const QString &avalue);
    SvgAttribute(uint aid, const SvgElement &elem);
    SvgAttribute(const Transform &ts);
    SvgAttribute(const QString &aname, const QString &avalue);
    SvgAttribute(const SvgAttribute &a);
    bool operator== (const SvgAttribute &t) const;
    bool operator!= (const SvgAttribute &t) const;

    bool isNull() const;

    uint id() const;
    QString name() const;
    QString value() const;
    void setValue(const QString &text);
    SvgElement referencedElement() const;
    Transform transform() const;

    bool isDefault() const;
    bool isExternal() const;
    bool isTransform() const;
    bool isStyle() const;
    bool isReference() const;

private:
    QExplicitlySharedDataPointer<SvgAttributeData> impl;
    SvgAttribute(SvgAttributeData *t);
};

class SvgComment : public SvgNode
{
public:
    SvgComment() : SvgNode() {}
    SvgComment(const SvgComment &x) : SvgNode(x) {}
    SvgComment& operator= (const SvgComment &);
    QString data() const;

private:
    SvgComment(SvgCommentPrivate*);

    friend class SvgDocument;
    friend class SvgNode;
};

class SvgText : public SvgNode
{
public:
    SvgText() : SvgNode() {}
    SvgText(const SvgText &x) : SvgNode(x) {}
    SvgText& operator= (const SvgText &);
    QString text() const;
    void setDtd(bool flag);

private:
    SvgText(SvgTextPrivate*);

    friend class SvgDocument;
    friend class SvgNode;
};

class SvgDeclaration : public SvgNode
{
public:
    SvgDeclaration() : SvgNode() {}
    SvgDeclaration(const SvgDeclaration &x) : SvgNode(x) {}
    SvgDeclaration& operator= (const SvgDeclaration &);
    QString data() const;
    void setData(const QString &d);

private:
    SvgDeclaration(SvgDeclarationPrivate*);

    friend class SvgDocument;
    friend class SvgNode;
};

SvgElement prevElement(const SvgElement &elem);
SvgElement prevSiblingElement(const SvgElement &elem);
void removeAndMoveToPrev(SvgElement &elem);
void removeAndMoveToPrevSibling(SvgElement &elem);
void nextNode(SvgNode &node, const SvgNode &root);
void nextElement(SvgElement &elem, const SvgElement &root, bool ignoreChild = false);

#endif // SVGDOM_H
