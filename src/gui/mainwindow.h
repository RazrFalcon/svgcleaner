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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui/QComboBox>
#include <QtGui/QMainWindow>

#include "arguments.h"
#include "ui_mainwindow.h"

struct ProcessData {
    quint32 timeFull;
    quint32 pos;
    float compressMax;
    float compressMin;
    quint32 inputSize;
    quint32 outputSize;
    quint32 timeMax;
    quint32 timeMin;
    quint32 crashed;
};

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private:
    ProcessData m_data;
    QComboBox *cmbSort;
    QList<SVGInfo> itemList;
    QElapsedTimer totalTime;
    QList<ToThread> arguments;
    QFutureWatcher<SVGInfo> *m_cleaningWatcher;
    static int m_sortType;

    void createStatistics();
    void enableButtons(bool value);
    void removeThumbs();
    static bool customSort(const SVGInfo &s1, const SVGInfo &s2);

private slots:
    void on_actionCompareView_triggered();
    void on_actionInfo_triggered();
    void on_actionPause_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionWizard_triggered();
    void on_itemsScroll_valueChanged(int value);
    void prepareStart();
    void sortingChanged(int value);
    void onFileCleaned(int value);
    void onFinished();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
};

#endif // MAINWINDOW_H
