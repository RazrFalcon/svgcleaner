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
                        elem.setAttribute(AttrId::xlink_href, QString(QL1C('#') + hash.value(key)));
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

void BaseCleaner::joinLinearGradients(SvgElement &parent, SvgElement &child) const
{
    foreach (const SvgElement &elem, child.childElements())
        parent.appendChild(elem);
    parent.removeAttribute(AttrId::xlink_href);
    child.parentElement().removeChild(child);
}

// remove element with dependency checking
SvgElement BaseCleaner::smartElementRemove(const SvgElement &elem, bool returnPreviousElement) const
{
    if (elem.parentElement().tagName() == E_defs) {
        // if removed element is 'linearGradient'
        // and it has xlink to another 'linearGradient'
        // and this parent 'linearGradient' used only by 1 gradients
        // than merge parent 'linearGradient' with it only child

        // TODO: add example

        if (elem.tagName() == E_linearGradient && elem.hasAttribute(AttrId::xlink_href)) {
            SvgElement defElem = elemFromDefs(elem.xlinkId());
            const int useCount = defUsageCount(QL1C('#') + defElem.id());
            if (useCount == 1 && !defElem.hasAttribute(AttrId::xlink_href)) {
                SvgElement usesElem = findUsedElement(QL1C('#') + defElem.id());
                Q_ASSERT(usesElem.isNull() == false);
                joinLinearGradients(usesElem, defElem);
            }
        }
    } else {
        // if removed element has linked defs used only by this element - removes it
        foreach (const int &attrId, Properties::linkableStyleAttributesIds) {
            if (!elem.hasAttribute(attrId))
                continue;
            SvgElement defElem = elemFromDefs(elem.defIdFromAttribute(attrId));
            if (!isDefUsed(attrId, defElem.id(), elem)) {
                smartElementRemove(defElem);
            }
        }
    }

    return elem.parentElement().removeChild(elem, returnPreviousElement);
}

bool BaseCleaner::isDefUsed(int attrId, const QString &defId, const SvgElement &baseElem) const
{
    element_loop(svgElement()) {
        if (elem.xlinkId() == defId)
            return true;
        if (elem.defIdFromAttribute(attrId) == defId && elem != baseElem)
            return true;
        nextElement(elem, root);
    }
    return false;
}

int BaseCleaner::defUsageCount(const QString &defId) const
{
    int i = 0;
    element_loop(svgElement()) {
        if (elem.attribute(AttrId::xlink_href) == defId)
            i++;
        nextElement(elem, root);
    }
    return i;
}

SvgElement BaseCleaner::elemFromDefs(const QString &id) const
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

SvgElement BaseCleaner::findUsedElement(const QString &xlink) const
{
    element_loop(svgElement()) {
        if (elem.attribute(AttrId::xlink_href) == xlink)
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

QRectF BaseCleaner::viewBoxRect()
{
    QRectF rect;
    if (svgElement().hasAttribute(AttrId::viewBox)) {
        QStringList list = svgElement().attribute(AttrId::viewBox).split(' ', QString::SkipEmptyParts);
        rect.setRect(toDouble(list.at(0)), toDouble(list.at(1)),
                     toDouble(list.at(2)), toDouble(list.at(3)));
    } else if (svgElement().hasAttribute(AttrId::width) && svgElement().hasAttribute(AttrId::height)) {
        rect.setRect(0, 0, svgElement().doubleAttribute(AttrId::width),
                           svgElement().doubleAttribute(AttrId::height));
    }
    return rect;
}

QString BaseCleaner::getFreeId(int startId) const
{
    QString newId;
    while (newId.isEmpty() || !isFreeId(newId))
        newId = "SVGCleanerId_" + fromDouble(startId++);
    return newId;
}

bool BaseCleaner::isFreeId(const QString &id) const
{
    element_loop(svgElement()) {
        if (elem.id() == id)
            return false;
        nextElement(elem, root);
    }
    return true;
}
