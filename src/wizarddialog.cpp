#include <QMessageBox>
#include <QFileDialog>
#include <QWheelEvent>
#include <QDirIterator>
#include <QProcess>
#include <QtDebug>

#include "itemwidget.h"
#include "someutils.h"
#include "wizarddialog.h"

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    loadSettings();
    setupGUI();

    // create properties tabs from interface.xml
    gInfo = new GuiInfo;
    connect(gInfo,SIGNAL(newPage(QString,QString,QWidget*)),
            this,SLOT(createPage(QString,QString,QWidget*)));
    cmbBoxPreset->addItems(gInfo->presets());
    gInfo->start();
    cmbBoxPreset->setCurrentIndex(settings->value("Wizard/preset",0).toInt());
    resetItems(cmbBoxPreset->currentText());

    adjustSize();
}

WizardDialog::~WizardDialog()
{
    delete gInfo;
    delete settings;
}

void WizardDialog::loadSettings()
{
    settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                             "svgcleaner", "config");

    chBoxRecursive->setChecked(settings->value("Wizard/recursive").toBool());
    lineEditInDir->setText(settings->value("Wizard/lastInDir").toString());
    lineEditOutDir->setText(settings->value("Wizard/lastOutDir",QDir::homePath()).toString());
    lineEditPrefix->setText(settings->value("Wizard/prefix").toString());
    lineEditSuffix->setText(settings->value("Wizard/suffix",tr("_cleaned")).toString());

    gBoxCompress->setChecked(settings->value("Wizard/compress",true).toBool());
    spinBoxCompress->setValue(settings->value("Wizard/compressLevel",9).toInt());
    cmbBoxCompressor->setCurrentIndex(settings->value("Wizard/compressor",0).toInt());
    rBtnSaveSuffix->setChecked(settings->value("Wizard/compressType",false).toBool());
}

void WizardDialog::setupGUI()
{
    // setup type radioButtons
    connect(radioBtn1,SIGNAL(clicked()),this,SLOT(radioSelected()));
    connect(radioBtn2,SIGNAL(clicked()),this,SLOT(radioSelected()));
    connect(radioBtn3,SIGNAL(clicked()),this,SLOT(radioSelected()));
    QString str("radioBtn");
    str.append(settings->value("Wizard/type","1").toString());
    QRadioButton *btn = findChild<QRadioButton *>(str);
    btn->click();

    connect(lineEditInDir,SIGNAL(textChanged(QString)),this,SLOT(loadFiles()));
    connect(chBoxRecursive,SIGNAL(clicked()),this,SLOT(loadFiles()));

    // Placeholder text work only on qt>4.7
#if QT_VERSION >= 0x040700
    lineEditPrefix->setPlaceholderText(tr("prefix"));
    lineEditSuffix->setPlaceholderText(tr("suffix"));
    linePresetName->setPlaceholderText(tr("name"));
#endif
    connect(lineEditPrefix,SIGNAL(textChanged(QString)),this,SLOT(createExample()));
    connect(lineEditSuffix,SIGNAL(textChanged(QString)),this,SLOT(createExample()));
    createExample();

    QListWidgetItem *itemFiles = new QListWidgetItem(listWidget);
    ItemWidget *widgetFiles = new ItemWidget("files");
    itemFiles->setToolTip(tr("Files"));
    listWidget->setItemWidget(itemFiles,widgetFiles);

    QListWidgetItem *itemPresets = new QListWidgetItem(listWidget);
    ItemWidget *widgetPresets = new ItemWidget("presets");
    itemPresets->setToolTip(tr("Presets"));
    listWidget->setItemWidget(itemPresets,widgetPresets);
    connect(listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                            this,SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
    listWidget->setCurrentRow(0);
    listWidget->installEventFilter(this);

    if (!checkFor7z()) {
        cmbBoxCompressor->removeItem(1);
        cmbBoxCompressor->setToolTip(tr("Install <b>p7zip</b> to use it as compressor "
                                        "for svgz files."));
    }

    loadFiles();

    // setup icons
    btnOpenInDir->setIcon(QIcon(":/open.svgz"));
    btnOpenOutDir->setIcon(QIcon(":/open.svgz"));
    setWindowIcon(QIcon(":/svgcleaner.svgz"));
}

bool WizardDialog::checkFor7z()
{
    QProcess zipproc;
    zipproc.start("7z");
    zipproc.waitForFinished();
    return !QString(zipproc.readAll()).isEmpty();
}

void WizardDialog::radioSelected()
{
    frameOutDir->setVisible(radioBtn1->isChecked());
    frameRename->setVisible(radioBtn2->isChecked());
    QRadioButton *rBtn = static_cast<QRadioButton *>(sender());
    settings->setValue("Wizard/type",rBtn->accessibleName());
}

void WizardDialog::createPage(const QString &name, const QString &iconName, QWidget *widget)
{
    stackedWidget->addWidget(widget);

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    ItemWidget *w = new ItemWidget(iconName);
    item->setSizeHint(QSize(64,64));
    item->setToolTip(name);
    listWidget->setItemWidget(item,w);
    listWidget->setFixedWidth(64*listWidget->count()+5);
}

void WizardDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
         current = previous;
    stackedWidget->setCurrentIndex(listWidget->row(current));
    groupBoxMain->setTitle(current->toolTip());
}

void WizardDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if        (buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        if (checkForWarnings()) {
            saveSettings();
            done(QDialog::Accepted);
        }
    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Cancel) {
        close();
    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
        resetFields();
        resetItems("None");
    }
}

ToThread WizardDialog::threadArguments()
{
    ToThread threadArgs;
    threadArgs.args = argLine();
    threadArgs.inputFiles = getInFiles();
    threadArgs.outputFiles = getOutFiles();
    threadArgs.level = QString::number(spinBoxCompress->value());
    threadArgs.compressWith = cmbBoxCompressor->currentText();
    return threadArgs;
}

QString WizardDialog::argLine()
{
    QString tmpStr;
    foreach(QWidget *w, this->findChildren<QWidget *>()) {
        QString name = w->accessibleName();
        if (!name.isEmpty() && w->isEnabled()) {
            if      (w->inherits("QComboBox"))
                tmpStr.append(name+"="
                              +gInfo->valueAt(name,qobject_cast<QComboBox *>(w)->currentIndex()));
            else if (w->inherits("QCheckBox"))
                tmpStr.append(name+"="
                              +gInfo->valueAt(name,qobject_cast<QCheckBox *>(w)->isChecked()));
            else if (w->inherits("QSpinBox"))
                tmpStr.append(name+"="
                              +QString::number(qobject_cast<QSpinBox *>(w)->value()));
            else if (w->inherits("QDoubleSpinBox"))
                tmpStr.append(name+"="
                              +QString::number(qobject_cast<QDoubleSpinBox *>(w)->value()));
            tmpStr.append(":");
        }
    }
    tmpStr.remove(QRegExp("::"));
    return tmpStr;
}

void WizardDialog::loadFiles()
{
    if (!QFile(lineEditInDir->text()).exists()) {
        lineEditInDir->setValue(0);
        return;
    }
    fileList.clear();
    QDirIterator dirIt(lineEditInDir->text(),
                       chBoxRecursive->isChecked()?QDirIterator::Subdirectories
                                                  :QDirIterator::NoIteratorFlags);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (QFileInfo(dirIt.filePath()).suffix().contains(QRegExp("svg$|svgz$")))
                fileList.append(dirIt.filePath());
    }
    lineEditInDir->setValue(fileList.count());
}

QStringList WizardDialog::getInFiles()
{
    QStringList list;
    foreach (QFileInfo file, fileList)
        list.append(file.absoluteFilePath());
    list.sort();
    return list;
}

QStringList WizardDialog::getOutFiles()
{
    QStringList list;
    if        (radioBtn1->isChecked()) {
        foreach (QFileInfo file, fileList)
            list.append(lineEditOutDir->text()
                        +QString(file.absoluteFilePath()).remove(lineEditInDir->text()));
    } else if (radioBtn2->isChecked()) {
        foreach (QFileInfo file, fileList)
            list.append(file.absolutePath()+QDir::separator()+lineEditPrefix->text()
                       +file.baseName()+lineEditSuffix->text()+"."+file.suffix());
    } else if (radioBtn3->isChecked()) {
        list = getInFiles();
    }

    if (!gBoxCompress->isChecked())
        list.replaceInStrings(QRegExp("svgz$"),"svg");
    else if (rBtnCompressAll->isChecked())
        list.replaceInStrings(QRegExp("svg$"),"svgz");
    list.sort();

    return list;
}

