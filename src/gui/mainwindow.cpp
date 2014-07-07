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
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QShortcut>
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

int MainWindow::m_sortType = -1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    setAcceptDrops(true);
    qRegisterMetaType<SVGInfo>("SVGInfo");

    // setup GUI
    scrollArea->installEventFilter(this);
    progressBar->hide();
    itemsScroll->hide();
    actionPause->setVisible(false);

    m_isExitNow = false;

    // load settings
    Settings settings;

    // create sorting combobox
    QWidget *w = new QWidget(toolBar);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(w);
    cmbSort = new QComboBox();
    cmbSort->addItem(tr("Sort by name"));
    cmbSort->addItem(tr("Sort by size"));
    cmbSort->addItem(tr("Sort by cleaning"));
    cmbSort->addItem(tr("Sort by attributes"));
    cmbSort->addItem(tr("Sort by elements"));
    cmbSort->addItem(tr("Sort by time"));
    cmbSort->setMinimumHeight(toolBar->height());
    cmbSort->setEnabled(false);
    connect(cmbSort, SIGNAL(currentIndexChanged(int)), this, SLOT(sortingChanged(int)));
    toolBar->addWidget(cmbSort);

    QString suffix = "<br><span style=\"color:#808080;\">%1</span>";
    actionWizard->setToolTip(tr("Open the wizard") + suffix.arg("Ctrl+W"));
    actionStart->setToolTip(tr("Start processing") + suffix.arg("Ctrl+R"));
    actionPause->setToolTip(tr("Pause processing") + suffix.arg("Ctrl+R"));
    actionStop->setToolTip(tr("Stop cleaning") + suffix.arg("Ctrl+S"));

    actionCompareView->setChecked(settings.flag(SettingKey::GUI::CompareView));
    on_actionCompareView_triggered();

    // setup shortcuts
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit, this);
    connect(quitShortcut, SIGNAL(activated()), qApp, SLOT(quit()));

    resize(settings.value(SettingKey::GUI::MainSize, QSize(1000, 650)).toSize());

    QTimer::singleShot(10, actionWizard, SLOT(trigger()));
}

void MainWindow::on_actionWizard_triggered()
{
    WizardDialog wizard;
    if (wizard.exec()) {
        m_threadData = wizard.threadData();
        m_files = wizard.files();
        actionStart->setEnabled(true);
    }
}

