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

#include <QVector>

#include "paths/paths.h"
#include "stringwalker.h"
#include "replacer.h"

// TODO: replace equal 'fill', 'stroke', 'stop-color', 'flood-color' and 'lighting-color' attr
//       with 'color' attr
//       addon_the_couch.svg
// TODO: If 'x1' = 'x2' and 'y1' = 'y2', then the area to be painted will be painted as
//       a single color using the color and opacity of the last gradient stop.
// TODO: try to recalculate 'userSpaceOnUse' to 'objectBoundingBox'
// TODO: maybe move frequently used styles to CDATA
// TODO: rename element id when changing tagname

void Replacer::convertSizeToViewbox() const
{
    if (!svgElement().hasAttribute(AttrId::viewBox)) {
        if (svgElement().hasAttribute(AttrId::width) && svgElement().hasAttribute(AttrId::height)) {
            QString width  = fromDouble(svgElement().doubleAttribute(AttrId::width));
            QString height = fromDouble(svgElement().doubleAttribute(AttrId::height));
            svgElement().setAttribute(AttrId::viewBox, QString(QL1S("0 0 %1 %2")).arg(width).arg(height));
            svgElement().removeAttribute(AttrId::width);
            svgElement().removeAttribute(AttrId::height);
        }
    } else {
        QRectF rect = viewBoxRect();
        if (rect.isNull())
            return;
        if (svgElement().hasAttribute(AttrId::width)) {
            if (isZero(rect.width() - svgElement().doubleAttribute(AttrId::width)))
                svgElement().removeAttribute(AttrId::width);
        }
        if (svgElement().hasAttribute(AttrId::height)) {
            if (isZero(rect.height() - svgElement().doubleAttribute(AttrId::height)))
                svgElement().removeAttribute(AttrId::height);
        }
    }
}

// TODO: join paths with only style different
//       Anonymous_Chesspiece_-_bishop.svg, dune73_Firewall-2D.svg
// TODO: replace paths with use, when paths has only first segment different
//       Anonymous_Flag_of_South_Korea.svg
// TODO: join paths with equal styles to one 'd' attr
// TODO: split and replace with 'use', paths which contains equal subpaths
void Replacer::processPaths() const
{
    bool skip = true;
    foreach (const int &id, Keys.pathsKeysId() + Keys.pathsUtilsKeysId()) {
        if (Keys.flag(id)) {
            skip = false;
            break;
        }
    }
    if (skip)
        return;

    element_loop (svgElement()) {
        if (elem.tagName() != E_path)
            continue;

        bool canApplyTransform = false;
        if (Keys.flag(Key::ApplyTransformsToPaths))
            canApplyTransform = isPathValidToTransform(elem);
        bool isTransformApplied = false;
        Path().processPath(elem, canApplyTransform, &isTransformApplied);
        if (canApplyTransform) {
            if (isTransformApplied) {
                updateLinkedDefTransform(elem);
                elem.removeTransform();
            }
        }
        if (elem.attribute(AttrId::d).isEmpty())
            elem = smartElementRemove(elem, true);
    }
}


/*!
    \page equal-paths-to-use.html
    \title Replace paths with only first segments different by the 'use'

    If paths are different only by start position - we can replace them.
    Before:
    \code
        <path d="m 10 20 l 15 -10 l 28 40 l -15 10 z"/>
        <path d="m 60 30 l 15 -10 l 28 40 l -15 10 z"/>
    \endcode
    After:
    \code
        <path id="p" d="m 10 20 l 15 -10 l 28 40 l -15 10 z"/>
        <use x="50" y="10" xlink:href="#p"/>
    \endcode
*/
struct Paths {
    SvgElement elem;
    Transform absTr;
    PathSegmentList segList;
};

void Replacer::replaceEqualPathsWithUse() const
{
    QList<Paths> pathsList;
    element_loop (svgElement()) {
        if (    elem.tagName() == E_path
            && !hasParent(elem, Element::E_defs)
            && !elem.isUsed())
        {
            Paths p;
            p.elem = elem;
            p.absTr = elem.absoluteTransform();
            p.segList = elem.attributeItem(AttrId::d).segList();
            pathsList << p;
        }
    }

    for (int i = 0; i < pathsList.size(); ++i) {
        SvgElement elem1 = pathsList.at(i).elem;
        if (elem1.hasStyleAttributes())
            continue;
        const Transform ats1 = pathsList.at(i).absTr;
        const PathSegmentList segList1 = pathsList.at(i).segList;

        // TODO: check paths before current
        // for (int j = 0; j < pathsList.size(); ++j) {
        for (int j = i+1; j < pathsList.size(); ++j) {
            if (elem1 == pathsList.at(j).elem)
                continue;

            const PathSegmentList segList2 = pathsList.at(j).segList;

            if (segList1.size() != segList2.size())
                continue;

            // first/Move segments must be different
            if (segList1.first() == segList2.first())
                continue;

            bool isEqual = true;
            // skip first segment in both lists
            for (int k = 1; k < segList1.size(); ++k) {
                if (segList1.at(k) != segList2.at(k)) {
                    isEqual = false;
                    break;
                }
            }
            if (!isEqual)
                continue;

            const Transform ats2 = pathsList.at(j).absTr;
            // both elements should have transform
            if (!ats1.isNull() && !ats2.isNull()) {
                // only translate transform supported by SVG
                if (ats1.type() != TransformPrivate::Translate)
                    continue;
                // only paths with equal transform can be reused
                if (ats1 != ats2)
                    continue;
            } else if (!ats1.isNull() || !ats2.isNull()) {
                // skip if only one element has transform
                continue;
            }

            SvgElement elem2 = pathsList.at(j).elem;
            elem2.setTagName(Element::E_use);
            if (elem1.attribute(AttrId::id).isEmpty())
                elem1.setAttribute(AttrId::id, genFreeId());
            elem2.setReferenceElement(AttrId::xlink_href, elem1);
            elem2.setAttribute(AttrId::x, fromDouble(segList2.first().x - segList1.first().x));
            elem2.setAttribute(AttrId::y, fromDouble(segList2.first().y - segList1.first().y));
            elem2.removeAttribute(AttrId::bbox);
            elem2.removeAttribute(AttrId::transform);

            elem2.removeAttribute(AttrId::d);

            pathsList.removeAt(j);
            j--;
        }
    }
}

// TODO: transform paths with equal transforms and equal defs
// 7zip.svg
bool Replacer::isPathValidToTransform(SvgElement &pathElem) const
{
    if (!pathElem.hasTransform())
        return false;

    // non proportional transform could not be applied to path with stroke
    if (pathElem.hasParentAttribute(AttrId::stroke, true)) {
        if (pathElem.parentAttribute(AttrId::stroke, true) != V_none) {
            if (!pathElem.transform().isProportionalScale())
                return false;
        }
    }

    if (pathElem.hasAttribute(AttrId::clip_path) || pathElem.hasAttribute(AttrId::mask))
        return false;
    if (pathElem.isUsed())
        return false;
    if (pathElem.hasAttribute(AttrId::filter)) {
        // we can apply transform to blur filter, but only when it's used by only this path
        if (pathElem.referencedElement(AttrId::filter).usesCount() != 1)
            return false;
        if (!isBlurFilter(pathElem.referencedElement(AttrId::filter)))
            return false;
    }


    static const IntList attrList = IntList() << AttrId::fill << AttrId::stroke;
    foreach (const uint &attrId, attrList) {
        if (pathElem.hasReference(attrId)) {
            SvgElement refElem = pathElem.referencedElement(attrId);
            if (refElem.usesCount() > 1)
                return false;

            if (   refElem.tagName() != E_linearGradient
                && refElem.tagName() != E_radialGradient)
                return false;
            if (refElem.attribute(AttrId::gradientUnits) != QL1S("userSpaceOnUse"))
                return false;
        }
    }
    return true;
}

bool Replacer::isBlurFilter(const SvgElement &elem)
{
    static const QStringList filterAttrs = QStringList()
        << A_x << A_y << A_width << A_height;

    if (elem.childElementCount() == 1) {
        if (elem.firstChildElement().tagName() == E_feGaussianBlur) {
            // cannot apply transform to filter with not default region
            if (!elem.hasAttributes(filterAttrs))
                return true;
        }
    }
    return false;
}

