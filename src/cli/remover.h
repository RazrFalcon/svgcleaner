#ifndef REMOVER_H
#define REMOVER_H

#include "basecleaner.h"

struct DefsElemStruct
{
    SvgElement elem;
    QString tagName;
    bool hasChildren;
    StringMap attrMap;
};

class Remover : public BaseCleaner
{
public:
    explicit Remover(XMLDocument *doc);
    void removeUnreferencedIds();
    void removeUnusedDefs();
    void removeUnusedXLinks();
    void removeDuplicatedDefs();
    void removeElements();
    void cleanSvgElementAttribute();
    void removeAttributes();
    void processStyleAttr(SvgElement elem = SvgElement());
    void removeGroups();

private:
    QList<StringHash> styleHashList;
    StringHash parentHash;

    void cleanStyle(const SvgElement &elem, StringHash &hash);
    void removeDefaultValue(StringHash &hash, const QString &name);
    void removeGroup(SvgElement &elem);
    bool isInvisibleElementsExist(const SvgElement &elem);
    void cleanAttribute(SvgElement &elem, const QString &startWith, QStringList &attrList);
    void ungroupSwitch(SvgElement elem);
    SvgElement genGroup(SvgElement &currElem, SvgElement &parentGroup);
    void mergeGroups(QList<SvgElement> &gNodeList);
};

#endif // REMOVER_H
