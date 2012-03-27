#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <QtCore/QObject>
#include <QtCore/QFileInfoList>

class FileFinder : public QObject
{
    Q_OBJECT

public:
    explicit FileFinder(QObject *parent = 0);

public slots:
    void startSearch(const QString &startDir, bool recursive);
    void stopSearch();
    
private:
    bool stop;
    QFileInfoList searchForFiles(const QString &startDir, bool recursive);

signals:
    void finished(QFileInfoList);
};

#endif // FILEFINDER_H
