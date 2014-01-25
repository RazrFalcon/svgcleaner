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

#include "basecleaner.h"

BaseCleaner::BaseCleaner(XMLDocument *doc)
{
    m_doc = doc;
    m_svgElem = svgElement(doc);
    m_defsElem = defsElement(doc, m_svgElem);
}

XMLDocument* BaseCleaner::document() const
{
    return m_doc;
}

SvgElement BaseCleaner::svgElement() const
{
    return m_svgElem;
}

SvgElement BaseCleaner::defsElement() const
{
    return m_defsElem;
}

SvgElement BaseCleaner::defsElement(XMLDocument *doc, SvgElement &svgElem)
{
    XMLElement *child;
    for (child = svgElem.xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "defs") == 0) {
            break;
        }
    }
    if (child == 0) {
        XMLElement* element = doc->NewElement("defs");
        svgElem.xmlElement()->InsertFirstChild(element);
        child = element;
    }
    return SvgElement(child);
}

QList<XMLNode *> BaseCleaner::childNodeList(XMLNode *node)
{
    QList<XMLNode *> list;
    for (XMLNode *child = node->FirstChild(); child; child = child->NextSibling())
        list << child;
    return list;
}

QList<SvgElement> BaseCleaner::childElemList(XMLDocument *doc)
{
    QList<SvgElement> list;
    for (XMLElement *child = doc->FirstChildElement(); child; child = child->NextSiblingElement())
        list << SvgElement(child);
    return list;
}

void BaseCleaner::updateXLinks(const StringHash &hash)
{
    CharList xlinkStyles;
    xlinkStyles << "fill" << "stroke" << "filter" << "clip-path";

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        for (int i = 0; i < xlinkStyles.size(); ++i) {
            if (currElem.hasAttribute(xlinkStyles.at(i))) {
                QString attrValue = currElem.attribute(xlinkStyles.at(i));
                if (attrValue.startsWith(QL1S("url"))) {
                    QString url = attrValue.mid(5, attrValue.size()-6);
                    if (hash.contains(url)) {
                        currElem.setAttribute(xlinkStyles.at(i),
                                              QString("url(#" + hash.value(url) + ")"));
                    }
                }
            }
        }
        if (currElem.hasAttribute("xlink:href")) {
            QString value = currElem.xlinkId();
            QString elemId = currElem.id();
            foreach (const QString &key, hash.keys()) {
                if (value == key) {
                    if (hash.value(key) != elemId)
                        currElem.setAttribute("xlink:href", QString("#" + hash.value(key)));
                    else
                        currElem.removeAttribute("xlink:href");
                    break;
                }
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

SvgElement BaseCleaner::svgElement(XMLDocument *doc)
{
    XMLElement *child;
    for (child = doc->FirstChildElement(); child; child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "svg") == 0) {
            break;
        }
    }
    return SvgElement(child);
}

SvgElement BaseCleaner::findDefElement(const QString &id)
{
    for (XMLElement *child = defsElement().xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (child->Attribute("id") != 0)
            if (!strcmp(child->Attribute("id"), id.toLatin1()))
                return SvgElement(child);
    }
    return SvgElement();
}

SvgElement BaseCleaner::findElement(const QString &id, XMLElement *parent)
{
    if (!parent)
        parent = svgElement().xmlElement();
    SvgElement elem;
    for (XMLElement *child = parent->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (child->Attribute("id") != 0)
            if (!strcmp(child->Attribute("id"), id.toLatin1())) {
                elem = SvgElement(child);
                break;
            }
        if (!child->NoChildren()) {
            elem = findElement(id, child);
            if (!elem.isNull())
                break;
        }
    }
    return elem;
}

bool BaseCleaner::hasParent(const SvgElement &elem, const QString &tagName)
{
    SvgElement parent = elem.parentElement();
    while (!parent.isNull()) {
        if (parent.tagName() == tagName)
            return true;
        parent = parent.parentElement();
    }
    return false;
}

QString BaseCleaner::findAttribute(const SvgElement &elem, const char *attrName)
{
    SvgElement parent = elem;
    while (!parent.isNull()) {
        if (parent.hasAttribute(attrName))
            return parent.attribute(attrName);
        parent = parent.parentElement();
    }
    return "";
}

QString BaseCleaner::absoluteTransform(const SvgElement &elem)
{
    QString transform;
    SvgElement parent = elem;
    while (!parent.isNull()) {
        transform += parent.attribute("transform") + " ";
        parent = parent.parentElement();
    }
    return Transform(transform).simplified();
}

QRectF BaseCleaner::viewBoxRect()
{
    QRectF rect;
    if (svgElement().hasAttribute("viewBox")) {
        QStringList list = svgElement().attribute("viewBox").split(" ", QString::SkipEmptyParts);
        rect.setRect(list.at(0).toDouble(), list.at(1).toDouble(),
                     list.at(2).toDouble(), list.at(3).toDouble());
    } else if (svgElement().hasAttribute("width") && svgElement().hasAttribute("height")) {
        rect.setRect(0, 0, svgElement().doubleAttribute("width"),
                           svgElement().doubleAttribute("height"));
    }
    return rect;
}
