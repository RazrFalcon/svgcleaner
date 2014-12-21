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

#include <QDir>

#ifdef USE_IPC
#include <QSharedMemory>
#include <QBuffer>
#include "../3rdparty/systemsemaphore/systemsemaphore.h"
#endif

#include "remover.h"
#include "replacer.h"

void printLine(int keyId, const QString &desc = QString())
{
    if (desc.isEmpty())
        qDebug("  %s %s", qPrintable(Keys.keyName(keyId).leftJustified(35, ' ')),
                          qPrintable(Keys.description(keyId)));
    else
        qDebug("  %s %s", qPrintable(Keys.keyName(keyId).leftJustified(35, ' ')), qPrintable(desc));
}

void printLine(const QString &key, const QString &desc)
{
    qDebug("  %s %s", qPrintable(key.leftJustified(35, ' ')), qPrintable(desc));
}

void showPresetInfo(const QString &presetName)
{
    QList<int> list;
    if (presetName.endsWith(Preset::Basic)) {
        Keys.setPreset(Preset::Basic);
        list = Keys.basicPresetKeys();
    } else if (presetName.endsWith(Preset::Complete)) {
        Keys.setPreset(Preset::Complete);
        list = Keys.completePresetKeys();
    } else if (presetName.endsWith(Preset::Extreme)) {
        Keys.setPreset(Preset::Extreme);
        list = Keys.extremePresetKeys();
    }
    foreach (const int &key, list) {
        if (   key == Key::TransformPrecision
            || key == Key::AttributesPrecision
            || key == Key::CoordsPrecision) {
            qDebug() << Keys.keyName(key) + "=" + QString::number(Keys.intNumber(key));
        } else if (key == Key::RemoveTinyGaussianBlur) {
            qDebug() << Keys.keyName(key) + "=" + QString::number(Keys.doubleNumber(key));
        } else {
            qDebug() << Keys.keyName(key);
        }
    }
}

void showHelp()
{
    Keys.prepareDescription();

    qDebug() << "SVG Cleaner could help you to clean up your SVG files from unnecessary data.";
    qDebug() << "";
    qDebug() << "Usage:";
    qDebug() << "  svgcleaner-cli <in-file> <out-file> [--preset=] [--options]";
    qDebug() << "Show options included in preset:";
    qDebug() << "  svgcleaner-cli --info --preset=<name>";
    qDebug() << "";
    qDebug() << "Presets:";
    printLine("--preset=basic",    "Basic cleaning");
    printLine("--preset=complete", "Complete cleaning [default]");
    printLine("--preset=extreme",  "Extreme cleaning");
    qDebug() << "";
    qDebug() << "Options:";
    qDebug() << "";
    printLine("-h --help", "Show this text");
    printLine("-v --version", "Show version");
    qDebug() << "";

    qDebug() << "Elements:";
    foreach (const int &key, Keys.elementsKeysId()) {
        if (key == Key::RemoveTinyGaussianBlur)
            printLine(Keys.keyName(key) + "=<0..1.0>",
                      Keys.description(key) + QString(" [default: %1]")
                        .arg(Keys.doubleNumber(Key::RemoveTinyGaussianBlur)));
        else
            printLine(key);
    }
    qDebug() << "";
    qDebug() << "Attributes:";
    foreach (const int &key, Keys.attributesKeysId())
        printLine(key);
    qDebug() << "Additional:";
    foreach (const int &key, Keys.attributesUtilsKeysId())
        printLine(key);
    qDebug() << "";
    qDebug() << "Paths:";
    foreach (const int &key, Keys.pathsKeysId())
        printLine(key, Keys.description(key));
    qDebug() << "";
    qDebug() << "Optimizations:";
    foreach (const int &key, Keys.optimizationsKeysId()) {
        if (   key == Key::TransformPrecision
            || key == Key::AttributesPrecision
            || key == Key::CoordsPrecision) {
            printLine(Keys.keyName(key) + "=<1..8>",
                      Keys.description(key) + QString(" [default: %1]").arg(Keys.doubleNumber(key)));
        } else
            printLine(key);
    }
    qDebug() << "Additional:";
    foreach (const int &key, Keys.optimizationsUtilsKeysId())
        printLine(key);
}

