#ifndef REPLACER_H
#define REPLACER_H

#include "paths.h"
#include "basecleaner.h"

class Replacer : public BaseCleaner
{
public:
    explicit Replacer(XMLDocument *doc);
    void convertSizeToViewbox();
    void processPaths();
    void fixWrongAttr();
    void convertUnits();
    void convertCDATAStyle();
    void prepareDefs();
    void splitStyleAttr();
    void sortDefs();
    void roundDefs();
    void convertBasicShapes();
    void mergeGradients();
    void finalFixes();
    void trimIds();
    void calcElemAttrCount(const QString &text);
    void groupElementsByStyles(SvgElement parentElem = SvgElement());
    void applyTransformMatrices();
    void markUsedElements();

private:
    SvgElement findLinearGradient(const QString &id);
};

#endif // REPLACER_H
