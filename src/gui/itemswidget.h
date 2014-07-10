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

#ifndef ITEMSWIDGET_H
#define ITEMSWIDGET_H

#include <QtGui/QScrollArea>

#include "thumbwidget.h"

class ItemsWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ItemsWidget(QWidget *parent = 0);
    void setScroll(QScrollBar *scrollBar);
    void appendData(SVGInfo *info);
    void clear();
    void setCompareView(bool flag);
    int itemsCount();
    int dataCount();

public slots:
    void scrollTo(int pos);
    void sort(int type);

private:
    static int m_sortType;
    bool isCompareView;
    QScrollBar *m_scrollBar;
    QList<ThumbWidget *> m_itemList;
    QList<SVGInfo *> m_dataList;
    QVBoxLayout *m_lay;
    QWidget *m_contentWidget;
    int itemHeight;

    bool haveFreeSpace();
    static bool itemSort(SVGInfo *s1, SVGInfo *s2);

protected:
    void resizeEvent(QResizeEvent *);
};

#endif // ITEMSWIDGET_H
