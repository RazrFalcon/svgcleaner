#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QSettings>

#include "cleanerthread.h"
#include "arguments.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    float compressMax;
    float compressMin;
    int timeMax;
    int timeMin;
    int inputSize;
    int outputSize;
    int position;
    QList<CleanerThread *> cleanerList;
    QList<SVGInfo> itemList;
    QSettings *settings;
    QTime time;
    ToThread arguments;

    void enableButtons(bool value);
    void removeThumbs();
    void createStatistics();
    void killThreads();

private slots:
    void errorHandler(const QString &text);
    void cleaningFinished();
    void on_actionWizard_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_itemScroll_valueChanged(int value);
    void prepareStart();
    void progress(SVGInfo);
    void threadsCountChanged();
    void on_actionCompareView_triggered();
    void on_actionInfo_triggered();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
