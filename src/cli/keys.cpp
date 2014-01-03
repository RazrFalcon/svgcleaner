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

#include <QtDebug>

#include "keys.h"

Keys::Keys()
{
    keyHash.insert(Key::KeepProlog, false);
    keyHash.insert(Key::KeepComments, false);
    keyHash.insert(Key::KeepProcessingInstruction, false);
    keyHash.insert(Key::KeepUnusedDefs, false);
    keyHash.insert(Key::KeepNonSvgElements, false);
    keyHash.insert(Key::KeepMetadata, false);
    keyHash.insert(Key::KeepInkscapeElements, false);
    keyHash.insert(Key::KeepSodipodiElements, false);
    keyHash.insert(Key::KeepIllustratorElements, false);
    keyHash.insert(Key::KeepCorelDrawElements, false);
    keyHash.insert(Key::KeepMSVisioElements, false);
    keyHash.insert(Key::KeepInvisibleElements, false);
    keyHash.insert(Key::KeepEmptyContainer, false);
    keyHash.insert(Key::KeepGroups, false);
    keyHash.insert(Key::KeepDuplicatedDefs, false);
    keyHash.insert(Key::KeepSinglyGradients, false);
    keyHash.insert(Key::KeepTinyGaussianBlur, false);
    keyHash.insert(Key::StdDeviation, 0.1);

    keyHash.insert(Key::KeepSvgVersion, false);
    keyHash.insert(Key::KeepUnreferencedIds, false);
    keyHash.insert(Key::KeepNamedIds, false);
    keyHash.insert(Key::KeepNotAppliedAttributes, false);
    keyHash.insert(Key::KeepDefaultAttributes, false);
    keyHash.insert(Key::KeepInkscapeAttributes, false);
    keyHash.insert(Key::KeepSodipodiAttributes, false);
    keyHash.insert(Key::KeepIllustratorAttributes, false);
    keyHash.insert(Key::KeepCorelDrawAttributes, false);
    keyHash.insert(Key::KeepMSVisioAttributes, false);
    keyHash.insert(Key::KeepStrokeProps, false);
    keyHash.insert(Key::KeepFillProps, false);
    keyHash.insert(Key::KeepGradientCoordinates, false);
    keyHash.insert(Key::KeepUnusedXLinks, false);
    keyHash.insert(Key::SkipElemByStyleGroup, false);
    keyHash.insert(Key::SkipIdsTrim, false);
    keyHash.insert(Key::JoinStyleAttributes, false);

    keyHash.insert(Key::KeepAbsolutePaths, false);
    keyHash.insert(Key::KeepUnusedSymbolsFromPath, false);
    keyHash.insert(Key::KeepTinySegments, false);
    keyHash.insert(Key::KeepOriginalCurveTo, false);

    keyHash.insert(Key::NoViewbox, false);
    keyHash.insert(Key::SkipColorToRRGGBB, false);
    keyHash.insert(Key::SkipRRGGBBToRGB, false);
    keyHash.insert(Key::KeepBasicShapes, false);
    keyHash.insert(Key::KeepTransforms, false);
    keyHash.insert(Key::KeepUnsortedDefs, false);
    keyHash.insert(Key::SkipRoundingNumbers, false);
    keyHash.insert(Key::TransformPrecision, 5);
    keyHash.insert(Key::CoordsPrecision, 3);
    keyHash.insert(Key::AttributesPrecision, 3);

    keyHash.insert(Key::NotCompact, false);
}

bool Keys::flag(const QString &key) const
{
    return keyHash.value(key).toBool();
}

int Keys::intNumber(const QString &key) const
{
    return keyHash.value(key).toInt();
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
    return keyHash.value(key).toDouble();
}

void Keys::parseOptions(const QStringList &list)
{
    m_transformPrecision = intNumber(Key::TransformPrecision);
    m_attributesPrecision = intNumber(Key::AttributesPrecision);
    m_coordinatesPrecision = intNumber(Key::CoordsPrecision);

    QStringList keys = keyHash.keys();
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
                    keyHash.insert(flag, value.toInt());
                    if (flag == Key::TransformPrecision)
                        m_transformPrecision = value.toInt();
                    else if (flag == Key::AttributesPrecision)
                        m_attributesPrecision = value.toInt();
                    else if (flag == Key::CoordsPrecision)
                        m_coordinatesPrecision = value.toInt();
                } else {
                    isError = true;
                }
            } else if (flag == Key::StdDeviation) {
                if (value.toDouble() >= 0.01 && value.toDouble() <= 0.5)
                    keyHash.insert(flag, value.toDouble());
                else
                    isError = true;
            } else {
                keyHash.insert(flag, true);
            }
        } else {
            qDebug() << "Error: SVG Cleaner do not support option: " + flag;
            exit(1);
        }
        if (isError) {
            qDebug() << "Error: wrong value for option " + flag;
            exit(1);
        }
    }
}
