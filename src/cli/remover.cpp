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

#include "remover.h"

// TODO: remove elements not included in selected spec, like flow* elements
// TODO: remove "tspan" without attributes
// TODO: remove "symbol"
// TODO: remove elem from defs if it used only by one use elem

// TODO: remove elements covered by other elements
// Leomarc_sign_clearway_1.svg

void Remover::cleanSvgElementAttribute()
{
    bool isXlinkUsed = false;

    element_loop(svgElement()) {
        if (elem.hasAttribute(A_xlink_href)) {
            isXlinkUsed = true;
            break;
        }
        nextElement(elem, root);
    }

    if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
        StringSet ignoreAttr = Properties::presentationAttributes;
        ignoreAttr << "xmlns" << A_width << A_height << A_viewBox;

        if (!Keys.flag(Key::RemoveSvgVersion))
            ignoreAttr << "version";
        if (isXlinkUsed)
            ignoreAttr << "xmlns:xlink";
        if (!Keys.flag(Key::RemoveInkscapeAttributes))
            ignoreAttr << "xmlns:inkscape";
        if (!Keys.flag(Key::RemoveSodipodiAttributes))
            ignoreAttr << "xmlns:sodipodi";
        if (!Keys.flag(Key::RemoveAdobeAttributes))
            ignoreAttr << "xmlns:x" << "xmlns:i" << "xmlns:graph";
        if (!Keys.flag(Key::RemoveCorelDrawAttributes))
            ignoreAttr << "xmlns:odm" << "xmlns:corel-charset";
        if (!Keys.flag(Key::RemoveMSVisioAttributes))
            ignoreAttr << "xmlns:v";
        if (!Keys.flag(Key::RemoveSketchAttributes))
            ignoreAttr << "xmlns:sketch";
        foreach (const QString &attrName, svgElement().attributesList()) {
            if (!ignoreAttr.contains(attrName))
                svgElement().removeAttribute(attrName);
        }
    } else {
        if (Keys.flag(Key::RemoveSvgVersion))
            svgElement().removeAttribute("version");
    }

    // fix missing or wrong xmlns:xlink attribute
    if (isXlinkUsed
        && (!svgElement().hasAttribute("xmlns:xlink")
            || svgElement().attribute("xmlns:xlink") != "http://www.w3.org/1999/xlink"))
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (Keys.flag(Key::RemoveDefaultAttributes))
        svgElement().removeAttributeIf(A_display, "inline");
    // TODO: add default attributes removing
}

void Remover::removeUnusedDefs()
{
    StringSet defsIdList;
    foreach (const SvgElement &elem, defsElement().childElements())
        if (elem.tagName() != E_clipPath)
            defsIdList << elem.id();

    StringSet currDefsIdList = defsIdList;
    while (!currDefsIdList.isEmpty()) {
        currDefsIdList = defsIdList;
        QList<SvgElement> list = svgElement().childElements();
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            if (elem.hasAttribute(A_xlink_href))
                currDefsIdList.remove(elem.xlinkId());
            foreach (const QString &attrName, Properties::linkableStyleAttributes) {
                if (elem.hasAttribute(attrName)) {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(UrlPrefix))
                        currDefsIdList.remove(url.mid(5, url.size()-6));
                }
            }
            if (elem.hasChildElement())
                list << elem.childElements();
        }

        foreach (const SvgElement &elem, defsElement().childElements()) {
            if (currDefsIdList.contains(elem.id())) {
                defsIdList.remove(elem.id());
                defsElement().removeChild(elem);
            }
        }
    }
}

void Remover::removeUnusedXLinks()
{
    QStringList xlinkStyles;
    xlinkStyles << A_fill << A_stroke << A_filter << A_clip_path << A_xlink_href;

    StringSet xlinkSet;
    StringSet idSet;
    SvgElement elem = svgElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        foreach (const QString &attrName, xlinkStyles) {
            if (elem.hasAttribute(attrName)) {
                if (QString(attrName) == A_xlink_href) {
                    if (!elem.attribute(A_xlink_href).startsWith(QL1S("data")))
                        xlinkSet << elem.xlinkId();
                } else {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(UrlPrefix))
                        xlinkSet << url.mid(5, url.size()-6); // remove url(# and )
                }
            }
        }
        if (elem.hasAttribute(A_id))
            idSet << elem.id();

        nextElement(elem, root);
    }
    foreach (const QString &id, idSet)
        xlinkSet.remove(id);

    elem = svgElement();
    root = elem;
    while (!elem.isNull()) {
        foreach (const QString &attrName, xlinkStyles) {
            if (elem.hasAttribute(attrName)) {
                if (attrName == A_xlink_href) {
                    if (xlinkSet.contains(elem.xlinkId()))
                        elem.removeAttribute(attrName);
                } else {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(UrlPrefix)) {
                        if (xlinkSet.contains(url.mid(5, url.size()-6)))  // remove url(# and )
                            elem.removeAttribute(attrName);
                    }
                }
            }
        }
        nextElement(elem, root);
    }
}

