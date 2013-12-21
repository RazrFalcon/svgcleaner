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

#include <QtGui/QIcon>

#include "itemwidget.h"

ItemWidget::ItemWidget(const QString &icon, QWidget *parent) :
    QWidget(parent)
{
    lblIcon = new QLabel(this);
    layout = new QVBoxLayout(this);
    lblIcon->setPixmap(QIcon(":/" + icon + ".svgz").pixmap(46, 46));
    lblIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblIcon);
    setLayout(layout);
}

ItemWidget::~ItemWidget()
{
    delete lblIcon;
    delete layout;
}
