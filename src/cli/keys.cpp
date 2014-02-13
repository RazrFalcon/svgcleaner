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

#include <QtDebug>

#include "keys.h"

namespace Preset {
    const QString Basic    = "basic";
    const QString Complete = "complete";
    const QString Extreme  = "extreme";
    const QString Custom   = "custom";
}

namespace KeyStr {
    const QString RemoveProlog                = "--remove-prolog";
    const QString RemoveComments              = "--remove-comments";
    const QString RemoveProcInstruction       = "--remove-proc-instr";
    const QString RemoveUnusedDefs            = "--remove-unused-defs";
    const QString RemoveNonSvgElements        = "--remove-nonsvg-elts";
    const QString RemoveMetadata              = "--remove-metadata-elts";
    const QString RemoveInkscapeElements      = "--remove-inkscape-elts";
    const QString RemoveSodipodiElements      = "--remove-sodipodi-elts";
    const QString RemoveAdobeElements         = "--remove-ai-elts";
    const QString RemoveCorelDrawElements     = "--remove-corel-elts";
    const QString RemoveMSVisioElements       = "--remove-msvisio-elts";
    const QString RemoveSketchElements        = "--remove-sketch-elts";
    const QString RemoveInvisibleElements     = "--remove-invisible-elts";
    const QString RemoveEmptyContainers       = "--remove-empty-containers";
    const QString RemoveTinyGaussianBlur      = "--remove-gaussian-blur";
    const QString RemoveDuplicatedDefs        = "--remove-duplicated-defs";
    const QString RemoveOutsideElements       = "--remove-outside-elts";
    const QString ReplaceEqualEltsByUse       = "--equal-elts-to-use";
    const QString UngroupContainers           = "--ungroup-containers";
    const QString MergeGradients              = "--merge-gradients";

    const QString RemoveSvgVersion            = "--remove-version";
    const QString RemoveUnreferencedIds       = "--remove-unreferenced-ids";
    const QString TrimIds                     = "--trim-ids";
    const QString KeepNamedIds                = "--keep-named-ids";
    const QString RemoveNotAppliedAttributes  = "--remove-notappl-atts";
    const QString RemoveDefaultAttributes     = "--remove-default-atts";
    const QString RemoveInkscapeAttributes    = "--remove-inkscape-atts";
    const QString RemoveSodipodiAttributes    = "--remove-sodipodi-atts";
    const QString RemoveAdobeAttributes       = "--remove-ai-atts";
    const QString RemoveCorelDrawAttributes   = "--remove-corel-atts";
    const QString RemoveMSVisioAttributes     = "--remove-msvisio-atts";
    const QString RemoveSketchAttributes      = "--remove-sketch-atts";
    const QString RemoveStrokeProps           = "--remove-stroke-props";
    const QString RemoveFillProps             = "--remove-fill-props";
    const QString RemoveUnusedXLinks          = "--remove-unused-xlinks";
    const QString GroupElemByStyle            = "--group-elts-by-styles";
    const QString JoinStyleAttributes         = "--join-style-atts";
    const QString SimplifyTransformMatrix     = "--simplify-transform-matrix";
    const QString ApplyTransformsToDefs       = "--apply-transforms-to-defs";
    const QString ApplyTransformsToShapes     = "--apply-transforms-to-shapes";

    const QString ConvertToRelative           = "--convert-to-relative";
    const QString RemoveUnneededSymbols       = "--remove-unneeded-symbols";
    const QString RemoveTinySegments          = "--remove-tiny-segments";
    const QString ConvertSegments             = "--convert-segments";
    const QString ApplyTransformsToPaths      = "--apply-transforms-to-paths";

    const QString CreateViewbox               = "--create-viewbox";
    const QString ConvertColorToRRGGBB        = "--colors-to-rrggbb";
    const QString ConvertRRGGBBToRGB          = "--rrggbb-to-rgb";
    const QString ConvertBasicShapes          = "--convert-basic-shapes";
    const QString TransformPrecision          = "--transform-precision";
    const QString CoordsPrecision             = "--coordinates-precision";
    const QString AttributesPrecision         = "--attributes-precision";
    const QString CompactOutput               = "--compact-output";
    const QString SortDefs                    = "--sort-defs";

