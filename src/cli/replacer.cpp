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

// TODO: remove spaces at end of line in text elem
// TODO: replace equal 'fill', 'stroke', 'stop-color', 'flood-color' and 'lighting-color' attr
//       with 'color' attr
//       addon_the_couch.svg
// TODO: If 'x1' = 'x2' and 'y1' = 'y2', then the area to be painted will be painted as
//       a single color using the color and opacity of the last gradient stop.
// TODO: merge "tspan" elements with similar styles
// TODO: try to recalculate 'userSpaceOnUse' to 'objectBoundingBox'

void Replacer::convertSizeToViewbox()
{
    if (!svgElement().hasAttribute(A_viewBox)) {
        if (svgElement().hasAttribute(A_width) && svgElement().hasAttribute(A_height)) {
            QString width  = roundNumber(svgElement().doubleAttribute(A_width));
            QString height = roundNumber(svgElement().doubleAttribute(A_height));
            svgElement().setAttribute(A_viewBox, QString("0 0 %1 %2").arg(width).arg(height));
            svgElement().removeAttribute(A_width);
            svgElement().removeAttribute(A_height);
        }
    } else {
        QRectF rect = viewBoxRect();
        if (rect.isNull())
            return;
        if (svgElement().hasAttribute(A_width)) {
            if (isZero(rect.width() - svgElement().doubleAttribute(A_width)))
                svgElement().removeAttribute(A_width);
        }
        if (svgElement().hasAttribute(A_height)) {
            if (isZero(rect.height() - svgElement().doubleAttribute(A_height)))
                svgElement().removeAttribute(A_height);
        }
    }
}

// TODO: join paths with only style different
//       Anonymous_Chesspiece_-_bishop.svg
// TODO: replace paths with use, when paths has only first segment different
//       Anonymous_Flag_of_South_Korea.svg
void Replacer::processPaths()
{
    bool skip = true;
    foreach (int id, Keys.pathsKeysId()) {
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
                    elem.removeAttribute(A_transform);
                }
            }
            if (elem.attribute(A_d).isEmpty()) {
                elem.parentElement().removeChild(elem);
                removed = true;
            }
        }

        if (!removed) {
            if (elem.hasChildElement())
                list << elem.childElements();
        }
    }
}

