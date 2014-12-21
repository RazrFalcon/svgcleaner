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

Q_CORE_EXPORT double qstrtod(const char *s00, char const **se, bool *ok);

bool isZero(double value)
{
    u_static double minValue = 1.0 / pow(10, Keys::get().coordinatesPrecision());
    return (qAbs(value) < minValue);
}

bool isZeroTs(double value)
{
    u_static double minValue = 1.0 / pow(10, Keys::get().transformPrecision());
    return (qAbs(value) < minValue);
}

QString fromDouble(double value, Round::RoundType type)
{
    int precision;
    if (type == Round::Coordinate)
        precision = Keys::get().coordinatesPrecision();
    else if (type == Round::Attribute)
        precision = Keys::get().attributesPrecision();
    else
        precision = Keys::get().transformPrecision();
    return fromDouble(value, precision);
}

QString fromDouble(double value, int precision)
{
    QVarLengthArray<ushort> array;
    doubleToVarArr(array, value, precision);
    return QString(reinterpret_cast<QChar *>(array.data()), array.size());
}

void doubleToVarArr(QVarLengthArray<ushort> &arr, double value, int precision)
{
    double fractpart, intpart;
    fractpart = modf(value, &intpart);

    // round number when fraction part is really small
    // when fraction part is smaller than 1% of integer part
    // 24.2008 -> 24.2
    // 2.01738 -> 2.02
    // 3.004   -> 3
    if (qAbs(fractpart/intpart*100) < 1.1f) {
        double v = pow(10, (precision-1));
        double fractpart2 = qRound(fractpart * v) / v;
        value = intpart + fractpart2;
    }

    uint multiplier = 1;
    while (precision--)
        multiplier *= 10;
    double tmpValue = qRound64(qAbs(value) * multiplier);

    static const ushort zero = '0';
    if (qFuzzyIsNull(tmpValue)) {
        arr.append(zero);
        return;
    }

    double newValue = tmpValue/multiplier;

    int decimalPointPos = Tools::numbersBeforePoint(newValue);
    int zeroAfterPoint  = Tools::zerosAfterPoint(newValue);

    qulonglong l = tmpValue;
    ushort buff[65];
    ushort *p = buff + 65;
    static const ushort point = '.';
    static const ushort sign  = '-';
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
            *(--p) = zero + c;
            charCount++;
        }
        pos--;
        if (pos == decimalPointPos && decimalPointPos != 0) {
            if (charCount > 0)
                *(--p) = point;
            isTrailingZero = false;
        }
        l /= 10;
    }
    while (zeroAfterPoint--)
        *(--p) = zero;
    if (decimalPointPos == 0) {
        *(--p) = point;
        u_static const bool useLeadingZero = !Keys::get().flag(Key::RemoveUnneededSymbols);
        if (useLeadingZero)
            *(--p) = zero;
    }
    if (value < 0)
        *(--p) = sign;

    arr.append(p, 65 - (p - buff));
}

int Tools::numbersBeforePoint(double value)
{
    int v = floor(value);
    int count = 0;
    while(v) {
        v /= 10;
        count++;
    }
    return count;
}

int Tools::zerosAfterPoint(double value)
{
    double v = qAbs(value);
    int count = 0;
    while(v < 0.1) {
        v *= 10;
        count++;
    }
    return count;
}

double toDouble(const QString &str)
{
    return StringWalker(str).number(StringWalker::NoSkip);
}

// check is space or non printable character
bool isSpace(ushort ch)
{
    // '9'  is character tabulation
    // '10' is line feed (LF)
    // '11' is line tabulation
    // '12' is form feed (FF)
    // '13' is carriage return (CR)
    // '32' is UTF-8 space
    if ((ch >= 9 && ch <= 13) || ch == 32)
        return true;
    return false;
}

