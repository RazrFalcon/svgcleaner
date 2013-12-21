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

#ifndef REPLACER_H
#define REPLACER_H

#include "paths.h"
#include "basecleaner.h"

struct LineGradStruct {
    SvgElement elem;
    QString id;
    StringMap attrs;
    QList<StringMap> stopAttrs;
};

class Replacer : public BaseCleaner
{
public:
    explicit Replacer(XMLDocument *doc);
    void convertSizeToViewbox();
    void processPaths();
    void fixWrongAttr();
    void convertUnits();
    void convertCDATAStyle();
    void prepareDefs();
    void splitStyleAttr();
    void sortDefs();
    void roundDefs();
    void convertBasicShapes();
    void mergeGradients();
    void finalFixes();
    void trimIds();
    void calcElemAttrCount(const QString &text);
    void groupElementsByStyles(SvgElement parentElem = SvgElement());
    void markUsedElements();
    void applyTransformToDefs();

private:
    SvgElement findLinearGradient(const QString &id);
    void mergeGradientsWithEqualStopElem();
};

#endif // REPLACER_H
