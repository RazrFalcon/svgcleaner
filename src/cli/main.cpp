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

// TODO: custom xml printing

void printLine(const QString &key, const QString &desc = QString())
{
    if (desc.isEmpty())
        qDebug("  %s %s", qPrintable(key.leftJustified(35, ' ')),
                          qPrintable(Keys.description(key)));
    else
        qDebug("  %s %s", qPrintable(key.leftJustified(35, ' ')), qPrintable(desc));
}

void showPresetInfo(const QString &presetName)
{
    QStringList list;
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
    foreach (const QString &key, list) {
        if (key == Key::TransformPrecision || key == Key::AttributesPrecision
            || key == Key::CoordsPrecision) {
            qDebug() << key + "=" + QString::number(Keys.intNumber(key));
        } else if (key == Key::RemoveTinyGaussianBlur) {
            qDebug() << key + "=" + QString::number(Keys.doubleNumber(key));
        } else {
            qDebug() << key;
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
    qDebug() << "  svgcleaner-cli --info --preset=";
    qDebug() << "";
    qDebug() << "Presets:";
    printLine("--preset basic", "Basic cleaning");
    printLine("--preset complete", "Complete cleaning");
    printLine("--preset extreme", "Extreme cleaning");
    qDebug() << "";
    qDebug() << "Options:";
    qDebug() << "";
    printLine("-h --help", "Show this text");
    printLine("-v --version", "Show version");
    qDebug() << "";

    qDebug() << "Elements:";
    foreach (const QString &key, Keys.elementsKeys()) {
        if (key == Key::RemoveTinyGaussianBlur)
            printLine(key + "=<0..1.0>",
                    Keys.description(key) + QString(" [default: %1]").arg(Keys.doubleNumber(key)));
        else
            printLine(key);
    }
    qDebug() << "";
    qDebug() << "Attributes:";
    foreach (const QString &key, Keys.attributesKeys())
        printLine(key);
    qDebug() << "Additional:";
    foreach (const QString &key, Keys.attributesUtilsKeys())
        printLine(key);
    qDebug() << "";
    qDebug() << "Paths:";
    foreach (const QString &key, Keys.pathsKeys())
        printLine(key, Keys.description(key));
    qDebug() << "";
    qDebug() << "Optimizations:";
    foreach (const QString &key, Keys.optimizationsKeys()) {
        if (key == Key::TransformPrecision || key == Key::AttributesPrecision
            || key == Key::CoordsPrecision) {
            printLine(key + "=<1..8>",
                    Keys.description(key) + QString(" [default: %1]").arg(Keys.doubleNumber(key)));
        } else
            printLine(key);
    }
    qDebug() << "Additional:";
    foreach (const QString &key, Keys.optimizationsUtilsKeys())
        printLine(key);
}

void processFile(const QString &firstFile, const QString &secondFile)
{
    QFile inputFile(firstFile);
    if (!Keys.flag(Key::ShortOutput))
        qDebug("The initial file size is: %u", (int)inputFile.size());

    XMLDocument doc;
    doc.LoadFile(firstFile.toUtf8().constData());
    if (Tools::svgElement(&doc).isNull()) {
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
            qFatal("Error: cannot open input file.");
        QTextStream inputStream(&inputFile);
        QString svgText = inputStream.readAll();
        svgText.replace("<svg:", "<");
        svgText.replace("</svg:", "</");
        doc.Clear();
        doc.Parse(svgText.toUtf8().constData());
        if (Tools::svgElement(&doc).isNull())
            qFatal("Error: invalid svg file.");
    }

    Replacer replacer(&doc);
    Remover remover(&doc);

    replacer.calcElemAttrCount("initial");

    // mandatory fixes used to simplify subsequent functions
    replacer.splitStyleAttr();
    replacer.convertUnits();
    // TODO: add key
    replacer.convertCDATAStyle();
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
    if (Keys.flag(Key::ConvertBasicShapes))
        replacer.convertBasicShapes();
    if (Keys.flag(Key::UngroupGroups))
        remover.removeGroups();
    replacer.processPaths();
    if (Keys.flag(Key::GroupElemByStyle))
        replacer.groupElementsByStyles();
    if (Keys.flag(Key::ApplyTransformsToDefs))
        replacer.applyTransformToDefs();
    if (Keys.flag(Key::TrimIds))
        replacer.trimIds();
    replacer.roundNumericAttributes();
    replacer.finalFixes();

    if (Keys.flag(Key::JoinStyleAttributes))
        replacer.joinStyleAttr();
    if (Keys.flag(Key::SortDefs))
        replacer.sortDefs();

    // TODO: check is out file smaller than original
    QFile outFile(secondFile);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("Error: could not open out file for write.");

    XMLPrinter printer(0, Keys.flag(Key::CompactOutput));
    doc.Print(&printer);
    QString outStr = QString::fromUtf8(printer.CStr());

    QTextStream outStream(&outFile);
    // TODO: get rid of this lines
    outStr.replace(">\n<tspan", "><tspan");
    outStr.replace("&apos;", "'");
    outStream << outStr;
    outFile.close();

    if (!Keys.flag(Key::ShortOutput))
        qDebug("The final file size is: %u", (int)outFile.size());

    replacer.calcElemAttrCount("final");
}

int main(int argc, char *argv[])
{
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
        qFatal("Error: input file does not exist.");
    if (!QFileInfo(outputFile).absoluteDir().exists())
        qFatal("Error: output folder does not exist.");

    Keys.parseOptions(argList);
    processFile(inputFile, outputFile);

    return 0;
}