    const QString ShortOutput                 = "--short-output";
}

Keys::Keys(QObject *parent) : QObject(parent)
{
    flags = new QSet<int>;
    setPreset(Preset::Complete);
}

bool Keys::flag(const int &key) const
{
    return flags->contains(key);
}

int Keys::intNumber(const int &key) const
{
    return numHash.value(key);
}

double Keys::doubleNumber(const int &key) const
{
    return numHash.value(key);
}

int Keys::coordinatesPrecision() const
{
    return m_coordinatesPrecision;
}

int Keys::attributesPrecision() const
{
    return m_attributesPrecision;
}

int Keys::transformPrecision() const
{
    return m_transformPrecision;
}

void Keys::prepareDescription()
{
    descHash.insert(Key::RemoveProlog,
                    tr("Remove XML prolog"));
    descHash.insert(Key::RemoveComments,
                    tr("Remove XML comments"));
    descHash.insert(Key::RemoveProcInstruction,
                    tr("Remove XML processing instruction"));
    descHash.insert(Key::RemoveUnusedDefs,
                    tr("Remove unused elements in 'defs' element"));
    descHash.insert(Key::RemoveNonSvgElements,
                    tr("Remove non SVG elements"));
    descHash.insert(Key::RemoveMetadata,
                    tr("Remove metadata elements"));
    descHash.insert(Key::RemoveInkscapeElements,
                    tr("Remove Inkscape namespaced elements"));
    descHash.insert(Key::RemoveSodipodiElements,
                    tr("Remove SodiPodi namespaced elements"));
    descHash.insert(Key::RemoveAdobeElements,
                    tr("Remove Adobe Illustrator namespaced elements"));
    descHash.insert(Key::RemoveCorelDrawElements,
                    tr("Remove CorelDRAW namespaced elements"));
    descHash.insert(Key::RemoveMSVisioElements,
                    tr("Remove MS Visio namespaced elements"));
    descHash.insert(Key::RemoveSketchElements,
                    tr("Remove Sketch namespaced elements"));
    descHash.insert(Key::RemoveInvisibleElements,
                    tr("Remove invisible elements"));
    descHash.insert(Key::RemoveEmptyContainers,
                    tr("Remove empty containers elements"));
    descHash.insert(Key::RemoveOutsideElements,
                    tr("Remove elements which is outside the viewbox"));
    descHash.insert(Key::ReplaceEqualEltsByUse,
                    tr("Replace equal elements by the 'use'"));
    descHash.insert(Key::UngroupContainers,
                    tr("Ungroup container elements, when possible"));
    descHash.insert(Key::RemoveDuplicatedDefs,
                    tr("Remove duplicate elements in the 'defs' element"));
    descHash.insert(Key::MergeGradients,
                    tr("Merge 'linearGradient' into 'radialGradient', when possible"));
    descHash.insert(Key::RemoveTinyGaussianBlur,
                    tr("Remove Gaussian blur filters with deviation lower than"));

    descHash.insert(Key::RemoveSvgVersion,
                    tr("Remove SVG version"));
    descHash.insert(Key::RemoveUnreferencedIds,
                    tr("Remove unreferenced id's"));
    descHash.insert(Key::TrimIds,
                    tr("Trim 'id' attributes"));
    descHash.insert(Key::KeepNamedIds,
                    tr("Keep unreferenced id's which contains only letters"));
    descHash.insert(Key::RemoveNotAppliedAttributes,
                    tr("Remove not applied attributes"));
    descHash.insert(Key::RemoveDefaultAttributes,
                    tr("Remove attributes with default values"));
    descHash.insert(Key::RemoveInkscapeAttributes,
                    tr("Remove Inkscape namespaced attributes"));
    descHash.insert(Key::RemoveSodipodiAttributes,
                    tr("Remove SodiPodi namespaced attributes"));
    descHash.insert(Key::RemoveAdobeAttributes,
                    tr("Remove Adobe Illustrator namespaced attributes"));
    descHash.insert(Key::RemoveCorelDrawAttributes,
                    tr("Remove CorelDRAW namespaced attributes"));
    descHash.insert(Key::RemoveMSVisioAttributes,
                    tr("Remove MS Visio namespaced attributes"));
    descHash.insert(Key::RemoveSketchAttributes,
                    tr("Remove Sketch namespaced attributes"));
    descHash.insert(Key::RemoveStrokeProps,
                    tr("Remove stroke properties when no stroking"));
    descHash.insert(Key::RemoveFillProps,
                    tr("Remove fill properties when no filling"));
    descHash.insert(Key::RemoveUnusedXLinks,
                    tr("Remove XLinks which pointed to nonexistent elements"));
    descHash.insert(Key::GroupElemByStyle,
                    tr("Group elements by style properties"));
    descHash.insert(Key::JoinStyleAttributes,
                    tr("Merge style properties into 'style' attribute"));
    descHash.insert(Key::SimplifyTransformMatrix,
                    tr("Simplify transform matrices into short equivalent, when possible"));
    descHash.insert(Key::ApplyTransformsToDefs,
                    tr("Apply transform matrices to gradients, when possible"));
    descHash.insert(Key::ApplyTransformsToShapes,
                    tr("Apply transform matrices to basic shapes, when possible"));

    descHash.insert(Key::ConvertToRelative,
                    tr("Convert absolute coordinates into relative ones"));
    descHash.insert(Key::RemoveUnneededSymbols,
                    tr("Remove unneeded symbols in 'd' attribute"));
    descHash.insert(Key::RemoveTinySegments,
                    tr("Remove tiny or empty segments"));
    descHash.insert(Key::ConvertSegments,
                    tr("Convert segments into shorter equivalent, when possible"));
    descHash.insert(Key::ApplyTransformsToPaths,
                    tr("Apply transform matrices, when possible"));

    descHash.insert(Key::CreateViewbox,
                    tr("Convert 'height' and 'width' attributes into 'viewBox' attribute"));
    descHash.insert(Key::ConvertColorToRRGGBB,
                    tr("Convert colors into #RRGGBB format"));
    descHash.insert(Key::ConvertRRGGBBToRGB,
                    tr("Convert #RRGGBB colors into #RGB format, when possible"));
    descHash.insert(Key::ConvertBasicShapes,
                    tr("Convert 'polygon', 'polyline', 'line', 'rect' into paths"));
    descHash.insert(Key::TransformPrecision,
                    tr("Set rounding precision for transformations"));
    descHash.insert(Key::CoordsPrecision,
                    tr("Set rounding precision for coordinates"));
    descHash.insert(Key::AttributesPrecision,
                    tr("Set rounding precision for attributes"));
    descHash.insert(Key::CompactOutput,
                    tr("Save file with only required whitespace and newlines"));
    descHash.insert(Key::SortDefs,
                    tr("Sort elements by name in 'defs' element"));
}

