#ifndef REMOVER_H
#define REMOVER_H

#include "tools.h"

class Remover
{
public:
    explicit Remover(QDomDocument dom);
    void removeUnreferencedIds();
    void removeUnusedDefs();
    void removeUnusedXLinks();
    void removeDuplicatedDefs();
    void removeElements();
    void cleanSvgElementAttribute();
    void removeAttributes();
    void processStyleAttr(SvgElement elem = QDomElement());
    void removeGroups();

private:
    QDomDocument m_dom;
    SvgElement m_svgElem;
    SvgElement m_defsElem;

    void cleanStyle(const SvgElement &elem, StringHash *hash);
    void removeDefaultValue(StringHash *hash, const QString &name);
    void removeGroup(SvgElement elem);
    bool isInvisibleElementsExist(SvgElement elem);
    void updateXLinks(StringHash hash);
    void cleanAttribute(SvgElement elem, QRegExp rx);
    void ungroupSwitch(SvgElement elem);

    QSet<QString> m_usedElemList;
    SvgElement genGroup(SvgElement currElem, SvgElement parentGroup);
    void mergeGroups(QList<SvgElement> gNodeList);
};

#endif // REMOVER_H
