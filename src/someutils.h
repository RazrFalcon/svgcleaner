#ifndef SOMEUTILS_H
#define SOMEUTILS_H

#include <QObject>
#include <QStringList>

class SomeUtils : public QObject
{
    Q_OBJECT

public:
    explicit SomeUtils(QObject *parent = 0);
    QString findFile(QString name, QString defaultFolder);
    QString prepareSize(int bytes);
    QString prepareTime(int ms);
    int getTotalSize(const QStringList &list, int count = -1);
};

#endif // SOMEUTILS_H
