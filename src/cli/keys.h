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
#include <QSet>
#include <QObject>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;
typedef QSet<QString> StringSet;
typedef QSet<const char *> CharSet;
typedef QList<const char *> CharList;

namespace Preset {
    extern const QString Basic;
    extern const QString Complete;
    extern const QString Extreme;
    extern const QString Custom;
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
    extern const QString RemoveProlog;
    extern const QString RemoveComments;
    extern const QString RemoveProcInstruction;
    extern const QString RemoveUnusedDefs;
    extern const QString RemoveNonSvgElements;
    extern const QString RemoveMetadata;
    extern const QString RemoveInkscapeElements;
    extern const QString RemoveSodipodiElements;
    extern const QString RemoveAdobeElements;
    extern const QString RemoveCorelDrawElements;
    extern const QString RemoveMSVisioElements;
    extern const QString RemoveSketchElements;
    extern const QString RemoveInvisibleElements;
    extern const QString RemoveEmptyContainers;
    extern const QString RemoveTinyGaussianBlur;
    extern const QString RemoveDuplicatedDefs;
    extern const QString RemoveOutsideElements;
    extern const QString ReplaceEqualEltsByUse;
    extern const QString UngroupContainers;
    extern const QString MergeGradients;

    extern const QString RemoveSvgVersion;
    extern const QString RemoveUnreferencedIds;
    extern const QString TrimIds;
    extern const QString KeepNamedIds;
    extern const QString RemoveNotAppliedAttributes;
    extern const QString RemoveDefaultAttributes;
    extern const QString RemoveInkscapeAttributes;
    extern const QString RemoveSodipodiAttributes;
    extern const QString RemoveAdobeAttributes;
    extern const QString RemoveCorelDrawAttributes;
    extern const QString RemoveMSVisioAttributes;
    extern const QString RemoveSketchAttributes;
    extern const QString RemoveStrokeProps;
    extern const QString RemoveFillProps;
    extern const QString RemoveUnusedXLinks;
    extern const QString GroupElemByStyle;
    extern const QString JoinStyleAttributes;
    extern const QString SimplifyTransformMatrix;
    extern const QString ApplyTransformsToDefs;
    extern const QString ApplyTransformsToShapes;
    // TODO: remove bitmaps, as utility

    extern const QString ConvertToRelative;
    extern const QString RemoveUnneededSymbols;
    extern const QString RemoveTinySegments;
    extern const QString ConvertSegments;
    extern const QString ApplyTransformsToPaths;

    extern const QString CreateViewbox;
    extern const QString ConvertColorToRRGGBB;
    extern const QString ConvertRRGGBBToRGB;
    extern const QString ConvertBasicShapes;
    extern const QString TransformPrecision;
    extern const QString CoordsPrecision;
    extern const QString AttributesPrecision;
    extern const QString CompactOutput;
    extern const QString SortDefs;
    // TODO: convert all shapes to path, as utility

    extern const QString ShortOutput;
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
    QList<int> optimizationsKeysId();
    QList<int> optimizationsUtilsKeysId();
    QList<int> basicPresetKeys();
    QList<int> completePresetKeys();
    QList<int> extremePresetKeys();
    void setPreset(const QString &name);
    QString preset();
    QStringList& allKeys();
    QString keyName(const int &keyId);
    QString presetDescription(const QString &name);

private:
    QHash<int, double> numHash;
    QSet<int> *flags;
    QHash<int, QString> descHash;
    int m_coordinatesPrecision;
    int m_attributesPrecision;
    int m_transformPrecision;
    QString m_preset;

    Keys(QObject *parent = 0);
    Keys(Keys const&);
    void operator=(Keys const&);
};

#endif // KEYS_H
