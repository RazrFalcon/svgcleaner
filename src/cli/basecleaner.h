#ifndef BASECLEANER_H
#define BASECLEANER_H

#include "tools.h"

class BaseCleaner
{
public:
    BaseCleaner(XMLDocument *doc);
    virtual ~BaseCleaner() {}
    XMLDocument* document();
    SvgElement svgElement();
    SvgElement defsElement();
    void updateXLinks(const StringHash &hash);

private:
    XMLDocument *m_doc;
    SvgElement m_svgElem;
    SvgElement m_defsElem;
};

#endif // BASECLEANER_H
