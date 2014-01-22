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

#ifndef KEYS_H
#define KEYS_H

#include <QStringList>
#include <QHash>
#include <QMap>
#include <QObject>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;
typedef QSet<QString> StringSet;
typedef QSet<const char *> CharSet;
typedef QList<const char *> CharList;

namespace Preset {
    static const QString Basic    = "basic";
    static const QString Complete = "complete";
    static const QString Extreme  = "extreme";
    static const QString Custom   = "custom";
}

namespace Key {
    enum Key {
        RemoveProlog,
        RemoveComments,
        RemoveProcInstruction,
        RemoveUnusedDefs,
        RemoveNonSvgElements,
        RemoveMetadata,
        RemoveInkscapeElements,
        RemoveSodipodiElements,
        RemoveAdobeElements,
        RemoveCorelDrawElements,
        RemoveMSVisioElements,
        RemoveSketchElements,
        RemoveInvisibleElements,
        RemoveEmptyContainers,
        RemoveDuplicatedDefs,
        RemoveOutsideElements,
        ReplaceEqualEltsByUse,
        UngroupContainers,
        MergeGradients,
        RemoveTinyGaussianBlur,

        RemoveSvgVersion,
        RemoveUnreferencedIds,
        TrimIds,
        RemoveNotAppliedAttributes,
        RemoveDefaultAttributes,
        RemoveInkscapeAttributes,
        RemoveSodipodiAttributes,
        RemoveAdobeAttributes,
        RemoveCorelDrawAttributes,
        RemoveMSVisioAttributes,
        RemoveSketchAttributes,
        RemoveStrokeProps,
        RemoveFillProps,
        RemoveUnusedXLinks,
        GroupElemByStyle,
        SimplifyTransformMatrix,
        ApplyTransformsToDefs,
        ApplyTransformsToShapes,

        JoinStyleAttributes,
        KeepNamedIds,

        ConvertToRelative,
        RemoveUnneededSymbols,
        RemoveTinySegments,
        ConvertSegments,
        ApplyTransformsToPaths,

        CreateViewbox,
        ConvertColorToRRGGBB,
        ConvertRRGGBBToRGB,
        ConvertBasicShapes,
        TransformPrecision,
        CoordsPrecision,
        AttributesPrecision,
        CompactOutput,

        SortDefs,

        ShortOutput
    };
}

namespace KeyStr {
    static const QString RemoveProlog                = "--remove-prolog";
    static const QString RemoveComments              = "--remove-comments";
    static const QString RemoveProcInstruction       = "--remove-proc-instr";
    static const QString RemoveUnusedDefs            = "--remove-unused-defs";
    static const QString RemoveNonSvgElements        = "--remove-nonsvg-elts";
    static const QString RemoveMetadata              = "--remove-metadata-elts";
    static const QString RemoveInkscapeElements      = "--remove-inkscape-elts";
    static const QString RemoveSodipodiElements      = "--remove-sodipodi-elts";
    static const QString RemoveAdobeElements         = "--remove-ai-elts";
    static const QString RemoveCorelDrawElements     = "--remove-corel-elts";
    static const QString RemoveMSVisioElements       = "--remove-msvisio-elts";
    static const QString RemoveSketchElements        = "--remove-sketch-elts";
    static const QString RemoveInvisibleElements     = "--remove-invisible-elts";
    static const QString RemoveEmptyContainers       = "--remove-empty-containers";
    static const QString RemoveTinyGaussianBlur      = "--remove-gaussian-blur";
    static const QString RemoveDuplicatedDefs        = "--remove-duplicated-defs";
    static const QString RemoveOutsideElements       = "--remove-outside-elts";
    static const QString ReplaceEqualEltsWithUse     = "--equal-elts-to-use";
    static const QString UngroupContainers           = "--ungroup-containers";
    static const QString MergeGradients              = "--merge-gradients";

