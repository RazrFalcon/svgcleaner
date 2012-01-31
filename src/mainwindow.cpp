#include <QWheelEvent>
#include <QKeyEvent>
#include <QShortcut>
#include <QThread>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>

#include "aboutdialog.h"
#include "cleanerthread.h"
#include "someutils.h"
#include "thumbwidget.h"
#include "wizarddialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    qRegisterMetaType<SVGInfo>("SVGInfo");

//    static const QString foo;
    const QString foo = QT_TR_NOOP("foo");

    // setup GUI
    actionWizard->setIcon(QIcon(":/wizard.svgz"));
    actionStart->setIcon(QIcon(":/start.svgz"));
    actionStop->setIcon(QIcon(":/stop.svgz"));
    actionThreads->setIcon(QIcon(":/cpu.svgz"));
    actionInfo->setIcon(QIcon(":/information.svgz"));
    scrollArea->installEventFilter(this);
    progressBar->hide();
    itemScroll->hide();

    // load settings
    settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                             "svgcleaner", "config");

    // create sorting combobox
    QWidget *w = new QWidget();
    w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    toolBar->addWidget(w);
    cmbSort = new QComboBox();
    cmbSort->addItem(tr("Sort by name"));
    cmbSort->addItem(tr("Sort by size"));
    cmbSort->addItem(tr("Sort by compression"));
    cmbSort->addItem(tr("Sort by attributes"));
    cmbSort->addItem(tr("Sort by elements"));
    cmbSort->addItem(tr("Sort by time"));
    cmbSort->setMinimumHeight(toolBar->height());
    connect(cmbSort,SIGNAL(currentIndexChanged(int)),this,SLOT(sortingChanged(int)));
    toolBar->addWidget(cmbSort);

    // setup threads menu
    int threadCount = settings->value("threadCount",QThread::idealThreadCount()).toInt();
    QMenu *menu = new QMenu(this);
    QActionGroup *group = new QActionGroup(actionThreads);
    for (int i = 1; i < QThread::idealThreadCount()+1; ++i) {
        QAction *action = new QAction(QString::number(i),group);
        action->setCheckable(true);
        connect(action,SIGNAL(triggered()),this,SLOT(threadsCountChanged()));
        if (i == threadCount)
            action->setChecked(true);
    }
    menu->addActions(group->actions());
    actionThreads->setMenu(menu);
    actionThreads->setToolTip(tr("Threads selected: ")+QString::number(threadCount));

    actionCompareView->setChecked(settings->value("compareView",true).toBool());
    on_actionCompareView_triggered();

    // setup shortcuts
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit,this);
    connect(quitShortcut,SIGNAL(activated()),qApp,SLOT(quit()));

    setWindowIcon(QIcon(":/svgcleaner.svgz"));
    resize(900,650);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionWizard_triggered()
{
    WizardDialog wizard;
    if (wizard.exec()) {
        arguments = wizard.threadArguments();
        actionStart->setEnabled(true);
    }
    qDebug()<<"cli arguments:"<<arguments.args;
}

