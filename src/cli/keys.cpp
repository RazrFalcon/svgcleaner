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

using namespace Key;

Keys::Keys(QObject *parent) : QObject(parent)
{
    setPreset(Preset::Complete);
}

bool Keys::flag(const QString &key) const
{
    return hash.value(key).toBool();
}

int Keys::intNumber(const QString &key) const
{
    return hash.value(key).toInt();
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

double Keys::doubleNumber(const QString &key) const
{
    return hash.value(key).toDouble();
}

void Keys::parseOptions(QStringList &list)
{
    if (list.isEmpty()) {
        m_transformPrecision   = intNumber(Key::TransformPrecision);
        m_attributesPrecision  = intNumber(Key::AttributesPrecision);
        m_coordinatesPrecision = intNumber(Key::CoordsPrecision);
        return;
    }

    if (list.first().startsWith("--preset")) {
        QString preset = list.takeFirst();
        preset.remove("--preset=");
        if (preset == Preset::Complete)
            setPreset(Preset::Complete);
        else if (preset == Preset::Basic)
            setPreset(Preset::Basic);
        else if (preset == Preset::Extreme)
            setPreset(Preset::Extreme);
    }

    m_transformPrecision   = intNumber(Key::TransformPrecision);
    m_attributesPrecision  = intNumber(Key::AttributesPrecision);
    m_coordinatesPrecision = intNumber(Key::CoordsPrecision);

    if (list.isEmpty())
        return;

    QStringList keys = hash.keys();
    foreach (QString flag, list) {
        bool isError = false;
        QString value;
        if (flag.contains("=")) {
            QStringList tmpList = flag.split("=");
            Q_ASSERT(tmpList.count() == 2);
            flag  = tmpList.first();
            value = tmpList.last();
            if (!value.contains(QRegExp("[0-9\\.\\-]")))
                isError = true;
        }
        if (keys.contains(flag) && !isError) {
            if (   flag == Key::TransformPrecision
                || flag == Key::AttributesPrecision
                || flag == Key::CoordsPrecision)
            {
                if (value.toInt() > 0 && value.toInt() <= 8) {
                    hash.insert(flag, value.toInt());
                    if (flag == Key::TransformPrecision)
                        m_transformPrecision = value.toInt();
                    else if (flag == Key::AttributesPrecision)
                        m_attributesPrecision = value.toInt();
                    else if (flag == Key::CoordsPrecision)
                        m_coordinatesPrecision = value.toInt();
                } else {
                    isError = true;
                }
            } else if (flag == Key::RemoveTinyGaussianBlur) {
                if (value.toDouble() >= 0.01 && value.toDouble() <= 1.0)
                    hash.insert(flag, value.toDouble());
                else
                    isError = true;
            } else {
                hash.insert(flag, true);
            }
        } else {
            qFatal("Error: SVG Cleaner does not support option: %s", qPrintable(flag));
        }
        if (isError)
            qFatal("Error: wrong value for option: %s", qPrintable(flag));
    }
}

void Keys::prepareDescription()
{
    descHash.insert(RemoveProlog,
                    tr("Remove xml prolog"));
    descHash.insert(RemoveComments,
                    tr("Remove xml comments"));
    descHash.insert(RemoveProcInstruction,
                    tr("Remove xml processing instruction"));
    descHash.insert(RemoveUnusedDefs,
                    tr("Remove unused elements in 'defs' element"));
    descHash.insert(RemoveNonSvgElements,
                    tr("Remove non svg elements"));
    descHash.insert(RemoveMetadata,
                    tr("Remove metadata elements"));
    descHash.insert(RemoveInkscapeElements,
                    tr("Remove Inkscape namespaced elements"));
    descHash.insert(RemoveSodipodiElements,
                    tr("Remove SodiPodi namespaced elements"));
    descHash.insert(RemoveAdobeElements,
                    tr("Remove Adobe Illustrator namespaced elements"));
    descHash.insert(RemoveCorelDrawElements,
                    tr("Remove CorelDRAW namespaced elements"));
    descHash.insert(RemoveMSVisioElements,
                    tr("Remove MS Visio namespaced elements"));
    descHash.insert(RemoveInvisibleElements,
                    tr("Remove invisible elements"));
    descHash.insert(RemoveEmptyContainers,
                    tr("Remove empty containers elements"));
    descHash.insert(UngroupGroups,
                    tr("Ungroup 'group' elements, when possible"));
    descHash.insert(RemoveDuplicatedDefs,
                    tr("Remove duplicate elements in 'defs' element"));
    descHash.insert(MergeGradients,
                    tr("Merge 'linearGradient' into 'radialGradient', when possible"));
    descHash.insert(RemoveTinyGaussianBlur,
                    tr("Remove Gaussian blur filters with deviation lower than:"));

    descHash.insert(RemoveSvgVersion,
                    tr("Remove SVG version"));
    descHash.insert(RemoveUnreferencedIds,
                    tr("Remove unreferenced id's"));
    descHash.insert(KeepNamedIds,
                    tr("Keep unreferenced id's which contains only letters"));
    descHash.insert(RemoveNotAppliedAttributes,
                    tr("Remove not applied attributes, like 'font-size' for 'rect' element"));
    descHash.insert(RemoveDefaultAttributes,
                    tr("Remove attributes with default values"));
    descHash.insert(RemoveInkscapeAttributes,
                    tr("Remove Inkscape namespaced attributes"));
    descHash.insert(RemoveSodipodiAttributes,
                    tr("Remove SodiPodi namespaced attributes"));
    descHash.insert(RemoveAdobeAttributes,
                    tr("Remove Adobe Illustrator namespaced attributes"));
    descHash.insert(RemoveCorelDrawAttributes,
                    tr("Remove CorelDRAW namespaced attributes"));
    descHash.insert(RemoveMSVisioAttributes,
                    tr("Remove MS Visio namespaced attributes"));
    descHash.insert(RemoveStrokeProps,
                    tr("Remove stroke properties when no stroking"));
    descHash.insert(RemoveFillProps,
                    tr("Remove fill properties when no filling"));
    descHash.insert(RemoveGradientCoordinates,
                    tr("Remove unneeded gradient attributes"));
    descHash.insert(RemoveUnusedXLinks,
                    tr("Remove XLinks which pointed to nonexistent elements"));
    descHash.insert(GroupElemByStyle,
                    tr("Group elements by style properties"));
    descHash.insert(TrimIds,
                    tr("Trim 'id' attributes into hexadecimal format"));
    descHash.insert(JoinStyleAttributes,
                    tr("Merge style properties into 'style' attribute"));

    descHash.insert(ConvertToRelative,
                    tr("Convert absolute coordinates into relative ones"));
    descHash.insert(RemoveUnneededSymbols,
                    tr("Remove unneeded symbols in 'd' attribute"));
    descHash.insert(RemoveTinySegments,
                    tr("Remove tiny or empty segments"));
    descHash.insert(ConvertCurves,
                    tr("Convert cubic Bezier curves to shorthand, when possible"));

    descHash.insert(CreateViewbox,
                    tr("Convert 'height' and 'width' attributes into 'viewBox' attribute"));
    descHash.insert(ConvertColorToRRGGBB,
                    tr("Convert colors into #RRGGBB format"));
    descHash.insert(ConvertRRGGBBToRGB,
                    tr("Convert #RRGGBB colors into #RGB format, when possible"));
    descHash.insert(ConvertBasicShapes,
                    tr("Convert polygon, polyline, line, rect into paths"));
    descHash.insert(ApplyTransforms,
                    tr("Apply transform matrices"));
    descHash.insert(TransformPrecision,
                    tr("Set rounding precision for transformations"));
    descHash.insert(CoordsPrecision,
                    tr("Set rounding precision for coordinates"));
    descHash.insert(AttributesPrecision,
                    tr("Set rounding precision for attributes"));
    descHash.insert(CompactOutput,
                    tr("Save svg with only required whitespace and newlines"));
    descHash.insert(SortDefs,
                    tr("Sort elements by name in 'defs' element"));
}

QString Keys::description(const QString &key)
{
    return descHash.value(key);
}

QStringList Keys::elementsKeys()
{
    static QStringList list = QStringList()
        << RemoveProlog
        << RemoveComments
        << RemoveProcInstruction
        << RemoveUnusedDefs
        << RemoveNonSvgElements
        << RemoveMetadata
        << RemoveInkscapeElements
        << RemoveSodipodiElements
        << RemoveAdobeElements
        << RemoveCorelDrawElements
        << RemoveMSVisioElements
        << RemoveInvisibleElements
        << RemoveEmptyContainers
        << UngroupGroups
        << RemoveDuplicatedDefs
        << MergeGradients
        << RemoveTinyGaussianBlur;
    return list;
}

QStringList Keys::attributesKeys()
{
    static QStringList list = QStringList()
        << RemoveSvgVersion
        << RemoveUnreferencedIds
        << RemoveNotAppliedAttributes
        << RemoveDefaultAttributes
        << RemoveInkscapeAttributes
        << RemoveSodipodiAttributes
        << RemoveAdobeAttributes
        << RemoveCorelDrawAttributes
        << RemoveMSVisioAttributes
        << RemoveStrokeProps
        << RemoveFillProps
        << RemoveGradientCoordinates
        << RemoveUnusedXLinks
        << GroupElemByStyle
        << TrimIds;
    return list;
}

QStringList Keys::attributesUtilsKeys()
{
    static QStringList list = QStringList()
        << JoinStyleAttributes
        << KeepNamedIds;
    return list;
}

QStringList Keys::pathsKeys()
{
    static QStringList list = QStringList()
        << ConvertToRelative
        << RemoveUnneededSymbols
        << RemoveTinySegments
        << ConvertCurves;
    return list;
}

QStringList Keys::optimizationsKeys()
{
    static QStringList list = QStringList()
        << CreateViewbox
        << ConvertColorToRRGGBB
        << ConvertRRGGBBToRGB
        << ConvertBasicShapes
        << ApplyTransforms
        << CompactOutput
        << TransformPrecision
        << CoordsPrecision
        << AttributesPrecision;
    return list;
}

QStringList Keys::optimizationsUtilsKeys()
{
    static QStringList list = QStringList() << SortDefs;
    return list;
}

QStringList Keys::basicPresetKeys()
{
    static QStringList list = QStringList()
        << RemoveComments
        << RemoveNonSvgElements
        << RemoveEmptyContainers
        << RemoveDuplicatedDefs
        << RemoveNotAppliedAttributes
        << RemoveDefaultAttributes
        << RemoveUnusedXLinks
        << RemoveTinySegments
        << ConvertColorToRRGGBB;
    return list;
}

QStringList Keys::completePresetKeys()
{
    static QStringList list = QStringList()
        << elementsKeys()
        << attributesKeys()
        << pathsKeys()
        << optimizationsKeys();
    return list;
}

QStringList Keys::extremePresetKeys()
{
    static QStringList list = QStringList()
        << completePresetKeys();
    return list;
}

void Keys::setPreset(const QString &name)
{
    // set default
    static QStringList allKeys = QStringList()
        << elementsKeys()
        << attributesKeys()
        << attributesUtilsKeys()
        << pathsKeys()
        << optimizationsKeys()
        << optimizationsUtilsKeys()
        << ShortOutput;
    foreach (const QString &key, allKeys)
        hash.insert(key, false);
    hash.insert(RemoveTinyGaussianBlur, 0.0);
    hash.insert(TransformPrecision, 8);
    hash.insert(CoordsPrecision, 8);
    hash.insert(AttributesPrecision, 8);


    if (name == Preset::Basic) {
        foreach (const QString &key, basicPresetKeys())
            hash.insert(key, true);
        hash.insert(RemoveTinyGaussianBlur, 0.0);
        hash.insert(TransformPrecision, 8);
        hash.insert(CoordsPrecision, 6);
        hash.insert(AttributesPrecision, 6);
    } else if (name == Preset::Complete) {
        foreach (const QString &key, completePresetKeys())
            hash.insert(key, true);
        hash.insert(RemoveTinyGaussianBlur, 0.1);
        hash.insert(TransformPrecision, 5);
        hash.insert(CoordsPrecision, 3);
        hash.insert(AttributesPrecision, 3);
    } else if (name == Preset::Extreme) {
        foreach (const QString &key, extremePresetKeys())
            hash.insert(key, true);
        hash.insert(RemoveTinyGaussianBlur, 0.2);
        hash.insert(TransformPrecision, 3);
        hash.insert(CoordsPrecision, 1);
        hash.insert(AttributesPrecision, 1);
    } else {
        qDebug("Error: wrong preset name");
    }
}
