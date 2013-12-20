#include "replacer.h"
#include "keys.h"

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

Replacer::Replacer(XMLDocument *doc) : BaseCleaner(doc)
{
}

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
void Replacer::processPaths()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();

        bool removed = false;
        if (currElem.tagName() == "path") {
            Path().processPath(currElem);
            if (currElem.attribute("d").isEmpty()) {
                currElem.parentNode().removeChild(currElem);
                removed = true;
            }
        }

        if (!removed) {
            if (currElem.hasChildren())
                list << currElem.childElemList();
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

    QSet<QString> attributes = Props::digitList;
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
                // didn't understand how to convert r="40%" like attributes
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
            QStringList classList = currElem.attribute("class").split(QRegExp(" +"));
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
        if (currElem.parentNode() != defsElement()) {
            if (Props::defsList.contains(currElem.tagName()))
                defsElement().appendChild(currElem);
            else if (currElem.hasChildren())
                list << currElem.childElemList();
        }
    }

    // ungroup all defs in defs
    list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.parentNode().tagName() == "defs"
            && currElem.parentNode() != defsElement()) {
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
        SvgElement currElem = list.takeFirst();
        QString currTag = currElem.tagName();

        // remove empty attributes
        foreach (const QString &attr, currElem.attributesList()) {
            if (currElem.attribute(attr).isEmpty()) {
                currElem.removeAttribute(attr);
            }
        }

        // remove wrong fill and stroke attributes like:
        // fill="url(#radialGradient001) rgb(0, 0, 0)"
        foreach (const QString &attrName, tmpList) {
            if (currElem.hasAttribute(attrName)) {
                QString attrValue = currElem.attribute(attrName);
                if (attrValue.contains("url")) {
                    int pos = attrValue.indexOf(' ');
                    if (pos > 0) {
                        attrValue.remove(pos, attrValue.size());
                        currElem.setAttribute(attrName, attrValue);
                    }
                }
            }
        }

        if (currTag == "use") {
            if (currElem.attribute("width").toDouble() < 0)
                currElem.setAttribute("width", "0");
            if (currElem.attribute("height").toDouble() < 0)
                currElem.setAttribute("height", "0");
        } else if (currTag == "rect") {
            // fix wrong 'rx', 'ry' attributes in 'rect' elem
            // remove, if one of 'r' is null
            if ((currElem.hasAttribute("rx") && currElem.hasAttribute("ry"))
                && (currElem.attribute("rx") == 0 || currElem.attribute("ry") == 0)) {
                currElem.removeAttribute("rx");
                currElem.removeAttribute("ry");
            }

            // if only one 'r', create missing with same value
            if (!currElem.hasAttribute("rx") && currElem.hasAttribute("ry"))
                currElem.setAttribute("rx", currElem.attribute("ry"));
            if (!currElem.hasAttribute("ry") && currElem.hasAttribute("rx"))
                currElem.setAttribute("ry", currElem.attribute("rx"));

            // rx/ry can not be bigger then width/height
            qreal halfWidth = currElem.attribute("width").toDouble() / 2;
            qreal halfHeight = currElem.attribute("height").toDouble() / 2;
            if (currElem.hasAttribute("rx") && currElem.attribute("rx").toDouble() >= halfWidth)
                currElem.setAttribute("rx", Tools::roundNumber(halfWidth));
            if (currElem.hasAttribute("ry") && currElem.attribute("ry").toDouble() >= halfHeight)
                currElem.setAttribute("ry", Tools::roundNumber(halfHeight));
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

// TODO: Gradient offset values less than 0 (or less than 0%) are rounded up to 0%. Gradient offset
//       values greater than 1 (or greater than 100%) are rounded down to 100%.
void Replacer::finalFixes()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        QString currTag = currElem.tagName();

        currElem.removeAttribute(CleanerAttr::UsedElement);

        if (!Keys::get().flag(Key::KeepNotAppliedAttributes)) {
            if (currTag == "rect") {
                if (currElem.attribute("rx").toDouble() == currElem.attribute("ry").toDouble())
                    currElem.removeAttribute("ry");
            } else if (currTag == "use") {
                if (currElem.attribute("x") == "0")
                    currElem.removeAttribute("x");
                if (currElem.attribute("y") == "0")
                    currElem.removeAttribute("y");
            }
        }

        if (!Keys::get().flag(Key::KeepGradientCoordinates)) {
            if (currTag == "linearGradient"
                && (currElem.hasAttribute("x2") || currElem.hasAttribute("y2"))) {
                if (currElem.attribute("x1") == currElem.attribute("x2")) {
                    currElem.removeAttribute("x1");
                    currElem.setAttribute("x2", "0");
                }
                if (currElem.attribute("y1") == currElem.attribute("y2")) {
                    currElem.removeAttribute("y1");
                    currElem.removeAttribute("y2");
                }
                // remove 'gradientTransform' attr if only x2=0 attr left
                if (   !currElem.hasAttribute("x1") && currElem.attribute("x2") == "0"
                    && !currElem.hasAttribute("y1") && !currElem.hasAttribute("y2")) {
                    currElem.removeAttribute("gradientTransform");
                }
            } else if (currTag == "radialGradient") {
                if (currElem.attribute("fx") == currElem.attribute("cx"))
                    currElem.removeAttribute("fx");
                if (currElem.attribute("fy") == currElem.attribute("cy"))
                    currElem.removeAttribute("fy");
            }
        }

        // remove empty defs
        if (currElem.tagName() == "defs") {
            if (!currElem.hasChildren())
                currElem.parentNode().removeChild(currElem);
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::trimIds()
{
    int pos = 0;
    StringHash idHash;
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("id")) {
            QString newId = QString::number(pos, 16);
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
            QString value = currElem.attribute("xlink:href");
            if (value.left(5) != QLatin1String("data:")) {
                const QString id = value.mid(1);
                currElem.setAttribute("xlink:href", QString("#" + idHash.value(id)));
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Replacer::calcElemAttrCount(const QString &text)
{
    int elemCount = 0;
    int attrCount = 0;
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        elemCount++;
        attrCount += currElem.attributesCount();
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
    qDebug() << "The " + text + " number of elements is: " + QString::number(elemCount);
    qDebug() << "The " + text + " number of attributes is: " + QString::number(attrCount);
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

void Replacer::roundDefs()
{
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        QStringList attrList = currElem.attributesList();

        foreach (const QString &attr, Props::digitList) {
            if (attrList.contains(attr)) {
                QString value = currElem.attribute(attr);
                if (!value.contains("%")) {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::ATTRIBUTE);
                    Q_ASSERT_X(ok == true, value.toAscii(), "wrong unit type");
                    currElem.setAttribute(attr, attrVal);
                }
            }
        }
        foreach (const QString &attr, Props::filterDigitList) {
            if (attrList.contains(attr)) {
                QString value = currElem.attribute(attr);
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
                    currElem.setAttribute(attr, tmpStr);
                } else {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::TRANSFORM);
                    Q_ASSERT_X(ok == true, value.toAscii(),
                               qPrintable("wrong unit type in " + currElem.id()));
                    currElem.setAttribute(attr, attrVal);
                }
            }
        }
        if (!Keys::get().flag(Key::KeepTransforms)) {
            if (attrList.contains("gradientTransform")) {
                Transform ts(currElem.attribute("gradientTransform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    currElem.removeAttribute("gradientTransform");
                else {
                    // NOTE: some kind of bug...
                    // with rotate transform some files are crashed
                    // pitr_green_double_arrows_set_2.svg
                    if (!transform.contains("rotate"))
                        currElem.setAttribute("gradientTransform", transform);
                }
            }
            if (attrList.contains("transform")) {
                Transform ts(currElem.attribute("transform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    currElem.removeAttribute("transform");
                else
                    currElem.setAttribute("transform", transform);
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
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
        if (   ctag == "polygon" || ctag == "polyline" || ctag == "line"
            || ctag == "rect"/* || ctag == "circle" || ctag == "ellipse"*/) {
            QString dAttr;

            if (ctag == "line") {
                dAttr = QString("M %1,%2 %3,%4").arg(currElem.attribute("x1"), currElem.attribute("y1"),
                                                     currElem.attribute("x2"), currElem.attribute("y2"));
                currElem.removeAttributes(QStringList() << "x1" << "y1" << "x2" << "y2");
            } else if (ctag == "rect") {
                if (currElem.attribute("rx").toDouble() == 0 || currElem.attribute("ry").toDouble() == 0) {
                    qreal x1 = currElem.attribute("x").toDouble() + currElem.attribute("width").toDouble();
                    qreal y1 = currElem.attribute("y").toDouble() + currElem.attribute("height").toDouble();
                    qreal x = QString::number(currElem.attribute("x").toDouble(), 'f').toDouble();
                    qreal y = QString::number(currElem.attribute("y").toDouble(), 'f').toDouble();
                    dAttr = QString("M %1,%2 H%3 V%4 H%1 z")
                            .arg(x).arg(y)
                            .arg(QString::number(x1))
                            .arg(QString::number(y1));

                    currElem.removeAttributes(QStringList() << "x" << "y" << "width" << "height"
                                                            << "rx" << "ry");

                } else {
                    qreal x  = currElem.attribute("x").toDouble();
                    qreal y  = currElem.attribute("y").toDouble();
                    qreal w  = currElem.attribute("width").toDouble();
                    qreal h  = currElem.attribute("height").toDouble();
                    qreal rx = currElem.attribute("rx").toDouble();
                    qreal ry = currElem.attribute("ry").toDouble();
                    if (rx == 0) rx = ry;
                    if (ry == 0) ry = rx;

                    currElem.removeAttributes(QStringList() << "x" << "y" << "width"
                                                            << "height" << "rx" << "ry");
                    dAttr =   QString("M %1,%2 ").arg(x+rx).arg(y)
                            + QString("H %1 ").arg(x+w-rx)
                            + QString("A %1,%2 0 0 1 %3,%4 ").arg(rx).arg(ry).arg(x+w).arg(y+ry)
                            + QString("V %1 ").arg(y+h-ry)
                            + QString("A %1,%2 0 0 1 %3,%4 ").arg(rx).arg(ry).arg(x+w-rx).arg(y+h)
                            + QString("H %1 ").arg(x+rx)
                            + QString("A %1,%2 0 0 1 %3,%4 ").arg(rx).arg(ry).arg(x).arg(y+h-ry)
                            + QString("V %1 ").arg(y+ry)
                            + QString("A %1,%2 0 0 1 %3,%4").arg(rx).arg(ry).arg(x+rx).arg(y);
                }
                // generate bigger text, than before...
//            } else if (ctag == "circle") {
//                qreal x = currElem.attribute("cx").toDouble();
//                qreal y = currElem.attribute("cy").toDouble();
//                qreal r = currElem.attribute("r").toDouble();

//                currElem.removeAttributes(QStringList() << "cx" << "cy" << "r");

//                qreal x1 = x + r;
//                qreal x2 = x - r;

//                dAttr =   QString("M %1,%2 ").arg(x1).arg(y)
//                        + QString("A %1,%1 0 1 0 %3,%4 ").arg(r).arg(x2).arg(y)
//                        + QString("A %1,%1 0 1 0 %3,%4").arg(r).arg(x1).arg(y);
//            } else if (ctag == "ellipse") {
//                qreal x = currElem.attribute("cx").toDouble();
//                qreal y = currElem.attribute("cy").toDouble();
//                qreal rx = currElem.attribute("rx").toDouble();
//                qreal ry = currElem.attribute("ry").toDouble();

//                currElem.removeAttributes(QStringList() << "cx" << "cy" << "rx" << "ry");

//                qreal x1 = x + rx;
//                qreal x2 = x - rx;

//                dAttr =   QString("M %1,%2 ").arg(x1).arg(y)
//                        + QString("A %1,%2 0 1 0 %3,%4 ").arg(rx).arg(ry).arg(x2).arg(y)
//                        + QString("A %1,%2 0 1 0 %3,%4").arg(rx).arg(ry).arg(x1).arg(y);
            } else if (ctag == "polyline" || ctag == "polygon") {
                QList<Segment> segmentList;
                // TODO: smart split
                // orangeobject_background-ribbon.svg
                QStringList tmpList
                    = currElem.attribute("points").split(QRegExp(" |,"), QString::SkipEmptyParts);
                for (int j = 0; j < tmpList.count(); j += 2) {
                    bool ok;
                    Segment seg;
                    seg.command = Command::MoveTo;
                    seg.absolute = true;
                    seg.srcCmd = false;
                    seg.x = tmpList.at(j).toDouble(&ok);
                    Q_ASSERT(ok == true);
                    seg.y = tmpList.at(j+1).toDouble(&ok);
                    Q_ASSERT(ok == true);
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
    bool flag = Keys::get().flag(Key::JoinStyleAttributes);
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (!flag || currElem.tagName().contains("feFlood")) {
            if (currElem.hasAttribute("style")) {
                StringHash hash = Tools::splitStyle(currElem.attribute("style"));
                foreach (const QString &key, hash.keys()) {
                    // ignore attributes like "-inkscape-font-specification"
                    // NOTE: qt render prefer property attributes instead of "style" attribute
                    if (key.at(0) != '-'/* && !currElem.hasAttribute(key)*/)
                        currElem.setAttribute(key, hash.value(key));
                }
                currElem.removeAttribute("style");
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
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
            linkList << currElem.attribute("xlink:href").remove("#");

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
            QString currLink = currElem.attribute("xlink:href").remove("#");
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
    mergeGradientsWithEqualStopElem();
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
            if (lgs1.id != lgs2.id
                && lgs1.stopAttrs.size() == lgs2.stopAttrs.size())
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
                }
            }
        }
    }
}

SvgElement Replacer::findLinearGradient(const QString &id)
{
    QList<SvgElement> list = defsElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.tagName() == "linearGradient" && currElem.id() == id) {
            return currElem;
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

// TODO: works badly on cbeau_shm_projection_of_circular_motion.svg
void Replacer::groupElementsByStyles(SvgElement parentElem)
{
    // first start
    if (parentElem.isNull()) {

        parentElem = svgElement();
    }

    StringHash groupHash;
    QList<SvgElement> similarElemList;
    QStringList additionalAttrList;
    additionalAttrList << "text-align" << "line-height";
    QStringList ignoreAttrList;
    ignoreAttrList << "clip-path" << "mask" << "filter" << "opacity";
    QList<SvgElement> list = parentElem.childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.isGroup() || currElem.tagName() == "flowRoot")
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
            if (currElem.isUsed() || currElem.tagName() == "use") {
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
            if (currElem.isUsed() || currElem.tagName() == "use") {
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
                    if (parentElem.tagName() == "flowRoot") {
                        int flowParaCount = 0;
                        foreach (SvgElement childElem, parentElem.childElemList()) {
                            if (childElem.tagName() == "flowPara")
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
                        else if (parentElem.tagName() == "svg" && !lastGroupHash.contains("transform"))
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
                            similarElem.parentNode().removeChild(similarElem);
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
    QSet<QString> usedElemList;
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.tagName() == "use") {
            if (currElem.hasAttribute("xlink:href"))
                usedElemList << currElem.attribute("xlink:href").remove("#");
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
