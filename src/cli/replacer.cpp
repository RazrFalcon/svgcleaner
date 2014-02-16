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
    if (!svgElement().hasAttribute("viewBox")) {
        if (svgElement().hasAttribute("width") && svgElement().hasAttribute("height")) {
            QString width  = Tools::roundNumber(svgElement().doubleAttribute("width"));
            QString height = Tools::roundNumber(svgElement().doubleAttribute("height"));
            svgElement().setAttribute("viewBox", QString("0 0 %1 %2").arg(width).arg(height));
            svgElement().removeAttribute("width");
            svgElement().removeAttribute("height");
        }
    } else {
        QRectF rect = viewBoxRect();
        if (rect.isNull())
            return;
        if (svgElement().hasAttribute("width")) {
            if (Tools::isZero(rect.width() - svgElement().doubleAttribute("width")))
                svgElement().removeAttribute("width");
        }
        if (svgElement().hasAttribute("height")) {
            if (Tools::isZero(rect.height() - svgElement().doubleAttribute("height")))
                svgElement().removeAttribute("height");
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

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        bool removed = false;
        if (elem.tagName() == "path") {
            bool canApplyTransform = false;
            if (Keys.flag(Key::ApplyTransformsToPaths))
                canApplyTransform = isPathValidToTransform(elem, defsHash);
            bool isPathApplyed = false;
            Path().processPath(elem, canApplyTransform, &isPathApplyed);
            if (canApplyTransform) {
                if (isPathApplyed) {
                    updateLinkedDefTransform(elem);
                    elem.removeAttribute("transform");
                }
            }
            if (elem.attribute("d").isEmpty()) {
                elem.parentElement().removeChild(elem);
                removed = true;
            }
        }

        if (!removed) {
            if (elem.hasChildren())
                list << elem.childElemList();
        }
    }
}

