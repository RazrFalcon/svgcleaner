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

#ifndef REMOVER_H
#define REMOVER_H

#include "basecleaner.h"

class Remover : public BaseCleaner
{
public:
    explicit Remover(SvgDocument &doc) : BaseCleaner(doc) {}
    void removeUnreferencedIds() const;
    void removeUnusedDefs() const;
    void removeUnusedDefsAttributes() const;
    void removeDuplicatedDefs() const;
    void removeElements() const;
    void removeElementsFinal() const;
    void removeNonElementAttributes() const;
    void cleanSvgElementAttribute() const;
    void checkXlinkDeclaration() const;
    void removeAttributes() const;
    void cleanPresentationAttributes() const;
    void removeGroups() const;
    void ungroupSwitchElement() const;
    void ungroupAElement() const;
    void removeElementsOutsideTheViewbox() const;

private:
    void detectEqualLinearGradients(SvgElement &elem1) const;
    void detectEqualRadialGradients(SvgElement &elem1) const;
    void detectEqualFilters(SvgElement &elem1) const;
    void detectEqualClipPaths(SvgElement &elem1) const;
    void cleanStyle(SvgElement &elem) const;
    static bool isElementInvisible(SvgElement &elem);
    static bool isElementInvisible2(SvgElement &elem);
    void megreGroupWithChild(SvgElement &groupElem, SvgElement &childElem,
                             bool isParentToChild) const;
    static bool isDoctype(const QString &str);
    void prepareViewBoxRect(QRectF &viewBox) const;
};

#endif // REMOVER_H
