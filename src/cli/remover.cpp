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
// TODO: remove equal styles in used element and it use
// TODO: remove items which out of viewbox
//       Anonymous_butterfly_and_flowers.svg
// TODO: remove "tspan" without attributes
// TODO: remove "feBlend" element with in or in2 linked to empty object
//       babayasin_air_hammer.svg
// TODO: remove "symbol"
// TODO: remove elem from defs if it used only by one use elem
void Remover::cleanSvgElementAttribute()
{
    bool isXlinkUsed = false;
    QList<SvgElement> nodeList = svgElement().childElemList();
    while (!nodeList.isEmpty()) {
        SvgElement currElem = nodeList.takeFirst();
        if (currElem.hasAttribute("xlink:href")) {
            isXlinkUsed = true;
            break;
        }
        if (currElem.hasChildren())
            nodeList << currElem.childElemList();
    }

    if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
        StringSet ignoreAttr = Props::presentationAttributes;
        ignoreAttr << "xmlns" << "width" << "height" << "viewBox";

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
            if (!ignoreAttr.contains(attrName)) {
                svgElement().removeAttribute(attrName);
            }
        }
    } else {
        if (Keys.flag(Key::RemoveSvgVersion))
            svgElement().removeAttribute("version");
    }

    // fix missing xmlns:xlink attribute
    if (isXlinkUsed && !svgElement().hasAttribute("xmlns:xlink"))
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (Keys.flag(Key::RemoveDefaultAttributes)) {
        if (svgElement().hasAttribute("display")) {
            if (svgElement().attribute("display") == "inline")
                svgElement().removeAttribute("display");
        }
    }
}

void Remover::removeUnusedDefs()
{
    StringSet defsIdList;
    foreach (const SvgElement &elem, defsElement().childElemList())
        if (elem.tagName() != QLatin1String("clipPath"))
            defsIdList << elem.id();

    StringSet currDefsIdList = defsIdList;
    while (!currDefsIdList.isEmpty()) {
        currDefsIdList = defsIdList;
        QList<SvgElement> list = svgElement().childElemList();
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            if (elem.hasAttribute("xlink:href"))
                currDefsIdList.remove(elem.xlinkId());
            foreach (const char* attrName, Props::linkableStyleAttributes) {
                if (elem.hasAttribute(attrName)) {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(QLatin1String("url(")))
                        currDefsIdList.remove(url.mid(5, url.size()-6));
                }
            }
            if (elem.hasChildren())
                list << elem.childElemList();
        }

        foreach (const SvgElement &elem, defsElement().childElemList()) {
            if (currDefsIdList.contains(elem.id())) {
                defsIdList.remove(elem.id());
                defsElement().removeChild(elem);
            }
        }
    }
}

void Remover::removeUnusedXLinks()
{
    CharList xlinkStyles;
    xlinkStyles << "fill" << "stroke" << "filter" << "clip-path" << "xlink:href";

    StringSet xlinkSet;
    StringSet idSet;
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        foreach (const char *attrName, xlinkStyles) {
            if (elem.hasAttribute(attrName)) {
                if (QString(attrName) == "xlink:href") {
                    if (!elem.attribute("xlink:href").contains("base64"))
                        xlinkSet << elem.xlinkId();
                } else {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(QLatin1String("url(")))
                        xlinkSet << url.mid(5, url.size()-6);
                }
            }
        }
        if (elem.hasAttribute("id"))
            idSet << elem.id();

        if (elem.hasChildren())
            list << elem.childElemList();
    }
    foreach (const QString &id, idSet)
        xlinkSet.remove(id);


    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        foreach (const QString &attrName, xlinkStyles) {
            if (elem.hasAttribute(attrName)) {
                if (attrName == "xlink:href") {
                    if (xlinkSet.contains(elem.xlinkId()))
                        elem.removeAttribute(attrName);
                } else {
                    QString url = elem.attribute(attrName);
                    if (url.startsWith(QLatin1String("url("))) {
                        if (xlinkSet.contains(url.mid(5, url.size()-6)))
                            elem.removeAttribute(attrName);
                    }
                }
            }
        }
        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

