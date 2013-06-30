#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QStringList>

#include <QtDebug>

#include "keys.h"
#include "remover.h"
#include "replacer.h"

// TODO: add support for preset file reading
// TODO: custom xml to text covertor
// TODO: attribute sort + id first
// TODO: add render error detecting

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
    qDebug() << "  --remove-named-ids       Disable skipping of unreferenced id's which contains only letters.";
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

    qDebug() << "Paths:";
    qDebug() << "  --keep-absolute-paths    Disable absolute to relative coordinates converting in path element.";
    qDebug() << "  --keep-unused-symbols    Keep unused symbols in 'd' attribute from 'path' element.";
    qDebug() << "  --keep-empty-segments    Keep empty segment in path element.";
    qDebug() << "  --keep-lineto            Disable converting 'lineto' segment to 'vertical lineto/horizontal lineto', when posible.";

    qDebug() << "Optimization:";
    qDebug() << "  --no-viewbox             Disable 'viewBox' attribute creating from 'height' and 'width' in 'svg' element.";
    qDebug() << "  --join-style-atts        Do not convert style properties into SVG attributes.";
    qDebug() << "  --skip-color-to-rrggbb   Skip converting 'rgb(255,255,255)', 'color-name' into #RRGGBB format.";
    qDebug() << "  --skip-rrggbb-to-rgb     Skip color converting from #RRGGBB into #RGB format, when posible.";
    qDebug() << "  --keep-basic-shapes      Disable converting basic shapes (rect, line, polygon, polyline) into path.";
    qDebug() << "  --keep-transform         Skip merging and simplifying of 'transform' matrices.";
    qDebug() << "  --keep-unsorted-defs     Disable element by name sorting in 'defs' element.";
    qDebug() << "  --skip-rounding-numbers  Skip rounding numbers.";
    qDebug() << "  --transfs-prec=<1..9>    Set rounding precision for transformations [default: 5].";
    qDebug() << "  --coords-prec=<1..9>     Set rounding precision for coordinates [default: 4].";
    qDebug() << "  --attrs-prec=<1..9>      Set rounding precision for attributes [default: 3].";

    qDebug() << "Output:";
    qDebug() << "  --indent=<-1..4>         Set xml indent for output svg [default: 0].";
}

QString prepareSvg(QString str)
{
    str.replace("\t", " ");
    str.replace("\n", " ");
    str.replace("<svg:", "<");
    str.replace("</svg:", "</");
    if (!Keys::get().flag(Key::KeepProlog))
        str.remove(QRegExp("<\\!DOCTYPE.*\\.dtd('|\")>(\n|)"));
    return str;
}

bool processFile(const QString &firstFile, const QString &secondFile)
{

    QFile inputFile(firstFile);
    qDebug() << "The initial file size is: " + QString::number(inputFile.size());
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: cannot open input file.";
        return false;
    }
    QTextStream inputStream(&inputFile);
    QDomDocument inputSvg;
    QString svgText = prepareSvg(inputStream.readAll());
    inputSvg.setContent(svgText);

    if (inputSvg.elementsByTagName("svg").count() == 0) {
        qDebug() << firstFile;
        qDebug() << "Error: it's a not well-formed SVG file.";
        return false;
    }

    Replacer replacer(inputSvg);
    Remover remover(inputSvg);

    replacer.calcElemAttrCount("initial");

    if (!Keys::get().flag(Key::KeepDefaultAttributes))
        remover.cleanSvgElementAttribute();

    // mandatory fixes used to simplify subsequent functions
    replacer.convertUnits();
    replacer.fixWrongAttr();
    replacer.convertCDATAStyle();
    replacer.prepareDefs();
    replacer.joinStyleAttr();

    if (!Keys::get().flag(Key::NoViewbox))
        replacer.convertSizeToViewbox();
    if (!Keys::get().flag(Key::KeepDuplicatedDefs))
        remover.removeDuplicatedDefs();
    if (!Keys::get().flag(Key::KeepSinglyGradients))
        replacer.mergeGradients();
    remover.removeElements();
    remover.removeAttributes();
    if (!Keys::get().flag(Key::KeepUnusedDefs))
        remover.removeUnusedDefs();
    if (!Keys::get().flag(Key::KeepUnreferencedIds))
        remover.removeUnreferencedIds();
    if (!Keys::get().flag(Key::KeepUnusedXLinks))
        remover.removeUnusedXLinks();
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
    if (!Keys::get().flag(Key::SkipRoundingNumbers))
        replacer.roundDefs();
    replacer.splitStyleAttr();
    replacer.finalFixes();

    QFile outFile(secondFile);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: could not open out file for write.";
        return false;
    }
    // TODO: check is out file smaller than original
    QTextStream out(&outFile);
    // remove unneeded new lines in text elements, created by default xml format,
    // but wrong in svg
    QString outStr = inputSvg.toString(Keys::get().intNumber(Key::Indent));
    if (!Keys::get().flag(Key::KeepProlog))
        outStr.remove(QRegExp("<\\!DOCTYPE.*\\.dtd('|\")>(\n|)"));
    outStr.replace(">\n<tspan", "><tspan");
    out << outStr;
    outFile.close();

    qDebug() << "The final file size is: " + QString::number(outFile.size());
    replacer.calcElemAttrCount("final");
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.5");

    QStringList argList = app.arguments();
    // remove executable path
    argList.removeFirst();

    if (argList.contains("-v") || argList.contains("--version")) {
        qDebug() << qPrintable(app.applicationVersion());
        return 0;
    }

    if (argList.contains("-h") || argList.contains("--help")
        || argList.isEmpty() || argList.count() == 1) {
        showHelp();
        return 0;
    }

    QString inputFile  = argList.takeFirst();
    QString outputFile = argList.takeFirst();

    if (!QFile(inputFile).exists()) {
        qDebug() << "Error: input file does not exist.";
        return 0;
    }
    if (!QFileInfo(outputFile).absoluteDir().exists()) {
        qDebug() << "Error: output folder does not exist.";
        return 0;
    }

    Keys::get().parceOptions(argList);
    processFile(inputFile, outputFile);

    return 0;
}
