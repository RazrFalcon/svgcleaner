#ifndef SVGELEMENT_H
#define SVGELEMENT_H

#include <QStringList>
#include <QMap>

typedef QMap<QString, QString> StringMap;
typedef QHash<QString, QString> StringHash;

#include "tinyxml2.h"

#define ToChar(x) x.toStdString().c_str()

using namespace tinyxml2;

// QDomElement like wrapper class for XMLElement
class SvgElement
{
public:
    SvgElement();
    SvgElement(XMLElement *elem);

    bool hasAttribute(const char *name) const;
    bool hasAttribute(const QString &name) const;
    bool hasAttributes(const QStringList &list) const;
    bool hasChildren() const;
    bool isContainer() const;
    bool isGroup() const;
    bool isNull() const;
    bool isReferenced() const;
    bool isText() const;
    double doubleAttribute(const QString &name) const;
    int attributesCount() const;
    int childElementCount() const;
    QList<SvgElement> childElemList() const;
    QString attribute(const QString &name) const;
    QString genStyleString() const;
    QString id() const;
    StringMap attributesMap() const;
    QStringList attributesList() const;
    QString tagName() const;
    StringHash styleHash() const;
    SvgElement insertBefore(const SvgElement &elemNew, const SvgElement &elemBefore) const;
    SvgElement parentNode() const;
    void appendChild(const SvgElement &elem);
    void clear();
    void removeAttribute(const QString &name);
    void removeAttributes(const QStringList &list);
    void removeChild(const SvgElement &elem);
    void setAttribute(const QString &name, const QString &value);
    void setStylesFromHash(const StringHash &hash);
    void setTagName(const QString &name);

    XMLElement* xmlElement() const { return m_elem; }
    bool operator==(const SvgElement &elem1) {
        return elem1.xmlElement() == this->xmlElement();
    }
    bool operator!=(const SvgElement &elem1) {
        return elem1.xmlElement() != this->xmlElement();
    }
    void operator=(const SvgElement &elem) { m_elem = elem.xmlElement(); }

private:
    XMLElement *m_elem;
};

#endif // SVGELEMENT_H