// TODO: refract this func
void Remover::removeDuplicatedDefs()
{
    StringHash xlinkToReplace;
    SvgElementList defsList = defsElement().childElements();

    // using of structure is faster than actual node accessing
    QList<DefsElemStruct> elemStructList;
    for (int i = 0; i < defsList.count(); ++i) {
        SvgElement elem = defsList.at(i);
        QString tagName = elem.tagName();

        if (tagName == E_linearGradient || tagName == E_radialGradient
                || tagName == E_filter || tagName == E_clipPath) {
            StringHash map = elem.attributesHash(true);

            // prepare attributes
            if (tagName == E_linearGradient) {
                if (   !map.contains(A_x1) && isZero(elem.doubleAttribute(A_x2))
                    && !map.contains(A_y1) && !map.contains(A_y2)) {
                    map.remove(A_gradientTransform);
                }
                if (map.contains(A_gradientTransform)) {
                    Transform gts(map.value(A_gradientTransform));
                    if (!gts.isMirrored() && gts.isProportionalScale()) {
                        gts.setOldXY(map.value(A_x1).toDouble(),
                                     map.value(A_y1).toDouble());
                        map.insert(A_x1, roundNumber(gts.newX()));
                        map.insert(A_y1, roundNumber(gts.newY()));
                        gts.setOldXY(map.value(A_x2).toDouble(),
                                     map.value(A_y2).toDouble());
                        map.insert(A_x2, roundNumber(gts.newX()));
                        map.insert(A_y2, roundNumber(gts.newY()));
                        map.remove(A_gradientTransform);
                    } else {
                        map.insert(A_gradientTransform, gts.simplified());
                    }
                }
                if (map.contains(A_x2) || map.contains(A_y2)) {
                    if (isZero(map.value(A_x1).toDouble() - map.value(A_x2).toDouble())) {
                        map.remove(A_x1);
                        map.insert(A_x2, V_null);
                    }
                    if (isZero(map.value(A_y1).toDouble() - map.value(A_y2).toDouble())) {
                        map.remove(A_y1);
                        map.remove(A_y2);
                    }
                }
            } else if (tagName == E_radialGradient) {
                if (map.contains(A_gradientTransform)) {
                    Transform gts(elem.attribute(A_gradientTransform));
                    if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                        gts.setOldXY(map.value(A_fx).toDouble(),
                                     map.value(A_fy).toDouble());
                        if (map.contains(A_fx))
                            map.insert(A_fx, roundNumber(gts.newX()));
                        if (map.contains(A_fy))
                            map.insert(A_fy, roundNumber(gts.newY()));
                        gts.setOldXY(map.value(A_cx).toDouble(),
                                     map.value(A_cy).toDouble());
                        map.insert(A_cx, roundNumber(gts.newX()));
                        map.insert(A_cy, roundNumber(gts.newY()));

                        map.insert(A_r, roundNumber(map.value(A_r).toDouble()
                                                                  * gts.scaleFactor()));
                        map.remove(A_gradientTransform);
                    } else {
                        map.insert(A_gradientTransform, gts.simplified());
                    }
                }
                qreal fx = map.value(A_fx).toDouble();
                qreal fy = map.value(A_fy).toDouble();
                qreal cx = map.value(A_cx).toDouble();
                qreal cy = map.value(A_cy).toDouble();
                if (isZero(qAbs(fx-cx)))
                    map.remove(A_fx);
                if (isZero(qAbs(fy-cy)))
                    map.remove(A_fy);
            }

            QList<StringHash> stopAttrs;
            SvgElement stopChild = elem.firstChildElement();
            while (!stopChild.isNull()) {
                stopAttrs << stopChild.attributesHash(true);
                stopChild = stopChild.nextSiblingElement();
            }

            static const QStringList linearGradient = QStringList()
                << A_gradientTransform << A_xlink_href << A_x1 << A_y1 << A_x2 << A_y2
                << A_gradientUnits << A_spreadMethod << A_externalResourcesRequired;

            static const QStringList radialGradient = QStringList()
                << A_gradientTransform << A_xlink_href << A_cx << A_cy << A_r << A_fx << A_fy
                << A_gradientUnits << A_spreadMethod << A_externalResourcesRequired;

            const QStringList filterList = QStringList()
                << A_gradientTransform << A_xlink_href << A_x << A_y << A_width << A_height
                << QL1S("filterRes") << QL1S("filterUnits") << A_primitiveUnits
                << A_externalResourcesRequired;

            QStringList tmpList;
            if (tagName == E_filter) {
                tmpList.reserve(filterList.size());
                foreach (const QString &attrName, filterList) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            } else if (tagName == E_linearGradient) {
                tmpList.reserve(linearGradient.size());
                foreach (const QString &attrName, linearGradient) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            } else if (tagName == E_radialGradient) {
                tmpList.reserve(radialGradient.size());
                foreach (const QString &attrName, radialGradient) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            }

            DefsElemStruct es = { elem, tagName, elem.hasChildElement(), map, tmpList, elem.id() };
            elemStructList << es;
        }
    }

    // process gradients
    for (int i = 0; i < elemStructList.count(); ++i) {
        DefsElemStruct des1 = elemStructList.at(i);
        QString id1 = des1.id;
        if (des1.tagName == E_linearGradient || des1.tagName == E_radialGradient) {
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                QString id2 = des2.id;
                if (des1.tagName == des2.tagName && id1 != id2) {
                    if ((des1.hasChildren && des2.hasChildren)
                        || (!des1.hasChildren && !des2.hasChildren))
                    {
                        if (des1.attrList == des2.attrList) {
                            bool isRemove = true;
                            if (des1.hasChildren && des2.hasChildren)
                                isRemove = isGradientsEqual(des1.elem, des2.elem);
                            if (isRemove) {
                                if (xlinkToReplace.values().contains(id2)) {
                                    for (int k = 0; k < xlinkToReplace.keys().count(); ++k) {
                                        if (xlinkToReplace.value(xlinkToReplace.keys().at(k)) == id2)
                                            xlinkToReplace.insert(xlinkToReplace.keys().at(k), id1);
                                    }
                                }
                                xlinkToReplace.insert(id2, id1);
                                defsElement().removeChild(des2.elem);
                                elemStructList.removeAt(j);
                                for (int e = 0; e < elemStructList.size(); ++e) {
                                    if (elemStructList.at(e).attrMap.value(A_xlink_href).mid(1) == id2)
                                        elemStructList[e].attrMap.insert(A_xlink_href, "#" + id1);
                                }
                                j--;
                            }
                        }
                    }
                }
            }
        } else if (des1.tagName == E_filter) {
            // process feGaussianBlur filter
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                QString id2 = des2.id;
                if (des1.tagName == des2.tagName && id1 != id2
                    && des1.elem.childElementCount() == 1 && des2.elem.childElementCount() == 1)
                {
                    if (des1.attrList == des2.attrList) {
                        SvgElement child1 = des1.elem.firstChildElement();
                        SvgElement child2 = des2.elem.firstChildElement();
                        if (   child1.tagName() == E_feGaussianBlur
                            && child2.tagName() == E_feGaussianBlur) {
                            if (child1.attribute(A_stdDeviation)
                                    == child2.attribute(A_stdDeviation))
                            {
                                xlinkToReplace.insert(id2, id1);
                                defsElement().removeChild(des2.elem);
                                elemStructList.removeAt(j);
                                j--;
                            }
                        }
                    }
                }
            }
        } else if (des1.tagName == E_clipPath) {
            // process clipPath
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                QString id2 = des2.id;
                if (des1.tagName == des2.tagName && id1 != id2
                    && des1.elem.childElementCount() == 1 && des2.elem.childElementCount() == 1)
                {
                    SvgElement child1 = des1.elem.firstChildElement();
                    SvgElement child2 = des2.elem.firstChildElement();
                    if (child1.tagName() == child2.tagName()
                        && child1.tagName() == E_path
                        && child1.attribute(A_d) == child2.attribute(A_d))
                    {
                        if (child1.attribute(A_transform) == child2.attribute(A_transform)) {
                            xlinkToReplace.insert(id2, id1);
                            defsElement().removeChild(des2.elem);
                            elemStructList.removeAt(j);
                            j--;
                        } else {
                            child2.setTagName(E_use);
                            child2.removeAttribute(A_d);
                            child2.setAttribute(A_xlink_href, "#" + child1.id());
                            child1.setAttribute(CleanerAttr::UsedElement, "1");
                            if (child1.parentElement().tagName() == E_clipPath) {
                                SvgElement newUse = document().createElement(E_use);
                                newUse.setAttribute(A_xlink_href, "#" + child1.id());
                                newUse.setAttribute(A_transform, child1.attribute(A_transform));
                                child1.removeAttribute(A_transform);
                                des1.elem.insertBefore(newUse, child1);
                                defsElement().insertBefore(child1, des1.elem);
                            }
                        }
                    }
                }
            }
        }
    }

    updateXLinks(xlinkToReplace);
}

