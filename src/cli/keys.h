#ifndef KEYS_H
#define KEYS_H

#include <QtCore/QVariantHash>
#include <QtCore/QStringList>

namespace Key {
    static const QString KeepProlog = "--keep-prolog";
    static const QString KeepComments = "--keep-comments";
    static const QString KeepProcessingInstruction = "--keep-proc-instr";
    static const QString KeepUnusedDefs = "--keep-unused-defs";
    static const QString KeepNonSvgElements = "--keep-nonsvg-elts";
    static const QString KeepMetadata = "--keep-metadata-elts";
    static const QString KeepInkscapeElements = "--keep-inkscape-elts";
    static const QString KeepSodipodiElements = "--keep-sodipodi-elts";
    static const QString KeepIllustratorElements = "--keep-ai-elts";
    static const QString KeepCorelDrawElements = "--keep-cdr-elts";
    static const QString KeepMSVisioElements = "--keep-msvisio-elts";
    static const QString KeepInvisibleElements = "--keep-invisible-elts";
    static const QString KeepEmptyContainer = "--keep-empty-conts";
    static const QString KeepGroups = "--keep-groups";
    static const QString KeepDuplicatedDefs = "--keep-dupl-defs";
    static const QString KeepSinglyGradients = "--keep-singly-grads";
    static const QString KeepTinyGaussianBlur = "--keep-gaussian-blur";
    static const QString StdDeviation = "--std-dev";

    static const QString KeepSvgVersion = "--keep-version";
    static const QString KeepUnreferencedIds = "--keep-unref-ids";
    static const QString RemoveNamedIds = "--remove-named-ids";
    static const QString KeepNotAppliedAttributes = "--keep-notappl-atts";
    static const QString KeepDefaultAttributes = "--keep-default-atts";
    static const QString KeepInkscapeAttributes = "--keep-inkscape-atts";
    static const QString KeepSodipodiAttributes = "--keep-sodipodi-atts";
    static const QString KeepIllustratorAttributes = "--keep-ai-atts";
    static const QString KeepCorelDrawAttributes = "--keep-cdr-atts";
    static const QString KeepMSVisioAttributes = "--keep-msvisio-atts";
    static const QString KeepStrokeProps = "--keep-stroke-props";
    static const QString KeepFillProps = "--keep-fill-props";
    static const QString KeepGradientCoordinates = "--keep-grad-coords";
    static const QString KeepUnusedXLinks = "--keep-unused-xlinks";
    static const QString SkipElemByStyleGroup = "--skip-style-group";

    static const QString KeepAbsolutePaths = "--keep-absolute-paths";
    static const QString KeepUnusedSymbolsFromPath = "--keep-unused-symbols";
    static const QString KeepEmptySegments = "--keep-empty-segments";
    static const QString KeepLinesTo = "--keep-lineto";

    static const QString NoViewbox = "--no-viewbox";
    static const QString JoinStyleAttributes = "--join-style-atts";
    static const QString SkipColorToRRGGBB = "--skip-color-to-rrggbb";
    static const QString SkipRRGGBBToRGB = "--skip-rrggbb-to-rgb";
    static const QString KeepBasicShapes = "--keep-basic-shapes";
    static const QString KeepTransforms = "--keep-transform";
    static const QString SkipTransformsApplying = "--skip-transform-appl";
    static const QString KeepUnsortedDefs = "--keep-unsorted-defs";
    static const QString SkipRoundingNumbers = "--skip-rounding-numbers";
    static const QString TransformPrecision = "--transfs-prec";
    static const QString CoordsPrecision = "--coords-prec";
    static const QString AttributesPrecision = "--attributes-prec";

    static const QString Indent = "--indent";

//    static const QString ConvertCurvesCs = "--convert-curves-cs";
//    static const QString ConvertCurvesToLines = "--convert-curves-lines";
//    static const QString convertCurvesQT = "--convert-curves-qt";
//    static const QString RoundNumbersToInt = "--round-numbers-int";
}

// singleton
class Keys
{
public:
    static Keys& get() {
        static Keys instance;
        return instance;
    }
    QVariant value(const QString &key);
    bool flag(const QString &key);
    int intNumber(const QString &key);
    double doubleNumber(const QString &key);
    void parceOptions(const QStringList &list);

private:
    QVariantHash keyHash;

    Keys();
    Keys(Keys const&);
    void operator=(Keys const&);
};

#endif // KEYS_H
