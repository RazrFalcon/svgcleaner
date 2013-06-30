#include <QtCore/QProcess>
#include <QtCore/QTextStream>

#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QScrollBar>
#include <QtGui/QWheelEvent>

#include "itemwidget.h"
#include "someutils.h"
#include "wizarddialog.h"

// TODO: rewrite presets
// TODO: save last options automaticly
// TODO: hide multithreading option if detected only one core

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    loadSettings();
    setupGUI();
    setupToolTips();
    adjustSize();
}

WizardDialog::~WizardDialog()
{
    delete settings;
    deleteThreads();
    delete fileSearch;
}

void WizardDialog::loadSettings()
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             "svgcleaner", "config");

    chBoxRecursive->setChecked(settings->value("Wizard/Recursive").toBool());
    lineEditInDir->setText(settings->value("Wizard/LastInDir").toString());
    lineEditOutDir->setText(settings->value("Wizard/LastOutDir", QDir::homePath()).toString());
    lineEditPrefix->setText(settings->value("Wizard/Prefix").toString());
    lineEditSuffix->setText(settings->value("Wizard/Suffix", "_cleaned").toString());

    gBoxCompress->setChecked(settings->value("Wizard/Compress", true).toBool());
    cmbBoxCompress->setCurrentIndex(settings->value("Wizard/CompressLevel",4).toInt());
    if (settings->value("Wizard/CompressType",true).toBool())
        rBtnSaveSuffix->setChecked(true);
    else
        rBtnCompressAll->setChecked(true);

    loadPresets();
    int presetNum = settings->value("Wizard/Preset", 0).toInt();
    if (presetNum > cmbBoxPreset->count() || presetNum < 0)
        presetNum = 0;
    cmbBoxPreset->setCurrentIndex(presetNum);

    // threading
    spinBoxThreads->setValue(settings->value("Wizard/ThreadCount",
                                             QThread::idealThreadCount()).toInt());
    spinBoxThreads->setMaximum(QThread::idealThreadCount());
    gBoxThreads->setChecked(settings->value("Wizard/ThreadingEnabled", true).toBool());
}

