#ifndef CLEANERTHREAD_H
#define CLEANERTHREAD_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QTime>

#include "arguments.h"

class CleanerThread : public QObject
{
    Q_OBJECT

public:
    explicit CleanerThread(ToThread args, QObject *parent = 0);

public slots:
    void startNext(const QString &inPath, const QString &outPath);

signals:
    void cleaned(const SVGInfo);
//    void criticalError(QString);

private:
    QProcess *proc;
    QString currentIn;
    QString currentOut;
    QString outSVG;
    QString scriptOutput;
//    QString scriptErrors;
    QTime cleaningTime;
    ToThread arguments;
    int findValue(const QString &text);
    void unzip(const QString &inPath);
    SVGInfo info();

private slots:
    void finished(int);
    void readyRead();
//    void readyReadError();
};

#endif // CLEANERTHREAD_H
