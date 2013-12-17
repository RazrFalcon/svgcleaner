#include <cmath>

#include "keys.h"
#include "tools.h"

Transform::Transform(const QString &text)
{
    Q_ASSERT(text.isEmpty() == false);
    if (text.isEmpty())
        return;
    m_points = mergeMatrixes(text);

    // calculate
    m_xScale = sqrt(pow(m_points.at(0), 2) + pow(m_points.at(2), 2));
    m_yScale = sqrt(pow(m_points.at(1), 2) + pow(m_points.at(3), 2));
}

void Transform::setOldXY(qreal prevX, qreal prevY)
{
    // NOTE: must be set
    m_baseX = prevX;
    m_baseY = prevY;
}

qreal Transform::newX() const
{
    return m_points.at(0)*m_baseX + m_points.at(2)*m_baseY + m_points.at(4);
}

qreal Transform::newY() const
{
    return m_points.at(1)*m_baseX + m_points.at(3)*m_baseY + m_points.at(5);
}

// http://www.w3.org/TR/SVG/coords.html#EstablishingANewUserSpace
QList<qreal> Transform::mergeMatrixes(QString text)
{
    text = Tools::removeEdgeSpaces(text);
    QStringList transList = text.split(QRegExp("\\) +"), QString::SkipEmptyParts);

    QList<TransMatrix> transMatrixList;
    for (int i = 0; i < transList.count(); ++i) {
        QString transformType = QString(transList.at(i)).remove(QRegExp("( +|)\\(.*")).toLower();
        // transform values can be separeted by: ',' , ', ', ' '
        QStringList pointsStr = QString(transList.at(i)).remove(QRegExp(".*\\(|\\)"))
                                .split(QRegExp(",(| )| "), QString::SkipEmptyParts);

        QList<qreal> points;
        for (int i = 0; i < pointsStr.count(); ++i) {
            bool ok = false;
            points << pointsStr.at(i).toDouble(&ok);
            Q_ASSERT(ok == true);
        }

        // transform rotate(<rotate-angle> [<cx> <cy>]) to
        // translate(<cx>, <cy>) rotate(<rotate-angle>) translate(-<cx>, -<cy>)
        if (transformType == "rotate" && points.count() == 3) {
            return mergeMatrixes(QString("translate(%1 %2) rotate(%3) translate(-%1 -%2)")
                                 .arg(points.at(0)).arg(points.at(1)).arg(points.at(2)));
        }

        TransMatrix matrix;
        matrix.setToIdentity();
        if (transformType == "matrix") {
            matrix(0,0) = points.at(0);
            matrix(0,1) = points.at(2);
            matrix(0,2) = points.at(4);

            matrix(1,0) = points.at(1);
            matrix(1,1) = points.at(3);
            matrix(1,2) = points.at(5);
        } else if (transformType == "translate") {
            if (points.count() == 1)
                points << 0.0;
            matrix(0,2) = points.at(0);
            matrix(1,2) = points.at(1);
        } else if (transformType == "scale") {
            if (points.count() == 1)
                points << points.at(0);
            matrix(0,0) = points.at(0);
            matrix(1,1) = points.at(1);
        } else if (transformType == "rotate") {
            matrix(0,0) = cos((points.at(0)/180)*M_PI);
            matrix(1,0) = sin((points.at(0)/180)*M_PI);

            matrix(0,1) = -sin((points.at(0)/180)*M_PI);
            matrix(1,1) = cos((points.at(0)/180)*M_PI);
        } else if (transformType == "skewX") {
            matrix(0,1) = tan(points.at(0));
        } else if (transformType == "skewY") {
            matrix(1,0) = tan(points.at(0));
        } else {
            qDebug() << transformType;
            qDebug() << "Error: wrong transform matrix";
            exit(0);
        }
        transMatrixList << matrix;
    }

    TransMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);

    QList<qreal> pointList;
    pointList.reserve(6);
    pointList << newMatrix(0,0) << newMatrix(1,0) << newMatrix(0,1)
              << newMatrix(1,1) << newMatrix(0,2) << newMatrix(1,2);
    return pointList;
}