void WizardDialog::setupGUI()
{
    // NOTE: not implemented
    chBoxRmNonSvgAttr->hide();
    chBoxRmClipAttr->hide();
    chBoxConvCToL->hide();
    chBoxConvCToS->hide();
    chBoxConvQToT->hide();
    chBoxRecalcCoord->hide();

    // setup type radioButtons
    connect(radioBtn1, SIGNAL(clicked()), this, SLOT(radioSelected()));
    connect(radioBtn2, SIGNAL(clicked()), this, SLOT(radioSelected()));
    connect(radioBtn3, SIGNAL(clicked()), this, SLOT(radioSelected()));
    QString str("radioBtn");
    str.append(settings->value("Wizard/Type", "1").toString()); // FIXME: change name
    QRadioButton *rbtn = findChild<QRadioButton *>(str);
    rbtn->click();

    connect(lineEditInDir,  SIGNAL(textChanged(QString)), this, SLOT(loadFiles()));
    connect(chBoxRecursive, SIGNAL(clicked()),            this, SLOT(loadFiles()));

    // alignment fixes
    gridLayoutOutput->setAlignment(spinBoxIndent, Qt::AlignRight);

    // Placeholder text work only on qt>4.7
#if QT_VERSION >= 0x040700
    lineEditPrefix->setPlaceholderText(tr("prefix"));
    lineEditSuffix->setPlaceholderText(tr("suffix"));
#endif
    connect(lineEditPrefix, SIGNAL(textChanged(QString)), this, SLOT(createExample()));
    connect(lineEditSuffix, SIGNAL(textChanged(QString)), this, SLOT(createExample()));
    createExample();

    // list of all pages in wizard
    QStringList pageList;
    pageList << tr("Main")
             << tr("Presets")
             << tr("Elements")
             << tr("Attributes")
             << tr("Paths")
             << tr("Optimization")
             << tr("Output");
    QStringList pageListNotTr;
    pageListNotTr << "main" << "presets" << "elements" << "attributes" << "paths"
                  << "optimization" << "output";
    // create icons for pages in "tabbar", which is QListWidget
    int baseIconSize = 64;
#ifdef Q_OS_MAC
    baseIconSize = 70;
#endif
    for (int i = 0; i < pageList.count(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(listWidget);
        ItemWidget *w = new ItemWidget(pageListNotTr.at(i));
        item->setToolTip(pageList.at(i));
        item->setSizeHint(QSize(baseIconSize, baseIconSize));
        listWidget->setItemWidget(item, w);
    }
    listWidget->setFixedWidth(baseIconSize*listWidget->count()+5);
    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                            this, SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
    listWidget->setCurrentRow(0);
    listWidget->installEventFilter(this);

    lblOverwrite->setText("<span style=\"color:#ff0000;\">"+
                          tr("Warning! The original files will be destroyed!")+"</span>");

    // setup icons
    btnOpenInDir->setIcon( QIcon(":/open.svgz"));
    btnOpenOutDir->setIcon(QIcon(":/open.svgz"));
    setWindowIcon(QIcon(":/svgcleaner.svgz"));
    listWidget->setFocus();

    searchThread = new QThread(this);
    fileSearch = new FileFinder();
    qRegisterMetaType<QFileInfoList>("QFileInfoList");
    connect(fileSearch, SIGNAL(finished(QFileInfoList)), this, SLOT(loadFinished(QFileInfoList)));
    connect(this, SIGNAL(start(QString, bool)), fileSearch, SLOT(startSearch(QString, bool)));
    fileSearch->moveToThread(searchThread);
    searchThread->start();
    loadFiles();
}

void WizardDialog::setupToolTips()
{
    chBoxGroupByStyle->setToolTip(tr("For example") + ":\n\n(" + tr("before") + ")\n"
        "<path style=\"fill:#fff\" d=\"...\"/>\n<path style=\"fill:#fff\" d=\"...\"/>\n"
        "<path style=\"fill:#fff\" d=\"...\"/>\n\n(" + tr("after") + ")\n"
        "<g style=\"fill:#fff\">\n  <path d=\"...\"/>\n  <path d=\"...\"/>"
        "\n  <path d=\"...\"/>\n</g>");
}

void WizardDialog::radioSelected()
{
    frameOutDir->setVisible(radioBtn1->isChecked());
    frameRename->setVisible(radioBtn2->isChecked());
    lblOverwrite->setVisible(radioBtn3->isChecked());
    QRadioButton *rBtn = static_cast<QRadioButton *>(sender());
    settings->setValue("Wizard/Type", rBtn->accessibleName());
}

void WizardDialog::createExample()
{
    lblExample->setText(tr("For example") + ": " + lineEditPrefix->text()
                        + tr("filename" ) + lineEditSuffix->text() + ".svg");
}

void WizardDialog::loadPresets()
{
    cmbBoxPreset->clear();
    cmbBoxPreset->addItem("Complete");
    presets.clear();
    presets += QDir("presets").entryInfoList(QStringList("*.preset"));
    presets += QDir("/usr/share/svgcleaner/presets/")
               .entryInfoList(QStringList("*.preset")); // Linux only. On Windows return nothing.
    presets += QDir(settingPath() + "/presets/")
               .entryInfoList(QStringList("*.preset"));
    presets += QDir("../SVGCleaner/presets/")
               .entryInfoList(QStringList("*.preset")); // Qt Creator shadow build
    for (int i = 0; i < presets.count(); ++i)
        cmbBoxPreset->addItem(presets.at(i).baseName());
}

void WizardDialog::loadFiles()
{
    if (!QFile(lineEditInDir->text()).exists()) {
        lineEditInDir->setValue(0);
        return;
    }
    lineEditInDir->showLoading(true);
    fileSearch->stopSearch();
    emit start(lineEditInDir->text(), chBoxRecursive->isChecked());
}

void WizardDialog::loadFinished(QFileInfoList list)
{
    fileList = list;
    lineEditInDir->setValue(fileList.count());
    lineEditInDir->showLoading(false);
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
    if (buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        if (checkForWarnings()) {
            saveSettings();
            done(QDialog::Accepted);
        }
    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Cancel) {
        close();
    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
        resetFields();
    }
}

ToThread WizardDialog::threadArguments()
{
    ToThread threadArgs;
    threadArgs.args          = argsLine();
    threadArgs.inputFiles    = getInFiles();
    threadArgs.outputFiles   = genOutFiles();
    threadArgs.compressLevel = compressValue();
    threadArgs.cliPath       = SomeUtils::findBinFile("svgcleaner-cli");
    threadArgs.zipPath       = SomeUtils::findBinFile("7za");
    return threadArgs;
}

QStringList WizardDialog::argsLine()
{
    QStringList tmpList;
    // 2 - page number
    for (int i = 2; i < stackedWidget->count(); ++i) {
        foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
            QString name = w->accessibleName();
            if (!name.isEmpty()) {
                // TODO: maybe add '--' to definition in gui
                if (w->inherits("QCheckBox") && !qobject_cast<QCheckBox *>(w)->isChecked())
                    tmpList.append("--" + name);
                else if (w->inherits("QRadioButton") && !isDefault(w))
                    tmpList.append("--" + name);
                else if (w->inherits("QComboBox") && !isDefault(w))
                    tmpList.append("--" + name + "="+qobject_cast<QComboBox *>(w)->currentText());
                else if (w->inherits("QSpinBox") && !isDefault(w))
                    tmpList.append("--" + name + "="
                                   + QString::number(qobject_cast<QSpinBox *>(w)->value()));
                else if (w->inherits("QDoubleSpinBox") && !isDefault(w))
                    tmpList.append("--" + name + "="
                                   + QString::number(qobject_cast<QDoubleSpinBox *>(w)->value()));
            }
        }
    }
    return tmpList;
}