QString Tools::trimColor(const QString &color)
{
    QString newColor = color.toLower();

    // convert 'rgb (255, 255, 255)' to #RRGGBB
    if (Keys::get().flag(Key::ConvertColorToRRGGBB)) {
        if (newColor.contains(QL1S("rgb"))) {
            StringWalker sw(newColor);
            QVector<double> nums;
            nums.reserve(3);
            while (!sw.atEnd()) {
                sw.jumpTo(QL1C('('));
                sw.skipSpaces();
                sw.next();
                for (int i = 0; i < 3; ++i) {
                    nums << sw.number();
                    if (sw.current() == LengthType::Percent)
                        sw.next();
                    if (sw.current() == QL1C(','))
                        sw.next();
                }
                sw.jumpTo(QL1C(')'));
                sw.next();
            }
            // convert 'rgb (100%, 100%, 100%)' to 'rgb (255, 255, 255)'
            if (newColor.contains(LengthType::Percent)) {
                for (int i = 0; i < 3; ++i)
                    nums[i] = nums.at(i) * 255 / 100;
            }
            newColor = QL1C('#');
            foreach (const double &value, nums)
                newColor += QString::number((int)value, 16).rightJustified(2, QL1C('0'));
        }

        // check is color set by name
        if (!newColor.contains(QL1C('#')))
            newColor = replaceColorName(newColor);
    }

    if (Keys::get().flag(Key::ConvertRRGGBBToRGB)) {
        if (newColor.startsWith(QL1C('#'))) {
            // try to convert #rrggbb to #rgb
            if (newColor.size() == 7) { // #000000
                int inter = 0;
                for (int i = 1; i < 6; i += 2) {
                    if (newColor.at(i) == newColor.at(i+1))
                        inter++;
                }
                if (inter == 3)
                    newColor = QL1C('#') + newColor.at(1) + newColor.at(3) + newColor.at(5);
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

// http://www.w3.org/TR/SVG11/coords.html#Units
QString Tools::convertUnitsToPx(const QString &text, double baseValue)
{
    QString unit;
    double number = 0;
    StringWalker sw(text);
    while (!sw.atEnd()) {
        number = sw.number();
        while ((sw.current().isLetter() || sw.current() == LengthType::Percent) && !sw.atEnd()) {
            unit += sw.current();
            sw.next();
        }
    }

    if (unit == LengthType::px)
        return fromDouble(number, Round::Attribute);

    // fix string parsing, getNum func detect 'e' char as exponent...
    if (unit == QL1S("x"))
        unit = LengthType::ex;
    else if (unit == QL1S("m"))
        unit = LengthType::em;

    // note that all relative units depends on users screen dpi
    // and cleaner use 90dpi as default

    if (unit == LengthType::pt)
        number = number * 1.25;
    else if (unit == LengthType::pc)
        number = number * 15;
    else if (unit == LengthType::mm)
        number = number * 3.543307;
    else if (unit == LengthType::cm)
        number = number * 35.43307;
    else if (unit == LengthType::in)
        number = number * 90;
    else if (unit == LengthType::Percent)
        number = number * baseValue / 100;
    else if (unit == LengthType::em)
        number = number * baseValue;
    else if (unit == LengthType::ex)
        number = number * baseValue / 2;
    else
        return text;

    return fromDouble(number, Round::Attribute);
}


// Simple 'const QChar *' wrapper

StringWalker::StringWalker(const QString &text)
{
    str = text.constData();
    end = str + text.size();
}

int StringWalker::jumpTo(const QChar &c)
{
    int len = 0;
    while (!atEnd() && *str != c) {
        len++;
        str++;
    }
    return len;
}

int StringWalker::jumpToSpace()
{
    int len = 0;
    while (!isSpace(str->unicode())) {
        str++;
        len++;
    }
    return len;
}

QString StringWalker::readBefore(int len) const
{
    return QString(str - len, len);
}

void StringWalker::next()
{
    str++;
}

void StringWalker::next(int count)
{
    for (int i = 0; i < count; ++i)
        str++;
}

void StringWalker::skipSpaces()
{
    while (isSpace(str->unicode()))
        str++;
}

bool StringWalker::atEnd() const
{
    return (str == end);
}

bool StringWalker::isValid() const
{
    return str;
}

QChar StringWalker::current() const
{
    return *str;
}

const QChar *&StringWalker::data()
{
    return str;
}

// the number() code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
double StringWalker::number(Opt opt)
{
    skipSpaces();

    const int maxLen = 255; // technically doubles can go til 308+ but whatever
    char temp[maxLen+1];
    int pos = 0;

    if (*str == QL1C('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QL1C('+')) {
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->unicode();
        ++str;
    }
    if (*str == QL1C('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->unicode();
        ++str;
    }
    bool exponent = false;
    if ((*str == QL1C('e') || *str == QL1C('E')) && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QL1C('-') || *str == QL1C('+')) && pos < maxLen) {
            temp[pos++] = str->unicode();
            ++str;
        }
        while (isDigit(str->unicode()) && pos < maxLen) {
            temp[pos++] = str->unicode();
            ++str;
        }
    }

    temp[pos] = '\0';

    double val;
    if (!exponent && pos < 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if (*t == '-') {
            neg = true;
            ++t;
        }
        while (*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if (*t == '.') {
            ++t;
            int div = 1;
            while (*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((double)ival)/((double)div);
        } else {
            val = ival;
        }
        if (neg)
            val = -val;
    } else {
        bool ok = false;
        val = qstrtod(temp, 0, &ok);
    }

    skipSpaces();
    if (opt == SkipComma && *str == QL1C(','))
        ++str;
    return val;
}

// the isDigit code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
// '0' is 0x30 and '9' is 0x39
bool StringWalker::isDigit(ushort ch)
{
    static quint16 magic = 0x3ff;
    return ((ch >> 4) == 3) && (magic >> (ch & 15));
}