void Replacer::updateLinkedDefTransform(SvgElement &elem) const
{
    static const IntList attrList = IntList() << AttrId::fill << AttrId::stroke << AttrId::filter;
    foreach (const uint &attrId, attrList) {
        if (!elem.hasReference(attrId))
            continue;

        SvgElement defElem = elem.referencedElement(attrId);
        if ((  defElem.tagName() == E_linearGradient
            || defElem.tagName() == E_radialGradient)
               && defElem.attribute(AttrId::gradientUnits) == QL1S("userSpaceOnUse"))
        {
            Transform gts = defElem.transform();
            Transform ts = elem.transform();
            if (!gts.isNull())
                ts.append(gts);
            defElem.removeTransform();
            defElem.setTransform(ts);
        } else if (defElem.tagName() == E_filter) {
            Transform ts = elem.transform();
            SvgElement stdDevElem = defElem.firstChildElement();
            double oldStd = stdDevElem.doubleAttribute(AttrId::stdDeviation);
            QString newStd = fromDouble(oldStd * ts.scaleFactor());
            stdDevElem.setAttribute(AttrId::stdDeviation, newStd);
        }
    }
}

class EqElement {
public:
    IntHash attrHash;
    SvgElement elem;
    bool operator ==(const EqElement &elem) const {
        return elem.elem == this->elem;
    }
};

// FIXME: parent styles should be set to elem before moving to defs
//        address-book-new.svg
// TODO: reuse groups
void Replacer::replaceEqualElementsByUse() const
{
    static const IntList rectAttrs = IntList()
        << AttrId::x << AttrId::y << AttrId::width << AttrId::height << AttrId::rx << AttrId::ry;
    static const IntList circleAttrs = IntList()
        << AttrId::cx << AttrId::cy << AttrId::r;
    static const IntList ellipseAttrs = IntList()
        << AttrId::cx << AttrId::cy << AttrId::rx << AttrId::ry;

    QList<EqElement> elemList;
    element_loop (svgElement()) {

        bool canUse = false;
        if (elem.tagName() == E_path || elem.tagName() == E_rect)
            canUse = true;
        if (hasParent(elem, E_defs)) {
            if (   elem.parentElement().tagName() != E_defs
                && elem.parentElement().tagName() != E_clipPath)
                canUse = false;
        }

        if (!canUse)
            continue;

        EqElement e;
        IntHash hash;
        if (elem.tagName() == E_path) {
            hash.insert(AttrId::d, elem.attribute(AttrId::d));
        } else if (elem.tagName() == E_rect) {
            foreach (const uint &attrId, rectAttrs)
                hash.insert(attrId, elem.attribute(attrId));
        } else if (elem.tagName() == E_circle) {
            foreach (const uint &attrId, circleAttrs)
               hash.insert(attrId, elem.attribute(attrId));
        } else if (elem.tagName() == E_ellipse) {
            foreach (const uint &attrId, ellipseAttrs)
                hash.insert(attrId, elem.attribute(attrId));
        }
        e.attrHash = hash;
        e.elem = elem;
        elemList << e;
    }

    SvgElementList defsPathsList;
    element_loop (defsElement()) {
        if (   elem.tagName() == E_path
            && elem.parentElement().tagName() != E_clipPath
            && elem.styleHash().isEmpty())
        {
            defsPathsList << elem;
        }
    }

    while (!elemList.isEmpty()) {
        EqElement mainEqElem = elemList.takeFirst();
        QList<EqElement> equalElems;

        // TODO: hash compare too slow
        // atlas.svg
        for (int j = 0; j < elemList.size(); ++j) {
            if (   mainEqElem.elem.tagName() == elemList.at(j).elem.tagName()
                && mainEqElem.elem != elemList.at(j).elem
                && mainEqElem.attrHash == elemList.at(j).attrHash) {
                    equalElems << elemList.at(j);
            }
        }

        if (equalElems.isEmpty())
            continue;

        // TODO: find elem in defs before creating new one
        SvgElement newElem;

        // try to find 'path' elem in defs
        if (mainEqElem.elem.tagName() == E_path) {
            foreach (const SvgElement &elem, defsPathsList) {
                if (elem.tagName() == E_path && elem != mainEqElem.elem) {
                    if (elem.attribute(AttrId::d) == mainEqElem.elem.attribute(AttrId::d)) {
                        newElem = elem;
                        break;
                    }
                }
            }
        }

        // if not found - create new one
        if (newElem.isNull()) {
            newElem = document().createElement(mainEqElem.elem.tagName());
            foreach (const uint &attrId, mainEqElem.attrHash.keys())
                newElem.setAttribute(mainEqElem.elem.attributeItem(attrId));
            defsElement().appendChild(newElem);
            defsPathsList << newElem;
        }

        // gen new id
        if (!newElem.hasAttribute(AttrId::id))
            newElem.setAttribute(AttrId::id, genFreeId());

        // convert elements to 'use'
        equalElems << mainEqElem;
        foreach (const EqElement &eqElem, equalElems) {
            if (eqElem.elem == newElem)
                continue;

            SvgElement elem = eqElem.elem;
            elem.setTagName(E_use);
            elem.setReferenceElement(AttrId::xlink_href, newElem);
            elem.removeAttribute(AttrId::bbox);
            foreach (const uint &attrId, mainEqElem.attrHash.keys())
                elem.removeAttribute(attrId);
            elemList.removeOne(eqElem);
        }
    }
}

// TODO: process use element which is used too
// webmichl_wristwatch_1_-_chronometer.svg
void Replacer::moveStyleFromUsedElemToUse() const
{
    element_loop (svgElement()) {

        if (!elem.isUsed())
            continue;

        if (!hasParent(elem, E_defs))
            continue;

        if (Properties::defsList.contains(elem.tagName()))
            continue;

        SvgElementList usedElemList = elem.linkedElements();

        // use elem cannot overwrite style properties of used element
        foreach (const SvgAttribute &attr, elem.attributesList()) {
            foreach (SvgElement usedElem, usedElemList) {
                if (attr.isStyle())
                    usedElem.removeAttribute(attr.id());
            }
        }

        SvgElement firstUse = usedElemList.first();
        foreach (const uint &attrId, firstUse.styleAttributesList()) {
            QString value = firstUse.attribute(attrId);
            bool isEqual = true;
            foreach (const SvgElement &usedElem, usedElemList) {
                if (usedElem.attribute(attrId) != value) {
                    isEqual = false;
                    break;
                }
            }

            if (isEqual) {
                elem.setAttribute(firstUse.attributeItem(attrId));
                foreach (SvgElement usedElem, usedElemList)
                    usedElem.removeAttribute(attrId);
            }
        }
    }
}

