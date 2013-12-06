#include "keys.h"
#include "remover.h"

// TODO: remove equal styles in used element and it use
// TODO: remove items which out of viewbox
//       Anonymous_butterfly_and_flowers.svg
// TODO: remove "tspan" without attributes
// TODO: remove "feBlend" element with in or in2 linked to empty object
//       babayasin_air_hammer.svg
// TODO: xlink:href could not contains uri with spaces, remove this elements

Remover::Remover(XMLDocument *doc) : BaseCleaner(doc)
{
}

void Remover::cleanSvgElementAttribute()
{
    bool isXlinkUsed = false;
    QList<SvgElement> nodeList = svgElement().childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        if (currElem.hasAttribute("xlink:href")) {
            isXlinkUsed = true;
            break;
        }
        if (currElem.hasChildren())
            nodeList << currElem.childElemList();
    }

    QSet<QString> ignoreAttr = Props::styleAttributes;
    ignoreAttr << "xmlns" << "width" << "height" << "viewBox";

    if (Keys::get().flag(Key::KeepSvgVersion))
        ignoreAttr << "version";
    if (isXlinkUsed)
        ignoreAttr << "xmlns:xlink";
    foreach (const QString &attrName, svgElement().attributesList()) {
        if (!ignoreAttr.contains(attrName)) {
            svgElement().removeAttribute(attrName);
        }
    }

    if (isXlinkUsed && !svgElement().hasAttribute("xmlns:xlink"))
        svgElement().setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    // dirty way, but svg cannot be processed by default style cleaning func,
    // because in svg node we cannot remove default values
    if (svgElement().hasAttribute("display")) {
        if (svgElement().attribute("display") == "inline")
            svgElement().removeAttribute("display");
    }
}

void Remover::removeUnusedDefs()
{
    QSet<QString> defsIdList;
    defsIdList << "";
    while (!defsIdList.isEmpty()) {
        defsIdList.clear();

        foreach (const SvgElement &elem, defsElement().childElemList())
            if (elem.tagName() != "clipPath")
                defsIdList << elem.id();

        QList<SvgElement> list = svgElement().childElemList();
        while (!list.empty()) {
            SvgElement currElem = list.takeFirst();
            if (currElem.hasAttribute("xlink:href"))
                defsIdList.remove(currElem.attribute("xlink:href").remove("#"));
            foreach (const QString &attrName, Props::linkableStyleAttributes) {
                if (currElem.hasAttribute(attrName)) {
                    QString url = currElem.attribute(attrName);
                    defsIdList.remove(url.mid(5, url.size()-6));
                }
            }
            if (currElem.hasChildren())
                list << currElem.childElemList();
        }

        foreach (const SvgElement &elem, defsElement().childElemList()) {
            if (defsIdList.contains(elem.id()))
                defsElement().removeChild(elem);
        }
    }
}

void Remover::removeUnusedXLinks()
{
    QSet<QString> xlinkSet;
    QSet<QString> idSet;
    QList<SvgElement> list = svgElement().childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("xlink:href")) {
            if (!currElem.attribute("xlink:href").contains("base64"))
                xlinkSet << currElem.attribute("xlink:href").remove("#");
        }
        if (currElem.hasAttribute("id"))
            idSet << currElem.id();

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
    QSet<QString>::iterator it = idSet.begin();
    while (it != idSet.end()) {
        xlinkSet.remove(*it);
        ++it;
    }

    list = svgElement().childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        if (currElem.hasAttribute("xlink:href")) {
            if (xlinkSet.contains(currElem.attribute("xlink:href").remove("#"))) {
                currElem.removeAttribute("xlink:href");
            }
        }
        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

