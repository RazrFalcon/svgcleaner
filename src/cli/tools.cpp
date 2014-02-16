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

#include <QVector>
#include <cmath>

#include "tools.h"

namespace Props {
const StringSet fillList = StringSet() << "fill" << "fill-rule" << "fill-opacity";
const StringSet strokeList = StringSet()
    << "stroke" << "stroke-width" << "stroke-linecap" << "stroke-linejoin" << "stroke-miterlimit"
    << "stroke-dasharray" << "stroke-dashoffset" << "stroke-opacity";

const StringSet presentationAttributes = StringSet()
    << "alignment-baseline" << "baseline-shift" << "clip-path" << "clip-rule" << "clip"
    << "color-interpolation-filters" << "color-interpolation" << "color-profile"
    << "color-rendering" << "color" << "cursor" << "direction" << "display" << "dominant-baseline"
    << "enable-background" << "fill-opacity" << "fill-rule" << "fill" << "filter" << "flood-color"
    << "flood-opacity" << "font-family" << "font-size-adjust" << "font-size" << "font-stretch"
    << "font-style" << "font-variant" << "font-weight" << "glyph-orientation-horizontal"
    << "glyph-orientation-vertical" << "image-rendering" << "kerning" << "letter-spacing"
    << "lighting-color" << "marker-end" << "marker-mid" << "marker-start" << "mask" << "opacity"
    << "overflow" << "pointer-events" << "shape-rendering" << "stop-color" << "stop-opacity"
    << "stroke-dasharray" << "stroke-dashoffset" << "stroke-linecap" << "stroke-linejoin"
    << "stroke-miterlimit" << "stroke-opacity" << "stroke-width" << "stroke" << "text-anchor"
    << "text-decoration" << "text-rendering" << "unicode-bidi" << "visibility" << "word-spacing"
    << "writing-mode";

const CharList linkableStyleAttributes = CharList()
    << "clip-path" << "fill" << "mask" << "filter" << "stroke" << "marker-start"
    << "marker-mid" << "marker-end";

const QStringList linearGradient = QStringList()
    << "gradientTransform" << "xlink:href" << "x1" << "y1" << "x2" << "y2"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

const QStringList radialGradient = QStringList()
    << "gradientTransform" << "xlink:href" << "cx" << "cy" << "r" << "fx" << "fy"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

const QStringList filter = QStringList()
    << "gradientTransform" << "xlink:href" << "x" << "y" << "width" << "height" << "filterRes"
    << "filterUnits" << "primitiveUnits" << "externalResourcesRequired";

const StringSet maskAttributes = StringSet()
    << "x" << "y" << "width" << "height"
    << "maskUnits" << "maskContentUnits" << "externalResourcesRequired";

const StringSet digitList = StringSet()
    << "x" << "y" << "x1" << "y1" << "x2" << "y2" << "width" << "height" << "r" << "rx" << "ry"
    << "fx" << "fy" << "cx" << "cy" << "dx" << "dy" << "offset";

const StringSet filterDigitList = StringSet()
    << "stdDeviation" << "baseFrequency" << "k" << "k1" << "k2" << "k3" << "specularConstant"
    << "dx" << "dy" << "stroke-dasharray";

const StringSet defsList = StringSet()
    << "altGlyphDef" << "clipPath" << "cursor" << "filter" << "linearGradient"
    << "marker" << "mask" << "pattern" << "radialGradient"/* << "symbol"*/;

const StringSet referencedElements = StringSet()
    << "a" << "altGlyphDef" << "clipPath" << "color-profile" << "cursor" << "filter" << "font"
    << "font-face" << "foreignObject" << "image" << "marker" << "mask" << "pattern" << "script"
    << "style" << "switch" << "text" << "view";

const StringSet textElements = StringSet()
    << "text" << "tspan" << "flowRoot" << "flowPara" << "flowSpan" << "textPath";

const StringSet textAttributes = StringSet()
    << "font-style" << "font-variant" << "font-weight" << "font-weight" << "font-stretch"
    << "font-size" << "font-size-adjust" << "kerning" << "letter-spacing" << "word-spacing"
    << "text-decoration" << "writing-mode" << "glyph-orientation-vertical"
    << "glyph-orientation-horizontal" << "direction" << "text-anchor" << "dominant-baseline"
    << "alignment-baseline" << "baseline-shift";
const QVariantHash defaultStyleValues = Tools::initDefaultStyleHash();

const StringSet svgElementList = StringSet()
    << "a" << "altGlyph" << "altGlyphDef" << "altGlyphItem" << "animate" << "animateColor"
    << "animateMotion" << "animateTransform" << "circle" << "clipPath" << "color-profile"
    << "cursor" << "defs" << "desc" << "ellipse" << "feBlend" << "feColorMatrix"
    << "feComponentTransfer" << "feComposite" << "feConvolveMatrix" << "feDiffuseLighting"
    << "feDisplacementMap" << "feDistantLight" << "feFlood" << "feFuncA" << "feFuncB" << "feFuncG"
    << "feFuncR" << "feGaussianBlur" << "feImage" << "feMerge" << "feMergeNode" << "feMorphology"
    << "feOffset" << "fePointLight" << "feSpecularLighting" << "feSpotLight" << "feTile"
    << "feTurbulence" << "filter" << "font" << "font-face" << "font-face-format" << "font-face-name"
    << "font-face-src" << "font-face-uri" << "foreignObject" << "g" << "glyph" << "glyphRef"
    << "hkern" << "image" << "line" << "linearGradient" << "marker" << "mask" << "metadata"
    << "missing-glyph" << "mpath" << "path" << "pattern" << "polygon" << "polyline"
    << "radialGradient" << "rect" << "script" << "set" << "stop" << "style" << "svg" << "switch"
    << "symbol" << "text" << "textPath" << "title" << "tref" << "flowRoot" << "flowRegion"
    << "flowPara" << "flowSpan" << "tspan" << "use" << "view" << "vkern";

const StringSet elementsUsingXLink = StringSet()
    << "a" << "altGlyph" << "color-profile" << "cursor" << "feImage" << "filter" << "font-face-uri"
    << "glyphRef" << "image" << "linearGradient" << "mpath" << "pattern" << "radialGradient"
    << "script" << "textPath" << "use" << "animate" << "animateColor" << "animateMotion"
    << "animateTransform" << "set" << "tref";

const StringSet containers = StringSet()
    << "a" << "defs" << "glyph" << "g" << "marker" /*<< "mask"*/ << "missing-glyph" /*<< "pattern"*/
    << "svg" << "switch" <<  "symbol";

const StringSet stopAttributes = StringSet()
    << "offset" << "stop-color" << "stop-opacity";

const StringSet lengthTypes = StringSet()
    << "em" << "ex" << "px" << "in" << "cm" << "mm" << "pt" << "pc";
}

