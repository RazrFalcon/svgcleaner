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

#ifndef SVGDOM_H
#define SVGDOM_H

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QTextStream>

#include "enums.h"
#include "transform.h"

class SvgDocumentPrivate;
class SvgNodePrivate;
class SvgElementPrivate;
class SvgDeclarationPrivate;
class SvgTextPrivate;
class SvgCommentPrivate;

class SvgNode;
class SvgElement;
class SvgComment;
class SvgDeclaration;
class SvgText;
class SvgDocument;

typedef QHash<QString,QString> StringHash;
typedef QHash<int,QString> IntHash;
typedef QMap<QString,QString> StringMap;
typedef QSet<QString> StringSet;
typedef QList<SvgElement> SvgElementList;
typedef QList<SvgNode> SvgNodeList;
typedef QList<int> IntList;

#define element_loop(_x) \
    SvgElement elem = _x; \
    SvgElement root = elem; \
    while (!elem.isNull())

#define element_loop_next(_x) \
    elem = root = _x; \
    while (!elem.isNull())


class SvgNode
{
public:
    SvgNode();
    SvgNode(const SvgNode &);
    SvgNode& operator= (const SvgNode &);
    bool operator== (const SvgNode &) const;
    bool operator!= (const SvgNode &) const;
    ~SvgNode();

    SvgNode insertBefore(const SvgNode &newChild, const SvgNode &refChild);
    SvgNode removeChild(const SvgNode &oldChild);
    SvgNode appendChild(const SvgNode &newChild);
    bool hasChildren() const;

    QString nodeName() const;
    SvgNode parentNode() const;
    SvgNodeList childNodes() const;
    SvgNode firstChild() const;
    SvgNode lastChild() const;
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

protected:
    SvgNodePrivate* impl;
    SvgNode(SvgNodePrivate*);

private:
    friend class SvgDocument;
    friend class SvgElement;

    void save(QTextStream &, int) const;
};

class SvgDocument : public SvgNode
{
public:
    SvgDocument() { impl = 0; }
    SvgDocument(const SvgDocument& x) : SvgNode(x) {}
    SvgDocument& operator= (const SvgDocument&);
    ~SvgDocument() {}

    SvgElement createElement(const QString &tagName);
    SvgComment createComment(const QString &data);
    SvgDeclaration createDeclaration(const QString &data);
    SvgText createText(const QString &text);

    SvgElement documentElement() const;

    bool loadFile(const QString &filePath);
    QString toString(int = 1) const;
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

    QString attribute(const int &attrId, const QString &defValue = QString()) const;
    QString attribute(const QString &name, const QString &defValue = QString()) const;
    QString extAttribute(const QString &name, const QString &defValue = QString()) const;
    void setAttribute(const int &attrId, const QString &value);
    void setAttribute(const QString &name, const QString& value);
    void setAttributeHash(const IntHash &baseHash, const StringHash &extHash);
    void removeAttribute(int attrId);
    void removeAttribute(const QVariant &name);
    bool hasAttribute(int attrId) const;
    bool hasAttribute(const QString &name) const;
    bool hasExtAttribute(const QString &name) const;
    bool hasAttributes(const IntList &list) const;
    bool hasAttributes(const QStringList &list) const;
    bool hasAttributes() const;
    QString xlinkId() const;
    QString id() const;
    SvgElement removeChild(const SvgElement &oldChild, bool returnPreviousElement = false);
    bool hasChildrenElement() const;
    int childElementCount() const;
    StringHash attributesHash(bool ignoreId) const;
    void removeAttributeIf(int attrId, const QString &value);
    void removeAttributeIf(const QString &name, const QString &value);
    double doubleAttribute(int attrId);
    IntList baseAttributesList() const;
    QStringList extAttributesList() const;
    QStringList attributesList() const;
    SvgElement parentElement() const;
    SvgElementList childElements() const;
    SvgElement firstChildElement() const;
    IntList styleAttributesList() const;

    bool hasLinkedDef() const;
    bool isContainer() const;
    bool isGroup() const;
    bool hasImportantAttrs(const IntList &ignoreList = IntList());
    bool hasLinkedStyle();
    bool isUsed() const;
    bool hasChildWithTagName(const QString &name) const;
    bool hasTextChild() const;
    int attributesCount() const;
    QString defIdFromAttribute(const int &attrId);
    IntHash styleHash() const;
    void removeAttributes(const QStringList &list);
    void setStylesFromHash(const IntHash &hash);
    void setTransform(const QString &transform, bool fromParent = false);

    QString tagName() const;
    void setTagName(const QString &name);

    QString text() const;

private:
    SvgElement(SvgElementPrivate*);

    friend class SvgDocument;
    friend class SvgNode;
};

class SvgComment : public SvgNode
{
public:
    SvgComment() : SvgNode() {}
    SvgComment(const SvgComment &x) : SvgNode(x) {}
    SvgComment& operator= (const SvgComment &);

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
void nextElement(SvgElement &elem, const SvgElement &root, bool ignoreChild = false);


#endif // SVGDOM_H