// set default values to all settings from interface.xml
void WizardDialog::resetItems(const QString &preset)
{
    foreach(QWidget *w, this->findChildren<QWidget *>()) {
    QString name = w->accessibleName();
    if (!name.isEmpty()) {
        if      (w->inherits("QComboBox"))
            qobject_cast<QComboBox *>(w)->setCurrentIndex((int)gInfo->defaultValue(preset,name));
        else if (w->inherits("QCheckBox"))
            qobject_cast<QCheckBox *>(w)->setChecked((int)gInfo->defaultValue(preset,name));
        else if (w->inherits("QSpinBox"))
            qobject_cast<QSpinBox *>(w)->setValue((int)gInfo->defaultSpintValue(preset,name));
        else if (w->inherits("QDoubleSpinBox"))
            qobject_cast<QDoubleSpinBox *>(w)->setValue(gInfo->defaultSpintValue(preset,name));
        }
    }
}

void WizardDialog::resetFields()
{
    cmbBoxPreset->setCurrentIndex(0);
    lineEditInDir->clear();
    lineEditOutDir->setText(QDir::homePath());
    lineEditPrefix->clear();
    lineEditSuffix->setText(tr("_cleaned"));
    radioBtn1->setChecked(true);
    chBoxRecursive->setChecked(false);
    gBoxCompress->setChecked(true);
    rBtnSaveSuffix->setChecked(true);
    cmbBoxCompressor->setCurrentIndex(0);
    spinBoxCompress->setValue(9);
}

