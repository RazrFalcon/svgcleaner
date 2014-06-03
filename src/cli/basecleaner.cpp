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
#include "transform.h"

BaseCleaner::BaseCleaner(SvgDocument doc)
{
    m_doc = doc;
    m_svgElem = svgElement(doc);
    m_defsElem = defsElement(doc, m_svgElem);
}

SvgDocument BaseCleaner::document() const
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

SvgElement BaseCleaner::defsElement(SvgDocument doc, SvgElement &svgElem)
{
    QList<SvgNode> list = svgElem.childNodes();
    while (!list.isEmpty()) {
        SvgNode node = list.takeFirst();
        if (node.isElement()) {
            if (node.toElement().tagName() == E_defs)
                return node.toElement();
        }
    }
    SvgElement newDefs = doc.createElement(E_defs);
    svgElem.insertBefore(newDefs, svgElem.firstChild());
    return newDefs;
}

void BaseCleaner::updateXLinks(const StringHash &hash)
{
    IntList xlinkStyles;
    xlinkStyles << AttrId::fill << AttrId::stroke << AttrId::filter << AttrId::clip_path;

    element_loop(svgElement()) {
        for (int i = 0; i < xlinkStyles.size(); ++i) {
            if (elem.hasAttribute(xlinkStyles.at(i))) {
                QString attrValue = elem.attribute(xlinkStyles.at(i));
                if (attrValue.startsWith(UrlPrefix)) {
                    QString url = attrValue.mid(5, attrValue.size()-6);
                    if (hash.contains(url)) {
                        elem.setAttribute(xlinkStyles.at(i),
                                              QString("url(#" + hash.value(url) + ")"));
                    }
                }
            }
        }
        if (elem.hasAttribute(AttrId::xlink_href)) {
            QString value = elem.xlinkId();
            QString elemId = elem.id();
            foreach (const QString &key, hash.keys()) {
                if (value == key) {
                    if (hash.value(key) != elemId)
                        elem.setAttribute(AttrId::xlink_href, QString("#" + hash.value(key)));
                    else
                        elem.removeAttribute(AttrId::xlink_href);
                    break;
                }
            }
        }
        nextElement(elem, root);
    }
}

SvgElement BaseCleaner::svgElement(SvgDocument doc)
{
    QList<SvgNode> list = doc.childNodes();
    while (!list.isEmpty()) {
        SvgNode node = list.takeFirst();
        if (node.isElement()) {
            if (node.toElement().tagName() == E_svg)
                return node.toElement();
        }

    }
    return SvgElement();
}

SvgElement BaseCleaner::findInDefs(const QString &id)
{
    SvgElement elem = defsElement().firstChildElement();
    while (!elem.isNull()) {
        if (elem.id() == id)
            return elem;
        elem = elem.nextSiblingElement();
    }
    return SvgElement();
}

SvgElement BaseCleaner::findElement(const QString &id, SvgElement parent)
{
    if (parent.isNull())
        parent = svgElement();
    element_loop(parent) {
        if (elem.id() == id)
            return elem;
        nextElement(elem, root);
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

QString BaseCleaner::findAttribute(const SvgElement &elem, int attrId) const
{
    SvgElement parent = elem;
    while (!parent.isNull()) {
        if (parent.hasAttribute(attrId))
            return parent.attribute(attrId);
        parent = parent.parentElement();
    }
    return "";
}

QString BaseCleaner::absoluteTransform(const SvgElement &elem)
{
    QString transform;
    SvgElement parent = elem;
    while (!parent.isNull()) {
        transform += parent.attribute(AttrId::transform) + " ";
        parent = parent.parentElement();
    }
    return Transform(transform).simplified();
}

QRectF BaseCleaner::viewBoxRect()
{
    QRectF rect;
    if (svgElement().hasAttribute(AttrId::viewBox)) {
        QStringList list = svgElement().attribute(AttrId::viewBox).split(' ', QString::SkipEmptyParts);
        rect.setRect(list.at(0).toDouble(), list.at(1).toDouble(),
                     list.at(2).toDouble(), list.at(3).toDouble());
    } else if (svgElement().hasAttribute(AttrId::width) && svgElement().hasAttribute(AttrId::height)) {
        rect.setRect(0, 0, svgElement().doubleAttribute(AttrId::width),
                           svgElement().doubleAttribute(AttrId::height));
    }
    return rect;
}