void MainWindow::prepareStart()
{
    QPixmapCache::clear();
    m_isStop = false;
    m_data.compressMax = 0;
    m_data.compressMin = 99;
    m_data.timeMax = 0;
    m_data.timeMin = ULONG_MAX;
    m_data.inputSize = 0;
    m_data.timeFull = 0;
    m_data.outputSize = 0;
    m_data.pos = 0;
    m_data.cleaned = 0;
    m_data.crashed = 0;
    removeThumbs();
    enableButtons(false);
    itemsScroll->hide();
    itemsScroll->setMaximum(0);
    itemsScroll->setValue(0);
    itemList.clear();
    itemList.reserve(m_files.size());
    progressBar->setValue(0);
    progressBar->setMaximum(100);
    itemLayout->addStretch(100);
    cmbSort->setCurrentIndex(0);

    foreach (QLabel *lbl, gBoxFiles->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0");
    foreach (QLabel *lbl, gBoxCleaned->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0%");
    foreach (QLabel *lbl, gBoxTime->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("000ms");
    lblITotalFiles->setText(QString::number(m_files.size()));
}

void MainWindow::on_actionStart_triggered()
{
    if (!actionStart->isEnabled())
        return;

    if (itemList.isEmpty() || !actionStop->isEnabled()) {
        totalTime.start();
        prepareStart();
    }

    if (!actionPause->isVisible()) {
        enableButtons(false);
        actionPause->setVisible(true);
        actionStart->setVisible(false);
        m_isStop = false;

        foreach (CleanerThread *cleaner, cleanerList)
            cleaner->invokeStartNext(m_files.at(m_data.pos++));

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
    QThreadPool::globalInstance()->setMaxThreadCount(threadCount);

    progressBar->setMaximum(m_files.size());
    for (int i = 0; i < threadCount; ++i) {
        ThreadData tth = m_threadData;
        tth.id = QString::number(i);

        QThread *thread = new QThread(this);
        thread->setObjectName("CleanerThread" + QString::number(i));
        CleanerThread *cleaner = new CleanerThread(tth);
        cleanerList << cleaner;
        connect(cleaner, SIGNAL(cleaned(SVGInfo)), this, SLOT(onFileCleaned(SVGInfo)),
                Qt::QueuedConnection);
        cleaner->moveToThread(thread);
        thread->start();
        cleaner->invokeStartNext(m_files.at(m_data.pos++));
    }
}

void MainWindow::onFileCleaned(const SVGInfo &info)
{
    if (m_data.pos < m_files.size()) {
        if (!m_isStop) {
            CleanerThread *cleaner = qobject_cast<CleanerThread *>(sender());
            if (cleaner)
                cleaner->invokeStartNext(m_files.at(m_data.pos++));
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

    m_data.cleaned++;
    if (m_data.cleaned == m_files.size())
        onFinished();

    progressBar->setValue(progressBar->value()+1);

    itemList.append(info);
    if (!info.errString.isEmpty()) {
        lblICrashed->setText(QString::number(++m_data.crashed));
    } else {
        m_data.inputSize  += info.inSize;
        m_data.outputSize += info.outSize;
        if (info.compress > m_data.compressMax && info.compress < 100)
            m_data.compressMax = info.compress;
        if (info.compress < m_data.compressMin && info.compress > 0)
            m_data.compressMin = info.compress;
        if (info.time > m_data.timeMax)
            m_data.timeMax = info.time;
        if (info.time < m_data.timeMin)
            m_data.timeMin = info.time;
        m_data.timeFull += info.time;
    }

    int available = scrollArea->height()/itemLayout->itemAt(0)->geometry().height();
    if (available >= itemLayout->count() || itemLayout->isEmpty()) {
        itemLayout->insertWidget(itemLayout->count()-1, new ThumbWidget(info,
                                                                actionCompareView->isChecked()));
    } else {
        itemsScroll->show();
        itemsScroll->setMaximum(itemsScroll->maximum()+1);
    }
    createStatistics();
}

void MainWindow::createStatistics()
{
    // files
    int files = m_data.cleaned - m_data.crashed;
    lblICleaned->setText(QString::number(files));
    lblITotalSizeBefore->setText(SomeUtils::prepareSize(m_data.inputSize));
    lblITotalSizeAfter->setText(SomeUtils::prepareSize(m_data.outputSize));

    // cleaned
    if (m_data.outputSize != 0 && m_data.inputSize != 0)
        lblIAverComp->setText(QString::number(100-((qreal)m_data.outputSize/m_data.inputSize)*100,
                                              'f', 2) + "%");
    lblIMaxCompress->setText(QString::number(100-m_data.compressMin, 'f', 2) + "%");
    lblIMinCompress->setText(QString::number(100-m_data.compressMax, 'f', 2) + "%");

    // time
    lblIFullTime->setText(SomeUtils::prepareTime(totalTime.nsecsElapsed()));
    lblIMaxTime->setText(SomeUtils::prepareTime(m_data.timeMax));
    if (files != 0)
        lblIAverageTime->setText(SomeUtils::prepareTime(quint64(m_data.timeFull)/files));
    if (m_data.timeMin != ULONG_MAX)
        lblIMinTime->setText(SomeUtils::prepareTime(m_data.timeMin));
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
    foreach (CleanerThread *cleaner, cleanerList) {
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
    cleanerList.removeOne(cleaner);
    cleaner->thread()->quit();
    cleaner->thread()->wait();
    delete cleaner->thread();
    delete cleaner;
}

void MainWindow::removeThumbs()
{
    QLayoutItem *item;
    while ((item = itemLayout->takeAt(0)) != 0)
        item->widget()->deleteLater();
}

void MainWindow::enableButtons(bool value)
{
    actionStart->setVisible(value);
    actionPause->setVisible(!value);
    actionStop->setEnabled(!value);
    actionWizard->setEnabled(value);
    actionInfo->setEnabled(value);
    cmbSort->setEnabled(value);
    progressBar->setVisible(!value);
}

void MainWindow::on_itemsScroll_valueChanged(int value)
{
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(value++), actionCompareView->isChecked());
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

    int i = itemsScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++), actionCompareView->isChecked());
    Settings settings;
    settings.setValue(SettingKey::GUI::CompareView, actionCompareView->isChecked());
}

void MainWindow::on_actionInfo_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

bool MainWindow::customSort(const SVGInfo &s1, const SVGInfo &s2)
{
    if (m_sortType == 0)
        return s1.outPath.toLower() < s2.outPath.toLower();
    else if (m_sortType == 1)
        return s1.outSize < s2.outSize;
    else if (m_sortType == 2)
        return s1.compress < s2.compress;
    else if (m_sortType == 3)
        return s1.attrFinal < s2.attrFinal;
    else if (m_sortType == 4)
        return s1.elemFinal < s2.elemFinal;
    else if (m_sortType == 5)
        return s1.time < s2.time;
    return s1.outPath.toLower() < s2.outPath.toLower();
}

void MainWindow::sortingChanged(int value)
{
    if (itemList.isEmpty())
        return;
    m_sortType = value;
    qSort(itemList.begin(), itemList.end(), &MainWindow::customSort);
    int i = itemsScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++), actionCompareView->isChecked());
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == scrollArea) {
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

void MainWindow::resizeEvent(QResizeEvent *)
{
    if (itemList.isEmpty())
        return;

    if (scrollAreaWidgetContents->height() > scrollArea->height()) {
        // 2 - because the last widget is spacer
        QWidget *w = itemLayout->takeAt(itemLayout->count()-2)->widget();
        itemLayout->removeWidget(w);
        w->deleteLater();
    } else {
        int available = scrollArea->height()/itemLayout->itemAt(0)->geometry().height();
        if (available >= itemLayout->count() && itemList.count() > available) {
            // NOTE: this is a slow method, but it works
            QList<ThumbWidget *> list = findChildren<ThumbWidget *>();
            itemLayout->insertWidget(itemLayout->count()-1,
                                     new ThumbWidget(itemList.at(list.count()),
                                                     actionCompareView->isChecked()));
        }
    }
    itemsScroll->setMaximum(itemList.count()-itemLayout->count()+1);
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

    if (paths.isEmpty())
        return;

    WizardDialog wizard;
    wizard.setPathList(paths);
    if (wizard.exec()) {
        m_threadData = wizard.threadData();
        actionStart->setEnabled(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!cleanerList.isEmpty()) {
        int ans = QMessageBox::question(this, "SVG Cleaner",
                                        tr("Cleaning is not finished.\nDid you really want to exit?"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            foreach (CleanerThread *cleaner, cleanerList) {
                cleaner->forceStop();
                removeCleaner(cleaner);
            }
        } else {
            event->ignore();
        }
    }
    Settings().setValue(SettingKey::GUI::MainSize, size());
}
