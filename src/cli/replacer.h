#ifndef REPLACER_H
#define REPLACER_H

#include "paths.h"
#include "tools.h"

typedef QPair<SvgElement,QStringList> ElemListPair;
typedef QPair<QString, int> RepetitionItem;
typedef QList<RepetitionItem> RepetitionList;

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
    void groupElementsByStyles(SvgElement parentElem = SvgElement());
    void applyTransformMatrices();

private:
    QDomDocument m_dom;
    SvgElement m_svgElem;
    SvgElement m_defsElem;
    QSet<QString> m_usedElemList;

    SvgElement findLinearGradient(const QString &id);
    RepetitionList findRepetitionList(const QList<SvgElement> &list);
    RepetitionList findRepetitionList(QList<ElemListPair> list);
    RepetitionList genRepetitionList(const QList<QStringList> &list);
};

#endif // REPLACER_H
