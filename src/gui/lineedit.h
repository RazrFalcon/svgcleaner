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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QStyleOptionFrameV2>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = 0);
    void showLoading(bool value);

private:
    QLabel *lbl;
    int angle;
    int timerId;
    QRect progressRect;
    void initStyleOption(QStyleOptionFrameV2 *option) const;
    QColor textColor;

public slots:
    void setValue(const int &value);

protected:
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);

};

#endif // LINEEDIT_H
