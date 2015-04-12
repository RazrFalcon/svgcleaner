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
        ignoreAttr << QL1S("xmlns") << QL1S("xmlns:xlink") << A_width << A_height << A_viewBox;

        if (!Keys.flag(Key::RemoveSvgVersion))
            ignoreAttr << QL1S("version");
        if (!Keys.flag(Key::RemoveInkscapeAttributes))
            ignoreAttr << QL1S("xmlns:inkscape");
        if (!Keys.flag(Key::RemoveSodipodiAttributes))
            ignoreAttr << QL1S("xmlns:sodipodi");
        if (!Keys.flag(Key::RemoveAdobeAttributes))
            ignoreAttr << QL1S("xmlns:x") << QL1S("xmlns:i") << QL1S("xmlns:graph");
        if (!Keys.flag(Key::RemoveCorelDrawAttributes))
            ignoreAttr << QL1S("xmlns:odm") << QL1S("xmlns:corel-charset");
        if (!Keys.flag(Key::RemoveMSVisioAttributes))
            ignoreAttr << QL1S("xmlns:v");
        if (!Keys.flag(Key::RemoveSketchAttributes))
            ignoreAttr << QL1S("xmlns:sketch");
        foreach (const QString &attr, svgElement().attributesNamesList()) {
            if (!ignoreAttr.contains(attr)) {
                if (attr == Attribute::A_transform)
                    svgElement().removeTransform();
                else
                    svgElement().removeAttribute(attr);
            }
        }
    } else {
        if (Keys.flag(Key::RemoveSvgVersion))
            svgElement().removeAttribute(QL1S("version"));
    }

    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (Keys.flag(Key::RemoveDefaultAttributes))
        svgElement().removeAttributeIf(AttrId::display, QL1S("inline"));
    // TODO: add default attributes removing
}

void Remover::checkXlinkDeclaration()
{
    bool isXlinkUsed = false;
    element_loop (svgElement()) {
        if (elem.hasAttribute(AttrId::xlink_href)) {
            isXlinkUsed = true;
            break;
        }
    }
    if (!isXlinkUsed) {
        svgElement().removeAttribute(QL1S("xmlns:xlink"));
        return;
    }

    // fix missing or wrong xmlns:xlink attribute
    if (svgElement().attribute(QL1S("xmlns:xlink")) != QL1S("http://www.w3.org/1999/xlink"))
        svgElement().setAttribute(QL1S("xmlns:xlink"), QL1S("http://www.w3.org/1999/xlink"));
}

void Remover::removeUnusedDefs()
{
    bool isAnyRemoved = true;
    while (isAnyRemoved) {
        isAnyRemoved = false;
        loop_children (defsElement()) {
            if (elem.isUsed())
                continue;

            // 'clipPath' can be unused, but elements in it can be used
            if (elem.hasChildren()) {
                // TODO: maybe recursive
                loop_children_arg (clipChild, elem) {
                    if (clipChild.isUsed()) {
                        SvgElement tElem = clipChild;
                        clipChild = clipChild.nextSiblingElement();
                        defsElement().insertBefore(tElem, elem);
                        clipChild = prevSiblingElement(clipChild);
                    }
                }
            }
            removeAndMoveToPrevSibling(elem);
            isAnyRemoved = true;
        }
    }
}