// TODO: detect not only equal, but with diff less than 1%
// TODO: add other defs
void Remover::removeDuplicatedDefs()
{
    StringHash xlinkToReplace;
    QList<SvgElement> defsList = Tools::childElemList(defsElement());

    // using of structure is faster than actual node accessing
    QList<DefsElemStruct> elemStructList;
    for (int i = 0; i < defsList.count(); ++i) {
        SvgElement elem = defsList.at(i);
        DefsElemStruct es = { elem, elem.tagName(), elem.hasChildren(), elem.attributesMap() };
        elemStructList << es;
    }

    for (int i = 0; i < elemStructList.count(); ++i) {
        DefsElemStruct des1 = elemStructList.at(i);
        QString id1 = des1.attrMap.value("id");
        if (des1.tagName == "linearGradient" || des1.tagName == "radialGradient") {
            for (int j = i; j < elemStructList.count(); ++j) {
                DefsElemStruct des2 = elemStructList.at(j);
                if (des1.tagName == des2.tagName && des1.attrMap.value("id") != des2.attrMap.value("id")) {
                    if ((des1.hasChildren && des2.hasChildren) || (!des1.hasChildren && !des2.hasChildren)) {
                        bool isRemove = false;
                        if (des1.tagName == "linearGradient")
                            isRemove = Tools::isAttributesEqual(des1.attrMap, des2.attrMap,
                                                            Props::linearGradient);
                        else if (des1.tagName == "radialGradient")
                            isRemove = Tools::isAttributesEqual(des1.attrMap, des2.attrMap,
                                                            Props::radialGradient);
                        if (isRemove) {
                            if (des1.hasChildren && des2.hasChildren)
                                isRemove = Tools::isGradientsEqual(des1.elem, des2.elem);

                            if (isRemove) {
                                QString id2 = des2.attrMap.value("id");
                                if (xlinkToReplace.values().contains(id2)) {
                                    for (int k = 0; k < xlinkToReplace.keys().count(); ++k) {
                                        if (xlinkToReplace.value(xlinkToReplace.keys().at(k)) == id2)
                                            xlinkToReplace.insert(xlinkToReplace.keys().at(k), id1);
                                    }
                                }
                                xlinkToReplace.insert(id2, id1);
                                defsElement().removeChild(des2.elem);
                                elemStructList.removeAt(j);
                                j--;
                            }
                        }
                    }
                }
            }
        }
    }
    updateXLinks(xlinkToReplace);
}

