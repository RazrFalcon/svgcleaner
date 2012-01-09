#include <QScrollArea>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDir>
#include <QtDebug>

#include "guiinfo.h"

GuiInfo::GuiInfo(QObject *parent) :
    QObject(parent)
{
    QDomDocument doc;
#ifdef Q_OS_WIN
    doc.setContent(loadFile("C:/Users/Razr/Desktop/svgcleaner/interface.xml"));
#else
    doc.setContent(loadFile("/usr/share/svgcleaner/interface.xml"));
#endif
    presetMap.insert("None",doc);

#ifdef Q_OS_WIN
    QDir presetDir("presets");
#else
    QDir presetDir("/usr/share/svgcleaner/presets/");
#endif
    QFileInfoList files = presetDir.entryInfoList(QStringList("*.preset"));
    QDir presetDir2(QDir::homePath()+"/.config/svgcleaner/preset/");
    files += presetDir2.entryInfoList(QStringList("*.preset"));
    for (int i = 0; i < files.count(); ++i) {
        QDomDocument dom;
        dom.setContent(loadFile(files.at(i).absoluteFilePath()));
        QString presetName = dom.namedItem("preset").toElement().attribute("name");
        presetMap.insert(presetName,dom);
    }
}

QString GuiInfo::loadFile(const QString &file)
{
    QFile inputFile(file);
    inputFile.open(QFile::ReadOnly);
    QTextStream textStream(&inputFile);
    QString str = textStream.readAll();
    inputFile.close();
    return str;
}

QStringList GuiInfo::presets()
{
    return presetMap.keys();
}

QString GuiInfo::presetInfo(const QString &preset)
{
    QString outStr;
    QDomNodeList valueList = presetMap.value(preset).namedItem("preset").childNodes();
    for (int i = 0; i < valueList.count(); ++i) {
        outStr += presetMap.value("None")
                  .elementsByTagName(valueList.at(i).toElement().tagName()).at(0)
                  .toElement().attribute("name")+"\n";
    }
    return outStr;
}

void GuiInfo::start()
{
    QDomNodeList pageList = presetMap.value("None").namedItem("arguments").childNodes();
    for (int page = 0; page < pageList.count(); ++page) {
        QDomElement pageElement = pageList.at(page).toElement();

        QScrollArea *area = new QScrollArea();
        area->setFrameStyle(QFrame::NoFrame);
        area->setWidgetResizable(true);

        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0,0,0,0);
        widget->setLayout(layout);
        area->setWidget(widget);

        QDomNodeList itemList = pageList.at(page).childNodes();
        for (int item = 0; item < itemList.count(); ++item) {
            QString type = itemList.at(item).toElement().attribute("type");
            if      (type == "chBox")
                layout->addWidget(createCheckBox(itemList.at(item)));
            else if (type == "cmbBox")
                layout->addWidget(createComboBox(itemList.at(item)));
            else if (type == "spinBox")
                layout->addWidget(createSpinBox(itemList.at(item)));
            else if (type == "chBox_sub")
                layout->addWidget(createSub(itemList.at(item)));
        }
        layout->addStretch(100);
        emit newPage(pageElement.attribute("name"),pageElement.attribute("icon"),area);
    }
}

int GuiInfo::interDefValue(const QDomNode &node)
{
    QDomNodeList valueList = node.childNodes();
    int value = 0;
    for (int i = 0; i < valueList.count(); ++i) {
        if (valueList.at(i).toElement().text() == node.toElement().attribute("default"))
            value = i;
    }

    return value;
}

int GuiInfo::presetDefValue(const QString &name, const QString &string)
{
    if (string.isEmpty())
        return 0;

    QDomNodeList valueList = presetMap.value("None").elementsByTagName(name).at(0).childNodes();
    int value = 0;
    for (int i = 0; i < valueList.count(); ++i) {
        if (valueList.at(i).toElement().text() == string)
            value = i;
    }
    return value;
}