void Replacer::convertUnits() const
{
    QRectF rect = viewBoxRect();
    // TODO: merge code
    SvgAttribute attr = svgElement().attributeItem(AttrId::width);
    if (!attr.isNone()) {
        QString value = attr.value();
        if (value.contains(LengthType::Percent) && rect.isNull())
            qFatal("could not convert width in percentage into px without viewBox");
        bool ok;
        double width = toDouble(convertUnitsToPx(value, rect.width()), &ok);
        if (!ok)
            qFatal("could not convert width to px");
        attr.setValue(fromDouble(width));
    }

    attr = svgElement().attributeItem(AttrId::height);
    if (!attr.isNone()) {
        QString value = attr.value();
        if (value.contains(LengthType::Percent) && rect.isNull())
            qFatal("could not convert height in percentage into px without viewBox");
        bool ok;
        double height = toDouble(convertUnitsToPx(value, rect.height()), &ok);
        if (!ok)
            qFatal("could not convert height to px");
        attr.setValue(fromDouble(height));
    }

    static const IntList numericStyleList = IntList()
        << AttrId::fill_opacity << AttrId::opacity << AttrId::stop_opacity
        << AttrId::stroke_miterlimit << AttrId::stroke_opacity << AttrId::stroke_width
        << AttrId::font_size << AttrId::kerning << AttrId::letter_spacing << AttrId::word_spacing
        << AttrId::baseline_shift << AttrId::stroke_dashoffset;

    // TODO: process 'offset' attr with %
    element_loop (document().documentElement()) {
        QString currTag = elem.tagName();

        foreach (const uint &attrId, numericStyleList) {
            if (!elem.hasAttribute(attrId))
                continue;

            QString value = elem.attribute(attrId);
            if (value.isEmpty())
                continue;

            bool ok = false;
            double num = toDouble(value, &ok);
            if (!ok && !value.startsWith(UrlPrefix)) {
                if (attrId == AttrId::stroke_width) {
                    if (value.endsWith(LengthType::Percent)) {
                        static QRectF m_viewBoxRect = viewBoxRect();
                        if (m_viewBoxRect.isNull())
                            qFatal("could not detect viewBox");
                        elem.setAttribute(attrId, convertUnitsToPx(value, m_viewBoxRect.width()));
                    }
                    else if (value.endsWith(LengthType::em) || value.endsWith(LengthType::ex)) {
                        QString fontSizeStr = elem.parentAttribute(AttrId::font_size);
                        double fontSize = toDouble(convertUnitsToPx(fontSizeStr));
                        if (fontSize == 0)
                            qFatal("could not convert em/ex values "
                                   "without font-size attribute is set.");
                        elem.setAttribute(attrId, convertUnitsToPx(value, fontSize));
                    } else {
                        elem.setAttribute(attrId, convertUnitsToPx(value));
                    }
                } else {
                    elem.setAttribute(attrId, convertUnitsToPx(value));
                }
                num = toDouble(elem.attribute(attrId));
            }
            elem.setAttribute(attrId, fromDouble(num, Round::Attribute));
        }

        if (elem.hasAttribute(AttrId::font_size)) {
            bool ok = false;
            QString fontSizeStr = elem.attribute(AttrId::font_size);
            toDouble(fontSizeStr, &ok);
            if (!ok) {
                if (   fontSizeStr.endsWith(LengthType::ex)
                    || fontSizeStr.endsWith(LengthType::em)
                    || fontSizeStr.endsWith(LengthType::Percent))
                {
                    QString parentFontSize = elem.parentAttribute(AttrId::font_size);
                    if (parentFontSize.isEmpty() || parentFontSize == V_zero)
                        qFatal("could not calculate relative font-size");
                    QString newFontSize = convertUnitsToPx(fontSizeStr,
                                                                  toDouble(parentFontSize));
                    if (newFontSize == V_zero)
                        elem.removeAttribute(AttrId::font_size);
                    else
                        elem.setAttribute(AttrId::font_size, newFontSize);
                } else {
                    QString newFontSize = convertUnitsToPx(fontSizeStr);
                    elem.setAttribute(AttrId::font_size, newFontSize);
                }
            }
        }

        foreach (const uint &attrId, Properties::digitListIds) {
            if (!elem.hasAttribute(attrId))
                continue;
            // fix attributes like:
            // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
            // FIXME: ignores list based attr
            QString attrValue = elem.attribute(attrId);
            if (attrValue.contains(QL1C(' ')))
                attrValue = attrValue.left(attrValue.indexOf(QL1C(' ')));

            // TODO: process gradients attrs
            if (attrValue.contains(LengthType::Percent)) {
                if (currTag != E_radialGradient && currTag != E_linearGradient) {
                    QString attrName = attrIdToStr(attrId);
                    if (attrName.contains(A_x) || attrId == AttrId::width)
                        attrValue = convertUnitsToPx(attrValue, rect.width());
                    else if (attrName.contains(A_y) || attrId == AttrId::height)
                       attrValue = convertUnitsToPx(attrValue, rect.height());
                }
            } else if (attrValue.endsWith(LengthType::ex) || attrValue.endsWith(LengthType::em)) {
                double fontSize = toDouble(elem.parentAttribute(AttrId::font_size, true));
                if (fontSize == 0)
                    qFatal("could not convert em/ex values "
                           "without font-size attribute is set");
                attrValue = convertUnitsToPx(attrValue, fontSize);
            } else {
                attrValue = convertUnitsToPx(attrValue);
            }
            elem.setAttribute(attrId, attrValue);
        }
    }
}

void Replacer::convertColors() const
{
    static const bool isConvertColors
            = (Keys.flag(Key::ConvertColorToRRGGBB) || Keys.flag(Key::ConvertRRGGBBToRGB));
    if (!isConvertColors)
        return;

    static const IntList colorAttrs = IntList() << AttrId::fill << AttrId::stroke << AttrId::color
                                                << AttrId::stop_color << AttrId::flood_color;

    element_loop (document().documentElement()) {
        foreach (const uint &attrId, colorAttrs) {
            if (!elem.hasAttribute(attrId))
                continue;

            if (elem.hasReference(attrId))
                continue;

            QString value = elem.attribute(attrId);
            if (value == V_none)
                continue;

            // attribute cannot contain url as string
            // if it does - remove it
            if (value.startsWith(UrlPrefix)) {
                elem.removeAttribute(attrId);
                continue;
            }

            QString newColor = value.toLower();

            // convert 'rgb (255, 255, 255)' to #RRGGBB
            if (Keys.flag(Key::ConvertColorToRRGGBB)) {
                if (newColor.contains(QL1S("rgb"))) {
                    StringWalker sw(newColor);
                    QVector<double> nums;
                    nums.reserve(3);
                    while (!sw.atEnd()) {
                        sw.jumpTo(QL1C('('));
                        sw.skipSpaces();
                        sw.next();
                        for (int i = 0; i < 3; ++i) {
                            nums << sw.number();
                            if (sw.current() == LengthType::Percent)
                                sw.next();
                            if (sw.current() == QL1C(','))
                                sw.next();
                        }
                        sw.jumpTo(QL1C(')'));
                        sw.next();
                    }
                    // convert 'rgb (100%, 100%, 100%)' to 'rgb (255, 255, 255)'
                    if (newColor.contains(LengthType::Percent)) {
                        for (int i = 0; i < 3; ++i)
                            nums[i] = nums.at(i) * 255 / 100;
                    }
                    newColor = QL1C('#');
                    foreach (const double &value, nums)
                        newColor += QString::number((int)value, 16).rightJustified(2, QL1C('0'));
                }

                // check is color set by name
                if (!newColor.contains(QL1C('#'))) {
                    QString colorHash = initColorsHash().value(newColor);
                    if (!colorHash.isEmpty())
                        newColor = colorHash;
                }
            }

            if (Keys.flag(Key::ConvertRRGGBBToRGB)) {
                if (newColor.startsWith(QL1C('#'))) {
                    // try to convert #rrggbb to #rgb
                    if (newColor.size() == 7) { // #000000
                        int inter = 0;
                        for (int i = 1; i < 6; i += 2) {
                            if (newColor.at(i) == newColor.at(i+1))
                                inter++;
                        }
                        if (inter == 3)
                            newColor = QL1C('#') + newColor.at(1) + newColor.at(3) + newColor.at(5);
                    }
                }
            }
            elem.setAttribute(attrId, newColor);
        }
    }
}

// Extract attributes from 'style' attribute.
// Also removes non standard attributes.
IntHash splitStyle(const QString &style, SvgElement &elem)
{
    IntHash hash;
    if (style.isEmpty())
        return hash;

    static const QChar colonChar     = QL1C(':');
    static const QChar semicolonChar = QL1C(';');
    static const QChar asteriskChar  = QL1C('*');
    static const QChar andChar       = QL1C('&');

    StringWalker sw(style);
    while (sw.isValid() && !sw.atEnd()) {
        sw.skipSpaces();

        // skip comments inside attribute value
        if (sw.current() == QL1C('/')) {
            sw.next(2); // skip /*
            sw.jumpTo(asteriskChar);
            sw.next(2); // skip */
            sw.skipSpaces();
        }

        if (sw.current() == QL1C('-'))
            break;

        const int len = sw.jumpTo(colonChar);
        uint id = sw.readBeforeId(len);
        if (!isDefaultAttribute(id)) {

            // do not process styles which are linked to data in ENTITY
            // like: '&st0;'
            QString name = sw.readBefore(len);
            if (!name.isEmpty() && name.at(0) == andChar)
                break;

            id = 0;
        }

        sw.next(); // skip ':'

        sw.skipSpaces();

        QString value = sw.readBefore(sw.jumpTo(semicolonChar));
        if (!sw.atEnd())
            sw.next(); // skip ';'
        sw.skipSpaces();

        if (id == 0)
            continue;

        // add to hash
        if (elem.isNull())
            hash.insert(id, value);
        else
            elem.setAttribute(id, value);
    }

    return hash;
}

/*
 * 'xmlns' links and element styles can be set in DTD, ENTITY.
 * Mostly used by Adobe Illustrator.
 *
 * For example:
 *     <!ENTITY ns_flows "http://ns.adobe.com/Flows/1.0/">
 *     <!ENTITY st1 "fill:url(#SVGID_1_);">
 *
 * Only link and style supported.
 */
