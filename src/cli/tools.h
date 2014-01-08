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

#ifndef TOOLS_H
#define TOOLS_H

// TODO: remove regex as much as possible
#include <QRegExp>
#include <QStringList>
#include <QRectF>
#include <QtDebug>

#include "svgelement.h"

class TransformMatrix
{
public:
    TransformMatrix() { setToIdentity(); }
    void setToIdentity()
    {
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                if (row == col)
                    m[col][row] = 1.0f;
                else
                    m[col][row] = 0.0f;
            }
        }
    }

    TransformMatrix operator *(const TransformMatrix &matrix)
    {
        TransformMatrix result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                qreal sum(0.0f);
                for (int j = 0; j < 3; ++j)
                    sum += m[j][row] * matrix.m[col][j];
                result.m[col][row] = sum;
            }
        }
        return result;
    }
    qreal& operator()(int row, int column)
    {
        Q_ASSERT(row >= 0 && row < 3 && column >= 0 && column < 3);
        return m[column][row];
    }
private:
    qreal m[3][3];
};

class Transform
{
public:
    explicit Transform(const QString &text);
    void setOldXY(qreal prevX, qreal prevY);
    qreal newX() const;
    qreal newY() const;
    QString simplified() const;
    qreal scaleFactor() const;
    qreal xScale() const;
    qreal yScale() const;
    bool isProportionalScale();
    bool isMirrored();
    bool isRotating();

private:
    QList<qreal> m_points;
    qreal m_baseX;
    qreal m_baseY;
    qreal m_xScale;
    qreal m_yScale;

    QList<TransformMatrix> parseTransform(const QStringRef &text);
    QList<qreal> mergeMatrixes(QString text);
};

class Tools
{
public:
    explicit Tools() {}
    enum RoundType { COORDINATE, TRANSFORM, ATTRIBUTE };
    static bool isAttributesEqual(const StringMap &map1, const StringMap &map2,
                                  const StringSet &attrList);
    static bool isGradientsEqual(const SvgElement &elem1, const SvgElement &elem2);
    static bool isZero(qreal value);
    static SvgElement svgElement(XMLDocument *doc);
    static SvgElement defsElement(XMLDocument *doc, SvgElement &svgElem);
    static QList<XMLNode *> childNodeList(XMLNode *node);
    static QList<SvgElement> childElemList(const SvgElement &node);
    static QList<SvgElement> childElemList(XMLDocument *doc);
    static QString convertUnitsToPx(const QString &text, qreal baseValue = 0);
    static QString replaceColorName(const QString &color);
    static QString roundNumber(qreal value, RoundType type = COORDINATE);
    static QString roundNumber(qreal value, int precision);
    static QString styleHashToString(const StringHash &hash);
    static QString trimColor(QString color);
    static void sortNodes(QList<SvgElement> &nodeList);
    static QVariantHash initDefaultStyleHash();
    static QRectF viewBoxRect(const SvgElement &svgElem);
    static StringHash splitStyle(QString style);
    static QString removeEdgeSpaces(const QString &str);
    static qreal getNum(const QChar *&str);
    static qreal strToDouble(const QString &str);

private:
    static bool nodeByTagNameSort(const SvgElement &node1, const SvgElement &node2);
    static bool isDigit(ushort ch);
    static qreal toDouble(const QChar *&str);
};

