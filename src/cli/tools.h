#ifndef TOOLS_H
#define TOOLS_H

#include <QtCore/QMap>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QVariantHash>
#include <QtCore/QRectF>
#include <QtXml/QDomNode>
// NOTE: only one dependence from gui
#include <QtGui/QGenericMatrix>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;
typedef QGenericMatrix<3,3,qreal> TransMatrix;

namespace RegEx {
    static const QString lengthTypes = "em|ex|px|in|cm|mm|pt|pc|%";
    static const QRegExp xlinkUrl = QRegExp(".*url\\(#|\\).*");
}

class SvgElement : public QDomElement
{
public:
    explicit SvgElement() : QDomElement() {}
    SvgElement(const QDomElement &elem) : QDomElement(elem) {}

    bool hasAttributeByList(const QStringList &list);
    bool hasAttributeByList(const QSet<QString> &list);
    bool hasAttributes(const QStringList &list);
    bool isStyleContains(const QString &text);
    void removeAttributes(const QStringList &list);
    QStringList attributesList();
    QList<SvgElement> childElemList();
    QList<QDomNode> childNodeList();
    bool isReferenced();
    bool isText() const;
    bool isContainer() const;
    bool isGroup() const;
    StringHash styleHash();
    void setStyle(const QString &text);
    void appendStyle(const QString &text);
    void setAttribute(const QString &name, const QVariant &value);
    QString style() const;
    QString id() const;    
};

class Transform
{
public:
    explicit Transform(const QString &text);
    void setOldXY(qreal prevX, qreal prevY);
    qreal newX();
    qreal newY();
    QString simplified() const;
    qreal scaleFactor();

private:
    QList<qreal> m_points;
    bool m_isXMirror;
    bool m_isYMirror;
    qreal m_baseX;
    qreal m_baseY;
    qreal m_angle;
    qreal m_xScale;
    qreal m_yScale;
    qreal m_xMove;
    qreal m_yMove;

    QList<qreal> mergeMatrixes(QString text);
};

class Tools
{
public:
    explicit Tools() {}
    enum RoundType { COORDINATES, TRANSFORM, ATTRIBUTES };
    static bool isAttrEqual(QDomElement elem1, QDomElement node2, const QSet<QString> &atrr);
    static QDomNode findDefsNode(const QDomNode SvgElement);
    static QList<QDomNode> childNodeList(QDomNode node);
    static QList<SvgElement> childElemList(QDomNode node);
    static QString convertUnitsToPx(const QString &text, qreal baseValue = 0);
    static QString replaceColorName(const QString &color);
    static QString roundNumber(qreal value, RoundType type = COORDINATES);
    static QString styleHashToString(const StringHash &hash);
    static QString trimColor(QString color);
    static StringHash splitStyle(QString style);
    static void sortNodes(QList<QDomNode> *nodeList);
    static QVariantHash initDefaultStyleHash();
    static QSet<QString> usedElemList(const SvgElement &svgNode);
    static QRectF viewBoxRect(const SvgElement &svgNode);
};

// TODO: add percentages attr list
// TODO: move all this to SVGElement class
// TODO: sort in order of max using
namespace Props {
static const QSet<QString> fillList = QSet<QString>() << "fill" << "fill-rule" << "fill-opacity";
static const QSet<QString> strokeList = QSet<QString>()
    << "stroke" << "stroke-width" << "stroke-linecap" << "stroke-linejoin" << "stroke-miterlimit"
    << "stroke-dasharray" << "stroke-dashoffset" << "stroke-opacity";

static const QSet<QString> styleAttrList = QSet<QString>()
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

// ordered by the degree of use
static const QSet<QString> linearGradient  = QSet<QString>()
    << "gradientTransform" << "xlink:href" << "style" << "x1" << "y1" << "x2" << "y2"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

// ordered by the degree of use
static const QSet<QString> radialGradient = QSet<QString>()
    << "gradientTransform" << "xlink:href" << "style" << "cx" << "cy" << "r" << "fx" << "fy"
    << "gradientUnits" << "spreadMethod" << "externalResourcesRequired";

static const QSet<QString> digitList = QSet<QString>()
    << "x" << "y" << "x1" << "y1" << "x2" << "y2" << "width" << "height" << "r" << "rx" << "ry"
    << "fx" << "fy" << "cx" << "cy" << "offset";

static const QSet<QString> filterDigitList = QSet<QString>()
    << "stdDeviation" << "baseFrequency" << "k" << "k1" << "k2" << "k3" << "specularConstant"
    << "dx" << "dy";

static const QSet<QString> defsList = QSet<QString>()
    << "altGlyphDef" << "clipPath" << "cursor" << "filter" << "linearGradient"
    << "marker" << "mask" << "pattern" << "radialGradient"/* << "symbol"*/;

static const QSet<QString> referencedElements = QSet<QString>()
    << "a" << "altGlyphDef" << "clipPath" << "color-profile" << "cursor" << "filter" << "font"
    << "font-face" << "foreignObject" << "image" << "marker" << "mask" << "pattern" << "script"
    << "style" << "switch" << "text" << "view";

static const QSet<QString> textElements = QSet<QString>()
    << "text" << "tspan" << "flowRoot" << "flowPara" << "flowSpan";

static const QSet<QString> textAttributes = QSet<QString>()
    << "font-style" << "font-variant" << "font-weight" << "font-weight" << "font-stretch"
    << "font-size" << "font-size-adjust" << "kerning" << "letter-spacing" << "word-spacing"
    << "text-decoration" << "writing-mode" << "glyph-orientation-vertical"
    << "glyph-orientation-horizontal" << "direction" << "text-anchor" << "dominant-baseline"
    << "alignment-baseline" << "baseline-shift";

static const QVariantHash defaultStyleValues = Tools::initDefaultStyleHash();

static const QSet<QString> svgElementList = QSet<QString>()
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

static const QSet<QString> containers = QSet<QString>()
    << "a" << "defs" << "glyph" << "g" << "marker" /*<< "mask"*/ << "missing-glyph" /*<< "pattern"*/
    << "svg" << "switch" <<  "symbol";
}

#endif // TOOLS_H