void WizardDialog::on_btnOpenInDir_clicked()
{
    QString currPath = lineEditInDir->text();
    if (currPath.isEmpty())
        currPath = QDir::homePath();

    QString path = QFileDialog::getExistingDirectory(this,tr("Select input folder"),
                                                     currPath,
                                                     QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        lineEditInDir->setText(path);
}

void WizardDialog::on_btnOpenOutDir_clicked()
{
    QString currPath = lineEditOutDir->text();
    if (currPath.isEmpty())
        currPath = QDir::homePath();

    QString path = QFileDialog::getExistingDirectory(this,tr("Select output folder"),
                                                     currPath,
                                                     QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        lineEditOutDir->setText(path);
}

bool WizardDialog::checkForWarnings()
{
    bool check = true;
    if (lineEditInDir->text().isEmpty()) {
        createWarning(tr("Input folder is not selected."));
        check = false;
    } else if (lineEditOutDir->text().isEmpty()) {
        createWarning(tr("Output folder is not selected."));
        check = false;
    } else if (lineEditPrefix->text().isEmpty() && lineEditSuffix->text().isEmpty()) {
        createWarning(tr("You must set prefix and suffix for this save method."));
        check = false;
    } else if (!QDir(lineEditInDir->text()).exists()) {
        createWarning(tr("Input folder is not exist."));
        check = false;
    } else if (radioBtn3->isChecked() && cmbBoxCompressor->currentText().contains("7z")
               && gBoxCompress->isChecked()) {
        createWarning(tr("Program can't work in this method.\n7z can't overwrite original file."));
        check = false;
    } else if (fileList.isEmpty()) {
        createWarning(tr("Input folder didn't contain any svg, svgz files."));
        check = false;
    }
    return check;
}

void WizardDialog::createWarning(const QString &text)
{
    QMessageBox::warning(this,tr("Warning"),text,QMessageBox::Ok);
}

void WizardDialog::on_cmbBoxPreset_currentIndexChanged(const QString &name)
{
    resetItems(name);
    textPresetInfo->setText(gInfo->presetInfo(name));
}

void WizardDialog::on_btnSavePreset_clicked()
{
    if (linePresetName->text().isEmpty()) {
        createWarning(tr("You must set preset name."));
        return;
    }

    // generate path
    QString path = QFileInfo(settings->fileName()).absolutePath()+"/preset/";
    // overwrite old preset?
    if (QFile(path+linePresetName->text()+".preset").exists()) {
        int ansver = QMessageBox::warning(this,tr("Warning"),
                                          tr("Preset already exist.\nOverwrite?"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (ansver == QMessageBox::No)
            return;
    }
    QDir().mkpath(path);
    path += linePresetName->text()+".preset";

    // create preset xml
    QDomDocument *domDoc = new QDomDocument;
    QDomElement mainElement;
    mainElement = domDoc->createElement("preset");
    mainElement.setAttribute("name",linePresetName->text());

    foreach(QWidget *w, this->findChildren<QWidget *>()) {
        QString name = w->accessibleName();
        if (!name.isEmpty() && w->isEnabled()) {
            QDomElement tagElem = domDoc->createElement(name);
            int defValue = gInfo->defaultValue("None",name);
            int defValueS = gInfo->defaultSpintValue("None",name);
            if        (w->inherits("QComboBox")) {
                if (qobject_cast<QComboBox *>(w)->currentIndex() == defValue)
                tagElem.setAttribute("default",
                    gInfo->valueAt(name,qobject_cast<QComboBox *>(w)->currentIndex()));
            } else if (w->inherits("QCheckBox")) {
                if (qobject_cast<QCheckBox *>(w)->isChecked() == defValue)
                tagElem.setAttribute("default",
                    gInfo->valueAt(name,qobject_cast<QCheckBox *>(w)->isChecked()));
            } else if (w->inherits("QSpinBox")) {
                if (qobject_cast<QSpinBox *>(w)->value() == defValueS)
                tagElem.setAttribute("default",
                    QString::number(qobject_cast<QSpinBox *>(w)->value()));
            } else if (w->inherits("QDoubleSpinBox")) {
                if (qobject_cast<QDoubleSpinBox *>(w)->value() == defValueS)
                tagElem.setAttribute("default",
                    QString::number(qobject_cast<QDoubleSpinBox *>(w)->value()));
            }
            if (!tagElem.attribute("default").isEmpty())
                mainElement.appendChild(tagElem);
        }
    }
    domDoc->appendChild(mainElement);

    // write to file
    QFile inputFile(path);
    inputFile.open(QFile::WriteOnly);
    QTextStream textStream(&inputFile);
    textStream << domDoc->toString();
    inputFile.close();
}

void WizardDialog::on_btnRmPreset_clicked()
{
    QString path = QFileInfo(settings->fileName()).absolutePath()+"/preset/";
    QFile file(path+cmbBoxPreset->currentText()+".preset");
    if (file.exists()) {
        file.remove();
        cmbBoxPreset->removeItem(cmbBoxPreset->currentIndex());
    } else {
        createWarning(tr("You can't remove default preset."));
    }
}

void WizardDialog::createExample()
{
    lblExample->setText(tr("Example: ")+lineEditPrefix->text()
                       +tr("filename" )+lineEditSuffix->text()+".svg");
}

void WizardDialog::saveSettings()
{
    settings->setValue("Wizard/recursive",chBoxRecursive->isChecked());
    settings->setValue("Wizard/lastInDir",lineEditInDir->text());
    settings->setValue("Wizard/lastOutDir",lineEditOutDir->text());
    settings->setValue("Wizard/prefix",lineEditPrefix->text());
    settings->setValue("Wizard/suffix",lineEditSuffix->text());
    settings->setValue("Wizard/compress",gBoxCompress->isChecked());
    settings->setValue("Wizard/compressLevel",spinBoxCompress->value());
    settings->setValue("Wizard/compressor",cmbBoxCompressor->currentIndex());
    settings->setValue("Wizard/compressType",rBtnSaveSuffix->isChecked());
    settings->setValue("Wizard/preset",cmbBoxPreset->currentIndex());
}

bool WizardDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == listWidget && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->delta() > 0) {
            if (listWidget->currentRow() > 0)
                listWidget->setCurrentRow(listWidget->currentRow()-1);
            return true;
        } else {
            if (listWidget->currentRow() < listWidget->count()-1)
                listWidget->setCurrentRow(listWidget->currentRow()+1);
            return true;
        }
    }
    return false;
}
