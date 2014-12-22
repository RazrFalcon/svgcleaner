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

#include "paths.h"
#include "replacer.h"

// TODO: replace equal 'fill', 'stroke', 'stop-color', 'flood-color' and 'lighting-color' attr
//       with 'color' attr
//       addon_the_couch.svg
// TODO: If 'x1' = 'x2' and 'y1' = 'y2', then the area to be painted will be painted as
//       a single color using the color and opacity of the last gradient stop.
// TODO: try to recalculate 'userSpaceOnUse' to 'objectBoundingBox'
// TODO: maybe move frequently used styles to CDATA

void Replacer::convertSizeToViewbox()
{
    if (!svgElement().hasAttribute(AttrId::viewBox)) {
        if (svgElement().hasAttribute(AttrId::width) && svgElement().hasAttribute(AttrId::height)) {
            QString width  = fromDouble(svgElement().doubleAttribute(AttrId::width));
            QString height = fromDouble(svgElement().doubleAttribute(AttrId::height));
            svgElement().setAttribute(AttrId::viewBox, QString("0 0 %1 %2").arg(width).arg(height));
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
void Replacer::processPaths()
{
    bool skip = true;
    foreach (const int &id, Keys.pathsKeysId()) {
        if (Keys.flag(id)) {
            skip = false;
            break;
        }
    }
    if (skip)
        return;

    QHash<QString,int> defsHash;
    if (Keys.flag(Key::ApplyTransformsToPaths))
        defsHash = calcDefsUsageCount();

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        bool removed = false;
        if (elem.tagName() == E_path) {
            bool canApplyTransform = false;
            if (Keys.flag(Key::ApplyTransformsToPaths))
                canApplyTransform = isPathValidToTransform(elem, defsHash);
            bool isPathApplyed = false;
            Path().processPath(elem, canApplyTransform, &isPathApplyed);
            if (canApplyTransform) {
                if (isPathApplyed) {
                    updateLinkedDefTransform(elem);
                    elem.removeTransform();
                }
            }
            if (elem.attribute(AttrId::d).isEmpty()) {
                smartElementRemove(elem);
                removed = true;
            }
        }

        if (!removed) {
            if (elem.hasChildrenElement())
                list << elem.childElements();
        }
    }
}

bool Replacer::isPathValidToTransform(SvgElement &pathElem, QHash<QString,int> &defsIdHash)
{
    if (pathElem.hasTransform()) {
        // non proportional transform could not be applied to path with stroke
        bool hasStroke = false;
        SvgElement parentElem = pathElem;
        while (!parentElem.isNull()) {
            if (parentElem.hasAttribute(AttrId::stroke)) {
                if (parentElem.attribute(AttrId::stroke) != V_none) {
                    hasStroke = true;
                    break;
                }
            }
            parentElem = parentElem.parentElement();
        }
        if (hasStroke) {
            if (pathElem.transform().isProportionalScale())
                return false;
        }
    } else {
        return false;
    }
    if (pathElem.hasAttribute(AttrId::clip_path) || pathElem.hasAttribute(AttrId::mask))
        return false;
    if (pathElem.isUsed())
        return false;
    if (pathElem.hasAttribute(AttrId::filter)) {
        // we can apply transform to blur filter, but only when it's used by only this path
        QString filterId = pathElem.defIdFromAttribute(AttrId::filter);
        if (defsIdHash.value(filterId) > 1)
            return false;
        if (!isBlurFilter(filterId))
            return false;
    }

    IntList attrList;
    attrList << AttrId::fill << AttrId::stroke;
    foreach (const int &attrId, attrList) {
        if (pathElem.hasAttribute(attrId)) {
            QString defId = pathElem.defIdFromAttribute(attrId);
            if (!defId.isEmpty()) {
                if (defsIdHash.value(defId) > 1)
                    return false;
            }
            if (!defId.isEmpty()) {
                SvgElement defElem = elemFromDefs(defId);
                if (!defElem.isNull()) {
                    if (   defElem.tagName() != E_linearGradient
                        && defElem.tagName() != E_radialGradient)
                        return false;
                    if (defElem.attribute(AttrId::gradientUnits) != "userSpaceOnUse")
                        return false;
                }
            }
        }
    }
    return true;
}

bool Replacer::isBlurFilter(const QString &id)
{
    static const QStringList filterAttrs = QStringList()
        << A_x << A_y << A_width << A_height;
    element_loop(defsElement().firstChildElement()) {
        if (elem.tagName() == E_filter) {
            if (elem.id() == id) {
                if (elem.childElementCount() == 1) {
                    if (elem.firstChildElement().tagName() == E_feGaussianBlur) {
                        // cannot apply transform to filter with not default region
                        if (!elem.hasAttributes(filterAttrs))
                            return true;
                    }
                }
            }
        }
        elem = elem.nextSiblingElement();
    }
    return false;
}

void Replacer::updateLinkedDefTransform(SvgElement &elem)
{
    IntList attrList;
    attrList << AttrId::fill << AttrId::stroke << AttrId::filter;
    foreach (const int &attrId, attrList) {
        QString defId = elem.defIdFromAttribute(attrId);
        if (!defId.isEmpty()) {
            SvgElement defElem = elemFromDefs(defId);
            if (!defElem.isNull()) {
                if ((  defElem.tagName() == E_linearGradient
                    || defElem.tagName() == E_radialGradient)
                       && defElem.attribute(AttrId::gradientUnits) == "userSpaceOnUse")
                {
                    Transform gts = defElem.transform();
                    Transform ts = elem.transform();
                    if (!gts.isNull()) {
                        ts.append(gts);
                    }
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
    }
}

class EqElement {
public:
    QString tagName;
    IntHash attrHash;
    SvgElement elem;
    bool operator ==(const EqElement &elem) const {
        return elem.elem == this->elem;
    }
};

// FIXME: parent styles should be set to elem before moving to defs
//        address-book-new.svg
// TODO: reuse groups
void Replacer::replaceEqualElementsByUse()
{
    static const IntList rectAttrs = IntList()
        << AttrId::x << AttrId::y << AttrId::width << AttrId::height << AttrId::rx << AttrId::ry;
    static const IntList circleAttrs = IntList() << AttrId::cx << AttrId::cy << AttrId::r;
    static const IntList ellipseAttrs = IntList()
        << AttrId::cx << AttrId::cy << AttrId::rx << AttrId::ry;
    SvgElementList list = svgElement().childElements();
    int newAttrId = 0;

    QList<EqElement> elemList;
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.hasChildrenElement())
            list << elem.childElements();

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
        e.tagName = elem.tagName();
        IntHash hash;
        if (e.tagName == E_path) {
            hash.insert(AttrId::d, elem.attribute(AttrId::d));
        } else if (e.tagName == E_rect) {
            foreach (const int &attrId, rectAttrs)
                hash.insert(attrId, elem.attribute(attrId));
        } else if (e.tagName == E_circle) {
            foreach (const int &attrId, circleAttrs)
               hash.insert(attrId, elem.attribute(attrId));
        } else if (e.tagName == E_ellipse) {
            foreach (const int &attrId, ellipseAttrs)
                hash.insert(attrId, elem.attribute(attrId));
        }
        e.attrHash = hash;
        e.elem = elem;
        elemList << e;
    }

    while (!elemList.isEmpty()) {
        EqElement mainEqElem = elemList.takeFirst();
        QList<EqElement> equalElems;

        for (int j = 0; j < elemList.size(); ++j) {
            if (   mainEqElem.tagName == elemList.at(j).tagName
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
        if (mainEqElem.tagName == E_path) {
            SvgElementList dlist = defsElement().childElements();
            while (!dlist.isEmpty()) {
                SvgElement dElem = dlist.takeFirst();
                // TODO: why 'clipPath' check is needed
                if (   dElem.tagName() == E_path
                    && dElem.parentElement().tagName() != E_clipPath
                    && dElem != mainEqElem.elem)
                {
                    if (dElem.attribute(AttrId::d) == mainEqElem.elem.attribute(AttrId::d)) {
                        if (dElem.styleHash().isEmpty()) {
                            newElem = dElem;
                            break;
                        }
                    }
                }
                if (dElem.hasChildren())
                    dlist << dElem.childElements();
            }
        }

        // if not found - create new one
        if (newElem.isNull()) {
            newElem = document().createElement(mainEqElem.tagName);
            foreach (const int &attrId, mainEqElem.attrHash.keys())
                newElem.setAttribute(attrId, mainEqElem.attrHash.value(attrId));
            defsElement().appendChild(newElem);
        }

        // gen new id
        if (!newElem.hasAttribute(AttrId::id))
            newElem.setAttribute(AttrId::id, getFreeId(newAttrId++));

        newElem.setAttribute(AttrId::used_element, "1");

        // convert elements to 'use'
        equalElems << mainEqElem;
        foreach (EqElement eqElem, equalElems) {
            if (eqElem.elem == newElem)
                continue;

            SvgElement elem = eqElem.elem;
            elem.setTagName(E_use);
            elem.setAttribute(AttrId::xlink_href, QL1C('#') + newElem.id());
            elem.removeAttribute(AttrId::bbox);
            foreach (const int &attrId, mainEqElem.attrHash.keys())
                elem.removeAttribute(attrId);
            elemList.removeOne(eqElem);
        }
    }
}

// TODO: process use element which is used too
// webmichl_wristwatch_1_-_chronometer.svg
void Replacer::moveStyleFromUsedElemToUse()
{
    IntList useAttrs = IntList() << AttrId::x << AttrId::y << AttrId::width << AttrId::height;

    QHash<QString,SvgElement> elemXLink;
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == E_use)
            elemXLink.insertMulti(elem.xlinkId(), elem);
        if (elem.hasChildrenElement())
            list << elem.childElements();
    }

    list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.hasChildrenElement())
            list << elem.childElements();

        if (!elem.isUsed())
            continue;

        SvgElementList usedElemList = elemXLink.values(elem.id());
        // use elem cannot overwrite style properties of used element
        foreach (SvgElement usedElem, usedElemList) {
            foreach (const int &attrId, elem.styleAttributesList()) {
                usedElem.removeAttribute(attrId);
            }
        }

        if (!hasParent(elem, E_defs))
            continue;

        QHash<QString, int> attrsCount;
        foreach (const SvgElement &usedElem, usedElemList) {
            IntList usedElemAttrs = usedElem.styleAttributesList();
            if (usedElem.hasTransform())
                usedElemAttrs << AttrId::transform;
            foreach (const int &attrId, usedElemAttrs) {
                if (!usedElem.hasAttribute(attrId))
                    continue;

                const bool isCount = !(attrId == AttrId::transform
                                       || (   usedElem.hasAttributes(useAttrs)
                                           /*|| usedElem.hasLinkedDef()*/));
                if (isCount) {
                    QString attr = attrIdToStr(attrId) + ":" + usedElem.attribute(attrId);
                    if (attrsCount.contains(attr))
                        attrsCount.insert(attr, attrsCount.value(attr) + 1);
                    else
                        attrsCount.insert(attr, 1);
                }
            }
        }
        foreach (const QString &attr, attrsCount.keys()) {
            if (attrsCount.value(attr) != usedElemList.count())
                continue;
            // do not replace transform when used elem has already has transform
            if (attr.startsWith(A_transform)
                && (elem.hasTransform() || attrsCount.value(attr) == 1))
                continue;

            QString attrName = attr;
            attrName.remove(attrName.indexOf(':'), attrName.size());
            QString attrValue = attr;
            attrValue.remove(0, attrValue.indexOf(':') + 1);
            foreach (SvgElement usedElem, usedElemList)
                usedElem.removeAttribute(attrName);
            elem.setAttribute(attrName, attrValue);
        }
    }
}

void Replacer::convertUnits()
{
    QRectF rect = viewBoxRect();
    // TODO: merge code
    if (svgElement().hasAttribute(AttrId::width)) {
        QString widthStr = svgElement().attribute(AttrId::width);
        if (widthStr.contains(LengthType::Percent) && rect.isNull())
            qFatal("Error: could not convert width in percentage into px without viewBox");
        bool ok;
        double width = Tools::convertUnitsToPx(widthStr, rect.width()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert width to px");
        svgElement().setAttribute(AttrId::width, fromDouble(width));
    }
    if (svgElement().hasAttribute(AttrId::height)) {
        QString heightStr = svgElement().attribute(AttrId::height);
        if (heightStr.contains(LengthType::Percent) && rect.isNull())
            qFatal("Error: could not convert height in percentage into px without viewBox");
        bool ok;
        double height = Tools::convertUnitsToPx(heightStr, rect.height()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert height to px");
        svgElement().setAttribute(AttrId::height, fromDouble(height));
    }

    // TODO: process 'offset' attr with %
    element_loop(document().documentElement()) {
        QString currTag = elem.tagName();
        if (elem.hasAttribute(AttrId::font_size)) {
            bool ok = false;
            QString fontSizeStr = elem.attribute(AttrId::font_size);
            fontSizeStr.toDouble(&ok);
            if (!ok) {
                if (   fontSizeStr.endsWith(LengthType::ex)
                    || fontSizeStr.endsWith(LengthType::em)
                    || fontSizeStr.endsWith(LengthType::Percent))
                {
                    QString parentFontSize = findAttribute(elem.parentElement(), AttrId::font_size);
                    if (parentFontSize.isEmpty() || parentFontSize == V_null)
                        qFatal("Error: could not calculate relative font-size");
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr,
                                                                  toDouble(parentFontSize));
                    if (newFontSize == V_null)
                        elem.removeAttribute(AttrId::font_size);
                    else
                        elem.setAttribute(AttrId::font_size, newFontSize);
                } else {
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr);
                    elem.setAttribute(AttrId::font_size, newFontSize);
                }
            }
        }
        foreach (const int &attrId, Properties::digitListIds) {
            if (!elem.hasAttribute(attrId))
                continue;
            // fix attributes like:
            // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
            // FIXME: ignores list based attr
            QString attrValue = elem.attribute(attrId);
            if (attrValue.contains(" "))
                attrValue = attrValue.left(attrValue.indexOf(" "));

            // TODO: process gradients attrs
            if (attrValue.contains(LengthType::Percent)) {
                if (currTag != E_radialGradient && currTag != E_linearGradient) {
                    QString attrName = attrIdToStr(attrId);
                    if (attrName.contains(A_x) || attrId == AttrId::width)
                        attrValue = Tools::convertUnitsToPx(attrValue, rect.width());
                    else if (attrName.contains(A_y) || attrId == AttrId::height)
                       attrValue = Tools::convertUnitsToPx(attrValue, rect.height());
                }
            } else if (attrValue.endsWith(LengthType::ex) || attrValue.endsWith(LengthType::em)) {
                double fontSize = toDouble(findAttribute(elem, AttrId::font_size));
                if (fontSize == 0)
                    qFatal("Error: could not convert em/ex values "
                           "without font-size attribute is set");
                attrValue = Tools::convertUnitsToPx(attrValue, fontSize);
            } else {
                attrValue = Tools::convertUnitsToPx(attrValue);
            }
            elem.setAttribute(attrId, attrValue);
        }

        nextElement(elem, root);
    }
}

void Replacer::convertColors()
{
    static const bool isConvertColors
            = (Keys.flag(Key::ConvertColorToRRGGBB) || Keys.flag(Key::ConvertRRGGBBToRGB));
    if (!isConvertColors)
        return;

    static const IntList colorAttrs = IntList() << AttrId::fill << AttrId::stroke << AttrId::color
                                                << AttrId::stop_color << AttrId::flood_color;

    element_loop(document().documentElement()) {
        foreach (const int &attrId, colorAttrs) {
            if (!elem.hasAttribute(attrId))
                continue;

            QString value = elem.attribute(attrId);
            if (!value.isEmpty() && value != V_none && !value.startsWith(UrlPrefix))
                elem.setAttribute(attrId, Tools::trimColor(value));
        }
        nextElement(elem, root);
    }
}

IntHash splitStyle(const QString &style)
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
        if (sw.current() == '/') {
            sw.next(2); // skip /*
            sw.jumpTo(asteriskChar);
            sw.next(2); // skip */
            sw.skipSpaces();
        }

        QString name = sw.readBefore(sw.jumpTo(colonChar));
        sw.next(); // skip ':'

        // do not process styles which are linked to data in ENTITY
        // like: '&st0;'
        if (!name.isEmpty() && name.at(0) == andChar)
            break;

        sw.skipSpaces();

        QString value = sw.readBefore(sw.jumpTo(semicolonChar));
        if (!sw.atEnd())
            sw.next(); // skip ';'
        sw.skipSpaces();

        // add to hash
        if (!name.isEmpty() && name.at(0) != QL1C('-')) {
            int attrId = attrStrToId(name);
            if (attrId != -1)
                hash.insert(attrStrToId(name), value);
        }
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
void Replacer::convertEntityData()
{
    SvgNodeList nodeList = document().childNodes();
    QString text;
    while (!nodeList.isEmpty()) {
        SvgNode node = nodeList.takeFirst();
        if (node.isText()) {
            QString dtdText = node.toText().text();
            if (!dtdText.startsWith("<!DOCTYPE"))
                break;
            text = dtdText;
            break;
        }
    }
    if (text.isEmpty() || !text.contains('['))
        return;

    StringHash entityHash;

    StringWalker sw(text);
    sw.jumpTo('[');
    sw.next();
    sw.skipSpaces();
    while (sw.isValid() && !sw.atEnd()) {
        if (sw.current() != '<')
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
        if (sw.current() != '>')
            break;
        sw.next();
        sw.skipSpaces();

        entityHash.insert("&" + name + ";", data);
    }

    if (entityHash.isEmpty())
        return;

    // replace styles
    element_loop(svgElement()) {
        if (elem.hasAttribute(AttrId::style)) {
            QString value = elem.attribute(AttrId::style);
            foreach (const QString &name, entityHash.keys()) {
                if (value.contains(name)) {
                    value.replace(name, entityHash.value(name));
                }
            }
            elem.setAttribute(AttrId::style, value);
        }
        nextElement(elem, root);
    }

    // replace links
    foreach (const QString &attrName, svgElement().extAttributesList()) {
        if (attrName.startsWith("xmlns")) {
            QString value = svgElement().attribute(attrName);
            if (entityHash.keys().contains(value))
                svgElement().setAttribute(attrName, entityHash.value(value));
        }
    }
}

// TODO: rewrite parsing (atlas.svg)
void Replacer::convertCDATAStyle()
{
    QStringList styleList;
    element_loop(document().documentElement()) {
        if (elem.tagName() == E_style) {
            styleList << elem.text();
            elem = elem.parentElement().removeChild(elem, true);
        }
        nextElement(elem, root);
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
        text.replace("\n", " ").replace("\t", " ");
        if (text.isEmpty())
            continue;
        // remove comments
        // better to use positive lookbehind, but qt4 didn't support it
        text.remove(QRegExp("[^\\*]\\/(?!\\*)"));
        text.remove(QRegExp("[^\\/]\\*(?!\\/)"));
        text.remove(QRegExp("\\/\\*[^\\/\\*]*\\*\\/"));
        QStringList classList = text.split(QRegExp(" +(\\.|@)"), QString::SkipEmptyParts);
        foreach (const QString &currClass, classList) {
            QStringList tmpList = currClass.split(QRegExp("( +|)\\{"));
            if (tmpList.size() == 2) {
                classHash.insert(tmpList.at(0).trimmed(),
                                 QString(tmpList.at(1)).remove(QRegExp("\\}.*")));
            }
        }
    }

    element_loop_next(document().documentElement()) {
        if (!elem.hasAttribute(AttrId::class_)) {
            nextElement(elem, root);
            continue;
        }

        IntHash newHash;
        QStringList classList = elem.attribute(AttrId::class_).split(" ", QString::SkipEmptyParts);
        foreach (const QString &classStr, classList) {
            if (classHash.contains(classStr)) {
                IntHash tempHash = splitStyle(classHash.value(classStr));
                foreach (const int &key, tempHash.keys())
                    newHash.insert(key, tempHash.value(key));
            }
        }
        IntHash oldHash = elem.styleHash();
        foreach (const int &attrId, oldHash.keys())
            newHash.insert(attrId, oldHash.value(attrId));
        elem.setStylesFromHash(newHash);
        elem.removeAttribute(AttrId::class_);

        nextElement(elem, root);
    }
}

void Replacer::prepareDefs()
{
    StringSet defsList;
    defsList << E_altGlyphDef << E_clipPath << E_cursor << E_filter << E_linearGradient
             << E_marker << E_mask << E_pattern << E_radialGradient /* << E_symbol*/;

    // move all gradient, filters, etc. to 'defs' element
    element_loop(svgElement()) {
        if (elem.parentElement() != defsElement()) {
            if (defsList.contains(elem.tagName())) {
                SvgElement tElem = elem;
                nextElement(elem, root, true);
                defsElement().appendChild(tElem);
                continue;
            }
        }
        nextElement(elem, root);
    }

    // ungroup all defs in defs
    element_loop_next(svgElement()) {
        if (   elem.parentElement().tagName() == E_defs
            && elem.parentElement() != defsElement())
        {
            SvgElement tElem = elem;
            nextElement(elem, root, true);
            defsElement().appendChild(tElem);
        } else {
            nextElement(elem, root);
        }
    }

    // remove empty defs
    element_loop_next(svgElement()) {
        if (    elem.tagName() == E_defs
            && !elem.hasChildren()
            &&  elem != defsElement())
        {
            SvgElement tElem = elem;
            nextElement(elem, root, true);
            elem.parentElement().removeChild(tElem);
        } else {
            nextElement(elem, root);
        }
    }

    // move 'defs' element to first place in 'svg' element
    svgElement().insertBefore(defsElement(), svgElement().firstChildElement());
}

void Replacer::fixWrongAttr()
{
    IntList tmpList = IntList() << AttrId::fill << AttrId::stroke;
    element_loop(svgElement()) {
        QString currTag = elem.tagName();

        // remove single quotes from 'font-family' value
        if (elem.hasAttribute(AttrId::font_family)) {
            if (elem.attribute(AttrId::font_family).contains('\'')) {
                elem.setAttribute(AttrId::font_family,
                                  elem.attribute(AttrId::font_family).remove('\''));
            }
        }

        // remove wrong fill and stroke attributes like:
        // fill="url(#radialGradient001) rgb(0, 0, 0)"
        foreach (const int &attrId, tmpList) {
            if (elem.hasAttribute(attrId)) {
                QString attrValue = elem.attribute(attrId);
                if (attrValue.contains("url")) {
                    int pos = attrValue.indexOf(' ');
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
        }

        if (currTag == E_use) {
            if (elem.doubleAttribute(AttrId::width) < 0)
                elem.setAttribute(AttrId::width, V_null);
            if (elem.doubleAttribute(AttrId::height) < 0)
                elem.setAttribute(AttrId::height, V_null);
        } else if (currTag == E_rect) {
            // fix wrong 'rx', 'ry' attributes in 'rect' elem
            // remove, if one of 'r' is null
            if ((elem.hasAttribute(AttrId::rx) && elem.hasAttribute(AttrId::ry))
                && (elem.attribute(AttrId::rx) == 0 || elem.attribute(AttrId::ry) == 0)) {
                elem.removeAttribute(AttrId::rx);
                elem.removeAttribute(AttrId::ry);
            }

            // if only one 'r', create missing with same value
            if (!elem.hasAttribute(AttrId::rx) && elem.hasAttribute(AttrId::ry))
                elem.setAttribute(AttrId::rx, elem.attribute(AttrId::ry));
            if (!elem.hasAttribute(AttrId::ry) && elem.hasAttribute(AttrId::rx))
                elem.setAttribute(AttrId::ry, elem.attribute(AttrId::rx));

            // rx/ry can not be bigger then width/height
            double halfWidth = elem.doubleAttribute(AttrId::width) / 2;
            double halfHeight = elem.doubleAttribute(AttrId::height) / 2;
            if (elem.hasAttribute(AttrId::rx) && elem.doubleAttribute(AttrId::rx) >= halfWidth)
                elem.setAttribute(AttrId::rx, fromDouble(halfWidth));
            if (elem.hasAttribute(AttrId::ry) && elem.doubleAttribute(AttrId::ry) >= halfHeight)
                elem.setAttribute(AttrId::ry, fromDouble(halfHeight));
        }

        nextElement(elem, root);
    }
}

// TODO: remove spaces from 'enable-background' attr
void Replacer::finalFixes()
{
    element_loop(svgElement()) {
        QString tagName = elem.tagName();

        elem.removeAttribute(AttrId::used_element);

        if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
            if (tagName == E_rect || tagName == E_use) {
                elem.removeAttributeIf(AttrId::ry, elem.attribute(AttrId::rx));
                elem.removeAttributeIf(AttrId::x, V_null);
                elem.removeAttributeIf(AttrId::y, V_null);
            }
            else if (tagName == E_circle) {
                elem.removeAttributeIf(AttrId::cx, V_null);
                elem.removeAttributeIf(AttrId::cy, V_null);
            }
            else if (tagName == E_line) {
                elem.removeAttributeIf(AttrId::x1, V_null);
                elem.removeAttributeIf(AttrId::y1, V_null);
                elem.removeAttributeIf(AttrId::x2, V_null);
                elem.removeAttributeIf(AttrId::y2, V_null);
            } else if (tagName == E_marker) {
                elem.removeAttributeIf("refX", V_null);
                elem.removeAttributeIf("refY", V_null);
            }

            if (tagName == E_linearGradient
                && (elem.hasAttribute(AttrId::x2) || elem.hasAttribute(AttrId::y2))) {
                if (elem.attribute(AttrId::x1) == elem.attribute(AttrId::x2)) {
                    elem.removeAttribute(AttrId::x1);
                    elem.setAttribute(AttrId::x2, V_null);
                }
                if (elem.attribute(AttrId::y1) == elem.attribute(AttrId::y2)) {
                    elem.removeAttribute(AttrId::y1);
                    elem.removeAttribute(AttrId::y2);
                }
                // remove 'gradientTransform' attr if only x2=0 attr left
                if (   !elem.hasAttribute(AttrId::x1) && elem.attribute(AttrId::x2) == V_null
                    && !elem.hasAttribute(AttrId::y1) && !elem.hasAttribute(AttrId::y2)) {
                    elem.removeTransform();
                }
            } else if (tagName == E_radialGradient) {
                double fx = elem.doubleAttribute(AttrId::fx);
                double fy = elem.doubleAttribute(AttrId::fy);
                double cx = elem.doubleAttribute(AttrId::cx);
                double cy = elem.doubleAttribute(AttrId::cy);
                if (isZero(qAbs(fx-cx)))
                    elem.removeAttribute(AttrId::fx);
                if (isZero(qAbs(fy-cy)))
                    elem.removeAttribute(AttrId::fy);
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

        nextElement(elem, root);
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
void Replacer::trimIds()
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
    StringHash idHash;
    QList<int> numList = QList<int>() << 0;
    bool isSkipped = true;
    bool disableDigitId = Keys.flag(Key::DisableDigitId);
    element_loop(svgElement()) {
        if (!elem.hasAttribute(AttrId::id)) {
            nextElement(elem, root);
            continue;
        }

        // by XML spec 'id' attribute could not start with digit
        // so we need skip it
        if (disableDigitId) {
            QChar c = charList.at(numList.last());
            while (c.isDigit() || c == 'Z') {
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

        idHash.insert(elem.id(), newId);
        elem.setAttribute(AttrId::id, newId);

        pos++;
        isSkipped = false;
        nextElement(elem, root);
    }

    element_loop_next(svgElement()) {
        foreach (const int &attrId, Properties::linkableStyleAttributesIds) {
            if (elem.hasAttribute(attrId)) {
                QString url = elem.attribute(attrId);
                if (url.startsWith(UrlPrefix)) {
                    url = url.mid(5, url.size()-6);
                    elem.setAttribute(attrId, QString("url(#" + idHash.value(url) + ")"));
                }
            }
        }
        if (elem.hasAttribute(AttrId::xlink_href)) {
            QString link = elem.attribute(AttrId::xlink_href);
            if (!link.startsWith(QL1S("data:"))) {
                link.remove(0,1);
                elem.setAttribute(AttrId::xlink_href, QString(QL1C('#') + idHash.value(link)));
            }
        }
        nextElement(elem, root);
    }
}

struct AttrData {
    int count;
    QString value;
};

// TODO: remove 'tspan' without attributes
void Replacer::groupTextElementsStyles()
{
    IntList importantAttrs;
    importantAttrs << AttrId::x << AttrId::y << AttrId::dx << AttrId::dy << AttrId::rotate
                   << AttrId::textLength << AttrId::lengthAdjust;

    element_loop(svgElement()) {
        nextElement(elem, root);

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
            Transform::Types tsType = ts.type();
            tsType &= ~(Transform::ProportionalScale);
            if (tsType == Transform::Translate) {
                ts.setOldXY(elem.doubleAttribute(AttrId::x), elem.doubleAttribute(AttrId::y));
                elem.setAttribute(AttrId::x, fromDouble(ts.newX()));
                elem.setAttribute(AttrId::y, fromDouble(ts.newY()));
                elem.removeTransform();

                tspan = elem.firstChildElement();
                tspan_root = elem;
                while (!tspan.isNull()) {
                    ts.setOldXY(tspan.doubleAttribute(AttrId::x), tspan.doubleAttribute(AttrId::y));
                    tspan.setAttribute(AttrId::x, fromDouble(ts.newX()));
                    tspan.setAttribute(AttrId::y, fromDouble(ts.newY()));
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
            foreach (const int &attrId, firstTspan.baseAttributesList()) {
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
            foreach (const int &attrId, tspan.baseAttributesList()) {
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
        foreach (const int &attrId, attrCountHash.keys()) {
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
                foreach (const int &attrId, elem.baseAttributesList()) {
                    if (attrId == AttrId::xlink_href)
                        continue;
                    parentElem.setAttribute(attrId, elem.attribute(attrId));
                    elem.removeAttribute(attrId);
                }
            }
        }
    }
}

void Replacer::sortDefs()
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

void Replacer::roundNumericAttributes()
{
    IntList listBasedAttrList;
    listBasedAttrList << AttrId::stdDeviation << AttrId::baseFrequency << AttrId::dx
                      << AttrId::dy << AttrId::stroke_dasharray;
    IntList filterDigitList;
    filterDigitList << AttrId::stdDeviation << AttrId::baseFrequency << AttrId::k << AttrId::k1
                    << AttrId::k2 << AttrId::k3 << AttrId::specularConstant << AttrId::dx
                    << AttrId::dy << AttrId::stroke_dasharray;

    element_loop(svgElement()) {
        foreach (const int &attrId, filterDigitList) {
            if (elem.hasAttribute(attrId) && elem.attribute(attrId) != V_none) {
                QString value = elem.attribute(attrId);
                // process list based attributes
                if (listBasedAttrList.contains(attrId)) {
                    // TODO: get rid of regex
                    QStringList tmpList = value.split(QRegExp("(,|) |,"), QString::SkipEmptyParts);
                    QString tmpStr;
                    foreach (const QString &text, tmpList) {
                        bool ok;
                        if (attrId == AttrId::stroke_dasharray)
                            // stroke-dasharray do not support numbers without leading zero
                            tmpStr += QString::number(text.toDouble(&ok)) + " ";
                        else
                            tmpStr += fromDouble(text.toDouble(&ok), Round::Transform) + " ";
                        if (!ok)
                            qFatal("Error: could not process value: '%s'",
                                   qPrintable(attrIdToStr(attrId) + "=" + value));
                    }
                    tmpStr.chop(1);
                    elem.setAttribute(attrId, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = fromDouble(value.toDouble(&ok), Round::Transform);
                    if (!ok)
                        qFatal("Error: could not process value: '%s'",
                               qPrintable(attrIdToStr(attrId) + "=" + value));
                    elem.setAttribute(attrId, attrVal);
                }
            }
        }
        foreach (const int &attrId, Properties::digitListIds) {
            if (elem.hasAttribute(attrId)) {
                QString value = elem.attribute(attrId);
                if (   !value.contains(LengthType::Percent) && !value.contains(" ")
                    && !value.contains(",") && !value.isEmpty()) {
                    bool ok;
                    QString attrVal = fromDouble(value.toDouble(&ok), Round::Attribute);
                    if (!ok)
                        qFatal("Error: could not process value: '%s'",
                               qPrintable(attrIdToStr(attrId) + "=" + value));
                    elem.setAttribute(attrId, attrVal);
                }
            }
        }

        nextElement(elem, root);
    }
}

// TODO: try to convert thin rect to line-to path
// view-calendar-list.svg
// TODO: check converting without default attributes

// http://www.w3.org/TR/SVG/shapes.html
void Replacer::convertBasicShapes()
{
    element_loop(svgElement()) {
        QString ctag = elem.tagName();
        if (ctag == E_polygon || ctag == E_polyline || ctag == E_line || ctag == E_rect) {
            QString dAttr;
            if (ctag == E_line) {
                dAttr = QString("M %1,%2 %3,%4")
                        .arg(elem.attribute(AttrId::x1, "0"), elem.attribute(AttrId::y1, "0"),
                             elem.attribute(AttrId::x2, "0"), elem.attribute(AttrId::y2, "0"));
                elem.removeAttributes(QStringList() << A_x1 << A_y1 << A_x2 << A_y2);
            } else if (ctag == E_rect) {
                if (elem.doubleAttribute(AttrId::rx) == 0 || elem.doubleAttribute(AttrId::ry) == 0) {
                    double x = elem.doubleAttribute(AttrId::x);
                    double y = elem.doubleAttribute(AttrId::y);
                    double x1 = x + elem.doubleAttribute(AttrId::width);
                    double y1 = y + elem.doubleAttribute(AttrId::height);
                    dAttr = QString("M %1,%2 H%3 V%4 H%1 z").arg(x).arg(y).arg(x1).arg(y1);
                    elem.removeAttributes(QStringList() << A_x << A_y << A_width << A_height
                                                        << A_rx << A_ry);
                }
            } else if (ctag == E_polyline || ctag == E_polygon) {
                QList<Segment> segmentList;
                QString path = elem.attribute(AttrId::points).simplified();
                StringWalker sw(path);
                while (!sw.atEnd()) {
                    Segment seg;
                    seg.command = Command::MoveTo;
                    seg.absolute = true;
                    seg.srcCmd = segmentList.isEmpty();
                    seg.x = sw.number();
                    seg.y = sw.number();
                    segmentList.append(seg);
                }
                if (ctag == E_polygon) {
                    Segment seg;
                    seg.command = Command::ClosePath;
                    seg.absolute = false;
                    seg.srcCmd = true;
                    segmentList.append(seg);
                }
                dAttr = Path().segmentsToPath(segmentList);
                elem.removeAttribute(AttrId::points);
            }
            if (!dAttr.isEmpty()) {
                elem.setAttribute(AttrId::d, dAttr);
                elem.setTagName(E_path);
            }
        }

        nextElement(elem, root);
    }
}

void Replacer::splitStyleAttributes()
{
    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        if (!elem.tagName().contains(E_feFlood)) {
            if (elem.hasAttribute(AttrId::style)) {
                IntHash hash = splitStyle(elem.attribute(AttrId::style));
                foreach (const int &keyId, hash.keys())
                    elem.setAttribute(keyId, hash.value(keyId));
                elem.removeAttribute(AttrId::style);
            }
        }
        nextElement(elem, root);
    }
}

void Replacer::joinStyleAttr()
{
    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        QStringList attrs;
        foreach (const QString &attrName, Properties::presentationAttributes) {
            if (elem.hasAttribute(attrName)) {
                attrs << attrName + ":" + elem.attribute(attrName);
                elem.removeAttribute(attrName);
            }
        }
        elem.setAttribute(AttrId::style, attrs.join(";"));
        nextElement(elem, root);
    }
}

// Move linearGradient child stop elements to radialGradient or linearGradient
// which inherits of this linearGradient.
// Only when inherited linearGradient used only once.
void Replacer::mergeGradients()
{
    QStringList linkList;
    SvgElementList list = defsElement().childElements();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute(AttrId::xlink_href))
            linkList << currElem.xlinkId();

    }
    list = svgElement().childElements();
    IntList attrList = IntList() << AttrId::fill << AttrId::stroke;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        foreach (const int &attrId, attrList) {
            if (currElem.hasAttribute(attrId)) {
                QString id = currElem.defIdFromAttribute(attrId);
                if (!id.isEmpty())
                    linkList << id;
            }
        }
        if (currElem.hasChildrenElement())
            list << currElem.childElements();
    }

    list = defsElement().childElements();
    StringHash xlinkHash;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();

        if (currElem.tagName() != E_radialGradient && currElem.tagName() != E_linearGradient)
            continue;
        if (!currElem.hasAttribute(AttrId::xlink_href))
            continue;
        if (currElem.hasChildrenElement())
            continue;

        QString currLink = currElem.xlinkId();
        if (linkList.count(currLink) != 1)
            continue;

        SvgElement lineGradElem = findElement(currLink);
        if (lineGradElem.isNull() || !lineGradElem.hasChildrenElement())
            continue;

        foreach (const SvgElement &elem, lineGradElem.childElements())
            currElem.appendChild(elem);
        xlinkHash.insert(lineGradElem.id(), currElem.id());
        defsElement().removeChild(lineGradElem);
    }
    updateXLinks(xlinkHash);
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

struct LineGradStruct {
    SvgElement elem;
    QString id;
    StringHash attrs;
    QList<StringHash> stopAttrs;
};

void Replacer::mergeGradientsWithEqualStopElem()
{
    SvgElementList list = defsElement().childElements();
    QList<LineGradStruct> lineGradList;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if ((currElem.tagName() == E_linearGradient || currElem.tagName() == E_radialGradient)
            && currElem.hasChildrenElement()) {
            LineGradStruct lgs;
            lgs.elem = currElem;
            lgs.id = currElem.id();
            lgs.attrs = currElem.attributesHash(true);
            // TODO: to IntHash
            foreach (const SvgElement &stopElem, currElem.childElements()) {
                StringHash stopAttrs = stopElem.attributesHash(true);
                // set default values if missed
                if (!stopAttrs.contains(A_stop_opacity))
                    stopAttrs.insert(A_stop_opacity, "1");
                if (!stopAttrs.contains(A_offset))
                    stopAttrs.insert(A_offset, "0");
                if (!stopAttrs.contains(A_stop_color))
                    stopAttrs.insert(A_stop_color, "#000000");
                lgs.stopAttrs << stopAttrs;
            }
            lineGradList << lgs;
        }
    }

    for (int i = 0; i < lineGradList.size(); ++i) {
        LineGradStruct lgs1 = lineGradList.at(i);
        for (int j = i; j < lineGradList.size(); ++j) {
            LineGradStruct lgs2 = lineGradList.at(j);
            if (lgs1.id != lgs2.id && lgs1.stopAttrs.size() == lgs2.stopAttrs.size())
            {
                bool stopEqual = true;
                for (int s = 0; s < lgs1.stopAttrs.size(); ++s) {
                    if (lgs1.stopAttrs.at(s) != lgs2.stopAttrs.at(s)) {
                        stopEqual = false;
                        break;
                    }
                }
                if (stopEqual) {
                    lgs2.elem.setAttribute(AttrId::xlink_href, QL1C('#') + lgs1.id);
                    foreach (SvgElement stopElem, lgs2.elem.childElements())
                        lgs2.elem.removeChild(stopElem);
                    lineGradList.removeAt(j);
                    j--;
                }
            }
        }
    }
}

void Replacer::calcElementsBoundingBox()
{
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (elem.tagName() == E_rect) {
            elem.setAttribute(AttrId::bbox,
                                          elem.attribute(AttrId::x)
                                  + " " + elem.attribute(AttrId::y)
                                  + " " + elem.attribute(AttrId::width)
                                  + " " + elem.attribute(AttrId::height));
        } else if (elem.tagName() == E_circle) {
            double r = elem.doubleAttribute(AttrId::r);
            double x = elem.doubleAttribute(AttrId::cx) - r;
            double y = elem.doubleAttribute(AttrId::cy) - r;
            elem.setAttribute(AttrId::bbox,
                                          fromDouble(x)
                                  + " " + fromDouble(y)
                                  + " " + fromDouble(qAbs(r*2))
                                  + " " + fromDouble(qAbs(r*2)));
        } else if (elem.tagName() == E_ellipse) {
            double rx = elem.doubleAttribute(AttrId::rx);
            double ry = elem.doubleAttribute(AttrId::ry);
            double x = elem.doubleAttribute(AttrId::cx) - rx;
            double y = elem.doubleAttribute(AttrId::cy) - ry;
            elem.setAttribute(AttrId::bbox,
                                          fromDouble(x)
                                  + " " + fromDouble(y)
                                  + " " + fromDouble(qAbs(rx*2))
                                  + " " + fromDouble(qAbs(ry*2)));
        }
        // all other basic shapes bounding boxes are calculated in Paths class

        if (elem.hasChildrenElement())
            list << elem.childElements();
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

// TODO: partial transform attr group
//       demo.svg, applications-development.svg
// TODO: group non successively used attributes
//       Anonymous_City_flag_of_Gijon_Asturies_Spain.svg
void Replacer::_groupElementsByStyles(SvgElement parentElem)
{
    // first start
    if (parentElem.isNull())
        parentElem = svgElement();

    IntHash groupHash;
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
            foreach (const int &attrId, additionalAttrList) {
                if (currElem.hasAttribute(attrId))
                    groupHash.insert(attrId, currElem.attribute(attrId));
            }
            if (    currElem.hasTransform()
                && !currElem.hasLinkedDef()
                && !parentElem.hasLinkedDef()) {
                groupHash.insert(AttrId::transform, currElem.transform().simplified());
            }

            // we can not group elements by some attributes
            foreach (const int &attrId, ignoreAttrList) {
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
            IntHash lastGroupHash = groupHash;
            // remove attributes which do not exist or are different in next element
            foreach (const int &attrId, groupHash.keys()) {
                if (!currElem.hasAttribute(attrId))
                    groupHash.remove(attrId);
                else if (currElem.attribute(attrId) != groupHash.value(attrId))
                    groupHash.remove(attrId);
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
                    foreach (const int &attrId, lastGroupHash.keys()) {
                        if (parentGElem.hasAttribute(attrId) && attrId == AttrId::transform) {
                            Transform ts = parentGElem.transform();
                            Transform ts2 = Transform::create(lastGroupHash.value(attrId));
                            ts.append(ts2);
                            parentGElem.setTransform(ts);
                        } else {
                            parentGElem.setAttribute(attrId, lastGroupHash.value(attrId));
                        }
                    }
                    // move elem to group
                    foreach (SvgElement similarElem, similarElemList) {
                        foreach (const int &attrId, lastGroupHash.keys())
                            similarElem.removeAttribute(attrId);
                        // if we remove attr from group and now it does not have any
                        // important attributes - we can ungroup it
                        if (similarElem.isGroup() && !similarElem.hasImportantAttrs()) {
                            foreach (SvgElement gChildElem, similarElem.childElements())
                                parentGElem.appendChild(gChildElem);
                            similarElem.parentElement().removeChild(similarElem);
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

void Replacer::groupElementsByStyles()
{
    _groupElementsByStyles();
}

void Replacer::markUsedElements()
{
    StringSet usedElemList;
    SvgElement elem = svgElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        if (elem.tagName() == E_use || elem.tagName() == E_textPath) {
            if (elem.hasAttribute(AttrId::xlink_href))
                usedElemList << elem.xlinkId();
        }
        nextElement(elem, root);
    }

    elem = svgElement();
    while (!elem.isNull()) {
        QString id = elem.id();
        if (!id.isEmpty()) {
            if (usedElemList.contains(id))
                elem.setAttribute(AttrId::used_element, "1");
        }
        nextElement(elem, root);
    }
}

// TODO: store linked elements as refs
QHash<QString,int> Replacer::calcDefsUsageCount()
{
    IntList attrList;
    attrList << AttrId::fill << AttrId::filter << AttrId::stroke;
    QHash<QString,int> idHash;
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        foreach (const int &attrId, attrList) {
            QString id = elem.defIdFromAttribute(attrId);
            if (!id.isEmpty()) {
                if (idHash.contains(id))
                    idHash.insert(id, idHash.value(id) + 1);
                else
                    idHash.insert(id, 1);
            }
        }
        QString xlink = elem.xlinkId();
        if (!xlink.isEmpty()) {
            if (idHash.contains(xlink))
                idHash.insert(xlink, idHash.value(xlink) + 1);
            else
                idHash.insert(xlink, 1);
        }

        if (elem.hasChildrenElement())
            list << elem.childElements();
    }
    return idHash;
}

void Replacer::applyTransformToDefs()
{
    SvgElementList list = defsElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == E_linearGradient) {
            if (elem.hasTransform()) {
                Transform gts = elem.transform();
                if (gts.isProportionalScale()) {
                    gts.setOldXY(elem.doubleAttribute(AttrId::x1),
                                 elem.doubleAttribute(AttrId::y1));
                    elem.setAttribute(AttrId::x1, fromDouble(gts.newX()));
                    elem.setAttribute(AttrId::y1, fromDouble(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute(AttrId::x2),
                                 elem.doubleAttribute(AttrId::y2));
                    elem.setAttribute(AttrId::x2, fromDouble(gts.newX()));
                    elem.setAttribute(AttrId::y2, fromDouble(gts.newY()));
                    elem.removeTransform();
                }
            }
        } else if (elem.tagName() == E_radialGradient) {
            if (elem.hasTransform()) {
                Transform gts = elem.transform();
                if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                    gts.setOldXY(elem.doubleAttribute(AttrId::fx),
                                 elem.doubleAttribute(AttrId::fy));
                    if (elem.hasAttribute(AttrId::fx))
                        elem.setAttribute(AttrId::fx, fromDouble(gts.newX()));
                    if (elem.hasAttribute(AttrId::fy))
                        elem.setAttribute(AttrId::fy, fromDouble(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute(AttrId::cx),
                                 elem.doubleAttribute(AttrId::cy));
                    elem.setAttribute(AttrId::cx, fromDouble(gts.newX()));
                    elem.setAttribute(AttrId::cy, fromDouble(gts.newY()));

                    elem.setAttribute(AttrId::r, fromDouble(elem.doubleAttribute(AttrId::r)
                                                            * gts.scaleFactor()));
                    elem.removeTransform();
                }
            }
        }
        if (elem.hasChildrenElement())
            list << elem.childElements();
    }
}

void Replacer::applyTransformToShapes()
{
    // TODO: add another shapes

    QHash<QString,int> defsHash = calcDefsUsageCount();

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        SvgElement firstChild = elem.firstChildElement();

        if (   elem.isGroup()
            && elem.hasTransform()
            && !elem.hasAttribute(AttrId::clip_path)
            && !elem.hasAttribute(AttrId::mask)
            && !elem.hasAttribute(AttrId::filter)
            && elem.childElementCount() == 1
            && !elem.isUsed()
            && firstChild.tagName() != E_use
            && !firstChild.isGroup()
            && !firstChild.isUsed()
            && !firstChild.hasAttribute(AttrId::clip_path))
        {
            firstChild.setTransform(elem.transform(), true);
            elem.removeTransform();
        }
        else if (   elem.tagName() == E_rect
                 && elem.hasTransform()
                 && !elem.hasAttribute(AttrId::clip_path)
                 && !elem.hasAttribute(AttrId::mask)
                 && defsHash.value(elem.defIdFromAttribute(AttrId::filter)) < 2
                 && defsHash.value(elem.defIdFromAttribute(AttrId::stroke)) < 2
                 && defsHash.value(elem.defIdFromAttribute(AttrId::fill)) < 2)
        {
            bool canApplyTransform = true;
            if (elem.hasAttribute(AttrId::fill)) {
                SvgElement fillDef = elemFromDefs(elem.defIdFromAttribute(AttrId::fill));
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (elem.hasAttribute(AttrId::stroke)) {
                SvgElement fillDef = elemFromDefs(elem.defIdFromAttribute(AttrId::stroke));
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (canApplyTransform) {
                Transform ts = elem.transform();
                if (   !ts.isMirrored()
                    && !ts.isRotating()
                    && !ts.isSkew()
                    &&  ts.isProportionalScale()) {
                    ts.setOldXY(elem.doubleAttribute(AttrId::x), elem.doubleAttribute(AttrId::y));
                    elem.setAttribute(AttrId::x, fromDouble(ts.newX()));
                    elem.setAttribute(AttrId::y, fromDouble(ts.newY()));
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

void Replacer::calcNewStrokeWidth(SvgElement &elem, double scaleFactor)
{
    SvgElement parentElem = elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(AttrId::stroke_width)) {
            double strokeWidth = Tools::convertUnitsToPx(parentElem.attribute(AttrId::stroke_width))
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