bool WizardDialog::isDefault(QWidget *w)
{
    QString description = w->accessibleDescription();
    QString className = QString(w->metaObject()->className());
    bool flag = true;

    if      ("QComboBox" == className
             && description.toInt() != qobject_cast<QComboBox *>(w)->currentIndex())
        flag = false;
    else if ("QRadioButton" == className
             && description != qobject_cast<QRadioButton *>(w)->accessibleName()) {
        if (qobject_cast<QRadioButton *>(w)->isChecked())
            flag = false;
    } else if ("QSpinBox" == className
             && description.toInt() != qobject_cast<QSpinBox *>(w)->value())
        flag = false;
    else if ("QDoubleSpinBox" == className
             && description.toDouble() != qobject_cast<QDoubleSpinBox *>(w)->value())
        flag = false;

    return flag;
}

QStringList WizardDialog::getInFiles()
{
    QStringList list;
    foreach (QFileInfo file, fileList)
        list.append(file.absoluteFilePath());
    list.sort();
    return list;
}

QStringList WizardDialog::genOutFiles()
{
    QStringList list;
    if (radioBtn1->isChecked()) {
        foreach (QFileInfo file, fileList) {
            list.append(QDir(lineEditOutDir->text()).absolutePath()
                        +QString(file.absoluteFilePath())
                        .remove(QDir(lineEditInDir->text()).absolutePath()));
        }
    } else if (radioBtn2->isChecked()) {
        foreach (QFileInfo file, fileList)
            list.append(file.absolutePath()+QDir::separator()+lineEditPrefix->text()
                       +file.baseName()+lineEditSuffix->text()+"."+file.suffix());
    } else if (radioBtn3->isChecked()) {
        list = getInFiles();
    }

    if (!gBoxCompress->isChecked())
        list.replaceInStrings(QRegExp("svgz$"), "svg");
    else if (rBtnCompressAll->isChecked())
        list.replaceInStrings(QRegExp("svg$"), "svgz");
    list.sort();

    return list;
}

QString WizardDialog::compressValue()
{
    int currPos = cmbBoxCompress->currentIndex();
    switch (currPos) {
        case 0: return "1";
        case 1: return "3";
        case 2: return "5";
        case 3: return "7";
        case 4: return "9";
    }
    return "9";
}

void WizardDialog::resetFields()
{
    chBoxRecursive->setChecked(false);
    cmbBoxCompress->setCurrentIndex(4);
    cmbBoxPreset->setCurrentIndex(0);
    gBoxCompress->setChecked(true);
    lineEditInDir->clear();
    lineEditOutDir->setText(QDir::homePath());
    lineEditPrefix->clear();
    lineEditSuffix->setText("_cleaned");
    radioBtn1->click();
    rBtnSaveSuffix->setChecked(true);
}