// TODO: detect not only equal, but with diff less than 1%
// TODO: refract this func
void Remover::removeDuplicatedDefs()
{
    StringHash xlinkToReplace;
    QList<SvgElement> defsList = defsElement().childElemList();

    // using of structure is faster than actual node accessing
    QList<DefsElemStruct> elemStructList;
    for (int i = 0; i < defsList.count(); ++i) {
        SvgElement elem = defsList.at(i);
        QString tagName = elem.tagName();

        if (tagName == "linearGradient" || tagName == "radialGradient"
                || tagName == "filter" || tagName == "clipPath") {
            StringMap map = elem.attributesMap(true);

            // prepare attributes
            if (tagName == "linearGradient") {
                if (   !map.contains("x1") && Tools::isZero(elem.doubleAttribute("x2"))
                    && !map.contains("y1") && !map.contains("y2")) {
                    map.remove("gradientTransform");
                }
                if (map.contains("gradientTransform")) {
                    Transform gts(map.value("gradientTransform"));
                    if (!gts.isMirrored() && gts.isProportionalScale()) {
                        gts.setOldXY(map.value("x1").toDouble(),
                                     map.value("y1").toDouble());
                        map.insert("x1", Tools::roundNumber(gts.newX()));
                        map.insert("y1", Tools::roundNumber(gts.newY()));
                        gts.setOldXY(map.value("x2").toDouble(),
                                     map.value("y2").toDouble());
                        map.insert("x2", Tools::roundNumber(gts.newX()));
                        map.insert("y2", Tools::roundNumber(gts.newY()));
                        map.remove("gradientTransform");
                    } else {
                        map.insert("gradientTransform", gts.simplified());
                    }
                }
                if (map.contains("x2") || map.contains("y2")) {
                    if (Tools::isZero(map.value("x1").toDouble() - map.value("x2").toDouble())) {
                        map.remove("x1");
                        map.insert("x2", "0");
                    }
                    if (Tools::isZero(map.value("y1").toDouble() - map.value("y2").toDouble())) {
                        map.remove("y1");
                        map.remove("y2");
                    }
                }
            } else if (tagName == "radialGradient") {
                if (map.contains("gradientTransform")) {
                    Transform gts(elem.attribute("gradientTransform"));
                    if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                        gts.setOldXY(map.value("fx").toDouble(),
                                     map.value("fy").toDouble());
                        if (map.contains("fx"))
                            map.insert("fx", Tools::roundNumber(gts.newX()));
                        if (map.contains("fy"))
                            map.insert("fy", Tools::roundNumber(gts.newY()));
                        gts.setOldXY(map.value("cx").toDouble(),
                                     map.value("cy").toDouble());
                        map.insert("cx", Tools::roundNumber(gts.newX()));
                        map.insert("cy", Tools::roundNumber(gts.newY()));

                        map.insert("r", Tools::roundNumber(map.value("r").toDouble()
                                                                  * gts.scaleFactor()));
                        map.remove("gradientTransform");
                    } else {
                        map.insert("gradientTransform", gts.simplified());
                    }
                }
                qreal fx = map.value("fx").toDouble();
                qreal fy = map.value("fy").toDouble();
                qreal cx = map.value("cx").toDouble();
                qreal cy = map.value("cy").toDouble();
                if (Tools::isZero(qAbs(fx-cx)))
                    map.remove("fx");
                if (Tools::isZero(qAbs(fy-cy)))
                    map.remove("fy");
            }

            QList<StringMap> stopAttrs;
            foreach (SvgElement stopElem, elem.childElemList())
                stopAttrs << stopElem.attributesMap(true);

            QStringList tmpList;
            if (tagName == "filter") {
                tmpList.reserve(Props::filter.size());
                foreach (const QString &attrName, Props::filter) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            } else if (tagName == "linearGradient") {
                tmpList.reserve(Props::linearGradient.size());
                foreach (const QString &attrName, Props::linearGradient) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            } else if (tagName == "radialGradient") {
                tmpList.reserve(Props::radialGradient.size());
                foreach (const QString &attrName, Props::radialGradient) {
                    if (map.contains(attrName))
                        tmpList << map.value(attrName);
                }
            }

            DefsElemStruct es = { elem, tagName, elem.hasChildren(), map, tmpList, elem.id() };
            elemStructList << es;
        }
    }

    // process gradients
    for (int i = 0; i < elemStructList.count(); ++i) {
        DefsElemStruct des1 = elemStructList.at(i);
        QString id1 = des1.id;
        if (des1.tagName == QLatin1String("linearGradient") || des1.tagName == "radialGradient") {
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
                                isRemove = Tools::isGradientsEqual(des1.elem, des2.elem);
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
                                    if (elemStructList.at(e).attrMap.value("xlink:href").mid(1) == id2)
                                        elemStructList[e].attrMap.insert("xlink:href", "#" + id1);
                                }
                                j--;
                            }
                        }
                    }
                }
            }
        } else if (des1.tagName == "filter") {
            // process feGaussianBlur filter
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                QString id2 = des2.id;
                if (des1.tagName == des2.tagName && id1 != id2
                    && des1.elem.childElementCount() == 1 && des2.elem.childElementCount() == 1)
                {
                    if (des1.attrList == des2.attrList) {
                        SvgElement child1 = des1.elem.firstChild();
                        SvgElement child2 = des2.elem.firstChild();
                        if (   child1.tagName() == "feGaussianBlur"
                            && child2.tagName() == "feGaussianBlur") {
                            if (child1.attribute("stdDeviation")
                                    == child2.attribute("stdDeviation"))
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
        } else if (des1.tagName == "clipPath") {
            // process clipPath
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                QString id2 = des2.id;
                if (des1.tagName == des2.tagName && id1 != id2
                    && des1.elem.childElementCount() == 1 && des2.elem.childElementCount() == 1)
                {
                    SvgElement child1 = des1.elem.firstChild();
                    SvgElement child2 = des2.elem.firstChild();
                    if (child1.tagName() == child2.tagName()
                        && child1.tagName() == "path"
                        && child1.attribute("d") == child2.attribute("d"))
                    {
                        if (child1.attribute("transform") == child2.attribute("transform")) {
                            xlinkToReplace.insert(id2, id1);
                            defsElement().removeChild(des2.elem);
                            elemStructList.removeAt(j);
                            j--;
                        } else {
                            child2.setTagName("use");
                            child2.removeAttribute("d");
                            child2.setAttribute("xlink:href", "#" + child1.attribute("id"));
                            child1.setAttribute(CleanerAttr::UsedElement, "1");
                            if (child1.parentElement().tagName() == "clipPath") {
                                SvgElement newUse(document()->NewElement("use"));
                                newUse.setAttribute("xlink:href", "#" + child1.attribute("id"));
                                newUse.setAttribute("transform", child1.attribute("transform"));
                                child1.removeAttribute("transform");
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

void Remover::removeUnreferencedIds()
{
    // find
    StringSet m_allIdList;
    StringSet m_allLinkList;

    QStringList xlinkAttrList;
    xlinkAttrList << "xlink:href" << "inkscape:path-effect" << "inkscape:perspectiveID";

    CharList urlAttrList = Props::linkableStyleAttributes;

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QStringList attrList = elem.attributesList();

        // collect all id's
        if (attrList.indexOf("id") != -1)
            m_allIdList << elem.id();

        foreach (const QString &attr, xlinkAttrList) {
            if (attrList.indexOf(attr) != -1)
                m_allLinkList << elem.attribute(attr).remove(0,1);
        }

        foreach (const QString &attr, urlAttrList) {
            if (attrList.indexOf(attr) != -1) {
                QString attrValue = elem.attribute(attr);
                if (attrValue.contains("url")) {
                    attrValue = attrValue.mid(5, attrValue.size()-6);
                    m_allLinkList << attrValue;
                }
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
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

    // remove
    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();

        if (m_allIdList.contains(currElem.id()))
            currElem.removeAttribute("id");

        if (m_allIdList.contains(currElem.attribute("clip-path")))
            currElem.removeAttribute("id");

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Remover::removeElements()
{
    bool isAnyRemoved = true;
    while (isAnyRemoved) {
        isAnyRemoved = false;

        // have to use XMLNode insted of SvgElement, because after converting to element
        // detecting and removing of "comment" or "processing instruction" is impossible
        QList<XMLNode *> nodeList = Tools::childNodeList(document());
        while (!nodeList.isEmpty()) {
            XMLNode *currNode = nodeList.takeFirst();
            SvgElement currElem = SvgElement(currNode->ToElement());

            bool removeThisNode = false;
            if (!currElem.isNull()) {
                QString currTag = currElem.tagName();
                if (   (currElem.isContainer()
                        || currTag == "flowRegion")
                    && !currElem.hasChildren()
                    && currTag != "glyph"
                    && Keys.flag(Key::RemoveEmptyContainers))
                    removeThisNode = true;
                else if (currTag.contains("metadata")
                         && Keys.flag(Key::RemoveMetadata))
                    removeThisNode = true;
                else if (currTag.contains("sodipodi")
                         && Keys.flag(Key::RemoveSodipodiElements))
                    removeThisNode = true;
                else if (   currTag.contains("inkscape")
                         && currTag != "inkscape:path-effect"
                         && Keys.flag(Key::RemoveInkscapeElements))
                    removeThisNode = true;
                else if ((   currTag.startsWith("a:")
                          || currTag.startsWith("i:"))
                         && Keys.flag(Key::RemoveAdobeElements))
                    removeThisNode = true;
                else if (currTag.startsWith("v:")
                         && Keys.flag(Key::RemoveMSVisioElements))
                    removeThisNode = true;
                else if (currTag.startsWith("c:")
                         && Keys.flag(Key::RemoveCorelDrawElements))
                    removeThisNode = true;
                else if (currTag.startsWith("sketch:")
                         && Keys.flag(Key::RemoveSketchElements))
                    removeThisNode = true;
                else if (currTag == "foreignObject"
                         && Keys.flag(Key::RemoveInvisibleElements))
                    removeThisNode = true;
                else if (currTag == "use"
                         && (!currElem.hasAttribute("xlink:href") ||
                             findElement(currElem.xlinkId()).isNull())
                         && Keys.flag(Key::RemoveInvisibleElements))
                    removeThisNode = true;
                else if (!Props::svgElementList.contains(currTag)
                         && currNode->ToText() == 0
                         && Keys.flag(Key::RemoveNonSvgElements))
                    removeThisNode = true;

                if (Keys.flag(Key::RemoveInvisibleElements)) {
                    if (currTag == "title")
                        removeThisNode = true;
                    else if (currTag == "desc")
                        removeThisNode = true;
                    else if (currTag == "script")
                        removeThisNode = true;
                    else if ((     currTag == "linearGradient"
                                || currTag == "radialGradient")
                             && !currElem.hasChildren()
                             && !currElem.hasAttribute("xlink:href")) {
                        removeThisNode = true;
                    } else if (    currTag == "image"
                               && !currElem.attribute("xlink:href").startsWith("data"))
                        removeThisNode = true;
                    else if (    currElem.isReferenced()
                             && !currElem.hasAttribute("id")
                             &&  currElem.parentElement().tagName() == "defs")
                        removeThisNode = true;
                    else if (isElementInvisible(currElem))
                        removeThisNode = true;
                }
            }

            if (currNode->ToComment() != 0
                && Keys.flag(Key::RemoveComments))
                removeThisNode = true;
            else if (currNode->ToDeclaration() != 0
                     && Keys.flag(Key::RemoveProcInstruction))
                removeThisNode = true;
            else if (currNode->Value()[0] != '\0'
                     && isDoctype(QString(currNode->Value()))
                     && Keys.flag(Key::RemoveProlog))
                removeThisNode = true;

            if (removeThisNode) {
                currNode->Parent()->DeleteChild(currNode);
                isAnyRemoved = true;
            }

            if (!currNode->NoChildren())
                nodeList << Tools::childNodeList(currNode);
        }
    }

    // ungroup "a" element
    QList<SvgElement> elemList = svgElement().childElemList();
    while (!elemList.isEmpty()) {
        SvgElement currElem = elemList.takeFirst();
        if (currElem.tagName() == "a") {
            if ((currElem.hasAttribute("id") && currElem.attributesCount() == 1)
                || currElem.attributesCount() == 0)
            {
                foreach (const SvgElement &childElem, currElem.childElemList())
                    currElem.parentElement().insertBefore(childElem, currElem);
                currElem.parentElement().removeChild(currElem);
                currElem.clear();
            }
        }
        if (!currElem.isNull())
            if (currElem.hasChildren())
                elemList << currElem.childElemList();
    }

    // TODO: add switch element ungroup

    qreal stdDevLimit = Keys.doubleNumber(Key::RemoveTinyGaussianBlur);
    if (stdDevLimit != 0) {
        elemList = defsElement().childElemList();
        while (!elemList.isEmpty()) {
            SvgElement currElem = elemList.takeFirst();
            if (currElem.tagName() == "feGaussianBlur") {
                // FIXME: check for stdDeviation with transform of all linked element applied
                if (stdDevLimit != 0.0) {
                    if (currElem.parentElement().childElementCount() == 1) {
                        // 'stdDeviation' can contains not only one value
                        // we process it when it contains only one value
                        QString stdDev = currElem.attribute("stdDeviation");
                        if (!stdDev.contains(",") && !stdDev.contains(" ")) {
                            bool ok = true;
                            if (stdDev.toDouble(&ok) <= stdDevLimit) {
                                Q_ASSERT(ok == true);
                                defsElement().removeChild(currElem.parentElement());
                            }
                        }
                    }
                }
            }
            if (currElem.hasChildren())
                elemList << currElem.childElemList();
        }
    }
}

bool Remover::isDoctype(const QString &str)
{
    if (str.at(0).isLower() || str.size() < 2)
        return false;
    if (str.startsWith(QLatin1Char('!')))
        return true;
    if (str.startsWith(QLatin1String("DOCTYPE")))
        return true;
    if (str.startsWith(QLatin1String("ENTITY")))
        return true;
    if (str.startsWith(QLatin1String("]>")) || str.startsWith(QLatin1String("\n]>")))
        return true;
    return false;
}

void Remover::removeElementsFinal()
{
    if (!Keys.flag(Key::RemoveInvisibleElements))
        return;

    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (isElementInvisible2(elem))
            elem.parentElement().removeChild(elem);
        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

bool Remover::isElementInvisible2(SvgElement &elem)
{
    // if 'fill' or 'stroke' linked to non existing define - set attribute value to 'none'
    if (elem.isContainer())
        return false;

    SvgElement parent = elem.parentElement();
    bool hasWrongParent = false;
    while (!parent.isNull()) {
        if (parent.isTagName("defs") || parent.isTagName("switch")) {
            hasWrongParent = true;
            break;
        }
        parent = parent.parentElement();
    }
    if (!hasWrongParent) {
        // elements with no 'fill' and 'stroke' are invisible
        if (    findAttribute(elem, "fill") == "none"
            && (   findAttribute(elem, "stroke") == "none"
                || findAttribute(elem, "stroke").isEmpty())
            && !elem.isUsed()) {
            if (elem.hasAttribute("filter")) {
                SvgElement filterElem = findDefElement(elem.defIdFromAttribute("filter"));
                if (filterElem.childElementCount() == 1) {
                    if (filterElem.firstChild().tagName() == "feGaussianBlur") {
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

bool Remover::isElementInvisible(SvgElement &elem)
{
    QString tagName = elem.tagName();
    //remove elements "rect", "pattern" and "image" with height or width <= 0
    if (tagName == "rect" || tagName == "pattern" || tagName == "image") {
        if (elem.hasAttribute("width") && elem.hasAttribute("height")) {
            QRectF rect = Tools::viewBoxRect(svgElement());
            bool ok = false;
            qreal width  = Tools::convertUnitsToPx(elem.attribute("width"), rect.width()).toDouble(&ok);
            Q_ASSERT(ok == true);
            qreal height = Tools::convertUnitsToPx(elem.attribute("height"), rect.height()).toDouble(&ok);
            Q_ASSERT(ok == true);
            if (width <= 0 || height <= 0)
                return true;
        }
    }

    // TODO: ungroup flowPara with only "id" attr
    // ryanlerch_OCAL_Introduction.svg
//    if (tagName == "flowPara") {
//        if (!elem.hasText() && !elem.hasChildren())
//            return true;
//    }

    if (isElementInvisible2(elem))
        return true;

    // remove elements with opacity="0"
    if (elem.hasAttribute("opacity")) {
        if (elem.doubleAttribute("opacity") <= 0) {
            return true;
        }
    }

    // remove elements with "display=none"
    if (elem.hasAttribute("display")) {
        if (elem.attribute("display") == "none")
            return true;
    }

    if (elem.hasAttribute("visibility")) {
        if (elem.attribute("visibility") == "hidden")
            return true;
    }

    // remove "path" elements with empty "d" attr
    if (tagName == "path")
        if (elem.attribute("d").isEmpty())
            return true;

    // A negative value is an error. A value of zero disables rendering of this element.
    if (tagName == "use") {
        if (elem.hasAttribute("width"))
            if (elem.doubleAttribute("width") == 0)
                return true;
        if (elem.hasAttribute("height"))
            if (elem.doubleAttribute("height") == 0)
                return true;
    }

    // remove "polygon", "polyline" elements with empty "points" attr
    if (tagName == "polygon" || tagName == "polyline")
        if (elem.attribute("points").isEmpty())
            return true;

    // remove "circle" elements with "r" <= 0
    if (tagName == "circle")
        if (elem.attribute("r").toDouble() <= 0)
            return true;

    // remove "ellipse" elements with "rx|ry" <= 0
    if (tagName == "ellipse")
        if (   elem.attribute("rx").toFloat() <= 0
            || elem.attribute("ry").toFloat() <= 0)
            return true;

    // remove "switch" with no attributes or with only "id" attribute
    if (tagName == "switch" && !elem.hasChildren()) {
        if (elem.attributesCount() == 0)
            return true;
        else if (elem.hasAttribute("id") && elem.attributesCount() == 1)
            return true;
    }

    return false;
}

// TODO: process '*-rendering' attribute
void Remover::removeAttributes()
{
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QStringList baseAttrList = elem.attributesList();
        if (!baseAttrList.isEmpty()) {
            QStringList attrList = baseAttrList;
            // sodipodi:type="inkscape:offset" supported only by inkscape,
            // and its creates problems in other renders
            foreach (const QString &attrName, attrList) {
                if (attrName.startsWith("inkscape") && Keys.flag(Key::RemoveInkscapeAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith("sodipodi") && Keys.flag(Key::RemoveSodipodiAttributes))
                    attrList.removeOne(attrName);
                if ((attrName.startsWith("i:") || attrName.startsWith("a:"))
                    && Keys.flag(Key::RemoveAdobeAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith("v:") && Keys.flag(Key::RemoveMSVisioAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith("c:") && Keys.flag(Key::RemoveCorelDrawAttributes))
                    attrList.removeOne(attrName);
                if (attrName.startsWith("sketch:") && Keys.flag(Key::RemoveSketchAttributes))
                    attrList.removeOne(attrName);
            }

            if (Keys.flag(Key::RemoveDefaultAttributes)) {
                if (elem.attribute("spreadMethod") == "pad")
                    attrList.removeOne("spreadMethod");
                if (elem.tagName() == "clipPath") {
                    if (elem.attribute("clipPathUnits") == "userSpaceOnUse")
                        attrList.removeOne("clipPathUnits");
                }
            }
            if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
                if (attrList.contains("desc"))
                    attrList.removeOne("desc");
                // xlink:href could not contains uri with spaces
                if (attrList.contains("xlink:href")) {
                    QString xlink = elem.attribute("xlink:href");
                    if (xlink.indexOf(' ') != -1 && !xlink.startsWith("data:"))
                        attrList.removeOne("xlink:href");
                }
                if (!elem.isTagName("svg")) {
                    foreach (const QString &attrName, attrList) {
                        if (attrName.startsWith("xmlns"))
                            attrList.removeOne(attrName);
                    }
                }

                if (attrList.contains("marker"))
                    if (elem.attribute("marker") == "none")
                        elem.removeAttribute("marker");

                // path inside clipPath needs to contains only d attribute
                // TODO: remove all style props from path used only in clipPath or mask
                if (elem.tagName() == "path" || elem.tagName() == "use") {
                    QString parentTag = elem.parentElement().tagName();
                    if (parentTag == "clipPath"/* || parentTag == "defs"*/) {
                        foreach (const QString &attrName, attrList) {
                            bool removeAttr = true;
                            if (attrName == "d" || attrName == "transform" || attrName == "id")
                                removeAttr = false;
                            if (elem.tagName() == "use" && attrName == "xlink:href")
                                removeAttr = false;
                            if (removeAttr)
                                attrList.removeOne(attrName);
                        }
                    }
                }

                static CharList strokeAndFill = CharList() << "fill" << "stroke";
                foreach (const char *attrName, strokeAndFill) {
                    if (attrList.contains(attrName)) {
                        QString url = elem.attribute(attrName);
                        if (url.startsWith("url(")) {
                            SvgElement defElem = findDefElement(url.mid(5, url.size()-6));
                            if (defElem.isNull()) {
                                elem.setAttribute(attrName, "none");
                            }
                        }
                    }
                }

                // remove all text based values from non text elements
                if (!elem.isText()) {
                    foreach (const QString &attrName, attrList) {
                        if (   attrName.contains("font")
                            || attrName.contains("text")
                            || Props::textAttributes.contains(attrName))
                        {
                            attrList.removeOne(attrName);
                        }
                    }
                    attrList.removeOne("writing-mode");
                    attrList.removeOne("line-height");
                    attrList.removeOne("block-progression");
                }
            }


            if (elem.tagName() == "stop") {
                if (elem.doubleAttribute("offset") < 0.0001)
                    attrList.removeOne("offset");
            }

            // TODO: 'display' attr remove

            foreach (const QString &attrName, baseAttrList) {
                if (attrList.indexOf(attrName) == -1)
                    elem.removeAttribute(attrName);
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }

    // remove xml:space when no child has multispace text
    list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (elem.hasAttribute("xml:space")) {
            bool canRemove = true;
            QList<XMLNode *> list2 = Tools::childNodeList(elem.xmlElement());
            while (!list2.isEmpty()) {
                XMLNode *elem2 = list2.takeFirst();
                QString tagName = QLatin1String(elem2->Value());
                if (tagName == "tspan" || tagName == "flowPara") {
                    if (elem2->FirstChild() != 0) {
                        if (elem2->FirstChild()->ToText() != 0) {
                            if (QString(elem2->FirstChild()->ToText()->Value()).contains("  ")) {
                                canRemove = false;
                                break;
                            }
                        }
                    }
                }
                if (!elem2->NoChildren())
                    list2 << Tools::childNodeList(elem2);
            }
            if (canRemove)
                elem.removeAttribute("xml:space");
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }

    removeNonElementAttributes();
}

void Remover::removeNonElementAttributes()
{
    StringSet circle;
    circle << "transform" << "cx" << "cy" << "r" << "id";
    circle.unite(Props::presentationAttributes);

    StringSet ellipse;
    ellipse << "transform" << "cx" << "cy" << "rx" << "ry" << "id";
    ellipse.unite(Props::presentationAttributes);

    QList<SvgElement> elemList = svgElement().childElemList();
    while (!elemList.isEmpty()) {
        SvgElement elem = elemList.takeFirst();
        QString tagName = elem.tagName();
        if (tagName == "circle" || tagName == "ellipse") {
            foreach (const QString &attrName, elem.attributesList()) {
                if (!circle.contains(attrName) && tagName == "circle")
                    elem.removeAttribute(attrName);
                else if (!ellipse.contains(attrName) && tagName == "ellipse")
                    elem.removeAttribute(attrName);
            }
        }
        if (elem.hasChildren())
            elemList << elem.childElemList();
    }
}

void Remover::cleanPresentationAttributes(SvgElement elem)
{
    if (elem.isNull())
        elem = svgElement();

    styleHashList << elem.styleMap();
    parentHash.unite(styleHashList.last());
    parentAttrs = parentHash.keys().toSet();

    QList<SvgElement> list = elem.childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        StringMap hash = currElem.styleMap();
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
        if (currElem.hasChildren())
            cleanPresentationAttributes(currElem);
    }

    styleHashList.removeLast();
    parentHash.clear();
    foreach (const StringMap &hash, styleHashList)
        parentHash.unite(hash);
}

// removes default value, only if parent style did't contain same attribute
// needed for all inherited attributes
void Remover::cleanStyle(const SvgElement &elem, StringMap &hash)
{
    static bool isRemoveNotApplied = Keys.flag(Key::RemoveNotAppliedAttributes);    
    static bool isConvertColors
            = (Keys.flag(Key::ConvertColorToRRGGBB) || Keys.flag(Key::ConvertRRGGBBToRGB));

    if (isRemoveNotApplied) {
        // remove style props which already defined in parent style
        foreach (const QString &attr, parentHash.keys()) {
            if (attr != "opacity" && !elem.isUsed()) {
                if (hash.contains(attr))
                    if (hash.value(attr) == parentHash.value(attr))
                        hash.remove(attr);
            }
        }
    }

    // convert units
    static QStringList numericStyleList
            = QStringList() << "fill-opacity" << "opacity" << "stop-opacity" << "stroke-miterlimit"
                            << "stroke-opacity" << "stroke-width" << "font-size" << "kerning"
                            << "letter-spacing" << "word-spacing" << "baseline-shift"
                            << "stroke-dashoffset";
    foreach (const QString &key, numericStyleList) {
        QString value = hash.value(key);
        if (!value.isEmpty()) {
            bool ok = false;
            qreal num = value.toDouble(&ok);
            if (!ok && !value.startsWith("url(")) {
                hash.insert(key, Tools::convertUnitsToPx(value));
                num = hash.value(key).toDouble();
            }
            hash.insert(key, Tools::roundNumber(num, Tools::ATTRIBUTE));
        }
    }

    // remove all fill properties if fill is off
    static bool isRemoveFillProps = Keys.flag(Key::RemoveFillProps);
    if (isRemoveFillProps
        && parentAttrs.contains("fill")
        && parentHash.value("fill") == "none"
        && (hash.value("fill") == "none" || hash.value("fill-opacity") == "0"))
    {
        foreach (const QString &attr, Props::fillList)
            hash.remove(attr);
    } else if (isConvertColors) {
        QString fill = hash.value("fill");
        if (!fill.isEmpty() && fill != "none" && !fill.startsWith("url"))
            hash.insert("fill", Tools::trimColor(fill));
    }

    // remove all stroke properties if stroke is off
    static bool isRemoveStrokeProps = Keys.flag(Key::RemoveStrokeProps);
    if (isRemoveStrokeProps
        && (   hash.value("stroke") == "none"
            || hash.value("stroke-opacity") == "0"
            || hash.value("stroke-width") == "0")) {
        foreach (const QString &attr, Props::strokeList)
            hash.remove(attr);
        if (parentHash.value("stroke") != "none")
            hash.insert("stroke", "none");
    } else {
        if (isConvertColors) {
            QString stroke = hash.value("stroke");
            if (   !stroke.isEmpty()
                &&  stroke != "none"
                && !stroke.startsWith("url"))
            {
                hash.insert("stroke", Tools::trimColor(hash.value("stroke")));
            }
        }
        // trim array
        if (hash.contains("stroke-dasharray"))
            hash.insert("stroke-dasharray", QString(hash.value("stroke-dasharray"))
                                             .replace(", ", ","));
    }

    if (isRemoveNotApplied) {
        hash.remove("pointer-events");

        // remove clip-rule if elem not inside clipPath
        if (hash.contains("clip-rule")) {
            bool isElemInsideClipPath = false;
            SvgElement parent = elem.parentElement();
            while (!parent.isNull()) {
                if (parent.tagName() == "clipPath") {
                    isElemInsideClipPath = true;
                    break;
                }
                parent = parent.parentElement();
            }
            if (!isElemInsideClipPath)
                hash.remove("clip-rule");
        }

        // 'enable-background' is only applicable to container elements
        if (!elem.isContainer())
            hash.remove("enable-background");

        if (!elem.isTagName("svg") && !elem.isTagName("pattern") && !elem.isTagName("marker"))
            hash.remove("overflow");
    }

    static bool isRemoveInkscapeAttributes = Keys.flag(Key::RemoveInkscapeAttributes);
    if (isRemoveInkscapeAttributes)
        hash.remove("-inkscape-font-specification");

    static bool isRemoveDefaultAttributes = Keys.flag(Key::RemoveDefaultAttributes);
    if (isRemoveDefaultAttributes) {
        foreach (const QString &attrName, hash.keys())
            removeDefaultValue(hash, attrName);
    }

    // trim colors
    if (isConvertColors) {
        foreach (const QString &attrName, QStringList() << "color" << "stop-color" << "flood-color") {
            if (hash.contains(attrName))
                hash.insert(attrName, Tools::trimColor(hash.value(attrName)));
        }
    }
}

void Remover::removeDefaultValue(StringMap &hash, const QString &name)
{
    if (parentAttrs.contains(name))
        return;

    if (name == "fill" || name == "stop-color") {
        static QStringList defValues = QStringList() << "#000" << "#000000" << "black";
        if (defValues.contains(hash.value(name)))
            hash.remove(name);
    } else if (Props::defaultStyleValues.contains(name)) {
        const QVariant value = Props::defaultStyleValues.value(name);
        if (value.type() == QVariant::String) {
            if (value == hash.value(name))
                hash.remove(name);
        } else if (!hash.value(name).isEmpty()
                   && Tools::strToDouble(hash.value(name)) == value.toDouble()) {
            hash.remove(name);
        }
    }
}

void Remover::removeGroups()
{
    QStringList illegalGAttrList;
    illegalGAttrList << "mask" << "clip-path" << "filter";

    bool isAnyGroupRemoved = true;
    while (isAnyGroupRemoved) {
        isAnyGroupRemoved = false;
        QList<SvgElement> list = svgElement().childElemList();
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            SvgElement parent = elem.parentElement();
            if (elem.isGroup() && elem.childElementCount() == 0) {
                // remove empty group
                parent.removeChild(elem);
            } else if (   elem.isGroup()
                       && elem.parentElement().tagName() != "switch")
            {
                if (   !elem.hasImportantAttrs()
                    && !elem.firstChild().hasAttribute("mask"))
                {
                    // ungroup group without attributes
                    foreach (SvgElement childElem, elem.childElemList())
                        parent.insertBefore(childElem, elem);
                    parent.removeChild(elem);
                    isAnyGroupRemoved = true;
                } else if (   !elem.isUsed()
                           && parent.isGroup()
                           && !elem.hasAttributes(illegalGAttrList))
                {
                    SvgElement firstChild = elem.firstChild();
                    if (elem.childElementCount() == 1
                        && firstChild.tagName() != "switch")
                    {
                        // ungroup group with only one child
                        parent.insertBefore(firstChild, elem);
                        megreGroups(elem, firstChild, false);
                        parent.removeChild(elem);
                        isAnyGroupRemoved = true;
                    } else if (    parent.childElementCount() == 1
                               &&  parent.tagName() != "svg"
                               && !parent.hasAttributes(illegalGAttrList))
                    {
                        megreGroups(elem, parent, true);
                        foreach (SvgElement childElem, elem.childElemList())
                            parent.insertBefore(childElem, elem);
                        parent.removeChild(elem);
                        isAnyGroupRemoved = true;
                    }
                }
            }
            if (elem.hasChildren())
                list << elem.childElemList();
        }
    }
}

void Remover::megreGroups(SvgElement parentElem, SvgElement childElem, bool isParentToChild)
{
    QStringList ignoreAttrList = QStringList() << "id";
    if (!isParentToChild)
        ignoreAttrList << "stroke" << "stroke-width" << "fill";

    foreach (const QString &attrName, parentElem.attributesList()) {
        if (childElem.hasAttribute(attrName) && attrName == "transform") {
            childElem.setTransform(parentElem.attribute(attrName), !isParentToChild);
        } else if (attrName == "opacity") {
            if (parentElem.hasAttribute("opacity") && childElem.hasAttribute("opacity")) {
                qreal newOp = parentElem.doubleAttribute("opacity")
                              * childElem.doubleAttribute("opacity");
                childElem.setAttribute("opacity", Tools::roundNumber(newOp));
            } else {
                childElem.setAttribute(attrName, parentElem.attribute(attrName));
            }
        } else if (!ignoreAttrList.contains(attrName) || !childElem.hasAttribute(attrName)) {
            childElem.setAttribute(attrName, parentElem.attribute(attrName));
        }
    }
}
