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
        static const QString CompareView = "GUI/CompareView";
    }
    namespace Wizard {
        static const QString SaveMode = "Wizard/SaveMode";
        static const QString RecursiveScan = "Wizard/RecursiveScan";
        static const QString LastInPaths = "Wizard/LastInPaths";
        static const QString LastOutDir = "Wizard/LastOutDir";
        static const QString Prefix = "Wizard/Prefix";
        static const QString Suffix = "Wizard/Suffix";
        static const QString Compress = "Wizard/Compress";
        static const QString CompressLevel = "Wizard/CompressLevel";
        static const QString CompressType = "Wizard/CompressType";
        static const QString Preset = "Wizard/Preset";
        static const QString ThreadingEnabled = "Wizard/ThreadingEnabled";
        static const QString ThreadsCount = "Wizard/ThreadCount";
        static const QString LastKeys = "Wizard/LastKeys";
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
