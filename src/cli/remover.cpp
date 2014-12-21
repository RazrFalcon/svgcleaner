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
// TODO: remove "symbol"
// TODO: remove elem from defs if it used only by one use elem

// TODO: remove elements covered by other elements
// Leomarc_sign_clearway_1.svg

void Remover::cleanSvgElementAttribute()
{
    if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
        StringSet ignoreAttr = Properties::presentationAttributes;
        ignoreAttr << "xmlns" << "xmlns:xlink" << A_width << A_height << A_viewBox;

        if (!Keys.flag(Key::RemoveSvgVersion))
            ignoreAttr << "version";
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

    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (Keys.flag(Key::RemoveDefaultAttributes))
        svgElement().removeAttributeIf(AttrId::display, "inline");
    // TODO: add default attributes removing
}

void Remover::checkXlinkDeclaration()
{
    bool isXlinkUsed = false;
    element_loop(svgElement()) {
        if (elem.hasAttribute(AttrId::xlink_href)) {
            isXlinkUsed = true;
            break;
        }
        nextElement(elem, root);
    }
    if (!isXlinkUsed) {
        svgElement().removeAttribute("xmlns:xlink");
        return;
    }

    // fix missing or wrong xmlns:xlink attribute
    if (svgElement().attribute("xmlns:xlink") != "http://www.w3.org/1999/xlink")
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
}

void Remover::removeUnusedDefs()
{
    StringSet defsIdList;

    SvgElement dElem = defsElement().firstChildElement();
    while (!dElem.isNull()) {
        // 'clipPath' can be unused, but elements in it can be used
        // so we need to check it too
        if (dElem.tagName() == E_clipPath && dElem.hasChildren()) {
            SvgElement child = dElem.firstChildElement();
            while (!child.isNull()) {
                defsIdList << child.id();
                child = child.nextSiblingElement();
            }
        }
        // remove defs with equal 'id' attribute
        if (defsIdList.contains(dElem.id())) {
            SvgElement tElem = dElem;
            dElem = dElem.previousSiblingElement();
            defsElement().removeChild(tElem);
        } else {
            defsIdList << dElem.id();
        }
        dElem = dElem.nextSiblingElement();
    }

    StringSet unusedIdList = defsIdList;
    while (!unusedIdList.isEmpty()) {
        unusedIdList = defsIdList;

        SvgElement elem = svgElement();
        SvgElement root = elem;
        while (!elem.isNull()) {
            if (elem.hasAttribute(AttrId::xlink_href))
                unusedIdList.remove(elem.xlinkId());
            foreach (const int &attrId, Properties::linkableStyleAttributesIds) {
                if (elem.hasAttribute(attrId)) {
                    QString url = elem.attribute(attrId);
                    if (url.startsWith(UrlPrefix))
                        unusedIdList.remove(url.mid(5, url.size()-6));
                }
            }
            nextElement(elem, root);
        }

        SvgElementList list = defsElement().childElements();
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            if (elem.hasChildren())
                list << elem.childElements();
            if (!unusedIdList.contains(elem.id()))
                continue;

            // ungroup 'clipPath' before remove
            if (elem.tagName() == E_clipPath) {
                foreach (const SvgElement &clipChild, elem.childElements())
                    list << defsElement().insertBefore(clipChild, elem.nextSibling()).toElement();
            }
            defsIdList.remove(elem.id());
            defsElement().removeChild(elem);
        }
    }
}