void processFile(const QString &inPath, const QString &outPath)
{
    if (!Keys.flag(Key::ShortOutput))
        qDebug("The initial file size is: %u", (int)QFile(inPath).size());

    SvgDocument doc;
    bool flag = doc.loadFile(inPath);
    if (!flag)
        qFatal("%s", qPrintable(doc.lastError()));
    if (BaseCleaner::svgElement(doc).isNull())
        qFatal("Error: invalid svg file");

    Replacer replacer(doc);
    Remover remover(doc);

    replacer.calcElemAttrCount("initial");

    // TODO: fix double clean issues

    // mandatory fixes used to simplify subsequent functions
    replacer.convertEntityData();
    replacer.splitStyleAttributes();
    // TODO: add key
    replacer.convertCDATAStyle();
    replacer.convertUnits();
    replacer.convertColors();
    replacer.prepareDefs();
    replacer.fixWrongAttr();
    replacer.markUsedElements();
    replacer.roundNumericAttributes();

    remover.cleanSvgElementAttribute();
    if (Keys.flag(Key::CreateViewbox))
        replacer.convertSizeToViewbox();
    if (Keys.flag(Key::RemoveUnusedDefs))
        remover.removeUnusedDefs();
    if (Keys.flag(Key::RemoveDuplicatedDefs))
        remover.removeDuplicatedDefs();
    if (Keys.flag(Key::MergeGradients)) {
        replacer.mergeGradients();
        replacer.mergeGradientsWithEqualStopElem();
    }
    remover.removeElements();
    remover.removeAttributes();
    remover.removeElementsFinal();
    if (Keys.flag(Key::RemoveUnreferencedIds))
        remover.removeUnreferencedIds();
    if (Keys.flag(Key::RemoveUnusedXLinks))
        remover.removeUnusedXLinks();
    remover.cleanPresentationAttributes();
    if (Keys.flag(Key::ApplyTransformsToShapes))
        replacer.applyTransformToShapes();
    if (Keys.flag(Key::RemoveOutsideElements))
        replacer.calcElementsBoundingBox();
    if (Keys.flag(Key::ConvertBasicShapes))
        replacer.convertBasicShapes();
    if (Keys.flag(Key::UngroupContainers)) {
        remover.ungroupAElement();
        remover.ungroupSwitchElement();
        remover.removeGroups();
    }
    replacer.processPaths();
    if (Keys.flag(Key::RemoveOutsideElements))
        remover.removeElementsOutsideTheViewbox();
    if (Keys.flag(Key::ReplaceEqualEltsByUse))
        replacer.replaceEqualElementsByUse();
    if (Keys.flag(Key::RemoveNotAppliedAttributes))
        replacer.moveStyleFromUsedElemToUse();
    if (Keys.flag(Key::GroupTextStyles))
        replacer.groupTextElementsStyles();
    if (Keys.flag(Key::GroupElemByStyle))
        replacer.groupElementsByStyles();
    if (Keys.flag(Key::ApplyTransformsToDefs))
        replacer.applyTransformToDefs();
    if (Keys.flag(Key::TrimIds))
        replacer.trimIds();
    remover.checkXlinkDeclaration();
    if (Keys.flag(Key::SortDefs))
        replacer.sortDefs();
    replacer.finalFixes();
    if (Keys.flag(Key::JoinStyleAttributes))
        replacer.joinStyleAttr();

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("Error: could not write output file");

    int indent = 1;
    if (Keys.flag(Key::CompactOutput))
        indent = -1;
    outFile.write(doc.toString(indent).toUtf8());
    outFile.close();

    if (!Keys.flag(Key::ShortOutput))
        qDebug("The final file size is: %u", (int)QFile(outPath).size());

    replacer.calcElemAttrCount("final");
}

// the code underneath is from QtCore module (qcorecmdlineargs_p.h) (LGPLv2 license)
#ifdef Q_OS_WIN
#include "windows.h"
template<typename Char>
static QVector<Char*> qWinCmdLine(Char *cmdParam, int length, int &argc)
{
    QVector<Char*> argv(8);
    Char *p = cmdParam;
    Char *p_end = p + length;

    argc = 0;

    while (*p && p < p_end) {
        while (QChar((short)(*p)).isSpace())
            p++;
        if (*p && p < p_end) {
            int quote;
            Char *start, *r;
            if (*p == Char('\"') || *p == Char('\'')) {
                quote = *p;
                start = ++p;
            } else {
                quote = 0;
                start = p;
            }
            r = start;
            while (*p && p < p_end) {
                if (quote) {
                    if (*p == quote) {
                        p++;
                        if (QChar((short)(*p)).isSpace())
                            break;
                        quote = 0;
                    }
                }
                if (*p == '\\') {
                    if (*(p+1) == quote)
                        p++;
                } else {
                    if (!quote && (*p == Char('\"') || *p == Char('\''))) {
                        quote = *p++;
                        continue;
                    } else if (QChar((short)(*p)).isSpace() && !quote)
                        break;
                }
                if (*p)
                    *r++ = *p++;
            }
            if (*p && p < p_end)
                p++;
            *r = Char('\0');

            if (argc >= (int)argv.size()-1)
                argv.resize(argv.size()*2);
            argv[argc++] = start;
        }
    }
    argv[argc] = 0;

    return argv;
}

