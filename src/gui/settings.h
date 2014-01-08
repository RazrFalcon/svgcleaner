#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QSettings>

namespace SettingKey {
    namespace GUI {
        static const QString CompareView = "GUI/CompareView";
    }
    namespace Wizard {
        static const QString SaveMode = "Wizard/SaveMode";
        static const QString RecursiveScan = "Wizard/RecursiveScan";
        static const QString LastInDir = "Wizard/LastInDir";
        static const QString LastOutDir = "Wizard/LastOutDir";
        static const QString Prefix = "Wizard/Prefix";
        static const QString Suffix = "Wizard/Suffix";
        static const QString Compress = "Wizard/Compress";
        static const QString CompressLevel = "Wizard/CompressLevel";
        static const QString CompressType = "Wizard/CompressType";
        static const QString Preset = "Wizard/Preset";
        static const QString ThreadingEnabled = "Wizard/ThreadingEnabled";
        static const QString ThreadsCount = "Wizard/ThreadCount";
    }
}

class Settings : public QSettings
{
    Q_OBJECT

public:
    explicit Settings(QObject *parent = 0);
    bool flag(const QString &key, bool defValue = true);
    int integer(const QString &key, int defValue = -1);
    QByteArray array(const QString &key);
    QString string(const QString &key, const QString &defValue = QString());
};

#endif // SETTINGS_H
