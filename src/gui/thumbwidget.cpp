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

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtGui/QResizeEvent>

#include "someutils.h"
#include "thumbwidget.h"

ThumbWidget::ThumbWidget(SVGInfo *info, bool compare, QWidget *parent) :
    QFrame(parent)
{
    setupUi(this);
#ifdef Q_OS_WIN
    setFrameShadow(QFrame::Plain);
#endif
    refill(info, compare);
    // do no run immediately, to skip default events and make correct text elide
    QTimer::singleShot(100, this, SLOT(elideName()));
}

void ThumbWidget::refill(SVGInfo *info, bool compare)
{
    iconsWidget->setPaths(info->inPath, info->outPath, compare);

    m_name = QFileInfo(info->outPath).fileName();
    elideName();

    if (!info->errString.isEmpty()) {
        lblSizes->setText("0 -> 0 (0.00%)");
        lblElem->setText( "0 -> 0 (0.00%)");
        lblAttr->setText( "0 -> 0 (0.00%)");
        lblTime->setText( "0");
        iconsWidget->setError(info->errString);
    } else {
        lblSizes->setText(QString("%1 -> %2 (%3%)")
                          .arg(SomeUtils::prepareSize(info->inSize))
                          .arg(SomeUtils::prepareSize(info->outSize))
                          .arg(QString::number(info->compress, 'f', 2)));

        float elemPerc = ((float)info->elemFinal / info->elemInitial) * 100;
        lblElem->setText(QString("%1 -> %2 (%3%)")
                         .arg(info->elemInitial)
                         .arg(info->elemFinal)
                         .arg(QString::number(elemPerc, 'f', 2)));

        float attrPerc = ((float)info->attrFinal / info->attrInitial) * 100;
        lblAttr->setText(QString("%1 -> %2 (%3%)")
                         .arg(info->attrInitial)
                         .arg(info->attrFinal)
                         .arg(QString::number(attrPerc, 'f', 2)));
        lblTime->setText(SomeUtils::prepareTime(info->time));
    }
}

void ThumbWidget::elideName()
{
    QFontMetrics fm(lblName->font());
    QString name = fm.elidedText(m_name, Qt::ElideLeft, lblName->width());
    lblName->setText(name);
}

void ThumbWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    elideName();
}
