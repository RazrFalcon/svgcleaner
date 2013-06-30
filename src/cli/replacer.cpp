#include "replacer.h"
#include "keys.h"

// TODO: trim id names
// TODO: remove empty spaces at end of line in text elem
// TODO: round font size to int
// TODO: round style attributes
// TODO: replace equal 'fill', 'stroke', 'stop-color', 'flood-color' and 'lighting-color' attr
//       with 'color' attr

// TODO: read about 'marker'
// TODO: try to group similar elems to use
// gaerfield_data-center.svg

// TODO: If ‘x1’ = ‘x2’ and ‘y1’ = ‘y2’, then the area to be painted will be painted as
//       a single color using the color and opacity of the last gradient stop.

Replacer::Replacer(QDomDocument dom)
{
    m_dom = dom;
    m_svgElem = dom.elementsByTagName("svg").at(0).toElement();
    m_defsElem = Tools::findDefsNode(m_svgElem).toElement();
    if (m_defsElem.isNull()) {
        QDomElement elem = m_dom.createElement("defs");
        m_defsElem = m_svgElem.insertBefore(elem, m_svgElem.firstChild()).toElement();
    } else {
        if (m_svgElem.firstChild() != m_defsElem)
            m_defsElem = m_svgElem.insertBefore(m_defsElem, m_svgElem.firstChild()).toElement();
    }
}

void Replacer::convertSizeToViewbox()
{
    if (!m_svgElem.hasAttribute("viewBox")) {
        if (m_svgElem.hasAttribute("width") && m_svgElem.hasAttribute("height")) {
            QString width  = Tools::roundNumber(m_svgElem.attribute("width").toDouble());
            QString height = Tools::roundNumber(m_svgElem.attribute("height").toDouble());
            m_svgElem.setAttribute("viewBox", QString("0 0 %1 %2").arg(width).arg(height));
            m_svgElem.removeAttribute("width");
            m_svgElem.removeAttribute("height");
        }
    }
}

