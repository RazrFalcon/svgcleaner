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

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QtDebug>

#include "remover.h"
#include "replacer.h"

void printLine(const int &keyId, const QString &desc = QString())
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

    XMLDocument doc;
    doc.LoadFile(inPath.toUtf8().constData());
    if (BaseCleaner::svgElement(&doc).isNull()) {
        QFile inputFile(inPath);
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
            qFatal("Error: cannot open input file");
        QTextStream inputStream(&inputFile);
        QString svgText = inputStream.readAll();
        svgText.replace("<svg:", "<");
        svgText.replace("</svg:", "</");
        doc.Clear();
        doc.Parse(svgText.toUtf8().constData());
        if (BaseCleaner::svgElement(&doc).isNull())
            qFatal("Error: invalid svg file");
    }

    Replacer replacer(&doc);
    Remover remover(&doc);

    replacer.calcElemAttrCount("initial");

    // mandatory fixes used to simplify subsequent functions
    replacer.splitStyleAttr();
    // TODO: add key
    replacer.convertCDATAStyle();
    replacer.convertUnits();
    replacer.prepareDefs();
    replacer.fixWrongAttr();
    replacer.markUsedElements();

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
        remover.removeGroups();
    }
    replacer.processPaths();
    if (Keys.flag(Key::ReplaceEqualEltsByUse))
        replacer.replaceEqualElementsByUse();
    if (Keys.flag(Key::RemoveNotAppliedAttributes))
        replacer.moveStyleFromUsedElemToUse();
    if (Keys.flag(Key::RemoveOutsideElements))
        remover.removeElementsOutsideTheViewbox();
    if (Keys.flag(Key::GroupElemByStyle))
        replacer.groupElementsByStyles();
    if (Keys.flag(Key::ApplyTransformsToDefs))
        replacer.applyTransformToDefs();
    if (Keys.flag(Key::TrimIds))
        replacer.trimIds();
    replacer.roundNumericAttributes();
    // TODO: check only for xmlns:xlink
    remover.cleanSvgElementAttribute();
    if (Keys.flag(Key::SortDefs))
        replacer.sortDefs();
    replacer.finalFixes();
    if (Keys.flag(Key::JoinStyleAttributes))
        replacer.joinStyleAttr();

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("Error: could not write output file");

    SVGPrinter printer(0, Keys.flag(Key::CompactOutput));
    doc.Print(&printer);
    outFile.write(printer.CStr());
    outFile.close();

    if (!Keys.flag(Key::ShortOutput))
        qDebug("The final file size is: %u", (int)QFile(outPath).size());

    replacer.calcElemAttrCount("final");
}

int main(int argc, char *argv[])
{
    // TODO: maybe get rid of QCoreApplication
    QCoreApplication app(argc, argv);

    QStringList argList = app.arguments();
    // remove executable path
    argList.removeFirst();

    if (argList.contains("-v") || argList.contains("--version")) {
        qDebug() << 0.6;
        return 0;
    }

    if (argList.contains("-h") || argList.contains("--help") || argList.size() < 2) {
        showHelp();
        return 0;
    }

    if (argList.contains("--info") && argList.size() == 2) {
        showPresetInfo(argList.at(1));
        return 0;
    }

    QString inputFile  = argList.takeFirst();
    QString outputFile = argList.takeFirst();

    if (!QFile(inputFile).exists())
        qFatal("Error: input file does not exist");
    if (!QFileInfo(outputFile).absoluteDir().exists())
        qFatal("Error: output folder does not exist");

    Keys.parseOptions(argList);
    processFile(inputFile, outputFile);

    return 0;
}
