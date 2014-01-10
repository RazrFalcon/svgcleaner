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
    m_svgElem = Tools::svgElement(doc);
    m_defsElem = Tools::defsElement(doc, m_svgElem);
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
                if (attrValue.startsWith("url")) {
                    QString url = attrValue.mid(5, attrValue.size()-6);
                    if (hash.contains(url)) {
                        currElem.setAttribute(xlinkStyles.at(i),
                                              QString("url(#" + hash.value(url) + ")"));
                    }
                }
            }
        }
        if (currElem.hasAttribute("xlink:href")) {
            QString value = currElem.attribute("xlink:href");
            value.remove(0,1); // #
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

SvgElement BaseCleaner::findDefElem(const QString &id)
{
    for (XMLElement *child = defsElement().xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (child->Attribute("id") != 0)
            if (!strcmp(child->Attribute("id"), id.toLatin1()))
                return SvgElement(child);
    }
    return SvgElement();
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
