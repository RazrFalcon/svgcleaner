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

#include <QtCore/QElapsedTimer>
#include <QtGui/QComboBox>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>

#include "cleanerthread.h"
#include "arguments.h"
#include "dockwidget.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private:
    bool m_isStop;
    QComboBox *m_cmbSort;
    ThreadData m_threadData;
    QList<StringPair> m_files;
    QList<CleanerThread *> m_cleanerList;
    DockWidget *m_dockWidget;
    QDockWidget *m_dockStatistics;

    void enableButtons(bool value);

private slots:
    void on_actionCompareView_triggered();
    void on_actionInfo_triggered();
    void on_actionPause_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionWizard_triggered();
    void on_itemsScroll_valueChanged(int value);
    void prepareStart();
    void onFileCleaned(SVGInfo *info);
    void onFinished();
    void removeCleaner(CleanerThread *cleaner = 0);
    void setupDock();
    void showWizard(const QStringList &pathList = QStringList());

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
};

#endif // MAINWINDOW_H
