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

#include "basecleaner.h"
#include "transform.h"

BaseCleaner::BaseCleaner(SvgDocument doc) : m_doc(doc)
{
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

SvgElement BaseCleaner::svgElement(SvgDocument doc)
{
    foreach (const SvgNode &node, doc.childNodes()) {
        if (node.isElement()) {
            if (node.toElement().tagName() == E_svg)
                return node.toElement();
        }

    }
    return SvgElement();
}

void BaseCleaner::joinLinearGradients(SvgElement &parent, SvgElement &child)
{
    foreach (const SvgElement &elem, child.childElements())
        parent.appendChild(elem);
    parent.setReferenceElement(AttrId::xlink_href, SvgElement());
    smartElementRemove(child);
}

// remove element with dependency checking
SvgElement BaseCleaner::smartElementRemove(SvgElement &rmElem, bool isReturnPrev)
{
    if (rmElem.parentElement().tagName() == E_defs) {
        // if removed element is 'linearGradient'
        // and it has xlink to another 'linearGradient'
        // and this parent 'linearGradient' used only by 1 gradient,
        // than merge parent 'linearGradient' with it only child
        if (rmElem.tagName() == E_linearGradient && rmElem.hasReference(AttrId::xlink_href)) {
            SvgElement defElem = rmElem.referencedElement(AttrId::xlink_href);
            if (defElem.usesCount() == 1 && !defElem.hasReference(AttrId::xlink_href)) {
                SvgElementList usedList = defElem.linkedElements();
                joinLinearGradients(usedList.first(), defElem);
            } else {
                rmElem.removeReferenceElement(AttrId::xlink_href);
            }
        }

        if (rmElem.tagName() == E_radialGradient && rmElem.hasReference(AttrId::xlink_href)) {
            rmElem.removeReferenceElement(AttrId::xlink_href);
        }
    }

    // if refereced elements of removed element was used only by it,
    // than we can remove them too
    if (rmElem.hasReferencedDefs()) {
        foreach (const uint &attrId, rmElem.referencedAttributes()) {
            if (!hasParent(rmElem, E_defs))
                continue;
            if (rmElem.referencedElement(attrId).usesCount() == 1) {
                SvgElement tElem = rmElem.referencedElement(attrId);
                smartElementRemove(tElem);
            }
            rmElem.removeReferenceElement(attrId);
        }
    }

    // if we remove used element - replace all attributes linked to it with 'none'
    if (rmElem.usesCount() != 0) {
        foreach (SvgElement usedElem, rmElem.linkedElements()) {
            const uint attrId = usedElem.referenceAttribute(rmElem);
            if (attrId != 0) {
                usedElem.removeReferenceElement(attrId);
                if (attrId != AttrId::xlink_href)
                    usedElem.setAttribute(attrId, V_none);
            }
        }
    }

    // alse process all children items
    loop_children (rmElem)
        smartElementRemove(elem);

    return rmElem.parentElement().removeChild(rmElem, isReturnPrev);
}

void BaseCleaner::removeAndMoveToPrev(SvgElement &elem)
{
    SvgElement tElem = elem;
    elem = prevElement(elem);
    smartElementRemove(tElem);
}

void BaseCleaner::removeAndMoveToPrevSibling(SvgElement &elem)
{
    SvgElement tElem = elem;
    elem = prevSiblingElement(elem);
    smartElementRemove(tElem);
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

bool BaseCleaner::hasUsedParent(const SvgElement &elem)
{
    SvgElement parent = elem;
    while (!parent.isNull()) {
        if (parent.isUsed())
            return true;
        parent = parent.parentElement();
    }
    return false;
}

QRectF BaseCleaner::viewBoxRect() const
{
    QRectF rect;
    if (svgElement().hasAttribute(AttrId::viewBox)) {
        QStringList list = svgElement().attribute(AttrId::viewBox).split(QL1C(' '),
                                                                         QString::SkipEmptyParts);
        rect.setRect(toDouble(list.at(0)), toDouble(list.at(1)),
                     toDouble(list.at(2)), toDouble(list.at(3)));
    } else if (svgElement().hasAttribute(AttrId::width) && svgElement().hasAttribute(AttrId::height)) {
        rect.setRect(0, 0, svgElement().doubleAttribute(AttrId::width),
                           svgElement().doubleAttribute(AttrId::height));
    }
    return rect;
}

QString BaseCleaner::genFreeId() const
{
    return QL1S("SVGCleanerId_") + document().takeFreeId();
}

// TODO: maybe store colors as QColor
bool BaseCleaner::isGradientStopsEqual(const SvgElement &elem1, const SvgElement &elem2)
{
    if (elem1.childElementCount() != elem2.childElementCount())
        return false;

    if (elem1.childElementCount() == 0)
        return true;

    static const QString oneStr = QL1S("1");
    static const QString colorStr = QL1S("#000000");

    SvgElement child1 = elem1.firstChildElement();
    SvgElement child2 = elem2.firstChildElement();
    while (!child1.isNull()) {
        if (child1.tagName() != child2.tagName())
            return false;

        // TODO: test which attribute is usually different first and sort checks
        if (   child1.attribute(AttrId::offset, V_zero)
            != child2.attribute(AttrId::offset, V_zero))
            return false;
        if (   child1.attribute(AttrId::stop_color, colorStr)
            != child2.attribute(AttrId::stop_color, colorStr))
            return false;
        if (   child1.attribute(AttrId::stop_opacity, oneStr)
            != child2.attribute(AttrId::stop_opacity, oneStr))
            return false;

        child1 = child1.nextSiblingElement();
        child2 = child2.nextSiblingElement();
    }

    return true;
}