void Remover::removeUnreferencedIds()
{
    // find
    QSet<QString> m_allIdList;
    QSet<QString> m_allLinkList;

    QStringList xlinkAttrList;
    xlinkAttrList << "xlink:href" << "inkscape:path-effect" << "inkscape:perspectiveID";

    QStringList urlAttrList = Props::linkableStyleAttributes;

    QList<SvgElement> list = svgElement().childElemList();
    while (!list.empty()) {
        SvgElement elem = list.takeFirst();
        QStringList attrList = elem.attributesList();

        // collect all id's
        if (attrList.contains("id"))
            m_allIdList << elem.id();

        for (int i = 0; i < xlinkAttrList.count(); ++i) {
            if (attrList.contains(xlinkAttrList.at(i)))
                m_allLinkList << elem.attribute(xlinkAttrList.at(i)).remove("#");
        }

        for (int i = 0; i < urlAttrList.count(); ++i) {
            QString attr = urlAttrList.at(i);
            if (attrList.contains(attr)) {
                QString attrValue = elem.attribute(attr);
                if (attrValue.contains("url"))
                    attrValue = attrValue.mid(5, attrValue.size()-6);
                    m_allLinkList << attrValue;
            }
        }

        if (elem.hasChildren())
            list << elem.childElemList();
    }

    // remove all linked ids
    foreach (const QString &text, m_allLinkList)
        m_allIdList.remove(text);

    if (Keys::get().flag(Key::KeepNamedIds)) {
        // skip id's whithout digits
        foreach (const QString &text, m_allIdList) {
            if (!text.contains(QRegExp("\\d")))
                m_allIdList.remove(text);
        }
    }

    // remove
    list = svgElement().childElemList();
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();

        if (m_allIdList.contains(currElem.id()))
            currElem.removeAttribute("id");

        if (m_allIdList.contains(currElem.attribute("clip-path")))
            currElem.removeAttribute("id");

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Remover::removeElements()
{
    // have to use XMLNode insted of SvgElement, because after converting to element
    // detecting and removing of "comment" or "processing instruction" is impossible
    QList<XMLNode *> nodeList = Tools::childNodeList(document());
    while (!nodeList.empty()) {
        XMLNode *currNode = nodeList.takeFirst();
        SvgElement currElem = SvgElement(currNode->ToElement());

        bool removeThisNode = false;
        if (!currElem.isNull()) {
            QString currTag = currElem.tagName();
            if (Props::containers.contains(currTag) && !currElem.hasChildren()
                && !Keys::get().flag(Key::KeepEmptyContainer))
                removeThisNode = true;
            else if (currTag == "metadata" && !Keys::get().flag(Key::KeepMetadata))
                removeThisNode = true;
            else if (currTag.contains("sodipodi") && !Keys::get().flag(Key::KeepSodipodiElements))
                removeThisNode = true;
            else if (currTag.contains("inkscape") && !Keys::get().flag(Key::KeepInkscapeElements)
                     && currTag != "inkscape:path-effect")
                removeThisNode = true;
            else if (currTag.startsWith("a:") && !Keys::get().flag(Key::KeepIllustratorElements))
                removeThisNode = true;
            else if (currTag.startsWith("v:") && !Keys::get().flag(Key::KeepMSVisioElements))
                removeThisNode = true;
            else if (currTag.startsWith("c:") && !Keys::get().flag(Key::KeepCorelDrawElements))
                removeThisNode = true;
            else if (currTag == "foreignObject")
                removeThisNode = true;
            else if (currTag == "title")
                removeThisNode = true;
            else if (currTag == "desc")
                removeThisNode = true;
            else if (currTag == "defs" && !currElem.hasChildren())
                removeThisNode = true;
            else if (currTag == "linearGradient" || currTag == "radialGradient") {
                if (!currElem.hasChildren() && !currElem.hasAttribute("xlink:href"))
                    removeThisNode = true;
            } else if (currTag == "image" && !currElem.attribute("xlink:href").contains("base64"))
                removeThisNode = true;
            else if (currElem.isReferenced() && !currElem.hasAttribute("id")
                     && currElem.parentNode().tagName() == "defs")
                removeThisNode = true;
            else if (!Props::svgElementList.contains(currElem.tagName())
                     && !currNode->ToText() != 0
                     && !Keys::get().flag(Key::KeepNonSvgElements)) {
                removeThisNode = true;
            } else if (isInvisibleElementsExist(currElem) && !Keys::get().flag(Key::KeepInvisibleElements))
                removeThisNode = true;
        }

        if (currNode->ToComment() != 0 && !Keys::get().flag(Key::KeepComments))
            removeThisNode = true;
        else if (currNode->ToDeclaration() != 0 && !Keys::get().flag(Key::KeepProcessingInstruction))
            removeThisNode = true;
        else if (QString(currNode->Value()).contains(QRegExp("DOCTYPE|ENTITY|\\]\\>")) && !Keys::get().flag(Key::KeepProlog))
            removeThisNode = true;
        else if (currNode->ToText() != 0) {
            if (currNode->ToText()->Value() == 0)
                removeThisNode = true;
        }

        if (removeThisNode)
            currNode->Parent()->DeleteChild(currNode);

        if (!currNode->NoChildren())
            nodeList << Tools::childNodeList(currNode);
    }

    // ungroup "a" element
    QList<SvgElement> elemList = Tools::childElemList(svgElement());
    SvgElement prevElem;
    while (!elemList.empty()) {
        SvgElement currElem = elemList.takeFirst();
        if (currElem.tagName() == "a") {
            if ((currElem.hasAttribute("id") && currElem.attributesCount() == 1)
                || currElem.attributesCount() == 0)
            {
                foreach (const SvgElement &elem, currElem.childElemList())
                    prevElem.insertBefore(elem, currElem);
                currElem.parentNode().removeChild(currElem);
            }
        }
        prevElem = currElem;
        if (currElem.hasChildren())
            elemList << Tools::childElemList(currElem);
    }

    // distributions-pentubuntu.svg
    // FIXME: switch style attr have to be cleaned before it, and other attr have to be removed
    qreal stdDevLimit = Keys::get().doubleNumber(Key::StdDeviation);
    elemList = Tools::childElemList(svgElement());
    while (!elemList.empty()) {
        SvgElement currElem = elemList.takeFirst();
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
        if (currElem.tagName() == "feGaussianBlur") {
            // remove "feGaussianBlur" element with "stdDeviation" value
            // lower than "--std-deviation-limit"
            // FIXME: check for stdDeviation with transform of all linked element applied
            if (!Keys::get().flag(Key::KeepTinyGaussianBlur)) {
                if (currElem.parentNode().childElementCount() == 1) {
                    // 'stdDeviation' can contains not only one value
                    // we process when it contains only one value
                    QString stdDev = currElem.attribute("stdDeviation");
                    if (!stdDev.contains(",") && !stdDev.contains(" ")) {
                        bool ok = true;
                        if (stdDev.toDouble(&ok) <= stdDevLimit) {
                            Q_ASSERT(ok == true);
                            defsElement().removeChild(currElem.parentNode());
                        }
                    }
                }
            }
        }
        if (currElem.hasChildren())
            elemList << Tools::childElemList(currElem);
    }
}

void Remover::ungroupSwitch(SvgElement elem)
{
    SvgElement parent = elem.parentNode();
    foreach (const SvgElement &currElem, elem.childElemList())
        parent.insertBefore(currElem, elem);
    parent.removeChild(elem);
}

bool Remover::isInvisibleElementsExist(const SvgElement &elem)
{
    QString tagName = elem.tagName();
    //remove elements "rect", "pattern" and "image" with height or width <= 0
    if (tagName == "rect" || tagName == "pattern" || tagName == "image") {
        if (elem.hasAttribute("width") && elem.hasAttribute("height")) {
            QRectF rect = Tools::viewBoxRect(svgElement());
            bool ok = false;
            qreal width  = Tools::convertUnitsToPx(elem.attribute("width"), rect.width()).toDouble(&ok);
            Q_ASSERT(ok == true);
            qreal height = Tools::convertUnitsToPx(elem.attribute("height"), rect.height()).toDouble(&ok);
            Q_ASSERT(ok == true);
            if (width <= 0 || height <= 0)
                return true;
        }
    }

    // TODO: ungroup flowPara with only "id" attr
    // ryanlerch_OCAL_Introduction.svg

    // remove elements with opacity="0"
    if (elem.hasAttribute("opacity")) {
        if (elem.doubleAttribute("opacity") <= 0) {
            return true;
        }
    }

    // remove elements with "display=none"
    if (elem.hasAttribute("display")) {
        if (elem.attribute("display") == "none")
            return true;
    }

    // remove "path" elements with empty "d" attr
    if (tagName == "path")
        if (elem.attribute("d").isEmpty())
            return true;

    // A negative value is an error. A value of zero disables rendering of this element.
    if (tagName == "use") {
        if (elem.hasAttribute("width"))
            if (elem.attribute("width").toDouble() == 0)
                return true;
        if (elem.hasAttribute("height"))
            if (elem.attribute("height").toDouble() == 0)
                return true;
    }

    // remove "polygon", "polyline" elements with empty "points" attr
    if (tagName == "polygon" || tagName == "polyline")
        if (elem.attribute("points").isEmpty())
            return true;

    // remove "circle" elements with "r" <= 0
    if (tagName == "circle")
        if (elem.attribute("r").toDouble() <= 0)
            return true;

    // remove "ellipse" elements with "rx|ry" <= 0
    if (tagName == "ellipse")
        if (   elem.attribute("rx").toFloat() <= 0
            || elem.attribute("ry").toFloat() <= 0)
            return true;

    // remove "switch" with no attributes or with only "id" attribute
    if (tagName == "switch" && !elem.hasChildren()) {
        if (elem.attributesCount() == 0)
            return true;
        else if (elem.hasAttribute("id") && elem.attributesCount() == 1)
            return true;
    }

    return false;
}

// TODO: remove 'class' attr which has linked to empty object
// aaha_Gear.svg
void Remover::removeAttributes()
{
    QList<SvgElement> list = Tools::childElemList(document());
    while (!list.empty()) {
        SvgElement currElem = list.takeFirst();
        QStringList attrList = currElem.attributesList();
        if (!attrList.isEmpty()) {
            // sodipodi:type="inkscape:offset" supported only by inkscape,
            // and its creates problems in other renders
            if (!Keys::get().flag(Key::KeepInkscapeAttributes))
                cleanAttribute(currElem, "inkscape", attrList);

            if (!Keys::get().flag(Key::KeepSodipodiAttributes))
                cleanAttribute(currElem, "sodipodi", attrList);

            if (!Keys::get().flag(Key::KeepIllustratorAttributes)) {
                cleanAttribute(currElem, "i:", attrList);
                cleanAttribute(currElem, "a:", attrList);
            }

            if (!Keys::get().flag(Key::KeepMSVisioAttributes))
                cleanAttribute(currElem, "v:", attrList);

            if (!Keys::get().flag(Key::KeepCorelDrawAttributes))
                cleanAttribute(currElem, "c:", attrList);

            if (!Keys::get().flag(Key::KeepDefaultAttributes)) {
                if (currElem.attribute("spreadMethod") == "pad")
                    currElem.removeAttribute("spreadMethod");
            }

            // remove empty attributes
            for (const XMLAttribute *child = currElem.xmlElement()->FirstAttribute(); child;
                 child = child->Next()) {
                if (child->Value() == 0)
                    currElem.xmlElement()->DeleteAttribute(child->Name());
            }
        }

        if (currElem.hasChildren())
            list << currElem.childElemList();
    }
}

void Remover::cleanAttribute(SvgElement &elem, const QString &startWith, QStringList &attrList)
{
    foreach (const QString &attrName, attrList) {
        if (attrName.startsWith(startWith)) {
            elem.removeAttribute(attrName);
            attrList.removeOne(attrName);
        }
    }
}

void Remover::processStyleAttr(SvgElement elem)
{
    // TODO: process svgElement() too
    if (elem.isNull()) {
        elem = svgElement();
        m_usedElemList = Tools::usedElemList(svgElement());
    }

    StringHash currHash = elem.styleHash();
    styleHashList << currHash;
    for (int i = 0; i < currHash.count(); ++i)
        parentHash.insert(currHash.keys().at(i), currHash.value(currHash.keys().at(i)));

    QList<SvgElement> nodeList = elem.childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        StringHash hash = currElem.styleHash();
        cleanStyle(currElem, hash);
        foreach (QString attr, Props::styleAttributes)
            currElem.removeAttribute(attr);
        if (!hash.isEmpty()) {
            foreach (const QString &str, hash.keys())
                currElem.setAttribute(str, hash.value(str));
        }
        if (currElem.hasChildren())
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
void Remover::cleanStyle(const SvgElement &elem, StringHash &hash)
{
    if (!elem.isText() && !Keys::get().flag(Key::KeepNotAppliedAttributes)) {
        // remove all text based values from non text elements
        foreach (const QString &attr, hash.keys()) {
            if (attr.contains("font") || attr.contains("text"))
                hash.remove(attr);
        }
        hash.remove("writing-mode");
    } else {
        // remove default text style values
        foreach (const QString &attr, Props::textAttributes)
            if (!parentHash.contains(attr))
                removeDefaultValue(hash, attr);
    }

    if (hash.contains("stroke-width"))
        hash.insert("stroke-width", Tools::convertUnitsToPx(hash.value("stroke-width")));
    if (hash.contains("stroke-dashoffset"))
        hash.insert("stroke-dashoffset",
                     Tools::convertUnitsToPx(hash.value("stroke-dashoffset")));

    // remove all fill properties if fill is off
    if (parentHash.contains("fill") && parentHash.value("fill") == "none"
        && !Keys::get().flag(Key::KeepFillProps)) {
        if (hash.value("fill") == "none" || hash.value("fill-opacity") == "0") {
            foreach (const QString &attr, Props::fillList)
                hash.remove(attr);
        }
    } else {
        // remove default values
        foreach (const QString &attr, Props::fillList)
            if (!parentHash.contains(attr))
                removeDefaultValue(hash, attr);
    }

    // remove all stroke properties if stroke is off
    if (  (hash.value("stroke") == "none" || hash.value("stroke-opacity") == "0"
        || hash.value("stroke-width") == "0") && !Keys::get().flag(Key::KeepStrokeProps)) {
        foreach (const QString &attr, Props::strokeList)
            hash.remove(attr);

        if (parentHash.contains("stroke"))
            if (parentHash.value("stroke") != "none")
                hash.insert("stroke", "none");
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
        if (hash.contains("stroke-dasharray"))
            hash.insert("stroke-dasharray", QString(hash.value("stroke-dasharray"))
                                             .replace(", ", ","));
    }

    // remove default values
    foreach (const QString &attr, Props::defaultStyleValues.keys()) {
        if (hash.contains(attr) && !parentHash.contains(attr)) {
            removeDefaultValue(hash, attr);
        }
    }

    // remove clip-rule if no clip-path
    if (hash.contains("clip-rule") && !hash.contains("clip-path"))
        hash.remove("clip-rule");

    // 'enable-background' is only applicable to container elements
    if (!Props::containers.contains(elem.tagName()))
        hash.remove("enable-background");

    if (elem.tagName() != "svg" && elem.tagName() != "pattern" && elem.tagName() != "marker")
        hash.remove("overflow");

    if (!Keys::get().flag(Key::KeepInkscapeAttributes))
        hash.remove("-inkscape-font-specification");

    if (!parentHash.contains("opacity"))
        removeDefaultValue(hash, "opacity");

    foreach (const QString &key, hash.keys()) {
        if (!key.contains("font")) {
            QString value = hash.value(key);
            QString lengthType = value.right(2);
            if (Props::lengthTypes.contains(lengthType) && !value.contains("url"))
            {
                hash.insert(key, Tools::convertUnitsToPx(hash.value(key)));
                value = hash.value(key);
            }

            bool ok = false;
            value.toDouble(&ok);
            if (ok) {
                QString number = Tools::roundNumber(value.toDouble(), Tools::ATTRIBUTE);
                hash.insert(key, number);
            }
        }
    }

    // trim colors
    if (!Keys::get().flag(Key::SkipColorToRRGGBB)) {
        if (hash.contains("fill"))
            if (hash.value("fill") != "none" && !hash.value("fill").contains("url"))
                hash.insert("fill", Tools::trimColor(hash.value("fill")));
        if (hash.contains("stroke"))
            if (hash.value("stroke") != "none" && !hash.value("stroke").contains("url"))
                hash.insert("stroke", Tools::trimColor(hash.value("stroke")));
        if (hash.contains("stop-color"))
            hash.insert("stop-color", Tools::trimColor(hash.value("stop-color")));
        if (hash.contains("color"))
            hash.insert("color", Tools::trimColor(hash.value("color")));
        if (hash.contains("flood-color"))
            hash.insert("flood-color", Tools::trimColor(hash.value("flood-color")));
    }

    if (!parentHash.contains("stroke-width"))
        removeDefaultValue(hash, "stroke-width");

    // remove style props which already defined in parent style
    // ignore used/linked elements and opacity
    foreach (const QString &attr, parentHash.keys()) {
        if (attr != "opacity" && !m_usedElemList.contains(elem.id())) {
            if (hash.contains(attr))
                if (hash.value(attr) == parentHash.value(attr))
                    hash.remove(attr);
        }
    }
}

void Remover::removeDefaultValue(StringHash &hash, const QString &name)
{
    QVariantList list = Props::defaultStyleValues.values(name);
    foreach (const QVariant &value, list) {
        if (value.type() == QVariant::String) {
            if (value == hash.value(name)) {
                hash.remove(name);
                break;
            }
        } else if (!hash.value(name).isEmpty()) {
            bool ok = false;
            if (hash.value(name).toDouble(&ok) == value.toDouble()) {
                Q_ASSERT(ok == true);
                hash.remove(name);
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
    m_usedElemList = Tools::usedElemList(svgElement());
    QList<SvgElement> nodeList = svgElement().childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        // ungroup only if: current group is not linked to any 'use',
        // not contains mask, clip-path, filter, opacity attributes
        if (currElem.tagName() == "g" && !m_usedElemList.contains(currElem.id())
            && !currElem.hasAttribute("mask") && !currElem.hasAttribute("clip-path")
            && !currElem.hasAttribute("opacity") && !currElem.hasAttribute("filter"))
        {
            removeGroup(currElem);
        }
    }

    // ungrouping is pretty brutal, so we need to fix some issues
    // TODO: write what this part do...
    nodeList = svgElement().childElemList();
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
                if (!currElem.hasAttribute("opacity") && !m_usedElemList.contains(currElem.id())
                    && currElem.styleHash() == lastGNode.styleHash() && !currElem.styleHash().isEmpty()
                    && currElem.attribute("transform") == lastGNode.attribute("transform"))
                {
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

void Remover::removeGroup(SvgElement &elem)
{
    SvgElement firstParentG = elem;
    while (firstParentG.parentNode().tagName() != "svg")
        firstParentG = firstParentG.parentNode();

    SvgElement newGroupElem;
    QList<SvgElement> nodeList = elem.childElemList();
    while (!nodeList.empty()) {
        SvgElement currElem = nodeList.takeFirst();
        // ungroup only if: current group is not linked to any 'use',
        // not contains mask, clip-path, filter, opacity attributes
        if (currElem.tagName() == "g" && !m_usedElemList.contains(currElem.id())
            && !currElem.hasAttribute("mask") && !currElem.hasAttribute("clip-path")
            && !currElem.hasAttribute("opacity") && !currElem.hasAttribute("filter"))
        {
            // TODO: ignore current group, but process childs
            removeGroup(currElem);
            newGroupElem.clear();
        } else {
            if (newGroupElem.isNull())
                newGroupElem = genGroup(currElem, firstParentG);

            if (newGroupElem.isNull())
                svgElement().insertBefore(currElem, firstParentG);
            else
                newGroupElem.appendChild(currElem);
        }
    }
    elem.parentNode().removeChild(elem);
}

SvgElement Remover::genGroup(SvgElement &currElem, SvgElement &parentGroup)
{
    QString transformAttr;
    QString idAttr;

    // TODO: rewrite style merge algorithm
    QList<StringHash> styles;
    SvgElement parentElem = currElem.parentNode();
    if (parentGroup.hasAttribute("id"))
        idAttr = parentGroup.id();
    while (parentElem.tagName() != "svg") {
        if (parentElem.hasAttribute("transform"))
            transformAttr = parentElem.attribute("transform") + " " + transformAttr;
        StringHash hash = parentElem.styleHash();
        if (!hash.isEmpty())
            styles.prepend(hash);
        parentElem = parentElem.parentNode();
    }

    // merge styles
    StringHash newStyleHash;
    if (!styles.isEmpty()) {
        newStyleHash = styles.first();
        for (int i = 1; i < styles.count(); ++i) {
            StringHash hash = styles.at(i);
            QStringList keys = hash.keys();
            for (int j = 0; j < keys.count(); ++j)
                newStyleHash.insert(keys.at(j), hash.value(keys.at(j)));
            styles.removeAt(i);
            i--;
        }
    }

    if (!transformAttr.isEmpty())
        transformAttr = Transform(transformAttr).simplified();

    if (!transformAttr.isEmpty() || !newStyleHash.isEmpty()) {
        SvgElement newGElem = SvgElement(document()->NewElement("g"));
        if (!transformAttr.isEmpty())
            newGElem.setAttribute("transform", transformAttr);
        if (!newStyleHash.isEmpty())
            newGElem.setStylesFromHash(newStyleHash);
        if (!idAttr.isEmpty())
            newGElem.setAttribute("id", idAttr);
        return svgElement().insertBefore(newGElem, parentGroup);
    }
    return SvgElement();
}

void Remover::mergeGroups(QList<SvgElement> &gNodeList)
{
    SvgElement newGElem = SvgElement(document()->NewElement("g"));
    SvgElement oldGElem = gNodeList.first();
    newGElem = svgElement().insertBefore(newGElem, oldGElem);
    foreach (const QString &attr, Props::styleAttributes) {
        if (oldGElem.hasAttribute(attr)) {
            newGElem.setAttribute(attr, oldGElem.attribute(attr));
        }
    }
    newGElem.setAttribute("transform", gNodeList.first().attribute("transform"));
    Q_ASSERT(newGElem.isNull() == false);
    for (int i = 0; i < gNodeList.count(); ++i) {
        SvgElement cElem = gNodeList.at(i);
        foreach (SvgElement elem, cElem.childElemList())
            newGElem.appendChild(elem);
        cElem.parentNode().removeChild(cElem);
    }
}
