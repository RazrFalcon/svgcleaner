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

#include <QtGui/QPixmapCache>

#include "someutils.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationVersion("0.6.2");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // load translation for SVG Cleaner
    QString locale = QLocale::system().name();
#ifdef Q_OS_LINUX
    app.addLibraryPath("/usr/share/svgcleaner/translations");
#endif

    QTranslator translator;
#ifdef Q_OS_MAC
    if (translator.load("svgcleaner_" + locale, "../translations"))
        app.installTranslator(&translator);
#else
    if (translator.load("svgcleaner_" + locale, "translations"))
        app.installTranslator(&translator);
#endif

    // load translation for Qt
    QTranslator qtTranslator;
#ifdef Q_OS_WIN
    if (qtTranslator.load("qt_" + locale, "translations"))
        app.installTranslator(&qtTranslator);
#elif defined(Q_OS_MAC)
    if (qtTranslator.load("qt_" + locale, "../translations"))
        app.installTranslator(&qtTranslator);
#else
    if (qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);
#endif

    QPixmapCache::setCacheLimit(512000);

    MainWindow w;
    w.show();

    return app.exec();
}
