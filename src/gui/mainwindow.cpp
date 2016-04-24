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
#include <QtCore/QThreadPool>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmapCache>

#if QT_VERSION >= 0x050000
    #include <QMenu>
    #include <QMessageBox>
    #include <QShortcut>
    #include <QtConcurrent>
#else
    #include <QtGui/QMenu>
    #include <QtGui/QMessageBox>
    #include <QtGui/QShortcut>
#endif

#include <QtGui/QWheelEvent>
#include <QtDebug>

#include "aboutdialog.h"
#include "cleanerthread.h"
#include "someutils.h"
#include "thumbwidget.h"
#include "wizarddialog.h"
#include "settings.h"
#include "mainwindow.h"

// FIXME: processing files like image.svg and image.svgz from different threads causes problems
// TODO: add cleaning stat for each cleaning option

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    setAcceptDrops(true);
    qRegisterMetaType<SVGInfo>("SVGInfo");

    Settings settings;

    // setup dock
    m_dockStatistics = new QDockWidget(this);
    m_dockStatistics->setWindowTitle(tr("Statistics"));
    m_dockStatistics->setFeatures(QDockWidget::DockWidgetMovable);
    int dockPos = settings.integer(SettingKey::GUI::DockPosition, Qt::TopDockWidgetArea);
    // check is dock pos is valid
    if (!(dockPos == Qt::LeftDockWidgetArea || dockPos == Qt::RightDockWidgetArea
        || dockPos == Qt::TopDockWidgetArea || dockPos == Qt::BottomDockWidgetArea))
    {
        dockPos = Qt::TopDockWidgetArea;
    }
    addDockWidget(Qt::DockWidgetArea(dockPos), m_dockStatistics);
    connect(m_dockStatistics, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(setupDock()));
    m_dockWidget = new DockWidget(this);
    setupDock();

    // setup GUI
    itemsWidget->installEventFilter(this);
    progressBar->hide();
    itemsScroll->hide();
    actionPause->setVisible(false);

    // create sorting combobox
    QWidget *w = new QWidget(toolBar);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(w);
    m_cmbSort = new QComboBox();
    m_cmbSort->addItem(tr("Sort by name"));
    m_cmbSort->addItem(tr("Sort by size"));
    m_cmbSort->addItem(tr("Sort by cleaning"));
    m_cmbSort->addItem(tr("Sort by attributes"));
    m_cmbSort->addItem(tr("Sort by elements"));
    m_cmbSort->addItem(tr("Sort by time"));
    m_cmbSort->setMinimumHeight(toolBar->height());
    m_cmbSort->setEnabled(false);
    connect(m_cmbSort, SIGNAL(currentIndexChanged(int)), itemsWidget, SLOT(sort(int)));
    toolBar->addWidget(m_cmbSort);

    QString suffix = "<br><span style=\"color:#808080;\">%1</span>";
    actionWizard->setToolTip(tr("Open the wizard") + suffix.arg("Ctrl+W"));
    actionStart->setToolTip(tr("Start processing") + suffix.arg("Ctrl+R"));
    actionPause->setToolTip(tr("Pause processing") + suffix.arg("Ctrl+R"));
    actionStop->setToolTip(tr("Stop cleaning") + suffix.arg("Ctrl+S"));

    itemsWidget->setScroll(itemsScroll);

    actionCompareView->setChecked(settings.flag(SettingKey::GUI::CompareView));
    on_actionCompareView_triggered();

    // setup shortcuts
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit, this);
    connect(quitShortcut, SIGNAL(activated()), qApp, SLOT(quit()));

    resize(settings.value(SettingKey::GUI::MainSize, QSize(1000, 650)).toSize());
}

void MainWindow::on_actionWizard_triggered()
{
    showWizard();
}

void MainWindow::showWizard(const QStringList &pathList)
{
    WizardDialog wizard;
    if (!pathList.isEmpty())
        wizard.setPathList(pathList);
    if (wizard.exec()) {
        m_threadData = wizard.threadData();
        m_files = wizard.files();
        actionStart->setEnabled(true);
        prepareStart();
    }
}