QString Transform::simplified() const
{
    if (m_points.isEmpty())
        return "";

    QString transform;
    QList<qreal> pt = m_points;

    // [1 0 0 1 tx ty] = translate
    if (pt.at(0) == 1 && pt.at(1) == 0 && pt.at(2) == 0 && pt.at(3) == 1) {
        if (pt.at(5) != 0) {
            transform = QString("translate(%1 %2)")
                        .arg(Tools::roundNumber(pt.at(4), Tools::TRANSFORM),
                             Tools::roundNumber(pt.at(5), Tools::TRANSFORM));

        } else if (pt.at(4) != 0) {
            transform = QString("translate(%1)")
                        .arg(Tools::roundNumber(pt.at(4), Tools::TRANSFORM));
        }
        if (transform == "translate(0)" || transform == "translate(0 0)")
            transform.clear();
    } // [sx 0 0 sy 0 0] = scale
    else if (pt.at(1) == 0 && pt.at(2) == 0 && pt.at(4) == 0 && pt.at(5) == 0) {
        if (pt.at(0) != pt.at(3)) {
            transform = QString("scale(%1 %2)")
                        .arg(Tools::roundNumber(pt.at(0), Tools::TRANSFORM),
                             Tools::roundNumber(pt.at(3), Tools::TRANSFORM));
        } else {
            transform = QString("scale(%1)")
                        .arg(Tools::roundNumber(pt.at(0), Tools::TRANSFORM));
        }
    } // [cos(a) sin(a) -sin(a) cos(a) 0 0] = rotate
    else if (pt.at(0) == pt.at(3) && pt.at(1) > 0 && pt.at(2) < 0
             && pt.at(4) == 0 && pt.at(5) == 0) {
        transform = QString("rotate(%1)")
                    .arg(Tools::roundNumber(acos(pt.at(0))*(180/M_PI), Tools::TRANSFORM));
        if (transform == "rotate(0)")
            transform.clear();
    } // [1 0 tan(a) 1 0 0] = skewX
    else if (pt.at(0) == 1 && pt.at(1) == 0 && pt.at(3) == 1 && pt.at(4) == 0 && pt.at(5) == 0) {
        transform = QString("skewX(%1)")
                    .arg(Tools::roundNumber(atan(pt.at(2))*(180/M_PI), Tools::TRANSFORM));
        if (transform == "skewX(0)")
            transform.clear();
    } // [1 tan(a) 0 1 0 0] = skewY
    else if (pt.at(0) == 1 && pt.at(2) == 0 && pt.at(3) == 1 && pt.at(4) == 0 && pt.at(5) == 0) {
        transform = QString("skewY(%1)")
                    .arg(Tools::roundNumber(atan(pt.at(1))*(180/M_PI), Tools::TRANSFORM));
        if (transform == "skewY(0)")
            transform.clear();
    } else {
        transform = "matrix(";
        for (int i = 0; i < pt.count(); ++i)
            transform += Tools::roundNumber(pt.at(i), Tools::TRANSFORM) + " ";
        transform.chop(1);
        transform += ")";
        if (transform == "matrix(0 0 0 0 0 0)")
            transform.clear();
    }
    return transform;
}

qreal Transform::scaleFactor() const
{
    return m_xScale;
}


// New class

QString Tools::roundNumber(qreal value, RoundType type)
{
    // check is number is integer
    double fractpart, intpart;
    fractpart = modf(value, &intpart);
    if (qFuzzyCompare(fractpart, 0))
        return QString::number((int)value);

    int precision;
    if (type == COORDINATE)
        precision = Keys::get().coordinatesPrecision();
    else if (type == ATTRIBUTE)
        precision = Keys::get().attributesPrecision();
    else
        precision = Keys::get().transformPrecision();

    QString text = QString::number(value, 'f', precision);

    // 1.100 -> 1.1
    while (text.at(text.count()-1) == QLatin1Char('0'))
        text.chop(1);
    // 1. -> 1
    if (text.at(text.count()-1) == QLatin1Char('.')) {
        text.chop(1);
        // already integer
        return text;
    }
    // 3.00001 -> 3
    if ((int)intpart != 0) {
        const int pointPos = text.indexOf('.', 1);
        if (text.mid(pointPos+1, precision-1).remove(QLatin1Char('0')).isEmpty())
            return text.left(pointPos);
    }

    // 0.1 -> .1
    if (text.midRef(0, 2) == QLatin1String("0."))
        text.remove(0, 1);
    // -0.1 -> -.1
    else if (text.midRef(0, 3) == QLatin1String("-0."))
        text.remove(1, 1);

    if (text == QLatin1String("-0"))
        return QLatin1String("0");
    else if (text.isEmpty())
        return QLatin1String("0");

    return text;
}

