/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2015 Evgeniy Reizner
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

class Replacer : public BaseCleaner
{
public:
    explicit Replacer(SvgDocument &doc) : BaseCleaner(doc) {}
    void convertSizeToViewbox() const;
    void processPaths() const;
    void fixWrongAttr() const;
    void convertUnits() const;
    void convertColors() const;
    void convertEntityData() const;
    void convertCDATAStyle() const;
    void prepareDefs() const;
    void splitStyleAttributes() const;
    void joinStyleAttr() const;
    void sortDefs() const;
    void roundNumericAttributes() const;
    void prepareLinkedStyles() const;
    void convertBasicShapes() const;
    void mergeGradients() const;
    void finalFixes() const;
    void trimIds() const;
    void groupTextElementsStyles() const;
    void groupElementsByStyles() const;
    void applyTransformToDefs() const;
    void applyTransformToShapes() const;
    void mergeGradientsWithEqualStopElem() const;
    void calcElementsBoundingBox() const;
    void replaceEqualElementsByUse() const;
    void moveStyleFromUsedElemToUse() const;

private:
    bool isPathValidToTransform(SvgElement &pathElem) const;
    static bool isBlurFilter(const SvgElement &elem);
    void updateLinkedDefTransform(SvgElement &elem) const;
    static bool nodeByTagNameSort(const SvgElement &node1, const SvgElement &node2);
    static void calcNewStrokeWidth(SvgElement &elem, double scaleFactor);
    void _groupElementsByStyles(SvgElement parentElem = SvgElement()) const;
};

#endif // REPLACER_H
