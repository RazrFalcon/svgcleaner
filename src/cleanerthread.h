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

private:
    QProcess *proc;
    QString currentIn;
    QString currentOut;
    QString scriptOutput;
    QString outSVG;
    QTime cleaningTime;
    QTimer *cleaningTimer;
    ToThread arguments;

    bool validXML(const QString &file);
    int findValue(const QString &text);
    QString removeAttribute(const QString &file);
    SVGInfo info();

private slots:
    void readyRead();
    void finished(int);
};

#endif // CLEANERTHREAD_H