QString Keys::description(const int &key)
{
    return descHash.value(key);
}

QList<int> Keys::elementsKeysId()
{
    static QList<int> list = QList<int>()
        << Key::RemoveProlog
        << Key::RemoveComments
        << Key::RemoveProcInstruction
        << Key::RemoveUnusedDefs
        << Key::RemoveNonSvgElements
        << Key::RemoveMetadata
        << Key::RemoveInkscapeElements
        << Key::RemoveSodipodiElements
        << Key::RemoveAdobeElements
        << Key::RemoveCorelDrawElements
        << Key::RemoveMSVisioElements
        << Key::RemoveSketchElements
        << Key::RemoveInvisibleElements
        << Key::RemoveEmptyContainers
        << Key::RemoveDuplicatedDefs
        << Key::RemoveOutsideElements
        << Key::ReplaceEqualEltsByUse
        << Key::UngroupContainers
        << Key::MergeGradients
        << Key::RemoveTinyGaussianBlur;
    return list;
}

QList<int> Keys::attributesKeysId()
{
    static QList<int> list = QList<int>()
        << Key::RemoveSvgVersion
        << Key::RemoveUnreferencedIds
        << Key::TrimIds
        << Key::RemoveNotAppliedAttributes
        << Key::RemoveDefaultAttributes
        << Key::RemoveInkscapeAttributes
        << Key::RemoveSodipodiAttributes
        << Key::RemoveAdobeAttributes
        << Key::RemoveCorelDrawAttributes
        << Key::RemoveMSVisioAttributes
        << Key::RemoveSketchAttributes
        << Key::RemoveStrokeProps
        << Key::RemoveFillProps
        << Key::RemoveUnusedXLinks
        << Key::GroupElemByStyle
        << Key::SimplifyTransformMatrix
        << Key::ApplyTransformsToDefs
        << Key::ApplyTransformsToShapes;
    return list;
}