bool Replacer::isPathValidToTransform(SvgElement &pathElem, QHash<QString,int> &defsIdHash)
{
    if (pathElem.hasAttribute(A_transform)) {
        // non proportional transform could not be applied to path with stroke
        bool hasStroke = false;
        SvgElement parentElem = pathElem;
        while (!parentElem.isNull()) {
            if (parentElem.hasAttribute(A_stroke)) {
                if (parentElem.attribute(A_stroke) != V_none) {
                    hasStroke = true;
                    break;
                }
            }
            parentElem = parentElem.parentElement();
        }
        if (hasStroke) {
            Transform ts(pathElem.attribute(A_transform));
            if (!ts.isProportionalScale())
                return false;
        }
    } else {
        return false;
    }
    if (pathElem.hasAttribute(A_clip_path) || pathElem.hasAttribute(A_mask))
        return false;
    if (pathElem.isUsed())
        return false;
    if (pathElem.hasAttribute(A_filter)) {
        // we can apply transform to blur filter, but only when it's used by only this path
        QString filterId = pathElem.defIdFromAttribute(A_filter);
        if (defsIdHash.value(filterId) > 1)
            return false;
        if (!isBlurFilter(filterId))
            return false;
    }

    QStringList attrList;
    attrList << A_fill << A_stroke;
    foreach (const QString &attrName, attrList) {
        if (pathElem.hasAttribute(attrName)) {
            QString defId = pathElem.defIdFromAttribute(attrName);
            if (!defId.isEmpty()) {
                if (defsIdHash.value(defId) > 1)
                    return false;
            }
            if (!defId.isEmpty()) {
                SvgElement defElem = findInDefs(defId);
                if (!defElem.isNull()) {
                    if (   defElem.tagName() != E_linearGradient
                        && defElem.tagName() != E_radialGradient)
                        return false;
                    if (defElem.attribute("gradientUnits") != "userSpaceOnUse")
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
    QStringList attrList;
    attrList << A_fill << A_stroke << A_filter;
    foreach (const QString &attrName, attrList) {
        QString defId = elem.defIdFromAttribute(attrName);
        if (!defId.isEmpty()) {
            SvgElement defElem = findInDefs(defId);
            if (!defElem.isNull()) {
                if ((  defElem.tagName() == E_linearGradient
                    || defElem.tagName() == E_radialGradient)
                       && defElem.attribute("gradientUnits") == "userSpaceOnUse")
                {
                    QString gradTs = defElem.attribute(A_gradientTransform);
                    if (!gradTs.isEmpty()) {
                        Transform ts(elem.attribute(A_transform) + " " + gradTs);
                        defElem.setAttribute(A_gradientTransform, ts.simplified());
                    } else {
                        defElem.setAttribute(A_gradientTransform, elem.attribute(A_transform));
                    }
                } else if (defElem.tagName() == E_filter) {
                    Transform ts(elem.attribute(A_transform));
                    SvgElement stdDevElem = defElem.firstChildElement();
                    qreal oldStd = stdDevElem.doubleAttribute(A_stdDeviation);
                    QString newStd = roundNumber(oldStd * ts.scaleFactor());
                    stdDevElem.setAttribute(A_stdDeviation, newStd);
                }
            }
        }
    }
}

class EqElement {
public:
    QString tagName;
    StringHash attrHash;
    SvgElement elem;
    bool operator ==(const EqElement &elem) {
        return elem.elem == this->elem;
    }
};

// FIXME: parent styles should be set to elem before moving to defs
//        address-book-new.svg
// TODO: reuse groups
void Replacer::replaceEqualElementsByUse()
{
    QStringList rectAttrs;
    rectAttrs << A_x << A_y << A_width << A_height << A_rx << A_ry;
    QStringList circleAttrs;
    circleAttrs << A_cx << A_cy << A_r;
    QStringList ellipseAttrs;
    ellipseAttrs << A_cx << A_cy << A_rx << A_ry;
    SvgElementList list = svgElement().childElements();
    int newAttrId = 0;

    QList<EqElement> elemList;
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        bool canUse = false;
        if (elem.tagName() == E_path || elem.tagName() == E_rect)
            canUse = true;
        if (hasParent(elem, E_defs)) {
            if (elem.parentElement().tagName() != E_defs)
                canUse = false;
        }
        if (canUse) {
            EqElement e;
            e.tagName = elem.tagName();
            StringHash hash;
            if (e.tagName == E_path)
                hash.insert(A_d, elem.attribute(A_d));
            else if (e.tagName == E_rect) {
                foreach (const QString &attrName, rectAttrs)
                    hash.insert(attrName, elem.attribute(attrName));
            } else if (e.tagName == E_circle) {
                foreach (const QString &attrName, circleAttrs)
                   hash.insert(attrName, elem.attribute(attrName));
            } else if (e.tagName == E_ellipse) {
                foreach (const QString &attrName, ellipseAttrs)
                    hash.insert(attrName, elem.attribute(attrName));
            }
            e.attrHash = hash;
            e.elem = elem;
            elemList << e;
        }
        if (elem.hasChildElement())
            list << elem.childElements();
    }

    for (int i = 0; i < elemList.size(); ++i) {
        EqElement mainEqElem = elemList.at(i);
        QList<EqElement> equalElems;
        for (int j = i; j < elemList.size(); ++j) {
            if (   mainEqElem.tagName == elemList.at(j).tagName
                && mainEqElem.elem != elemList.at(j).elem
                && mainEqElem.attrHash == elemList.at(j).attrHash) {
                    equalElems << elemList.at(j);
            }
        }

        if (equalElems.size() == 1
            && equalElems.first().elem.styleHash() == mainEqElem.elem.styleHash())
        {
            SvgElement eqElem = equalElems.first().elem;
            SvgElement mainElem = mainEqElem.elem;
            mainElem.setAttribute(CleanerAttr::UsedElement, "1");
            eqElem.setTagName(E_use);
            foreach (const QString &attrName, mainEqElem.attrHash.keys())
                eqElem.removeAttribute(attrName);
            eqElem.removeAttribute(CleanerAttr::BoundingBox);
            if (!mainElem.hasAttribute(A_id))
                mainElem.setAttribute(A_id, "SVGCleanerId_" + QString::number(newAttrId++));
            eqElem.setAttribute(A_xlink_href, "#" + mainElem.id());
            if (eqElem.attribute(A_transform) == mainElem.attribute(A_transform))
                eqElem.removeAttribute(A_transform);
            else {
                Transform tr(eqElem.attribute(A_transform));
                tr.divide(mainElem.attribute(A_transform));
                eqElem.setAttribute(A_transform, tr.simplified());
            }
            elemList.removeOne(equalElems.first());
        } else if (equalElems.size() > 1) {
            SvgElement newElem = document().createElement(mainEqElem.tagName);
            newElem.setAttribute(A_id, "SVGCleanerId_" + QString::number(newAttrId++));
            foreach (const QString &attrName, mainEqElem.attrHash.keys())
                newElem.setAttribute(attrName, mainEqElem.attrHash.value(attrName));
            newElem.setAttribute(CleanerAttr::UsedElement, "1");
            defsElement().appendChild(newElem);
            equalElems << mainEqElem;
            foreach (EqElement eqElem, equalElems) {
                SvgElement elem = eqElem.elem;
                elem.setTagName(E_use);
                elem.setAttribute(A_xlink_href, "#" + newElem.id());
                elem.removeAttribute(CleanerAttr::BoundingBox);
                foreach (const QString &attrName, mainEqElem.attrHash.keys())
                    elem.removeAttribute(attrName);
                elemList.removeOne(eqElem);
            }
        }
    }
}

// TODO: process use element which is used too
// webmichl_wristwatch_1_-_chronometer.svg
void Replacer::moveStyleFromUsedElemToUse()
{
    QStringList useAttrs;
    useAttrs << A_x << A_y << A_width << A_height;

    QHash<QString,SvgElement> elemXLink;
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == E_use)
            elemXLink.insertMulti(elem.xlinkId(), elem);
        if (elem.hasChildElement())
            list << elem.childElements();
    }

    list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.isUsed()) {
            SvgElementList usedElemList = elemXLink.values(elem.id());
            // use elem could not overwrite style properties of used element
            foreach (SvgElement usedElem, usedElemList) {
                foreach (const QString &attrName, elem.styleAttributesList()) {
                    usedElem.removeAttribute(attrName);
                }
            }

            if (hasParent(elem, E_defs)) {
                QHash<QString, int> attrsCount;
                foreach (const SvgElement &usedElem, usedElemList) {
                    QStringList usedElemAttrs = usedElem.styleAttributesList();
                    if (usedElem.hasAttribute(A_transform))
                        usedElemAttrs << A_transform;
                    foreach (const QString &attrName, usedElemAttrs) {
                        if (usedElem.hasAttribute(attrName)) {
                            bool isCount = true;
                            if (attrName == A_transform
                                || (usedElem.hasAttributes(useAttrs) || usedElem.hasLinkedDef()))
                                isCount = false;
                            if (isCount) {
                                QString attr = attrName + ":" + usedElem.attribute(attrName);
                                if (attrsCount.contains(attr))
                                    attrsCount.insert(attr, attrsCount.value(attr) + 1);
                                else
                                    attrsCount.insert(attr, 1);
                            }
                        }
                    }
                }
                foreach (const QString &attr, attrsCount.keys()) {
                    // do not replace transform when used elem has already has transform
                    if (attrsCount.value(attr) == usedElemList.count()
                        && !(attr.startsWith(A_transform)
                             && (elem.hasAttribute(A_transform) || attrsCount.value(attr) == 1)))
                    {
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
        }

        if (elem.hasChildElement())
            list << elem.childElements();
    }
}

void Replacer::convertUnits()
{
    QRectF rect = viewBoxRect();
    if (svgElement().hasAttribute(A_width)) {
        QString widthStr = svgElement().attribute(A_width);
        if (widthStr.contains(LengthType::percent) && rect.isNull())
            qFatal("Error: could not convert width in percentage into px without viewBox");
        bool ok;
        qreal width = Tools::convertUnitsToPx(widthStr, rect.width()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert width to px");
        svgElement().setAttribute(A_width, roundNumber(width));
    }
    if (svgElement().hasAttribute(A_height)) {
        QString heightStr = svgElement().attribute(A_height);
        if (heightStr.contains(LengthType::percent) && rect.isNull())
            qFatal("Error: could not convert height in percentage into px without viewBox");
        bool ok;
        qreal height = Tools::convertUnitsToPx(heightStr, rect.height()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert height to px");
        svgElement().setAttribute(A_height, roundNumber(height));
    }

    // TODO: process 'offset' attr with %
    element_loop(document().documentElement()) {
        QString currTag = elem.tagName();
        if (elem.hasAttribute(A_font_size)) {
            bool ok = false;
            QString fontSizeStr = elem.attribute(A_font_size);
            fontSizeStr.toDouble(&ok);
            if (!ok) {
                if (   fontSizeStr.endsWith(LengthType::ex)
                    || fontSizeStr.endsWith(LengthType::em)
                    || fontSizeStr.endsWith(LengthType::percent))
                {
                    QString parentFontSize = findAttribute(elem.parentElement(), A_font_size);
                    if (parentFontSize.isEmpty() || parentFontSize == V_null)
                        qFatal("Error: could not calculate relative font-size");
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr, parentFontSize.toDouble());
                    if (newFontSize == V_null)
                        elem.removeAttribute(A_font_size);
                    else
                        elem.setAttribute(A_font_size, newFontSize);
                } else {
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr);
                    elem.setAttribute(A_font_size, newFontSize);
                }
            }
        }
        foreach (const QString &attrName, elem.attributesListBySet(Properties::digitList)) {
            // fix attributes like:
            // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
            // FIXME: ignores list based attr
            QString attrValue = elem.attribute(attrName);
//            qDebug() << attrName << attrValue;
            if (attrValue.contains(" "))
                attrValue = attrValue.left(attrValue.indexOf(" "));

            // TODO: process gradients attrs
            if (attrValue.contains(LengthType::percent)) {
                if (currTag != E_radialGradient && currTag != E_linearGradient) {
                    if (attrName.contains(A_x) || attrName == A_width)
                        attrValue = Tools::convertUnitsToPx(attrValue, rect.width());
                    else if (attrName.contains(A_y) || attrName == A_height)
                       attrValue = Tools::convertUnitsToPx(attrValue, rect.height());
                }
            } else if (attrValue.endsWith(LengthType::ex) || attrValue.endsWith(LengthType::em)) {
                qreal fontSize = findAttribute(elem, A_font_size).toDouble();
                if (fontSize == 0)
                    qFatal("Error: could not convert em/ex values "
                           "without font-size attribute is set.");
                attrValue = Tools::convertUnitsToPx(attrValue, fontSize);
            } else {
                attrValue = Tools::convertUnitsToPx(attrValue);
            }
            elem.setAttribute(attrName, attrValue);
        }

        nextElement(elem, root);
    }
}

StringHash splitStyle(const QString &style)
{
    StringHash hash;
    if (style.isEmpty())
        return hash;
    QStringList list = style.trimmed().split(QL1C(';'), QString::SkipEmptyParts);
    for (int i = 0; i < list.count(); ++i) {
        QString attr = list.at(i);
        int pos = attr.indexOf(QL1C(':'));
        if (pos != -1)
            hash.insert(attr.mid(0, pos).trimmed(), attr.mid(pos+1).trimmed());
    }
    return hash;
}

// TODO: style can be set in ENTITY
void Replacer::convertCDATAStyle()
{
    static const QString A_class = QL1S("class");

    QStringList styleList;
    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        if (elem.tagName() == E_style) {
            styleList << elem.text();
            elem.parentNode().removeChild(elem);
        }
        nextElement(elem, root);
    }
    if (styleList.isEmpty()) {
        // remove class attribute when no CDATA set
        SvgElement elem = document().documentElement();
        SvgElement root = elem;
        while (!elem.isNull()) {
            elem.removeAttribute(A_class);
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
            if (tmpList.size() == 2)
                classHash.insert(tmpList.at(0), QString(tmpList.at(1)).remove(QRegExp("\\}.*")));
        }
    }

    elem = document().documentElement();
    root = elem;
    while (!elem.isNull()) {
        if (elem.hasAttribute(A_class)) {
            StringHash newHash;
            QStringList classList = elem.attribute(A_class).split(" ", QString::SkipEmptyParts);
            for (int i = 0; i < classList.count(); ++i) {
                if (classHash.contains(classList.at(i))) {
                    StringHash tempHash = splitStyle(classHash.value(classList.at(i)));
                    foreach (const QString &key, tempHash.keys())
                        newHash.insert(key, tempHash.value(key));
                }
            }
            StringHash oldHash = elem.styleHash();
            foreach (const QString &key, oldHash.keys())
                newHash.insert(key, oldHash.value(key));
            elem.setStylesFromHash(newHash);
            elem.removeAttribute(A_class);
        }
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
    // fix bad Adobe Illustrator SVG exporting
    if (svgElement().attribute("xmlns") == "&ns_svg;")
        svgElement().setAttribute("xmlns", "http://www.w3.org/2000/svg");
    if (svgElement().attribute("xmlns:xlink") == "&ns_xlink;")
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    QStringList tmpList = QStringList() << A_fill << A_stroke;
    element_loop(svgElement()) {
        QString currTag = elem.tagName();

        // remove wrong fill and stroke attributes like:
        // fill="url(#radialGradient001) rgb(0, 0, 0)"
        foreach (const QString &attrName, tmpList) {
            if (elem.hasAttribute(attrName)) {
                QString attrValue = elem.attribute(attrName);
                if (attrValue.contains("url")) {
                    int pos = attrValue.indexOf(' ');
                    if (pos > 0) {
                        attrValue.remove(pos, attrValue.size());
                        elem.setAttribute(attrName, attrValue);
                    }
                }
            }
        }

        // gradient with stop elements does not need xlink:href attribute
        if (currTag == E_linearGradient || currTag == E_radialGradient) {
            if (elem.hasChildElement() && elem.hasAttribute(A_xlink_href))
                elem.removeAttribute(A_xlink_href);
        }

        if (currTag == E_use) {
            if (elem.doubleAttribute(A_width) < 0)
                elem.setAttribute(A_width, V_null);
            if (elem.doubleAttribute(A_height) < 0)
                elem.setAttribute(A_height, V_null);
        } else if (currTag == E_rect) {
            // fix wrong 'rx', 'ry' attributes in 'rect' elem
            // remove, if one of 'r' is null
            if ((elem.hasAttribute(A_rx) && elem.hasAttribute(A_ry))
                && (elem.attribute(A_rx) == 0 || elem.attribute(A_ry) == 0)) {
                elem.removeAttribute(A_rx);
                elem.removeAttribute(A_ry);
            }

            // if only one 'r', create missing with same value
            if (!elem.hasAttribute(A_rx) && elem.hasAttribute(A_ry))
                elem.setAttribute(A_rx, elem.attribute(A_ry));
            if (!elem.hasAttribute(A_ry) && elem.hasAttribute(A_rx))
                elem.setAttribute(A_ry, elem.attribute(A_rx));

            // rx/ry can not be bigger then width/height
            qreal halfWidth = elem.doubleAttribute(A_width) / 2;
            qreal halfHeight = elem.doubleAttribute(A_height) / 2;
            if (elem.hasAttribute(A_rx) && elem.doubleAttribute(A_rx) >= halfWidth)
                elem.setAttribute(A_rx, roundNumber(halfWidth));
            if (elem.hasAttribute(A_ry) && elem.doubleAttribute(A_ry) >= halfHeight)
                elem.setAttribute(A_ry, roundNumber(halfHeight));
        }

        nextElement(elem, root);
    }
}

void Replacer::finalFixes()
{
    element_loop(svgElement()) {
        QString tagName = elem.tagName();

        elem.removeAttribute(CleanerAttr::UsedElement);

        if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
            if (tagName == E_rect || tagName == E_use) {
                elem.removeAttributeIf(A_ry, elem.attribute(A_rx));
                elem.removeAttributeIf(A_x, V_null);
                elem.removeAttributeIf(A_y, V_null);
            }
            else if (tagName == E_circle) {
                elem.removeAttributeIf(A_cx, V_null);
                elem.removeAttributeIf(A_cy, V_null);
            }
            else if (tagName == E_line) {
                elem.removeAttributeIf(A_x1, V_null);
                elem.removeAttributeIf(A_y1, V_null);
                elem.removeAttributeIf(A_x2, V_null);
                elem.removeAttributeIf(A_y2, V_null);
            } else if (tagName == E_marker) {
                elem.removeAttributeIf("refX", V_null);
                elem.removeAttributeIf("refY", V_null);
            }

            if (tagName == E_linearGradient
                && (elem.hasAttribute(A_x2) || elem.hasAttribute(A_y2))) {
                if (elem.attribute(A_x1) == elem.attribute(A_x2)) {
                    elem.removeAttribute(A_x1);
                    elem.setAttribute(A_x2, V_null);
                }
                if (elem.attribute(A_y1) == elem.attribute(A_y2)) {
                    elem.removeAttribute(A_y1);
                    elem.removeAttribute(A_y2);
                }
                // remove 'gradientTransform' attr if only x2=0 attr left
                if (   !elem.hasAttribute(A_x1) && elem.attribute(A_x2) == V_null
                    && !elem.hasAttribute(A_y1) && !elem.hasAttribute(A_y2)) {
                    elem.removeAttribute(A_gradientTransform);
                }
            } else if (tagName == E_radialGradient) {
                qreal fx = elem.doubleAttribute(A_fx);
                qreal fy = elem.doubleAttribute(A_fy);
                qreal cx = elem.doubleAttribute(A_cx);
                qreal cy = elem.doubleAttribute(A_cy);
                if (isZero(qAbs(fx-cx)))
                    elem.removeAttribute(A_fx);
                if (isZero(qAbs(fy-cy)))
                    elem.removeAttribute(A_fy);
            }
        }

        if (Keys.flag(Key::RemoveInvisibleElements)) {
            // remove empty defs
            if (elem.tagName() == E_defs) {
                if (!elem.hasChildElement()) {
                    SvgElement tElem = prevElement(elem);
                    elem.parentElement().removeChild(elem);
                    elem = tElem;
                }
            }
        }

        nextElement(elem, root);
    }
}

void Replacer::plusOne(QList<int> &list, int offset)
{
    if (offset > list.size()-1)
        list << 0;
    // 62 is charList size
    if (list.at(offset) + 1 == 62) {
        list[offset] = 0;
        plusOne(list, offset+1);
    } else
        list[offset]++;
}

// convert id names to short one using custom numeral system
void Replacer::trimIds()
{
    QStringList charList;
    for (int i = 48; i < 58; ++i) // 0-9
        charList << QChar(i);
    for (int i = 65; i < 91; ++i) // A-Z
        charList << QChar(i);
    for (int i = 97; i < 123; ++i) // a-z
        charList << QChar(i);

    int pos = 0;
    StringHash idHash;
    element_loop(svgElement()) {
        if (elem.hasAttribute(A_id)) {
            QString newId;
            // gen new id
            QList<int> intList;
            intList.reserve(3);
            for (int j = 0; j < pos; ++j)
                plusOne(intList);
            while (!intList.isEmpty())
                newId += charList.at(intList.takeLast());
            if (newId.isEmpty())
                newId = V_null;

            idHash.insert(elem.id(), newId);
            elem.setAttribute(A_id, newId);
            pos++;
        }
        nextElement(elem, root);
    }

    element_loop_next(svgElement()) {
        foreach (const QString &attrName, Properties::linkableStyleAttributes) {
            if (elem.hasAttribute(attrName)) {
                QString url = elem.attribute(attrName);
                if (url.startsWith(UrlPrefix)) {
                    url = url.mid(5, url.size()-6);
                    elem.setAttribute(attrName, QString("url(#" + idHash.value(url) + ")"));
                }
            }
        }
        if (elem.hasAttribute(A_xlink_href)) {
            QString id = elem.attribute(A_xlink_href);
            if (!id.startsWith(QL1S("data:"))) {
                id.remove(0,1);
                elem.setAttribute(A_xlink_href, QString("#" + idHash.value(id)));
            }
        }
        nextElement(elem, root);
    }
}

void Replacer::calcElemAttrCount(const QString &text)
{
    quint32 elemCount = 0;
    quint32 attrCount = 0;

    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        elemCount++;
        attrCount += elem.attributesCount();
        nextElement(elem, root);
    }
    if (!Keys.flag(Key::ShortOutput)) {
        qDebug("The %s number of elements is: %u",   qPrintable(text), elemCount);
        qDebug("The %s number of attributes is: %u", qPrintable(text), attrCount);
    } else {
        qDebug("%u", elemCount);
        qDebug("%u", attrCount);
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
    QStringList listBasedAttrList;
    listBasedAttrList << A_stdDeviation << A_baseFrequency << A_dx << A_dy << A_stroke_dasharray;
    StringSet filterDigitList;
    filterDigitList << A_stdDeviation << A_baseFrequency << QL1S("k") << QL1S("k1") << QL1S("k2")
                    << QL1S("k3") << QL1S("specularConstant") << A_dx << A_dy << A_stroke_dasharray;

    element_loop(svgElement()) {
        QStringList attrList = elem.attributesList();
        foreach (const QString &attr, filterDigitList) {
            if (attrList.contains(attr) && elem.attribute(attr) != V_none) {
                QString value = elem.attribute(attr);
                // process list based attributes
                if (listBasedAttrList.contains(attr)) {
                    // TODO: get rid of regex
                    QStringList tmpList = value.split(QRegExp("(,|) |,"), QString::SkipEmptyParts);
                    QString tmpStr;
                    foreach (const QString &text, tmpList) {
                        bool ok;
                        if (attr == A_stroke_dasharray)
                            tmpStr += QString::number(text.toDouble(&ok)) + " ";
                        else
                            tmpStr += roundNumber(text.toDouble(&ok), Round::Transform) + " ";
                        if (!ok)
                            qFatal("Error: could not process value: '%s'",
                                   qPrintable(attr + "=" + value));
                    }
                    tmpStr.chop(1);
                    elem.setAttribute(attr, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = roundNumber(value.toDouble(&ok), Round::Transform);
                    if (!ok)
                        qFatal("Error: could not process value: '%s'", qPrintable(attr + "=" + value));
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        foreach (const QString &attr, Properties::digitList) {
            if (attrList.contains(attr)) {
                QString value = elem.attribute(attr);
                if (   !value.contains(LengthType::percent) && !value.contains(" ")
                    && !value.contains(",") && !value.isEmpty()) {
                    bool ok;
                    QString attrVal = roundNumber(value.toDouble(&ok), Round::Attribute);
                    if (!ok)
                        qFatal("Error: could not process value: '%s'", qPrintable(attr + "=" + value));
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        if (Keys.flag(Key::SimplifyTransformMatrix)) {
            if (attrList.contains(A_gradientTransform)) {
                Transform ts(elem.attribute(A_gradientTransform));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    elem.removeAttribute(A_gradientTransform);
                else
                    elem.setAttribute(A_gradientTransform, transform);
            }
            if (attrList.contains(A_transform)) {
                Transform ts(elem.attribute(A_transform));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    elem.removeAttribute(A_transform);
                else
                    elem.setAttribute(A_transform, transform);
            }
        }

        nextElement(elem, root);
    }
}

// TODO: try to convert thin rect to line-to path
// view-calendar-list.svg

// http://www.w3.org/TR/SVG/shapes.html
void Replacer::convertBasicShapes()
{
    element_loop(svgElement()) {
        QString ctag = elem.tagName();
        if (ctag == E_polygon || ctag == E_polyline || ctag == E_line || ctag == E_rect) {
            QString dAttr;
            if (ctag == E_line) {
                dAttr = QString("M %1,%2 %3,%4")
                        .arg(elem.attribute(A_x1), elem.attribute(A_y1),
                             elem.attribute(A_x2), elem.attribute(A_y2));
                elem.removeAttributes(QStringList() << A_x1 << A_y1 << A_x2 << A_y2);
            } else if (ctag == E_rect) {
                if (elem.doubleAttribute(A_rx) == 0 || elem.doubleAttribute(A_ry) == 0) {
                    qreal x = elem.doubleAttribute(A_x);
                    qreal y = elem.doubleAttribute(A_y);
                    qreal x1 = x + elem.doubleAttribute(A_width);
                    qreal y1 = y + elem.doubleAttribute(A_height);
                    dAttr = QString("M %1,%2 H%3 V%4 H%1 z").arg(x).arg(y).arg(x1).arg(y1);
                    elem.removeAttributes(QStringList() << A_x << A_y << A_width << A_height
                                                        << A_rx << A_ry);
                }
            } else if (ctag == E_polyline || ctag == E_polygon) {
                QList<Segment> segmentList;
                QString path = elem.attribute(A_points).simplified();
                const QChar *str = path.constData();
                const QChar *end = str + path.size();
                while (str != end) {
                    Segment seg;
                    seg.command = Command::MoveTo;
                    seg.absolute = true;
                    seg.srcCmd = segmentList.isEmpty();
                    seg.x = getNum(str);
                    seg.y = getNum(str);
                    segmentList.append(seg);
                }
                if (ctag == E_polygon) {
                    Segment seg;
                    seg.command = Command::ClosePath;
                    seg.absolute = false;
                    seg.srcCmd = segmentList.isEmpty();
                    segmentList.append(seg);
                }
                dAttr = Path().segmentsToPath(segmentList);
                elem.removeAttribute(A_points);
            }
            if (!dAttr.isEmpty()) {
                elem.setAttribute(A_d, dAttr);
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
    static const QChar signChar = QL1C('-');
    while (!elem.isNull()) {
        if (!elem.tagName().contains(E_feFlood)) {
            if (elem.hasAttribute(A_style)) {
                QString style = elem.attribute(A_style);
                int commentPos = 0;
                while (commentPos != -1) {
                    commentPos = style.indexOf("/*");
                    if (commentPos != -1)
                        style.remove(commentPos, style.indexOf("*/") - commentPos + 2);
                }
                style.remove("\n");
                StringHash hash = splitStyle(style);
                foreach (const QString &key, hash.keys()) {
                    // ignore attributes like "-inkscape-font-specification"
                    // qt render prefer property attributes instead of A_style attribute
                    if (key.at(0) != signChar)
                        elem.setAttribute(key, hash.value(key));
                }
                elem.removeAttribute(A_style);
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
        elem.setAttribute(A_style, attrs.join(";"));
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
        if (currElem.hasAttribute(A_xlink_href))
            linkList << currElem.xlinkId();

    }
    list = svgElement().childElements();
    QStringList attrList = QStringList() << A_fill << A_stroke;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        foreach (const QString &attrName, attrList) {
            if (currElem.hasAttribute(attrName)) {
                QString id = currElem.defIdFromAttribute(attrName);
                if (!id.isEmpty())
                    linkList << id;
            }
        }
        if (currElem.hasChildElement())
            list << currElem.childElements();
    }

    list = defsElement().childElements();
    StringHash xlinkHash;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if ((currElem.tagName() == E_radialGradient || currElem.tagName() == E_linearGradient)
                && currElem.hasAttribute(A_xlink_href) && !currElem.hasChildElement()) {
            QString currLink = currElem.xlinkId();
            if (linkList.count(currLink) == 1) {
                SvgElement lineGradElem = findElement(currLink);
                if (!lineGradElem.isNull()) {
                    if (lineGradElem.hasChildElement()) {
                        foreach (const SvgElement &elem, lineGradElem.childElements())
                            currElem.appendChild(elem);
                        xlinkHash.insert(lineGradElem.id(), currElem.id());
                        defsElement().removeChild(lineGradElem);
                    }
                }
            }
        }
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
void Replacer::mergeGradientsWithEqualStopElem()
{
    SvgElementList list = defsElement().childElements();
    QList<LineGradStruct> lineGradList;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if ((currElem.tagName() == E_linearGradient || currElem.tagName() == E_radialGradient)
            && currElem.hasChildElement()) {
            LineGradStruct lgs;
            lgs.elem = currElem;
            lgs.id = currElem.id();
            lgs.attrs = currElem.attributesHash(true);
            foreach (SvgElement stopElem, currElem.childElements())
                lgs.stopAttrs << stopElem.attributesHash(true);
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
                    lgs2.elem.setAttribute(A_xlink_href, "#" + lgs1.id);
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
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          elem.attribute(A_x)
                                  + " " + elem.attribute(A_y)
                                  + " " + elem.attribute(A_width)
                                  + " " + elem.attribute(A_height));
        } else if (elem.tagName() == E_circle) {
            qreal r = elem.doubleAttribute(A_r);
            qreal x = elem.doubleAttribute(A_cx) - r;
            qreal y = elem.doubleAttribute(A_cy) - r;
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          roundNumber(x)
                                  + " " + roundNumber(y)
                                  + " " + roundNumber(qAbs(r*2))
                                  + " " + roundNumber(qAbs(r*2)));
        } else if (elem.tagName() == E_ellipse) {
            qreal rx = elem.doubleAttribute(A_rx);
            qreal ry = elem.doubleAttribute(A_ry);
            qreal x = elem.doubleAttribute(A_cx) - rx;
            qreal y = elem.doubleAttribute(A_cy) - ry;
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          roundNumber(x)
                                  + " " + roundNumber(y)
                                  + " " + roundNumber(qAbs(rx*2))
                                  + " " + roundNumber(qAbs(ry*2)));
        }
        // all other basic shapes bounding boxes are calculated in Paths class

        if (elem.hasChildElement())
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
//       demo.svg
// TODO: group non successively used attributes
//       Anonymous_City_flag_of_Gijon_Asturies_Spain.svg
// TODO: group tspan styles to text element
void Replacer::groupElementsByStyles(SvgElement parentElem)
{
    // first start
    if (parentElem.isNull())
        parentElem = svgElement();

    StringHash groupHash;
    SvgElementList similarElemList;
    QStringList additionalAttrList;
    additionalAttrList << "text-align" << "line-height" << A_font;
    QStringList ignoreAttrList;
    ignoreAttrList << A_clip_path << A_mask << A_filter << A_opacity;
    SvgElementList list = parentElem.childElements();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.isGroup() || currElem.tagName() == E_flowRoot)
            groupElementsByStyles(currElem);

        if (groupHash.isEmpty()) {
            // get hash of all style attributes of element
            groupHash = currElem.styleHash();
            foreach (const QString &attrName, additionalAttrList) {
                if (currElem.hasAttribute(attrName))
                    groupHash.insert(attrName, currElem.attribute(attrName));
            }
            if (    currElem.hasAttribute(A_transform)
                && !currElem.hasLinkedDef()
                && !parentElem.hasLinkedDef()) {
                groupHash.insert(A_transform, currElem.attribute(A_transform));
            }

            // we can not group elements by some attributes
            foreach (const QString &attrName, ignoreAttrList) {
                if (groupHash.contains(attrName))
                    groupHash.remove(attrName);
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
            StringHash lastGroupHash = groupHash;
            // remove attributes which is not exist or different in next element
            foreach (const QString &attrName, groupHash.keys()) {
                if (!currElem.hasAttribute(attrName))
                    groupHash.remove(attrName);
                else if (currElem.attribute(attrName) != groupHash.value(attrName))
                    groupHash.remove(attrName);
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
                        else if (parentElem.tagName() == E_svg && !lastGroupHash.contains(A_transform))
                            canUseParent = true;
                    }

                    SvgElement parentGElem;
                    if (canUseParent) {
                        parentGElem = parentElem;
                    } else {
                        parentGElem = document().createElement(E_g);
                        parentGElem = parentElem.insertBefore(parentGElem, similarElemList.first()).toElement();
                    }
                    // move equal style attributes of selected elements to parent group
                    foreach (const QString &attrName, lastGroupHash.keys()) {
                        if (parentGElem.hasAttribute(attrName) && attrName == A_transform) {
                            Transform ts(parentGElem.attribute(A_transform) + " "
                                         + lastGroupHash.value(attrName));
                            parentGElem.setAttribute(A_transform, ts.simplified());
                        } else {
                            parentGElem.setAttribute(attrName, lastGroupHash.value(attrName));
                        }
                    }
                    // move elem to group
                    foreach (SvgElement similarElem, similarElemList) {
                        foreach (const QString &attrName, lastGroupHash.keys())
                            similarElem.removeAttribute(attrName);
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
                    groupElementsByStyles(parentGElem);
                }
                similarElemList.clear();
                if (!currElem.tagName().isEmpty() && !currElem.isGroup())
                    list.prepend(currElem);
            }
        }
    }
}

void Replacer::markUsedElements()
{
    StringSet usedElemList;
    SvgElement elem = svgElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        if (elem.tagName() == E_use || elem.tagName() == E_textPath) {
            if (elem.hasAttribute(A_xlink_href))
                usedElemList << elem.xlinkId();
        }
        nextElement(elem, root);
    }

    elem = svgElement();
    while (!elem.isNull()) {
        QString id = elem.id();
        if (!id.isEmpty()) {
            if (usedElemList.contains(id))
                elem.setAttribute(CleanerAttr::UsedElement, "1");
        }
        nextElement(elem, root);
    }
}

QHash<QString,int> Replacer::calcDefsUsageCount()
{
    QStringList attrList;
    attrList << A_fill << A_filter << A_stroke;
    QHash<QString,int> idHash;
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        for (int i = 0; i < attrList.size(); ++i) {
            QString id = elem.defIdFromAttribute(attrList.at(i));
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

        if (elem.hasChildElement())
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
            if (elem.hasAttribute(A_gradientTransform)) {
                Transform gts(elem.attribute(A_gradientTransform));
                if (gts.isProportionalScale()) {
                    gts.setOldXY(elem.doubleAttribute(A_x1),
                                 elem.doubleAttribute(A_y1));
                    elem.setAttribute(A_x1, roundNumber(gts.newX()));
                    elem.setAttribute(A_y1, roundNumber(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute(A_x2),
                                 elem.doubleAttribute(A_y2));
                    elem.setAttribute(A_x2, roundNumber(gts.newX()));
                    elem.setAttribute(A_y2, roundNumber(gts.newY()));
                    elem.removeAttribute(A_gradientTransform);
                }
            }
        } else if (elem.tagName() == E_radialGradient) {
            if (elem.hasAttribute(A_gradientTransform)) {
                Transform gts(elem.attribute(A_gradientTransform));
                if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                    gts.setOldXY(elem.doubleAttribute(A_fx),
                                 elem.doubleAttribute(A_fy));
                    if (elem.hasAttribute(A_fx))
                        elem.setAttribute(A_fx, roundNumber(gts.newX()));
                    if (elem.hasAttribute(A_fy))
                        elem.setAttribute(A_fy, roundNumber(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute(A_cx),
                                 elem.doubleAttribute(A_cy));
                    elem.setAttribute(A_cx, roundNumber(gts.newX()));
                    elem.setAttribute(A_cy, roundNumber(gts.newY()));

                    elem.setAttribute(A_r, roundNumber(elem.doubleAttribute(A_r)
                                                              * gts.scaleFactor()));
                    elem.removeAttribute(A_gradientTransform);
                }
            }
        }
        if (elem.hasChildElement())
            list << elem.childElements();
    }
}

void Replacer::applyTransformToShapes()
{
    QHash<QString,int> defsHash = calcDefsUsageCount();

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        // TODO: too many firstChildElement
        if (   elem.isGroup()
            && elem.hasAttribute(A_transform)
            && !elem.hasAttribute(A_clip_path)
            && !elem.hasAttribute(A_mask)
            && !elem.hasAttribute(A_filter)
            && elem.childElementCount() == 1
            && !elem.isUsed()
            && elem.firstChildElement().tagName() != E_use
            && !elem.firstChildElement().isGroup()
            && !elem.firstChildElement().isUsed()
            && !elem.firstChildElement().hasAttribute(A_clip_path))
        {
            elem.firstChildElement().setTransform(elem.attribute(A_transform), true);
            elem.removeAttribute(A_transform);
        }
        else if (   elem.tagName() == E_rect
                 && elem.hasAttribute(A_transform)
                 && !elem.hasAttribute(A_clip_path)
                 && !elem.hasAttribute(A_mask)
                 && defsHash.value(elem.defIdFromAttribute(A_filter)) < 2
                 && defsHash.value(elem.defIdFromAttribute(A_stroke)) < 2
                 && defsHash.value(elem.defIdFromAttribute(A_fill)) < 2)
        {
            bool canApplyTransform = true;
            if (elem.hasAttribute(A_fill)) {
                SvgElement fillDef = findInDefs(elem.defIdFromAttribute(A_fill));
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (elem.hasAttribute(A_stroke)) {
                SvgElement fillDef = findInDefs(elem.defIdFromAttribute(A_stroke));
                if (!fillDef.isNull() && fillDef.tagName() == E_pattern)
                    canApplyTransform = false;
            }
            if (canApplyTransform) {
                Transform ts(elem.attribute(A_transform));
                if (   !ts.isMirrored()
                    && !ts.isRotating()
                    && !ts.isSkew()
                    &&  ts.isProportionalScale()) {
                    ts.setOldXY(elem.doubleAttribute(A_x), elem.doubleAttribute(A_y));
                    elem.setAttribute(A_x, roundNumber(ts.newX()));
                    elem.setAttribute(A_y, roundNumber(ts.newY()));
                    QString newW = roundNumber(elem.doubleAttribute(A_width) * ts.scaleFactor());
                    elem.setAttribute(A_width, newW);
                    QString newH = roundNumber(elem.doubleAttribute(A_height) * ts.scaleFactor());
                    elem.setAttribute(A_height, newH);
                    QString newRx = roundNumber(elem.doubleAttribute(A_rx) * ts.scaleFactor());
                    elem.setAttribute(A_rx, newRx);
                    QString newRy = roundNumber(elem.doubleAttribute(A_ry) * ts.scaleFactor());
                    elem.setAttribute(A_ry, newRy);
                    updateLinkedDefTransform(elem);
                    calcNewStrokeWidth(elem, ts);
                    elem.removeAttribute(A_transform);
                }
            }
        }

        if (elem.hasChildElement())
            list << elem.childElements();
    }
}

void Replacer::calcNewStrokeWidth(SvgElement &elem, const Transform &transform)
{
    SvgElement parentElem = elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(A_stroke_width)) {
            qreal strokeWidth = Tools::convertUnitsToPx(parentElem.attribute(A_stroke_width)).toDouble();
            QString sw = roundNumber(strokeWidth * transform.scaleFactor(), Round::Attribute);
            elem.setAttribute(A_stroke_width, sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentElement();
    }
    if (!hasParentStrokeWidth) {
        elem.setAttribute(A_stroke_width, roundNumber(transform.scaleFactor(),
                                                                   Round::Attribute));
    }
}
