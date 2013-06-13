#ifndef REPLACER_H
#define REPLACER_H

#include "paths.h"
#include "tools.h"

class Replacer
{
public:
    explicit Replacer(QDomDocument dom);
    void convertSizeToViewbox();
    void processPaths();
    void fixWrongAttr();
    void convertUnits();
    void convertCDATAStyle();
    void prepareDefs();
    void joinStyleAttr();
    void sortDefs();
    void roundDefs();
    void processStyle(QDomElement elem);
    void convertBasicShapes();
    void splitStyleAttr();
    void mergeGradients();
    void finalFixes();
    void calcElemAttrCount(const QString &text);

private:
    QDomDocument m_dom;
    SvgElement m_svgElem;
    SvgElement m_defsElem;

    SvgElement findLinearGradient(const QString &id);
};

#endif // REPLACER_H
