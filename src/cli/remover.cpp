#include <QtDebug>

#include "keys.h"
#include "remover.h"

// TODO: remove equal styles in used element and it use
// TODO: remove items which out of viewbox
//       Anonymous_butterfly_and_flowers.svg

Remover::Remover(QDomDocument dom)
{
    m_dom = dom;
    m_svgElem = dom.elementsByTagName("svg").at(0).toElement();
    m_defsElem = Tools::findDefsNode(m_svgElem).toElement();
}

void Remover::cleanSvgElementAttribute()
{
    bool isXlinkUsed = false;
    QList<SvgElement> nodeList = m_svgElem.childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        if (currElem.hasAttribute("xmlns:xlink")) {
            isXlinkUsed = true;
            break;
        }
        if (currElem.hasChildNodes())
            nodeList << currElem.childElemList();
    }

    QString strIgnore = "xmlns|width|height|viewBox|enable-background|fill.*|stroke.*|style";
    if (Keys::get().flag(Key::KeepSvgVersion))
        strIgnore += "|version";
    if (!isXlinkUsed)
        strIgnore += "|xmlns:xlink";
    foreach (const QString &attr, m_svgElem.attributesList()) {
        if (!attr.contains(QRegExp("^(" + strIgnore + ")$"))) {
            m_svgElem.removeAttribute(attr);
        }
    }
    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (m_svgElem.style() == "display:inline")
        m_svgElem.removeAttribute("style");
}

void Remover::removeUnusedDefs()
{
    QSet<QString> defsIdList;
    defsIdList << "";
    while (!defsIdList.isEmpty()) {
        defsIdList.clear();

        foreach (const SvgElement &elem, m_defsElem.childElemList())
            if (elem.tagName() != "clipPath")
                defsIdList << elem.id();

        QList<SvgElement> nodeList = m_svgElem.childElemList();
        while (!nodeList.empty()) {
            SvgElement currElem = nodeList.takeFirst();

            if (currElem.hasAttribute("xlink:href"))
                defsIdList.remove(currElem.attribute("xlink:href").remove("#"));
            if (currElem.hasAttribute("style")) {
                QStringList tmpList = currElem.style().split(";").filter("url");
                for (int i = 0; i < tmpList.count(); ++i) {
                    QString url = tmpList.at(i);
                    url.replace(RegEx::xlinkUrl, "");
                    defsIdList.remove(url);
                }
            }

            if (currElem.hasChildNodes())
                nodeList << currElem.childElemList();
        }

        foreach (const SvgElement &elem, m_defsElem.childElemList()) {
            if (defsIdList.contains(elem.id()))
                m_defsElem.removeChild(elem);
        }
    }
}

