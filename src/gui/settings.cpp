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

#include "settings.h"

namespace SettingKey {
    namespace GUI {
        const QString CompareView = "GUI/CompareView";
        const QString MainSize    = "GUI/MainSize";
        const QString WizardSize  = "GUI/WizardSize";
    }
    namespace Wizard {
        const QString SaveMode         = "Wizard/SaveMode";
        const QString RecursiveScan    = "Wizard/RecursiveScan";
        const QString LastInPaths      = "Wizard/LastInPaths";
        const QString LastOutDir       = "Wizard/LastOutDir";
        const QString Prefix           = "Wizard/Prefix";
        const QString Suffix           = "Wizard/Suffix";
        const QString Compress         = "Wizard/Compress";
        const QString CompressLevel    = "Wizard/CompressLevel";
        const QString CompressType     = "Wizard/CompressType";
        const QString Preset           = "Wizard/Preset";
        const QString ThreadingEnabled = "Wizard/ThreadingEnabled";
        const QString ThreadsCount     = "Wizard/ThreadCount";
        const QString LastKeys         = "Wizard/LastKeys";
    }
}

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
