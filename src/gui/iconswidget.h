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

#ifndef ICONSWIDGET_H
#define ICONSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>

class IconsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconsWidget(QWidget *parent = 0);
    void setError(const QString &text);
    void setPaths(const QString &pathIn, const QString &pathOut, const bool compare);

private:
    bool compareView;
    bool crashed;
    bool newToolTip;
    bool refresh;
    QLabel *toolTip;
    QPixmap tooltipPix;
    QPixmap leftPix;
    QPixmap rightPix;
    QString inpath;
    QString outpath;
    QString errText;
    void makeToolTip();
    QPixmap renderSvg(const QString &path, const QRect &rect, bool cleaned);
    
private slots:
    void showToolTip();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
};

#endif // ICONSWIDGET_H
