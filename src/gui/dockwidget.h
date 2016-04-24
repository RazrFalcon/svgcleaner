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

#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QWidget>
#else
    #include <QtGui/QWidget>
#endif

#include <QtCore/QElapsedTimer>

#include "arguments.h"

#include "ui_dockwidget.h"

struct ProcessData {
    int pos;
    int cleaned;
    int totalFiles;
    float compressMax;
    float compressMin;
    quint32 inputSize;
    quint32 outputSize;
    quint64 timeFull;
    quint64 timeMax;
    quint64 timeMin;
    quint32 crashed;
};

class DockWidget : public QWidget, public Ui::DockWidget
{
    Q_OBJECT

public:
    explicit DockWidget(QWidget *parent = 0);
    void start();
    int &currentPos();
    bool isFinished();
    void clear();
    void setFilesCount(int count);
    void appendInfo(SVGInfo *info);
    void setViewLayout(const Qt::Orientation &orient);

private:
    ProcessData m_data;
    QElapsedTimer totalTime;
    Qt::Orientation m_currOrientation;

    void fillFilds();
};

#endif // DOCKWIDGET_H
