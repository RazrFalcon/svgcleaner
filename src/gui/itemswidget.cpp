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

#include <QtGui/QResizeEvent>

#if QT_VERSION >= 0x050000
    #include <QScrollBar>
#else
    #include <QtGui/QScrollBar>
#endif

#include <QtDebug>

#include "settings.h"
#include "itemswidget.h"

int ItemsWidget::m_sortType = -1;

ItemsWidget::ItemsWidget(QWidget *parent) :
    QScrollArea(parent)
{
    isCompareView = Settings().flag(SettingKey::GUI::CompareView);

    itemHeight = QFontMetrics(font()).height() * 7;
    if (itemHeight < 120)
        itemHeight = 120;

    m_contentWidget = new QWidget(this);
    m_lay = new QVBoxLayout(m_contentWidget);
    m_lay->setContentsMargins(4,0,4,4);
    m_lay->setSpacing(4);
    m_lay->addStretch();

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setWidget(m_contentWidget);
    setWidgetResizable(true);
}

void ItemsWidget::setScroll(QScrollBar *scrollBar)
{
    m_scrollBar = scrollBar;
}

bool ItemsWidget::haveFreeSpace()
{
    return ((m_itemList.size() + 1) * (itemHeight + m_lay->spacing()))
            < height() - m_lay->contentsMargins().bottom();
}

void ItemsWidget::appendData(SVGInfo *info)
{
    m_dataList.append(info);

    if (haveFreeSpace() || m_itemList.isEmpty()) {
        ThumbWidget *w = new ThumbWidget(info, isCompareView, this);
        w->setFixedHeight(itemHeight);
        m_itemList << w;
        m_lay->insertWidget(m_lay->count()-1, w);
    }
}

void ItemsWidget::clear()
{
    qDeleteAll(m_itemList);
    m_itemList.clear();
    m_dataList.clear();
}

void ItemsWidget::scrollTo(int pos)
{
    if (pos < 0)
        pos = 0;
    foreach (ThumbWidget *w, m_itemList) {
        if (pos < m_dataList.size()) {
            w->refill(m_dataList.at(pos), isCompareView);
            pos++;
        }
    }
}

void ItemsWidget::setCompareView(bool flag)
{
    isCompareView = flag;
    scrollTo(m_scrollBar->value());
}

int ItemsWidget::itemsCount()
{
    return m_itemList.size();
}

int ItemsWidget::dataCount()
{
    return m_dataList.size();
}

void ItemsWidget::sort(int type)
{
    if (m_dataList.isEmpty())
        return;
    m_sortType = type;
    qSort(m_dataList.begin(), m_dataList.end(), &ItemsWidget::itemSort);
    scrollTo(m_scrollBar->value());
}

bool ItemsWidget::itemSort(SVGInfo *s1, SVGInfo *s2)
{
    if (m_sortType == 0)
        return s1->outPath.toLower() < s2->outPath.toLower();
    else if (m_sortType == 1)
        return s1->outSize < s2->outSize;
    else if (m_sortType == 2)
        return s1->compress < s2->compress;
    else if (m_sortType == 3)
        return s1->attrFinal < s2->attrFinal;
    else if (m_sortType == 4)
        return s1->elemFinal < s2->elemFinal;
    else if (m_sortType == 5)
        return s1->time < s2->time;
    return s1->outPath.toLower() < s2->outPath.toLower();
}

void ItemsWidget::resizeEvent(QResizeEvent *event)
{
    if (m_dataList.isEmpty()) {
        QScrollArea::resizeEvent(event);
        return;
    }

    int i = m_scrollBar->value() + m_itemList.size();
    // try to add new widgets
    while (haveFreeSpace() && i < m_dataList.size()) {
        ThumbWidget *w = new ThumbWidget(m_dataList.at(i), isCompareView, this);
        w->setFixedHeight(itemHeight);
        m_itemList << w;
        m_lay->insertWidget(m_lay->count()-1, w);
        i++;
    }
    // when we increase window height and scroll at maximum - prepend new widgets
    if (i == m_dataList.size()) {
        i -= m_itemList.size();
        while (haveFreeSpace() && i > 0) {
            ThumbWidget *w = new ThumbWidget(m_dataList.at(i), isCompareView, this);
            w->setFixedHeight(itemHeight);
            m_itemList.prepend(w);
            m_lay->insertWidget(0, w);
            i--;
        }
    }
    int wh = height() - m_lay->contentsMargins().bottom();
    while (m_itemList.size() * (itemHeight + m_lay->spacing()) > wh && !m_itemList.isEmpty()) {
        m_lay->removeWidget(m_itemList.last());
        delete m_itemList.last();
        m_itemList.removeLast();
    }

    m_scrollBar->setMaximum(m_dataList.count() - m_itemList.count());

    QScrollArea::resizeEvent(event);
}