void Replacer::convertEntityData() const
{
    SvgNodeList nodeList = document().childNodes();
    QString text;
    while (!nodeList.isEmpty()) {
        SvgNode node = nodeList.takeFirst();
        if (node.isText()) {
            QString dtdText = node.toText().text();
            if (!dtdText.startsWith(QL1S("<!DOCTYPE")))
                break;
            text = dtdText;
            break;
        }
    }
    if (text.isEmpty() || !text.contains(QL1C('[')))
        return;

    StringHash entityHash;

    StringWalker sw(text);
    sw.jumpTo(QL1C('['));
    sw.next();
    sw.skipSpaces();
    while (sw.isValid() && !sw.atEnd()) {
        if (sw.current() != QL1C('<'))
            break;

        // skip '!ENTITY'
        sw.jumpToSpace();
        sw.next();

        // parse name
        QString name = sw.readBefore(sw.jumpToSpace());

        // skip unnecessary data
        sw.skipSpaces();
        if (sw.current() != QL1C('\"'))
            break;
        sw.next();

        // parse data
        QString data = sw.readBefore(sw.jumpTo(QL1C('\"')));

        sw.next();
        sw.skipSpaces();
        if (sw.current() != QL1C('>'))
            break;
        sw.next();
        sw.skipSpaces();

        entityHash.insert(QL1C('&') + name + QL1C(';'), data);
    }

    if (entityHash.isEmpty())
        return;

    // replace styles
    element_loop (svgElement()) {
        if (elem.hasAttribute(AttrId::style)) {
            QString value = elem.attribute(AttrId::style);
            foreach (const QString &name, entityHash.keys()) {
                if (value.contains(name)) {
                    value.replace(name, entityHash.value(name));
                }
            }
            elem.setAttribute(AttrId::style, value);
        }
    }

    // replace links
    foreach (const QString &attrName, svgElement().extAttributesList()) {
        if (attrName.startsWith(QL1S("xmlns"))) {
            QString value = svgElement().attribute(attrName);
            if (entityHash.contains(value)) {
                svgElement().setAttribute(attrName, entityHash.value(value));
            }
        }
    }
}

// TODO: rewrite parsing (atlas.svg)
void Replacer::convertCDATAStyle() const
{
    QStringList styleList;
    element_loop (document().documentElement()) {
        if (elem.tagName() == E_style) {
            styleList << elem.text();
            elem = elem.parentElement().removeChild(elem, true);
        }
    }
    if (styleList.isEmpty()) {
        // remove class attribute when no CDATA set
        SvgElement elem = document().documentElement();
        SvgElement root = elem;
        while (!elem.isNull()) {
            elem.removeAttribute(AttrId::class_);
            nextElement(elem, root);
        }
        return;
    }

    StringHash classHash;
    for (int i = 0; i < styleList.count(); ++i) {
        QString text = styleList.at(i);
        text.replace(QL1S("\n"), QL1S(" ")).replace(QL1S("\t"), QL1S(" "));
        if (text.isEmpty())
            continue;
        // remove comments
        // better to use positive lookbehind, but qt4 didn't support it
        text.remove(QRegExp(QL1S("[^\\*]\\/(?!\\*)")));
        text.remove(QRegExp(QL1S("[^\\/]\\*(?!\\/)")));
        text.remove(QRegExp(QL1S("\\/\\*[^\\/\\*]*\\*\\/")));
        QStringList classList = text.split(QRegExp(QL1S(" +(\\.|@)")), QString::SkipEmptyParts);
        foreach (const QString &currClass, classList) {
            QStringList tmpList = currClass.split(QRegExp(QL1S("( +|)\\{")));
            if (tmpList.size() == 2) {
                classHash.insert(tmpList.at(0).trimmed(),
                                 QString(tmpList.at(1)).remove(QRegExp(QL1S("\\}.*"))));
            }
        }
    }

    SvgElement emptyElem;
    element_loop (document().documentElement()) {
        if (!elem.hasAttribute(AttrId::class_))
            continue;

        IntHash newHash;
        QStringList classList = elem.attribute(AttrId::class_).split(QL1C(' '), QString::SkipEmptyParts);
        foreach (const QString &classStr, classList) {
            if (classHash.contains(classStr)) {

                IntHash tempHash = splitStyle(classHash.value(classStr), emptyElem);
                foreach (const uint &key, tempHash.keys())
                    newHash.insert(key, tempHash.value(key));
            }
        }
        foreach (const uint &attrId, newHash.keys()) {
            if (!elem.hasAttribute(attrId))
                elem.setAttribute(attrId, newHash.value(attrId));
        }
        elem.removeAttribute(AttrId::class_);
    }
}

void Replacer::prepareDefs() const
{
    // move all gradient, filters, etc. to 'defs' element
    element_loop (svgElement()) {
        if (elem.parentElement() != defsElement()) {
            if (Properties::defsList.contains(elem.tagName())) {
                SvgElement tElem = elem;
                elem = prevElement(elem);
                defsElement().appendChild(tElem);
                continue;
            }
        }
    }

    // ungroup all defs in defs
    element_loop (svgElement()) {
        if (   elem.parentElement().tagName() == E_defs
            && elem.parentElement() != defsElement())
        {
            SvgElement tElem = elem;
            elem = prevElement(elem);
            defsElement().appendChild(tElem);
        }
    }

    // remove empty defs
    element_loop (svgElement()) {
        if (    elem.tagName() == E_defs
            && !elem.hasChildren()
            &&  elem != defsElement())
        {
            elem = smartElementRemove(elem, true);
        } else {
            nextElement(elem, root);
        }
    }

    // move 'defs' element to first place in 'svg' element
    svgElement().insertBefore(defsElement(), svgElement().firstChildElement());
}

void Replacer::fixWrongAttr() const
{
    IntList tmpList = IntList() << AttrId::fill << AttrId::stroke;
    element_loop (svgElement()) {
        QString currTag = elem.tagName();

        // remove single quotes from 'font-family' value
        if (elem.hasAttribute(AttrId::font_family)) {
            if (elem.attribute(AttrId::font_family).contains(QL1C('\''))) {
                elem.setAttribute(AttrId::font_family,
                                  elem.attribute(AttrId::font_family).remove(QL1C('\'')));
            }
        }

        // remove wrong fill and stroke attributes like:
        // fill="url(#radialGradient001) rgb(0, 0, 0)"
        foreach (const uint &attrId, tmpList) {
            if (elem.hasAttribute(attrId)) {
                QString attrValue = elem.attribute(attrId);
                if (attrValue.contains(QL1S("url"))) {
                    int pos = attrValue.indexOf(QL1C(' '));
                    if (pos > 0) {
                        attrValue.remove(pos, attrValue.size());
                        elem.setAttribute(attrId, attrValue);
                    }
                }
            }
        }

        // gradient with stop elements does not need xlink:href attribute
        if (currTag == E_linearGradient || currTag == E_radialGradient) {
            if (elem.hasChildrenElement() && elem.hasAttribute(AttrId::xlink_href))
                elem.removeAttribute(AttrId::xlink_href);
        } else if (currTag == E_use) {
            if (elem.doubleAttribute(AttrId::width) < 0)
                elem.setAttribute(AttrId::width, V_zero);
            if (elem.doubleAttribute(AttrId::height) < 0)
                elem.setAttribute(AttrId::height, V_zero);
        } else if (currTag == E_rect) {
            // if only one radius are set or '0', create missing with same value
            if (elem.hasAttribute(AttrId::ry)
                    && (!elem.hasAttribute(AttrId::rx) || elem.doubleAttribute(AttrId::rx) == 0.0))
            {
                elem.setAttribute(AttrId::rx, elem.attribute(AttrId::ry));
            }

            if (elem.hasAttribute(AttrId::rx)
                    && (!elem.hasAttribute(AttrId::ry) || elem.doubleAttribute(AttrId::ry) == 0.0))
            {
                elem.setAttribute(AttrId::ry, elem.attribute(AttrId::rx));
            }

            // rx/ry can not be bigger then width/height
            double halfWidth = elem.doubleAttribute(AttrId::width) / 2;
            double halfHeight = elem.doubleAttribute(AttrId::height) / 2;
            if (elem.hasAttribute(AttrId::rx) && elem.doubleAttribute(AttrId::rx) >= halfWidth)
                elem.setAttribute(AttrId::rx, fromDouble(halfWidth));
            if (elem.hasAttribute(AttrId::ry) && elem.doubleAttribute(AttrId::ry) >= halfHeight)
                elem.setAttribute(AttrId::ry, fromDouble(halfHeight));
        }
    }
}