void Remover::removeUnusedXLinks()
{
    QSet<QString> xlinkSet;
    QSet<QString> idSet;
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        if (currElem.hasAttribute("xlink:href")) {
            if (!currElem.attribute("xlink:href").contains("base64"))
                xlinkSet << currElem.attribute("xlink:href").remove("#");
        }
        if (currElem.hasAttribute("id"))
            idSet << currElem.id();

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
    QSet<QString>::iterator it = idSet.begin();
    while (it != idSet.end()) {
        xlinkSet.remove(*it);
        ++it;
    }

    list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
        if (currElem.hasAttribute("xlink:href")) {
            if (xlinkSet.contains(currElem.attribute("xlink:href").remove("#"))) {
                currElem.removeAttribute("xlink:href");
            }
        }
        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// TODO: detect not only equal, but with diff less than 1%
// TODO: add other defs
// TODO: compare with child nodes
// TODO: refract
void Remover::removeDuplicatedDefs()
{
    StringHash xlinkToReplace;
    QDomNodeList defsList = m_defsElem.childNodes();
    for (int i = 0; i < defsList.count(); ++i) {
        SvgElement currElem = defsList.at(i).toElement();
        if (!currElem.hasChildNodes()) {
            for (int j = 0; j < defsList.count(); ++j) {
                SvgElement currElem2 = defsList.at(j).toElement();
                if (currElem2.id() != currElem.id()
                    && currElem2.tagName() == currElem.tagName() && !currElem2.hasChildNodes()) {

                    QSet<QString> currAttrList;
                    if (currElem.tagName() == "linearGradient")
                        currAttrList = Props::linearGradient;
                    else if (currElem.tagName() == "radialGradient")
                        currAttrList = Props::radialGradient;

                    if (!currAttrList.isEmpty()) {
                        if (Tools::isAttrEqual(currElem, currElem2, currAttrList)) {
                            if (xlinkToReplace.values().contains(currElem2.id())) {
                                for (int k = 0; k < xlinkToReplace.keys().count(); ++k) {
                                    if (xlinkToReplace.value(xlinkToReplace.keys().at(k)) == currElem2.id())
                                        xlinkToReplace.insert(xlinkToReplace.keys().at(k), currElem.id());
                                }
                            }
                            xlinkToReplace.insert(currElem2.id(), currElem.id());
                            m_defsElem.removeChild(currElem2);
                            j--;
                        }
                    }
                }
            }
        }
    }
    updateXLinks(xlinkToReplace);
}

void Remover::updateXLinks(StringHash hash)
{
    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();

        if (currElem.hasAttribute("style")) {
            QString style = currElem.style();
            QStringList tmpList = style.split(";");
            bool changed = false;
            for (int i = 0; i < tmpList.count(); ++i) {
                if (tmpList.at(i).contains("url")) {
                    QString url = QString(tmpList.at(i)).replace(RegEx::xlinkUrl, "");
                    if (hash.contains(url)) {
                        tmpList[i].replace(url, hash.value(url));
                        changed = true;
                    }
                }
            }
            if (changed)
                currElem.setStyle(tmpList.join(";"));
        }
        if (currElem.hasAttribute("xlink:href")) {
            foreach (const QString &key, hash.keys()) {
                if (currElem.attribute("xlink:href") == "#" + key) {
                    currElem.setAttribute("xlink:href", "#" + hash.value(key));
                    break;
                }
            }
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Remover::removeUnreferencedIds()
{
    // find
    QSet<QString> m_allIdList;
    QSet<QString> m_allLinkList;

    QStringList xlinkAttrList;
    xlinkAttrList << "xlink:href" << "inkscape:path-effect" << "inkscape:perspectiveID";

    QStringList urlAttrList;
    urlAttrList << "clip-path" << "fill" << "mask" << "filter";

    QList<SvgElement> list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();

        // collect all id's
        if (currElem.hasAttribute("id"))
            m_allIdList << currElem.id();


        for (int i = 0; i < xlinkAttrList.count(); ++i) {
            if (currElem.hasAttribute(xlinkAttrList.at(i)))
                m_allLinkList << currElem.attribute(xlinkAttrList.at(i)).remove("#");
        }

        for (int i = 0; i < urlAttrList.count(); ++i) {
            if (currElem.hasAttribute(urlAttrList.at(i))) {
                if (currElem.attribute(urlAttrList.at(i)).contains("url"))
                    m_allLinkList << currElem.attribute(urlAttrList.at(i)).remove(RegEx::xlinkUrl);
            }
        }

        if (currElem.hasAttribute("style")) {
            QString style = currElem.style();
            QStringList styleList = style.split(";").filter("url");
            for (int j = 0; j < styleList.count(); ++j)
                m_allLinkList << QString(styleList.at(j)).remove(RegEx::xlinkUrl);
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }


    // remove all linked ids
    foreach (const QString &text, m_allLinkList)
        m_allIdList.remove(text);

    if (Keys::get().flag(Key::RemoveNamedIds)) {
        // skip id's whithout digits
        foreach (const QString &text, m_allIdList) {
            if (!text.contains(QRegExp("\\d")))
                m_allIdList.remove(text);
        }
    }

    // remove
    list.clear();
    list = m_svgElem.childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();

        if (m_allIdList.contains(currElem.id()))
            currElem.removeAttribute("id");

        if (m_allIdList.contains(currElem.attribute("clip-path")))
            currElem.removeAttribute("id");

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

// TODO: remove gradients without xlink and child elements
// TODO: refract
void Remover::removeElements()
{
    // have to use QDomNode insted of SvgElement, because after converting to element
    // detecting and removing of "comment" or "processing instruction" is impossible
    QList<QDomNode> list = Tools::childNodeList(m_dom);
    while (!list.empty()) {
        QDomNode currNode = list.takeFirst();
        SvgElement currElem = currNode.toElement();
        QString currTag = currElem.tagName();

        bool removeThisNode = false;
        if (Props::containers.contains(currTag) && !currElem.hasChildNodes()
            && !Keys::get().flag(Key::KeepEmptyContainer))
            removeThisNode = true;
        else if (currTag == "metadata" && !Keys::get().flag(Key::KeepMetadata))
            removeThisNode = true;
        else if (isInvisibleElementsExist(currElem) && !Keys::get().flag(Key::KeepInvisibleElements))
            removeThisNode = true;
        else if (currTag.contains("sodipodi") && !Keys::get().flag(Key::KeepSodipodiElements))
            removeThisNode = true;
        else if (currTag.contains("inkscape") && !Keys::get().flag(Key::KeepInkscapeElements)
                 && currTag != "inkscape:path-effect")
            removeThisNode = true;
        else if (currTag.contains(QRegExp("^a\\:")) && !Keys::get().flag(Key::KeepIllustratorElements))
            removeThisNode = true;
        else if (currTag.contains(QRegExp("^v\\:")) && !Keys::get().flag(Key::KeepMSVisioElements))
            removeThisNode = true;
        else if (currTag.contains(QRegExp("^c\\:")) && !Keys::get().flag(Key::KeepCorelDrawElements))
            removeThisNode = true;
        else if (currTag == "foreignObject")
            removeThisNode = true;
        else if (currNode.isComment() && !Keys::get().flag(Key::KeepComments))
            removeThisNode = true;
        else if (currNode.isProcessingInstruction() && !Keys::get().flag(Key::KeepProcessingInstruction))
            removeThisNode = true;
        else if (currTag == "title")
            removeThisNode = true;
        else if (currTag == "desc")
            removeThisNode = true;
        else if (currTag == "defs" && currElem.childNodes().isEmpty())
            removeThisNode = true;
        else if (currTag == "image" && !currElem.attribute("xlink:href").contains("base64"))
            removeThisNode = true;
        else if (currElem.isReferenced() && !currElem.hasAttribute("id")
                 && currElem.parentNode().toElement().tagName() == "defs")
            removeThisNode = true;
        else if (!Props::svgElementList.contains(currElem.tagName())
                 && !currNode.isText()
                 && !Keys::get().flag(Key::KeepNonSvgElements)) {
            removeThisNode = true;
        }

        if (removeThisNode) {
            if (currNode.parentNode() == m_dom)
                m_dom.removeChild(currNode);
            else
                currNode.parentNode().removeChild(currNode);
        }

        if (currElem.hasChildNodes())
            list << currElem.childNodeList();
    }


    // distributions-pentubuntu.svg
    // FIXME: switch style attr have to be cleaned before it, and other attr have to be removed
    qreal stdDevLimit = Keys::get().doubleNumber(Key::StdDeviation);
    list = Tools::childNodeList(m_dom);
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();
//        if (currElem.tagName() == "switch") {
//            QStringList attrList = currElem.attributesList();
//            if (attrList.isEmpty() || (attrList.count() == 1 && attrList.first() == "id")) {
                // remove groups
//                foreach (SvgElement elem, currElem.childElemList()) {
//                    if (elem.isGroup())
//                        currElem.removeChild(elem);
//                }
//                ungroupSwitch(currElem);
//            }
        /*} else */if (currElem.tagName() == "feGaussianBlur") {
            // remove "feGaussianBlur" element with "stdDeviation" value
            // lower than "--std-deviation-limit"
            if (!Keys::get().flag(Key::KeepTinyGaussianBlur)) {
                if (currElem.parentNode().childNodes().count() == 1) {
                    // 'stdDeviation' can contains not only one value
                    // we process when it contains only one value
                    if (!currElem.attribute("stdDeviation").contains(QRegExp(",| "))) {
                        bool ok = true;
                        if (currElem.attribute("stdDeviation").toDouble(&ok) <= stdDevLimit) {
                            Q_ASSERT(ok == true);
                            QDomNode node = m_defsElem.removeChild(currElem.parentNode());
                            Q_ASSERT(node.isNull() == false);
                            // TODO: maybe remove xlink, but its slow...
                        }
                    }
                }
            }
        }
        if (currElem.hasChildNodes())
            list << currElem.childNodeList();
    }
}

void Remover::ungroupSwitch(SvgElement elem)
{
    SvgElement parent = elem.parentNode().toElement();
    foreach (const SvgElement &currElem, elem.childElemList())
        parent.insertBefore(currElem, elem);
    parent.removeChild(elem);
}

bool Remover::isInvisibleElementsExist(SvgElement elem)
{
    //remove elements "rect", "pattern" and "image" with height or width <= 0
    if (elem.tagName().contains(QRegExp("rect|pattern|image"))) {
        if (elem.hasAttributes(QStringList() << "width" << "height")) {
            QRectF rect = Tools::viewBoxRect(m_svgElem);
            bool ok = false;
            qreal width  = Tools::convertUnitsToPx(elem.attribute("width"), rect.width()).toDouble(&ok);
            Q_ASSERT(ok == true);
            qreal height = Tools::convertUnitsToPx(elem.attribute("height"), rect.height()).toDouble(&ok);
            Q_ASSERT(ok == true);
            if (width <= 0 || height <= 0)
                return true;
        }
    }

    StringHash hash = elem.styleHash();

    // TODO: finish
    // remove elements with opacity="0"
    if (hash.contains("opacity")) {
        bool ok = true;
        if (elem.styleHash().value("opacity").toDouble(&ok) == 0) {
            Q_ASSERT_X(ok == true, "error", qPrintable(elem.styleHash().value("opacity")));
            return true;
        }
    }

    // remove elements with "display=none"
    if (hash.value("display") == "none")
        return true;

    // remove "path" elements with empty "d" attr
    if (elem.tagName() == "path")
        if (elem.attribute("d").isEmpty())
            return true;

    // A negative value is an error. A value of zero disables rendering of this element.
    if (elem.tagName() == "use") {
        if (elem.hasAttribute("width"))
            if (elem.attribute("width").toDouble() == 0)
                return true;
        if (elem.hasAttribute("height"))
            if (elem.attribute("height").toDouble() == 0)
                return true;
    }

    // remove "polygon", "polyline" elements with empty "points" attr
    if (elem.tagName().contains(QRegExp("polygon|polyline")))
        if (elem.attribute("points").isEmpty())
            return true;

    // remove "circle" elements with "r" <= 0
    if (elem.tagName() == "circle")
        if (elem.attribute("r").toDouble() <= 0)
            return true;

    // remove "ellipse" elements with "rx|ry" <= 0
    if (elem.tagName() == "ellipse")
        if (   elem.attribute("rx").toFloat() <= 0
            || elem.attribute("ry").toFloat() <= 0)
            return true;

    // remove empty "text" elements
    if (elem.tagName() == "text")
        if (elem.text().isEmpty())
            return true;

    // remove "switch" with no attributes or with only "id" attribute
    if (elem.tagName() == "switch" && !elem.hasChildNodes()) {
        if (elem.attributes().count() == 0)
            return true;
        else if (elem.attributes().contains("id") && elem.attributes().count() == 1)
            return true;
    }

    return false;
}

// TODO: remove 'class' attr which has linked to empty object
// aaha_Gear.svg
void Remover::removeAttributes()
{
    QList<SvgElement> list = Tools::childElemList(m_dom);
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst().toElement();

        // NOTE: sodipodi:type="inkscape:offset" supported only by inkscape,
        // and its creates problems in other renderers

        // remove "inkscape.*", but not "inkscape:path-effect"
        if (!Keys::get().flag(Key::KeepInkscapeAttributes))
            cleanAttribute(currElem, QRegExp("inkscape:(?!path-effect).*"));

        if (!Keys::get().flag(Key::KeepSodipodiAttributes))
            cleanAttribute(currElem, QRegExp("sodipodi.*"));

        if (!Keys::get().flag(Key::KeepIllustratorAttributes))
            cleanAttribute(currElem, QRegExp("i\\:.*"));

        if (!Keys::get().flag(Key::KeepMSVisioAttributes))
            cleanAttribute(currElem, QRegExp("v\\:.*"));

        if (!Keys::get().flag(Key::KeepCorelDrawAttributes))
            cleanAttribute(currElem, QRegExp("c\\:.*"));

        if (!Keys::get().flag(Key::KeepDefaultAttributes)) {
            if (currElem.attribute("spreadMethod") == "pad") {
                currElem.removeAttribute("spreadMethod");
            }
        }

        // remove empty attributes
        QDomNamedNodeMap attrMap = currElem.attributes();
        for (int j = 0; j < attrMap.count(); ++j) {
            if (attrMap.item(j).toAttr().value().isEmpty())
                currElem.removeAttribute(attrMap.item(j).toAttr().name());
        }

        if (currElem.hasChildNodes())
            list << currElem.childElemList();
    }
}

void Remover::cleanAttribute(SvgElement elem, QRegExp rx)
{
    foreach (const QString &attr, elem.attributesList()) {
        if (attr.contains(rx)) {
            elem.removeAttribute(attr);
        }
    }
}

QList<StringHash> styleHashList;
StringHash parentHash;
void Remover::processStyleAttr(SvgElement elem)
{
    if (elem.isNull())
        elem = m_svgElem;

    m_usedElemList = Tools::usedElemList(m_svgElem);

    StringHash currHash = elem.styleHash();
    styleHashList << currHash;
    for (int i = 0; i < currHash.count(); ++i)
        parentHash.insert(currHash.keys().at(i), currHash.value(currHash.keys().at(i)));

    QList<SvgElement> nodeList = elem.childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        if (currElem.hasAttribute("style")) {
            StringHash hash = currElem.styleHash();
            cleanStyle(currElem, &hash);

            if (hash.isEmpty())
                currElem.removeAttribute("style");
            else {
                foreach (QString attr, Props::styleAttrList)
                    currElem.removeAttribute(attr);
                currElem.setStyle(Tools::styleHashToString(hash));
            }
        }

        if (currElem.hasChildNodes())
            processStyleAttr(currElem);
    }

    styleHashList.removeLast();
    parentHash.clear();
    foreach (const StringHash &hash, styleHashList) {
        foreach (const QString &attr, hash.keys()) {
            parentHash.insert(attr, hash.value(attr));
        }
    }
}

// removes default value, only if parent style did't contain same attribute
// needed for all inherited attributes
void Remover::cleanStyle(const SvgElement &elem, StringHash *hash)
{
    if (!elem.isText() && !Keys::get().flag(Key::KeepNotAppliedAttributes)) {
        // remove all text based values from non text elements
        foreach (const QString &attr, hash->keys()) {
            if (attr.contains("font") || attr.contains("text"))
                hash->remove(attr);
        }
        hash->remove("writing-mode");
    } else {
        // remove default text style values
        foreach (const QString &attr, Props::textAttributes)
            if (!parentHash.contains(attr))
                removeDefaultValue(hash, attr);
    }

    if (hash->contains("stroke-width"))
        hash->insert("stroke-width", Tools::convertUnitsToPx(hash->value("stroke-width")));
    if (hash->contains("stroke-dashoffset"))
        hash->insert("stroke-dashoffset",
                     Tools::convertUnitsToPx(hash->value("stroke-dashoffset")));

    // remove all fill properties if fill is off
    if (parentHash.contains("fill") && parentHash.value("fill") == "none"
        && !Keys::get().flag(Key::KeepFillProps)) {
        if (hash->value("fill") == "none" || hash->value("fill-opacity") == "0") {
            foreach (const QString &attr, Props::fillList)
                hash->remove(attr);
        }
    } else {
        // remove default values
        foreach (const QString &attr, Props::fillList)
            if (!parentHash.contains(attr))
                removeDefaultValue(hash, attr);
    }

    // remove all stroke properties if stroke is off
    if (  (hash->value("stroke") == "none" || hash->value("stroke-opacity") == "0"
        || hash->value("stroke-width") == "0") && !Keys::get().flag(Key::KeepStrokeProps)) {
        foreach (const QString &attr, Props::strokeList)
            hash->remove(attr);

        if (parentHash.contains("stroke"))
            if (parentHash.value("stroke") != "none")
                hash->insert("stroke", "none");
    } else {
        // remove default values
        QSet<QString> list = Props::strokeList;
        list.remove("stroke");
        list.remove("stroke-width");
        foreach (const QString &attr, list) {
            if (!parentHash.contains(attr))
                removeDefaultValue(hash, attr);
        }
        // trim array
        if (hash->contains("stroke-dasharray"))
            hash->insert("stroke-dasharray", QString(hash->value("stroke-dasharray"))
                                             .replace(", ", ","));
    }

    // remove default values
    foreach (const QString &attr, Props::defaultStyleValues.keys()) {
        if (hash->contains(attr) && !parentHash.contains(attr)) {
            removeDefaultValue(hash, attr);
        }
    }


    // remove clip-rule if no clip-path
    if (hash->contains("clip-rule") && !hash->contains("clip-path"))
        hash->remove("clip-rule");

    // 'enable-background' is only applicable to container elements
    if (!Props::containers.contains(elem.tagName()))
        hash->remove("enable-background");

    if (elem.tagName() != "svg" && elem.tagName() != "pattern" && elem.tagName() != "marker")
        hash->remove("overflow");

    if (!Keys::get().flag(Key::KeepInkscapeAttributes))
        hash->remove("-inkscape-font-specification");

    if (!parentHash.contains("opacity"))
        removeDefaultValue(hash, "opacity");

    foreach (const QString &key, hash->keys()) {
        if (!key.contains("font")) {
            QString value = hash->value(key);
            if (value.contains(QRegExp("(" + RegEx::lengthTypes + ")(\\ +|)$"))
                    && !value.contains("url")) {
                hash->insert(key, Tools::convertUnitsToPx(hash->value(key)));
                value = hash->value(key);
            }
            if (value.contains(QRegExp("^[0-9\\.]+$"))) {
                QString number = Tools::roundNumber(value.toDouble(), Tools::ATTRIBUTES);
                hash->insert(key, number);
            }
        }
    }

    // trim colors
    if (!Keys::get().flag(Key::SkipColorToRRGGBB)) {
        if (hash->contains("fill"))
            if (!hash->value("fill").contains(QRegExp("none|url")))
                hash->insert("fill", Tools::trimColor(hash->value("fill")));
        if (hash->contains("stroke"))
            if (!hash->value("stroke").contains(QRegExp("none|url")))
                hash->insert("stroke", Tools::trimColor(hash->value("stroke")));
        if (hash->contains("stop-color"))
            hash->insert("stop-color", Tools::trimColor(hash->value("stop-color")));
        if (hash->contains("color"))
            hash->insert("color", Tools::trimColor(hash->value("color")));
        if (hash->contains("flood-color"))
            hash->insert("flood-color", Tools::trimColor(hash->value("flood-color")));
    }

    if (!parentHash.contains("stroke-width"))
        removeDefaultValue(hash, "stroke-width");

    // remove style props which already defined in parent style
    // ignore used/linked elements and opacity
    foreach (const QString &attr, parentHash.keys()) {
        if (attr != "opacity" && !m_usedElemList.contains(elem.id())) {
            if (hash->contains(attr))
                if (hash->value(attr) == parentHash.value(attr))
                    hash->remove(attr);
        }
    }
}

void Remover::removeDefaultValue(StringHash *hash, const QString &name)
{
    QVariantList list = Props::defaultStyleValues.values(name);
    foreach (const QVariant &value, list) {
        if (value.type() == QVariant::String) {
            if (value == hash->value(name)) {
                hash->remove(name);
                break;
            }
        } else if (!hash->value(name).isEmpty()) {
            bool ok = false;
            if (hash->value(name).toDouble(&ok) == value.toDouble()) {
                Q_ASSERT(ok == true);
                hash->remove(name);
                break;
            }
        }
    }
}

// TODO: remove "symbol"
// TODO: apply transform from group
// rasor_SQL_Backup.svg

void Remover::removeGroups()
{
    // get all 'use' links to prevent ungouping element linked to this use
    m_usedElemList = Tools::usedElemList(m_svgElem);
    QList<SvgElement> nodeList = m_svgElem.childElemList();

    nodeList = m_svgElem.childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        StringHash currStyle = currElem.styleHash();
        // ungroup only if: current group is not linked to any 'use',
        // not contains mask, clip-path, filter, opacity attributes
        if (currElem.tagName() == "g" && !m_usedElemList.contains(currElem.id())
            && !currStyle.contains("mask") && !currStyle.contains("clip-path")
            && !currStyle.contains("opacity") && !currStyle.contains("filter"))
        {
            removeGroup(currElem);
        }
    }

    // ungrouping is pretty brutal, so we need to fix some issues
    nodeList = m_svgElem.childElemList();
    QList<SvgElement> gNodeList;
    SvgElement lastGNode;
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();

        if (currElem.isGroup()) {
            if (lastGNode.isNull()) {
                gNodeList << currElem;
                lastGNode = currElem;
            } else {
                bool flag = true;
                if (currElem.style() == lastGNode.style()
                    && currElem.hasAttribute("style") && !currElem.style().contains("opacity")
                    && currElem.attribute("transform") == lastGNode.attribute("transform")) {
                    gNodeList << currElem;
                } else {
                    flag = false;
                }

                if (!flag || nodeList.isEmpty()) {
                    if (gNodeList.count() > 1)
                        mergeGroups(gNodeList);
                    lastGNode.clear();
                    gNodeList.clear();
                }
            }
        } else {
            lastGNode.clear();
            gNodeList.clear();
        }
    }
}

