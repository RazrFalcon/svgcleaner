#include <QtCore/QThread>
#include <QtCore/QtDebug>

#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QShortcut>
#include <QtGui/QWheelEvent>

#include "aboutdialog.h"
#include "cleanerthread.h"
#include "someutils.h"
#include "thumbwidget.h"
#include "wizarddialog.h"
#include "mainwindow.h"

// FIXME: processing files like image.svg and image.svgz from different threads causes problems

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    qRegisterMetaType<SVGInfo>("SVGInfo");

    // setup GUI
    scrollArea->installEventFilter(this);
    progressBar->hide();
    itemScroll->hide();
    actionPause->setVisible(false);

    // load settings
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             "svgcleaner", "config");

    // create sorting combobox
    QWidget *w = new QWidget(toolBar);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(w);
    cmbSort = new QComboBox();
    cmbSort->addItem(tr("Sort by name"));
    cmbSort->addItem(tr("Sort by size"));
    cmbSort->addItem(tr("Sort by compression"));
    cmbSort->addItem(tr("Sort by attributes"));
    cmbSort->addItem(tr("Sort by elements"));
    cmbSort->addItem(tr("Sort by time"));
    cmbSort->setMinimumHeight(toolBar->height());
    cmbSort->setEnabled(false);
    connect(cmbSort, SIGNAL(currentIndexChanged(int)), this, SLOT(sortingChanged(int)));
    toolBar->addWidget(cmbSort);

    QString suffix = "<br><span style=\"color:#808080;\">%1</span>";
    actionWizard->setToolTip(tr("Open the wizard")+suffix.arg("Ctrl+W"));
    actionStart->setToolTip(tr("Start processing")+suffix.arg("Ctrl+R"));
    actionPause->setToolTip(tr("Pause processing")+suffix.arg("Ctrl+R"));
    actionStop->setToolTip(tr("Stop cleaning")+suffix.arg("Ctrl+S"));

    actionCompareView->setChecked(settings->value("CompareView", true).toBool());
    on_actionCompareView_triggered();

    // setup shortcuts
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit,this);
    connect(quitShortcut, SIGNAL(activated()), qApp, SLOT(quit()));

    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowIcon(QIcon(":/svgcleaner.svgz"));
    resize(1000, 650);
}

void MainWindow::on_actionWizard_triggered()
{
    WizardDialog wizard;
    if (wizard.exec()) {
        arguments = wizard.threadArguments();
        actionStart->setEnabled(true);
    }
}

void MainWindow::on_actionStart_triggered()
{
    if (!actionStart->isEnabled())
        return;

    if (itemList.isEmpty() || !actionStop->isEnabled()) {
        time = QTime::currentTime();
        time.start();
        prepareStart();
    }

    if (!actionPause->isVisible()) {
        enableButtons(false);
        actionPause->setVisible(true);
        actionStart->setVisible(false);
    }

    if (!arguments.args.isEmpty()) {
        qDebug() << "with keys:";
        foreach (QString key, arguments.args)
            qDebug() << key;
    }

    int threadCount = 1;
    if (settings->value("Wizard/ThreadingEnabled",true).toBool()) {
        threadCount = settings->value("Wizard/ThreadCount", QThread::idealThreadCount()).toInt();
        if (threadCount > arguments.inputFiles.count())
            threadCount = arguments.inputFiles.count();
    }

    for (int i = 0; i < threadCount; ++i) {
        QThread *thread = new QThread(this);
        CleanerThread *cleaner = new CleanerThread(arguments);
        cleanerList.append(cleaner);

        connect(cleaner, SIGNAL(cleaned(SVGInfo)),
                this, SLOT(progress(SVGInfo)),Qt::QueuedConnection);
        cleaner->moveToThread(thread);
        cleaner->startNext(arguments.inputFiles.at(position), arguments.outputFiles.at(position));
        thread->start();
        position++;
    }
}