void Remover::removeUnusedXLinks()
{
    IntList xlinkStyles = IntList() << AttrId::fill << AttrId::stroke << AttrId::filter
                                    << AttrId::clip_path << AttrId::xlink_href;

    StringSet xlinkSet;
    StringSet idSet;
    SvgElement elem = svgElement();
    SvgElement root = elem;
    while (!elem.isNull()) {
        foreach (const int &attrId, xlinkStyles) {
            if (elem.hasAttribute(attrId)) {
                if (attrId == AttrId::xlink_href) {
                    if (!elem.attribute(AttrId::xlink_href).startsWith(QL1S("data")))
                        xlinkSet << elem.xlinkId();
                } else {
                    QString url = elem.attribute(attrId);
                    if (url.startsWith(UrlPrefix))
                        xlinkSet << url.mid(5, url.size()-6); // remove url(# and )
                }
            }
        }
        if (elem.hasAttribute(AttrId::id))
            idSet << elem.id();

        nextElement(elem, root);
    }
    foreach (const QString &id, idSet)
        xlinkSet.remove(id);

    elem = svgElement();
    root = elem;
    while (!elem.isNull()) {
        foreach (const int &attrId, xlinkStyles) {
            if (elem.hasAttribute(attrId)) {
                if (attrId == AttrId::xlink_href) {
                    if (xlinkSet.contains(elem.xlinkId()))
                        elem.removeAttribute(attrId);
                } else {
                    QString url = elem.attribute(attrId);
                    if (url.startsWith(UrlPrefix)) {
                        if (xlinkSet.contains(url.mid(5, url.size()-6)))  // remove url(# and )
                            elem.removeAttribute(attrId);
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
                if (   !map.contains(A_x1) && isZero(elem.doubleAttribute(AttrId::x2))
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
                    Transform gts(elem.attribute(AttrId::gradientTransform));
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

            DefsElemStruct es = { elem, tagName, elem.hasChildrenElement(), map, tmpList, elem.id() };
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
                            if (child1.attribute(AttrId::stdDeviation)
                                    == child2.attribute(AttrId::stdDeviation))
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
                        && child1.attribute(AttrId::d) == child2.attribute(AttrId::d))
                    {
                        if (child1.attribute(AttrId::transform) == child2.attribute(AttrId::transform)) {
                            xlinkToReplace.insert(id2, id1);
                            defsElement().removeChild(des2.elem);
                            elemStructList.removeAt(j);
                            j--;
                        } else {
                            child2.setTagName(E_use);
                            child2.removeAttribute(AttrId::d);
                            child2.setAttribute(AttrId::xlink_href, "#" + child1.id());
                            child1.setAttribute(AttrId::used_element, "1");
                            if (child1.parentElement().tagName() == E_clipPath) {
                                SvgElement newUse = document().createElement(E_use);
                                newUse.setAttribute(AttrId::xlink_href, "#" + child1.id());
                                newUse.setAttribute(AttrId::transform,
                                                    child1.attribute(AttrId::transform));
                                child1.removeAttribute(AttrId::transform);
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

    static const IntList stopAttributes = IntList()
        << AttrId::offset << AttrId::stop_color << AttrId::stop_opacity;

    SvgElement child1 = elem1.firstChildElement();
    SvgElement child2 = elem2.firstChildElement();
    while (!child1.isNull()) {
        if (child1.tagName() != child2.tagName())
            return false;

        foreach (const int &attrId, stopAttributes) {
            if (child1.attribute(attrId) != child2.attribute(attrId))
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

    QString pathEffect = QL1S("inkscape:path-effect");
    QString perspectiveID = QL1S("inkscape:perspectiveID");

    IntList urlAttrList = Properties::linkableStyleAttributesIds;

    element_loop(svgElement()) {
        // collect all id's
        if (elem.hasAttribute(AttrId::id))
            m_allIdList << elem.id();
        if (elem.hasAttribute(AttrId::xlink_href))
            m_allLinkList << elem.attribute(AttrId::xlink_href).remove(0,1);
        if (elem.hasExtAttribute(pathEffect))
            m_allLinkList << elem.extAttribute(pathEffect).remove(0,1);
        if (elem.hasExtAttribute(perspectiveID))
            m_allLinkList << elem.extAttribute(perspectiveID).remove(0,1);

        foreach (const int &attrId, urlAttrList) {
            if (elem.hasAttribute(attrId)) {
                QString attrValue = elem.attribute(attrId);
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
            elem.removeAttribute(AttrId::id);

        if (m_allIdList.contains(elem.attribute(AttrId::clip_path)))
            elem.removeAttribute(AttrId::id);

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
                    && !currElem.hasChildrenElement()
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
                         && (!currElem.hasAttribute(AttrId::xlink_href) ||
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
                             && !currElem.hasChildrenElement()
                             && !currElem.hasAttribute(AttrId::xlink_href)) {
                        removeThisNode = true;
                    } else if (    currTag == E_image
                               && !currElem.attribute(AttrId::xlink_href).startsWith(QL1S("data")))
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
                    const QString stdDev = elem.attribute(AttrId::stdDeviation);
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
        if (elem.hasAttribute(AttrId::width) && elem.hasAttribute(AttrId::height)) {
            qreal width  = elem.doubleAttribute(AttrId::width);
            qreal height = elem.doubleAttribute(AttrId::height);
            if (width <= 0 || height <= 0)
                return true;
        }
    }

    // TODO: ungroup flowPara with only id attr
    // ryanlerch_OCAL_Introduction.svg
//    if (tagName == "flowPara") {
//        if (!elem.hasText() && !elem.hasChildren())
//            return true;
//    }

    if (isElementInvisible2(elem))
        return true;

    // remove elements with opacity=V_null
    if (elem.hasAttribute(AttrId::opacity)) {
        if (elem.doubleAttribute(AttrId::opacity) <= 0) {
            return true;
        }
    }

    // remove elements with "display=none"
    if (elem.hasAttribute(AttrId::display)) {
        if (elem.attribute(AttrId::display) == V_none)
            return true;
    }

    if (elem.hasAttribute(AttrId::visibility)) {
        static const QString v_hidden = "hidden";
        if (elem.attribute(AttrId::visibility) == v_hidden)
            return true;
    }

    // remove E_path elements with empty A_d attr
    if (tagName == E_path)
        if (elem.attribute(AttrId::d).isEmpty())
            return true;

    // A negative value is an error. A value of zero disables rendering of this element.
    if (tagName == E_use) {
        if (elem.hasAttribute(AttrId::width))
            if (elem.doubleAttribute(AttrId::width) == 0)
                return true;
        if (elem.hasAttribute(AttrId::height))
            if (elem.doubleAttribute(AttrId::height) == 0)
                return true;
    }

    // remove "polygon", "polyline" elements with empty A_points attr
    if (tagName == E_polygon || tagName == E_polyline)
        if (elem.attribute(AttrId::points).isEmpty())
            return true;

    // remove "circle" elements with A_r <= 0
    if (tagName == E_circle)
        if (elem.attribute(AttrId::r).toDouble() <= 0)
            return true;

    // remove "ellipse" elements with "rx|ry" <= 0
    if (tagName == E_ellipse)
        if (   elem.attribute(AttrId::rx).toFloat() <= 0
            || elem.attribute(AttrId::ry).toFloat() <= 0)
            return true;

    // remove "switch" with no attributes or with only A_id attribute
    if (tagName == E_switch && !elem.hasChildrenElement()) {
        if (elem.attributesCount() == 0)
            return true;
        else if (elem.hasAttribute(AttrId::id) && elem.attributesCount() == 1)
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
        if (    findAttribute(elem, AttrId::fill) == V_none
            && (   findAttribute(elem, AttrId::stroke) == V_none
                || findAttribute(elem, AttrId::stroke).isEmpty())
            && !elem.isUsed()) {
            if (elem.hasAttribute(AttrId::filter)) {
                SvgElement filterElem = elemFromDefs(elem.defIdFromAttribute(AttrId::filter));
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
        if (!elem.hasAttributes()) {
            nextElement(elem, root);
            continue;
        }

        QString tagName = elem.tagName();
        foreach (const QString &attrName, elem.extAttributesList()) {
            if (attrName.startsWith(QL1S("inkscape")) && Keys.flag(Key::RemoveInkscapeAttributes))
                elem.removeAttribute(attrName);
            if (attrName.startsWith(QL1S("sodipodi")) && Keys.flag(Key::RemoveSodipodiAttributes))
                elem.removeAttribute(attrName);
            if ((attrName.startsWith(QL1S("i:")) || attrName.startsWith(QL1S("a:")))
                && Keys.flag(Key::RemoveAdobeAttributes))
                elem.removeAttribute(attrName);
            if (attrName.startsWith(QL1S("v:")) && Keys.flag(Key::RemoveMSVisioAttributes))
                elem.removeAttribute(attrName);
            if (attrName.startsWith(QL1S("c:")) && Keys.flag(Key::RemoveCorelDrawAttributes))
                elem.removeAttribute(attrName);
            if (attrName.startsWith(QL1S("sketch:")) && Keys.flag(Key::RemoveSketchAttributes))
                elem.removeAttribute(attrName);
        }

        if (Keys.flag(Key::RemoveDefaultAttributes)) {
            if (elem.attribute(AttrId::spreadMethod) == QL1S("pad"))
                elem.removeAttribute(AttrId::spreadMethod);
            if (tagName == E_clipPath) {
                if (elem.attribute(AttrId::clipPathUnits) == QL1S("userSpaceOnUse"))
                    elem.removeAttribute(AttrId::clipPathUnits);
            }
        }
        if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
            // TODO: svg do not have 'desc' attribute, check why it's needed
//                if (attrList.contains(AttrId::desc))
//                    attrList.removeOne(AttrId::desc);

            // 'text-align' is not svg attribute
            if (elem.hasAttribute(AttrId::text_align)) {
                if (!elem.hasAttribute(AttrId::text_anchor))
                    elem.setAttribute(AttrId::text_anchor, elem.attribute(AttrId::text_align));
                elem.removeAttribute(AttrId::text_align);
            }

            // xlink:href could not contains uri with spaces
            if (elem.hasAttribute(AttrId::xlink_href)) {
                QString xlink = elem.attribute(AttrId::xlink_href);
                if (!xlink.startsWith(QL1S("data:"))) {
                    if (   xlink.indexOf(QL1C(' ')) != -1
                        || !Properties::elementsUsingXLink.contains(tagName))
                        elem.removeAttribute(AttrId::xlink_href);
                }
            }
            if (tagName != E_svg) {
                foreach (const QString &attr, elem.extAttributesList()) {
                    if (attr.startsWith(QL1S("xmlns")))
                        elem.removeAttribute(attr);
                }
            }

            if (elem.hasAttribute(AttrId::marker))
                if (elem.attribute(AttrId::marker) == V_none)
                    elem.removeAttribute(AttrId::marker);

            // elements inside clipPath needs to contains only geometry relevant attributes
            if (elem.parentElement().tagName() == E_clipPath) {
                foreach (const int &attrId, elem.baseAttributesList()) {
                    if (Properties::presentationAttributesIds.contains(attrId)) {
                        if (attrId != AttrId::filter)
                            elem.removeAttribute(attrId);
                    }
                }
            }

            static const IntList strokeAndFill = IntList() << AttrId::fill << AttrId::stroke;
            foreach (const int &attrId, strokeAndFill) {
                if (elem.hasAttribute(attrId)) {
                    QString url = elem.attribute(attrId);
                    if (url.startsWith(UrlPrefix)) {
                        SvgElement defElem = elemFromDefs(url.mid(5, url.size()-6));
                        if (defElem.isNull())
                            elem.setAttribute(attrId, V_none);
                    }
                }
            }

            // remove all text based attributes from non-text elements
            // FIXME: slow, check is child has actual text
            if (!elem.hasText() && tagName != E_text && !elem.hasTextChild()) {
                foreach (const int &attrId, Properties::textAttributesIds) {
                    if (elem.hasAttribute(attrId))
                        elem.removeAttribute(attrId);
                }
                foreach (const int &attrId, elem.baseAttributesList()) {
                    if (Properties::textAttributesIds.contains(attrId))
                        elem.removeAttribute(attrId);
                }
                elem.removeAttribute(QL1S("writing-mode"));
                elem.removeAttribute(QL1S("line-height"));
                elem.removeAttribute(QL1S("block-progression"));
            }
        }

        if (elem.hasAttribute(AttrId::d)
            && tagName != E_path
            && tagName != E_glyph
            && tagName != E_missing_glyph)
        {
            elem.removeAttribute(AttrId::d);
        }

        // TODO: disable by key, because do not supported by SVG spec
        if (tagName == E_stop) {
            if (!elem.attribute(AttrId::offset).contains(LengthType::percent)) {
                if (elem.doubleAttribute(AttrId::offset) < 0.0001)
                    elem.removeAttribute(AttrId::offset);
            }
        }

        // TODO: 'display' attr remove

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
    IntList circle;
    circle << AttrId::transform << AttrId::cx << AttrId::cy << AttrId::r << AttrId::id;
    circle << Properties::presentationAttributesIds;

    IntList ellipse;
    ellipse << AttrId::transform << AttrId::cx << AttrId::cy << AttrId::rx
            << AttrId::ry << AttrId::id << Properties::presentationAttributesIds;

    element_loop(svgElement()) {
        QString tagName = elem.tagName();
        if (tagName == E_circle || tagName == E_ellipse) {
            foreach (const int &attrId, elem.baseAttributesList()) {
                if (!circle.contains(attrId) && tagName == E_circle)
                    elem.removeAttribute(attrId);
                else if (!ellipse.contains(attrId) && tagName == E_ellipse)
                    elem.removeAttribute(attrId);
            }
        }
        nextElement(elem, root);
    }
}

void Remover::cleanPresentationAttributes()
{
    _cleanPresentationAttributes();
}

void Remover::_cleanPresentationAttributes(SvgElement parent)
{
    if (parent.isNull())
        parent = svgElement();

    styleHashList << parent.styleHash();
    parentHash.unite(styleHashList.last());
    parentAttrs = parentHash.keys().toSet();

    SvgElement elem = parent.firstChildElement();
    while (!elem.isNull()) {
        IntHash hash = elem.styleHash();
        cleanStyle(elem, hash);
        foreach (const int &attrId, elem.styleAttributesList()) {
            if (hash.contains(attrId))
                elem.setAttribute(attrId, hash.value(attrId));
            else
                elem.removeAttribute(attrId);
            hash.remove(attrId);
        }
        foreach (const int &attrId, hash.keys())
            elem.setAttribute(attrId, hash.value(attrId));
        if (elem.hasChildrenElement())
            _cleanPresentationAttributes(elem);
        elem = elem.nextSiblingElement();
    }

    styleHashList.removeLast();
    parentHash.clear();
    foreach (const IntHash &hash, styleHashList)
        parentHash.unite(hash);
}

// removes default value, only if parent style did't contain same attribute
// needed for all inherited attributes
void Remover::cleanStyle(const SvgElement &elem, IntHash &hash)
{
    static bool isRemoveNotApplied = Keys.flag(Key::RemoveNotAppliedAttributes);

    if (isRemoveNotApplied) {
        // remove style props which already defined in parent style
        foreach (const int &attrId, parentHash.keys()) {
            if (attrId != AttrId::opacity && !elem.isUsed()) {
                if (hash.contains(attrId))
                    if (hash.value(attrId) == parentHash.value(attrId))
                        hash.remove(attrId);
            }
        }
    }

    // convert units
    static IntList numericStyleList = IntList()
        << AttrId::fill_opacity << AttrId::opacity << AttrId::stop_opacity
        << AttrId::stroke_miterlimit << AttrId::stroke_opacity << AttrId::stroke_width
        << AttrId::font_size << AttrId::kerning << AttrId::letter_spacing << AttrId::word_spacing
        << AttrId::baseline_shift << AttrId::stroke_dashoffset;
    foreach (const int &attrId, numericStyleList) {
        QString value = hash.value(attrId);
        if (!value.isEmpty()) {
            bool ok = false;
            qreal num = value.toDouble(&ok);
            if (!ok && !value.startsWith(UrlPrefix)) {
                if (attrId == AttrId::stroke_width) {
                    if (value.endsWith(LengthType::percent)) {
                        static QRectF m_viewBoxRect = viewBoxRect();
                        if (m_viewBoxRect.isNull())
                            qFatal("Error: could not detect viewBox");
                        hash.insert(attrId, Tools::convertUnitsToPx(value, m_viewBoxRect.width()));
                    }
                    else if (value.endsWith(LengthType::em) || value.endsWith(LengthType::ex)) {
                        QString fontSizeStr = findAttribute(elem, AttrId::font_size);
                        qreal fontSize = Tools::convertUnitsToPx(fontSizeStr).toDouble();
                        if (fontSize == 0)
                            qFatal("Error: could not convert em/ex values "
                                   "without font-size attribute is set.");
                        hash.insert(attrId, Tools::convertUnitsToPx(value, fontSize));
                    } else {
                        hash.insert(attrId, Tools::convertUnitsToPx(value));
                    }
                } else
                    hash.insert(attrId, Tools::convertUnitsToPx(value));
                num = hash.value(attrId).toDouble();
            }
            hash.insert(attrId, roundNumber(num, Round::Attribute));
        }
    }


    if (Keys.flag(Key::RemoveFillProps)
        && parentAttrs.contains(AttrId::fill)
        && parentHash.value(AttrId::fill) == V_none
        && (hash.value(AttrId::fill) == V_none || hash.value(AttrId::fill_opacity) == V_null))
    {
        // TODO: is this case needed?

        static const IntList fillList
            = IntList() << AttrId::fill << AttrId::fill_rule << AttrId::fill_opacity;
        foreach (const int &attrId, fillList)
            hash.remove(attrId);
    } else if (Keys.flag(Key::RemoveFillProps)
               && hash.value(AttrId::fill) == V_none)
    {
        // remove all fill properties if fill is off
        static const IntList fillList
            = IntList() << AttrId::fill_rule << AttrId::fill_opacity;
        foreach (const int &attrId, fillList)
            hash.remove(attrId);
    }

    // remove all stroke properties if stroke is off
    if (Keys.flag(Key::RemoveStrokeProps)
        && (   hash.value(AttrId::stroke) == V_none
            || hash.value(AttrId::stroke_opacity) == V_null
            || hash.value(AttrId::stroke_width) == V_null)) {
        static const IntList strokeList = IntList()
             << AttrId::stroke << AttrId::stroke_width << AttrId::stroke_linecap
             << AttrId::stroke_linejoin << AttrId::stroke_miterlimit << AttrId::stroke_dasharray
             << AttrId::stroke_dashoffset << AttrId::stroke_opacity;
        foreach (const int &attrId, strokeList)
            hash.remove(attrId);
        if (parentHash.value(AttrId::stroke) != V_none)
            hash.insert(AttrId::stroke, V_none);
    } else {
        // trim dasharray
        if (hash.contains(AttrId::stroke_dasharray))
            hash.insert(AttrId::stroke_dasharray, QString(hash.value(AttrId::stroke_dasharray))
                                             .replace(", ", ","));
    }

    if (isRemoveNotApplied) {
        // TODO: this
//        static const QString a_pointer_events = QL1S("pointer-events");
//        hash.remove(a_pointer_events);

        // remove clip-rule if elem not inside clipPath
        if (hash.contains(AttrId::clip_rule)) {
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
                hash.remove(AttrId::clip_rule);
        }

        // 'enable-background' is only applicable to container elements
        if (!elem.isContainer())
            hash.remove(AttrId::enable_background);

        if (elem.tagName() != E_svg && elem.tagName() != E_pattern && elem.tagName() != E_marker)
            hash.remove(AttrId::overflow);
    }

    if (Keys.flag(Key::RemoveDefaultAttributes)) {
        foreach (const int &attrId, hash.keys())
            removeDefaultValue(hash, attrId);
    }
}

void Remover::removeDefaultValue(IntHash &hash, int attrId)
{
    if (parentAttrs.contains(attrId))
        return;

    static const QHash<int,QVariant> defaultStyleValues = initDefaultStyleHash();

    if (attrId == AttrId::fill || attrId == AttrId::stop_color) {
        static QStringList defValues = QStringList() << "#000" << "#000000" << "black";
        if (defValues.contains(hash.value(attrId)))
            hash.remove(attrId);
    } else if (defaultStyleValues.contains(attrId)) {
        const QVariant value = defaultStyleValues.value(attrId);
        if (value.type() == QVariant::String) {
            if (value == hash.value(attrId))
                hash.remove(attrId);
        } else if (!hash.value(attrId).isEmpty()
                   && strToDouble(hash.value(attrId)) == value.toDouble()) {
            hash.remove(attrId);
        }
    }
}

void Remover::removeGroups()
{
    IntList illegalGAttrList = IntList() << AttrId::mask << AttrId::clip_path << AttrId::filter;

    bool isAnyGroupRemoved = true;
    while (isAnyGroupRemoved) {
        isAnyGroupRemoved = false;
        element_loop(svgElement()) {
            nextElement(elem, root);
            if (!elem.isGroup() || (elem.isGroup() && elem.attribute(AttrId::display) == V_none)) {
                continue;
            }

            SvgElement parent = elem.parentElement();

            // if group do not have any child elements - remove it
            if (elem.childElementCount() == 0) {
                elem = parent.removeChild(elem, true);
                continue;
            }

            /* if group has only one child
             * before:
             * <g id="1">
             *   <rect id="2"/>
             * </g>
             *
             * after:
             * <rect id="2"/>
            */
            if (    elem.childElementCount() == 1
                && !elem.isUsed()
                && !elem.firstChildElement().isUsed()
                && !elem.hasAttributes(illegalGAttrList))
            {
                SvgElement child = elem.firstChildElement();
                parent.insertBefore(child, elem);
                megreGroupWithChild(elem, child, false);
                elem = parent.removeChild(elem, true);
                isAnyGroupRemoved = true;
                continue;
            }

            bool isOnlyTransform = false;
            if (elem.hasAttribute(AttrId::transform)) {
                if (!elem.hasImportantAttrs(IntList() << AttrId::transform)) {
                    int trAttrCount = 0;
                    SvgElement childElem = elem.firstChildElement();
                    while (!childElem.isNull()) {
                        if (   childElem.isUsed()
                            || childElem.tagName() == E_use
                            || childElem.isGroup())
                            break;
                        if (childElem.hasAttribute(AttrId::transform))
                            trAttrCount++;
                        childElem = childElem.nextSiblingElement();
                    }
                    if (trAttrCount == elem.childElementCount())
                        isOnlyTransform = true;
                }
            }

            // if group do not have any important attributes
            // or all children has transform attribute
            if (!elem.hasImportantAttrs() || isOnlyTransform) {
                QString parentTransfrom = elem.attribute(AttrId::transform);
                foreach (SvgElement childElem, elem.childElements()) {
                    if (isOnlyTransform)
                        childElem.setTransform(parentTransfrom, true);
                    parent.insertBefore(childElem, elem);
                }
                elem = parent.removeChild(elem, true);
                isAnyGroupRemoved = true;
                continue;
            }
        }
    }
}
void Remover::megreGroupWithChild(SvgElement &groupElem, SvgElement &childElem, bool isParentToChild) const
{
    QStringList ignoreAttrList = QStringList() << A_id;
    if (!isParentToChild)
        ignoreAttrList << A_stroke << A_stroke_width << A_fill;

    foreach (const QString &attrName, groupElem.attributesList()) {
        if (childElem.hasAttribute(attrName) && attrName == A_transform) {
            childElem.setTransform(groupElem.attribute(attrName), !isParentToChild);
        } else if (attrName == A_opacity) {
            if (groupElem.hasAttribute(AttrId::opacity) && childElem.hasAttribute(AttrId::opacity)) {
                qreal newOp =  groupElem.doubleAttribute(AttrId::opacity)
                              * childElem.doubleAttribute(AttrId::opacity);
                childElem.setAttribute(AttrId::opacity, roundNumber(newOp));
            } else {
                childElem.setAttribute(attrName, groupElem.attribute(attrName));
            }
        } else if (!ignoreAttrList.contains(attrName) || !childElem.hasAttribute(attrName)) {
            childElem.setAttribute(attrName, groupElem.attribute(attrName));
        }
    }
}

void Remover::ungroupSwitchElement()
{
    element_loop(svgElement()) {
        nextElement(elem, root);
        if (elem.tagName() != E_switch)
            continue;

        if (!elem.hasImportantAttrs()) {
            SvgElement gElem = elem.firstChildElement();
            SvgElement switchElem = gElem.parentElement();
            SvgElement pp = switchElem.parentElement();
            SvgElement newElem;

            // remove all elements ins 'switch' except first
            SvgElementList list = switchElem.childElements();
            while (!list.isEmpty()) {
                SvgElement child = list.takeFirst();
                if (child == gElem)
                    newElem = pp.insertBefore(child, switchElem).toElement();
                else
                    switchElem.removeChild(child);
            }
            pp.removeChild(switchElem);
            elem = newElem;
        } else if (elem.hasChildren()) {
            SvgElementList list = elem.childElements();
            if (!list.isEmpty())
                list.takeFirst();
            while (!list.isEmpty())
                elem.removeChild(list.takeFirst());
            SvgElement child = elem.firstChildElement();
            megreGroupWithChild(elem, child, true);
            SvgElement tmpElem = elem.parentElement().insertBefore(child, elem).toElement();
            elem.parentElement().removeChild(elem, true);
            elem = tmpElem;
        } else if (!elem.hasChildren()) {
            elem = elem.parentElement().removeChild(elem, true);
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
            if (elem.hasChildrenElement())
                list << elem.childElements();
    }
}

void _setupTransformForBBox(const SvgElement &elem, const QStringList &trList)
{
    SvgElement child = elem.firstChildElement();
    while (!child.isNull()) {
        if (child.isContainer()) {
            QStringList tmpTrList = trList;
            if (child.hasAttribute(AttrId::transform))
                tmpTrList << child.attribute(AttrId::transform);
            _setupTransformForBBox(child, tmpTrList);
        } else if (child.hasAttribute(AttrId::bbox)) {
            QStringList tmpTrList = trList;
            if (child.hasAttribute(AttrId::transform))
                tmpTrList << child.attribute(AttrId::transform);
            child.setAttribute(AttrId::bbox_transform, tmpTrList.join(" "));
        } else if (child.hasChildrenElement())
            _setupTransformForBBox(child, trList);
        child = child.nextSiblingElement();
    }
}

void Remover::prepareViewBoxRect(QRectF &viewBox)
{
    if (svgElement().hasAttribute(AttrId::width) || svgElement().hasAttribute(AttrId::height)) {
        qreal w = viewBox.width();
        if (svgElement().hasAttribute(AttrId::width))
            w = svgElement().doubleAttribute(AttrId::width);

        qreal h = viewBox.height();
        if (svgElement().hasAttribute(AttrId::height))
            h = svgElement().doubleAttribute(AttrId::height);

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
// TODO: better element removing (skadge_SVG_widgets_for_diagrams.svg)
// TODO: remove 'image' elements (applications-other.svg)
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

        if (   elem.hasAttribute(AttrId::bbox)
            && findAttribute(elem, AttrId::used_element).isEmpty()
            && !hasParent(elem, E_defs)
            && !hasParent(elem, E_flowRegion)) {
            QStringList pList = elem.attribute(AttrId::bbox).split(" ");
            QRectF rect(strToDouble(pList.at(0)), strToDouble(pList.at(1)),
                        strToDouble(pList.at(2)), strToDouble(pList.at(3)));
            // fix rect's with zero area
            if (rect.width() == 0)
                rect.setWidth(1);
            if (rect.height() == 0)
                rect.setHeight(1);

            if (elem.hasAttribute(AttrId::bbox_transform)) {
                Transform tr(elem.attribute(AttrId::bbox_transform));
                rect = tr.transformRect(rect);
            }

            QString stroke = findAttribute(elem, AttrId::stroke);
            if (!stroke.isEmpty() && stroke != V_none) {
                QString sws = findAttribute(elem, AttrId::stroke_width);
                qreal sw = 1;
                if (!sws.isEmpty())
                    sw = sws.toDouble();
                rect.adjust(-sw/2, -sw/2, sw, sw);
            }

            if (!viewBox.intersects(rect)) {
                smartElementRemove(elem);
                elem.clear();
            } else {
                elem.removeAttribute(AttrId::bbox);
                elem.removeAttribute(AttrId::bbox_transform);
            }
        } else if (elem.hasAttribute(AttrId::bbox)) {
            elem.removeAttribute(AttrId::bbox);
            elem.removeAttribute(AttrId::bbox_transform);
        }
        if (!elem.isNull())
            if (elem.hasChildrenElement())
                list << elem.childElements();
    }
}
