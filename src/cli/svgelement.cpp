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

#include "tools.h"
#include "svgelement.h"


SvgElement::SvgElement()
{
    m_elem = 0;
}

SvgElement::SvgElement(XMLElement *elem)
{
    m_elem = elem;
}

QList<SvgElement> SvgElement::childElemList() const
{
    QList<SvgElement> list;
    list.reserve(this->childElementCount());
    for (XMLElement *child = m_elem->FirstChildElement();
            child; child = child->NextSiblingElement())
        list << SvgElement(child);
    return list;
}

bool SvgElement::isReferenced() const
{
    return Props::referencedElements.contains(tagName());
}

bool SvgElement::isText() const
{
    return Props::textElements.contains(tagName());
}

bool SvgElement::hasText() const
{
    if (m_elem->FirstChild() != 0) {
        if (m_elem->FirstChild()->ToText() != 0) {
            if (m_elem->FirstChild()->ToText()->Value() != 0)
                return true;
        }
    }
    return false;
}

bool SvgElement::hasImportantAttrs()
{
    int attrCount = attributesCount();
    if (attrCount == 0)
        return false;
    if (Keys::get().flag(Key::RemoveUnreferencedIds)) {
        if (attrCount == 1 && hasAttribute("id"))
            return false;
    }
    return true;
}

bool SvgElement::hasLinkedStyle()
{
    static QStringList illegalStyleAttrList;
    if (illegalStyleAttrList.isEmpty())
        illegalStyleAttrList << "fill" << "stroke";
    foreach (const QString attrName, illegalStyleAttrList) {
        if (attribute(attrName).startsWith(QL1S("url(")))
            return true;
    }
    return false;
}

bool SvgElement::isUsed() const
{
    return hasAttribute(CleanerAttr::UsedElement);
}

bool SvgElement::isContainer() const
{
    return Props::containers.contains(tagName());
}

bool SvgElement::isGroup() const
{
    return !strcmp(m_elem->Name(), "g");
}

bool SvgElement::isNull() const
{
    return (m_elem == 0);
}

void SvgElement::removeAttributes(const QStringList &list)
{
    for (int i = 0; i < list.count(); ++i)
        removeAttribute(list.at(i));
}

StringMap SvgElement::attributesMap(bool ignoreId) const
{
    StringMap map;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        QString attrName = QString(child->Name());
        if (!(attrName == "id" && ignoreId)) {
            map.insert(attrName, QString(child->Value()));
        }
    }
    return map;
}

QStringList SvgElement::styleAttributesList() const
{
    QStringList list;
    list.reserve(attributesCount());
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        QString attrName = QL1S(child->Name());
        if (Props::presentationAttributes.contains(attrName))
           list << attrName;
    }
    return list;
}

QStringList SvgElement::attributesList() const
{
    QStringList list;
    list.reserve(attributesCount());
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next())
        list << QL1S(child->Name());
    return list;
}

QStringList SvgElement::attributesListBySet(const StringSet &set) const
{
    QStringList list;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        QString str = QL1S(child->Name());
        if (set.contains(str))
            list << str;
    }
    return list;
}

void SvgElement::setStylesFromHash(const StringHash &hash)
{
    foreach (const QString &attr, hash.keys())
        setAttribute(attr, hash.value(attr));
}

void SvgElement::setAttribute(const QString &name, const QString &value)
{
    if (value.isEmpty())
        m_elem->DeleteAttribute(ToChar(name));
    else
        m_elem->SetAttribute(ToChar(name), ToChar(value).constData());
}

void SvgElement::setAttribute(const char *name, const QString &value)
{
    if (value.isEmpty())
        m_elem->DeleteAttribute(name);
    else
        m_elem->SetAttribute(name, ToChar(value).constData());
}

QString SvgElement::id() const
{
    return attribute("id");
}

QString SvgElement::defIdFromAttribute(const QString &name)
{
    QString id = attribute(name);
    if (!id.startsWith(QL1S("url(")))
        return QString();
    return id.mid(5, id.size()-6);
}

QString SvgElement::xlinkId()
{
    return attribute("xlink:href").remove(0,1);
}

bool SvgElement::hasLinkedDef() const
{
    // TODO: is filter needed?
    static CharList illegalAttrList;
    if (illegalAttrList.isEmpty())
        illegalAttrList << "clip-path" << "mask" << "filter";
    foreach (const QString attrName, illegalAttrList) {
        if (hasAttribute(attrName))
            return true;
    }
    static CharList illegalStyleAttrList;
    if (illegalStyleAttrList.isEmpty())
        illegalStyleAttrList << "fill" << "stroke";
    foreach (const QString attrName, illegalStyleAttrList) {
        if (attribute(attrName).startsWith(QL1S("url(")))
            return true;
    }
    return false;
}

