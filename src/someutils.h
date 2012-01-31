#ifndef SOMEUTILS_H
#define SOMEUTILS_H

#include <QObject>
#include <QStringList>

class SomeUtils : public QObject
{
    Q_OBJECT

public:
    explicit SomeUtils(QObject *parent = 0);
    static QString findFile(QString name, QString defaultFolder);
    static QString prepareSize(int bytes);
    static QString prepareTime(int ms);
};
#endif // SOMEUTILS_H