QList<int> Keys::attributesUtilsKeysId()
{
    static QList<int> list = QList<int>()
        << Key::JoinStyleAttributes
        << Key::KeepNamedIds;
    return list;
}

QList<int> Keys::pathsKeysId()
{
    static QList<int> list = QList<int>()
        << Key::ConvertToRelative
        << Key::RemoveUnneededSymbols
        << Key::RemoveTinySegments
        << Key::ConvertSegments
        << Key::ApplyTransformsToPaths;
    return list;
}

QList<int> Keys::optimizationsKeysId()
{
    static QList<int> list = QList<int>()
        << Key::CreateViewbox
        << Key::ConvertColorToRRGGBB
        << Key::ConvertRRGGBBToRGB
        << Key::ConvertBasicShapes
        << Key::ApplyTransformsToDefs
        << Key::CompactOutput
        << Key::TransformPrecision
        << Key::CoordsPrecision
        << Key::AttributesPrecision;
    return list;
}

QList<int> Keys::optimizationsUtilsKeysId()
{
    static QList<int> list = QList<int>()
        << Key::SortDefs;
    return list;
}

QList<int> Keys::basicPresetKeys()
{
    static QList<int> list = QList<int>()
        << Key::RemoveComments
        << Key::RemoveNonSvgElements
        << Key::RemoveInvisibleElements
        << Key::RemoveEmptyContainers
        << Key::RemoveDuplicatedDefs
        << Key::RemoveNotAppliedAttributes
        << Key::RemoveDefaultAttributes
        << Key::RemoveUnusedXLinks
        << Key::ConvertColorToRRGGBB;
    return list;
}

QList<int> Keys::completePresetKeys()
{
    static QList<int> list = QList<int>()
        << elementsKeysId()
        << attributesKeysId()
        << Key::ConvertToRelative
        << Key::RemoveUnneededSymbols
        << Key::RemoveTinySegments
        << Key::ConvertSegments
        << optimizationsKeysId()
        << Key::SortDefs;
    return list;
}

QList<int> Keys::extremePresetKeys()
{
    static QList<int> list = QList<int>()
        << completePresetKeys()
        << Key::ApplyTransformsToPaths;
    return list;
}

