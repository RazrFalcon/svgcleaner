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

#ifndef BASECLEANER_H
#define BASECLEANER_H

#include <QRectF>

#include "transform.h"
#include "svgdom.h"
#include "tools.h"

#define Keys Keys::get()

using namespace Element;
using namespace Attribute;
using namespace DefaultValue;

class BaseCleaner
{
public:
    BaseCleaner(SvgDocument doc);
    virtual ~BaseCleaner() {}
    SvgDocument document() const;
    SvgElement svgElement() const;
    SvgElement defsElement() const;
    bool hasParent(const SvgElement &elem, const QString &tagName) const;
    bool hasUsedParent(const SvgElement &elem);
    QString parentAttribute(const SvgElement &elem, uint attrId) const;
    QRectF viewBoxRect();
    static SvgElement svgElement(SvgDocument doc);
    SvgElement smartElementRemove(SvgElement &rmElem, bool isReturnPrev = false) const;
    void removeAndMoveToPrev(SvgElement &elem);
    void removeAndMoveToPrevSibling(SvgElement &elem);
    QString genFreeId() const;
    void joinLinearGradients(SvgElement &parent, SvgElement &child) const;
    bool isGradientStopsEqual(const SvgElement &elem1, const SvgElement &elem2);

private:
    SvgDocument m_doc;
    SvgElement m_svgElem;
    SvgElement m_defsElem;

    SvgElement defsElement(SvgDocument doc, SvgElement &svgElem);
};

#endif // BASECLEANER_H