void WizardDialog::on_btnOpenInDir_clicked()
{
    QString currPath = lineEditInDir->text();
    if (currPath.isEmpty())
        currPath = QDir::homePath();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select an input folder"),
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

    QString path = QFileDialog::getExistingDirectory(this, tr("Select an output folder"),
                                                     currPath,
                                                     QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        lineEditOutDir->setText(path);
}

bool WizardDialog::checkForWarnings()
{
    bool check = true;
    if (lineEditInDir->text().isEmpty()) {
        createWarning(tr("An input folder is not selected."));
        check = false;
    } else if (lineEditOutDir->text().isEmpty()) {
        createWarning(tr("An output folder is not selected."));
        check = false;
    } else if (lineEditPrefix->text().isEmpty() && lineEditSuffix->text().isEmpty()) {
        createWarning(tr("You have to set a prefix or a suffix for this save method."));
        check = false;
    } else if (!QDir(lineEditInDir->text()).exists()) {
        createWarning(tr("An input folder is not exist."));
        check = false;
    } else if (fileList.isEmpty()) {
        createWarning(tr("An input folder did not contain any svg, svgz files."));
        check = false;
    } else if (SomeUtils::findBinFile("svgcleaner-cli").isEmpty()) {
        QMessageBox::critical(this, tr("Error"),
                        tr("The 'svgcleaner-cli' executable is not found in these folders:\n")
                        + SomeUtils::genSearchFolderList());
        check = false;
    } else if (SomeUtils::findBinFile("7za").isEmpty()) {
        QMessageBox::warning(this, tr("Warning"),
                        tr("The '7za' executable is not found in these folders:\n")
                        + SomeUtils::genSearchFolderList() + "\n\n"
                        + tr("You can not handle the SVGZ files."));
    } else if (QFileInfo(lineEditOutDir->text()).isDir()
               && !QFileInfo(lineEditOutDir->text()).isWritable()) {
        createWarning(tr("Selected output folder is not writable."));
        check = false;
    }
    return check;
}

void WizardDialog::createWarning(const QString &text)
{
    QMessageBox::warning(this, tr("Warning"), text, QMessageBox::Ok);
}

void WizardDialog::on_linePresetName_textChanged(const QString &text)
{
    btnSavePreset->setEnabled(!text.isEmpty());
}

void WizardDialog::on_btnSavePreset_clicked()
{
    if (linePresetName->text().isEmpty()) {
        createWarning(tr("You have to set preset name."));
        return;
    }

    // generate path
    QString path = settingPath() + "/presets/";
    // overwrite old preset?
    if (QFile(path+linePresetName->text()+".preset").exists()) {
        int ansver = QMessageBox::warning(this, tr("Warning"),
                                          tr("This preset already exists.\nOverwrite?"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (ansver == QMessageBox::No)
            return;
    }
    QDir().mkpath(path);
    path += linePresetName->text()+".preset";

    // write to file
    QFile inputFile(path);
    inputFile.open(QFile::WriteOnly);
    QTextStream textStream(&inputFile);

    for (int i = 2; i < stackedWidget->count(); ++i) {
    foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
        QString name = w->accessibleName();
        if (!name.isEmpty() && w->isEnabled()) {
            if (w->inherits("QCheckBox") && !qobject_cast<QCheckBox *>(w)->isChecked())
                textStream << name+"=no"+"\n";
            else if (w->inherits("QRadioButton") && !isDefault(w))
                textStream << name+"\n";
            else if (w->inherits("QComboBox") && !isDefault(w))
                textStream << name+"="
                              +QString::number(qobject_cast<QComboBox *>(w)->currentIndex())+"\n";
            else if (w->inherits("QSpinBox") && !isDefault(w))
                textStream << name+"="+QString::number(qobject_cast<QSpinBox *>(w)->value())+"\n";
            else if (w->inherits("QDoubleSpinBox") && !isDefault(w))
                textStream << name+"="
                              +QString::number(qobject_cast<QDoubleSpinBox *>(w)->value())+"\n";
        }
    }
    }
    inputFile.close();
    linePresetName->clear();
    loadPresets();
}

void WizardDialog::on_cmbBoxPreset_currentIndexChanged(const QString &text)
{
    setPreset(text);

    QString path = settingPath() + "/presets/";
    QFile file(path+cmbBoxPreset->currentText() + ".preset");
    btnRemovePreset->setVisible(file.exists());
}

void WizardDialog::setPreset(const QString &preset)
{
    QFile inputFile;
    for (int i = 0; i < presets.count(); ++i) {
        if (presets.at(i).baseName() == preset)
            inputFile.setFileName(presets.at(i).absoluteFilePath());
    }

    QMap<QString, QString> argMap;
    QString args;
    if (inputFile.exists() && inputFile.open(QFile::ReadOnly)) {
        QTextStream textStream(&inputFile);
        foreach (QString name, textStream.readAll().split("\n")) {
            args += QString(name).remove(QRegExp("=.*")) +"|";
            argMap.insert(QString(name).remove(QRegExp("=.*")),
                          QString(name).remove(QRegExp(".*=")));
        }
        inputFile.close();
    } else {
        args = "convert-abs-paths=yes";
        argMap.insert("convert-abs-paths", "yes");
    }
    QRegExp rx(args.remove(QRegExp("\\|*$")));

    if (args.contains("convert-abs-paths=yes"))
        radioBtnConvertPaths->toggle();

    textPresetInfo->clear();
    for (int i = 2; i < stackedWidget->count(); ++i) {
        foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
            QString name = w->accessibleName().remove(QRegExp("=.*"));
            QString line;
            if (w->parentWidget()->accessibleName() == "parent") // need for spacing
                line += "    ";
            if (w->inherits("QCheckBox") && w->isEnabled())
            {
                qobject_cast<QCheckBox *>(w)->setChecked(!name.contains(rx));
                if (!name.contains(rx))
                    line += qobject_cast<QCheckBox *>(w)->text();
            }
            else if (w->inherits("QRadioButton"))
            {
                if (w->accessibleName() == name+"="+argMap.value(name)) {
                    qobject_cast<QRadioButton *>(w)->toggle();
                    line += qobject_cast<QRadioButton *>(w)->text();
                }
            }
            else if (w->inherits("QComboBox"))
            {
                if (name.contains(rx))
                    qobject_cast<QComboBox *>(w)->setCurrentIndex(argMap.value(name).toInt());
                else
                {
                    qobject_cast<QComboBox *>(w)->setCurrentIndex(
                                              w->accessibleDescription().toInt());
                    line += findLabel(name)+" "+qobject_cast<QComboBox *>(w)->currentText();
                }
            }
            else if (w->inherits("QSpinBox"))
            {
                if (name.contains(rx))
                    qobject_cast<QSpinBox *>(w)->setValue(argMap.value(name).toInt());
                else
                {
                    qobject_cast<QSpinBox *>(w)->setValue(
                                             w->accessibleDescription().toInt());
                    line += findLabel(name)+" "
                            + QString::number(w->accessibleDescription().toInt());
                }
            }
            else if (w->inherits("QDoubleSpinBox"))
            {
                if (name.contains(rx))
                    qobject_cast<QDoubleSpinBox *>(w)->setValue(argMap.value(name).toDouble());
                else
                {
                    qobject_cast<QDoubleSpinBox *>(w)->setValue(
                                                   w->accessibleDescription().toDouble());
                    line += findLabel(name)+" "
                            + QString::number(w->accessibleDescription().toDouble());
                }
            }
            if (!QString(line).remove(" ").isEmpty() && w->isEnabled())
                textPresetInfo->append(line);
        }
    }
    textPresetInfo->verticalScrollBar()->setValue(0);
}

QString WizardDialog::findLabel(const QString &accessibleName)
{
    for (int i = 2; i < stackedWidget->count(); ++i) {
        foreach(QLabel *lbl, stackedWidget->widget(i)->findChildren<QLabel *>()) {
            if (lbl->accessibleName() == accessibleName)
                return lbl->text();
        }
    }
    return QString();
}

void WizardDialog::on_btnRemovePreset_clicked()
{
    QString path = settingPath() + "/presets/";
    QFile file(path + cmbBoxPreset->currentText() + ".preset");
    file.remove();
    cmbBoxPreset->removeItem(cmbBoxPreset->currentIndex());
}

void WizardDialog::saveSettings()
{
    settings->setValue("Wizard/LastInDir",        lineEditInDir->text());
    settings->setValue("Wizard/LastOutDir",       lineEditOutDir->text());
    settings->setValue("Wizard/Prefix",           lineEditPrefix->text());
    settings->setValue("Wizard/Suffix",           lineEditSuffix->text());
    settings->setValue("Wizard/Recursive",        chBoxRecursive->isChecked());
    settings->setValue("Wizard/Compress",         gBoxCompress->isChecked());
    settings->setValue("Wizard/ThreadingEnabled", gBoxThreads->isChecked());
    settings->setValue("Wizard/CompressType",     rBtnSaveSuffix->isChecked());
    settings->setValue("Wizard/Preset",           cmbBoxPreset->currentIndex());
    settings->setValue("Wizard/CompressLevel",    cmbBoxCompress->currentIndex());
    settings->setValue("Wizard/ThreadCount",      spinBoxThreads->value());
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

void WizardDialog::deleteThreads()
{
    // stops fileSearch loop
    fileSearch->stopSearch();
    searchThread->quit();
    // NOTE: program crash without it
    searchThread->wait();
    searchThread->deleteLater();
}

QString WizardDialog::settingPath()
{
    return QFileInfo(settings->fileName()).absolutePath();
}