namespace CleanerAttr {
    const char * const UsedElement = "used-element";
    const char * const BoundingBox = "bbox";
    const char * const BBoxTransform = "bbox-transform";
}


Q_CORE_EXPORT double qstrtod(const char *s00, char const **se, bool *ok);
Q_CORE_EXPORT char *qdtoa(double d, int mode, int ndigits, int *decpt,
                          int *sign, char **rve, char **digits_str);

QString Tools::roundNumber(qreal value, RoundType type)
{
    int precision;
    if (type == COORDINATE)
        precision = Keys::get().coordinatesPrecision();
    else if (type == ATTRIBUTE)
        precision = Keys::get().attributesPrecision();
    else
        precision = Keys::get().transformPrecision();
    return roundNumber(value, precision);
}

QString Tools::roundNumber(qreal value, int precision)
{
    double fractpart, intpart;
    fractpart = modf(value, &intpart);

    // round number when fraction part is really small
    // when fraction part is smaller than 1% of integer part
    // 24.2008 -> 24.2
    // 2.01738 -> 2.02
    // 3.004   -> 3
    if (qAbs(fractpart/intpart*100) < 1.1f) {
        qreal v = pow(10, (precision-1));
        qreal fractpart2 = qRound(fractpart * v) / v;
        value = intpart + fractpart2;
    }
    return doubleToStr(value, precision);
}

QString Tools::doubleToStr(const qreal value, int precision)
{
    uint multiplier = 1;
    while (precision--)
        multiplier *= 10;
    qreal tmpValue = qRound64(qAbs(value) * multiplier);

    if (qFuzzyCompare(tmpValue, 0.0))
        return "0";

    qreal newValue = tmpValue/multiplier;

    int decimalPointPos = numbersBeforePoint(newValue);
    int zeroAfterPoint = zerosAfterPoint(newValue);

    qulonglong l = tmpValue;
    ushort buff[65];
    ushort *p = buff + 65;
    static ushort m_zero = QChar('0').unicode();
    int pos = 0;
    while (l != 0) {
        pos++;
        l /= 10;
    }
    l = tmpValue;
    bool isTrailingZero = true;
    int charCount = 0;
    while (l != 0) {
        int c = l % 10;
        if (c != 0)
            isTrailingZero = false;
        if (!isTrailingZero) {
            *(--p) = m_zero + c;
            charCount++;
        }
        pos--;
        if (pos == decimalPointPos && decimalPointPos != 0) {
            if (charCount > 0)
                *(--p) = QChar('.').unicode();
            isTrailingZero = false;
        }
        l /= 10;
    }
    while (zeroAfterPoint--)
        *(--p) = m_zero;
    if (decimalPointPos == 0) {
        *(--p) = QChar('.').unicode();
        static const bool useLeadingZero = !Keys::get().flag(Key::RemoveUnneededSymbols);
        if (useLeadingZero)
            *(--p) = m_zero;
    }
    if (value < 0)
        *(--p) = QChar('-').unicode();
    return QString(reinterpret_cast<QChar *>(p), 65 - (p - buff));
}

