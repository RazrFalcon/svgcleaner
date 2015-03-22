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

#include "tools.h"

Q_CORE_EXPORT double qstrtod(const char *s00, char const **se, bool *ok);

QString fromDouble(double value, Round::RoundType type)
{
    return fromDouble(value, Keys::get().precision(type));
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

double toDouble(const QString &str, bool *ok)
{
    return StringWalker(str).number(StringWalker::NoSkip, ok);
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
    colors.insert(QL1S("aliceblue"), QL1S("#f0f8ff"));
    colors.insert(QL1S("antiquewhite"), QL1S("#faebd7"));
    colors.insert(QL1S("aqua"), QL1S("#00ffff"));
    colors.insert(QL1S("aquamarine"), QL1S("#7fffd4"));
    colors.insert(QL1S("azure"), QL1S("#f0ffff"));
    colors.insert(QL1S("beige"), QL1S("#f5f5dc"));
    colors.insert(QL1S("bisque"), QL1S("#ffe4c4"));
    colors.insert(QL1S("black"), QL1S("#000000"));
    colors.insert(QL1S("blanchedalmond"), QL1S("#ffebcd"));
    colors.insert(QL1S("blue"), QL1S("#0000ff"));
    colors.insert(QL1S("blueviolet"), QL1S("#8a2be2"));
    colors.insert(QL1S("brown"), QL1S("#a52a2a"));
    colors.insert(QL1S("burlywood"), QL1S("#deb887"));
    colors.insert(QL1S("cadetblue"), QL1S("#5f9ea0"));
    colors.insert(QL1S("chartreuse"), QL1S("#7fff00"));
    colors.insert(QL1S("chocolate"), QL1S("#d2691e"));
    colors.insert(QL1S("coral"), QL1S("#ff7f50"));
    colors.insert(QL1S("cornflowerblue"), QL1S("#6495ed"));
    colors.insert(QL1S("cornsilk"), QL1S("#fff8dc"));
    colors.insert(QL1S("crimson"), QL1S("#dc143c"));
    colors.insert(QL1S("cyan"), QL1S("#00ffff"));
    colors.insert(QL1S("darkblue"), QL1S("#00008b"));
    colors.insert(QL1S("darkcyan"), QL1S("#008b8b"));
    colors.insert(QL1S("darkgoldenrod"), QL1S("#b8860b"));
    colors.insert(QL1S("darkgray"), QL1S("#a9a9a9"));
    colors.insert(QL1S("darkgreen"), QL1S("#006400"));
    colors.insert(QL1S("darkkhaki"), QL1S("#bdb76b"));
    colors.insert(QL1S("darkmagenta"), QL1S("#8b008b"));
    colors.insert(QL1S("darkolivegreen"), QL1S("#556b2f"));
    colors.insert(QL1S("darkorange"), QL1S("#ff8c00"));
    colors.insert(QL1S("darkorchid"), QL1S("#9932cc"));
    colors.insert(QL1S("darkred"), QL1S("#8b0000"));
    colors.insert(QL1S("darksalmon"), QL1S("#e9967a"));
    colors.insert(QL1S("darkseagreen"), QL1S("#8fbc8f"));
    colors.insert(QL1S("darkslateblue"), QL1S("#483d8b"));
    colors.insert(QL1S("darkslategray"), QL1S("#2f4f4f"));
    colors.insert(QL1S("darkturquoise"), QL1S("#00ced1"));
    colors.insert(QL1S("darkviolet"), QL1S("#9400d3"));
    colors.insert(QL1S("deeppink"), QL1S("#ff1493"));
    colors.insert(QL1S("deepskyblue"), QL1S("#00bfff"));
    colors.insert(QL1S("dimgray"), QL1S("#696969"));
    colors.insert(QL1S("dodgerblue"), QL1S("#1e90ff"));
    colors.insert(QL1S("firebrick"), QL1S("#b22222"));
    colors.insert(QL1S("floralwhite"), QL1S("#fffaf0"));
    colors.insert(QL1S("forestgreen"), QL1S("#228b22"));
    colors.insert(QL1S("fuchsia"), QL1S("#ff00ff"));
    colors.insert(QL1S("gainsboro"), QL1S("#dcdcdc"));
    colors.insert(QL1S("ghostwhite"), QL1S("#f8f8ff"));
    colors.insert(QL1S("gold"), QL1S("#ffd700"));
    colors.insert(QL1S("goldenrod"), QL1S("#daa520"));
    colors.insert(QL1S("gray"), QL1S("#808080"));
    colors.insert(QL1S("green"), QL1S("#008000"));
    colors.insert(QL1S("greenyellow"), QL1S("#adff2f"));
    colors.insert(QL1S("honeydew"), QL1S("#f0fff0"));
    colors.insert(QL1S("hotpink"), QL1S("#ff69b4"));
    colors.insert(QL1S("indianred"), QL1S("#cd5c5c"));
    colors.insert(QL1S("indigo"), QL1S("#4b0082"));
    colors.insert(QL1S("ivory"), QL1S("#fffff0"));
    colors.insert(QL1S("khaki"), QL1S("#f0e68c"));
    colors.insert(QL1S("lavender"), QL1S("#e6e6fa"));
    colors.insert(QL1S("lavenderblush"), QL1S("#fff0f5"));
    colors.insert(QL1S("lawngreen"), QL1S("#7cfc00"));
    colors.insert(QL1S("lemonchiffon"), QL1S("#fffacd"));
    colors.insert(QL1S("lightblue"), QL1S("#add8e6"));
    colors.insert(QL1S("lightcoral"), QL1S("#f08080"));
    colors.insert(QL1S("lightcyan"), QL1S("#e0ffff"));
    colors.insert(QL1S("lightgoldenrodyellow"), QL1S("#fafad2"));
    colors.insert(QL1S("lightgreen"), QL1S("#90ee90"));
    colors.insert(QL1S("lightgrey"), QL1S("#d3d3d3"));
    colors.insert(QL1S("lightpink"), QL1S("#ffb6c1"));
    colors.insert(QL1S("lightsalmon"), QL1S("#ffa07a"));
    colors.insert(QL1S("lightseagreen"), QL1S("#20b2aa"));
    colors.insert(QL1S("lightskyblue"), QL1S("#87cefa"));
    colors.insert(QL1S("lightslategray"), QL1S("#778899"));
    colors.insert(QL1S("lightsteelblue"), QL1S("#b0c4de"));
    colors.insert(QL1S("lightyellow"), QL1S("#ffffe0"));
    colors.insert(QL1S("lime"), QL1S("#00ff00"));
    colors.insert(QL1S("limegreen"), QL1S("#32cd32"));
    colors.insert(QL1S("linen"), QL1S("#faf0e6"));
    colors.insert(QL1S("magenta"), QL1S("#ff00ff"));
    colors.insert(QL1S("maroon"), QL1S("#800000"));
    colors.insert(QL1S("mediumaquamarine"), QL1S("#66cdaa"));
    colors.insert(QL1S("mediumblue"), QL1S("#0000cd"));
    colors.insert(QL1S("mediumorchid"), QL1S("#ba55d3"));
    colors.insert(QL1S("mediumpurple"), QL1S("#9370db"));
    colors.insert(QL1S("mediumseagreen"), QL1S("#3cb371"));
    colors.insert(QL1S("mediumslateblue"), QL1S("#7b68ee"));
    colors.insert(QL1S("mediumspringgreen"), QL1S("#00fa9a"));
    colors.insert(QL1S("mediumturquoise"), QL1S("#48d1cc"));
    colors.insert(QL1S("mediumvioletred"), QL1S("#c71585"));
    colors.insert(QL1S("midnightblue"), QL1S("#191970"));
    colors.insert(QL1S("mintcream"), QL1S("#f5fffa"));
    colors.insert(QL1S("mistyrose"), QL1S("#ffe4e1"));
    colors.insert(QL1S("moccasin"), QL1S("#ffe4b5"));
    colors.insert(QL1S("navajowhite"), QL1S("#ffdead"));
    colors.insert(QL1S("navy"), QL1S("#000080"));
    colors.insert(QL1S("oldlace"), QL1S("#fdf5e6"));
    colors.insert(QL1S("olive"), QL1S("#808000"));
    colors.insert(QL1S("olivedrab"), QL1S("#6b8e23"));
    colors.insert(QL1S("orange"), QL1S("#ffa500"));
    colors.insert(QL1S("orangered"), QL1S("#ff4500"));
    colors.insert(QL1S("orchid"), QL1S("#da70d6"));
    colors.insert(QL1S("palegoldenrod"), QL1S("#eee8aa"));
    colors.insert(QL1S("palegreen"), QL1S("#98fb98"));
    colors.insert(QL1S("paleturquoise"), QL1S("#afeeee"));
    colors.insert(QL1S("palevioletred"), QL1S("#db7093"));
    colors.insert(QL1S("papayawhip"), QL1S("#ffefd5"));
    colors.insert(QL1S("peachpuff"), QL1S("#ffdab9"));
    colors.insert(QL1S("peru"), QL1S("#cd853f"));
    colors.insert(QL1S("pink"), QL1S("#ffc0cb"));
    colors.insert(QL1S("plum"), QL1S("#dda0dd"));
    colors.insert(QL1S("powderblue"), QL1S("#b0e0e6"));
    colors.insert(QL1S("purple"), QL1S("#800080"));
    colors.insert(QL1S("red"), QL1S("#ff0000"));
    colors.insert(QL1S("rosybrown"), QL1S("#bc8f8f"));
    colors.insert(QL1S("royalblue"), QL1S("#4169e1"));
    colors.insert(QL1S("saddlebrown"), QL1S("#8b4513"));
    colors.insert(QL1S("salmon"), QL1S("#fa8072"));
    colors.insert(QL1S("sandybrown"), QL1S("#f4a460"));
    colors.insert(QL1S("seagreen"), QL1S("#2e8b57"));
    colors.insert(QL1S("seashell"), QL1S("#fff5ee"));
    colors.insert(QL1S("sienna"), QL1S("#a0522d"));
    colors.insert(QL1S("silver"), QL1S("#c0c0c0"));
    colors.insert(QL1S("skyblue"), QL1S("#87ceeb"));
    colors.insert(QL1S("slateblue"), QL1S("#6a5acd"));
    colors.insert(QL1S("slategray"), QL1S("#708090"));
    colors.insert(QL1S("snow"), QL1S("#fffafa"));
    colors.insert(QL1S("springgreen"), QL1S("#00ff7f"));
    colors.insert(QL1S("steelblue"), QL1S("#4682b4"));
    colors.insert(QL1S("tan"), QL1S("#d2b48c"));
    colors.insert(QL1S("teal"), QL1S("#008080"));
    colors.insert(QL1S("thistle"), QL1S("#d8bfd8"));
    colors.insert(QL1S("tomato"), QL1S("#ff6347"));
    colors.insert(QL1S("turquoise"), QL1S("#40e0d0"));
    colors.insert(QL1S("violet"), QL1S("#ee82ee"));
    colors.insert(QL1S("wheat"), QL1S("#f5deb3"));
    colors.insert(QL1S("white"), QL1S("#ffffff"));
    colors.insert(QL1S("whitesmoke"), QL1S("#f5f5f5"));
    colors.insert(QL1S("yellow"), QL1S("#ffff00"));
    colors.insert(QL1S("yellowgreen"), QL1S("#9acd32"));
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
    // and SVG Cleaner use 90dpi as default

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

StringWalker::StringWalker(const QChar *astr, int size)
{
    str = astr;
    end = str + size;
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

uint StringWalker::readBeforeId(int len) const
{
    return hash(str - len, len);
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

const QChar &StringWalker::current() const
{
    return *str;
}

// the number() code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
double StringWalker::number(Opt opt, bool *ok)
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

    if (ok)
        *ok = (str == end);

    temp[pos] = '\0';

    double val;
    if (!exponent && pos <= 10) {
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
        val = qstrtod(temp, 0, 0);
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
