#ifndef SOMEUTILS_H
#define SOMEUTILS_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class SomeUtils : public QObject
{
    Q_OBJECT

public:
    explicit SomeUtils(QObject *parent = 0);
    static QString findBinFile(const QString &name);
    static QString prepareSize(const float bytes);
    static QString prepareTime(const float ms);
    static QString genSearchFolderList();
};
#endif // SOMEUTILS_H