void Remover::removeUnusedDefsAttributes()
{
    SvgElement elem = defsElement().firstChildElement();
    while (!elem.isNull()) {
        if (elem.tagName() == E_linearGradient) {
            if (elem.hasAttribute(AttrId::x2) || elem.hasAttribute(AttrId::y2)) {
                if (isZero(elem.doubleAttribute(AttrId::x1) - elem.doubleAttribute(AttrId::x2))) {
                    elem.removeAttribute(AttrId::x1);
                    elem.setAttribute(AttrId::x2, V_zero);
                }
                if (isZero(elem.doubleAttribute(AttrId::y1) - elem.doubleAttribute(AttrId::y2))) {
                    elem.removeAttribute(AttrId::y1);
                    elem.removeAttribute(AttrId::y2);
                }
            }
            // remove transform, if it cannot be apply to anything
            if (   !elem.hasAttribute(AttrId::x1) && isZero(elem.doubleAttribute(AttrId::x2))
                && !elem.hasAttribute(AttrId::y1) && !elem.hasAttribute(AttrId::y2))
            {
                elem.removeTransform();
            }
        } else if (elem.tagName() == E_radialGradient) {
            if (isZero(elem.doubleAttribute(AttrId::fx) - elem.doubleAttribute(AttrId::cx)))
                elem.removeAttribute(AttrId::fx);
            if (isZero(elem.doubleAttribute(AttrId::fy) - elem.doubleAttribute(AttrId::cy)))
                elem.removeAttribute(AttrId::fy);
        }
        elem = elem.nextSiblingElement();
    }
}

void Remover::removeDuplicatedDefs()
{
    loop_children (defsElement()) {
        QString tag = elem.tagName();
        if (tag == E_linearGradient)
            detectEqualLinearGradients(elem);
        else if (tag == E_radialGradient)
            detectEqualRadialGradients(elem);
        else if (tag == E_filter)
            detectEqualFilters(elem);
        else if (tag == E_clipPath)
            detectEqualClipPaths(elem);
    }
}

void Remover::detectEqualLinearGradients(SvgElement &elem1)
{
    static const IntList gradientAttrs = IntList()
        << AttrId::transform << AttrId::xlink_href << AttrId::x1 << AttrId::y1 << AttrId::x2
        << AttrId::y2 << AttrId::gradientUnits << AttrId::spreadMethod
        << AttrId::externalResourcesRequired;

    loop_children (defsElement()) {
        if (elem1 == elem || elem.tagName() != E_linearGradient)
            continue;

        bool isEqual = true;
        foreach (const uint &attrId, gradientAttrs) {
            if (attrId == AttrId::transform) {
                if (elem1.transform() != elem.transform()) {
                    isEqual = false;
                    break;
                }
            } else if (attrId == AttrId::xlink_href) {
                if (elem1.referencedElement(attrId).id() != elem.referencedElement(attrId).id()) {
                    isEqual = false;
                    break;
                }
            } else if (elem1.attribute(attrId) != elem.attribute(attrId)) {
                isEqual = false;
                break;
            }
        }

        if (isEqual)
            isEqual = isGradientStopsEqual(elem1, elem);

        if (isEqual) {
            foreach (SvgElement child, elem.linkedElements()) {
                uint attrId = child.referenceAttribute(elem);
                child.setReferenceElement(attrId, elem1);
            }
            SvgElement tElem = elem;
            elem = prevSiblingElement(elem);
            smartElementRemove(tElem);
        }
    }
}

void Remover::detectEqualRadialGradients(SvgElement &elem1)
{
    static const IntList gradientAttrs = IntList()
        << AttrId::transform << AttrId::xlink_href << AttrId::cx << AttrId::cy << AttrId::r
        << AttrId::fx << AttrId::fy << AttrId::gradientUnits << AttrId::spreadMethod
        << AttrId::externalResourcesRequired;

    SvgElement elem2 = defsElement().firstChildElement();
    while (!elem2.isNull()) {
        if (   elem1 == elem2
            || elem2.tagName() != E_radialGradient)
        {
            elem2 = elem2.nextSiblingElement();
            continue;
        }

        bool isEqual = true;
        foreach (uint attrId, gradientAttrs) {
            if (attrId == AttrId::transform) {
                if (elem1.transform() != elem2.transform()) {
                    isEqual = false;
                    break;
                }
            } else if (attrId == AttrId::xlink_href) {
                if (elem1.referencedElement(attrId).id() != elem2.referencedElement(attrId).id()) {
                    isEqual = false;
                    break;
                }
            } else if (elem1.hasAttribute(attrId) != elem2.hasAttribute(attrId)) {
                isEqual = false;
                break;
            } else if (elem1.attribute(attrId) != elem2.attribute(attrId)) {
                isEqual = false;
                break;
            }
        }

        if (isEqual)
            isEqual = isGradientStopsEqual(elem1, elem2);

        if (isEqual) {
            foreach (SvgElement child, elem2.linkedElements()) {
                uint attrId = child.referenceAttribute(elem2);
                child.setReferenceElement(attrId, elem1);
            }
            SvgElement tElem = elem2;
            elem2 = prevSiblingElement(elem2);
            smartElementRemove(tElem);
        }

        elem2 = elem2.nextSiblingElement();
    }
}