void MainWindow::prepareStart()
{
    QPixmapCache::clear();
    m_isStop = false;
    itemsWidget->clear();
    itemsScroll->hide();
    itemsScroll->setMaximum(0);
    itemsScroll->setValue(0);
    progressBar->setValue(0);
    progressBar->setMaximum(100);
    m_cmbSort->setCurrentIndex(0);
    m_dockWidget->clear();
    m_dockWidget->setFilesCount(m_files.size());
}

void MainWindow::on_actionStart_triggered()
{
    if (!actionStart->isEnabled())
        return;

    if (itemsWidget->itemsCount() == 0 || !actionStop->isEnabled()) {
        m_dockWidget->start();
        prepareStart();
        enableButtons(false);
    }

    if (!actionPause->isVisible()) {
        enableButtons(false);
        actionPause->setVisible(true);
        actionStart->setVisible(false);
        m_isStop = false;

        foreach (CleanerThread *cleaner, m_cleanerList)
            cleaner->invokeStartNext(m_files.at(m_dockWidget->currentPos()++));

        return;
    }

    if (!m_threadData.args.isEmpty()) {
        qDebug() << "with keys:";
        foreach (const QString &key, m_threadData.args)
            qDebug() << key;
    }

    int threadCount = 1;
    Settings settings;
    if (settings.flag(SettingKey::Wizard::ThreadingEnabled))
        threadCount = settings.integer(SettingKey::Wizard::ThreadsCount, QThread::idealThreadCount());
    if (threadCount > m_files.size())
        threadCount = m_files.size();
    QThreadPool::globalInstance()->setMaxThreadCount(threadCount);

    progressBar->setMaximum(m_files.size());
    for (int i = 0; i < threadCount; ++i) {
        ThreadData tth = m_threadData;
        tth.id = QString::number(i);

        QThread *thread = new QThread(this);
        thread->setObjectName("CleanerThread" + QString::number(i));
        CleanerThread *cleaner = new CleanerThread(tth);
        m_cleanerList << cleaner;
        connect(cleaner, SIGNAL(cleaned(SVGInfo*)), this, SLOT(onFileCleaned(SVGInfo*)),
                Qt::QueuedConnection);
        cleaner->moveToThread(thread);
        thread->start();
        cleaner->invokeStartNext(m_files.at(m_dockWidget->currentPos()++));
    }
}

void MainWindow::onFileCleaned(SVGInfo *info)
{
    if (m_dockWidget->currentPos() < m_files.size()) {
        if (!m_isStop) {
            CleanerThread *cleaner = qobject_cast<CleanerThread *>(sender());
            if (cleaner)
                cleaner->invokeStartNext(m_files.at(m_dockWidget->currentPos()++));
            else
                qDebug() << "Warning: sender() is null";
        }
    } else {
        CleanerThread *cleaner = qobject_cast<CleanerThread *>(sender());
        if (cleaner) {
#ifdef USE_IPC
            connect(cleaner, SIGNAL(finished()), this, SLOT(removeCleaner()));
            QMetaObject::invokeMethod(cleaner, "exit", Qt::QueuedConnection);
#else
            removeCleaner(cleaner);
#endif
        } else {
            qDebug() << "Warning: sender() is null";
        }
    }

    progressBar->setValue(progressBar->value()+1);
    m_dockWidget->appendInfo(info);
    itemsWidget->appendData(info);
    if (itemsWidget->dataCount() > itemsWidget->itemsCount()) {
        itemsScroll->show();
        itemsScroll->setMaximum(itemsScroll->maximum()+1);
    }
    if (m_dockWidget->isFinished())
        onFinished();
}

void MainWindow::onFinished()
{
    enableButtons(true);
}

void MainWindow::on_actionPause_triggered()
{
    m_isStop = true;
    actionPause->setVisible(false);
    actionStart->setVisible(true);
}

void MainWindow::on_actionStop_triggered()
{
    if (!actionStop->isEnabled())
        return;
    actionPause->setVisible(false);
    m_isStop = true;
    foreach (CleanerThread *cleaner, m_cleanerList) {
        cleaner->forceStop();
        removeCleaner(cleaner);
    }
    onFinished();
}