QString Tools::trimColor(QString color)
{
    color = color.toLower();

    // convert 'rgb (255, 255, 255)' to #RRGGBB
    if (!Keys::get().flag(Key::SkipColorToRRGGBB)) {
        if (color.contains("rgb")) {
            QStringList list = color.split(QRegExp("[^0-9\\.]"), QString::SkipEmptyParts);
            // convert 'rgb (100%, 100%, 100%)' to 'rgb (255, 255, 255)'
            if (color.contains("%")) {
                for (int i = 0; i < list.count(); ++i)
                    list.replace(i, QString::number((int)(list.at(i).toDouble()*255/100)));
            }
            color = "#";

            foreach (const QString &text, list)
                color += QString::number(text.toInt(), 16).rightJustified(2, '0');
        }

        // check is color set by name
        if (!color.contains("#"))
            color = replaceColorName(color);
    }

    if (!Keys::get().flag(Key::SkipRRGGBBToRGB)) {
        if (color.contains(QRegExp("[0-9a-f]"))) {
            // try to convert #rrggbb to #rgb
            if (color.size() == 7) { // #000000
                int inter = 0;
                for (int i = 1; i < 6; i += 2) {
                    if (color.at(i) == color.at(i+1))
                        inter++;
                }
                if (inter == 3)
                    color = '#' + color.at(1) + color.at(3) + color.at(5);
            }
        }
    }

    return color;
}

