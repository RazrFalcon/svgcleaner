/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

#include "keys.h"
#include "remover.h"
#include "replacer.h"

// TODO: add support for preset file reading
// TODO: custom xml printing

void showHelp()
{
    qDebug() << "SVG Cleaner could help you to clean up your SVG files from unnecessary data.";
    qDebug() << "";
    qDebug() << "Usage:";
    qDebug() << "  svgcleaner-cli <in-file> <out-file> [--option]";
    qDebug() << "";
    qDebug() << "By default, all options are on, and you can only disable it.";
    qDebug() << "Options:";
    qDebug() << "  -h --help                Show this text.";
    qDebug() << "  -v --version             Show version.";

    qDebug() << "Elements:";
    qDebug() << "  --keep-prolog            Disable xml prolog removing.";
    qDebug() << "  --keep-comments          Disable xml comments removing.";
    qDebug() << "  --keep-proc-instr        Disable xml processing instruction removing.";
    qDebug() << "  --keep-unused-defs       Disable removing of unused 'defs'.";
    qDebug() << "  --keep-nonsvg-elts       Keep all non svg elements.";
    qDebug() << "  --keep-metadata-elts     Keep metadata elements.";
    qDebug() << "  --keep-inkscape-elts     Keep Inkscape elements.";
    qDebug() << "  --keep-sodipodi-elts     Keep sodipodi elements.";
    qDebug() << "  --keep-ai-elts           Keep Adobe Illustrator elements.";
    qDebug() << "  --keep-cdr-elts          Keep CorelDRAW elements.";
    qDebug() << "  --keep-msvisio-elts      Keep MS Visio elements.";
    qDebug() << "  --keep-invisible-elts    Keep invisible elements.";
    qDebug() << "  --keep-empty-conts       Disable removing of empty container elements.";
    qDebug() << "  --keep-groups            Disable group collapsing, when possible.";
    qDebug() << "  --keep-dupl-defs         Disable removing of duplicated defining content in 'defs' element.";
    qDebug() << "  --keep-singly-grads      Do not merge 'linearGradient' into 'radialGradient',\n"
                "                           when 'linearGradient' linked only to one 'radialGradient'.";
    qDebug() << "  --keep-gaussian-blur     Disable removing 'feGaussianBlur' filters with 'stdDeviation' lower then '--std-dev'.";
    qDebug() << "  --std-dev=<0.01..0.5>    Set minimum value for 'stdDeviation' in 'feGaussianBlur' element [default: 0.1].";

    qDebug() << "Attributes:";
    qDebug() << "  --keep-version           Keep SVG version number.";
    qDebug() << "  --keep-unref-ids         Disable removing of unreferenced id's.";
    qDebug() << "  --keep-named-ids         Disable removing of unreferenced id's which contains only letters.";
    qDebug() << "  --keep-notappl-atts      Keep not applied attributes, like 'font-size' for 'rect' element.";
    qDebug() << "  --keep-default-atts      Skip removing attributes with default values.";
    qDebug() << "  --keep-inkscape-atts     Keep Inkscape attributes.";
    qDebug() << "  --keep-sodipodi-atts     Keep sodipodi attributes.";
    qDebug() << "  --keep-ai-atts           Keep Adobe Illustrator attributes.";
    qDebug() << "  --keep-cdr-atts          Keep CorelDRAW attributes.";
    qDebug() << "  --keep-msvisio-atts      Keep MS Visio attributes.";
    qDebug() << "  --keep-stroke-props      Keep stroke properties when no stroking.";
    qDebug() << "  --keep-fill-props        Keep fill properties when no filling.";
    qDebug() << "  --keep-grad-coords       Disable removing of unneeded gradient attributes.";
    qDebug() << "  --keep-unused-xlinks     Keep XLinks which pointed to nonexistent element.";
    qDebug() << "  --skip-style-group       Group elements by style properties.";
    qDebug() << "  --skip-ids-trim          Skip trimming to the id attributes into hexadecimal format.";
    qDebug() << "Attributes utilities:";
    qDebug() << "  --join-style-atts        Merge style properties into 'style' attribute.";

    qDebug() << "Paths:";
    qDebug() << "  --keep-absolute-paths    Disable absolute to relative coordinates converting in path element.";
    qDebug() << "  --keep-unused-symbols    Keep unused symbols in 'd' attribute from 'path' element.";
    qDebug() << "  --keep-tiny-segments     Keep tiny or empty segments in 'path' element.";
    qDebug() << "  --keep-curveto           Disable converting 'curveto' segment to short one, when possible.";

    qDebug() << "Optimization:";
    qDebug() << "  --no-viewbox             Disable 'viewBox' attribute creating from 'height' and 'width' in 'svg' element.";
    qDebug() << "  --skip-color-to-rrggbb   Skip converting 'rgb(255,255,255)' or color name into #RRGGBB format.";
    qDebug() << "  --skip-rrggbb-to-rgb     Skip color converting from #RRGGBB into #RGB format, when possible.";
    qDebug() << "  --keep-basic-shapes      Disable converting basic shapes (rect, line, polygon, polyline) into path.";
    qDebug() << "  --keep-transform         Skip applying of transform matrices.";
    qDebug() << "  --keep-unsorted-defs     Disable element by name sorting in 'defs' element.";
    qDebug() << "  --skip-rounding-numbers  Skip rounding numbers.";
    qDebug() << "  --transfs-prec=<1..8>    Set rounding precision for transformations [default: 5].";
    qDebug() << "  --coords-prec=<1..8>     Set rounding precision for coordinates [default: 3].";
    qDebug() << "  --attrs-prec=<1..8>      Set rounding precision for attributes [default: 3].";

    qDebug() << "Output:";
    qDebug() << "  --not-compact            Save svg with only required whitespace and newlines.";
}

