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

#ifndef REPLACER_H
#define REPLACER_H

#include "transform.h"
#include "basecleaner.h"

struct LineGradStruct {
    SvgElement elem;
    QString id;
    StringHash attrs;
    QList<StringHash> stopAttrs;
};

class Replacer : public BaseCleaner
{
public:
    explicit Replacer(SvgDocument &doc) : BaseCleaner(doc) {}
    void convertSizeToViewbox();
    void processPaths();
    void fixWrongAttr();
    void convertUnits();
    void convertColors();
    void convertEntityData();
    void convertCDATAStyle();
    void prepareDefs();
    void splitStyleAttributes();
    void joinStyleAttr();
    void sortDefs();
    void roundNumericAttributes();
    void convertBasicShapes();
    void mergeGradients();
    void finalFixes();
    void trimIds();
    void calcElemAttrCount(const QString &text);
    void groupTextElementsStyles();
    void groupElementsByStyles();
    void markUsedElements();
    void applyTransformToDefs();
    void applyTransformToShapes();
    void mergeGradientsWithEqualStopElem();
    void calcElementsBoundingBox();
    void replaceEqualElementsByUse();
    void moveStyleFromUsedElemToUse();

private:
    QHash<QString,int> calcDefsUsageCount();
    bool isPathValidToTransform(SvgElement &pathElem, QHash<QString, int> &defsIdHash);
    bool isBlurFilter(const QString &id);
    void updateLinkedDefTransform(SvgElement &elem);
    static bool nodeByTagNameSort(const SvgElement &node1, const SvgElement &node2);
    void calcNewStrokeWidth(SvgElement &elem, const Transform &transform);
    void _groupElementsByStyles(SvgElement parentElem = SvgElement());
};

#endif // REPLACER_H