// TODO: remove spaces from 'enable-background' attr
void Replacer::finalFixes() const
{
    element_loop (svgElement()) {
        QString tagName = elem.tagName();

        // TODO: to internal attr
        elem.removeAttribute(AttrId::bbox);

        if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
            if (tagName == E_rect || tagName == E_use) {
                elem.removeAttributeIf(AttrId::ry, elem.attribute(AttrId::rx));
                elem.removeAttributeIf(AttrId::x, V_zero);
                elem.removeAttributeIf(AttrId::y, V_zero);
            }
            else if (tagName == E_circle) {
                elem.removeAttributeIf(AttrId::cx, V_zero);
                elem.removeAttributeIf(AttrId::cy, V_zero);
            }
            else if (tagName == E_line) {
                elem.removeAttributeIf(AttrId::x1, V_zero);
                elem.removeAttributeIf(AttrId::y1, V_zero);
                elem.removeAttributeIf(AttrId::x2, V_zero);
                elem.removeAttributeIf(AttrId::y2, V_zero);
            } else if (tagName == E_marker) {
                elem.removeAttributeIf(QL1S("refX"), V_zero);
                elem.removeAttributeIf(QL1S("refY"), V_zero);
            }
        }

        if (Keys.flag(Key::RemoveInvisibleElements)) {
            // remove empty defs
            if (elem.tagName() == E_defs) {
                if (!elem.hasChildrenElement()) {
                    SvgElement tElem = prevElement(elem);
                    elem.parentElement().removeChild(elem);
                    elem = tElem;
                }
            }
        }
    }
}

void plusOne(QList<int> &list, int offset = 0)
{
    // 62 is charList size
    static const int magic = 62;

    if (offset > list.size()-1) {
        for (int i = 0; i < list.size(); ++i)
            list[i] = 0;
        list << 0;
        return;
    }
    if (list.at(offset) + 1 == magic) {
        list[offset] = 0;
        plusOne(list, offset+1);
    } else {
        list[offset]++;
    }
}

// convert id names to short one using custom numeral system
void Replacer::trimIds() const
{
    static QList<QChar> charList;
    if (charList.isEmpty()) {
        for (int i = 97; i < 123; ++i) // a-z
            charList << QChar(i);
        for (int i = 65; i < 91; ++i)  // A-Z
            charList << QChar(i);
        for (int i = 48; i < 58; ++i)  // 0-9
            charList << QChar(i);
    }

    int pos = 0;
//    StringHash idHash;
    QList<int> numList = QList<int>() << 0;
    bool isSkipped = true;
    const bool disableDigitId = Keys.flag(Key::DisableDigitId);
    element_loop (svgElement()) {
        if (!elem.hasAttribute(AttrId::id))
            continue;

        // by XML spec 'id' attribute could not start with digit
        // so we need skip it
        if (disableDigitId) {
            QChar c = charList.at(numList.last());
            while (c.isDigit() || c == QL1C('Z')) {
                plusOne(numList);
                c = charList.at(numList.last());
                isSkipped = true;
            }
        }

        // gen new id
        if (!isSkipped)
            plusOne(numList);
        QString newId;
        for (int i = numList.size()-1; i >= 0 ; --i)
            newId += charList.at(numList.at(i));

        if (disableDigitId)
            Q_ASSERT(newId.at(0).isDigit() == false);
        elem.setAttribute(AttrId::id, newId);

        pos++;
        isSkipped = false;
    }
}

struct AttrData {
    int count;
    QString value;
};

// TODO: remove 'tspan' without attributes
void Replacer::groupTextElementsStyles() const
{
    IntList importantAttrs;
    importantAttrs << AttrId::x << AttrId::y << AttrId::dx << AttrId::dy << AttrId::rotate
                   << AttrId::textLength << AttrId::lengthAdjust;

    element_loop (svgElement()) {
        if (elem.tagName() != E_text && elem.tagName() != E_textPath)
            continue;
        if (!elem.hasChildren()) {
            // remove 'text' element without children
            elem = elem.parentElement().removeChild(elem, true);
            continue;
        }

        // 'text' with only 'tspan' children is supported
        bool isOnlyTspan = true;
        SvgElement tspan = elem.firstChildElement();
        SvgElement tspan_root = elem;
        while (!tspan.isNull()) {
            if (tspan.tagName() != E_tspan) {
                isOnlyTspan = false;
                break;
            }
            nextElement(tspan, tspan_root);
        }
        if (!isOnlyTspan)
            continue;

        // apply transform
        if (elem.hasTransform() && elem.tagName() == E_text) {
            Transform ts = elem.transform();
            TransformPrivate::Types tsType = ts.type();
            tsType &= ~(TransformPrivate::ProportionalScale);
            if (tsType == TransformPrivate::Translate) {
                double x, y;
                ts.applyTranform(elem.doubleAttribute(AttrId::x),
                                 elem.doubleAttribute(AttrId::y), x, y);
                elem.setAttribute(AttrId::x, fromDouble(x));
                elem.setAttribute(AttrId::y, fromDouble(y));
                elem.removeTransform();

                tspan = elem.firstChildElement();
                tspan_root = elem;
                while (!tspan.isNull()) {
                    double x, y;
                    ts.applyTranform(tspan.doubleAttribute(AttrId::x),
                                     tspan.doubleAttribute(AttrId::y), x, y);
                    tspan.setAttribute(AttrId::x, fromDouble(x));
                    tspan.setAttribute(AttrId::y, fromDouble(y));
                    nextElement(tspan, tspan_root);
                }
            }
        }

        if (elem.hasText())
            continue;

        // if first 'tspan' has the same position as parent 'text'
        // remove it from 'tspan'
        SvgElement firstTspan = elem.firstChildElement();
        if (elem.tagName() == E_text) {
            if (firstTspan.attribute(AttrId::x) == elem.attribute(AttrId::x))
                firstTspan.removeAttribute(AttrId::x);
            if (firstTspan.attribute(AttrId::y) == elem.attribute(AttrId::y))
                firstTspan.removeAttribute(AttrId::y);
        }

        // if 'text' element has only one 'tspan' child - remove this 'tspan'
        // and move text and attributes to parent 'text' element
        if (   elem.childElementCount() == 1
            && firstTspan.tagName() == E_tspan
            && !firstTspan.hasChildrenElement())
        {
            foreach (const uint &attrId, firstTspan.baseAttributesList()) {
                if (attrId != AttrId::id)
                    elem.setAttribute(attrId, firstTspan.attribute(attrId));
            }

            SvgText textElem = document().createText(firstTspan.text());
            elem.appendChild(textElem);
            elem.removeChild(firstTspan);
            if (elem.tagName() == E_text)
                continue;
        }

        int tspanCount = 0;

        // find all uniq styles and calc it's count
        tspan = elem.firstChildElement();
        tspan_root = elem;
        QHash<int,AttrData> attrCountHash;
        while (!tspan.isNull()) {
            tspanCount++;
            foreach (const uint &attrId, tspan.baseAttributesList()) {
                if (importantAttrs.contains(attrId))
                    continue;

                if (   attrCountHash.contains(attrId)
                    && attrCountHash.value(attrId).value == tspan.attribute(attrId))
                {
                    AttrData data = attrCountHash.value(attrId);
                    data.count++;
                    attrCountHash.insert(attrId, data);
                } else {
                    AttrData data;
                    data.count = 1;
                    data.value = tspan.attribute(attrId);
                    attrCountHash.insert(attrId, data);
                }
            }
            nextElement(tspan, tspan_root);
        }

        // if all 'tspan' elements has equal attribute - move it to parent 'text' element
        foreach (const uint &attrId, attrCountHash.keys()) {
            if (attrCountHash.value(attrId).count == tspanCount) {
                tspan = elem.firstChildElement();
                tspan_root = elem;
                elem.setAttribute(attrId, tspan.attribute(attrId));
                while (!tspan.isNull()) {
                    tspan.removeAttribute(attrId);
                    nextElement(tspan, tspan_root);
                }
                attrCountHash.remove(attrId);
            }
        }

        // check is 'text' has only one 'textPath' child
        if (elem.tagName() == E_textPath) {
            if (elem.parentElement().childElementCount() == 1) {
                SvgElement parentElem = elem.parentElement();
                foreach (const uint &attrId, elem.baseAttributesList()) {
                    if (attrId == AttrId::xlink_href)
                        continue;
                    parentElem.setAttribute(attrId, elem.attribute(attrId));
                    elem.removeAttribute(attrId);
                }
            }
        }
    }
}