void processFile(const QString &firstFile, const QString &secondFile)
{
    QFile inputFile(firstFile);
    qDebug() << "The initial file size is: " + QString::number(inputFile.size());
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
    replacer.convertCDATAStyle();
    replacer.prepareDefs();
    replacer.fixWrongAttr();
    replacer.markUsedElements();

    if (!Keys::get().flag(Key::KeepDefaultAttributes))
        remover.cleanSvgElementAttribute();
    if (!Keys::get().flag(Key::NoViewbox))
        replacer.convertSizeToViewbox();
    if (!Keys::get().flag(Key::KeepUnusedDefs))
        remover.removeUnusedDefs();
    if (!Keys::get().flag(Key::KeepDuplicatedDefs))
        remover.removeDuplicatedDefs();
    // TODO: add key
    remover.removeElements();
    remover.removeAttributes();
    if (!Keys::get().flag(Key::KeepSinglyGradients))
        replacer.mergeGradients();
    if (!Keys::get().flag(Key::KeepUnreferencedIds))
        remover.removeUnreferencedIds();
    if (!Keys::get().flag(Key::KeepUnusedXLinks))
        remover.removeUnusedXLinks();
    if (!Keys::get().flag(Key::SkipIdsTrim))
        replacer.trimIds();
    if (!Keys::get().flag(Key::KeepDefaultAttributes))
        remover.processStyleAttr();
    if (!Keys::get().flag(Key::KeepUnsortedDefs))
        replacer.sortDefs();
    if (!Keys::get().flag(Key::KeepBasicShapes))
        replacer.convertBasicShapes();
    if (!Keys::get().flag(Key::KeepGroups))
        remover.removeGroups();
    if (!Keys::get().flag(Key::KeepAbsolutePaths))
        replacer.processPaths();
    if (!Keys::get().flag(Key::SkipElemByStyleGroup))
        replacer.groupElementsByStyles();
    if (!Keys::get().flag(Key::KeepTransforms))
        replacer.applyTransformToDefs();
    if (!Keys::get().flag(Key::SkipRoundingNumbers))
        replacer.roundNumericAttributes();
    replacer.finalFixes();

    if (Keys::get().flag(Key::JoinStyleAttributes))
        replacer.joinStyleAttr();

    // TODO: check is out file smaller than original
    QFile outFile(secondFile);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("Error: could not open out file for write.");

    XMLPrinter printer(0, !Keys::get().flag(Key::NotCompact));
    doc.Print(&printer);
    QString outStr = QString::fromUtf8(printer.CStr());

    QTextStream outStream(&outFile);
    outStr.replace(">\n<tspan", "><tspan");
    outStr.replace("&apos;", "'");
    outStream << outStr;
    outFile.close();

    qDebug() << "The final file size is: " + QString::number(outFile.size());
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

    if (argList.contains("-h") || argList.contains("--help")
        || argList.isEmpty() || argList.count() == 1) {
        showHelp();
        return 0;
    }

    QString inputFile  = argList.takeFirst();
    QString outputFile = argList.takeFirst();

    if (!QFile(inputFile).exists())
        qFatal("Error: input file does not exist.");
    if (!QFileInfo(outputFile).absoluteDir().exists())
        qFatal("Error: output folder does not exist.");

    Keys::get().parseOptions(argList);
    processFile(inputFile, outputFile);

    return 0;
}
