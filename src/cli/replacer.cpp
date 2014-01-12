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

// TODO: remove empty spaces at end of line in text elem
// TODO: round font size to int
// TODO: round style attributes
// TODO: replace equal 'fill', 'stroke', 'stop-color', 'flood-color' and 'lighting-color' attr
//       with 'color' attr
//       addon_the_couch.svg
// TODO: sort functions like in main
// TODO: try to group similar elems to use
//       gaerfield_data-center.svg, alnilam_Stars_Pattern.svg
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
        QRectF rect = Tools::viewBoxRect(svgElement());
        if (svgElement().hasAttribute("width")) {
            if (rect.width() == svgElement().doubleAttribute("width")
                || svgElement().attribute("width") == "100%")
                svgElement().removeAttribute("width");
        }
        if (svgElement().hasAttribute("height")) {
            if (rect.height() == svgElement().doubleAttribute("height")
                || svgElement().attribute("height") == "100%")
                svgElement().removeAttribute("height");
        }
    }
}

// TODO: remove identical paths
//       Anonymous_man_head.svg
//       input-keyboard-2.svg
//       applications-graphics.svg
// TODO: join paths with only style different
//       Anonymous_Chesspiece_-_bishop.svg
// TODO: replace paths with use, when paths has only first segment different
//       Anonymous_Flag_of_South_Korea.svg
void Replacer::processPaths()
{
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
                if (   defElem.tagName() == "linearGradient"
                    || defElem.tagName() == "radialGradient")
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

void Replacer::convertUnits()
{
    // get
    qreal width = 0;
    qreal height = 0;
    if (svgElement().hasAttribute("width")) {
        if (svgElement().attribute("width").contains("%") && svgElement().hasAttribute("viewBox")) {
            QRectF rect = Tools::viewBoxRect(svgElement());
            width = Tools::convertUnitsToPx(svgElement().attribute("width"), rect.width()).toDouble();
        } else {
            bool ok;
            width = Tools::convertUnitsToPx(svgElement().attribute("width")).toDouble(&ok);
            Q_ASSERT(ok == true);
        }
    }
    if (svgElement().hasAttribute("height")) {
        if (svgElement().attribute("height").contains("%") && svgElement().hasAttribute("viewBox")) {
            QRectF rect = Tools::viewBoxRect(svgElement());
            height = Tools::convertUnitsToPx(svgElement().attribute("height"), rect.height()).toDouble();
        } else {
            bool ok;
            height = Tools::convertUnitsToPx(svgElement().attribute("height")).toDouble(&ok);
            Q_ASSERT(ok == true);
        }
    }

    // TODO: For gradientUnits="userSpaceOnUse", percentages represent values relative to the current viewport.
    //       For gradientUnits="objectBoundingBox", percentages represent values relative to the bounding box for the object.

    // TODO: process 'offset' attr with %
    StringSet attributes = Props::digitList;
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        QString currTag = currElem.tagName();
        QStringList attrList = currElem.attributesList();
        for (int j = 0; j < attrList.count(); ++j) {
            if (attributes.contains(attrList.at(j))) {
                // TODO: to many setAttribute, rewrite

                // fix attributes like:
                // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
                // PS: bad way...
                // FIXME: ignores list based attr
                if (currElem.attribute(attrList.at(j)).contains(" "))
                    currElem.setAttribute(attrList.at(j), currElem.attribute(attrList.at(j))
                                                                  .remove(QRegExp(" .*")));

                QString value = currElem.attribute(attrList.at(j));
                // TODO: process gradients attrs
                if (value.contains("%")
                    && currTag != "radialGradient" && currTag != "linearGradient") {
                    if (attrList.at(j).contains("x"))
                        value = Tools::convertUnitsToPx(value, width);
                    else if (attrList.at(j).contains("y"))
                       value = Tools::convertUnitsToPx(value, height);
                } else {
                    value = Tools::convertUnitsToPx(value);
                }
                currElem.setAttribute(attrList.at(j), value);
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

// FIXME: gaim-4.svg
// TODO: style can be set in ENTITY
void Replacer::convertCDATAStyle()
{
    QStringList styleList;
    QList<XMLNode *> nodeList = Tools::childNodeList(document());
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
            nodeList << Tools::childNodeList(currNode);
    }
    if (styleList.isEmpty()) {
        // remove class attribute when no CDATA set
        QList<SvgElement> list = svgElement().childElemList();
        while (!list.isEmpty()) {
            SvgElement currElem = list.takeFirst();
            currElem.removeAttribute("class");
            if (currElem.hasChildren())
                list << currElem.childElemList();
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
        for (int j = 0; j < classList.count(); ++j) {
            QStringList tmpList = classList.at(j).split(QRegExp("( +|)\\{"));
            Q_ASSERT(tmpList.count() == 2);
            classHash.insert(tmpList.at(0), QString(tmpList.at(1)).remove(QRegExp("\\}.*")));
        }
    }

    QList<SvgElement> list = svgElement().childElemList();
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

// BUG: corrupts hwinfo-3.svg
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
    if (svgElement().attribute("xmlns") == QLatin1String("&ns_svg;"))
        svgElement().setAttribute("xmlns", "http://www.w3.org/2000/svg");
    if (svgElement().attribute("xmlns:xlink") == QLatin1String("&ns_xlink;"))
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

        // radialGradient with stop elements does not need xlink:href attribute
        if (currTag == "radialGradient") {
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
            // TODO: add other elements
            if (tagName == "rect") {
                if (elem.doubleAttribute("rx") == elem.doubleAttribute("ry"))
                    elem.removeAttribute("ry");
            }
            else if (tagName == "use") {
                if (elem.attribute("x") == "0")
                    elem.removeAttribute("x");
                if (elem.attribute("y") == "0")
                    elem.removeAttribute("y");
            }
            else if (tagName == "circle") {
                if (elem.attribute("cx") == "0")
                    elem.removeAttribute("cx");
                if (elem.attribute("cy") == "0")
                    elem.removeAttribute("cy");
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
                if (url.startsWith("url")) {
                    url = url.mid(5, url.size()-6);
                    currElem.setAttribute(attrName, QString("url(#" + idHash.value(url) + ")"));
                }
            }
        }
        if (currElem.hasAttribute("xlink:href")) {
            QString id = currElem.xlinkId();
            if (!id.startsWith(QLatin1String("data:"))) {
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
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        elemCount++;
        attrCount += currElem.attributesCount();
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
    if (!Keys.flag(Key::ShortOutput)) {
        qDebug("The %s number of elements is: %u",   qPrintable(text), elemCount);
        qDebug("The %s number of attributes is: %u", qPrintable(text), attrCount);
    } else {
        qDebug("%u", elemCount);
        qDebug("%u", attrCount);
    }
}

// TODO: sort with linking order
//       flag-um.svg
void Replacer::sortDefs()
{
    QList<SvgElement> list = defsElement().childElemList();
    if (!list.isEmpty()) {
        Tools::sortNodes(list);
        for (int i = 0; i < list.count(); ++i)
            defsElement().appendChild(list.at(i));
    }
}

// TODO: maybe round all viewBox width height x y attrs to integer
void Replacer::roundNumericAttributes()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement elem = list.takeFirst();
        QStringList attrList = elem.attributesList();

        foreach (const QString &attr, Props::digitList) {
            if (attrList.contains(attr)) {
                QString value = elem.attribute(attr);
                if (!value.contains("%")) {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::ATTRIBUTE);
                    Q_ASSERT_X(ok == true, value.toAscii(), "wrong unit type");
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        foreach (const QString &attr, Props::filterDigitList) {
            if (attrList.contains(attr)) {
                QString value = elem.attribute(attr);
                // process list based attributes
                if (attr == "stdDeviation" || attr == "baseFrequency" || attr == "dx") {
                    QStringList tmpList = value.split(QRegExp("(,|) "), QString::SkipEmptyParts);
                    QString tmpStr;
                    foreach (const QString &text, tmpList) {
                        bool ok;
                        tmpStr += Tools::roundNumber(text.toDouble(&ok), Tools::TRANSFORM) + " ";
                        Q_ASSERT(ok == true);
                    }
                    tmpStr.chop(1);
                    elem.setAttribute(attr, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::TRANSFORM);
                    Q_ASSERT_X(ok == true, value.toAscii(),
                               qPrintable("wrong unit type in " + elem.id()));
                    elem.setAttribute(attr, attrVal);
                }
            }
        }
        if (Keys.flag(Key::ApplyTransformsToDefs)) {
            if (attrList.contains("gradientTransform")) {
                Transform ts(elem.attribute("gradientTransform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    elem.removeAttribute("gradientTransform");
                else {
                    // NOTE: some kind of bug...
                    // with rotate transform some files are crashed
                    // pitr_green_double_arrows_set_2.svg
                    if (!transform.contains("rotate"))
                        elem.setAttribute("gradientTransform", transform);
                }
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
        SvgElement currElem = list.takeFirst();
        QString ctag = currElem.tagName();
        if (ctag == "polygon" || ctag == "polyline" || ctag == "line" || ctag == "rect") {
            QString dAttr;
            if (ctag == "line") {
                dAttr = QString("M %1,%2 %3,%4")
                        .arg(currElem.attribute("x1"), currElem.attribute("y1"),
                             currElem.attribute("x2"), currElem.attribute("y2"));
                currElem.removeAttributes(QStringList() << "x1" << "y1" << "x2" << "y2");
            } else if (ctag == "rect") {
                if (currElem.doubleAttribute("rx") == 0 || currElem.doubleAttribute("ry") == 0) {
                    qreal x = currElem.doubleAttribute("x");
                    qreal y = currElem.doubleAttribute("y");
                    qreal x1 = x + currElem.doubleAttribute("width");
                    qreal y1 = y + currElem.doubleAttribute("height");
                    dAttr = QString("M %1,%2 H%3 V%4 H%1 z").arg(x).arg(y).arg(x1).arg(y1);
                    currElem.removeAttributes(QStringList() << "x" << "y" << "width" << "height"
                                                            << "rx" << "ry");
                }
            } else if (ctag == "polyline" || ctag == "polygon") {
                QList<Segment> segmentList;
                QString path = currElem.attribute("points").remove("\t").remove("\n");
                const QChar *str = path.constData();
                const QChar *end = str + path.size();
                while (str != end) {
                    Segment seg;
                    seg.command = Command::MoveTo;
                    seg.absolute = true;
                    seg.srcCmd = false;
                    seg.x = Tools::getNum(str);
                    seg.y = Tools::getNum(str);
                    segmentList.append(seg);
                }
                if (ctag == "polygon") {
                    Segment seg;
                    seg.command = Command::ClosePath;
                    seg.absolute = false;
                    seg.srcCmd = true;
                    segmentList.append(seg);
                }
                dAttr = Path().segmentsToPath(segmentList);
                currElem.removeAttribute("points");
            }
            if (!dAttr.isEmpty()) {
                currElem.setAttribute("d", dAttr);
                currElem.setTagName("path");
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::splitStyleAttr()
{
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (!currElem.tagName().contains("feFlood")) {
            if (currElem.hasAttribute("style")) {
                StringHash hash = Tools::splitStyle(currElem.attribute("style"));
                foreach (const QString &key, hash.keys()) {
                    // ignore attributes like "-inkscape-font-specification"
                    // NOTE: qt render prefer property attributes instead of "style" attribute
                    if (key.at(0) != '-')
                        currElem.setAttribute(key, hash.value(key));
                }
                currElem.removeAttribute("style");
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::joinStyleAttr()
{
    QList<SvgElement> list = Tools::childElemList(document());
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
// FIXME: broke applications-graphics.svg
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

SvgElement Replacer::findLinearGradient(const QString &id)
{
    for (XMLElement *child = defsElement().xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (!strcmp(child->Attribute("id"), id.toLatin1()) && !strcmp(child->Name(), "linearGradient"))
            return SvgElement(child);
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
            for (int i = 0; i < ignoreAttrList.size(); ++i) {
                if (groupHash.contains(ignoreAttrList.at(i)))
                    groupHash.remove(ignoreAttrList.at(i));
            }
            if (!groupHash.isEmpty())
                similarElemList << currElem;
            // elem linked to 'use' have to store style properties only in elem, not in group
            if (currElem.isUsed() || currElem.isTagName("use")) {
                groupHash.clear();
                similarElemList.clear();
            }
        } else {
            StringHash lastGroupHash = groupHash;
            // remove attributes which is not exist or different in next element
            foreach (const QString &attrName, groupHash.keys()) {
                if (!currElem.hasAttribute(attrName))
                    groupHash.remove(attrName);
                else if (currElem.attribute(attrName) != groupHash.value(attrName))
                    groupHash.remove(attrName);
            }
            if (currElem.isUsed() || currElem.isTagName("use")) {
                groupHash.clear();
                similarElemList.clear();
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
                if (!currElem.tagName().isEmpty())
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
                if (!gts.isMirrored() && gts.isProportionalScale()) {
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
