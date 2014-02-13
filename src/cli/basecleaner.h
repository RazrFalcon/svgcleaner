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

#ifndef BASECLEANER_H
#define BASECLEANER_H

#include <QRectF>

#include "svgelement.h"
#include "tools.h"

#define Keys Keys::get()

class BaseCleaner
{
public:
    BaseCleaner(XMLDocument *doc);
    virtual ~BaseCleaner() {}
    XMLDocument* document() const;
    SvgElement svgElement() const;
    SvgElement defsElement() const;
    void updateXLinks(const StringHash &hash);
    SvgElement findDefElement(const QString &id);
    SvgElement findElement(const QString &id, XMLElement *parent = 0);
    bool hasParent(const SvgElement &elem, const QString &tagName);
    QString findAttribute(const SvgElement &elem, const char *attrName);
    QString absoluteTransform(const SvgElement &elem);
    QRectF viewBoxRect();
    static SvgElement svgElement(XMLDocument *doc);
    static QList<XMLNode *> childNodeList(XMLNode *node);
    static QList<SvgElement> childElemList(XMLDocument *doc);

private:
    XMLDocument *m_doc;
    SvgElement m_svgElem;
    SvgElement m_defsElem;

    SvgElement defsElement(XMLDocument *doc, SvgElement &svgElem);
};

#endif // BASECLEANER_H
