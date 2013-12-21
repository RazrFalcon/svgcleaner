/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

namespace Key {
    static const QString KeepProlog = "--keep-prolog";
    static const QString KeepComments = "--keep-comments";
    static const QString KeepProcessingInstruction = "--keep-proc-instr";
    static const QString KeepUnusedDefs = "--keep-unused-defs";
    static const QString KeepNonSvgElements = "--keep-nonsvg-elts";
    static const QString KeepMetadata = "--keep-metadata-elts";
    static const QString KeepInkscapeElements = "--keep-inkscape-elts";
    static const QString KeepSodipodiElements = "--keep-sodipodi-elts";
    static const QString KeepIllustratorElements = "--keep-ai-elts";
    static const QString KeepCorelDrawElements = "--keep-cdr-elts";
    static const QString KeepMSVisioElements = "--keep-msvisio-elts";
    static const QString KeepInvisibleElements = "--keep-invisible-elts";
    static const QString KeepEmptyContainer = "--keep-empty-conts";
    static const QString KeepGroups = "--keep-groups";
    static const QString KeepDuplicatedDefs = "--keep-dupl-defs";
    static const QString KeepSinglyGradients = "--keep-singly-grads";
    static const QString KeepTinyGaussianBlur = "--keep-gaussian-blur";
    static const QString StdDeviation = "--std-dev";

    static const QString KeepSvgVersion = "--keep-version";
    static const QString KeepUnreferencedIds = "--keep-unref-ids";
    static const QString KeepNamedIds = "--keep-named-ids";
    static const QString KeepNotAppliedAttributes = "--keep-notappl-atts";
    static const QString KeepDefaultAttributes = "--keep-default-atts";
    static const QString KeepInkscapeAttributes = "--keep-inkscape-atts";
    static const QString KeepSodipodiAttributes = "--keep-sodipodi-atts";
    static const QString KeepIllustratorAttributes = "--keep-ai-atts";
    static const QString KeepCorelDrawAttributes = "--keep-cdr-atts";
    static const QString KeepMSVisioAttributes = "--keep-msvisio-atts";
    static const QString KeepStrokeProps = "--keep-stroke-props";
    static const QString KeepFillProps = "--keep-fill-props";
    static const QString KeepGradientCoordinates = "--keep-grad-coords";
    static const QString KeepUnusedXLinks = "--keep-unused-xlinks";
    static const QString SkipElemByStyleGroup = "--skip-style-group";
    static const QString SkipIdsTrim = "--skip-ids-trim";

    static const QString KeepAbsolutePaths = "--keep-absolute-paths";
    static const QString KeepUnusedSymbolsFromPath = "--keep-unused-symbols";
    static const QString KeepEmptySegments = "--keep-empty-segments";
    static const QString KeepOriginalCurveTo = "--keep-curveto";

    static const QString NoViewbox = "--no-viewbox";
    static const QString JoinStyleAttributes = "--join-style-atts";
    static const QString SkipColorToRRGGBB = "--skip-color-to-rrggbb";
    static const QString SkipRRGGBBToRGB = "--skip-rrggbb-to-rgb";
    static const QString KeepBasicShapes = "--keep-basic-shapes";
    static const QString KeepTransforms = "--keep-transform";
    static const QString KeepUnsortedDefs = "--keep-unsorted-defs";
    static const QString SkipRoundingNumbers = "--skip-rounding-numbers";
    static const QString TransformPrecision = "--transfs-prec";
    static const QString CoordsPrecision = "--coords-prec";
    static const QString AttributesPrecision = "--attrs-prec";

    static const QString NotCompact = "--not-compact";
}

// singleton
class Keys
{
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
    void parseOptions(const QStringList &list);

private:
    QVariantHash keyHash;
    int m_coordinatesPrecision;
    int m_attributesPrecision;
    int m_transformPrecision;

    Keys();
    Keys(Keys const&);
    void operator=(Keys const&);
};

#endif // KEYS_H