void Replacer::sortDefs() const
{
    // sort only not used definitions
    SvgElementList list = defsElement().childElements();
    SvgElementList list2;
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() != E_use && !elem.isUsed())
            list2 << elem;
    }
    if (!list2.isEmpty()) {
        qSort(list2.begin(), list2.end(), &Replacer::nodeByTagNameSort);
        for (int i = 0; i < list2.count(); ++i)
            defsElement().appendChild(list2.at(i));
    }
}

bool Replacer::nodeByTagNameSort(const SvgElement &node1, const SvgElement &node2)
{
    return node1.tagName() < node2.tagName();
}

void Replacer::roundNumericAttributes() const
{
    IntList listBasedAttrList;
    listBasedAttrList << AttrId::stdDeviation << AttrId::baseFrequency << AttrId::dx
                      << AttrId::dy << AttrId::stroke_dasharray;
    IntList filterDigitList;
    filterDigitList << AttrId::stdDeviation << AttrId::baseFrequency << AttrId::k << AttrId::k1
                    << AttrId::k2 << AttrId::k3 << AttrId::specularConstant << AttrId::dx
                    << AttrId::dy << AttrId::stroke_dasharray;

    element_loop (svgElement()) {
        foreach (const uint &attrId, filterDigitList) {
            if (elem.hasAttribute(attrId) && elem.attribute(attrId) != V_none) {
                QString value = elem.attribute(attrId);
                // process list based attributes
                if (listBasedAttrList.contains(attrId)) {
                    // TODO: get rid of regex
                    QStringList tmpList = value.split(QRegExp(QL1S("(,|) |,")),
                                                      QString::SkipEmptyParts);
                    QString tmpStr;
                    foreach (const QString &text, tmpList) {
                        bool ok;
                        if (attrId == AttrId::stroke_dasharray)
                            // stroke-dasharray do not support numbers without leading zero
                            tmpStr += QString::number(toDouble(text, &ok)) + QL1C(' ');
                        else
                            tmpStr += fromDouble(toDouble(text, &ok), Round::Transform) + QL1C(' ');
                        if (!ok)
                            qFatal("could not process value: '%s'",
                                   qPrintable(attrIdToStr(attrId) + QL1C('=') + value));
                    }
                    tmpStr.chop(1);
                    elem.setAttribute(attrId, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = fromDouble(toDouble(value, &ok), Round::Transform);
                    if (!ok)
                        qFatal("could not process value: '%s'",
                               qPrintable(attrIdToStr(attrId) + QL1C('=') + value));
                    elem.setAttribute(attrId, attrVal);
                }
            }
        }
        foreach (const uint &attrId, Properties::digitListIds) {
            if (elem.hasAttribute(attrId)) {
                QString value = elem.attribute(attrId);
                if (   !value.contains(LengthType::Percent) && !value.contains(QL1C(' '))
                    && !value.contains(QL1C(',')) && !value.isEmpty()) {
                    bool ok;
                    QString attrVal = fromDouble(toDouble(value, &ok), Round::Attribute);
                    if (!ok)
                        qFatal("could not process value: '%s'",
                               qPrintable(attrIdToStr(attrId) + QL1C('=') + value));
                    elem.setAttribute(attrId, attrVal);
                }
            }
        }
    }
}

#include <QtDebug>
void Replacer::prepareLinkedStyles() const
{
    QHash<QString,SvgElement> defs;
    // must be callen only after all defs is moved to 'defs' group
    // Replacer::prepareDefs()
    element_loop (svgElement())
        defs.insert(elem.id(), elem);

    element_loop (svgElement()) {
        foreach (const uint &attrId, Properties::linkableStyleAttributesIds) {
            if (!elem.hasAttribute(attrId))
                continue;
            if (elem.defIdFromAttribute(attrId).isEmpty())
                continue;
            SvgElement defElem = defs.value(elem.defIdFromAttribute(attrId));
            if (defElem.isNull()) {
                if (Properties::presentationAttributesIds.contains(attrId))
                    elem.setAttribute(attrId, V_none);
                else if (Keys.flag(Key::RemoveUnusedXLinks))
                    elem.removeAttribute(attrId);
            } else {
                elem.setReferenceElement(attrId, defElem);
            }
        }

        if (elem.hasAttribute(AttrId::xlink_href)) {
            QString xlink = elem.attribute(AttrId::xlink_href);
            if (!xlink.startsWith(QL1S("data"))) {
                xlink.remove(0,1);
                SvgElement defElem = defs.value(xlink);
                if (!defElem.isNull()) {
                    elem.setReferenceElement(AttrId::xlink_href, defElem);
                } else if (Keys.flag(Key::RemoveUnusedXLinks)) {
                    elem.removeAttribute(AttrId::xlink_href);
                }
            }
        }
    }

    document().setBlockStringUrls(true);
}

// TODO: try to convert thin rect to line-to path
// view-calendar-list.svg
// TODO: check converting without default attributes

// http://www.w3.org/TR/SVG/shapes.html
void Replacer::convertBasicShapes() const
{
    element_loop (svgElement()) {
        QString ctag = elem.tagName();
        if (!(ctag == E_polygon || ctag == E_polyline || ctag == E_line || ctag == E_rect))
            continue;

        QString dAttrStr;
        PathSegmentList segList;
        if (ctag == E_line) {
            dAttrStr = QString(QL1S("M %1,%2 %3,%4"))
                         .arg(elem.attribute(AttrId::x1, QL1S("0")),
                              elem.attribute(AttrId::y1, QL1S("0")),
                              elem.attribute(AttrId::x2, QL1S("0")),
                              elem.attribute(AttrId::y2, QL1S("0")));
            segList = Path().pathToSegments(dAttrStr);
            elem.removeAttributes(QStringList() << A_x1 << A_y1 << A_x2 << A_y2);
        } else if (ctag == E_rect) {
            if (elem.doubleAttribute(AttrId::rx) == 0 || elem.doubleAttribute(AttrId::ry) == 0) {
                double x = elem.doubleAttribute(AttrId::x);
                double y = elem.doubleAttribute(AttrId::y);
                double x1 = x + elem.doubleAttribute(AttrId::width);
                double y1 = y + elem.doubleAttribute(AttrId::height);
                dAttrStr = QString(QL1S("M %1,%2 H%3 V%4 H%1 z")).arg(x).arg(y).arg(x1).arg(y1);
                segList = Path().pathToSegments(dAttrStr);
                elem.removeAttributes(QStringList() << A_x << A_y << A_width << A_height
                                                    << A_rx << A_ry);
            }
        } else if (ctag == E_polyline || ctag == E_polygon) {
            QString path = elem.attribute(AttrId::points).simplified();
            StringWalker sw(path);
            while (!sw.atEnd()) {
                PathSegment seg;
                seg.command = Command::MoveTo;
                seg.absolute = true;
                seg.srcCmd = segList.isEmpty();
                seg.x = sw.number();
                seg.y = sw.number();
                segList.append(seg);
            }
            if (ctag == E_polygon) {
                PathSegment seg;
                seg.command = Command::ClosePath;
                seg.absolute = false;
                seg.srcCmd = true;
                segList.append(seg);
            }
            dAttrStr = Path().segmentsToPath(segList);
            elem.removeAttribute(AttrId::points);
        }
        if (!dAttrStr.isEmpty()) {
            Q_ASSERT(segList.isEmpty() == false);
            elem.setAttribute(SvgAttribute(segList, dAttrStr));
            elem.setTagName(E_path);
        }
    }
}

void Replacer::splitStyleAttributes() const
{
    element_loop (document().documentElement()) {
        if (elem.tagName().contains(E_feFlood))
            continue;
        if (!elem.hasAttribute(AttrId::style))
            continue;

        splitStyle(elem.attribute(AttrId::style), elem);
        elem.removeAttribute(AttrId::style);
    }
}

void Replacer::joinStyleAttr() const
{
    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        QStringList attrs;
        foreach (const QString &attrName, Properties::presentationAttributes) {
            if (elem.hasAttribute(attrName)) {
                attrs << attrName + QL1S(":") + elem.attribute(attrName);
                elem.removeAttribute(attrName);
            }
        }
        elem.setAttribute(AttrId::style, attrs.join(QL1S(";")));
        nextElement(elem, root);
    }
}