// TODO: remove identical paths
// Anonymous_man_head.svg
void Replacer::processPaths()
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();

        if (currElem.tagName() == "path") {
            Path().processPath(currElem);
            if (currElem.attribute("d").isEmpty())
                currElem.parentNode().removeChild(currElem);
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Replacer::convertUnits()
{
    // get
    qreal width = 0;
    qreal height = 0;
    if (m_svgElem.hasAttribute("width")) {
        if (m_svgElem.attribute("width").contains("%") && m_svgElem.hasAttribute("viewBox")) {
            QStringList viewBoxList = m_svgElem.attribute("viewBox").split(" ");
            Q_ASSERT(viewBoxList.count() == 4);
            m_svgElem.setAttribute("width", Tools::convertUnitsToPx(m_svgElem.attribute("width"),
                                                                    viewBoxList.at(2).toDouble()));
        }
        bool ok;
        width = Tools::convertUnitsToPx(m_svgElem.attribute("width")).toDouble(&ok);
        Q_ASSERT(ok == true);
    }
    if (m_svgElem.hasAttribute("height")) {
        if (m_svgElem.attribute("height").contains("%") && m_svgElem.hasAttribute("viewBox")) {
            QStringList viewBoxList = m_svgElem.attribute("viewBox").split(" ");
            Q_ASSERT(viewBoxList.count() == 4);
            m_svgElem.setAttribute("height", Tools::convertUnitsToPx(m_svgElem.attribute("height"),
                                                                     viewBoxList.at(3).toDouble()));
        }
        bool ok;
        height = Tools::convertUnitsToPx(m_svgElem.attribute("height")).toDouble(&ok);
        Q_ASSERT(ok == true);
    }

    // TODO: For gradientUnits="userSpaceOnUse", percentages represent values relative to the current viewport.
    //       For gradientUnits="objectBoundingBox", percentages represent values relative to the bounding box for the object.

    QSet<QString> attributes = Props::digitList;
    QList<SvgElement> list = Tools::childElemList(m_dom);
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        QString currTag = currElem.tagName();
        QStringList attrList = currElem.attributesList();
        for (int j = 0; j < attrList.count(); ++j) {
            if (attributes.contains(attrList.at(j))) {
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

                // fix attributes like:
                // x="170.625 205.86629 236.34703 255.38924 285.87 310.99512 338.50049"
                // PS: bad way...
                // FIXME: ignores list based attr
                if (currElem.attribute(attrList.at(j)).contains(" "))
                    currElem.setAttribute(attrList.at(j), currElem.attribute(attrList.at(j))
                                                                  .remove(QRegExp(" .*")));
            }
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// FIXME: gaim-4.svg
void Replacer::convertCDATAStyle()
{
    QDomNodeList styleNodeList = m_dom.elementsByTagName("style");
    if (styleNodeList.isEmpty())
        return;

    StringHash classHash;
    for (int i = 0; i < styleNodeList.count(); ++i) {
        QString text = styleNodeList.at(i).toElement().text();
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
        styleNodeList.at(i).parentNode().removeChild(styleNodeList.at(i));
    }

    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
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

            if (currElem.hasAttribute("style")) {
                StringHash oldHash = currElem.styleHash();
                foreach (const QString &key, oldHash.keys())
                    newHash.insert(key, oldHash.value(key));
            }
            QString newStyle = Tools::styleHashToString(newHash);

            if (!newStyle.isEmpty())
                currElem.setStyle(newStyle);
            currElem.removeAttribute("class");
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// BUG: corrupts hwinfo-3.svg
void Replacer::prepareDefs()
{
    // move all gradient, filters, etc. to 'defs' element
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.parentNode().toElement() != m_defsElem) {
            if (Props::defsList.contains(currElem.tagName()))
                m_defsElem.appendChild(currElem);
            else if (currElem.hasChildNodes())
                list << currElem.childElemList();
        }
    }

    // ungroup all defs in defs
    list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.parentNode().toElement().tagName() == "defs"
            && currElem.parentNode() != m_defsElem) {
            m_defsElem.appendChild(currElem);
        } else if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Replacer::joinStyleAttr()
{
    QList<SvgElement> list = Tools::childElemList(m_dom);

    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttributeByList(Props::styleAttrList)) {
            StringHash styleHash;
            QString newStyle;
            foreach (const QString &attr, Props::styleAttrList) {
                if (currElem.hasAttribute(attr)) {
                    // insert all style attr to hash and remove spaces at end of attr value
                    styleHash.insert(attr, currElem.attribute(attr).remove(QRegExp(" +$")));
                    currElem.removeAttribute(attr);
                }
            }
            StringHash styleHash2 = currElem.styleHash();
            foreach (const QString &key, styleHash2.keys())
                styleHash.insert(key, styleHash2.value(key));
            newStyle = Tools::styleHashToString(styleHash);

            if (!newStyle.isEmpty())
                currElem.setStyle(newStyle);
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Replacer::fixWrongAttr()
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        QString currTag = currElem.tagName();

        // remove empty attributes
        foreach (const QString &attr, currElem.attributesList()) {
            if (currElem.attribute(attr).isEmpty()) {
                currElem.removeAttribute(attr);
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

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// TODO: Gradient offset values less than 0 (or less than 0%) are rounded up to 0%. Gradient offset
//       values greater than 1 (or greater than 100%) are rounded down to 100%.
// TODO: remove empty defs here
void Replacer::finalFixes()
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        QString currTag = currElem.tagName();

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
                    currElem.setAttribute("x2", 0);
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

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Replacer::calcElemAttrCount(const QString &text)
{
    int elemCount = 0;
    int attrCount = 0;

    QList<SvgElement> list = Tools::childElemList(m_dom);
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        elemCount++;
        attrCount += currElem.attributes().count();
        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
    qDebug() << "The " + text + " number of elements is: " + QString::number(elemCount);
    qDebug() << "The " + text + " number of attributes is: " + QString::number(attrCount);
}

void Replacer::sortDefs()
{
    if (m_defsElem.isNull())
        return;

    QList<QDomNode> nodeList;
    QDomNodeList defsList = m_defsElem.childNodes();
    for (int j = 0; j < defsList.count(); ++j)
        nodeList << defsList.at(j);

    if (!nodeList.isEmpty()) {
        Tools::sortNodes(&nodeList);
        for (int i = 0; i < nodeList.count(); ++i)
            m_defsElem.appendChild(nodeList.at(i));
    }
}

void Replacer::roundDefs()
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();

        foreach (const QString &attr, Props::digitList) {
            if (currElem.hasAttribute(attr)) {
                QString value = currElem.attribute(attr);
                if (!value.contains("%")) {
                    bool ok;
                    QString attrVal = Tools::roundNumber(value.toDouble(&ok), Tools::ATTRIBUTES);
                    Q_ASSERT_X(ok == true, value.toAscii(), "wrong unit type");
                    currElem.setAttribute(attr, attrVal);
                }
            }
        }
        foreach (const QString &attr, Props::filterDigitList) {
            if (currElem.hasAttribute(attr)) {
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
        // TODO: refract
        if (!Keys::get().flag(Key::KeepTransforms)) {
            if (currElem.hasAttribute("gradientTransform")) {
                Transform ts(currElem.attribute("gradientTransform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    currElem.removeAttribute("gradientTransform");
                else {
                    QString text = ts.simplified();
                    // NOTE: some kind of bug...
                    // with rotate transform some files are crashed
                    // pitr_green_double_arrows_set_2_cleaned.svg
                    if (!text.contains("rotate"))
                        currElem.setAttribute("gradientTransform", ts.simplified());
                }
            }
            if (currElem.hasAttribute("transform")) {
                Transform ts(currElem.attribute("transform"));
                QString transform = ts.simplified();
                if (transform.isEmpty())
                    currElem.removeAttribute("transform");
                else
                    currElem.setAttribute("transform", ts.simplified());
            }
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// TODO: try to convert thin rect to line-to path
// view-calendar-list.svg

// http://www.w3.org/TR/SVG/shapes.html
void Replacer::convertBasicShapes()
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
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
//            } else if (ctag == "circle") {
//                qreal x = currElem.attribute("cx").toDouble();
//                qreal y = currElem.attribute("cy").toDouble();
//                qreal r = currElem.attribute("r").toDouble();

//                Tools::removeAttributes(currElem, QStringList() << "cx" << "cy" << "r");

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

//                Tools::removeAttributes(currElem, QStringList() << "cx" << "cy" << "rx" << "ry");

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

                Path path;
                path.setSegments(segmentList);
                dAttr = path.segmentsToPath();

                currElem.removeAttribute("points");
            }
            if (!dAttr.isEmpty()) {
                currElem.setAttribute("d", dAttr);
                currElem.setTagName("path");
            }
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Replacer::splitStyleAttr()
{
    QList<SvgElement> list = Tools::childElemList(m_dom);
    bool flag = Keys::get().flag(Key::JoinStyleAttributes);
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        if (!flag || currElem.tagName().contains("feFlood")) {
            if (currElem.hasAttribute("style")) {
                StringHash hash = currElem.styleHash();
                foreach (QString key, hash.keys())
                    currElem.setAttribute(key, hash.value(key));
                currElem.removeAttribute("style");
            }
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// Move linearGradient child stop elements to radialGradient
// which inherits of this linearGradient.
// Only when inherited linearGradient used only once.
void Replacer::mergeGradients()
{
    QStringList linkList;
    QList<SvgElement> list = m_defsElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.tagName() == "radialGradient" || currElem.tagName() == "linearGradient") {
            if (currElem.hasAttribute("xlink:href")) {
                linkList << currElem.attribute("xlink:href").remove("#");
            }
        }
    }

    list = m_defsElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        if (currElem.tagName() == "radialGradient" && currElem.hasAttribute("xlink:href")) {
            QString currLink = currElem.attribute("xlink:href").remove("#");
            if (linkList.count(currLink) == 1) {
                SvgElement lineGradElem = findLinearGradient(currLink);
                if (!lineGradElem.isNull()) {
                    foreach (const SvgElement &elem, lineGradElem.childElemList())
                        currElem.appendChild(elem);
                    m_defsElem.removeChild(lineGradElem);
                }
            }
        }
    }
}

SvgElement Replacer::findLinearGradient(const QString &id)
{
    QList<SvgElement> list = m_defsElem.childElemList();
    while (!list.empty()) {
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
 *
 */
void Replacer::groupElementsByStyles(SvgElement parentElem)
{
    // first start
    if (parentElem.isNull()) {
        // elem linked to 'use' have to store style properties only in elem, not in group
        m_usedElemList = Tools::usedElemList(m_svgElem);
        groupElementsByStyles(m_svgElem);
    }

    // check whether there styles in child elements
    int styleCount = 0;
    foreach (const SvgElement &elem, parentElem.childElemList()) {
        if (elem.hasAttribute("style"))
            styleCount++;
    }

    RepetitionList styleRepetList;
    if (styleCount > 1) {
        // get list of all unique style properties sorted by the number of coincidences
        styleRepetList = findRepetitionList(parentElem.childElemList());
    } else {
        // if no styles - trying to find and process groups in this element
        QList<SvgElement> list = parentElem.childElemList();
        while (!list.isEmpty()) {
            SvgElement currElem = list.takeFirst();
            if (currElem.isGroup())
                groupElementsByStyles(currElem);
        }
        return;
    }

    if (styleRepetList.isEmpty())
        return;

    if (styleRepetList.first().second == 1) {
        SvgElement singleElem;
        foreach (const SvgElement &elem, parentElem.childElemList()) {
            if (!m_usedElemList.contains(elem.id())) {
                if (singleElem.isNull())
                    singleElem = elem;
                else
                    return;
            }
        }
        if (!singleElem.isNull()) {
//            StringHash hash =  singleElem.styleHash();
//            StringHash parentHash =  parentElem.styleHash();
//            foreach (const QString &attr, hash.keys()) {
//                qDebug() << attr;
//                if (parentHash.contains(attr))
//                    parentHash.remove(attr);
//            }
//            parentElem.setStyle(Tools::styleHashToString(parentHash));
            parentElem.removeAttribute("style");
        }

        return;
    }

    const QList<SvgElement> list = parentElem.childElemList();
    // store all processed groups, to prevent retreatment processing
    QList<SvgElement> precessedGList;
    SvgElement mainGElem;
    // process child elements
    while (!styleRepetList.isEmpty()) {
        // stop when no repetitions are left
        if (styleRepetList.first().second == 1)
            break;

        QString currStyle = styleRepetList.first().first;
        // if current style property repeats equal to child elements count
        if (styleRepetList.first().second == list.count()) {
            // create new group if parent elem is not a group
            if (mainGElem.isNull()) {
                if (parentElem.isGroup()) {
                    mainGElem = parentElem;
                } else {
                    mainGElem = m_dom.createElement("g");
                    mainGElem = parentElem.insertBefore(mainGElem, list.first()).toElement();
                }
            }
            mainGElem.appendStyle(currStyle);
            Q_ASSERT(mainGElem.isNull() == false);

            // move elem to group
            foreach (SvgElement elem, list) {
                QStringList tmpList = elem.style().split(";", QString::SkipEmptyParts);
                tmpList.removeOne(currStyle);
                QString newStyle = tmpList.join(";");
                elem.setStyle(newStyle);
                mainGElem.appendChild(elem);
            }
        } else {
            QList<SvgElement> childList = parentElem.childElemList();
            QList<ElemListPair> similarElemList;
            while (!childList.empty()) {
                SvgElement currElem = childList.takeFirst();
                if (currElem.isGroup() && !precessedGList.contains(currElem)) {
                    groupElementsByStyles(currElem);
                    precessedGList << currElem;
                }
                // NOTE: the slowest line
                QStringList currStyleList
                        = currElem.style().split(";", QString::SkipEmptyParts);

                bool isElemSimilar = false;
                if (currStyleList.contains(currStyle)
                    && currElem.tagName() != "use"
                    && !m_usedElemList.contains(currElem.id()))
                {
                    similarElemList << ElemListPair(currElem, currStyleList);
                    isElemSimilar = true;
                }
                if (!isElemSimilar || childList.isEmpty()) {
                    if (similarElemList.count() > 1) {
                        QStringList stylesToRemove;
                        stylesToRemove << currStyle;
                        // also remove style props which equal for current elem list
                        for (int j = 1; j < styleRepetList.count(); ++j) {
                            bool flag = true;
                            foreach (ElemListPair pair, similarElemList) {
                                if (!pair.second.contains(styleRepetList.at(j).first)) {
                                    flag = false;
                                    break;
                                }
                            }
                            if (flag)
                                stylesToRemove << styleRepetList.at(j).first;
                        }

                        // add first most popular style, which not exist before
                        // NOTE: actualy can add not only first
                        RepetitionList tmpRList = findRepetitionList(similarElemList);
                        for (int k = 0; k < tmpRList.count(); ++k) {
                            if (!tmpRList.at(k).first
                                    .contains(QRegExp("stroke|fill|opacity|marker|font-size"))) {
                                if (!stylesToRemove.contains(tmpRList.at(k).first)) {
                                    if (tmpRList.at(k).second > 1) {
                                        stylesToRemove << tmpRList.at(k).first;
                                        break;
                                    }
                                }
                            }
                        }

                        if (!stylesToRemove.isEmpty()) {
                            SvgElement newGElem = m_dom.createElement("g");
                            SvgElement firstElem = similarElemList.first().first;
                            newGElem = parentElem.insertBefore(newGElem, firstElem).toElement();
                            newGElem.setStyle(stylesToRemove.join(";"));
                            Q_ASSERT(newGElem.isNull() == false);

                            foreach (ElemListPair pair, similarElemList) {
                                QStringList tmpList = pair.second;
                                foreach (const QString &text, stylesToRemove)
                                    tmpList.removeAll(text);
                                QString newStyle = tmpList.join(";");
                                pair.first.setStyle(newStyle);
                                newGElem.appendChild(pair.first);
                            }
                        }
                    }
                    similarElemList.clear();
                }
            }
        }
        styleRepetList.removeFirst();
    }
}

RepetitionList Replacer::findRepetitionList(const QList<SvgElement> &list)
{
    QList<QStringList> styleList;
    foreach (const SvgElement &elem, list) {
        if (!m_usedElemList.contains(elem.id()))
            styleList << elem.style().split(";", QString::SkipEmptyParts);
    }
    return genRepetitionList(styleList);
}

RepetitionList Replacer::findRepetitionList(QList<ElemListPair> list)
{
    QList<QStringList> styleList;
    foreach (const ElemListPair &pair, list) {
        if (!m_usedElemList.contains(pair.first.id()))
            styleList << pair.second;
    }
    return genRepetitionList(styleList);
}

bool repetitionListSort(const RepetitionItem &s1, const RepetitionItem &s2)
{
    return s1.second > s2.second;
}

// create list of used unique styles sorted by the number of coincidences
RepetitionList Replacer::genRepetitionList(const QList<QStringList> &list)
{
    RepetitionList rList;
    foreach (const QStringList &styleList, list) {
        foreach (const QString &currStyle, styleList) {
            // TODO: 'opacity' can be grouped, sometimes
            // TODO: test for fill:url
            if (!currStyle.contains(QRegExp("^opacity|clip-path|font-weight|filter|mask|fill:url"))) {
                // find style in list
                int pos = -1;
                for (int j = 0; j < rList.count(); ++j) {
                    if (rList.at(j).first == currStyle) {
                        pos = j;
                        break;
                    }
                }
                if (pos == -1)
                    rList.append(RepetitionItem(currStyle, 1));
                else
                    rList[pos].second = rList.at(pos).second + 1;
            }
        }
    }
    qSort(rList.begin(), rList.end(), repetitionListSort);
    return rList;
}