bool SvgElement::hasAttribute(const QString &name) const
{
    return (m_elem->Attribute(ToChar(name)) != 0);
}

bool SvgElement::hasAttribute(const char *name) const
{
    return (m_elem->Attribute(name) != 0);
}

bool SvgElement::hasAttributes(const QStringList &list) const
{
    foreach (const QString &attrName, list)
        if (hasAttribute(attrName))
            return true;
    return false;
}

QString SvgElement::attribute(const QString &name) const
{
    const char *ch = ToChar(name);
    return QL1S(m_elem->Attribute(ch));
}

QString SvgElement::attribute(const char *name) const
{
    return QL1S(m_elem->Attribute(name));
}

bool SvgElement::attributeEqualTo(const char *attrName, const char *value)
{
    const char *attrValue = m_elem->Attribute(attrName);
    if (attrValue == 0)
        return false;
    return XMLUtil::StringEqual(attrValue, value);
}

double SvgElement::doubleAttribute(const char *name) const
{
    return attribute(name).toDouble();
}

double SvgElement::doubleAttribute(const QString &name) const
{
    return attribute(name).toDouble();
}

bool SvgElement::hasChildWithTagName(const char *name, XMLElement *parent) const
{
    if (!parent)
        parent = m_elem;
    for (XMLElement *child = parent->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (!strcmp(child->Name(), name))
            return true;
        if (!child->NoChildren())
            return hasChildWithTagName(name, child);
    }
    return false;
}

void SvgElement::removeAttribute(const QString &name)
{
    m_elem->DeleteAttribute(ToChar(name));
}

void SvgElement::removeAttribute(const char *name)
{
    m_elem->DeleteAttribute(name);
}

void SvgElement::removeAttributeIf(const char *name, const char *value)
{
    const char *attrValue = m_elem->Attribute(name);
    if (attrValue != 0) {
        if (!strcmp(attrValue, value))
            m_elem->DeleteAttribute(name);
    }
}

bool SvgElement::isTagName(const char *tagName) const
{
    return !strcmp(m_elem->Name(), tagName);
}

const char* SvgElement::tagNameChar() const
{
    return m_elem->Name();
}

QString SvgElement::tagName() const
{
    return QL1S(m_elem->Name());
}

SvgElement SvgElement::parentElement() const
{
    return SvgElement(m_elem->Parent()->ToElement());
}

SvgElement SvgElement::firstChild() const
{
    return SvgElement(m_elem->FirstChildElement());
}

void SvgElement::removeChild(const SvgElement &elem)
{
    m_elem->DeleteChild(elem.xmlElement());
}

bool SvgElement::hasChildren() const
{
    return !m_elem->NoChildren();
}

void SvgElement::appendChild(const SvgElement &elem)
{
    m_elem->InsertEndChild(elem.xmlElement());
}

void SvgElement::setTagName(const QString &name)
{
    m_elem->SetName(ToChar(name));
}

void SvgElement::setTransform(const QString &transform, bool fromParent)
{
    if (hasAttribute("transform")) {
        if (fromParent) {
            Transform ts(transform + " " + attribute("transform"));
            setAttribute("transform", ts.simplified());
        } else {
            Transform ts(attribute("transform") + " " + transform);
            setAttribute("transform", ts.simplified());
        }
    } else {
        setAttribute("transform", transform);
    }
}

SvgElement SvgElement::insertBefore(const SvgElement &elemNew, const SvgElement &elemBefore) const
{
    XMLElement *refElem = elemBefore.xmlElement()->PreviousSiblingElement();
    if (refElem == 0)
        return SvgElement(m_elem->InsertFirstChild(elemNew.xmlElement())->ToElement());
    return SvgElement(m_elem->InsertAfterChild(refElem, elemNew.xmlElement())->ToElement());
}

SvgElement SvgElement::insertLast(const SvgElement &elemNew) const
{
    return SvgElement(m_elem->InsertEndChild(elemNew.xmlElement())->ToElement());
}

StringMap SvgElement::styleMap() const
{
    StringMap hash;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        if (Props::presentationAttributes.contains(child->Name()))
            hash.insert(child->Name(), child->Value());
    }
    return hash;
}

StringHash SvgElement::styleHash() const
{
    StringHash hash;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        if (Props::presentationAttributes.contains(child->Name()))
            hash.insert(child->Name(), child->Value());
    }
    return hash;
}

int SvgElement::attributesCount() const
{
    int count = 0;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next())
        count++;
    return count;
}

int SvgElement::childElementCount() const
{
    int count = 0;
    for (XMLElement *child = m_elem->FirstChildElement(); child;
            child = child->NextSiblingElement())
    {
        count++;
    }
    return count;
}

void SvgElement::clear()
{
    m_elem = 0;
}