QString Tools::replaceColorName(const QString &color)
{
    static QHash<QString, QString> colors;
    colors.insert("aliceblue", "#f0f8ff");
    colors.insert("antiquewhite", "#faebd7");
    colors.insert("aqua", "#00ffff");
    colors.insert("aquamarine", "#7fffd4");
    colors.insert("azure", "#f0ffff");
    colors.insert("beige", "#f5f5dc");
    colors.insert("bisque", "#ffe4c4");
    colors.insert("black", "#000000");
    colors.insert("blanchedalmond", "#ffebcd");
    colors.insert("blue", "#0000ff");
    colors.insert("blueviolet", "#8a2be2");
    colors.insert("brown", "#a52a2a");
    colors.insert("burlywood", "#deb887");
    colors.insert("cadetblue", "#5f9ea0");
    colors.insert("chartreuse", "#7fff00");
    colors.insert("chocolate", "#d2691e");
    colors.insert("coral", "#ff7f50");
    colors.insert("cornflowerblue", "#6495ed");
    colors.insert("cornsilk", "#fff8dc");
    colors.insert("crimson", "#dc143c");
    colors.insert("cyan", "#00ffff");
    colors.insert("darkblue", "#00008b");
    colors.insert("darkcyan", "#008b8b");
    colors.insert("darkgoldenrod", "#b8860b");
    colors.insert("darkgray", "#a9a9a9");
    colors.insert("darkgreen", "#006400");
    colors.insert("darkkhaki", "#bdb76b");
    colors.insert("darkmagenta", "#8b008b");
    colors.insert("darkolivegreen", "#556b2f");
    colors.insert("darkorange", "#ff8c00");
    colors.insert("darkorchid", "#9932cc");
    colors.insert("darkred", "#8b0000");
    colors.insert("darksalmon", "#e9967a");
    colors.insert("darkseagreen", "#8fbc8f");
    colors.insert("darkslateblue", "#483d8b");
    colors.insert("darkslategray", "#2f4f4f");
    colors.insert("darkturquoise", "#00ced1");
    colors.insert("darkviolet", "#9400d3");
    colors.insert("deeppink", "#ff1493");
    colors.insert("deepskyblue", "#00bfff");
    colors.insert("dimgray", "#696969");
    colors.insert("dodgerblue", "#1e90ff");
    colors.insert("firebrick", "#b22222");
    colors.insert("floralwhite", "#fffaf0");
    colors.insert("forestgreen", "#228b22");
    colors.insert("fuchsia", "#ff00ff");
    colors.insert("gainsboro", "#dcdcdc");
    colors.insert("ghostwhite", "#f8f8ff");
    colors.insert("gold", "#ffd700");
    colors.insert("goldenrod", "#daa520");
    colors.insert("gray", "#808080");
    colors.insert("green", "#008000");
    colors.insert("greenyellow", "#adff2f");
    colors.insert("honeydew", "#f0fff0");
    colors.insert("hotpink", "#ff69b4");
    colors.insert("indianred", "#cd5c5c");
    colors.insert("indigo", "#4b0082");
    colors.insert("ivory", "#fffff0");
    colors.insert("khaki", "#f0e68c");
    colors.insert("lavender", "#e6e6fa");
    colors.insert("lavenderblush", "#fff0f5");
    colors.insert("lawngreen", "#7cfc00");
    colors.insert("lemonchiffon", "#fffacd");
    colors.insert("lightblue", "#add8e6");
    colors.insert("lightcoral", "#f08080");
    colors.insert("lightcyan", "#e0ffff");
    colors.insert("lightgoldenrodyellow", "#fafad2");
    colors.insert("lightgreen", "#90ee90");
    colors.insert("lightgrey", "#d3d3d3");
    colors.insert("lightpink", "#ffb6c1");
    colors.insert("lightsalmon", "#ffa07a");
    colors.insert("lightseagreen", "#20b2aa");
    colors.insert("lightskyblue", "#87cefa");
    colors.insert("lightslategray", "#778899");
    colors.insert("lightsteelblue", "#b0c4de");
    colors.insert("lightyellow", "#ffffe0");
    colors.insert("lime", "#00ff00");
    colors.insert("limegreen", "#32cd32");
    colors.insert("linen", "#faf0e6");
    colors.insert("magenta", "#ff00ff");
    colors.insert("maroon", "#800000");
    colors.insert("mediumaquamarine", "#66cdaa");
    colors.insert("mediumblue", "#0000cd");
    colors.insert("mediumorchid", "#ba55d3");
    colors.insert("mediumpurple", "#9370db");
    colors.insert("mediumseagreen", "#3cb371");
    colors.insert("mediumslateblue", "#7b68ee");
    colors.insert("mediumspringgreen", "#00fa9a");
    colors.insert("mediumturquoise", "#48d1cc");
    colors.insert("mediumvioletred", "#c71585");
    colors.insert("midnightblue", "#191970");
    colors.insert("mintcream", "#f5fffa");
    colors.insert("mistyrose", "#ffe4e1");
    colors.insert("moccasin", "#ffe4b5");
    colors.insert("navajowhite", "#ffdead");
    colors.insert("navy", "#000080");
    colors.insert("oldlace", "#fdf5e6");
    colors.insert("olive", "#808000");
    colors.insert("olivedrab", "#6b8e23");
    colors.insert("orange", "#ffa500");
    colors.insert("orangered", "#ff4500");
    colors.insert("orchid", "#da70d6");
    colors.insert("palegoldenrod", "#eee8aa");
    colors.insert("palegreen", "#98fb98");
    colors.insert("paleturquoise", "#afeeee");
    colors.insert("palevioletred", "#db7093");
    colors.insert("papayawhip", "#ffefd5");
    colors.insert("peachpuff", "#ffdab9");
    colors.insert("peru", "#cd853f");
    colors.insert("pink", "#ffc0cb");
    colors.insert("plum", "#dda0dd");
    colors.insert("powderblue", "#b0e0e6");
    colors.insert("purple", "#800080");
    colors.insert("red", "#ff0000");
    colors.insert("rosybrown", "#bc8f8f");
    colors.insert("royalblue", "#4169e1");
    colors.insert("saddlebrown", "#8b4513");
    colors.insert("salmon", "#fa8072");
    colors.insert("sandybrown", "#f4a460");
    colors.insert("seagreen", "#2e8b57");
    colors.insert("seashell", "#fff5ee");
    colors.insert("sienna", "#a0522d");
    colors.insert("silver", "#c0c0c0");
    colors.insert("skyblue", "#87ceeb");
    colors.insert("slateblue", "#6a5acd");
    colors.insert("slategray", "#708090");
    colors.insert("snow", "#fffafa");
    colors.insert("springgreen", "#00ff7f");
    colors.insert("steelblue", "#4682b4");
    colors.insert("tan", "#d2b48c");
    colors.insert("teal", "#008080");
    colors.insert("thistle", "#d8bfd8");
    colors.insert("tomato", "#ff6347");
    colors.insert("turquoise", "#40e0d0");
    colors.insert("violet", "#ee82ee");
    colors.insert("wheat", "#f5deb3");
    colors.insert("white", "#ffffff");
    colors.insert("whitesmoke", "#f5f5f5");
    colors.insert("yellow", "#ffff00");
    colors.insert("yellowgreen", "#9acd32");

    return colors.value(color);
}