bool Remover::isGradientsEqual(const SvgElement &elem1, const SvgElement &elem2)
{
    if (elem1.childElementCount() != elem2.childElementCount())
        return false;

    static const StringSet stopAttributes = StringSet()
        << A_offset << A_stop_color << A_stop_opacity;

    SvgElement child1 = elem1.firstChildElement();
    SvgElement child2 = elem2.firstChildElement();
    while (!child1.isNull()) {
        if (child1.tagName() != child2.tagName())
            return false;

        foreach (const QString &attrName, stopAttributes) {
            if (child1.attribute(attrName) != child2.attribute(attrName))
                return false;
        }

        child1 = child1.nextSiblingElement();
        child2 = child2.nextSiblingElement();
    }

    return true;
}

void Remover::removeUnreferencedIds()
{
    // find
    StringSet m_allIdList;
    StringSet m_allLinkList;

    QStringList xlinkAttrList;
    xlinkAttrList << A_xlink_href << "inkscape:path-effect" << "inkscape:perspectiveID";

    StringSet urlAttrList = Properties::linkableStyleAttributes;

    element_loop(svgElement()) {
        QStringList attrList = elem.attributesList();

        // collect all id's
        if (attrList.indexOf(A_id) != -1)
            m_allIdList << elem.id();

        foreach (const QString &attr, xlinkAttrList) {
            if (attrList.indexOf(attr) != -1)
                m_allLinkList << elem.attribute(attr).remove(0,1);
        }

        foreach (const QString &attr, urlAttrList) {
            if (attrList.indexOf(attr) != -1) {
                QString attrValue = elem.attribute(attr);
                if (attrValue.contains(UrlPrefix)) {
                    attrValue = attrValue.mid(5, attrValue.size()-6);
                    m_allLinkList << attrValue;
                }
            }
        }

        nextElement(elem, root);
    }

    // remove all linked ids
    foreach (const QString &text, m_allLinkList)
        m_allIdList.remove(text);

    if (Keys.flag(Key::KeepNamedIds)) {
        // skip id's whithout digits
        foreach (const QString &text, m_allIdList) {
            if (!text.contains(QRegExp("\\d")))
                m_allIdList.remove(text);
        }
    }

    // remove attributes
    element_loop_next(svgElement()) {
        if (m_allIdList.contains(elem.id()))
            elem.removeAttribute(A_id);

        if (m_allIdList.contains(elem.attribute(A_clip_path)))
            elem.removeAttribute(A_id);

        nextElement(elem, root);
    }
}

