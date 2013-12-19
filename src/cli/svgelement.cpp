#include "keys.h"
#include "tools.h"
#include "svgelement.h"

SvgElement::SvgElement()
{
    m_elem = 0;
}

SvgElement::SvgElement(XMLElement *elem)
{
    m_elem = elem;
}

QList<SvgElement> SvgElement::childElemList() const
{
    return Tools::childElemList(*this);
}

bool SvgElement::isReferenced() const
{
    return Props::referencedElements.contains(tagName());
}

bool SvgElement::isText() const
{
    return Props::textElements.contains(tagName());
}

bool SvgElement::hasImportantAttrs()
{
    int attrCount = attributesCount();
    if (attrCount == 0)
        return false;
    if (!Keys::get().flag(Key::KeepUnreferencedIds)) {
        if (attrCount == 1 && hasAttribute("id"))
            return false;
    }
    return true;
}

bool SvgElement::isUsed() const
{
    return hasAttribute(CleanerAttr::UsedElement);
}

bool SvgElement::isContainer() const
{
    return Props::containers.contains(tagName());
}

bool SvgElement::isGroup() const
{
    return !strcmp(m_elem->Name(), "g");
}

bool SvgElement::isNull() const
{
    return (m_elem == 0);
}

void SvgElement::removeAttributes(const QStringList &list)
{
    for (int i = 0; i < list.count(); ++i)
        removeAttribute(list.at(i));
}

StringMap SvgElement::attributesMap() const
{
    StringMap map;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next())
        map.insert(QString(child->Name()), QString(child->Value()));
    return map;
}

QStringList SvgElement::attributesList() const
{
    QStringList list;
    list.reserve(attributesCount());
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next())
        list << QString(child->Name());
    return list;
}

void SvgElement::setStylesFromHash(const StringHash &hash)
{
    foreach (const QString &attr, hash.keys())
        setAttribute(attr, hash.value(attr));
}

void SvgElement::setAttribute(const QString &name, const QString &value)
{
    if (value.isEmpty())
        m_elem->DeleteAttribute(ToChar(name));
    else
        m_elem->SetAttribute(ToChar(name), ToChar(value));
}

QString SvgElement::id() const
{
    return attribute("id");
}

QString SvgElement::defIdFromAttribute(const QString &name)
{
    QString id = attribute(name);
    if (!id.startsWith("url("))
        return QString();
    return id.mid(5, id.size()-6);
}

bool SvgElement::hasLinkedDef()
{
    static QStringList illegalAttrList;
    if (illegalAttrList.isEmpty())
        illegalAttrList << "clip-path" << "mask" << "filter";
    foreach (const QString attrName, illegalAttrList) {
        if (hasAttribute(attrName))
            return true;
    }
    static QStringList illegalStyleAttrList;
    if (illegalStyleAttrList.isEmpty())
        illegalStyleAttrList << "fill" << "stroke";
    foreach (const QString attrName, illegalStyleAttrList) {
        if (attribute(attrName).startsWith("url("))
            return true;
    }
    return false;
}

bool SvgElement::hasAttribute(const QString &name) const
{
    return hasAttribute(ToChar(name));
}

bool SvgElement::hasAttributes(const QStringList &list) const
{
    foreach (const QString &attrName, list)
        if (hasAttribute(attrName))
            return true;
    return false;
}

bool SvgElement::hasAttribute(const char *name) const
{
    return (m_elem->Attribute(name) != 0);
}

QString SvgElement::attribute(const QString &name) const
{
    const char *ch = ToChar(name);
    if (m_elem->Attribute(ch) == 0)
        return QString();
    return m_elem->Attribute(ch);
}

double SvgElement::doubleAttribute(const QString &name) const
{
    return m_elem->DoubleAttribute(ToChar(name));
}

void SvgElement::removeAttribute(const QString &name)
{
    m_elem->DeleteAttribute(ToChar(name));
}

QString SvgElement::tagName() const
{
    return m_elem->Name();
}

SvgElement SvgElement::parentNode() const
{
    return SvgElement(m_elem->Parent()->ToElement());
}

SvgElement SvgElement::firstChild() const
{
    return SvgElement(m_elem->FirstChildElement());
}

void SvgElement::removeChild(const SvgElement &elem)
{
    m_elem->DeleteChild(elem.xmlElement());
}

bool SvgElement::hasChildren() const
{
    return !m_elem->NoChildren();
}

void SvgElement::appendChild(const SvgElement &elem)
{
    m_elem->InsertEndChild(elem.xmlElement());
}

void SvgElement::setTagName(const QString &name)
{
    m_elem->SetName(ToChar(name));
}

void SvgElement::setTransform(const QString &transform, bool fromParent)
{
    if (hasAttribute("transform")) {
        if (fromParent) {
            Transform ts(transform + " " + attribute("transform"));
            setAttribute("transform", ts.simplified());
        } else {
            Transform ts(attribute("transform") + " " + transform);
            setAttribute("transform", ts.simplified());
        }
    } else if (hasAttribute("transform")) {
        setAttribute("transform", transform);
    }
}

SvgElement SvgElement::insertBefore(const SvgElement &elemNew, const SvgElement &elemBefore) const
{
    XMLElement *refElem = elemBefore.xmlElement()->PreviousSiblingElement();
    if (refElem == 0)
        return SvgElement(m_elem->InsertFirstChild(elemNew.xmlElement())->ToElement());
    return SvgElement(m_elem->InsertAfterChild(refElem, elemNew.xmlElement())->ToElement());
}

StringHash SvgElement::styleHash() const
{
    StringHash hash;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next()) {
        if (Props::styleAttributes.contains(child->Name()))
            hash.insert(child->Name(), child->Value());
    }
    return hash;
}

int SvgElement::attributesCount() const
{
    int count = 0;
    for (const XMLAttribute *child = m_elem->FirstAttribute(); child; child = child->Next())
        count++;
    return count;
}

int SvgElement::childElementCount() const
{
    int count = 0;
    for (XMLElement *child = m_elem->FirstChildElement(); child;
            child = child->NextSiblingElement())
    {
        count++;
    }
    return count;
}

void SvgElement::clear()
{
    m_elem = 0;
}
