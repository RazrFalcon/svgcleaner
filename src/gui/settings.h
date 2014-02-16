/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QSettings>

namespace SettingKey {
    namespace GUI {
        extern const QString CompareView;
        extern const QString MainSize;
        extern const QString WizardSize;
    }
    namespace Wizard {
        extern const QString SaveMode;
        extern const QString RecursiveScan;
        extern const QString LastInPaths;
        extern const QString LastOutDir;
        extern const QString Prefix;
        extern const QString Suffix;
        extern const QString Compress;
        extern const QString CompressLevel;
        extern const QString CompressType;
        extern const QString Preset;
        extern const QString ThreadingEnabled;
        extern const QString ThreadsCount;
        extern const QString LastKeys;
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