QStringList &Keys::allKeys()
{
    // must be in the same order as in Key namespace
    static QStringList allKeys = QStringList()
        << KeyStr::RemoveProlog
        << KeyStr::RemoveComments
        << KeyStr::RemoveProcInstruction
        << KeyStr::RemoveUnusedDefs
        << KeyStr::RemoveNonSvgElements
        << KeyStr::RemoveMetadata
        << KeyStr::RemoveInkscapeElements
        << KeyStr::RemoveSodipodiElements
        << KeyStr::RemoveAdobeElements
        << KeyStr::RemoveCorelDrawElements
        << KeyStr::RemoveMSVisioElements
        << KeyStr::RemoveSketchElements
        << KeyStr::RemoveInvisibleElements
        << KeyStr::RemoveEmptyContainers
        << KeyStr::RemoveDuplicatedDefs
        << KeyStr::RemoveOutsideElements
        << KeyStr::ReplaceEqualEltsByUse
        << KeyStr::UngroupContainers
        << KeyStr::MergeGradients
        << KeyStr::RemoveTinyGaussianBlur

        << KeyStr::RemoveSvgVersion
        << KeyStr::RemoveUnreferencedIds
        << KeyStr::TrimIds
        << KeyStr::RemoveNotAppliedAttributes
        << KeyStr::RemoveDefaultAttributes
        << KeyStr::RemoveInkscapeAttributes
        << KeyStr::RemoveSodipodiAttributes
        << KeyStr::RemoveAdobeAttributes
        << KeyStr::RemoveCorelDrawAttributes
        << KeyStr::RemoveMSVisioAttributes
        << KeyStr::RemoveSketchAttributes
        << KeyStr::RemoveStrokeProps
        << KeyStr::RemoveFillProps
        << KeyStr::RemoveUnusedXLinks
        << KeyStr::GroupElemByStyle
        << KeyStr::SimplifyTransformMatrix
        << KeyStr::ApplyTransformsToDefs
        << KeyStr::ApplyTransformsToShapes

        << KeyStr::JoinStyleAttributes
        << KeyStr::KeepNamedIds

        << KeyStr::ConvertToRelative
        << KeyStr::RemoveUnneededSymbols
        << KeyStr::RemoveTinySegments
        << KeyStr::ConvertSegments
        << KeyStr::ApplyTransformsToPaths

        << KeyStr::CreateViewbox
        << KeyStr::ConvertColorToRRGGBB
        << KeyStr::ConvertRRGGBBToRGB
        << KeyStr::ConvertBasicShapes
        << KeyStr::TransformPrecision
        << KeyStr::CoordsPrecision
        << KeyStr::AttributesPrecision
        << KeyStr::CompactOutput

        << KeyStr::SortDefs

        << KeyStr::ShortOutput;
    return allKeys;
}

QString Keys::keyName(const int &keyId)
{
    return allKeys().at(keyId);
}

QString Keys::presetDescription(const QString &name)
{
    if (name == Preset::Basic) {
        return tr("<b>Basic</b> preset are designed to remove all unnecessary data from SVG file, "
                  "without changing it structure. "
                  "Allows you to continue editing of the file.<br>"
                  "Сan not damage your files. Otherwise, please send this files to our email.");
    } else if (name == Preset::Complete) {
        return tr("<b>Complete</b> preset are designed to create a file which will be used only for showing. "
                  "This preset completely change file structure, what in most cases prevents future editing.<br>"
                  "Should not damage your files. Otherwise, please send this files to our email.");
    } else if (name == Preset::Extreme) {
        return tr("<b>Extreme</b> preset does the same that <b>Complete</b> do, but also enables some unstable features.<br>"
                  "It will definitely change displaying of your file and could even damage it.");
    } else if (name == Preset::Custom) {
        return tr("<b>Custom</b> preset is used to store your own cleaning options. "
                  "By default all options are off.");
    }
    return "";
}

