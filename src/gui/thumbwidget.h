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

#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QtGui/QWidget>

#include "arguments.h"
#include "ui_thumbwidget.h"

class ThumbWidget : public QWidget, private Ui::ThumbWidget
{
    Q_OBJECT

public:
    explicit ThumbWidget(const SVGInfo &info, bool compare = false, QWidget *parent = 0);
    void refill(const SVGInfo &info, bool compare = false);

private:
    QString m_name;

protected:
    void resizeEvent(QResizeEvent *);
};
#endif // THUMBWIDGET_H