void MainWindow::on_actionStart_triggered()
{
//    if (!actionStart->isEnabled())
//        return;

    if (pause) {
        actionStart->setIcon(QIcon(":/start.svgz"));
        pause = true;
        startNext();
        actionStop->setEnabled(false);
    } else {
        actionStart->setIcon(QIcon(":/pause.svgz"));
        pause = false;
    }

    time = QTime::currentTime();
    time.start();

    prepareStart();
    int threadCount = settings->value("threadCount",QThread::idealThreadCount()).toInt();
    if (arguments.inputFiles.count() < threadCount)
        threadCount = arguments.inputFiles.count();

    for (int i = 0; i < threadCount; ++i) {
        QThread *thread = new QThread(this);
        CleanerThread *cleaner = new CleanerThread(arguments);
        cleanerList.append(cleaner);
        connect(cleaner,SIGNAL(cleaned(SVGInfo)),
                this,SLOT(progress(SVGInfo)),Qt::QueuedConnection);
        connect(cleaner,SIGNAL(criticalError(QString)),this,SLOT(errorHandler(QString)));
        cleaner->moveToThread(thread);
        cleaner->startNext(arguments.inputFiles.at(position),arguments.outputFiles.at(position));
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
    enableButtons(false);
    removeThumbs();
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

void MainWindow::removeThumbs()
{
    QLayoutItem *item;
    while ((item = itemLayout->takeAt(0)) != 0)
        item->widget()->deleteLater();
}

void MainWindow::enableButtons(bool value)
{
//    actionStart->setEnabled(value);
    actionStop->setEnabled(!value);
    actionWizard->setEnabled(value);
    actionThreads->setEnabled(value);
    actionInfo->setEnabled(value);
    cmbSort->setEnabled(value);
    progressBar->setVisible(!value);
}

void MainWindow::progress(SVGInfo info)
{
    startNext();

    itemList.append(info);
    if (info.crashed)
        lblICrashed->setText(QString::number(lblICrashed->text().toInt()+1));
    else {
        inputSize += info.sizes[SVGInfo::INPUT];
        outputSize += info.sizes[SVGInfo::OUTPUT];
        if (info.compress > compressMax && info.compress < 100) compressMax = info.compress;
        if (info.compress < compressMin && info.compress > 0)   compressMin = info.compress;
        if (info.time > timeMax) timeMax = info.time;
        if (info.time < timeMin) timeMin = info.time;
        timeFull += info.time;
    }

    int available = scrollArea->height()/itemLayout->itemAt(0)->geometry().height();
    if (available >= itemLayout->count() || itemLayout->isEmpty()) {
        itemLayout->insertWidget(itemLayout->count()-1,
                                 new ThumbWidget(info,actionCompareView->isChecked()));
    } else {
        itemScroll->show();
        itemScroll->setMaximum(itemScroll->maximum()+1);
        if (itemScroll->value() == itemScroll->maximum()-1)
            itemScroll->setValue(itemScroll->value()+1);
    }
    createStatistics();
}

void MainWindow::startNext()
{
    if (pause)
        return;

    progressBar->setValue(progressBar->value()+1);
    CleanerThread *cleaner = qobject_cast<CleanerThread *>(sender());
    if (position < arguments.inputFiles.count()) {
        cleaner->startNext(arguments.inputFiles.at(position),
                           arguments.outputFiles.at(position));
        position++;
    } else {
        if (progressBar->value() == progressBar->maximum())
            cleaningFinished();
    }
}

void MainWindow::createStatistics()
{
    // files
    lblICleaned->setText(QString::number(progressBar->value()-lblICrashed->text().toInt()));
    lblITotalSizeBefore->setText(SomeUtils::prepareSize(inputSize));
    lblITotalSizeAfter->setText(SomeUtils::prepareSize(outputSize));

    // cleaned
    if (outputSize != 0 && inputSize != 0)
        lblIAverComp->setText(QString::number(((float)outputSize/inputSize)*100,'f',2)+"%");
    lblIMaxCompress->setText(QString::number(100-compressMin,'f',2)+"%");
    lblIMinCompress->setText(QString::number(100-compressMax,'f',2)+"%");

    // time
    int fullTime = time.elapsed();
    lblIFullTime->setText(SomeUtils::prepareTime(fullTime));
    lblIMaxTime->setText(SomeUtils::prepareTime(timeMax));
    if (lblICleaned->text().toInt() != 0)
        lblIAverageTime->setText(SomeUtils::prepareTime(timeFull/lblICleaned->text().toInt()));
    if (timeMin != 999999999)
        lblIMinTime->setText(SomeUtils::prepareTime(timeMin));
}

void MainWindow::errorHandler(const QString &text)
{
    killThreads();
    QMessageBox::critical(this,tr("Error"),
                          text+tr("\nProcessing will stop now."),
                          QMessageBox::Ok);
}

void MainWindow::on_actionStop_triggered()
{
    if (!actionStop->isEnabled())
        return;
    killThreads();
}

void MainWindow::cleaningFinished()
{
    killThreads();
}

void MainWindow::killThreads()
{
    foreach (QThread *th, findChildren<QThread *>()) {
        th->quit();
        th->deleteLater();
    }
    foreach (CleanerThread *cleaner, findChildren<CleanerThread *>())
        cleaner->deleteLater();
    enableButtons(true);
}

void MainWindow::threadsCountChanged()
{
    QAction *action = qobject_cast<QAction *>(sender());
    settings->setValue("threadCount",action->text());
    actionThreads->setToolTip(tr("Threads selected: ")+action->text());
}

void MainWindow::on_itemScroll_valueChanged(int value)
{
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(value++),actionCompareView->isChecked());
}

void MainWindow::on_actionCompareView_triggered()
{
    if (actionCompareView->isChecked())
        actionCompareView->setIcon(QIcon(":/thumbs-on.svgz"));
    else
        actionCompareView->setIcon(QIcon(":/thumbs-off.svgz"));

    int i = itemScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++),actionCompareView->isChecked());
    settings->setValue("compareView",actionCompareView->isChecked());
}

void MainWindow::on_actionInfo_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

int sortValue;
bool caseInsensitiveLessThan(SVGInfo &s1, SVGInfo &s2)
{
    if (sortValue == 0)
        return s1.paths.last().toLower() < s2.paths.last().toLower();
    else if (sortValue == 1)
        return s1.sizes.last() < s2.sizes.last();
    else if (sortValue == 2)
        return s1.compress > s2.compress;
    else if (sortValue == 3)
        return s1.attrFinal < s2.attrFinal;
    else if (sortValue == 4)
        return s1.elemFinal < s2.elemFinal;
    else if (sortValue == 5)
        return s1.time < s2.time;
    return s1.paths.last().toLower() < s2.paths.last().toLower();
}

void MainWindow::sortingChanged(int value)
{
    if (itemList.isEmpty())
        return;
    sortValue = value;
    qSort(itemList.begin(),itemList.end(),caseInsensitiveLessThan);
    int i = itemScroll->value();
    foreach (ThumbWidget *item, findChildren<ThumbWidget *>())
        item->refill(itemList.at(i++),actionCompareView->isChecked());
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
            // this is a slow method, but it works
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
    killThreads();
}