// TODO: process not only feGaussianBlur
void Remover::detectEqualFilters(SvgElement &elem1)
{
    static const IntList filterAttrs = IntList()
        << AttrId::transform << AttrId::xlink_href << AttrId::x << AttrId::y << AttrId::width
        << AttrId::height << AttrId::filterRes << AttrId::filterUnits << AttrId::primitiveUnits
        << AttrId::externalResourcesRequired;

    SvgElement elem2 = defsElement().firstChildElement();
    while (!elem2.isNull()) {
        if (   elem1 == elem2
            || elem2.tagName() != E_filter
            || elem1.childElementCount() != 1
            || elem1.childElementCount() != elem2.childElementCount())
        {
            elem2 = elem2.nextSiblingElement();
            continue;
        }

        bool isEqual = true;
        foreach (uint attrId, filterAttrs) {
            if (attrId == AttrId::transform && elem1.transform() != elem2.transform()) {
                isEqual = false;
                break;
            } else if (elem1.attribute(attrId) != elem2.attribute(attrId)) {
                isEqual = false;
                break;
            }
        }

        if (isEqual) {
            SvgElement child1 = elem1.firstChildElement();
            SvgElement child2 = elem2.firstChildElement();
            if (child1.tagName() != E_feGaussianBlur || child2.tagName() != E_feGaussianBlur)
                isEqual = false;
            if (child1.attribute(AttrId::stdDeviation) != child2.attribute(AttrId::stdDeviation))
                isEqual = false;
        }

        if (isEqual) {
            foreach (SvgElement child, elem2.linkedElements()) {
                uint attrId = child.referenceAttribute(elem2);
                child.setReferenceElement(attrId, elem1);
            }

            SvgElement tElem = elem2;
            elem2 = elem2.previousSiblingElement();
            smartElementRemove(tElem);
        }

        elem2 = elem2.nextSiblingElement();
    }
}

void Remover::detectEqualClipPaths(SvgElement &elem1)
{
    SvgElement elem2 = defsElement().firstChildElement();
    while (!elem2.isNull()) {
        if (   elem1 == elem2
            || elem2.tagName() != E_clipPath
            || elem1.childElementCount() != 1
            || elem1.childElementCount() != elem2.childElementCount())
        {
            elem2 = elem2.nextSiblingElement();
            continue;
        }

        bool isEqual = true;

        SvgElement child1 = elem1.firstChildElement();
        SvgElement child2 = elem2.firstChildElement();
        if (child1.tagName() != E_path || child2.tagName() != E_path)
            isEqual = false;
        if (child1.attribute(AttrId::d) != child2.attribute(AttrId::d))
            isEqual = false;

        if (isEqual) {
            if (child1.transform() == child2.transform()) {
                foreach (SvgElement child, elem2.linkedElements()) {
                    uint attrId = child.referenceAttribute(elem2);
                    child.setReferenceElement(attrId, elem1);
                }

                SvgElement tElem = elem2;
                elem2 = elem2.previousSiblingElement();
                smartElementRemove(tElem);
            } else {
                // TODO: shoud be done by Replacer::replaceEqualElementsByUse()
                child2.setTagName(E_use);
                child2.removeAttribute(AttrId::d);
                if (child1.id().isEmpty())
                    child1.setAttribute(AttrId::id, genFreeId());
                child2.setReferenceElement(AttrId::xlink_href, child1);
                if (child1.parentElement().tagName() == E_clipPath) {
                    SvgElement newUse = document().createElement(E_use);
                    newUse.setReferenceElement(AttrId::xlink_href, child1);
                    newUse.setTransform(child1.transform());
                    child1.removeTransform();
                    elem1.insertBefore(newUse, child1);
                    defsElement().insertBefore(child1, elem1);
                }
            }
        }

        elem2 = elem2.nextSiblingElement();
    }
}

