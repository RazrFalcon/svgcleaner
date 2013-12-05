#ifndef REPLACER_H
#define REPLACER_H

#include "paths.h"
#include "basecleaner.h"

typedef QPair<SvgElement,StringHash> ElemListPair;
typedef QList<QPair<QString,int> > RepetitionList;

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

private:
    QSet<QString> m_usedElemList;

    SvgElement findLinearGradient(const QString &id);
    RepetitionList findRepetitionList(const QList<SvgElement> &list);
    RepetitionList findRepetitionList(QList<ElemListPair> list);
    RepetitionList genRepetitionList(const QList<StringHash> &list);
    static bool repetitionListSort(const QPair<QString,int> &s1, const QPair<QString,int> &s2);
};

#endif // REPLACER_H
