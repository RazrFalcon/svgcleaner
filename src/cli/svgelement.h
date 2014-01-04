/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

#ifndef SVGELEMENT_H
#define SVGELEMENT_H

#include <QStringList>
#include <QMap>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;
typedef QSet<QString> StringSet;

#include "tinyxml2.h"

#define ToChar(x) x.toLatin1()

using namespace tinyxml2;

// QDomElement like wrapper class for XMLElement
class SvgElement
{
public:
    SvgElement();
    SvgElement(XMLElement *elem);

    bool hasAttribute(const QString &name) const;
    bool hasAttributes(const QStringList &list) const;
    bool hasChildren() const;
    bool hasLinkedDef();
    bool isContainer() const;
    bool isGroup() const;
    bool isNull() const;
    bool isReferenced() const;
    bool isText() const;
    bool hasText() const;
    bool hasImportantAttrs();
    bool hasLinkedStyle();
    bool isUsed() const;
    double doubleAttribute(const QString &name) const;
    int attributesCount() const;
    int childElementCount() const;
    QList<SvgElement> childElemList() const;
    QString attribute(const QString &name) const;
    QString id() const;
    QString defIdFromAttribute(const QString &name);
    StringMap attributesMap(bool ignoreId = false) const;
    QStringList styleAttributesList() const;
    QStringList attributesList() const;
    QString tagName() const;
    StringMap styleMap() const;
    StringHash styleHash() const;
    SvgElement insertBefore(const SvgElement &elemNew, const SvgElement &elemBefore) const;
    SvgElement parentElement() const;
    SvgElement firstChild() const;
    void appendChild(const SvgElement &elem);
    void clear();
    void removeAttribute(const QString &name);
    void removeAttribute(const char *name);
    void removeAttributes(const QStringList &list);
    void removeChild(const SvgElement &elem);
    void setAttribute(const QString &name, const QString &value);
    void setStylesFromHash(const StringHash &hash);
    void setTagName(const QString &name);
    void setTransform(const QString &transform, bool fromParent = false);

    XMLElement* xmlElement() const { return m_elem; }
    bool operator==(const SvgElement &elem1) {
        return elem1.xmlElement() == this->xmlElement();
    }
    bool operator!=(const SvgElement &elem1) {
        return elem1.xmlElement() != this->xmlElement();
    }
    void operator=(const SvgElement &elem) { m_elem = elem.xmlElement(); }

private:
    XMLElement *m_elem;
};

#endif // SVGELEMENT_H