void Remover::removeGroup(SvgElement elem)
{
    QList<SvgElement> nodeList = elem.childElemList();
    SvgElement newGroupElem;

    SvgElement firstParentG = elem;
    while (firstParentG.parentNode().toElement().tagName() != "svg")
        firstParentG = firstParentG.parentNode().toElement();

    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        StringHash currStyle = currElem.styleHash();
        // ungroup only if: current group is not linked to any 'use',
        // not contains mask, clip-path, filter, opacity attributes
        if (currElem.tagName() == "g" && !m_usedElemList.contains(currElem.id())
            && !currStyle.contains("mask") && !currStyle.contains("clip-path")
            && !currStyle.contains("opacity") && !currStyle.contains("filter")) {
            // TODO: ignore current group, but process childs
            removeGroup(currElem);
            newGroupElem.clear();
        } else {
            if (newGroupElem.isNull())
                newGroupElem = genGroup(currElem, firstParentG);

            if (newGroupElem.isNull())
                m_svgElem.insertBefore(currElem, firstParentG);
            else
                newGroupElem.appendChild(currElem);
        }
    }
    elem.parentNode().removeChild(elem);
}

SvgElement Remover::genGroup(SvgElement currElem, SvgElement parentGroup)
{
    QString transformAttr;
    QString styleAttr;
    QString idAttr;

    QStringList styles;
    SvgElement parentElem = currElem.parentNode().toElement();
    if (parentGroup.hasAttribute("id"))
        idAttr = parentGroup.id();
    while (parentElem.tagName() != "svg") {
        if (parentElem.hasAttribute("transform"))
            transformAttr = parentElem.attribute("transform") + " " + transformAttr;
        if (parentElem.hasAttribute("style"))
            styles.prepend(parentElem.style());
        parentElem = parentElem.parentNode().toElement();
    }

    // remove identical styles
    StringHash newStyleHash;
    if (!styles.isEmpty()) {
        newStyleHash = Tools::splitStyle(styles.first());
        for (int i = 1; i < styles.count(); ++i) {
            StringHash hash = Tools::splitStyle(styles.at(i));
            QStringList keys = hash.keys();
            for (int j = 0; j < keys.count(); ++j)
                newStyleHash.insert(keys.at(j), hash.value(keys.at(j)));
            styles.removeAt(i);
            i--;
        }
    }

    QList<QString> keyList = newStyleHash.keys();
    for (int k = 0; k < keyList.count(); ++k)
        styleAttr += keyList.at(k) + ":" + newStyleHash.value(keyList.at(k)) + ";";
    styleAttr.chop(1);

    if (!transformAttr.isEmpty())
        transformAttr = Transform(transformAttr).simplified();

    if (!transformAttr.isEmpty() || !styleAttr.isEmpty()) {
        SvgElement newGElem = m_dom.createElement("g");
        if (!transformAttr.isEmpty())
            newGElem.setAttribute("transform", transformAttr);
        if (!styleAttr.isEmpty())
            newGElem.setStyle(styleAttr);
        if (!idAttr.isEmpty())
            newGElem.setAttribute("id", idAttr);
        return m_svgElem.insertBefore(newGElem, parentGroup).toElement();
    }
    return SvgElement();
}

void Remover::mergeGroups(QList<SvgElement> gNodeList)
{
    SvgElement newGElem = m_dom.createElement("g");
    newGElem = m_svgElem.insertBefore(newGElem, gNodeList.first()).toElement();
    newGElem.setStyle(gNodeList.first().style());
    newGElem.setAttribute("transform", gNodeList.first().attribute("transform"));
    Q_ASSERT(newGElem.isNull() == false);
    for (int i = 0; i < gNodeList.count(); ++i) {
        SvgElement cElem = gNodeList.at(i);
        foreach (SvgElement elem, cElem.childElemList())
            newGElem.appendChild(elem);
        cElem.parentNode().removeChild(cElem);
    }
}