QStringList arguments(int &argc, char **argv)
{
    Q_UNUSED(argv);
    QStringList list;
    argc = 0;
    QString cmdLine = QString::fromWCharArray(GetCommandLine());
    QVector<wchar_t*> argvVec = qWinCmdLine<wchar_t>((wchar_t *)cmdLine.utf16(),
                                                     cmdLine.length(), argc);
    for (int a = 0; a < argc; ++a)
        list << QString::fromWCharArray(argvVec[a]);
    return list;
}
#else
QStringList arguments(int &argc, char **argv)
{
    QStringList list;
    const int ac = argc;
    char ** const av = argv;
    for (int a = 0; a < ac; ++a)
        list << QString::fromLocal8Bit(av[a]);
    return list;
}
#endif

#ifdef USE_IPC
SystemSemaphore *semaphore2 = 0;
QString appLog;
void slaveMessageOutput(QtMsgType type, const char *msg)
{
    appLog += QString(msg) + "\n";
    if (type == QtFatalMsg) {
        // emit to 'gui' that we crashed
        // crash will be detected by timeout anyway, but this way is much faster
        if (semaphore2)
            semaphore2->release();
        exit(1);
    }
}
#endif

void ownMessageOutput(QtMsgType type, const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    if (type == QtFatalMsg)
        exit(1);
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_UNIX
    setlocale(LC_ALL, "");
#endif

    QStringList argList = arguments(argc, argv);
    // remove executable path
    argList.removeFirst();

    if (argList.contains(QL1S("-v")) || argList.contains(QL1S("--version"))) {
        qDebug() << "0.7.0";
        return 0;
    }

    if (argList.contains(QL1S("-h")) || argList.contains(QL1S("--help")) || argList.size() < 2) {
        showHelp();
        return 0;
    }

    if (argList.contains(QL1S("--info")) && argList.size() == 2) {
        showPresetInfo(argList.at(1));
        return 0;
    }

#ifdef USE_IPC
    if (argList.first() == QL1S("--slave")) {
        argList.removeFirst();
        QString id = argList.takeFirst();
        QSharedMemory sharedMemory("SvgCleanerMem_" + id);
        SystemSemaphore semaphore1("SvgCleanerSemaphore1_" + id);
        semaphore2 = new SystemSemaphore("SvgCleanerSemaphore2_" + id);

        if (!sharedMemory.attach())
            qFatal("Error: unable to attach to shared memory segment.");

        qInstallMsgHandler(slaveMessageOutput);

        Keys.parseOptions(argList);

        // emit to 'gui' that 'cli' ready to clean files
        semaphore2->release();

        while (true) {
            // wait while 'gui' set paths to shared memory
            if (!semaphore1.acquire())
                break;

            // read shared memory
            QBuffer buffer;
            QDataStream in(&buffer);
            QString inFile;
            QString outFile;
            buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
            buffer.open(QBuffer::ReadWrite);
            in >> inFile;
            in >> outFile;

            // if both paths is empty - this is signal to stop 'cli'
            if (inFile.isEmpty() && outFile.isEmpty()) {
                semaphore2->release();
                break;
            }

            appLog.clear();

            // clean svg
            processFile(inFile, outFile);

            // write to shared memory
            buffer.seek(0);
            QDataStream out(&buffer);
            out << appLog;
            int size = buffer.size();
            char *to = (char*)sharedMemory.data();
            const char *from = buffer.data().data();
            memcpy(to, from, qMin(sharedMemory.size(), size));

            // emit to 'gui' that file was cleaned
            semaphore2->release();
        }
    }
    else
#endif
    {
        qInstallMsgHandler(ownMessageOutput);

        QString inputFile  = argList.takeFirst();
        QString outputFile = argList.takeFirst();

        if (!QFile(inputFile).exists())
            qFatal("Error: input file does not exist");
        if (!QFileInfo(outputFile).absoluteDir().exists())
            qFatal("Error: output folder does not exist");

        Keys.parseOptions(argList);
        processFile(inputFile, outputFile);
    }

    return 0;
}