void MainWindow::prepareStart()
{
    position = 0;
    compressMax = 0;
    compressMin = 99;
    timeMax = 0;
    timeMin = 999999999;
    timeFull = 0;
    inputSize = 0;
    outputSize = 0;
    removeThumbs();
    enableButtons(false);
    itemScroll->hide();
    itemScroll->setMaximum(0);
    itemScroll->setValue(0);
    itemList.clear();
    progressBar->setValue(0);
    progressBar->setMaximum(arguments.inputFiles.count());
    itemLayout->addStretch(100);
    cmbSort->setCurrentIndex(0);

    foreach (QLabel *lbl, gBoxSize->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0");
    foreach (QLabel *lbl, gBoxCompression->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("0%");
    foreach (QLabel *lbl, gBoxTime->findChildren<QLabel *>(QRegExp("^lblI.*")))
        lbl->setText("000ms");
    lblITotalFiles->setText(QString::number(arguments.outputFiles.count()));
}

void MainWindow::progress(SVGInfo info)
{
    progressBar->setValue(progressBar->value()+1);
    startNext();

    itemList.append(info);
    if (!info.errString.isEmpty())
        lblICrashed->setText(QString::number(lblICrashed->text().toInt()+1));
    else {
        inputSize  += info.inSize;
        outputSize += info.outSize;
        if (info.compress > compressMax && info.compress < 100) compressMax = info.compress;
        if (info.compress < compressMin && info.compress > 0)   compressMin = info.compress;
        if (info.time > timeMax) timeMax = info.time;
        if (info.time < timeMin) timeMin = info.time;
        timeFull += info.time;
    }

    int available = scrollArea->height()/itemLayout->itemAt(0)->geometry().height();
    if (available >= itemLayout->count() || itemLayout->isEmpty()) {
        itemLayout->insertWidget(itemLayout->count()-1, new ThumbWidget(info,
                                                                actionCompareView->isChecked()));
    } else {
        itemScroll->show();
        itemScroll->setMaximum(itemScroll->maximum()+1);
//        if (itemScroll->value() == itemScroll->maximum()-1)
//            itemScroll->setValue(itemScroll->value()+1);
    }
    createStatistics();
}

void MainWindow::startNext()
{
    CleanerThread *cleaner = qobject_cast<CleanerThread *>(sender());
    if (position < arguments.inputFiles.count()
            && actionPause->isVisible() && actionStop->isEnabled()) {
        QMetaObject::invokeMethod(cleaner, "startNext", Qt::QueuedConnection,
                                  Q_ARG(QString, arguments.inputFiles.at(position)),
                                  Q_ARG(QString, arguments.outputFiles.at(position)));
        position++;
    } else if (progressBar->value() == progressBar->maximum()) {
        cleaningFinished();
    } else if (!actionStop->isEnabled() || !actionPause->isVisible()) {
        cleaner->thread()->quit();
        cleaner->thread()->wait();
        cleaner->thread()->deleteLater();
    }
}

void MainWindow::on_actionPause_triggered()
{
    actionPause->setVisible(false);
    actionStart->setVisible(true);
}

void MainWindow::createStatistics()
{
    // files
    lblICleaned->setText(QString::number(progressBar->value()-lblICrashed->text().toInt()));
    lblITotalSizeBefore->setText(SomeUtils::prepareSize(inputSize));
    lblITotalSizeAfter->setText(SomeUtils::prepareSize(outputSize));

    // cleaned
    if (outputSize != 0 && inputSize != 0) // FIXME: change to .args
        lblIAverComp->setText(QString::number((outputSize/inputSize)*100, 'f', 2) + "%");
    lblIMaxCompress->setText(QString::number(100-compressMin, 'f', 2) + "%");
    lblIMinCompress->setText(QString::number(100-compressMax, 'f', 2) + "%");

    // time
    int fullTime = time.elapsed();
    lblIFullTime->setText(SomeUtils::prepareTime(fullTime));
    lblIMaxTime->setText(SomeUtils::prepareTime(timeMax));
    if (lblICleaned->text().toInt() != 0)
        lblIAverageTime->setText(SomeUtils::prepareTime(timeFull/lblICleaned->text().toInt()));
    if (timeMin != 999999999)
        lblIMinTime->setText(SomeUtils::prepareTime(timeMin));
}

void MainWindow::on_actionStop_triggered()
{
    if (!actionStop->isEnabled())
        return;
    actionPause->setVisible(false);
    enableButtons(true);
}

void MainWindow::cleaningFinished()
{
    deleteThreads();
    enableButtons(true);
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

void MainWindow::deleteThreads()
{
    foreach (QThread *th, findChildren<QThread *>()) {
        th->quit();
        th->wait();
        th->deleteLater();
    }
}

void MainWindow::on_itemScroll_valueChanged(int value)
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

    int i = itemScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++), actionCompareView->isChecked());
    settings->setValue("CompareView",  actionCompareView->isChecked());
}

void MainWindow::on_actionInfo_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

int sortValue;
bool customSort(SVGInfo &s1, SVGInfo &s2)
{
    if (sortValue == 0)
        return s1.outPath.toLower() < s2.outPath.toLower();
    else if (sortValue == 1)
        return s1.outSize < s2.outSize;
    else if (sortValue == 2)
        return s1.compress > s2.compress;
    else if (sortValue == 3)
        return s1.attrFinal < s2.attrFinal;
    else if (sortValue == 4)
        return s1.elemFinal < s2.elemFinal;
    else if (sortValue == 5)
        return s1.time < s2.time;
    return s1.outPath.toLower() < s2.outPath.toLower();
}

void MainWindow::sortingChanged(int value)
{
    if (itemList.isEmpty())
        return;
    sortValue = value;
    qSort(itemList.begin(),itemList.end(),customSort);
    int i = itemScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++), actionCompareView->isChecked());
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == scrollArea && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->delta() > 0)
            itemScroll->setValue(itemScroll->value()-1);
        else
            itemScroll->setValue(itemScroll->value()+1);
        return true;
    }
    if (obj == scrollArea && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_End)
            itemScroll->setValue(itemScroll->maximum());
        else if (keyEvent->key() == Qt::Key_Home)
            itemScroll->setValue(0);
        return true;
    }
    return false;
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
    itemScroll->setMaximum(itemList.count()-itemLayout->count()+1);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    deleteThreads();
}
