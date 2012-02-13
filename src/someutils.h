#ifndef SOMEUTILS_H
#define SOMEUTILS_H

#include <QObject>
#include <QStringList>

class SomeUtils : public QObject
{
    Q_OBJECT

public:
    explicit SomeUtils(QObject *parent = 0);
    static QString findFile(const QString &name, const QString &defaultFolder);
    static QString prepareSize(const float bytes);
    static QString prepareTime(const float ms);
};
#endif // SOMEUTILS_H