int GuiInfo::defaultValue(const QString &preset, const QString &name)
{
    if (preset == "None")
        return interDefValue(presetMap.value(preset).elementsByTagName(name).at(0));
    else
        return presetDefValue(name,presetMap.value(preset).elementsByTagName(name).at(0)
                                            .toElement().attribute("default"));
}

float GuiInfo::defaultSpintValue(const QString &preset, const QString &name)
{
    QDomNode node = presetMap.value(preset).elementsByTagName(name).at(0);
    return node.toElement().attribute("default").toFloat();
}

QString GuiInfo::valueAt(const QString &name, const int &pos)
{
    QDomNode node = presetMap.value("None").elementsByTagName(name).at(0).childNodes().at(pos);
    return node.toElement().text();
}

QCheckBox* GuiInfo::createCheckBox(const QDomNode &node)
{
    QCheckBox *chBox = new QCheckBox(node.toElement().attribute("name"));
    chBox->setChecked(interDefValue(node));
    chBox->setAccessibleName(node.toElement().tagName());

    return chBox;
}

QWidget* GuiInfo::createComboBox(const QDomNode &node)
{
    QLabel *lblTitle = new QLabel(node.toElement().attribute("name"));

    QComboBox *cmbBox = new QComboBox();
    cmbBox->setAccessibleName(node.toElement().tagName());
    cmbBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QDomNodeList valueList = node.childNodes();
    for (int i = 0; i < valueList.count(); ++i)
        cmbBox->addItem(valueList.at(i).toElement().text());
    cmbBox->setCurrentIndex(interDefValue(node));

    QWidget *widget = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(lblTitle);
    lay->addWidget(cmbBox);
    widget->setLayout(lay);

    return widget;
}

QWidget* GuiInfo::createSpinBox(const QDomNode &node)
{
    QLabel *lblTitle = new QLabel(node.toElement().attribute("name"));

    QWidget *widget = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(lblTitle);

    float defaultValue = node.toElement().attribute("default").toFloat();
    float min = node.toElement().attribute("min").toFloat();
    float max = node.toElement().attribute("max").toFloat();
    float step = node.toElement().attribute("step").toFloat();
    QString tagName = node.toElement().tagName();

    if (step >= 1)
        lay->addWidget(F1<QSpinBox>(defaultValue,min,max,step,tagName));
    else
        lay->addWidget(F1<QDoubleSpinBox>(defaultValue,min,max,step,tagName));
    widget->setLayout(lay);

    return widget;
}

QWidget* GuiInfo::createSub(const QDomNode &node)
{
    QCheckBox *chBox = new QCheckBox(node.toElement().attribute("name"));
    chBox->setChecked(interDefValue(node));
    chBox->setAccessibleName(node.toElement().tagName());

    QWidget *widget = new QWidget();
    QWidget *widgetSub = new QWidget();

    QVBoxLayout *lay = new QVBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(chBox);
    lay->addWidget(widgetSub);

    QVBoxLayout *laySub = new QVBoxLayout();
    laySub->setContentsMargins(30,0,0,0);

    QDomNodeList itemList = node.namedItem("sub").childNodes();
    for (int i = 0; i < itemList.count(); ++i) {
        QString type = itemList.at(i).toElement().attribute("type");
        if      (type == "chBox")
            laySub->addWidget(createCheckBox(itemList.at(i)));
        else if (type == "cmbBox")
            laySub->addWidget(createComboBox(itemList.at(i)));
        else if (type == "spinBox")
            laySub->addWidget(createSpinBox(itemList.at(i)));
        else if (type == "chBox_sub")
            laySub->addWidget(createSub(itemList.at(i)));

        connect(chBox,SIGNAL(toggled(bool)),
                widgetSub,SLOT(setVisible(bool)));
        connect(chBox,SIGNAL(toggled(bool)),
                laySub->itemAt(laySub->count()-1)->widget(),SLOT(setEnabled(bool)));
    }
    widgetSub->setLayout(laySub);
    widget->setLayout(lay);

    return widget;
}
