#ifndef CLEANERTHREAD_H
#define CLEANERTHREAD_H

#include <QObject>
#include <QProcess>
#include <QTime>

#include "arguments.h"

class CleanerThread : public QObject
{
    Q_OBJECT

public:
    explicit CleanerThread(ToThread args, QObject *parent = 0);
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
    ToThread arguments;
    int findValue(const QString &text);
    void unzip(const QString &inPath);
    SVGInfo info();

private slots:
    void finished(int);
    void readyRead();
    void readyReadError();
};

#endif // CLEANERTHREAD_H
