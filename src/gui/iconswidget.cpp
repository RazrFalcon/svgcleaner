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

#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QtGui/QBitmap>
#include <QtGui/QCursor>
#include <QtGui/QDesktopServices>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>

#include <QtSvg/QSvgRenderer>

#include "iconswidget.h"

IconsWidget::IconsWidget(QWidget *parent) :
    QWidget(parent)
{
    toolTip = new QLabel();
    toolTip->setWindowFlags(Qt::ToolTip);
    setMouseTracking(true);
    setFixedHeight(100);
    refresh = true;
    compareView = true;
    crashed = false;
    newToolTip = true;
}

void IconsWidget::setPaths(const QString &pathIn, const QString &pathOut, const bool compare)
{
    inpath = pathIn;
    outpath = pathOut;
    compareView = compare;
    newToolTip = true;
    refresh = true;
    if (toolTip->isVisible())
        makeToolTip();

    if (compare)
        setFixedWidth(205);
    else
        setFixedWidth(100);

    crashed = false;
    repaint();
}

void IconsWidget::setError(const QString &text)
{
    crashed = true;
    errText = text;
    repaint();
}

void IconsWidget::makeToolTip()
{
    QImage image(620, 310, QImage::Format_ARGB32);
    image.fill(0);
    tooltipPix = QPixmap::fromImage(image, Qt::NoOpaqueDetection | Qt::AutoColor);
    QPainter painter(&tooltipPix);
    QPalette pal;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setBrush(QBrush(pal.color(QPalette::Background)));
    painter.setPen(QPen(QColor("#636363"), 2));
    painter.drawRoundedRect(2, 2, image.width()-4, image.height()-4, 5, 5);
    painter.drawPixmap(5, 5, renderSvg(inpath, QRect(5, 5, 300, 300), false));
    painter.drawPixmap(310, 5, renderSvg(outpath, QRect(5, 5, 300, 300), true));

    toolTip->setPixmap(tooltipPix);
    toolTip->setMask(tooltipPix.mask());
    newToolTip = false;
}

void IconsWidget::enterEvent(QEvent *)
{
    QTimer::singleShot(500, this, SLOT(showToolTip()));
}

void IconsWidget::showToolTip()
{
    if (crashed)
        return;
    if (newToolTip)
        makeToolTip();
    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (rect().contains(cursPos)) {
        toolTip->show();
        this->setFocus();
    }
}

void IconsWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (crashed)
        return;

    QCursor cursor;
    QPoint cursorPos = mapFromGlobal(cursor.pos());

    if (rect().contains(cursorPos)) {
        QPoint point  = mapToGlobal(event->pos());
        QPoint point2 = mapToGlobal(QPoint(0, 0));
        if (point2.y() - tooltipPix.height() - 6 > 0) {
            toolTip->setGeometry(point.x() - tooltipPix.width()/2, point2.y() - tooltipPix.height()-6,
                                 tooltipPix.width(), tooltipPix.height());
        } else {
            toolTip->setGeometry(point.x() - tooltipPix.width()/2, point2.y() + height() + 5,
                                 tooltipPix.width(), tooltipPix.height());
        }
    }
}

void IconsWidget::leaveEvent(QEvent *)
{
    QPoint cursPos = mapFromGlobal(QCursor().pos());
    if (!rect().contains(cursPos))
        toolTip->hide();
}

void IconsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (crashed) {
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, errText);
        return;
    }

    if (compareView) {
        if (refresh)
            leftPix = renderSvg(inpath, QRect(0,   0, 100, 100), false);
        painter.drawPixmap(0, 0, leftPix);

        if (refresh)
            rightPix = renderSvg(outpath, QRect(0, 0, 100, 100), true);
        painter.drawPixmap(105, 0, rightPix);
    } else {
        if (refresh)
            rightPix = renderSvg(outpath, QRect(0, 0, width(), height()), true);
        painter.drawPixmap(0, 0, rightPix);
    }
    refresh = false;
}

QPixmap IconsWidget::renderSvg(const QString &path, const QRect &rect, bool cleaned)
{
    QString tpath = path;
    if (cleaned)
        tpath += "_cleaned" + QString::number(rect.height());
    else
        tpath += "_orig" + QString::number(rect.height());

    if (QPixmapCache::find(tpath)) {
        QPixmap pix;
        QPixmapCache::find(tpath, &pix);
        return pix;
    }

    QSvgRenderer renderer(path);
    QSize size = renderer.viewBox().size();
    size.scale(rect.width(), rect.height(), Qt::KeepAspectRatio);
    static int border = 3;
    QPixmap pix(rect.width(), rect.height());
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    renderer.render(&p, QRect(rect.x()+border, (rect.height()-size.height())/2+rect.y()+border,
                                size.width()-border*2, size.height()-border*2));
    QPixmapCache::insert(tpath, pix);
    return pix;
}

void IconsWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || crashed)
        return;
    if (event->x() < width()/2)
        QDesktopServices::openUrl(QUrl(inpath));
    else
        QDesktopServices::openUrl(QUrl(outpath));
}