void MainWindow::removeCleaner(CleanerThread *cleaner)
{
    if (!cleaner)
        cleaner = qobject_cast<CleanerThread *>(sender());
    if (!cleaner)
        return;
    m_cleanerList.removeOne(cleaner);
    cleaner->thread()->quit();
    cleaner->thread()->wait();
    delete cleaner->thread();
    delete cleaner;
}

void MainWindow::setupDock()
{
    const Qt::DockWidgetArea area = dockWidgetArea(m_dockStatistics);
    if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
        if (m_dockStatistics->widget() && !m_dockStatistics->widget()->inherits("DockWidget"))
            m_dockStatistics->widget()->deleteLater();
        m_dockWidget->setViewLayout(Qt::Vertical);
        m_dockStatistics->setWidget(m_dockWidget);
        m_dockStatistics->setTitleBarWidget(0);
        m_dockStatistics->setFixedHeight(QWIDGETSIZE_MAX);
    } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        m_dockWidget->setViewLayout(Qt::Horizontal);
        QWidget *w = new QWidget(this);
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_dockStatistics->setWidget(w);
        m_dockStatistics->setTitleBarWidget(m_dockWidget);
        m_dockStatistics->setFixedHeight(QFontMetrics(font()).height() * 1.5);
    }
}

void MainWindow::enableButtons(bool value)
{
    actionStart->setVisible(value);
    actionPause->setVisible(!value);
    actionStop->setEnabled(!value);
    actionWizard->setEnabled(value);
    actionInfo->setEnabled(value);
    m_cmbSort->setEnabled(value);
    progressBar->setVisible(!value);
}

void MainWindow::on_itemsScroll_valueChanged(int value)
{
    itemsWidget->scrollTo(value);
}

void MainWindow::on_actionCompareView_triggered()
{
    if (actionCompareView->isChecked()) {
        actionCompareView->setIcon(QIcon(":/thumbs-on.svgz"));
        actionCompareView->setToolTip(tr("Compare view: on"));
    } else {
        actionCompareView->setIcon(QIcon(":/thumbs-off.svgz"));
        actionCompareView->setToolTip(tr("Compare view: off"));
    }
    itemsWidget->setCompareView(actionCompareView->isChecked());

    Settings settings;
    settings.setValue(SettingKey::GUI::CompareView, actionCompareView->isChecked());
}

void MainWindow::on_actionInfo_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == itemsWidget) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->delta() > 0)
                itemsScroll->setValue(itemsScroll->value()-1);
            else
                itemsScroll->setValue(itemsScroll->value()+1);
            return true;
        }
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_End)
                itemsScroll->setValue(itemsScroll->maximum());
            else if (keyEvent->key() == Qt::Key_Home)
                itemsScroll->setValue(0);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if (!mime->hasUrls()) {
        event->ignore();
        return;
    }

    bool hasWrongFileTypes = false;
    QStringList paths;
    foreach (const QUrl &url, mime->urls()) {
        if (url.isLocalFile()) {
            QString path = url.toLocalFile();
            if (QFileInfo(path).isDir()) {
                paths << path;
            } else if (QFileInfo(path).isFile()) {
                QString suffix = QFileInfo(path).suffix().toLower();
                if (suffix == "svg" || suffix == "svgz")
                    paths << path;
                else
                    hasWrongFileTypes = true;
            }
        }
    }
    event->acceptProposedAction();

    if (hasWrongFileTypes)
        QMessageBox::warning(this, tr("Warning"),
                             tr("You can drop only svg(z) files or folders."));

    if (!paths.isEmpty())
        showWizard(paths);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_cleanerList.isEmpty()) {
        int ans = QMessageBox::question(this, "SVG Cleaner",
                                        tr("Cleaning is not finished.\nDid you really want to exit?"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            foreach (CleanerThread *cleaner, m_cleanerList) {
                cleaner->forceStop();
                removeCleaner(cleaner);
            }
        } else {
            event->ignore();
        }
    }
    Settings().setValue(SettingKey::GUI::MainSize, size());
    Settings().setValue(SettingKey::GUI::DockPosition, dockWidgetArea(m_dockStatistics));
}
