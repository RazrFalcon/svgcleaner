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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QSettings>
#include <QtCore/QElapsedTimer>

#include <QtGui/QComboBox>
#include <QtGui/QMainWindow>

#include "arguments.h"
#include "cleanerthread.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private:
    float compressMax;
    float compressMin;
    float timeFull;
    float inputSize;
    float outputSize;
    int position;
    int timeMax;
    int timeMin;
    QComboBox *cmbSort;
    QList<CleanerThread *> cleanerList;
    QList<SVGInfo> itemList;
    QSettings *settings;
    QElapsedTimer time;
    ToThread arguments;

    void createStatistics();
    void deleteThreads();
    void enableButtons(bool value);
    void removeThumbs();
    void startNext();

private slots:
    void cleaningFinished();
    void on_actionCompareView_triggered();
    void on_actionInfo_triggered();
    void on_actionPause_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionWizard_triggered();
    void on_itemScroll_valueChanged(int value);
    void prepareStart();
    void progress(SVGInfo);
    void sortingChanged(int value);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);
};

#endif // MAINWINDOW_H