void Remover::removeUnreferencedIds()
{
    // find
    StringSet m_allIdList;
    StringSet m_allLinkList;

    QString pathEffect = QL1S("inkscape:path-effect");
    QString perspectiveID = QL1S("inkscape:perspectiveID");

    element_loop (svgElement()) {
        // collect all id's
        if (elem.hasAttribute(AttrId::id))
            m_allIdList << elem.id();
        if (elem.hasAttribute(AttrId::xlink_href))
            m_allLinkList << elem.referencedElement(AttrId::xlink_href).id();
        if (elem.hasAttribute(pathEffect))
            m_allLinkList << elem.attribute(pathEffect).remove(0,1);
        if (elem.hasAttribute(perspectiveID))
            m_allLinkList << elem.attribute(perspectiveID).remove(0,1);

        foreach (const SvgAttribute &attr, elem.attributesList()) {
            if (attr.isReference())
                m_allLinkList << attr.referencedElement().id();
        }
    }

    // remove all linked ids
    foreach (const QString &text, m_allLinkList)
        m_allIdList.remove(text);

    if (Keys.flag(Key::KeepNamedIds)) {
        // skip id's whithout digits
        foreach (const QString &text, m_allIdList) {
            if (!text.contains(QRegExp(QL1S("\\d"))))
                m_allIdList.remove(text);
        }
    }

    // remove attributes
    element_loop (svgElement()) {
        if (m_allIdList.contains(elem.id()))
            elem.removeAttribute(AttrId::id);

        if (m_allIdList.contains(elem.referencedElement(AttrId::clip_path).id()))
            elem.removeAttribute(AttrId::id);
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

            if (currNode.isNull())
                continue;

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
                         && !currElem.hasReference(AttrId::xlink_href)
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
                             && !currElem.hasReference(AttrId::xlink_href)) {
                        removeThisNode = true;
                    } else if (    currTag == E_image
                               && !currElem.attribute(AttrId::xlink_href).startsWith(QL1S("data")))
                        removeThisNode = true;
                    else if (!currElem.isUsed()
                             && isElementInvisible(currElem)
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
                if (currNode.isElement()) {
                    SvgElement e = currNode.toElement();
                    smartElementRemove(e);
                } else {
                    currNode.parentNode().removeChild(currNode);
                }
                isAnyRemoved = true;
            }

            if (currNode.hasChildren())
                nodeList << currNode.childNodes();
        }
    }

    // TODO: move to separate func
    double stdDevLimit = Keys.doubleNumber(Key::RemoveTinyGaussianBlur);
    if (stdDevLimit == 0)
        return;
    element_loop (defsElement()) {
        if (elem.tagName() == E_feGaussianBlur) {
            // FIXME: check for stdDeviation with transform of all linked element applied
            if (stdDevLimit != 0.0) {
                if (elem.parentElement().childElementCount() == 1) {
                    // 'stdDeviation' can contains not only one value
                    // we process it when it contains only one value
                    const QString stdDev = elem.attribute(AttrId::stdDeviation);
                    if (!stdDev.contains(QL1S(",")) && !stdDev.contains(QL1S(" "))) {
                        bool ok = true;
                        if (stdDev.toDouble(&ok) <= stdDevLimit) {
                            if (!ok) {
                                qFatal("could not parse stdDeviation value: %s",
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

    element_loop (document().documentElement()) {
        if (isElementInvisible2(elem)) {
            removeAndMoveToPrev(elem);
        }
    }
}

bool Remover::isElementInvisible(SvgElement &elem)
{
    QString tagName = elem.tagName();
    //remove elements "rect", "pattern" and "image" with height or width <= 0
    if (tagName == E_rect || tagName == E_pattern || tagName == E_image) {
        if (elem.hasAttribute(AttrId::width) && elem.hasAttribute(AttrId::height)) {
            double width  = elem.doubleAttribute(AttrId::width);
            double height = elem.doubleAttribute(AttrId::height);
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
        static const QString v_hidden = QL1S("hidden");
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
        QString stroke = parentAttribute(elem, AttrId::stroke);
        if (    parentAttribute(elem, AttrId::fill) == V_none
            && (stroke == V_none || stroke.isEmpty())
            && !elem.isUsed()
            && elem.usesCount() == 0)
        {
            SvgElement filterElem = elem.referencedElement(AttrId::filter);
            if (!filterElem.isNull()) {
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
    element_loop (document().documentElement()) {
        if (!elem.hasAttributes())
            continue;

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
            if (elem.parentElement().tagName() == E_clipPath/* && elem.tagName() != E_use*/) {
                foreach (const uint &attrId, elem.styleAttributesList()) {
                    // TODO: detect filter type
                    // ignore filter, because blur filter can change shape of 'clipPath'
//                    if (attrId != AttrId::filter)
                    if (!elem.attributeItem(attrId).isReference())
                        elem.removeAttribute(attrId);
                }
            }

            // remove all text based attributes from non-text elements
            // FIXME: slow, check is child has actual text
            if (!elem.hasText() && tagName != E_text && !elem.hasTextChild()) {
                foreach (const uint &attrId, Properties::textAttributesIds) {
                    if (elem.hasAttribute(attrId))
                        elem.removeAttribute(attrId);
                }
                foreach (const uint &attrId, elem.baseAttributesList()) {
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
            if (!elem.attribute(AttrId::offset).contains(LengthType::Percent)) {
                if (elem.doubleAttribute(AttrId::offset) < 0.0001)
                    elem.removeAttribute(AttrId::offset);
            }
        }

        // TODO: remove 'display' attr
    }

    // remove xml:space when no child has multispace text
    element_loop (document().documentElement()) {
        if (elem.hasAttribute(QL1S("xml:space"))) {
            bool canRemove = true;
            SvgElement elem2 = document().documentElement();
            SvgElement root2 = elem;
            while (!elem2.isNull()) {
                if (Properties::textElements.contains(elem2.tagName())) {
                    if (elem2.hasText()) {
                        QString text = elem2.text();
                        if (   text.contains(QL1S("  "))
                            || text.startsWith(QL1C(' '))
                            || text.endsWith(QL1C(' ')))
                        {
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

    element_loop (svgElement()) {
        QString tagName = elem.tagName();
        if (tagName == E_circle) {
            foreach (const uint &attrId, elem.baseAttributesList()) {
                if (!circle.contains(attrId))
                    elem.removeAttribute(attrId);
            }
        }
        else if (tagName == E_ellipse) {
            foreach (const uint &attrId, elem.baseAttributesList()) {
                if (!ellipse.contains(attrId))
                    elem.removeAttribute(attrId);
            }
        }
    }
}

void Remover::cleanPresentationAttributes()
{
    element_loop (svgElement())
        cleanStyle(elem);
}

// removes default value, only if parent style didn't contain same attribute
// needed for all inherited attributes
void Remover::cleanStyle(SvgElement &elem)
{
    u_static bool isRemoveNotApplied = Keys.flag(Key::RemoveNotAppliedAttributes);

    if (isRemoveNotApplied) {
        // remove styles which already defined in parent elements
        foreach (const uint &attrId, elem.styleAttributesList()) {
            if (attrId != AttrId::opacity && !elem.isUsed() && elem.usesCount() == 0) {
                if (elem.hasParentAttribute(attrId)) {
                    if (elem.attribute(attrId) == elem.parentAttribute(attrId)) {
                        elem.removeAttribute(attrId);
                    }
                }
            }
        }
    }

    if (Keys.flag(Key::RemoveFillProps)
        && elem.hasParentAttribute(AttrId::fill)
        && elem.parentAttribute(AttrId::fill) == V_none
        && (   elem.attribute(AttrId::fill) == V_none
            || elem.attribute(AttrId::fill_opacity) == V_zero))
    {
        elem.removeAttribute(AttrId::fill);
        elem.removeAttribute(AttrId::fill_rule);
        elem.removeAttribute(AttrId::fill_opacity);
    }
    else if (   Keys.flag(Key::RemoveFillProps)
             && elem.attribute(AttrId::fill) == V_none)
    {
        // remove all fill attributes when fill=none, except 'fill',
        // because by default it's 'black'
        elem.removeAttribute(AttrId::fill_rule);
        elem.removeAttribute(AttrId::fill_opacity);
    }

    // remove all stroke properties if stroke is off
    if (Keys.flag(Key::RemoveStrokeProps)
        && (   elem.attribute(AttrId::stroke) == V_none
            || elem.attribute(AttrId::stroke_opacity) == V_zero
            || elem.attribute(AttrId::stroke_width) == V_zero))
    {
        static const IntList strokeList = IntList()
             << AttrId::stroke << AttrId::stroke_width << AttrId::stroke_linecap
             << AttrId::stroke_linejoin << AttrId::stroke_miterlimit << AttrId::stroke_dasharray
             << AttrId::stroke_dashoffset << AttrId::stroke_opacity;
        foreach (const uint &attrId, strokeList)
            elem.removeAttribute(attrId);

        // TODO: add test case for it
        if (elem.parentAttribute(AttrId::stroke) != V_none)
            elem.setAttribute(AttrId::stroke, V_none);
    } else {
        // trim dasharray
        if (elem.hasAttribute(AttrId::stroke_dasharray))
            elem.setAttribute(AttrId::stroke_dasharray,
                              QString(elem.attribute(AttrId::stroke_dasharray))
                                .replace(QL1S(", "), QL1S(",")));
    }
    if (isRemoveNotApplied) {
        // TODO: add clean key for this
//        static const QString a_pointer_events = QL1S("pointer-events");
//        hash.remove(a_pointer_events);

        // remove clip-rule if elem not inside clipPath
        if (elem.hasAttribute(AttrId::clip_rule) && !hasParent(elem, E_clipPath))
            elem.removeAttribute(AttrId::clip_rule);

        // TODO: 'enable-background' is actualy deprecated
        // 'enable-background' is only applicable to container elements
        if (!elem.isContainer())
            elem.removeAttribute(AttrId::enable_background);

        if (elem.tagName() != E_svg && elem.tagName() != E_pattern && elem.tagName() != E_marker)
            elem.removeAttribute(AttrId::overflow);
    }

    if (Keys.flag(Key::RemoveDefaultAttributes)) {
        static const QHash<uint,QVariant> defaultStyleValues = initDefaultStyleHash();
        foreach (const uint &attrId, elem.styleAttributesList()) {
            if (elem.hasParentAttribute(attrId))
                continue;

            if (attrId == AttrId::fill || attrId == AttrId::stop_color) {
                static const QStringList defValues = QStringList()
                    << QL1S("#000") << QL1S("#000000") << QL1S("black");
                if (defValues.contains(elem.attribute(attrId)))
                    elem.removeAttribute(attrId);
            } else if (defaultStyleValues.contains(attrId)) {
                const QVariant value = defaultStyleValues.value(attrId);
                if (value.type() == QVariant::String) {
                    if (value.toString() == elem.attribute(attrId))
                        elem.removeAttribute(attrId);
                } else {
                    if (!elem.attribute(attrId).isEmpty() &&
                           isZero(elem.doubleAttribute(attrId) - value.toDouble()))
                    elem.removeAttribute(attrId);
                }
            }
        }
    }
}

void Remover::removeGroups()
{
    IntList illegalGAttrList = IntList() << AttrId::mask << AttrId::clip_path << AttrId::filter;

    bool isAnyGroupRemoved = true;
    while (isAnyGroupRemoved) {
        isAnyGroupRemoved = false;
        element_loop (svgElement()) {
            if (!elem.isGroup())
                continue;

            if (elem.attribute(AttrId::display) == V_none)
                continue;

            SvgElement parent = elem.parentElement();

            // if group do not have any child elements - remove it
            if (!elem.hasChildren()) {
                elem = smartElementRemove(elem, true);
                continue;
            }

            if (elem.isUsed())
                continue;

            /* if group has only one child
             * before:
             * <g id="1">
             *   <rect id="2"/>
             * </g>
             *
             * after:
             * <rect id="2"/>
            */
            // TODO: ungroup used group
            if (    elem.childElementCount() == 1
                // && !elem.isUsed()
                && !elem.firstChildElement().isUsed()
                && !elem.hasAttributes(illegalGAttrList))
            {
                SvgElement child = elem.firstChildElement();
                parent.insertBefore(child, elem);
                megreGroupWithChild(elem, child, false);
                elem = smartElementRemove(elem, true);
                isAnyGroupRemoved = true;
                continue;
            }

            bool isOnlyTransform = false;
            if (elem.hasTransform()) {
                if (!elem.hasImportantAttrs(IntList() << AttrId::transform)) {
                    int trAttrCount = 0;
                    SvgElement childElem = elem.firstChildElement();
                    while (!childElem.isNull()) {
                        if (   childElem.isUsed()
                            || childElem.tagName() == E_use
                            || childElem.isGroup())
                            break;
                        if (childElem.hasTransform())
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
                Transform parentTransfrom = elem.transform();
                foreach (SvgElement childElem, elem.childElements()) {
                    if (isOnlyTransform)
                        childElem.setTransform(parentTransfrom, true);
                    parent.insertBefore(childElem, elem);
                }
                elem = smartElementRemove(elem, true);
                isAnyGroupRemoved = true;
                continue;
            }
        }
    }
}

void Remover::megreGroupWithChild(SvgElement &groupElem, SvgElement &childElem,
                                  bool isParentToChild) const
{
    IntList ignoreAttrList = IntList() << AttrId::id;
    if (!isParentToChild)
        ignoreAttrList << AttrId::stroke << AttrId::stroke_width << AttrId::fill;

    foreach (const SvgAttribute &attr, groupElem.attributesList()) {
        if (attr.isTransform() && childElem.hasTransform()) {
            childElem.setTransform(groupElem.transform(), !isParentToChild);
        } else if (attr.id() == AttrId::opacity) {
            if (groupElem.hasAttribute(AttrId::opacity) && childElem.hasAttribute(AttrId::opacity)) {
                // merge opacity
                double newOp =  groupElem.doubleAttribute(AttrId::opacity)
                              * childElem.doubleAttribute(AttrId::opacity);
                childElem.setAttribute(AttrId::opacity, fromDouble(newOp));
            } else {
                childElem.setAttribute(attr);
            }
        } else if (!ignoreAttrList.contains(attr.id()) || !childElem.hasAttribute(attr.id())) {
            childElem.setAttribute(attr);
        }
    }
}

void Remover::ungroupSwitchElement()
{
    element_loop (svgElement()) {
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

void _setupTransformForBBox(const SvgElement &elem, const Transform &parentTs)
{
    SvgElement child = elem.firstChildElement();
    while (!child.isNull()) {
        if (child.isContainer()) {
            Transform ts = parentTs;
            if (ts.isNull())
                ts = Transform(QString());
            if (child.hasTransform())
                ts.append(child.transform());
            _setupTransformForBBox(child, ts);
        } else if (child.hasAttribute(AttrId::bbox)) {
            Transform ts = parentTs;
            if (ts.isNull())
                ts = Transform(QString());
            if (child.hasTransform())
                ts.append(child.transform());
            child.setBBoxTransform(ts);
        } else if (child.hasChildrenElement()) {
            _setupTransformForBBox(child, parentTs);
        }
        child = child.nextSiblingElement();
    }
}

void Remover::prepareViewBoxRect(QRectF &viewBox)
{
    if (svgElement().hasAttribute(AttrId::width) || svgElement().hasAttribute(AttrId::height)) {
        double w = viewBox.width();
        if (svgElement().hasAttribute(AttrId::width))
            w = svgElement().doubleAttribute(AttrId::width);

        double h = viewBox.height();
        if (svgElement().hasAttribute(AttrId::height))
            h = svgElement().doubleAttribute(AttrId::height);

        double vbAspect = viewBox.width()/viewBox.height();
        double aspect = w/h;

        QSizeF s(w, h);
        if ((aspect > 1.0 && aspect > vbAspect) || aspect > vbAspect) {
            double asp = viewBox.height()/h;
            s.scale(w * asp, h * asp, Qt::KeepAspectRatio);
        } else {
            double asp = viewBox.width()/w;
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

    _setupTransformForBBox(svgElement(), Transform());

    SvgElementList list = svgElement().childElements();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (   elem.hasAttribute(AttrId::bbox)
            && !hasUsedParent(elem)
            && !hasParent(elem, E_defs)
            && !hasParent(elem, E_flowRegion)) {
            QStringList pList = elem.attribute(AttrId::bbox).split(QL1C(' '));
            QRectF rect(toDouble(pList.at(0)), toDouble(pList.at(1)),
                        toDouble(pList.at(2)), toDouble(pList.at(3)));

            if (rect.width() == 0 && rect.height() == 0) {
                smartElementRemove(elem);
                continue;
            }

            // fix rect's with zero area
            if (rect.width() == 0)
                rect.setWidth(1);
            if (rect.height() == 0)
                rect.setHeight(1);

            if (elem.bboxTransform().isValid()) {
                // cannot apply skew transform to rect
                if (elem.bboxTransform().isSkew())
                    continue;
                rect = elem.bboxTransform().transformRect(rect);
            }

            QString stroke = parentAttribute(elem, AttrId::stroke);
            if (!stroke.isEmpty() && stroke != V_none) {
                QString sws = parentAttribute(elem, AttrId::stroke_width);
                double sw = 1;
                if (!sws.isEmpty())
                    sw = toDouble(sws);
                rect.adjust(-sw/2, -sw/2, sw, sw);
            }

            // create bounding rect for visual debug
//            SvgElement rectElem = document().createElement(QL1S("rect"));
//            rectElem.setAttribute(AttrId::x, fromDouble(rect.x()));
//            rectElem.setAttribute(AttrId::y, fromDouble(rect.y()));
//            rectElem.setAttribute(AttrId::width, fromDouble(rect.width()));
//            rectElem.setAttribute(AttrId::height, fromDouble(rect.height()));
//            rectElem.setAttribute(QL1S("style"), QL1S("fill:none;stroke:red"));
//            rectElem.setAttribute(QL1S("from-id"), elem.id());
//            svgElement().appendChild(rectElem);

            if (!viewBox.intersects(rect)) {
                smartElementRemove(elem);
            } else {
                elem.removeAttribute(AttrId::bbox);
                elem.setBBoxTransform(Transform());
            }
        } else if (elem.hasAttribute(AttrId::bbox)) {
            elem.removeAttribute(AttrId::bbox);
            elem.setBBoxTransform(Transform());
        }
        if (!elem.isNull())
            if (elem.hasChildrenElement())
                list << elem.childElements();
    }
}