void Replacer::mergeGradients() const
{
    loop_children (defsElement()) {

        if (!elem.hasReference(AttrId::xlink_href))
            continue;

        // Move linearGradient child stop elements to radialGradient or linearGradient
        // which inherits of this linearGradient.
        // Only when inherited linearGradient used only once.
        if (   elem.referencedElement(AttrId::xlink_href).usesCount() == 1
            && (elem.tagName() == E_linearGradient || elem.tagName() == E_radialGradient))
        {
            SvgElement lg = elem.referencedElement(AttrId::xlink_href);
            foreach (const SvgElement &child, lg.childElements())
                elem.appendChild(child);
            elem.setReferenceElement(AttrId::xlink_href, SvgElement());
            smartElementRemove(lg);

            // start from beginning
            elem = defsElement().firstChildElement();
            continue;
        }

        // FIXME: this

        // If linearGradient linked to linearGradient, which has used by one or more elements
        // and all of this elements are linearGradient's -
        // than we can merge main linearGradient element with first element that used it.
        if (elem.tagName() == E_linearGradient && !elem.hasChildren())
        {
            SvgElement lg = elem.referencedElement(AttrId::xlink_href);
            SvgElementList usedElemList = lg.linkedElements();

            if (usedElemList.isEmpty())
                continue;

            bool isValid = true;
            foreach (const SvgElement &usedElem, usedElemList) {
                if (usedElem.tagName() != E_linearGradient) {
                    isValid = false;
                    break;
                }
            }
            if (!isValid)
                continue;

            foreach (SvgElement usedElem, usedElemList) {
                if (usedElem != lg)
                    usedElem.setReferenceElement(AttrId::xlink_href, elem);
            }
            foreach (const SvgElement &child, lg.childElements())
                elem.appendChild(child);
            elem.removeReferenceElement(AttrId::xlink_href);

            smartElementRemove(lg);

            // start from beginning
            elem = defsElement().firstChildElement();
            continue;
        }
    }
}

/*
 * This func replace several equal gradients with one
 *
 * Before:
 * <linearGradient id="linearGradient001">
 *   <stop offset=V_null stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 * <linearGradient id="linearGradient002">
 *   <stop offset=V_null stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 *
 * After:
 * <linearGradient id="linearGradient001">
 *   <stop offset=V_null stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 * <linearGradient id="linearGradient002" xlink:href="#linearGradient001">
 *
 */

void Replacer::mergeGradientsWithEqualStopElem() const
{
    SvgElement elem1 = defsElement().firstChildElement();
    while (!elem1.isNull()) {
        if (!(elem1.tagName() == E_linearGradient || elem1.tagName() == E_radialGradient)
            || !elem1.hasChildrenElement())
        {
            elem1 = elem1.nextSiblingElement();
            continue;
        }

        SvgElement elem2 = elem1.nextSiblingElement();
        while (!elem2.isNull()) {
            if (!(elem2.tagName() == E_linearGradient || elem2.tagName() == E_radialGradient)
                || !elem2.hasChildrenElement())
            {
                elem2 = elem2.nextSiblingElement();
                continue;
            }

            if (isGradientStopsEqual(elem1, elem2)) {
                elem2.setReferenceElement(AttrId::xlink_href, elem1);
                foreach (const SvgElement &stopElem, elem2.childElements())
                    elem2.removeChild(stopElem);
            }

            elem2 = elem2.nextSiblingElement();
        }

        elem1 = elem1.nextSiblingElement();
    }
}

void Replacer::calcElementsBoundingBox() const
{
    element_loop (svgElement()) {
        // cannot calculate bbox for element with transform
        if (elem.hasTransform())
            continue;

        if (elem.tagName() == E_rect) {
            QString str = elem.attribute(AttrId::x)
                            + QL1C(' ') + elem.attribute(AttrId::y)
                            + QL1C(' ') + elem.attribute(AttrId::width)
                            + QL1C(' ') + elem.attribute(AttrId::height);
            elem.setAttribute(AttrId::bbox, str);
        } else if (elem.tagName() == E_circle) {
            double r = elem.doubleAttribute(AttrId::r);
            double x = elem.doubleAttribute(AttrId::cx) - r;
            double y = elem.doubleAttribute(AttrId::cy) - r;
            QString str = fromDouble(x)
                            + QL1C(' ') + fromDouble(y)
                            + QL1C(' ') + fromDouble(qAbs(r*2))
                            + QL1C(' ') + fromDouble(qAbs(r*2));
            elem.setAttribute(AttrId::bbox, str);
        } else if (elem.tagName() == E_ellipse) {
            double rx = elem.doubleAttribute(AttrId::rx);
            double ry = elem.doubleAttribute(AttrId::ry);
            double x = elem.doubleAttribute(AttrId::cx) - rx;
            double y = elem.doubleAttribute(AttrId::cy) - ry;
            QString str = fromDouble(x)
                            + QL1C(' ') + fromDouble(y)
                            + QL1C(' ') + fromDouble(qAbs(rx*2))
                            + QL1C(' ') + fromDouble(qAbs(ry*2));
            elem.setAttribute(AttrId::bbox, str);
        }
        // all other basic shapes bounding boxes are calculated in Paths class
    }
}

/*
 * Tries to group elements with equal style properties.
 * After grouping we can remove these styles from original element, and thus simplify our svg.
 *
 * For example:
 *
 * (before)
 * <path style="fill:#fff" d="..."/>
 * <path style="fill:#fff" d="..."/>
 * <path style="fill:#fff" d="..."/>
 *
 * (after)
 * <g style="fill:#fff">
 *   <path d="..."/>
 *   <path d="..."/>
 *   <path d="..."/>
 * </g>
 */

void Replacer::groupElementsByStyles() const
{
    _groupElementsByStyles();
}