int Tools::numbersBeforePoint(qreal value)
{
    int v = floor(value);
    int count = 0;
    while(v) {
        v /= 10;
        count++;
    }
    return count;
}

int Tools::zerosAfterPoint(qreal value)
{
    qreal v = qAbs(value);
    int count = 0;
    while(v < 0.1) {
        v *= 10;
        count++;
    }
    return count;
}

qreal Tools::getNum(const QChar *&str)
{
    while (str->isSpace())
        ++str;
    qreal num = toDouble(str);
    while (str->isSpace())
        ++str;
    if (*str == QLatin1Char(','))
        ++str;
    return num;
}

qreal Tools::strToDouble(const QString &str)
{
    const QChar *ch = str.constData();
    return toDouble(ch);
}

// the isDigit code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
// '0' is 0x30 and '9' is 0x39
bool Tools::isDigit(ushort ch)
{
    static quint16 magic = 0x3ff;
    return ((ch >> 4) == 3) && (magic >> (ch & 15));
}

// the toDouble code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
qreal Tools::toDouble(const QChar *&str)
{
    const int maxLen = 255; // technically doubles can go til 308+ but whatever
    char temp[maxLen+1];
    int pos = 0;

    if (*str == QLatin1Char('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QLatin1Char('+')) {
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    if (*str == QLatin1Char('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    bool exponent = false;
    if ((*str == QLatin1Char('e') || *str == QLatin1Char('E')) && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QLatin1Char('-') || *str == QLatin1Char('+')) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
        while (isDigit(str->unicode()) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
    }

    temp[pos] = '\0';

    qreal val;
    if (!exponent && pos < 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if(*t == '-') {
            neg = true;
            ++t;
        }
        while(*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if(*t == '.') {
            ++t;
            int div = 1;
            while(*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((qreal)ival)/((qreal)div);
        } else {
            val = ival;
        }
        if (neg)
            val = -val;
    } else {
#if defined(Q_WS_QWS) && !defined(Q_OS_VXWORKS)
        if(sizeof(qreal) == sizeof(float))
            val = strtof(temp, 0);
        else
#endif
        {
            bool ok = false;
            val = qstrtod(temp, 0, &ok);
        }
    }
    return val;
}

QString Tools::trimColor(const QString &color)
{
    QString newColor = color.toLower();

    // convert 'rgb (255, 255, 255)' to #RRGGBB
    if (Keys::get().flag(Key::ConvertColorToRRGGBB)) {
        if (newColor.contains(QL1S("rgb"))) {
            const QChar *str = newColor.constData();
            const QChar *end = str + newColor.size();
            QVector<qreal> nums;
            nums.reserve(3);
            while (str != end) {
                while (str->isSpace() || *str != QLatin1Char('('))
                    ++str;
                ++str;
                for (int i = 0; i < 3; ++i) {
                    nums << getNum(str);
                    if (*str == QLatin1Char('%'))
                        ++str;
                    if (*str == QLatin1Char(','))
                        ++str;
                }
                while (*str != QLatin1Char(')'))
                    ++str;
                ++str;
            }
            // convert 'rgb (100%, 100%, 100%)' to 'rgb (255, 255, 255)'
            if (newColor.contains(QLatin1Char('%'))) {
                for (int i = 0; i < 3; ++i)
                    nums[i] = nums.at(i) * 255 / 100;
            }
            newColor = QLatin1Char('#');
            foreach (const qreal &value, nums)
                newColor += QString::number((int)value, 16).rightJustified(2, QLatin1Char('0'));
        }

        // check is color set by name
        if (!newColor.contains(QLatin1Char('#')))
            newColor = replaceColorName(newColor);
    }

    if (Keys::get().flag(Key::ConvertRRGGBBToRGB)) {
        if (newColor.startsWith(QLatin1Char('#'))) {
            // try to convert #rrggbb to #rgb
            if (newColor.size() == 7) { // #000000
                int inter = 0;
                for (int i = 1; i < 6; i += 2) {
                    if (newColor.at(i) == newColor.at(i+1))
                        inter++;
                }
                if (inter == 3)
                    newColor = QLatin1Char('#') + newColor.at(1) + newColor.at(3) + newColor.at(5);
            }
        }
    }
    return newColor;
}

QString Tools::replaceColorName(const QString &color)
{
    static QHash<QString, QString> colors;
    if (!colors.isEmpty())
        return colors.value(color);
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

QVariantHash Tools::initDefaultStyleHash()
{
    static QVariantHash hash;
    if (!hash.isEmpty())
        return hash;
    hash.insert("alignment-baseline", "auto");
    hash.insert("baseline-shift", "baseline");
    hash.insert("block-progression", "tb");
    hash.insert("clip", "auto");
    hash.insert("clip-path", "none");
    hash.insert("clip-rule", "nonzero");
    hash.insert("direction", "ltr");
    hash.insert("display", "inline");
    hash.insert("dominant-baseline", "auto");
    hash.insert("enable-background", "accumulate");
    hash.insert("fill-opacity", 1.0);
    hash.insert("fill-rule", "nonzero");
    hash.insert("filter", "none");
    hash.insert("flood-color", "black");
    hash.insert("font-size-adjust", "none");
    hash.insert("font-size", "medium");
    hash.insert("font-stretch", "normal");
    hash.insert("font-style", "normal");
    hash.insert("font-variant", "normal");
    hash.insert("font-weight", "normal");
    hash.insert("glyph-orientation-horizontal", "0deg");
    hash.insert("glyph-orientation-vertical", "auto");
    hash.insert("kerning", "auto");
    hash.insert("letter-spacing", "normal");
    hash.insert("marker-start", "none");
    hash.insert("marker-mid", "none");
    hash.insert("marker-end", "none");
    hash.insert("mask", "none");
    hash.insert("opacity", 1.0);
    hash.insert("overflow", "visible");
    hash.insert("pointer-events", "visiblePainted");
    hash.insert("stop-opacity", 1.0);
    hash.insert("stroke-dasharray", "none");
    hash.insert("stroke-dashoffset", 0);
    hash.insert("stroke-linecap", "butt");
    hash.insert("stroke-linejoin", "miter");
    hash.insert("stroke-miterlimit", 4.0);
    hash.insert("stroke", "none");
    hash.insert("stroke-opacity", 1.0);
    hash.insert("stroke-width", 1.0);
    hash.insert("text-anchor", "start");
    hash.insert("text-decoration", "none");
    hash.insert("visibility", "visible");
    hash.insert("word-spacing", "normal");
    hash.insert("writing-mode", "lr-tb");
    return hash;
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

StringHash Tools::splitStyle(const QString &style)
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

bool Tools::isZero(qreal value)
{
    static qreal minValue = 1 / pow(10, Keys::get().coordinatesPrecision());
    return (qAbs(value) < minValue);
}

bool Tools::isZeroTs(qreal value)
{
    static qreal minValue = 1 / pow(10, Keys::get().transformPrecision());
    return (qAbs(value) < minValue);
}

// http://www.w3.org/TR/SVG11/coords.html#Units
QString Tools::convertUnitsToPx(const QString &text, qreal baseValue)
{
    QString unit;
    qreal number = 0;
    const QChar *str = text.constData();
    const QChar *end = str + text.size();
    while (str != end) {
        number = getNum(str);
        while ((str->isLetter() || *str == QLatin1Char('%')) && str != end) {
            unit += *str;
            ++str;
        }
    }

    if (unit == QL1S("px"))
        return roundNumber(number, Tools::ATTRIBUTE);

    // fix string parsing, getNum func detect 'e' char as exponent...
    if (unit == QL1S("x"))
        unit = QL1S("ex");
    else if (unit == QL1S("m"))
        unit = QL1S("em");

    // note that all relative units depends on users screen dpi
    // and cleaner use 90dpi as default

    if (unit == QL1S("pt"))
        number = number * 1.25;
    else if (unit == QL1S("pc"))
        number = number * 15;
    else if (unit == QL1S("mm"))
        number = number * 3.543307;
    else if (unit == QL1S("cm"))
        number = number * 35.43307;
    else if (unit == QL1S("in"))
        number = number * 90;
    else if (unit == QL1S("%"))
        number = number * baseValue / 100;
    else if (unit == QL1S("em"))
        number = number * baseValue;
    else if (unit == QL1S("ex"))
        number = number * baseValue / 2;
    else
        return text;

    return roundNumber(number, Tools::ATTRIBUTE);
}