bool Tools::nodeByTagNameSort(const SvgElement &node1, const SvgElement &node2)
{
    return  QString::localeAwareCompare(node1.tagName(), node2.tagName()) < 0;
}

void Tools::sortNodes(QList<SvgElement> &nodeList)
{
    qSort(nodeList.begin(), nodeList.end(), &Tools::nodeByTagNameSort);
}

QVariantHash Tools::initDefaultStyleHash()
{
    static QVariantHash hash;
    if (!hash.isEmpty())
        return hash;
    hash.insert("font-style", "normal");
    hash.insert("font-variant", "normal");
    hash.insert("font-weight", "normal");
    hash.insert("font-stretch", "normal");
    hash.insert("font-size", "medium");
    hash.insert("font-size-adjust", "none");
    hash.insert("kerning", "auto");
    hash.insert("letter-spacing", "normal");
    hash.insert("word-spacing", "normal");
    hash.insert("text-decoration", "none");
    hash.insert("writing-mode", "lr-tb");
    hash.insert("glyph-orientation-vertical", "auto");
    hash.insert("glyph-orientation-horizontal", "0deg");
    hash.insert("direction", "ltr");
    hash.insert("text-anchor", "start");
    hash.insert("dominant-baseline", "auto");
    hash.insert("alignment-baseline", "auto");
    hash.insert("baseline-shift", "baseline");
    hash.insert("fill-rule", "nonzero");
    hash.insert("fill-opacity", 1.0);
    hash.insertMulti("fill", "#000");
    hash.insertMulti("fill", "#000000");
    hash.insertMulti("fill", "black");
    hash.insert("stroke-linecap", "butt");
    // NOTE: Qt render works strage without this option
    hash.insert("stroke-linejoin", "miter");
    hash.insert("stroke-dasharray", "none");
    hash.insert("stroke-miterlimit", 4.0);
    hash.insert("stroke-dashoffset", 0);
    hash.insert("stroke-opacity", 1.0);
    hash.insert("stroke-width", 1.0);
    hash.insert("opacity", 1.0);
    hash.insert("display", "inline");
    hash.insert("visibility", "visible");
    hash.insert("enable-background", "accumulate");
    hash.insert("marker", "none");
    hash.insert("marker-start", "none");
    hash.insert("marker-mid", "none");
    hash.insert("marker-end", "none");
    hash.insert("clip", "auto");
    hash.insert("stop-opacity", 1.0);
    hash.insertMulti("stop-color", "#000000");
    hash.insertMulti("stop-color", "#000");
    hash.insertMulti("stop-color", "black");
    hash.insert("block-progression", "tb");
    return hash;
}

QRectF Tools::viewBoxRect(const SvgElement &svgElem)
{
    Q_ASSERT(svgElem.tagName() == "svg");
    QRectF rect;
    if (svgElem.hasAttribute("viewBox")) {
        QStringList list = svgElem.attribute("viewBox").split(" ");
        rect.setRect(list.at(0).toDouble(), list.at(1).toDouble(),
                     list.at(2).toDouble(), list.at(3).toDouble());
    } else if (svgElem.hasAttribute("width") && svgElem.hasAttribute("height")) {
        rect.setRect(0, 0, svgElem.attribute("width").toDouble(),
                           svgElem.attribute("height").toDouble());
    } else {
        qDebug() << "Warning: can not detect viewBox";
    }
    return rect;
}

QList<XMLNode *> Tools::childNodeList(XMLNode *node)
{
    QList<XMLNode *> list;
    for (XMLNode *child = node->FirstChild(); child; child = child->NextSibling())
        list << child;
    return list;
}

QList<SvgElement> Tools::childElemList(XMLDocument *doc)
{
    QList<SvgElement> list;
    for (XMLElement *child = doc->FirstChildElement(); child; child = child->NextSiblingElement())
        list << SvgElement(child);
    return list;
}

