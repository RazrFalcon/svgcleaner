#include <QApplication>

#include "settings.h"

Settings::Settings(QObject *parent) :
    QSettings(QSettings::IniFormat, QSettings::UserScope, "svgcleaner", "config", parent)
{
}

bool Settings::flag(const QString &key, bool defValue)
{
    return value(key, defValue).toBool();
}

int Settings::integer(const QString &key, int defValue)
{
    return value(key, defValue).toInt();
}

QByteArray Settings::array(const QString &key)
{
    return value(key).toByteArray();
}

QString Settings::string(const QString &key, const QString &defValue)
{
    return value(key, defValue).toString();
}
