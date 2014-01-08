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

#include <QVariantHash>
#include <QStringList>
#include <QMap>
#include <QObject>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;
typedef QSet<QString> StringSet;

namespace Preset {
    static const QString Basic    = "basic";
    static const QString Complete = "complete";
    static const QString Extreme  = "extreme";
}

namespace Key {
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
    static const QString RemoveInvisibleElements     = "--remove-invisible-elts";
    static const QString RemoveEmptyContainers       = "--remove-empty-containers";
    static const QString RemoveTinyGaussianBlur      = "--remove-gaussian-blur";
    static const QString RemoveDuplicatedDefs        = "--remove-duplicated-defs";
    static const QString UngroupGroups               = "--ungroup-groups";
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
    static const QString RemoveStrokeProps           = "--remove-stroke-props";
    static const QString RemoveFillProps             = "--remove-fill-props";
    static const QString RemoveGradientCoordinates   = "--remove-gradient-coords";
    static const QString RemoveUnusedXLinks          = "--remove-unused-xlinks";
    static const QString GroupElemByStyle            = "--group-elements-by-styles";
    static const QString JoinStyleAttributes         = "--join-style-atts";
    // TODO: remove bitmaps, as utility

    static const QString ConvertToRelative           = "--convert-to-relative";
    static const QString RemoveUnneededSymbols       = "--remove-unneeded-symbols";
    static const QString RemoveTinySegments          = "--remove-tiny-segments";
    static const QString ConvertCurves               = "--convert-curves";

    static const QString CreateViewbox               = "--create-viewbox";
    static const QString ConvertColorToRRGGBB        = "--colors-to-rrggbb";
    static const QString ConvertRRGGBBToRGB          = "--rrggbb-to-rgb";
    static const QString ConvertBasicShapes          = "--convert-basic-shapes";
    static const QString ApplyTransforms             = "--apply-transforms";
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
    bool flag(const QString &key) const;
    int intNumber(const QString &key) const;
    int coordinatesPrecision() const;
    int attributesPrecision() const;
    int transformPrecision() const;
    double doubleNumber(const QString &key) const;
    void parseOptions(QStringList &list);
    void prepareDescription();
    QString description(const QString &key);
    QStringList elementsKeys();
    QStringList attributesKeys();
    QStringList attributesUtilsKeys();
    QStringList pathsKeys();
    QStringList optimizationsKeys();
    QStringList optimizationsUtilsKeys();
    QStringList basicPresetKeys();
    QStringList completePresetKeys();
    QStringList extremePresetKeys();
    void setPreset(const QString &name);

private:
    QVariantHash hash;
    StringHash descHash;
    int m_coordinatesPrecision;
    int m_attributesPrecision;
    int m_transformPrecision;

    Keys(QObject *parent = 0);
    Keys(Keys const&);
    void operator=(Keys const&);
};

#endif // KEYS_H
