#ifndef CLEANERTHREAD_H
#define CLEANERTHREAD_H

#include <QObject>
#include <QProcess>
#include <QTime>
#include <QTimer>

#include "arguments.h"

class CleanerThread : public QObject
{
    Q_OBJECT

public:
    explicit CleanerThread(ToThread args, QObject *parent = 0);
    ~CleanerThread();
    void startNext(const QString &inFile,const QString &outFile);

signals:
    void cleaned(const SVGInfo);
    void criticalError(QString);

private:
    QProcess *proc;
    QString currentIn;
    QString currentOut;
    QString outSVG;
    QString scriptOutput;
    QTime cleaningTime;
    QTimer *cleaningTimer;
    ToThread arguments;

    int findValue(const QString &text);
    QString prepareFile(const QString &file);
    SVGInfo info();

private slots:
    void finished(int);
    void readyRead();
    void readyReadError();
};

#endif // CLEANERTHREAD_H