void Remover::removeElements()
{
    static const QString metadata     = QL1S("metadata");
    static const QString sodipodi     = QL1S("sodipodi");
    static const QString inkscape     = QL1S("inkscape");
    static const QString inkscapePath = QL1S("inkscape:path-effect");
    static const QString adobePrefix  = QL1S("a:");
    static const QString illPrefix    = QL1S("i:");
    static const QString visioPrefix  = QL1S("v:");
    static const QString corelPrefix  = QL1S("c:");
    static const QString sketchPrefix = QL1S("sketch:");

    static const StringSet svgElementList = StringSet()
        << E_a << E_altGlyph << E_altGlyphDef << E_altGlyphItem << E_animate << E_animateColor
        << E_animateMotion << E_animateTransform << E_circle << E_clipPath << E_color_profile
        << E_cursor << E_defs << E_desc << E_ellipse << E_feBlend << E_feColorMatrix
        << E_feComponentTransfer << E_feComposite << E_feConvolveMatrix << E_feDiffuseLighting
        << E_feDisplacementMap << E_feDistantLight << E_feFlood
        << QL1S("feFuncA") << QL1S("feFuncB") << QL1S("feFuncG") << QL1S("feFuncR")
        << E_feGaussianBlur << E_feImage << E_feMerge << E_feMergeNode << E_feMorphology
        << E_feOffset << E_fePointLight << E_feSpecularLighting << E_feSpotLight << E_feTile
        << E_feTurbulence << E_filter << E_font << E_font_face << E_font_face_format << E_font_face_name
        << E_font_face_src << E_font_face_uri << E_foreignObject << E_g << E_glyph << E_glyphRef
        << E_hkern << E_image << E_line << E_linearGradient << E_marker << E_mask << E_metadata
        << E_missing_glyph << E_mpath << E_path << E_pattern << E_polygon << E_polyline
        << E_radialGradient << E_rect << E_script << E_set << E_stop << E_style << E_svg << E_switch
        << E_symbol << E_text << E_textPath << E_title << E_tref << E_flowRoot << E_flowRegion
        << E_flowPara << E_flowSpan << E_tspan << E_use << E_view << E_vkern;

    bool isAnyRemoved = true;
    while (isAnyRemoved) {
        isAnyRemoved = false;

        SvgNodeList nodeList = document().childNodes();
        while (!nodeList.isEmpty()) {
            SvgNode currNode = nodeList.takeFirst();
            SvgElement currElem = currNode.toElement();

            bool removeThisNode = false;
            if (!currElem.isNull()) {
                QString currTag = currElem.tagName();
                if (   (currElem.isContainer()
                        || currTag == E_flowRegion)
                    && !currElem.hasChildElement()
                    && currTag != E_glyph
                    && currTag != E_defs
                    && Keys.flag(Key::RemoveEmptyContainers))
                    removeThisNode = true;
                else if (currTag.contains(metadata)
                         && Keys.flag(Key::RemoveMetadata))
                    removeThisNode = true;
                else if (currTag.contains(sodipodi)
                         && Keys.flag(Key::RemoveSodipodiElements))
                    removeThisNode = true;
                else if (   currTag.contains(inkscape)
                         && currTag != inkscapePath
                         && Keys.flag(Key::RemoveInkscapeElements))
                    removeThisNode = true;
                else if ((   currTag.startsWith(adobePrefix)
                          || currTag.startsWith(illPrefix))
                         && Keys.flag(Key::RemoveAdobeElements))
                    removeThisNode = true;
                else if (currTag.startsWith(visioPrefix)
                         && Keys.flag(Key::RemoveMSVisioElements))
                    removeThisNode = true;
                else if (currTag.startsWith(corelPrefix)
                         && Keys.flag(Key::RemoveCorelDrawElements))
                    removeThisNode = true;
                else if (currTag.startsWith(sketchPrefix)
                         && Keys.flag(Key::RemoveSketchElements))
                    removeThisNode = true;
                else if (currTag == E_foreignObject
                         && Keys.flag(Key::RemoveInvisibleElements))
                    removeThisNode = true;
                else if (currTag == E_use
                         && (!currElem.hasAttribute(A_xlink_href) ||
                             findElement(currElem.xlinkId()).isNull())
                         && Keys.flag(Key::RemoveInvisibleElements))
                    removeThisNode = true;
                else if (!svgElementList.contains(currTag)
                         && !currNode.hasText()
                         && Keys.flag(Key::RemoveNonSvgElements))
                    removeThisNode = true;

                if (Keys.flag(Key::RemoveInvisibleElements)) {
                    if (currTag == E_title)
                        removeThisNode = true;
                    else if (currTag == E_desc)
                        removeThisNode = true;
                    else if (currTag == E_script)
                        removeThisNode = true;
                    else if ((     currTag == E_linearGradient
                                || currTag == E_radialGradient)
                             && !currElem.hasChildElement()
                             && !currElem.hasAttribute(A_xlink_href)) {
                        removeThisNode = true;
                    } else if (    currTag == E_image
                               && !currElem.attribute(A_xlink_href).startsWith(QL1S("data")))
                        removeThisNode = true;
                    else if (!currElem.isUsed() && isElementInvisible(currElem)
                             && !hasParent(currElem, E_defs))
                        removeThisNode = true;
                }
            }

            if (currNode.isComment()
                && Keys.flag(Key::RemoveComments)) {
                removeThisNode = true;
            } else if (currNode.isDeclaration()
                       && Keys.flag(Key::RemoveProcInstruction)) {
                removeThisNode = true;
            } else if (currNode.isText()
                       && isDoctype(currNode.toText().text())
                       && Keys.flag(Key::RemoveProlog)) {
                removeThisNode = true;
            }

            if (removeThisNode) {
                currNode.parentNode().removeChild(currNode);
                isAnyRemoved = true;
            }

            if (currNode.hasChildren())
                nodeList << currNode.childNodes();
        }
    }

    // TODO: add switch element ungroup
    // TODO: move to separate func
    qreal stdDevLimit = Keys.doubleNumber(Key::RemoveTinyGaussianBlur);
    if (stdDevLimit == 0)
        return;
    element_loop(defsElement()) {
        if (elem.tagName() == E_feGaussianBlur) {
            // FIXME: check for stdDeviation with transform of all linked element applied
            if (stdDevLimit != 0.0) {
                if (elem.parentElement().childElementCount() == 1) {
                    // 'stdDeviation' can contains not only one value
                    // we process it when it contains only one value
                    const QString stdDev = elem.attribute(A_stdDeviation);
                    if (!stdDev.contains(",") && !stdDev.contains(" ")) {
                        bool ok = true;
                        if (stdDev.toDouble(&ok) <= stdDevLimit) {
                            if (!ok) {
                                qFatal("Error: could not parse stdDeviation value: %s",
                                       qPrintable(stdDev));
                            }
                            SvgElement tElem = elem.parentElement().previousSiblingElement();
                            defsElement().removeChild(elem.parentElement());
                            elem = tElem;
                        }
                    }
                }
            }
        }
        nextElement(elem, root);
    }
}

bool Remover::isDoctype(const QString &str)
{
    if (str.isEmpty())
        return false;
    if (!str.startsWith(QL1C('<')))
        return false;
    if (str.startsWith(QL1S("<!DOCTYPE")))
        return true;
    if (str.startsWith(QL1S("<!ENTITY")))
        return true;
    if (str.startsWith(QL1S("]>")) || str.startsWith(QL1S("\n]>")))
        return true;
    return false;
}

void Remover::removeElementsFinal()
{
    if (!Keys.flag(Key::RemoveInvisibleElements))
        return;

    element_loop(document().documentElement()) {
        if (isElementInvisible2(elem)) {
            SvgElement tElem = prevElement(elem);
            elem.parentElement().removeChild(elem);
            elem = tElem;
        }
        nextElement(elem, root);
    }
}

