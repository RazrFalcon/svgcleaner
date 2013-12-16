#include "basecleaner.h"

BaseCleaner::BaseCleaner(XMLDocument *doc)
{
    m_doc = doc;
    m_svgElem = Tools::svgElement(doc);
    m_defsElem = Tools::defsElement(doc, m_svgElem);
}

XMLDocument* BaseCleaner::document() const
{
    return m_doc;
}

SvgElement BaseCleaner::svgElement() const
{
    return m_svgElem;
}

SvgElement BaseCleaner::defsElement() const
{
    return m_defsElem;
}

void BaseCleaner::updateXLinks(const StringHash &hash)
{
    QStringList xlinkStyles;
    xlinkStyles << "fill" << "stroke";

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.isEmpty()) {
        SvgElement currElem = list.takeFirst();
        for (int i = 0; i < xlinkStyles.size(); ++i) {
            if (currElem.hasAttribute(xlinkStyles.at(i))) {
                QString attrValue = currElem.attribute(xlinkStyles.at(i));
                if (attrValue.startsWith("url")) {
                    QString url = attrValue.mid(5, attrValue.size()-6);
                    if (hash.contains(url)) {
                        currElem.setAttribute(xlinkStyles.at(i),
                                              QString("url(#" + hash.value(url) + ")"));
                    }
                }
            }
        }
        if (currElem.hasAttribute("xlink:href")) {
            QString value = currElem.attribute("xlink:href");
            value.remove(0,1); // #
            QString elemId = currElem.id();
            foreach (const QString &key, hash.keys()) {
                if (value == key) {
                    if (hash.value(key) != elemId)
                        currElem.setAttribute("xlink:href", QString("#" + hash.value(key)));
                    else
                        currElem.removeAttribute("xlink:href");
                    break;
                }
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}