namespace Props {
static const StringSet fillList = StringSet() << "fill" << "fill-rule" << "fill-opacity";
static const StringSet strokeList = StringSet()
    << "stroke" << "stroke-width" << "stroke-linecap" << "stroke-linejoin" << "stroke-miterlimit"
    << "stroke-dasharray" << "stroke-dashoffset" << "stroke-opacity";

static const StringSet styleAttributes = StringSet()
    << "alignment-baseline" << "baseline-shift" << "clip" << "clip-path" << "clip-rule" << "color"
    << "color-interpolation" << "color-interpolation-filters" << "color-profile"
    << "color-rendering" << "cursor" << "direction" << "display" << "dominant-baseline"
    << "enable-background" << "fill" << "fill-opacity" << "fill-rule" << "filter" << "flood-color"
    << "flood-opacity" << "font" << "font-family" << "font-size" << "font-size-adjust"
    << "font-stretch" << "font-style" << "font-variant" << "font-weight"
    << "glyph-orientation-horizontal" << "glyph-orientation-vertical" << "image-rendering"
    << "kerning" << "letter-spacing" /*<< "lighting-color"*/ << "marker" << "marker-end"
    << "marker-mid" << "marker-start" << "mask" << "opacity" << "overflow" << "pointer-events"
    << "shape-rendering" << "stop-color" << "stop-opacity" << "stroke" << "stroke-dasharray"
    << "stroke-dashoffset" << "stroke-linecap" << "stroke-linejoin" << "stroke-miterlimit"
    << "stroke-opacity" << "stroke-width" << "text-anchor" << "text-decoration" << "text-rendering"
    << "unicode-bidi" << "visibility" << "word-spacing" << "writing-mode";

static const QStringList linkableStyleAttributes = QStringList()
    << "clip-path" << "fill" << "mask" << "filter" << "stroke" << "marker-start"
    << "marker-mid" << "marker-end";

// ordered by the degree of use
static const StringSet linearGradient = StringSet()
    << "gradientTransform" << "xlink:href" << "x1" << "y1" << "x2" << "y2"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

// ordered by the degree of use
static const StringSet radialGradient = StringSet()
    << "gradientTransform" << "xlink:href" << "cx" << "cy" << "r" << "fx" << "fy"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

static const StringSet filter = StringSet()
    << "gradientTransform" << "xlink:href" << "x" << "y" << "width" << "height" << "filterRes"
    << "filterUnits" << "primitiveUnits" << "externalResourcesRequired";

static const StringSet maskAttributes = StringSet()
    << "x" << "y" << "width" << "height"
    << "maskUnits" << "maskContentUnits" << "externalResourcesRequired";

static const StringSet digitList = StringSet()
    << "x" << "y" << "x1" << "y1" << "x2" << "y2" << "width" << "height" << "r" << "rx" << "ry"
    << "fx" << "fy" << "cx" << "cy" << "offset";

static const StringSet filterDigitList = StringSet()
    << "stdDeviation" << "baseFrequency" << "k" << "k1" << "k2" << "k3" << "specularConstant"
    << "dx" << "dy";

static const StringSet defsList = StringSet()
    << "altGlyphDef" << "clipPath" << "cursor" << "filter" << "linearGradient"
    << "marker" << "mask" << "pattern" << "radialGradient"/* << "symbol"*/;

static const StringSet referencedElements = StringSet()
    << "a" << "altGlyphDef" << "clipPath" << "color-profile" << "cursor" << "filter" << "font"
    << "font-face" << "foreignObject" << "image" << "marker" << "mask" << "pattern" << "script"
    << "style" << "switch" << "text" << "view";

static const StringSet textElements = StringSet()
    << "text" << "tspan" << "flowRoot" << "flowPara" << "flowSpan";

static const StringSet textAttributes = StringSet()
    << "font-style" << "font-variant" << "font-weight" << "font-weight" << "font-stretch"
    << "font-size" << "font-size-adjust" << "kerning" << "letter-spacing" << "word-spacing"
    << "text-decoration" << "writing-mode" << "glyph-orientation-vertical"
    << "glyph-orientation-horizontal" << "direction" << "text-anchor" << "dominant-baseline"
    << "alignment-baseline" << "baseline-shift";

static const QVariantHash defaultStyleValues = Tools::initDefaultStyleHash();

static const StringSet svgElementList = StringSet()
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

static const StringSet containers = StringSet()
    << "a" << "defs" << "glyph" << "g" << "marker" /*<< "mask"*/ << "missing-glyph" /*<< "pattern"*/
    << "svg" << "switch" <<  "symbol";

static const StringSet stopAttributes = StringSet()
    << "offset" << "stop-color" << "stop-opacity";

static const StringSet lengthTypes = StringSet()
    << "em" << "ex" << "px" << "in" << "cm" << "mm" << "pt" << "pc";
}

namespace CleanerAttr {
    static const QString UsedElement = "used-element";
}

#endif // TOOLS_H
