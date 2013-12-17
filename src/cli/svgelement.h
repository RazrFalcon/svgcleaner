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
    bool hasLinkedDef();
    bool isContainer() const;
    bool isGroup() const;
    bool isNull() const;
    bool isReferenced() const;
    bool isText() const;
    bool hasImportantAttrs();
    bool isUsed() const;
    double doubleAttribute(const QString &name) const;
    int attributesCount() const;
    int childElementCount() const;
    QList<SvgElement> childElemList() const;
    QString attribute(const QString &name) const;
    QString id() const;
    QString defIdFromAttribute(const QString &name);
    StringMap attributesMap() const;
    QStringList attributesList() const;
    QString tagName() const;
    StringHash styleHash() const;
    SvgElement insertBefore(const SvgElement &elemNew, const SvgElement &elemBefore) const;
    SvgElement parentNode() const;
    SvgElement firstChild() const;
    void appendChild(const SvgElement &elem);
    void clear();
    void removeAttribute(const QString &name);
    void removeAttributes(const QStringList &list);
    void removeChild(const SvgElement &elem);
    void setAttribute(const QString &name, const QString &value);
    void setStylesFromHash(const StringHash &hash);
    void setTagName(const QString &name);
    void setTransform(const QString &transform);

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
