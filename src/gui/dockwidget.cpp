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


#include "dockwidget.h"
#include "someutils.h"

DockWidget::DockWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    m_currOrientation = Qt::Horizontal;
    m_data.totalFiles = 0;
    clear();

    QFontMetrics fm(font());
    lblIAverCompH->setMinimumWidth(fm.width("00.00%  "));
    lblSizeH->setMinimumWidth(fm.width("0.0 " + QObject::tr("B")) * 2);
}

void DockWidget::start()
{
    totalTime.start();
}

int& DockWidget::currentPos()
{
    return m_data.pos;
}

bool DockWidget::isFinished()
{
    return (m_data.cleaned == m_data.totalFiles);
}

void DockWidget::clear()
{
    m_data.compressMax = 0;
    m_data.compressMin = 99;
    m_data.timeMax = 0;
    m_data.timeMin = ULONG_MAX;
    m_data.inputSize = 0;
    m_data.timeFull = 0;
    m_data.outputSize = 0;
    m_data.pos = 0;
    m_data.cleaned = 0;
    m_data.totalFiles = 0;
    m_data.crashed = 0;

    foreach (QLabel *lbl, gBoxFiles->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0");
    foreach (QLabel *lbl, gBoxCleaned->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0%");
    foreach (QLabel *lbl, gBoxTime->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0.0" + QObject::tr("ms"));

    lblITotalFilesH->setText("0/0");
    lblICrashedH->setText("");
    lblSizeH->setText(QString("0 %1/0 %1").arg(QObject::tr("B")));
    lblIAverCompH->setText("0%");
    lblIFullTimeH->setText("0.0" + QObject::tr("ms"));
}

void DockWidget::setFilesCount(int count)
{
    m_data.totalFiles = count;
    lblITotalFiles->setText(QString::number(count));

    QFontMetrics fm(font());
    lblITotalFilesH->setMinimumWidth(fm.width(QString::number(count) + "/") * 2);
}

void DockWidget::appendInfo(SVGInfo *info)
{
    m_data.cleaned++;
    if (!info->errString.isEmpty()) {
        m_data.crashed++;
        lblICrashed->setText(QString::number(m_data.crashed));
        lblICrashedH->setText(QString("(%1: %2)").arg(tr("crashed")).arg(m_data.crashed));
    } else {
        m_data.inputSize  += info->inSize;
        m_data.outputSize += info->outSize;
        if (info->compress > m_data.compressMax && info->compress < 100)
            m_data.compressMax = info->compress;
        if (info->compress < m_data.compressMin && info->compress > 0)
            m_data.compressMin = info->compress;
        if (info->time > m_data.timeMax)
            m_data.timeMax = info->time;
        if (info->time < m_data.timeMin)
            m_data.timeMin = info->time;
        m_data.timeFull += info->time;
    }
    fillFilds();
}

void DockWidget::fillFilds()
{
    if (m_data.totalFiles == 0)
        return;

    int files = m_data.cleaned - m_data.crashed;
    QString avrComp;
    if (m_data.outputSize != 0 && m_data.inputSize != 0)
        avrComp = QString::number(100-((qreal)m_data.outputSize/m_data.inputSize)*100, 'f', 2) + "%";
    QString fullTime = SomeUtils::prepareTime(totalTime.nsecsElapsed());

    if (m_currOrientation == Qt::Vertical) {
        // files
        lblICleaned->setText(QString::number(files));
        lblITotalSizeBefore->setText(SomeUtils::prepareSize(m_data.inputSize));
        lblITotalSizeAfter->setText(SomeUtils::prepareSize(m_data.outputSize));

        // cleaned
        lblIAverComp->setText(avrComp);
        lblIMaxCompress->setText(QString::number(100-m_data.compressMin, 'f', 2) + "%");
        lblIMinCompress->setText(QString::number(100-m_data.compressMax, 'f', 2) + "%");

        // time
        lblIFullTime->setText(fullTime);
        lblIMaxTime->setText(SomeUtils::prepareTime(m_data.timeMax));
        if (files != 0)
            lblIAverageTime->setText(SomeUtils::prepareTime(quint64(m_data.timeFull)/files));
        if (m_data.timeMin != ULONG_MAX)
            lblIMinTime->setText(SomeUtils::prepareTime(m_data.timeMin));
    } else {
        lblITotalFilesH->setText(QString::number(files) + "/" + QString::number(m_data.totalFiles));
        lblSizeH->setText(SomeUtils::prepareSize(m_data.outputSize) + "/"
                          + SomeUtils::prepareSize(m_data.inputSize));
        lblIAverCompH->setText(avrComp);
        if (files != 0) {
            QString str = fullTime;
            str += " (" + tr("avr") + ": " + SomeUtils::prepareTime(m_data.timeFull/files) + ")";
            lblIFullTimeH->setText(str);
        } else
            lblIFullTimeH->setText(fullTime);
    }
}

void DockWidget::setViewLayout(const Qt::Orientation &orient)
{
    m_currOrientation = orient;
    if (layout())
        delete layout();
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(4,4,4,4);
    if (orient == Qt::Vertical) {
        lay->addWidget(widget1);
        widget1->show();
        widget2->hide();
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    } else {
        lay->addWidget(widget2);
        widget1->hide();
        widget2->show();
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    fillFilds();
}