// TODO: partial transform attr group
//       demo.svg, applications-development.svg
// TODO: group non successively used attributes
//       Anonymous_City_flag_of_Gijon_Asturies_Spain.svg
void Replacer::_groupElementsByStyles(SvgElement parentElem) const
{
    // first start
    if (parentElem.isNull())
        parentElem = svgElement();

    SvgAttributeHash groupHash;
    SvgElementList similarElemList;
    IntList additionalAttrList;
    additionalAttrList << AttrId::text_align << AttrId::line_height << AttrId::font;
    IntList ignoreAttrList;
    ignoreAttrList << AttrId::clip_path << AttrId::mask << AttrId::filter << AttrId::opacity;
    SvgElementList list = parentElem.childElements();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.isGroup() || currElem.tagName() == E_flowRoot)
            _groupElementsByStyles(currElem);

        if (groupHash.isEmpty()) {
            // get hash of all style attributes of element
            groupHash = currElem.styleHash();
            foreach (const uint &attrId, additionalAttrList) {
                if (currElem.hasAttribute(attrId))
                    groupHash.insert(attrId, currElem.attributeItem(attrId));
            }
            if (    currElem.hasTransform()
                && !currElem.hasReferencedDefs()
                && !parentElem.hasReferencedDefs())
            {
                groupHash.insert(AttrId::transform, currElem.attributeItem(AttrId::transform));
            }

            // we can not group elements by some attributes
            foreach (const uint &attrId, ignoreAttrList) {
                if (groupHash.contains(attrId))
                    groupHash.remove(attrId);
            }
            if (!groupHash.isEmpty())
                similarElemList << currElem;
            // elem linked to 'use' have to store style properties only in elem, not in group
            if (currElem.isUsed()) {
                groupHash.clear();
                similarElemList.clear();
            }
        } else {
            if (currElem.isUsed())
                groupHash.clear();
            SvgAttributeHash lastGroupHash = groupHash;
            // remove attributes which do not exist or are different in next element
            foreach (const uint &attrId, groupHash.keys()) {
                if (!currElem.hasAttribute(attrId))
                    groupHash.remove(attrId);
                else {
                    SvgAttribute currAttr = currElem.attributeItem(attrId);
                    SvgAttribute gAttr = groupHash.value(attrId);
                    if (attrId == AttrId::transform) {
                        if (currAttr.transform() != gAttr.transform())
                            groupHash.remove(attrId);
                    } else if (currAttr.value() != gAttr.value()) {
                        groupHash.remove(attrId);
                    }

                }
            }

            // if hash of style attrs is empty - then current element is has completely different
            // style attributes and so we can not group it
            if (!groupHash.isEmpty())
                similarElemList << currElem;
            if (list.isEmpty() || groupHash.isEmpty()) {
                if (list.isEmpty())
                    lastGroupHash = groupHash;
                if (similarElemList.size() > 1) {
                    // find or create parent group

                    bool isValidFlowRoot = false;
                    if (parentElem.tagName() == E_flowRoot) {
                        int flowParaCount = 0;
                        foreach (SvgElement childElem, parentElem.childElements()) {
                            if (childElem.tagName() == E_flowPara)
                                flowParaCount++;
                        }
                        if (flowParaCount == similarElemList.size())
                            isValidFlowRoot = true;
                    }

                    bool canUseParent = false;
                    if (isValidFlowRoot)
                        canUseParent = true;
                    else if (parentElem.childElementCount() == similarElemList.size()) {
                        if (parentElem.isGroup())
                            canUseParent = true;
                        else if (parentElem.tagName() == E_svg
                                 && !lastGroupHash.contains(AttrId::transform))
                            canUseParent = true;
                    }

                    SvgElement parentGElem;
                    if (canUseParent) {
                        parentGElem = parentElem;
                    } else {
                        parentGElem = document().createElement(E_g);
                        parentGElem = parentElem.insertBefore(parentGElem,
                                                              similarElemList.first()).toElement();
                    }
                    // move equal style attributes of selected elements to parent group
                    foreach (const uint &attrId, lastGroupHash.keys()) {
                        if (parentGElem.hasAttribute(attrId) && attrId == AttrId::transform) {
                            Transform ts = lastGroupHash.value(attrId).transform();
                            parentGElem.setTransform(ts);
                        } else {
                            parentGElem.setAttribute(lastGroupHash.value(attrId));
                        }
                    }
                    // move elem to group
                    foreach (SvgElement similarElem, similarElemList) {
                        foreach (const uint &attrId, lastGroupHash.keys())
                            similarElem.removeAttribute(attrId);
                        // if we remove attr from group and now it does not have any
                        // important attributes - we can ungroup it
                        if (similarElem.isGroup() && !similarElem.hasImportantAttrs()) {
                            foreach (SvgElement gChildElem, similarElem.childElements())
                                parentGElem.appendChild(gChildElem);
                            smartElementRemove(similarElem);
                        } else {
                            parentGElem.appendChild(similarElem);
                        }
                    }
                    _groupElementsByStyles(parentGElem);
                }
                similarElemList.clear();
                if (!currElem.tagName().isEmpty() && !currElem.isGroup())
                    list.prepend(currElem);
            }
        }
    }
}

void Replacer::applyTransformToDefs() const
{
    // call only before Remover::removeUnusedDefsAttributes(),
    // because it can broke transform applying to attributes which was removed

    SvgElementList list = defsElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == E_linearGradient) {
            if (elem.hasTransform()) {
                Transform gts = elem.transform();
                if (gts.isProportionalScale()) {
                    double x, y;
                    gts.applyTranform(elem.doubleAttribute(AttrId::x1),
                                      elem.doubleAttribute(AttrId::y1), x, y);
                    elem.setAttribute(AttrId::x1, fromDouble(x));
                    elem.setAttribute(AttrId::y1, fromDouble(y));
                    gts.applyTranform(elem.doubleAttribute(AttrId::x2),
                                      elem.doubleAttribute(AttrId::y2), x, y);
                    elem.setAttribute(AttrId::x2, fromDouble(x));
                    elem.setAttribute(AttrId::y2, fromDouble(y));
                    elem.removeTransform();
                }
            }
        } else if (elem.tagName() == E_radialGradient) {
            if (elem.hasTransform()) {
                Transform gts = elem.transform();
                if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                    double x, y;
                    gts.applyTranform(elem.doubleAttribute(AttrId::fx),
                                      elem.doubleAttribute(AttrId::fy), x, y);
                    if (elem.hasAttribute(AttrId::fx))
                        elem.setAttribute(AttrId::fx, fromDouble(x));
                    if (elem.hasAttribute(AttrId::fy))
                        elem.setAttribute(AttrId::fy, fromDouble(y));
                    gts.applyTranform(elem.doubleAttribute(AttrId::cx),
                                      elem.doubleAttribute(AttrId::cy), x, y);
                    elem.setAttribute(AttrId::cx, fromDouble(x));
                    elem.setAttribute(AttrId::cy, fromDouble(y));

                    elem.setAttribute(AttrId::r, fromDouble(elem.doubleAttribute(AttrId::r)
                                                            * gts.scaleFactor()));
                    elem.removeTransform();
                }
            }
        }
        // TODO: should it be recursive?
        if (elem.hasChildrenElement())
            list << elem.childElements();
    }
}

void Replacer::applyTransformToShapes() const
{
    // TODO: add another shapes

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        SvgElement firstChild = elem.firstChildElement();

        if (   elem.isGroup()
            && elem.hasTransform()
            && !elem.hasReference(AttrId::clip_path)
            && !elem.hasReference(AttrId::mask)
            && !elem.hasReference(AttrId::filter)
            && elem.childElementCount() == 1
            && !elem.isUsed()
            && firstChild.tagName() != E_use
            && !firstChild.isGroup()
            && !firstChild.isUsed()
            && !firstChild.hasReference(AttrId::clip_path))
        {
            firstChild.setTransform(elem.transform(), true);
            elem.removeTransform();
        }
        else if (   elem.tagName() == E_rect
                 && elem.hasTransform()
                 && !elem.hasReference(AttrId::clip_path)
                 && !elem.hasReference(AttrId::mask)
                 && elem.referencedElement(AttrId::filter).usesCount() < 2
                 && elem.referencedElement(AttrId::stroke).usesCount() < 2
                 && elem.referencedElement(AttrId::fill).usesCount() < 2)
        {
            bool canApplyTransform = true;
            if (elem.hasReference(AttrId::fill)) {
                SvgElement fillDef = elem.referencedElement(AttrId::fill);
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (elem.hasReference(AttrId::stroke)) {
                SvgElement fillDef = elem.referencedElement(AttrId::stroke);
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (canApplyTransform) {
                Transform ts = elem.transform();
                if (   !ts.isMirrored()
                    && !ts.isRotating()
                    && !ts.isSkew()
                    &&  ts.isProportionalScale())
                {
                    double x, y;
                    ts.applyTranform(elem.doubleAttribute(AttrId::x),
                                     elem.doubleAttribute(AttrId::y), x, y);
                    elem.setAttribute(AttrId::x, fromDouble(x));
                    elem.setAttribute(AttrId::y, fromDouble(y));
                    const double sf = ts.scaleFactor();
                    QString newW = fromDouble(elem.doubleAttribute(AttrId::width) * sf);
                    elem.setAttribute(AttrId::width, newW);
                    QString newH = fromDouble(elem.doubleAttribute(AttrId::height) * sf);
                    elem.setAttribute(AttrId::height, newH);
                    QString newRx = fromDouble(elem.doubleAttribute(AttrId::rx) * sf);
                    elem.setAttribute(AttrId::rx, newRx);
                    QString newRy = fromDouble(elem.doubleAttribute(AttrId::ry) * sf);
                    elem.setAttribute(AttrId::ry, newRy);
                    updateLinkedDefTransform(elem);
                    calcNewStrokeWidth(elem, sf);
                    elem.removeTransform();
                }
            }
        }

        if (elem.hasChildrenElement())
            list << elem.childElements();
    }
}

// TODO: merge with Paths::calcNewStrokeWidth
void Replacer::calcNewStrokeWidth(SvgElement &elem, double scaleFactor)
{
    SvgElement parentElem = elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(AttrId::stroke_width)) {
            double strokeWidth = convertUnitsToPx(parentElem.attribute(AttrId::stroke_width))
                                    .toDouble();
            QString sw = fromDouble(strokeWidth * scaleFactor, Round::Attribute);
            elem.setAttribute(AttrId::stroke_width, sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentElement();
    }
    if (!hasParentStrokeWidth) {
        elem.setAttribute(AttrId::stroke_width, fromDouble(scaleFactor, Round::Attribute));
    }
}