bool Remover::isElementInvisible(SvgElement &elem)
{
    QString tagName = elem.tagName();
    //remove elements "rect", "pattern" and "image" with height or width <= 0
    if (tagName == E_rect || tagName == E_pattern || tagName == E_image) {
        if (elem.hasAttribute(A_width) && elem.hasAttribute(A_height)) {
            qreal width  = elem.doubleAttribute(A_width);
            qreal height = elem.doubleAttribute(A_height);
            if (width <= 0 || height <= 0)
                return true;
        }
    }

    // TODO: ungroup flowPara with only A_id attr
    // ryanlerch_OCAL_Introduction.svg
//    if (tagName == "flowPara") {
//        if (!elem.hasText() && !elem.hasChildren())
//            return true;
//    }

    if (isElementInvisible2(elem))
        return true;

    // remove elements with opacity=V_null
    if (elem.hasAttribute(A_opacity)) {
        if (elem.doubleAttribute(A_opacity) <= 0) {
            return true;
        }
    }

    // remove elements with "display=none"
    if (elem.hasAttribute(A_display)) {
        if (elem.attribute(A_display) == V_none)
            return true;
    }

    if (elem.hasAttribute(A_visibility)) {
        static const QString v_hidden = "hidden";
        if (elem.attribute(A_visibility) == v_hidden)
            return true;
    }

    // remove E_path elements with empty A_d attr
    if (tagName == E_path)
        if (elem.attribute(A_d).isEmpty())
            return true;

    // A negative value is an error. A value of zero disables rendering of this element.
    if (tagName == E_use) {
        if (elem.hasAttribute(A_width))
            if (elem.doubleAttribute(A_width) == 0)
                return true;
        if (elem.hasAttribute(A_height))
            if (elem.doubleAttribute(A_height) == 0)
                return true;
    }

    // remove "polygon", "polyline" elements with empty A_points attr
    if (tagName == E_polygon || tagName == E_polyline)
        if (elem.attribute(A_points).isEmpty())
            return true;

    // remove "circle" elements with A_r <= 0
    if (tagName == E_circle)
        if (elem.attribute(A_r).toDouble() <= 0)
            return true;

    // remove "ellipse" elements with "rx|ry" <= 0
    if (tagName == E_ellipse)
        if (   elem.attribute(A_rx).toFloat() <= 0
            || elem.attribute(A_ry).toFloat() <= 0)
            return true;

    // remove "switch" with no attributes or with only A_id attribute
    if (tagName == E_switch && !elem.hasChildElement()) {
        if (elem.attributesCount() == 0)
            return true;
        else if (elem.hasAttribute(A_id) && elem.attributesCount() == 1)
            return true;
    }

    return false;
}