    static const QString RemoveSvgVersion            = "--remove-version";
    static const QString RemoveUnreferencedIds       = "--remove-unreferenced-ids";
    static const QString TrimIds                     = "--trim-ids";
    static const QString KeepNamedIds                = "--keep-named-ids";
    static const QString RemoveNotAppliedAttributes  = "--remove-notappl-atts";
    static const QString RemoveDefaultAttributes     = "--remove-default-atts";
    static const QString RemoveInkscapeAttributes    = "--remove-inkscape-atts";
    static const QString RemoveSodipodiAttributes    = "--remove-sodipodi-atts";
    static const QString RemoveAdobeAttributes       = "--remove-ai-atts";
    static const QString RemoveCorelDrawAttributes   = "--remove-corel-atts";
    static const QString RemoveMSVisioAttributes     = "--remove-msvisio-atts";
    static const QString RemoveSketchAttributes      = "--remove-sketch-atts";
    static const QString RemoveStrokeProps           = "--remove-stroke-props";
    static const QString RemoveFillProps             = "--remove-fill-props";
    static const QString RemoveUnusedXLinks          = "--remove-unused-xlinks";
    static const QString GroupElemByStyle            = "--group-elts-by-styles";
    static const QString JoinStyleAttributes         = "--join-style-atts";
    static const QString SimplifyTransformMatrix     = "--simplify-transform-matrix";
    static const QString ApplyTransformsToDefs       = "--apply-transforms-to-defs";
    static const QString ApplyTransformsToShapes     = "--apply-transforms-to-shapes";
    // TODO: remove bitmaps, as utility

    static const QString ConvertToRelative           = "--convert-to-relative";
    static const QString RemoveUnneededSymbols       = "--remove-unneeded-symbols";
    static const QString RemoveTinySegments          = "--remove-tiny-segments";
    static const QString ConvertSegments             = "--convert-segments";
    static const QString ApplyTransformsToPaths      = "--apply-transforms-to-paths";

    static const QString CreateViewbox               = "--create-viewbox";
    static const QString ConvertColorToRRGGBB        = "--colors-to-rrggbb";
    static const QString ConvertRRGGBBToRGB          = "--rrggbb-to-rgb";
    static const QString ConvertBasicShapes          = "--convert-basic-shapes";
    static const QString TransformPrecision          = "--transform-precision";
    static const QString CoordsPrecision             = "--coordinates-precision";
    static const QString AttributesPrecision         = "--attributes-precision";
    static const QString CompactOutput               = "--compact-output";
    static const QString SortDefs                    = "--sort-defs";
    // TODO: convert all shapes to path, as utility

    static const QString ShortOutput                 = "--short-output";
}

// singleton
class Keys : public QObject
{
    Q_OBJECT

public:
    static Keys& get() {
        static Keys instance;
        return instance;
    }
    bool flag(const int &key) const;
    int intNumber(const int &key) const;
    int coordinatesPrecision() const;
    int attributesPrecision() const;
    int transformPrecision() const;
    double doubleNumber(const int &key) const;
    void parseOptions(QStringList &list);
    void prepareDescription();
    QString description(const int &key);
    QList<int> elementsKeysId();
    QList<int> attributesKeysId();
    QList<int> attributesUtilsKeysId();
    QList<int> pathsKeysId();
    QList<int> optimizationsKeys();
    QList<int> optimizationsUtilsKeys();
    QList<int> basicPresetKeys();
    QList<int> completePresetKeys();
    QList<int> extremePresetKeys();
    void setPreset(const QString &name);
    QStringList& allKeys();
    QString keyName(const int &key);

private:
    QHash<int, double> numHash;
    QSet<int> *flags;
    QHash<int, QString> descHash;
    int m_coordinatesPrecision;
    int m_attributesPrecision;
    int m_transformPrecision;

    Keys(QObject *parent = 0);
    Keys(Keys const&);
    void operator=(Keys const&);
};

#endif // KEYS_H