void Keys::setPreset(const QString &name)
{
    flags->clear();

    m_preset = name;
    if (name == Preset::Basic) {
        foreach (const int &key, basicPresetKeys())
            flags->insert(key);
        numHash.insert(Key::RemoveTinyGaussianBlur, 0.0);
        numHash.insert(Key::TransformPrecision, 8);
        numHash.insert(Key::CoordsPrecision, 6);
        numHash.insert(Key::AttributesPrecision, 6);
    } else if (name == Preset::Complete) {
        foreach (const int &key, completePresetKeys())
            flags->insert(key);
        numHash.insert(Key::RemoveTinyGaussianBlur, 0.1);
        numHash.insert(Key::TransformPrecision, 5);
        numHash.insert(Key::CoordsPrecision, 3);
        numHash.insert(Key::AttributesPrecision, 3);
    } else if (name == Preset::Extreme) {
        foreach (const int &key, extremePresetKeys())
            flags->insert(key);
        numHash.insert(Key::RemoveTinyGaussianBlur, 0.2);
        numHash.insert(Key::TransformPrecision, 3);
        numHash.insert(Key::CoordsPrecision, 2);
        numHash.insert(Key::AttributesPrecision, 2);
    } else if (name == Preset::Custom) {
        numHash.insert(Key::RemoveTinyGaussianBlur, 0.0);
        numHash.insert(Key::TransformPrecision, 8);
        numHash.insert(Key::CoordsPrecision, 8);
        numHash.insert(Key::AttributesPrecision, 8);
    } else {
        qFatal("Error: wrong preset name");
    }
}

QString Keys::preset()
{
    return m_preset;
}

void Keys::parseOptions(QStringList &list)
{
    m_transformPrecision   = intNumber(Key::TransformPrecision);
    m_attributesPrecision  = intNumber(Key::AttributesPrecision);
    m_coordinatesPrecision = intNumber(Key::CoordsPrecision);

    foreach (const QString &key, list) {
        if (key.isEmpty() || key.count(' ') != 0)
            list.removeOne(key);
    }
    if (list.isEmpty())
        return;

    if (list.first().startsWith(QLatin1String("--preset"))) {
        QString preset = list.takeFirst();
        preset.remove(QLatin1String("--preset="));
        if (preset == Preset::Complete)
            setPreset(Preset::Complete);
        else if (preset == Preset::Basic)
            setPreset(Preset::Basic);
        else if (preset == Preset::Extreme)
            setPreset(Preset::Extreme);
        else if (preset == Preset::Custom)
            setPreset(Preset::Custom);
    } else {
        setPreset(Preset::Custom);
    }

    if (list.isEmpty())
        return;

    foreach (QString flag, list) {
        bool isError = false;
        QString value;
        if (flag.contains(QLatin1Char('='))) {
            QStringList tmpList = flag.split(QLatin1Char('='));
            if (tmpList.size() != 2) {
                isError = true;
            } else {
                flag  = tmpList.first();
                value = tmpList.last();
                bool ok = false;
                value.toDouble(&ok);
                if (!ok)
                    isError = true;
            }
        }
        int index = allKeys().indexOf(flag);
        if (index != -1 && !isError) {
            if (   flag == KeyStr::TransformPrecision
                || flag == KeyStr::AttributesPrecision
                || flag == KeyStr::CoordsPrecision)
            {
                if (value.toInt() > 0 && value.toInt() <= 8) {
                    if (flag == KeyStr::TransformPrecision) {
                        m_transformPrecision = value.toInt();
                        numHash.insert(Key::TransformPrecision, value.toInt());
                    } else if (flag == KeyStr::AttributesPrecision) {
                        m_attributesPrecision = value.toInt();
                        numHash.insert(Key::AttributesPrecision, value.toInt());
                    } else if (flag == KeyStr::CoordsPrecision) {
                        m_coordinatesPrecision = value.toInt();
                        numHash.insert(Key::CoordsPrecision, value.toInt());
                    }
                } else {
                    isError = true;
                }
            } else if (flag == KeyStr::RemoveTinyGaussianBlur) {
                if (value.toDouble() >= 0 && value.toDouble() <= 1.0) {
                    numHash.insert(Key::CoordsPrecision, value.toDouble());
                } else
                    isError = true;
            } else {
                flags->insert(index);
            }
        } else {
            qFatal("Error: SVG Cleaner does not support option: %s", qPrintable(flag));
        }
        if (isError)
            qFatal("Error: wrong value for option: %s", qPrintable(flag));
    }
}