QList<SvgElement> Tools::childElemList(const SvgElement &node)
{
    QList<SvgElement> list;
    list.reserve(node.childElementCount());
    for (XMLElement *child = node.xmlElement()->FirstChildElement();
            child; child = child->NextSiblingElement())
        list << SvgElement(child);
    return list;
}

QString Tools::removeEdgeSpaces(const QString &str)
{
    QString tmpstr = str;
    while (tmpstr.at(0) == QLatin1Char(' '))
        tmpstr.remove(0,1);
    while (tmpstr.at(tmpstr.size()-1) == QLatin1Char(' '))
        tmpstr.remove(tmpstr.size()-1,1);
    return tmpstr;
}

StringHash Tools::splitStyle(QString style)
{
    StringHash hash;
    if (style.isEmpty())
        return hash;
    QStringList list = removeEdgeSpaces(style).split(";", QString::SkipEmptyParts);
    for (int i = 0; i < list.count(); ++i) {
        QString attr = list.at(i);
        int pos = attr.indexOf(QLatin1Char(':'));
        if (pos != -1)
            hash.insert(removeEdgeSpaces(attr.mid(0, pos)), removeEdgeSpaces(attr.mid(pos+1)));
    }
    return hash;
}

QString Tools::styleHashToString(const StringHash &hash)
{
    QString outStr;
    foreach (const QString &key, hash.keys())
        outStr += key + ":" + hash.value(key) + ";";
    outStr.chop(1);
    return outStr;
}

bool Tools::isAttributesEqual(const StringMap &map1, const StringMap &map2,
                              const QSet<QString> &attrList)
{
    foreach (const QString &attr, attrList) {
        if (map1.value(attr) != map2.value(attr))
            return false;
    }
    return true;
}

bool Tools::isGradientsEqual(const SvgElement &elem1, const SvgElement &elem2)
{
    if (elem1.childElementCount() != elem2.childElementCount())
        return false;

    QList<SvgElement> list1 = elem1.childElemList();
    QList<SvgElement> list2 = elem2.childElemList();

    for (int i = 0; i < list1.size(); ++i) {
        SvgElement childElem1 = list1.at(i);
        SvgElement childElem2 = list2.at(i);

        if (childElem1.tagName() != childElem2.tagName())
            return false;

        foreach (const QString &attrName, Props::stopAttributes) {
            if (childElem1.attribute(attrName) != childElem2.attribute(attrName))
                return false;
        }
    }
    return true;
}

SvgElement Tools::svgElement(XMLDocument *doc)
{
    XMLElement *child;
    for (child = doc->FirstChildElement(); child; child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "svg") == 0) {
            break;
        }
    }
    return SvgElement(child);
}

SvgElement Tools::defsElement(XMLDocument *doc, SvgElement &svgElem)
{
    XMLElement *child;
    for (child = svgElem.xmlElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "defs") == 0) {
            break;
        }
    }
    if (child == 0) {
        XMLElement* element = doc->NewElement("defs");
        svgElem.xmlElement()->InsertFirstChild(element);
        child = element;
    }
    return SvgElement(child);
}

QString Tools::convertUnitsToPx(const QString &text, qreal baseValue)
{
    if (text.contains("px"))
        return QString(text).remove("px");

    // TODO: em/ex
    if (text.contains("em") || text.contains("ex"))
        return text;

    QString value;
    QString unit;
    if (text.size() == 1) {
        if (text.at(0).isDigit()) {
            value = text;
        } else {
            value = "0";
        }
    } else if (text.rightRef(1) == QLatin1String("%")) {
        unit = "%";
        value = text.mid(0, text.size() - 1);
    } else if (text.at(text.size()-1).isLetter()) {
        unit = text.right(2);
        value = text.mid(0, text.size() - 2);
    } else {
        value = text;
    }

    bool ok = false;
    qreal number = value.toDouble(&ok);
    Q_ASSERT(ok == true);

    if (unit == QLatin1String("pt"))
        number = number * 1.25;
    else if (unit == QLatin1String("pc"))
        number = number * 15;
    else if (unit == QLatin1String("mm"))
        number = number * 3.543307;
    else if (unit == QLatin1String("cm"))
        number = number * 35.43307;
    else if (unit == QLatin1String("in"))
        number = number * 90;
    else if (unit == QLatin1String("%") && baseValue > 0)
        number = number * baseValue / 100;
    else
        return text;

    return roundNumber(number);
}