bool Remover::isElementInvisible2(SvgElement &elem)
{
    // if 'fill' or 'stroke' linked to non existing define - set attribute value to 'none'
    if (elem.isContainer())
        return false;

    SvgElement parent = elem.parentElement();
    bool hasWrongParent = false;
    while (!parent.isNull()) {
        if (parent.tagName() == E_defs || parent.tagName() == E_switch) {
            hasWrongParent = true;
            break;
        }
        parent = parent.parentElement();
    }
    if (!hasWrongParent) {
        // elements with no 'fill' and 'stroke' are invisible
        if (    findAttribute(elem, A_fill) == V_none
            && (   findAttribute(elem, A_stroke) == V_none
                || findAttribute(elem, A_stroke).isEmpty())
            && !elem.isUsed()) {
            if (elem.hasAttribute(A_filter)) {
                SvgElement filterElem = findInDefs(elem.defIdFromAttribute(A_filter));
                if (filterElem.childElementCount() == 1) {
                    if (filterElem.firstChildElement().tagName() == E_feGaussianBlur) {
                        return true;
                    }
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

void Remover::removeAttributes()
{    
    SvgElement elem = document().documentElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        QStringList baseAttrList = elem.attributesList();
        if (!baseAttrList.isEmpty()) {
            QString tagName = elem.tagName();
            QStringList attrList = baseAttrList;
            // sodipodi:type="inkscape:offset" supported only by inkscape,
            // and its creates problems in other renders
            foreach (const QString &attrName, attrList) {
                if (attrName.startsWith(QL1S("inkscape")) && Keys.flag(Key::RemoveInkscapeAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith(QL1S("sodipodi")) && Keys.flag(Key::RemoveSodipodiAttributes))
                    attrList.removeOne(attrName);
                if ((attrName.startsWith(QL1S("i:")) || attrName.startsWith(QL1S("a:")))
                    && Keys.flag(Key::RemoveAdobeAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith(QL1S("v:")) && Keys.flag(Key::RemoveMSVisioAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith(QL1S("c:")) && Keys.flag(Key::RemoveCorelDrawAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith(QL1S("sketch:")) && Keys.flag(Key::RemoveSketchAttributes))
                    attrList.removeOne(attrName);
            }

            if (Keys.flag(Key::RemoveDefaultAttributes)) {
                if (elem.attribute(QL1S("spreadMethod")) == QL1S("pad"))
                    attrList.removeOne(QL1S("spreadMethod"));
                if (tagName == E_clipPath) {
                    if (elem.attribute(QL1S("clipPathUnits")) == QL1S("userSpaceOnUse"))
                        attrList.removeOne(QL1S("clipPathUnits"));
                }
            }
            if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
                if (attrList.contains(QL1S("desc")))
                    attrList.removeOne(QL1S("desc"));
                // xlink:href could not contains uri with spaces
                if (attrList.contains(A_xlink_href)) {
                    QString xlink = elem.attribute(A_xlink_href);
                    if (!xlink.startsWith(QL1S("data:"))) {
                        if (xlink.indexOf(QL1C(' ')) != -1)
                            attrList.removeOne(A_xlink_href);
                        else if (!Properties::elementsUsingXLink.contains(tagName))
                            attrList.removeOne(A_xlink_href);
                    }
                }
                if (tagName != E_svg) {
                    foreach (const QString &attrName, attrList) {
                        if (attrName.startsWith(QL1S("xmlns")))
                            attrList.removeOne(attrName);
                    }
                }

                if (attrList.contains(A_marker))
                    if (elem.attribute(A_marker) == V_none)
                        elem.removeAttribute(A_marker);

                // path inside clipPath needs to contains only d attribute
                if (tagName == E_path || tagName == E_use) {
                    QString parentTag = elem.parentElement().tagName();
                    if (parentTag == E_clipPath) {
                        foreach (const QString &attrName, attrList) {
                            bool removeAttr = true;
                            if (   attrName == A_d
                                || attrName == A_transform
                                || attrName == A_filter
                                || attrName == A_id)
                                removeAttr = false;
                            if (tagName == E_use
                                && attrName == A_xlink_href)
                                removeAttr = false;
                            if (removeAttr)
                                attrList.removeOne(attrName);
                        }
                    }
                }

                static QStringList strokeAndFill = QStringList() << A_fill << A_stroke;
                foreach (const QString &attrName, strokeAndFill) {
                    if (attrList.contains(attrName)) {
                        QString url = elem.attribute(attrName);
                        if (url.startsWith(UrlPrefix)) {
                            SvgElement defElem = findInDefs(url.mid(5, url.size()-6));
                            if (defElem.isNull())
                                elem.setAttribute(attrName, V_none);
                        }
                    }
                }

                // remove all text based attributes from non-text elements
                // FIXME: slow, check is child has actual text
                static QStringList textElemList;
                if (textElemList.isEmpty())
                    textElemList << E_text << E_tspan << E_flowRoot;
                if (!elem.hasText() && tagName != E_text
                    && !elem.hasTextChild())
                {
                    foreach (const QString &attrName, attrList) {
                        if (   attrName.contains(A_font)
                            || attrName.contains(A_text)
                            || Properties::textAttributes.contains(attrName))
                        {
                            attrList.removeOne(attrName);
                        }
                    }
                    attrList.removeOne(QL1S("writing-mode"));
                    attrList.removeOne(QL1S("line-height"));
                    attrList.removeOne(QL1S("block-progression"));
                }
            }

            if (attrList.contains(A_d)
                && tagName != E_path
                && tagName != E_glyph
                && tagName != E_missing_glyph)
                attrList.removeOne(A_d);

            if (tagName == E_stop) {
                if (!elem.attribute(A_offset).contains(LengthType::percent)) {
                    if (elem.doubleAttribute(A_offset) < 0.0001)
                        attrList.removeOne(A_offset);
                }
            }

            // TODO: 'display' attr remove

            foreach (const QString &attrName, baseAttrList) {
                if (attrList.indexOf(attrName) == -1)
                    elem.removeAttribute(attrName);
            }
        }

        nextElement(elem, root);
    }

    // remove xml:space when no child has multispace text
    elem = document().documentElement();
    root = elem;
    while (!elem.isNull()) {
        if (elem.hasAttribute(QL1S("xml:space"))) {
            bool canRemove = true;
            SvgElement elem2 = document().documentElement();
            SvgElement root2 = elem;
            while (!elem2.isNull()) {
                if (Properties::textElements.contains(elem2.tagName())) {
                    if (elem2.hasText()) {
                        QString text = elem2.text();
                        if (text.contains(QL1S("  ")) || text.startsWith(' ') || text.endsWith(' ')) {
                            canRemove = false;
                            break;
                        }
                    }
                }
                nextElement(elem2, root2);
            }
            if (canRemove)
                elem.removeAttribute(QL1S("xml:space"));
        }

        nextElement(elem, root);
    }

    removeNonElementAttributes();
}

void Remover::removeNonElementAttributes()
{
    StringSet circle;
    circle << A_transform << A_cx << A_cy << A_r << A_id;
    circle.unite(Properties::presentationAttributes);

    StringSet ellipse;
    ellipse << A_transform << A_cx << A_cy << A_rx << A_ry << A_id;
    ellipse.unite(Properties::presentationAttributes);

    element_loop(svgElement()) {
        QString tagName = elem.tagName();
        if (tagName == E_circle || tagName == E_ellipse) {
            foreach (const QString &attrName, elem.attributesList()) {
                if (!circle.contains(attrName) && tagName == E_circle)
                    elem.removeAttribute(attrName);
                else if (!ellipse.contains(attrName) && tagName == E_ellipse)
                    elem.removeAttribute(attrName);
            }
        }
        nextElement(elem, root);
    }
}

void Remover::cleanPresentationAttributes(SvgElement elem)
{
    if (elem.isNull())
        elem = svgElement();

    styleHashList << elem.styleHash();
    parentHash.unite(styleHashList.last());
    parentAttrs = parentHash.keys().toSet();

    SvgElement currElem = elem.firstChildElement();
    while (!currElem.isNull()) {
        StringHash hash = currElem.styleHash();
        cleanStyle(currElem, hash);
        foreach (const QString &attrName, currElem.styleAttributesList()) {
            if (hash.contains(attrName))
                currElem.setAttribute(attrName, hash.value(attrName));
            else
                currElem.removeAttribute(attrName);
            hash.remove(attrName);
        }
        foreach (const QString &attrName, hash.keys())
            currElem.setAttribute(attrName, hash.value(attrName));
        if (currElem.hasChildElement())
            cleanPresentationAttributes(currElem);
        currElem = currElem.nextSiblingElement();
    }

    styleHashList.removeLast();
    parentHash.clear();
    foreach (const StringHash &hash, styleHashList)
        parentHash.unite(hash);
}

// removes default value, only if parent style did't contain same attribute
// needed for all inherited attributes
void Remover::cleanStyle(const SvgElement &elem, StringHash &hash)
{
    static bool isRemoveNotApplied = Keys.flag(Key::RemoveNotAppliedAttributes);
    static bool isConvertColors
            = (Keys.flag(Key::ConvertColorToRRGGBB) || Keys.flag(Key::ConvertRRGGBBToRGB));


    if (isRemoveNotApplied) {
        // remove style props which already defined in parent style
        foreach (const QString &attr, parentHash.keys()) {
            if (attr != A_opacity && !elem.isUsed()) {
                if (hash.contains(attr))
                    if (hash.value(attr) == parentHash.value(attr))
                        hash.remove(attr);
            }
        }
    }

    // convert units
    static QStringList numericStyleList
            = QStringList() << A_fill_opacity << A_opacity << A_stop_opacity << A_stroke_miterlimit
                            << A_stroke_opacity << A_stroke_width << A_font_size << A_kerning
                            << A_letter_spacing << A_word_spacing << A_baseline_shift
                            << A_stroke_dashoffset;
    foreach (const QString &key, numericStyleList) {
        QString value = hash.value(key);
        if (!value.isEmpty()) {
            bool ok = false;
            qreal num = value.toDouble(&ok);
            if (!ok && !value.startsWith(UrlPrefix)) {
                if (key == A_stroke_width) {
                    if (value.endsWith(LengthType::percent)) {
                        static QRectF m_viewBoxRect = viewBoxRect();
                        if (m_viewBoxRect.isNull())
                            qFatal("Error: could not detect viewBox");
                        hash.insert(key, Tools::convertUnitsToPx(value, m_viewBoxRect.width()));
                    }
                    else if (value.endsWith(LengthType::em) || value.endsWith(LengthType::ex)) {
                        QString fontSizeStr = findAttribute(elem, A_font_size);
                        qreal fontSize = Tools::convertUnitsToPx(fontSizeStr).toDouble();
                        if (fontSize == 0)
                            qFatal("Error: could not convert em/ex values "
                                   "without font-size attribute is set.");
                        hash.insert(key, Tools::convertUnitsToPx(value, fontSize));
                    } else {
                        hash.insert(key, Tools::convertUnitsToPx(value));
                    }
                } else
                    hash.insert(key, Tools::convertUnitsToPx(value));
                num = hash.value(key).toDouble();
            }
            hash.insert(key, roundNumber(num, Round::Attribute));
        }
    }

    // remove all fill properties if fill is off
    if (Keys.flag(Key::RemoveFillProps)
        && parentAttrs.contains(A_fill)
        && parentHash.value(A_fill) == V_none
        && (hash.value(A_fill) == V_none || hash.value(A_fill_opacity) == V_null))
    {
        static const StringSet fillList = StringSet() << A_fill << A_fill_rule << A_fill_opacity;
        foreach (const QString &attr, fillList)
            hash.remove(attr);
    } else if (isConvertColors) {
        QString fill = hash.value(A_fill);
        if (!fill.isEmpty() && fill != V_none && !fill.startsWith(UrlPrefix))
            hash.insert(A_fill, Tools::trimColor(fill));
    }

    // remove all stroke properties if stroke is off
    if (Keys.flag(Key::RemoveStrokeProps)
        && (   hash.value(A_stroke) == V_none
            || hash.value(A_stroke_opacity) == V_null
            || hash.value(A_stroke_width) == V_null)) {
        static const StringSet strokeList = StringSet()
             << A_stroke << A_stroke_width << A_stroke_linecap << A_stroke_linejoin
             << A_stroke_miterlimit << A_stroke_dasharray << A_stroke_dashoffset << A_stroke_opacity;
        foreach (const QString &attr, strokeList)
            hash.remove(attr);
        if (parentHash.value(A_stroke) != V_none)
            hash.insert(A_stroke, V_none);
    } else {
        if (isConvertColors) {
            QString stroke = hash.value(A_stroke);
            if (   !stroke.isEmpty()
                &&  stroke != V_none
                && !stroke.startsWith(UrlPrefix))
            {
                hash.insert(A_stroke, Tools::trimColor(hash.value(A_stroke)));
            }
        }
        // trim array
        if (hash.contains(A_stroke_dasharray))
            hash.insert(A_stroke_dasharray, QString(hash.value(A_stroke_dasharray))
                                             .replace(", ", ","));
    }

    if (isRemoveNotApplied) {
        static const QString a_pointer_events = QL1S("pointer-events");
        hash.remove(a_pointer_events);

        // remove clip-rule if elem not inside clipPath
        if (hash.contains(A_clip_rule)) {
            bool isElemInsideClipPath = false;
            SvgElement parent = elem.parentElement();
            while (!parent.isNull()) {
                if (parent.tagName() == E_clipPath) {
                    isElemInsideClipPath = true;
                    break;
                }
                parent = parent.parentElement();
            }
            if (!isElemInsideClipPath)
                hash.remove(A_clip_rule);
        }

        // 'enable-background' is only applicable to container elements
        if (!elem.isContainer())
            hash.remove(A_enable_background);

        if (elem.tagName() != E_svg && elem.tagName() != E_pattern && elem.tagName() != E_marker)
            hash.remove(A_overflow);
    }

    if (Keys.flag(Key::RemoveDefaultAttributes)) {
        foreach (const QString &attrName, hash.keys())
            removeDefaultValue(hash, attrName);
    }

    // trim colors
    if (isConvertColors) {
        foreach (const QString &attrName, QStringList() << A_color << A_stop_color << A_flood_color) {
            if (hash.contains(attrName))
                hash.insert(attrName, Tools::trimColor(hash.value(attrName)));
        }
    }
}

void Remover::removeDefaultValue(StringHash &hash, const QString &name)
{
    if (parentAttrs.contains(name))
        return;

    static const QVariantHash defaultStyleValues = initDefaultStyleHash();

    if (name == A_fill || name == A_stop_color) {
        static QStringList defValues = QStringList() << "#000" << "#000000" << "black";
        if (defValues.contains(hash.value(name)))
            hash.remove(name);
    } else if (defaultStyleValues.contains(name)) {
        const QVariant value = defaultStyleValues.value(name);
        if (value.type() == QVariant::String) {
            if (value == hash.value(name))
                hash.remove(name);
        } else if (!hash.value(name).isEmpty()
                   && strToDouble(hash.value(name)) == value.toDouble()) {
            hash.remove(name);
        }
    }
}

// TODO: works bad on applications-education-school.svg
// FIXME: rewrite
void Remover::removeGroups()
{
    QStringList illegalGAttrList;
    illegalGAttrList << A_mask << A_clip_path << A_filter;

    bool isAnyGroupRemoved = true;
    while (isAnyGroupRemoved) {
        isAnyGroupRemoved = false;
        SvgElementList list = svgElement().childElements();
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            SvgElement parent = elem.parentElement();
            if (elem.isGroup() && elem.childElementCount() == 0) {
                // remove empty group
                parent.removeChild(elem);
            } else if (   elem.isGroup()
                       && elem.parentElement().tagName() != E_switch)
            {
                if (   parent.isGroup()
                    && !elem.isUsed()
                    && !elem.hasAttributes(illegalGAttrList))
                {
                    // merge parent group with current group
                    SvgElement firstChild = elem.firstChildElement();
                    if (elem.childElementCount() == 1
                        && firstChild.tagName() != E_switch)
                    {
                        // ungroup group with only one child group
                        parent.insertBefore(firstChild, elem);
                        megreGroupWithChild(elem, firstChild, false);
                        parent.removeChild(elem);
                        isAnyGroupRemoved = true;
                    } else if (    parent.childElementCount() == 1
//                               &&  parent.tagName() != E_svg
                               && !parent.hasAttributes(illegalGAttrList))
                    {
                        // TODO: why this branch need for?
                        megreGroupWithChild(elem, parent, true);
                        foreach (const SvgElement &childElem, elem.childElements())
                            parent.insertBefore(childElem, elem);
                        parent.removeChild(elem);
                        isAnyGroupRemoved = true;
                    }
                } else {
                    bool isOnlyTransform = false;
                    int attrCount = elem.attributesCount();
                    if (   (   attrCount == 1
                            && elem.hasAttribute(A_transform))
                        || (   attrCount == 2
                            && elem.hasAttribute(A_transform)
                            && elem.hasAttribute(A_id)
                            && Keys.flag(Key::RemoveUnreferencedIds)))
                    {
                        int trAttrCount = 0;
                        foreach (const SvgElement &childElem, elem.childElements()) {
                            if (   childElem.isUsed()
                                || childElem.tagName() == E_use
                                || childElem.isGroup())
                                break;
                            if (childElem.hasAttribute(A_transform))
                                trAttrCount++;
                        }
                        if (trAttrCount == elem.childElementCount())
                            isOnlyTransform = true;
                    }

                    if (!elem.hasImportantAttrs() || isOnlyTransform) {
                        // ungroup group without attributes
                        QString parentTransfrom = elem.attribute(A_transform);
                        foreach (SvgElement childElem, elem.childElements()) {
                            if (isOnlyTransform)
                                childElem.setTransform(parentTransfrom, true);
                            parent.insertBefore(childElem, elem);
                        }
                        parent.removeChild(elem);
                        isAnyGroupRemoved = true;
                    }
                }
            }
            if (elem.hasChildElement())
                list << elem.childElements();
        }
    }
}

void Remover::megreGroupWithChild(SvgElement &groupElem, SvgElement &childElem, bool isParentToChild)
{
    QStringList ignoreAttrList = QStringList() << A_id;
    if (!isParentToChild)
        ignoreAttrList << A_stroke << A_stroke_width << A_fill;

    foreach (const QString &attrName, groupElem.attributesList()) {
        if (childElem.hasAttribute(attrName) && attrName == A_transform) {
            childElem.setTransform(groupElem.attribute(attrName), !isParentToChild);
        } else if (attrName == A_opacity) {
            if (groupElem.hasAttribute(A_opacity) && childElem.hasAttribute(A_opacity)) {
                qreal newOp =  groupElem.doubleAttribute(A_opacity)
                              * childElem.doubleAttribute(A_opacity);
                childElem.setAttribute(A_opacity, roundNumber(newOp));
            } else {
                childElem.setAttribute(attrName, groupElem.attribute(attrName));
            }
        } else if (!ignoreAttrList.contains(attrName) || !childElem.hasAttribute(attrName)) {
            childElem.setAttribute(attrName, groupElem.attribute(attrName));
        }
    }
}

void Remover::ungroupAElement()
{
    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == E_a && !elem.hasImportantAttrs()) {
            foreach (const SvgElement &childElem, elem.childElements())
                elem.parentElement().insertBefore(childElem, elem);
            elem.parentElement().removeChild(elem);
            elem.clear();
        }
        if (!elem.isNull())
            if (elem.hasChildElement())
                list << elem.childElements();
    }
}

void _setupTransformForBBox(const SvgElement &elem, const QStringList &trList)
{
    SvgElement child = elem.firstChildElement();
    while (!child.isNull()) {
        if (child.isContainer()) {
            QStringList tmpTrList = trList;
            if (child.hasAttribute(A_transform))
                tmpTrList << child.attribute(A_transform);
            _setupTransformForBBox(child, tmpTrList);
        } else if (child.hasAttribute(CleanerAttr::BoundingBox)) {
            QStringList tmpTrList = trList;
            if (child.hasAttribute(A_transform))
                tmpTrList << child.attribute(A_transform);
            child.setAttribute(CleanerAttr::BBoxTransform, tmpTrList.join(" "));
        } else if (child.hasChildElement())
            _setupTransformForBBox(child, trList);
        child = child.nextSiblingElement();
    }
}

void Remover::prepareViewBoxRect(QRectF &viewBox)
{
    if (svgElement().hasAttribute(A_width) || svgElement().hasAttribute(A_height)) {
        qreal w = viewBox.width();
        if (svgElement().hasAttribute(A_width))
            w = svgElement().doubleAttribute(A_width);

        qreal h = viewBox.height();
        if (svgElement().hasAttribute(A_height))
            h = svgElement().doubleAttribute(A_height);

        qreal vbAspect = viewBox.width()/viewBox.height();
        qreal aspect = w/h;

        QSizeF s(w, h);
        if ((aspect > 1.0 && aspect > vbAspect) || aspect > vbAspect) {
            qreal asp = viewBox.height()/h;
            s.scale(w * asp, h * asp, Qt::KeepAspectRatio);
        } else {
            qreal asp = viewBox.width()/w;
            s.scale(w * asp, h * asp, Qt::KeepAspectRatio);
        }
        viewBox.moveTo(viewBox.x() + (viewBox.width() - s.width()) / 2,
                       viewBox.y() + (viewBox.height() - s.height()) / 2);
        viewBox.setSize(s);
    }
}

// TODO: add blur support
void Remover::removeElementsOutsideTheViewbox()
{
    QRectF viewBox = viewBoxRect();
    if (viewBox.isNull())
        return;
    prepareViewBoxRect(viewBox);

    _setupTransformForBBox(svgElement(), QStringList());

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (   elem.hasAttribute(CleanerAttr::BoundingBox)
            && findAttribute(elem, CleanerAttr::UsedElement).isEmpty()
            && !hasParent(elem, E_defs)
            && !hasParent(elem, E_flowRegion)) {
            QStringList pList = elem.attribute(CleanerAttr::BoundingBox).split(" ");
            QRectF rect(strToDouble(pList.at(0)), strToDouble(pList.at(1)),
                        strToDouble(pList.at(2)), strToDouble(pList.at(3)));
            // fix rect's with zero area
            if (rect.width() == 0)
                rect.setWidth(1);
            if (rect.height() == 0)
                rect.setHeight(1);

            if (elem.hasAttribute(CleanerAttr::BBoxTransform)) {
                Transform tr(elem.attribute(CleanerAttr::BBoxTransform));
                rect = tr.transformRect(rect);
            }

            QString stroke = findAttribute(elem, A_stroke);
            if (!stroke.isEmpty() && stroke != V_none) {
                QString sws = findAttribute(elem, A_stroke_width);
                qreal sw = 1;
                if (!sws.isEmpty())
                    sw = sws.toDouble();
                rect.adjust(-sw/2, -sw/2, sw, sw);
            }

            if (!viewBox.intersects(rect)) {
                elem.parentElement().removeChild(elem);
                elem.clear();
            } else {
                elem.removeAttribute(CleanerAttr::BoundingBox);
                elem.removeAttribute(CleanerAttr::BBoxTransform);
            }
        } else if (elem.hasAttribute(CleanerAttr::BoundingBox)) {
            elem.removeAttribute(CleanerAttr::BoundingBox);
            elem.removeAttribute(CleanerAttr::BBoxTransform);
        }
        if (!elem.isNull())
            if (elem.hasChildElement())
                list << elem.childElements();
    }
}