bool Replacer::isPathValidToTransform(SvgElement &pathElem, QHash<QString,int> &defsIdHash)
{
    if (pathElem.hasAttribute("transform")) {
        // non proportional transform could not be applied to path with stroke
        bool hasStroke = false;
        SvgElement parentElem = pathElem;
        while (!parentElem.isNull()) {
            if (parentElem.hasAttribute("stroke")) {
                if (parentElem.attribute("stroke") != "none") {
                    hasStroke = true;
                    break;
                }
            }
            parentElem = parentElem.parentElement();
        }
        if (hasStroke) {
            Transform ts(pathElem.attribute("transform"));
            if (!ts.isProportionalScale())
                return false;
        }
    } else {
        return false;
    }
    if (pathElem.hasAttribute("clip-path") || pathElem.hasAttribute("mask"))
        return false;
    if (pathElem.isUsed())
        return false;
    if (pathElem.hasAttribute("filter")) {
        // we can apply transform to blur filter, but only when it's used by only this path
        QString filterId = pathElem.defIdFromAttribute("filter");
        if (defsIdHash.value(filterId) > 1)
            return false;
        if (!isBlurFilter(filterId))
            return false;
    }

    QStringList attrList;
    attrList << "fill" << "stroke";
    foreach (const QString &attrName, attrList) {
        if (pathElem.hasAttribute(attrName)) {
            QString defId = pathElem.defIdFromAttribute(attrName);
            if (!defId.isEmpty()) {
                if (defsIdHash.value(defId) > 1)
                    return false;
            }
            if (!defId.isEmpty()) {
                SvgElement defElem = findDefElement(defId);
                if (!defElem.isNull()) {
                    if (   defElem.tagName() != "linearGradient"
                        && defElem.tagName() != "radialGradient")
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
    QStringList filterAttrs;
    filterAttrs << "x" << "y" << "width" << "height";
    QList<SvgElement> list = defsElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == "filter") {
            if (elem.id() == id) {
                if (elem.childElementCount() == 1) {
                    if (elem.firstChild().tagName() == "feGaussianBlur") {
                        // cannot apply transform to filter with not default region
                        if (!elem.hasAttributes(filterAttrs))
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

void Replacer::updateLinkedDefTransform(SvgElement &elem)
{
    QStringList attrList;
    attrList << "fill" << "stroke" << "filter";
    foreach (const QString &attrName, attrList) {
        QString defId = elem.defIdFromAttribute(attrName);
        if (!defId.isEmpty()) {
            SvgElement defElem = findDefElement(defId);
            if (!defElem.isNull()) {
                if ((  defElem.tagName() == "linearGradient"
                    || defElem.tagName() == "radialGradient")
                       && defElem.attribute("gradientUnits") == "userSpaceOnUse")
                {
                    QString gradTs = defElem.attribute("gradientTransform");
                    if (!gradTs.isEmpty()) {
                        Transform ts(elem.attribute("transform") + " " + gradTs);
                        defElem.setAttribute("gradientTransform", ts.simplified());
                    } else {
                        defElem.setAttribute("gradientTransform", elem.attribute("transform"));
                    }
                } else if (defElem.tagName() == "filter") {
                    Transform ts(elem.attribute("transform"));
                    SvgElement stdDevElem = defElem.firstChild();
                    qreal oldStd = stdDevElem.doubleAttribute("stdDeviation");
                    QString newStd = Tools::roundNumber(oldStd * ts.scaleFactor());
                    stdDevElem.setAttribute("stdDeviation", newStd);
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
        return elem.elem.xmlElement() == this->elem.xmlElement();
    }
};

// FIXME: parent styles should be set to elem before moving to defs
//        address-book-new.svg
// TODO: reuse groups
void Replacer::replaceEqualElementsByUse()
{
    QStringList rectAttrs;
    rectAttrs << "x" << "y" << "width" << "height" << "rx" << "ry";
    QStringList circleAttrs;
    circleAttrs << "cx" << "cy" << "r";
    QStringList ellipseAttrs;
    ellipseAttrs << "cx" << "cy" << "rx" << "ry";
    QList<SvgElement> list = svgElement().childElemList();
    int newAttrId = 0;

    QList<EqElement> elemList;
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        bool canUse = false;
        if (elem.isTagName("path") || elem.isTagName("rect"))
            canUse = true;
        if (hasParent(elem, "defs")) {
            if (!elem.parentElement().isTagName("defs"))
                canUse = false;
        }
        if (canUse) {
            EqElement e;
            e.tagName = elem.tagName();
            StringHash hash;
            if (e.tagName == "path")
                hash.insert("d", elem.attribute("d"));
            else if (e.tagName == "rect") {
                foreach (const QString &attrName, rectAttrs)
                    hash.insert(attrName, elem.attribute(attrName));
            } else if (e.tagName == "circle") {
                foreach (const QString &attrName, circleAttrs)
                   hash.insert(attrName, elem.attribute(attrName));
            } else if (e.tagName == "ellipse") {
                foreach (const QString &attrName, ellipseAttrs)
                    hash.insert(attrName, elem.attribute(attrName));
            }
            e.attrHash = hash;
            e.elem = elem;
            elemList << e;
        }
        if (elem.hasChildren())
            list << elem.childElemList();
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
            eqElem.setTagName("use");
            foreach (const QString &attrName, mainEqElem.attrHash.keys())
                eqElem.removeAttribute(attrName);
            eqElem.removeAttribute(CleanerAttr::BoundingBox);
            if (!mainElem.hasAttribute("id"))
                mainElem.setAttribute("id", "SVGCleanerId_" + QString::number(newAttrId++));
            eqElem.setAttribute("xlink:href", "#" + mainElem.attribute("id"));
            if (eqElem.attribute("transform") == mainElem.attribute("transform"))
                eqElem.removeAttribute("transform");
            else {
                Transform tr(eqElem.attribute("transform"));
                tr.divide(mainElem.attribute("transform"));
                eqElem.setAttribute("transform", tr.simplified());
            }
            elemList.removeOne(equalElems.first());
        } else if (equalElems.size() > 1) {
            SvgElement newElem = document()->NewElement(ToChar(mainEqElem.tagName));
            newElem.setAttribute("id", "SVGCleanerId_" + QString::number(newAttrId++));
            foreach (const QString &attrName, mainEqElem.attrHash.keys())
                newElem.setAttribute(attrName, mainEqElem.attrHash.value(attrName));
            newElem.setAttribute(CleanerAttr::UsedElement, "1");
            defsElement().insertLast(newElem);
            equalElems << mainEqElem;
            foreach (EqElement eqElem, equalElems) {
                SvgElement elem = eqElem.elem;
                elem.setTagName("use");
                elem.setAttribute("xlink:href", "#" + newElem.attribute("id"));
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
    useAttrs << "x" << "y" << "width" << "height";

    QHash<QString,SvgElement> elemXLink;
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.isTagName("use"))
            elemXLink.insertMulti(elem.xlinkId(), elem);
        if (elem.hasChildren())
            list << elem.childElemList();
    }

    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.isUsed()) {
            QList<SvgElement> usedElemList = elemXLink.values(elem.id());
            // use elem could not overwrite style properties of used element
            foreach (SvgElement usedElem, usedElemList) {
                foreach (const QString &attrName, elem.styleAttributesList()) {
                    usedElem.removeAttribute(attrName);
                }
            }

            if (hasParent(elem, "defs")) {
                QHash<QString, int> attrsCount;
                foreach (const SvgElement &usedElem, usedElemList) {
                    QStringList usedElemAttrs = usedElem.styleAttributesList();
                    if (usedElem.hasAttribute("transform"))
                        usedElemAttrs << "transform";
                    foreach (const QString &attrName, usedElemAttrs) {
                        if (usedElem.hasAttribute(attrName)) {
                            bool isCount = true;
                            if (attrName == "transform"
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
                        && !(attr.startsWith(QL1S("transform"))
                             && (elem.hasAttribute("transform") || attrsCount.value(attr) == 1)))
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

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::convertUnits()
{
    QRectF rect = viewBoxRect();
    if (svgElement().hasAttribute("width")) {
        QString widthStr = svgElement().attribute("width");
        if (widthStr.contains('%') && rect.isNull())
            qFatal("Error: could not convert width in percentage into px without viewBox");
        bool ok;
        qreal width = Tools::convertUnitsToPx(widthStr, rect.width()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert width to px");
        svgElement().setAttribute("width", Tools::roundNumber(width));
    }
    if (svgElement().hasAttribute("height")) {
        QString heightStr = svgElement().attribute("height");
        if (heightStr.contains('%') && rect.isNull())
            qFatal("Error: could not convert height in percentage into px without viewBox");
        bool ok;
        qreal height = Tools::convertUnitsToPx(heightStr, rect.height()).toDouble(&ok);
        if (!ok)
            qFatal("Error: could not convert height to px");
        svgElement().setAttribute("height", Tools::roundNumber(height));
    }

    // TODO: process 'offset' attr with %
    QList<SvgElement> list = childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QString currTag = elem.tagName();
        if (elem.hasAttribute("font-size")) {
            bool ok = false;
            QString fontSizeStr = elem.attribute("font-size");
            fontSizeStr.toDouble(&ok);
            if (!ok) {
                if (   fontSizeStr.endsWith(QL1S("ex"))
                    || fontSizeStr.endsWith(QL1S("em"))
                    || fontSizeStr.endsWith(QL1S("%")))
                {
                    QString parentFontSize = findAttribute(elem.parentElement(), "font-size");
                    if (parentFontSize.isEmpty() || parentFontSize == "0")
                        qFatal("Error: could not calculate relative font-size");
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr, parentFontSize.toDouble());
                    if (newFontSize == "0")
                        elem.removeAttribute("font-size");
                    else
                        elem.setAttribute("font-size", newFontSize);
                } else {
                    QString newFontSize = Tools::convertUnitsToPx(fontSizeStr);
                    elem.setAttribute("font-size", newFontSize);
                }
            }
        }
        foreach (const QString &attrName, elem.attributesListBySet(Props::digitList)) {
            // fix attributes like:
            // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
            // FIXME: ignores list based attr
            QString attrValue = elem.attribute(attrName);
            if (attrValue.contains(" "))
                attrValue = attrValue.left(attrValue.indexOf(" "));

            // TODO: process gradients attrs
            if (attrValue.contains(QL1S("%"))) {
                if (currTag != "radialGradient" && currTag != "linearGradient") {
                    if (attrName.contains("x") || attrName == "width")
                        attrValue = Tools::convertUnitsToPx(attrValue, rect.width());
                    else if (attrName.contains("y") || attrName == "height")
                       attrValue = Tools::convertUnitsToPx(attrValue, rect.height());
                }
            } else if (attrValue.endsWith(QL1S("ex")) || attrValue.endsWith(QL1S("em"))) {
                qreal fontSize = findAttribute(elem, "font-size").toDouble();
                if (fontSize == 0)
                    qFatal("Error: could not convert em/ex values "
                           "without font-size attribute is set.");
                attrValue = Tools::convertUnitsToPx(attrValue, fontSize);
            } else {
                attrValue = Tools::convertUnitsToPx(attrValue);
            }
            elem.setAttribute(attrName, attrValue);
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

// TODO: style can be set in ENTITY
void Replacer::convertCDATAStyle()
{
    QStringList styleList;
    QList<XMLNode *> nodeList = childNodeList(document());
    while (!nodeList.isEmpty()) {
        XMLNode *currNode = nodeList.takeFirst();
        if (currNode->ToElement() != 0) {
            if (!strcmp(currNode->ToElement()->Name(), "style")) {
                if (currNode->FirstChild() != 0)
                    styleList << currNode->FirstChild()->ToText()->Value();
                currNode->Parent()->DeleteChild(currNode);
            }
        }
        if (!currNode->NoChildren())
            nodeList << childNodeList(currNode);
    }
    if (styleList.isEmpty()) {
        // remove class attribute when no CDATA set
        QList<SvgElement> list = childElemList(document());
        while (!list.isEmpty()) {
            SvgElement elem = list.takeFirst();
            elem.removeAttribute("class");
            if (elem.hasChildren())
                list << elem.childElemList();
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

    QList<SvgElement> list = childElemList(document());
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("class")) {
            StringHash newHash;
            QStringList classList = currElem.attribute("class").split(" ", QString::SkipEmptyParts);
            for (int i = 0; i < classList.count(); ++i) {
                if (classHash.contains(classList.at(i))) {
                    StringHash tempHash = Tools::splitStyle(classHash.value(classList.at(i)));
                    foreach (const QString &key, tempHash.keys())
                        newHash.insert(key, tempHash.value(key));
                }
            }
            StringHash oldHash = currElem.styleHash();
            foreach (const QString &key, oldHash.keys())
                newHash.insert(key, oldHash.value(key));
            currElem.setStylesFromHash(newHash);
            currElem.removeAttribute("class");
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::prepareDefs()
{
    // move all gradient, filters, etc. to 'defs' element
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.parentElement() != defsElement()) {
            if (Props::defsList.contains(currElem.tagName()))
                defsElement().appendChild(currElem);

        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }

    // ungroup all defs in defs
    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.parentElement().tagName() == "defs"
            && currElem.parentElement() != defsElement()) {
            defsElement().appendChild(currElem);
        } else if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::fixWrongAttr()
{
    // fix bad Adobe Illustrator SVG exporting
    if (svgElement().attributeEqualTo("xmlns", "&ns_svg;"))
        svgElement().setAttribute("xmlns", "http://www.w3.org/2000/svg");
    if (svgElement().attributeEqualTo("xmlns:xlink", "&ns_xlink;"))
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    QList<SvgElement> list = svgElement().childElemList();
    QStringList tmpList = QStringList() << "fill" << "stroke";
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QString currTag = elem.tagName();

        // remove empty attributes
        for (const XMLAttribute *child = elem.xmlElement()->FirstAttribute();
             child; child = child->Next()) {
            if (child->Value()[0] == '\0')
                elem.removeAttribute(child->Name());
        }

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
        if (currTag == "linearGradient" || currTag == "radialGradient") {
            if (elem.hasChildren() && elem.hasAttribute("xlink:href"))
                elem.removeAttribute("xlink:href");
        }

        if (currTag == "use") {
            if (elem.doubleAttribute("width") < 0)
                elem.setAttribute("width", "0");
            if (elem.doubleAttribute("height") < 0)
                elem.setAttribute("height", "0");
        } else if (currTag == "rect") {
            // fix wrong 'rx', 'ry' attributes in 'rect' elem
            // remove, if one of 'r' is null
            if ((elem.hasAttribute("rx") && elem.hasAttribute("ry"))
                && (elem.attribute("rx") == 0 || elem.attribute("ry") == 0)) {
                elem.removeAttribute("rx");
                elem.removeAttribute("ry");
            }

            // if only one 'r', create missing with same value
            if (!elem.hasAttribute("rx") && elem.hasAttribute("ry"))
                elem.setAttribute("rx", elem.attribute("ry"));
            if (!elem.hasAttribute("ry") && elem.hasAttribute("rx"))
                elem.setAttribute("ry", elem.attribute("rx"));

            // rx/ry can not be bigger then width/height
            qreal halfWidth = elem.doubleAttribute("width") / 2;
            qreal halfHeight = elem.doubleAttribute("height") / 2;
            if (elem.hasAttribute("rx") && elem.doubleAttribute("rx") >= halfWidth)
                elem.setAttribute("rx", Tools::roundNumber(halfWidth));
            if (elem.hasAttribute("ry") && elem.doubleAttribute("ry") >= halfHeight)
                elem.setAttribute("ry", Tools::roundNumber(halfHeight));
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::finalFixes()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QString tagName = elem.tagName();

        elem.removeAttribute(CleanerAttr::UsedElement);

        if (Keys.flag(Key::RemoveNotAppliedAttributes)) {
            if (tagName == "rect" || tagName == "use") {
                elem.removeAttributeIf("ry", ToChar(elem.attribute("rx")));
                elem.removeAttributeIf("x", "0");
                elem.removeAttributeIf("y", "0");
            }
            else if (tagName == "circle") {
                elem.removeAttributeIf("cx", "0");
                elem.removeAttributeIf("cy", "0");
            }
            else if (tagName == "line") {
                elem.removeAttributeIf("x1", "0");
                elem.removeAttributeIf("y1", "0");
                elem.removeAttributeIf("x2", "0");
                elem.removeAttributeIf("y2", "0");
            } else if (tagName == "marker") {
                elem.removeAttributeIf("refX", "0");
                elem.removeAttributeIf("refY", "0");
            }

            if (tagName == "linearGradient"
                && (elem.hasAttribute("x2") || elem.hasAttribute("y2"))) {
                if (elem.attribute("x1") == elem.attribute("x2")) {
                    elem.removeAttribute("x1");
                    elem.setAttribute("x2", "0");
                }
                if (elem.attribute("y1") == elem.attribute("y2")) {
                    elem.removeAttribute("y1");
                    elem.removeAttribute("y2");
                }
                // remove 'gradientTransform' attr if only x2=0 attr left
                if (   !elem.hasAttribute("x1") && elem.attribute("x2") == "0"
                    && !elem.hasAttribute("y1") && !elem.hasAttribute("y2")) {
                    elem.removeAttribute("gradientTransform");
                }
            } else if (tagName == "radialGradient") {
                qreal fx = elem.doubleAttribute("fx");
                qreal fy = elem.doubleAttribute("fy");
                qreal cx = elem.doubleAttribute("cx");
                qreal cy = elem.doubleAttribute("cy");
                if (Tools::isZero(qAbs(fx-cx)))
                    elem.removeAttribute("fx");
                if (Tools::isZero(qAbs(fy-cy)))
                    elem.removeAttribute("fy");
            }
        }

        if (Keys.flag(Key::RemoveInvisibleElements)) {
            // remove empty defs
            if (elem.tagName() == "defs") {
                if (!elem.hasChildren())
                    elem.parentElement().removeChild(elem);
            }
        }

        if (elem.hasChildren() && !elem.isNull())
            list << elem.childElemList();
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
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("id")) {
            QString newId;
            // gen new id
            QList<int> intList;
            intList.reserve(3);
            for (int j = 0; j < pos; ++j)
                plusOne(intList);
            while (!intList.isEmpty())
                newId += charList.at(intList.takeLast());
            if (newId.isEmpty())
                newId = "0";

            idHash.insert(currElem.id(), newId);
            currElem.setAttribute("id", newId);
            pos++;
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }

    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        foreach (const QString &attrName, Props::linkableStyleAttributes) {
            if (currElem.hasAttribute(attrName)) {
                QString url = currElem.attribute(attrName);
                if (url.startsWith(QL1S("url"))) {
                    url = url.mid(5, url.size()-6);
                    currElem.setAttribute(attrName, QString("url(#" + idHash.value(url) + ")"));
                }
            }
        }
        if (currElem.hasAttribute("xlink:href")) {
            QString id = currElem.attribute("xlink:href");
            if (!id.startsWith(QL1S("data:"))) {
                id.remove(0,1);
                currElem.setAttribute("xlink:href", QString("#" + idHash.value(id)));
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::calcElemAttrCount(const QString &text)
{
    quint32 elemCount = 0;
    quint32 attrCount = 0;
    QList<SvgElement> list = childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        elemCount++;
        attrCount += elem.attributesCount();
        if (elem.hasChildren())
            list << elem.childElemList();
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
    QList<SvgElement> list = defsElement().childElemList();
    QList<SvgElement> list2;
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (!elem.isTagName("use") && !elem.isUsed())
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
    return QString::localeAwareCompare(node1.tagName(), node2.tagName()) < 0;
}

void Replacer::roundNumericAttributes()
{
    QList<SvgElement> list = svgElement().childElemList();
    QStringList listBasedAttrList;
    listBasedAttrList << "stdDeviation" << "baseFrequency" << "dx" << "dy" << "stroke-dasharray";
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QStringList attrList = elem.attributesList();
        foreach (const QString &attr, Props::filterDigitList) {
            if (attrList.contains(attr) && elem.attribute(attr) != "none") {
                QString value = elem.attribute(attr);
                // process list based attributes
                if (listBasedAttrList.contains(attr)) {
                    // TODO: get rid of regex
                    QStringList tmpList = value.split(QRegExp("(,|) |,"), QString::SkipEmptyParts);
                    QString tmpStr;
                    foreach (const QString &text, tmpList) {
                        bool ok;
                        if (attr == "stroke-dasharray")
                            tmpStr += QString::number(text.toDouble(&ok)) + " ";
                        else
                            tmpStr += Tools::roundNumber(text.toDouble(&ok), Tools::TRANSFORM) + " ";
                        if (!ok)
                            qFatal("Error: could not process value: %s",
                                   qPrintable(attr + "=" + value));
                    }
                    tmpStr.chop(1);
                    elem.setAttribute(attr, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::TRANSFORM);
                    if (!ok)
                        qFatal("Error: could not process value: %s", qPrintable(attr + "=" + value));
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        foreach (const QString &attr, Props::digitList) {
            if (attrList.contains(attr)) {
                QString value = elem.attribute(attr);
                if (!value.contains("%") && !value.contains(" ") && !value.contains(",")) {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::ATTRIBUTE);
                    if (!ok)
                        qFatal("Error: could not process value: %s", qPrintable(attr + "=" + value));
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        if (Keys.flag(Key::SimplifyTransformMatrix)) {
            if (attrList.contains("gradientTransform")) {
                Transform ts(elem.attribute("gradientTransform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    elem.removeAttribute("gradientTransform");
                else
                    elem.setAttribute("gradientTransform", transform);
            }
            if (attrList.contains("transform")) {
                Transform ts(elem.attribute("transform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    elem.removeAttribute("transform");
                else
                    elem.setAttribute("transform", transform);
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

// TODO: try to convert thin rect to line-to path
// view-calendar-list.svg

// http://www.w3.org/TR/SVG/shapes.html
void Replacer::convertBasicShapes()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QString ctag = elem.tagName();
        if (ctag == "polygon" || ctag == "polyline" || ctag == "line" || ctag == "rect") {
            QString dAttr;
            if (ctag == "line") {
                dAttr = QString("M %1,%2 %3,%4")
                        .arg(elem.attribute("x1"), elem.attribute("y1"),
                             elem.attribute("x2"), elem.attribute("y2"));
                elem.removeAttributes(QStringList() << "x1" << "y1" << "x2" << "y2");
            } else if (ctag == "rect") {
                if (elem.doubleAttribute("rx") == 0 || elem.doubleAttribute("ry") == 0) {
                    qreal x = elem.doubleAttribute("x");
                    qreal y = elem.doubleAttribute("y");
                    qreal x1 = x + elem.doubleAttribute("width");
                    qreal y1 = y + elem.doubleAttribute("height");
                    dAttr = QString("M %1,%2 H%3 V%4 H%1 z").arg(x).arg(y).arg(x1).arg(y1);
                    elem.removeAttributes(QStringList() << "x" << "y" << "width" << "height"
                                                        << "rx" << "ry");
                }
            } else if (ctag == "polyline" || ctag == "polygon") {
                QList<Segment> segmentList;
                QString path = elem.attribute("points").remove("\t").remove("\n");
                const QChar *str = path.constData();
                const QChar *end = str + path.size();
                while (str != end) {
                    Segment seg;
                    seg.command = Command::MoveTo;
                    seg.absolute = true;
                    seg.srcCmd = segmentList.isEmpty();
                    seg.x = Tools::getNum(str);
                    seg.y = Tools::getNum(str);
                    segmentList.append(seg);
                }
                if (ctag == "polygon") {
                    Segment seg;
                    seg.command = Command::ClosePath;
                    seg.absolute = false;
                    seg.srcCmd = segmentList.isEmpty();
                    segmentList.append(seg);
                }
                dAttr = Path().segmentsToPath(segmentList);
                elem.removeAttribute("points");
            }
            if (!dAttr.isEmpty()) {
                elem.setAttribute("d", dAttr);
                elem.setTagName("path");
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::splitStyleAttr()
{
    QList<SvgElement> list = childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (!elem.tagName().contains("feFlood")) {
            if (elem.hasAttribute("style")) {
                QString style = elem.attribute("style");
                int commentPos = 0;
                while (commentPos != -1) {
                    commentPos = style.indexOf("/*");
                    if (commentPos != -1)
                        style.remove(commentPos, style.indexOf("*/") - commentPos + 2);
                }
                style.remove("\n");
                StringHash hash = Tools::splitStyle(style);
                foreach (const QString &key, hash.keys()) {
                    // ignore attributes like "-inkscape-font-specification"
                    // qt render prefer property attributes instead of "style" attribute
                    if (key.at(0) != '-')
                        elem.setAttribute(key, hash.value(key));
                }
                elem.removeAttribute("style");
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::joinStyleAttr()
{
    QList<SvgElement> list = childElemList(document());
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QStringList attrs;
        foreach (const QString &attrName, Props::presentationAttributes) {
            if (elem.hasAttribute(attrName)) {
                attrs << attrName + ":" + elem.attribute(attrName);
                elem.removeAttribute(attrName);
            }
        }
        elem.setAttribute("style", attrs.join(";"));

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

// Move linearGradient child stop elements to radialGradient or linearGradient
// which inherits of this linearGradient.
// Only when inherited linearGradient used only once.
void Replacer::mergeGradients()
{
    QStringList linkList;
    QList<SvgElement> list = defsElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("xlink:href"))
            linkList << currElem.xlinkId();

    }
    list = svgElement().childElemList();
    QStringList attrList = QStringList() << "fill" << "stroke";
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        foreach (const QString &attrName, attrList) {
            if (currElem.hasAttribute(attrName)) {
                QString id = currElem.defIdFromAttribute(attrName);
                if (!id.isEmpty())
                    linkList << id;
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }

    list = defsElement().childElemList();
    StringHash xlinkHash;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if ((currElem.tagName() == "radialGradient" || currElem.tagName() == "linearGradient")
                && currElem.hasAttribute("xlink:href") && !currElem.hasChildren()) {
            QString currLink = currElem.xlinkId();
            if (linkList.count(currLink) == 1) {
                SvgElement lineGradElem = findLinearGradient(currLink);
                if (!lineGradElem.isNull()) {
                    if (lineGradElem.hasChildren()) {
                        foreach (const SvgElement &elem, lineGradElem.childElemList())
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
 *   <stop offset="0" stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 * <linearGradient id="linearGradient002">
 *   <stop offset="0" stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 *
 * After:
 * <linearGradient id="linearGradient001">
 *   <stop offset="0" stop-color="white"/>
 *   <stop offset="1" stop-color="black"/>
 * </linearGradient>
 * <linearGradient id="linearGradient002" xlink:href="#linearGradient001">
 *
 */
void Replacer::mergeGradientsWithEqualStopElem()
{
    QList<SvgElement> list = defsElement().childElemList();
    QList<LineGradStruct> lineGradList;
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if ((currElem.tagName() == "linearGradient" || currElem.tagName() == "radialGradient")
            && currElem.hasChildren()) {
            LineGradStruct lgs;
            lgs.elem = currElem;
            lgs.id = currElem.id();
            lgs.attrs = currElem.attributesMap(true);
            foreach (SvgElement stopElem, currElem.childElemList())
                lgs.stopAttrs << stopElem.attributesMap(true);
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
                    lgs2.elem.setAttribute("xlink:href", "#" + lgs1.id);
                    foreach (SvgElement stopElem, lgs2.elem.childElemList())
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
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (elem.isTagName("rect")) {
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          elem.attribute("x")
                                  + " " + elem.attribute("y")
                                  + " " + elem.attribute("width")
                                  + " " + elem.attribute("height"));
        } else if (elem.isTagName("circle")) {
            qreal r = elem.doubleAttribute("r");
            qreal x = elem.doubleAttribute("cx") - r;
            qreal y = elem.doubleAttribute("cy") - r;
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          Tools::roundNumber(x)
                                  + " " + Tools::roundNumber(y)
                                  + " " + Tools::roundNumber(qAbs(r*2))
                                  + " " + Tools::roundNumber(qAbs(r*2)));
        } else if (elem.isTagName("ellipse")) {
            qreal rx = elem.doubleAttribute("rx");
            qreal ry = elem.doubleAttribute("ry");
            qreal x = elem.doubleAttribute("cx") - rx;
            qreal y = elem.doubleAttribute("cy") - ry;
            elem.setAttribute(CleanerAttr::BoundingBox,
                                          Tools::roundNumber(x)
                                  + " " + Tools::roundNumber(y)
                                  + " " + Tools::roundNumber(qAbs(rx*2))
                                  + " " + Tools::roundNumber(qAbs(ry*2)));
        }
        // all other basic shapes bounding boxes are calculated in Paths class

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

SvgElement Replacer::findLinearGradient(const QString &id)
{
    for (XMLElement *child = defsElement().xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (child->Attribute("id")) {
            if (!strcmp(child->Attribute("id"), id.toLatin1()) && !strcmp(child->Name(), "linearGradient"))
                return SvgElement(child);
        }
    }
    return SvgElement();
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
    QList<SvgElement> similarElemList;
    QStringList additionalAttrList;
    additionalAttrList << "text-align" << "line-height" << "font";
    QStringList ignoreAttrList;
    ignoreAttrList << "clip-path" << "mask" << "filter" << "opacity";
    QList<SvgElement> list = parentElem.childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.isGroup() || currElem.isTagName("flowRoot"))
            groupElementsByStyles(currElem);

        if (groupHash.isEmpty()) {
            // get hash of all style attributes of element
            groupHash = currElem.styleHash();
            foreach (const QString &attrName, additionalAttrList) {
                if (currElem.hasAttribute(attrName))
                    groupHash.insert(attrName, currElem.attribute(attrName));
            }
            if (    currElem.hasAttribute("transform")
                && !currElem.hasLinkedDef()
                && !parentElem.hasLinkedDef()) {
                groupHash.insert("transform", currElem.attribute("transform"));
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
                    if (parentElem.isTagName("flowRoot")) {
                        int flowParaCount = 0;
                        foreach (SvgElement childElem, parentElem.childElemList()) {
                            if (childElem.isTagName("flowPara"))
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
                        else if (parentElem.isTagName("svg") && !lastGroupHash.contains("transform"))
                            canUseParent = true;
                    }

                    SvgElement parentGElem;
                    if (canUseParent) {
                        parentGElem = parentElem;
                    } else {
                        parentGElem = SvgElement(document()->NewElement("g"));
                        parentGElem = parentElem.insertBefore(parentGElem, similarElemList.first());
                    }
                    // move equal style attributes of selected elements to parent group
                    foreach (const QString &attrName, lastGroupHash.keys()) {
                        if (parentGElem.hasAttribute(attrName) && attrName == "transform") {
                            Transform ts(parentGElem.attribute("transform") + " "
                                         + lastGroupHash.value(attrName));
                            parentGElem.setAttribute("transform", ts.simplified());
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
                            foreach (SvgElement gChildElem, similarElem.childElemList())
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
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.tagName() == "use" || currElem.tagName() == "textPath") {
            if (currElem.hasAttribute("xlink:href"))
                usedElemList << currElem.xlinkId();
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }

    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        QString id = currElem.id();
        if (!id.isEmpty()) {
            if (usedElemList.contains(id))
                currElem.setAttribute(CleanerAttr::UsedElement, "1");
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

QHash<QString,int> Replacer::calcDefsUsageCount()
{
    QStringList attrList;
    attrList << "fill" << "filter" << "stroke";
    QHash<QString,int> idHash;
    QList<SvgElement> list = svgElement().childElemList();
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

        if (elem.hasChildren())
            list << elem.childElemList();
    }
    return idHash;
}

void Replacer::applyTransformToDefs()
{
    QList<SvgElement> list = defsElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        if (elem.tagName() == "linearGradient") {
            if (elem.hasAttribute("gradientTransform")) {
                Transform gts(elem.attribute("gradientTransform"));
                if (gts.isProportionalScale()) {
                    gts.setOldXY(elem.doubleAttribute("x1"),
                                 elem.doubleAttribute("y1"));
                    elem.setAttribute("x1", Tools::roundNumber(gts.newX()));
                    elem.setAttribute("y1", Tools::roundNumber(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute("x2"),
                                 elem.doubleAttribute("y2"));
                    elem.setAttribute("x2", Tools::roundNumber(gts.newX()));
                    elem.setAttribute("y2", Tools::roundNumber(gts.newY()));
                    elem.removeAttribute("gradientTransform");
                }
            }
        } else if (elem.tagName() == "radialGradient") {
            if (elem.hasAttribute("gradientTransform")) {
                Transform gts(elem.attribute("gradientTransform"));
                if (!gts.isMirrored() && gts.isProportionalScale() && !gts.isRotating()) {
                    gts.setOldXY(elem.doubleAttribute("fx"),
                                 elem.doubleAttribute("fy"));
                    if (elem.hasAttribute("fx"))
                        elem.setAttribute("fx", Tools::roundNumber(gts.newX()));
                    if (elem.hasAttribute("fy"))
                        elem.setAttribute("fy", Tools::roundNumber(gts.newY()));
                    gts.setOldXY(elem.doubleAttribute("cx"),
                                 elem.doubleAttribute("cy"));
                    elem.setAttribute("cx", Tools::roundNumber(gts.newX()));
                    elem.setAttribute("cy", Tools::roundNumber(gts.newY()));

                    elem.setAttribute("r", Tools::roundNumber(elem.doubleAttribute("r")
                                                              * gts.scaleFactor()));
                    elem.removeAttribute("gradientTransform");
                }
            }
        }
        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::applyTransformToShapes()
{
    QHash<QString,int> defsHash = calcDefsUsageCount();

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();

        if (   elem.isGroup()
            && elem.hasAttribute("transform")
            && !elem.hasAttribute("clip-path")
            && !elem.hasAttribute("mask")
            && !elem.hasAttribute("filter")
            && elem.childElementCount() == 1
            && !elem.isUsed()
            && !elem.firstChild().isTagName("use")
            && !elem.firstChild().isGroup()
            && !elem.firstChild().isUsed()
            && !elem.firstChild().hasAttribute("clip-path"))
        {
            elem.firstChild().setTransform(elem.attribute("transform"), true);
            elem.removeAttribute("transform");
        }
        else if (   elem.isTagName("rect")
                 && elem.hasAttribute("transform")
                 && !elem.hasAttribute("clip-path")
                 && !elem.hasAttribute("mask")
                 && defsHash.value(elem.defIdFromAttribute("filter")) < 2
                 && defsHash.value(elem.defIdFromAttribute("stroke")) < 2
                 && defsHash.value(elem.defIdFromAttribute("fill")) < 2)
        {
            bool canApplyTransform = true;
            if (elem.hasAttribute("fill")) {
                SvgElement fillDef = findDefElement(elem.defIdFromAttribute("fill"));
                if (!fillDef.isNull() && fillDef.isTagName("pattern"))
                    canApplyTransform = false;
            }
            if (elem.hasAttribute("stroke")) {
                SvgElement fillDef = findDefElement(elem.defIdFromAttribute("stroke"));
                if (!fillDef.isNull() && fillDef.isTagName("pattern"))
                    canApplyTransform = false;
            }
            if (canApplyTransform) {
                Transform ts(elem.attribute("transform"));
                if (   !ts.isMirrored()
                    && !ts.isRotating()
                    && !ts.isSkew()
                    &&  ts.isProportionalScale()) {
                    ts.setOldXY(elem.doubleAttribute("x"), elem.doubleAttribute("y"));
                    elem.setAttribute("x", Tools::roundNumber(ts.newX()));
                    elem.setAttribute("y", Tools::roundNumber(ts.newY()));
                    QString newW = Tools::roundNumber(elem.doubleAttribute("width") * ts.scaleFactor());
                    elem.setAttribute("width", newW);
                    QString newH = Tools::roundNumber(elem.doubleAttribute("height") * ts.scaleFactor());
                    elem.setAttribute("height", newH);
                    QString newRx = Tools::roundNumber(elem.doubleAttribute("rx") * ts.scaleFactor());
                    elem.setAttribute("rx", newRx);
                    QString newRy = Tools::roundNumber(elem.doubleAttribute("ry") * ts.scaleFactor());
                    elem.setAttribute("ry", newRy);
                    updateLinkedDefTransform(elem);
                    calcNewStrokeWidth(elem, ts);
                    elem.removeAttribute("transform");
                }
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }
}

void Replacer::calcNewStrokeWidth(SvgElement &elem, const Transform &transform)
{
    SvgElement parentElem = elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute("stroke-width")) {
            qreal strokeWidth = Tools::convertUnitsToPx(parentElem.attribute("stroke-width")).toDouble();
            QString sw = Tools::roundNumber(strokeWidth * transform.scaleFactor(), Tools::ATTRIBUTE);
            elem.setAttribute("stroke-width", sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentElement();
    }
    if (!hasParentStrokeWidth) {
        elem.setAttribute("stroke-width", Tools::roundNumber(transform.scaleFactor(),
                                                                   Tools::ATTRIBUTE));
    }
}
