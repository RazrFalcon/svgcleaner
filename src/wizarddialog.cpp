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

    // list of all pages in wizard
    pageList<<tr("Files")
            <<tr("Presets")
            <<tr("Elements")
            <<tr("Attributes")
            <<tr("Paths")
            <<tr("Optimization")
            <<tr("Output");
    loadSettings();
    setupGUI();
    adjustSize();
}

WizardDialog::~WizardDialog()
{
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
    lineEditSuffix->setText(settings->value("Wizard/suffix","_cleaned").toString());

    chBoxFullLog->setChecked(settings->value("Wizard/fullLog",false).toBool());
    gBoxCompress->setChecked(settings->value("Wizard/compress",true).toBool());
    cmbBoxCompress->setCurrentIndex(settings->value("Wizard/compressLevel",4).toInt());
    if (settings->value("Wizard/compressType",true).toBool())
        rBtnSaveSuffix->setChecked(true);
    else
        rBtnCompressAll->setChecked(true);
}

void WizardDialog::setupGUI()
{
    // setup type radioButtons
    connect(radioBtn1,SIGNAL(clicked()),this,SLOT(radioSelected()));
    connect(radioBtn2,SIGNAL(clicked()),this,SLOT(radioSelected()));
    connect(radioBtn3,SIGNAL(clicked()),this,SLOT(radioSelected()));
    QString str("radioBtn");
    str.append(settings->value("Wizard/type","1").toString());
    QRadioButton *rbtn = findChild<QRadioButton *>(str);
    rbtn->click();

    connect(lineEditInDir,SIGNAL(textChanged(QString)),this,SLOT(loadFiles()));
    connect(chBoxRecursive,SIGNAL(clicked()),this,SLOT(loadFiles()));

    // alignment fixes
    gridLayoutOutput->setAlignment(spinBoxIndent,Qt::AlignRight);
    gridLayoutOutput->setAlignment(cmbBoxEmptyTags,Qt::AlignRight);
    gridLayoutOutput->setAlignment(cmbBoxQuote,Qt::AlignRight);

    // Placeholder text work only on qt>4.7
#if QT_VERSION >= 0x040700
    lineEditPrefix->setPlaceholderText(tr("prefix"));
    lineEditSuffix->setPlaceholderText(tr("suffix"));
#endif
    connect(lineEditPrefix,SIGNAL(textChanged(QString)),this,SLOT(createExample()));
    connect(lineEditSuffix,SIGNAL(textChanged(QString)),this,SLOT(createExample()));
    createExample();

    // create icons for pages in "tabbar", which is QListWidget
    for (int i = 0; i < pageList.count(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(listWidget);
        ItemWidget *widgetFiles = new ItemWidget(pageList.at(i));
        item->setToolTip(pageList.at(i));
        item->setSizeHint(QSize(64,64));
        listWidget->setItemWidget(item,widgetFiles);
    }
    listWidget->setFixedWidth(64*listWidget->count()+5);
    connect(listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                            this,SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
    listWidget->setCurrentRow(0);
    listWidget->installEventFilter(this);

    loadPresets();
    if (QFile(QDir::homePath()+"/.config/svgcleaner/presets/last.preset").exists())
        cmbBoxPreset->setCurrentIndex(settings->value("Wizard/preset",1).toInt());
    else
        cmbBoxPreset->setCurrentIndex(settings->value("Wizard/preset",0).toInt());
    setPreset(cmbBoxPreset->currentText());

    loadFiles();

    //! replace this icons with smaller
    // setup icons
    btnOpenInDir->setIcon(QIcon(":/open.svgz"));
    btnOpenOutDir->setIcon(QIcon(":/open.svgz"));
    setWindowIcon(QIcon(":/svgcleaner.svgz"));
}

void WizardDialog::radioSelected()
{
    frameOutDir->setVisible(radioBtn1->isChecked());
    frameRename->setVisible(radioBtn2->isChecked());
    QRadioButton *rBtn = static_cast<QRadioButton *>(sender());
    settings->setValue("Wizard/type",rBtn->accessibleName());
}

void WizardDialog::createExample()
{
    lblExample->setText(tr("Example: ")+lineEditPrefix->text()
                       +tr("filename" )+lineEditSuffix->text()+".svg");
}

void WizardDialog::loadPresets()
{
    cmbBoxPreset->clear();
    cmbBoxPreset->addItem("None");
    presets.clear();
    presets += QDir("presets").entryInfoList(QStringList("*.preset"));
    presets += QDir("/usr/share/svgcleaner/presets/")
            .entryInfoList(QStringList("*.preset"));
    presets += QDir(QDir::homePath()+"/.config/svgcleaner/presets/")
            .entryInfoList(QStringList("*.preset"));
    for (int i = 0; i < presets.count(); ++i)
        cmbBoxPreset->addItem(presets.at(i).baseName());
}

void WizardDialog::loadFiles()
{
    if (!QFile(lineEditInDir->text()).exists()) {
        lineEditInDir->setValue(0);
        return;
    }
    fileList.clear();
    QDirIterator dirIt(lineEditInDir->text(),
                       chBoxRecursive->isChecked()?(QDirIterator::Subdirectories
                                                   |QDirIterator::FollowSymlinks)
                                                  :QDirIterator::NoIteratorFlags);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile() && !QFileInfo(dirIt.filePath()).isSymLink())
            if (QFileInfo(dirIt.filePath()).suffix().contains(QRegExp("svg$|svgz$")))
                fileList.append(dirIt.filePath());
    }
    lineEditInDir->setValue(fileList.count());
}

void WizardDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
         current = previous;
    stackedWidget->setCurrentIndex(listWidget->row(current));
    groupBoxMain->setTitle(pageList.at(stackedWidget->currentIndex()));
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
        setPreset("None");
    }
}

ToThread WizardDialog::threadArguments()
{
    ToThread threadArgs;
    threadArgs.args = argsLine();
    threadArgs.inputFiles = getInFiles();
    threadArgs.outputFiles = getOutFiles();
    threadArgs.level = compressValue();
    threadArgs.cleanerPath = SomeUtils::findFile("svgcleaner.pl","/usr/bin/");

    // save current settings to temp preset
    linePresetName->setText("last");
    on_btnSavePreset_clicked();

    return threadArgs;
}

QStringList WizardDialog::argsLine()
{
    QStringList tmpList;
    for (int i = 2; i < stackedWidget->count(); ++i) {
    foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
        QString name = w->accessibleName();
        if (!name.isEmpty()) {
            if (w->inherits("QCheckBox") && !qobject_cast<QCheckBox *>(w)->isChecked())
                tmpList.append("--"+name+"=no");
            else if (w->inherits("QRadioButton") && !isDefault(w))
                tmpList.append("--"+name);
            else if (w->inherits("QLineEdit") && !isDefault(w))
                tmpList.append("--"+name+"="+qobject_cast<QLineEdit *>(w)->text());
            else if (w->inherits("QComboBox") && !isDefault(w))
                tmpList.append("--"+name+"="+qobject_cast<QComboBox *>(w)->currentText());
            else if (w->inherits("QSpinBox") && !isDefault(w))
                tmpList.append("--"+name+"="
                                +QString::number(qobject_cast<QSpinBox *>(w)->value()));
            else if (w->inherits("QDoubleSpinBox") && !isDefault(w))
                tmpList.append("--"+name+"="
                                +QString::number(qobject_cast<QDoubleSpinBox *>(w)->value()));
        }
    }
    }
    if (chBoxFullLog->isChecked())
        tmpList.append("--quiet=no");

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
    } else if ("QLineEdit" == className
             && description != qobject_cast<QLineEdit *>(w)->text())
        flag = false;
    else if ("QSpinBox" == className
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

QStringList WizardDialog::getOutFiles()
{
    QStringList list;
    if        (radioBtn1->isChecked()) {
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
        list.replaceInStrings(QRegExp("svgz$"),"svg");
    else if (rBtnCompressAll->isChecked())
        list.replaceInStrings(QRegExp("svg$"),"svgz");
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

void WizardDialog::setPreset(const QString &preset)
{
    if (preset == "None") {
        resetToDefault();
        return;
    }

    QFile inputFile;
    for (int i = 0; i < presets.count(); ++i) {
        if (presets.at(i).baseName() == preset)
            inputFile.setFileName(presets.at(i).absoluteFilePath());
    }
    if (!inputFile.exists())
        return;

    inputFile.open(QFile::ReadOnly);
    QTextStream textStream(&inputFile);
    QStringList args = textStream.readAll().split("\n");
    inputFile.close();

    for (int i = 2; i < stackedWidget->count(); ++i) {
        foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
            foreach (QString name, args) {
                if (w->accessibleName().contains(QString(name).remove(QRegExp("=.*")))) {
                    args.removeOne(name); // small speedup
                    double pos = QString(QString(name).remove(QRegExp(".*="))).toDouble();
                    if (w->inherits("QCheckBox"))
                        qobject_cast<QCheckBox *>(w)->setChecked(false);
                    else if (w->inherits("QComboBox"))
                        qobject_cast<QComboBox *>(w)->setCurrentIndex(pos);
                    else if (w->inherits("QRadioButton"))
                        qobject_cast<QRadioButton *>(w)->setChecked(true);
                    else if (w->inherits("QLineEdit"))
                        qobject_cast<QLineEdit *>(w)->setText(QString(name).remove(QRegExp(".*=")));
                    else if (w->inherits("QSpinBox"))
                        qobject_cast<QSpinBox *>(w)->setValue(pos);
                    else if (w->inherits("QDoubleSpinBox"))
                        qobject_cast<QDoubleSpinBox *>(w)->setValue(pos);
                }
            }
        }
    }
}

void WizardDialog::resetToDefault()
{
    for (int i = 2; i < stackedWidget->count(); ++i) {
        foreach(QWidget *w, stackedWidget->widget(i)->findChildren<QWidget *>()) {
            if (w->inherits("QCheckBox"))
                qobject_cast<QCheckBox *>(w)->setChecked(true);
            else if (w->inherits("QComboBox"))
                qobject_cast<QComboBox *>(w)->setCurrentIndex(w->accessibleDescription().toInt());
            else if (w->inherits("QRadioButton"))
                qobject_cast<QRadioButton *>(w)->setChecked(true);
            else if (w->inherits("QLineEdit"))
                qobject_cast<QLineEdit *>(w)->setText(w->accessibleDescription());
            else if (w->inherits("QSpinBox"))
                qobject_cast<QSpinBox *>(w)->setValue(w->accessibleDescription().toInt());
            else if (w->inherits("QDoubleSpinBox"))
                qobject_cast<QDoubleSpinBox *>(w)->setValue(w->accessibleDescription().toDouble());
        }
    }
}

void WizardDialog::resetFields()
{
    chBoxFullLog->setChecked(false);
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

    QString path = QFileDialog::getExistingDirectory(this,tr("Select an input folder"),
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

    QString path = QFileDialog::getExistingDirectory(this,tr("Select an output folder"),
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
        createWarning(tr("You have to set a prefix and a suffix for this save method."));
        check = false;
    } else if (!QDir(lineEditInDir->text()).exists()) {
        createWarning(tr("An input folder is not exist."));
        check = false;
    } else if (fileList.isEmpty()) {
        createWarning(tr("An input folder did not contain any svg, svgz files."));
        check = false;
    } else if (!checkFor7z()) {
        createWarning(tr("You have to install 7-Zip to use SVG Cleaner."));
        check = false;
    } else if (!checkForPerl()) {
        createWarning(tr("You have to install Perl to use SVG Cleaner."));
        check = false;
    }
    return check;
}

void WizardDialog::createWarning(const QString &text)
{
    QMessageBox::warning(this,tr("Warning"),text,QMessageBox::Ok);
}

/*
    On Windows:
    You have to download 7-Zip CLI. Then rename 7za.exe to 7z.exe
    and put it right to SVGCleaner.exe.

    http://downloads.sourceforge.net/sevenzip/7za920.zip
*/
bool WizardDialog::checkFor7z()
{
    QProcess zipProc;
    zipProc.start("7z");
    zipProc.waitForFinished();
    return !QString(zipProc.readAll()).isEmpty();
}

bool WizardDialog::checkForPerl()
{
#ifdef Q_OS_LINUX
    QProcess perlProc;
    perlProc.start("which",QStringList("perl"));
    perlProc.waitForFinished();
    return !QString(perlProc.readAll()).contains("which: no perl in");
#else
    return true;
#endif
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
    QString path = QFileInfo(settings->fileName()).absolutePath()+"/presets/";
    // overwrite old preset?
    if (QFile(path+linePresetName->text()+".preset").exists() && linePresetName->text() != "last") {
        int ansver = QMessageBox::warning(this,tr("Warning"),
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
        if (!name.isEmpty() || w->isVisible()) {
            if (w->inherits("QCheckBox") && !qobject_cast<QCheckBox *>(w)->isChecked())
                textStream << name+"=no"+"\n";
            else if (w->inherits("QRadioButton") && !isDefault(w))
                textStream << name+"\n";
            else if (w->inherits("QLineEdit") && !isDefault(w))
                textStream << name+"="+qobject_cast<QLineEdit *>(w)->text()+"\n";
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

    QString path = QFileInfo(settings->fileName()).absolutePath()+"/preset/";
    QFile file(path+cmbBoxPreset->currentText()+".preset");
    btnRemovePreset->setVisible(file.exists());
}

void WizardDialog::on_btnRemovePreset_clicked()
{
    QString path = QFileInfo(settings->fileName()).absolutePath()+"/preset/";
    QFile file(path+cmbBoxPreset->currentText()+".preset");
    file.remove();
    cmbBoxPreset->removeItem(cmbBoxPreset->currentIndex());
}

void WizardDialog::saveSettings()
{
    settings->setValue("Wizard/recursive",chBoxRecursive->isChecked());
    settings->setValue("Wizard/lastInDir",lineEditInDir->text());
    settings->setValue("Wizard/lastOutDir",lineEditOutDir->text());
    settings->setValue("Wizard/prefix",lineEditPrefix->text());
    settings->setValue("Wizard/suffix",lineEditSuffix->text());
    settings->setValue("Wizard/compress",gBoxCompress->isChecked());
    settings->setValue("Wizard/compressLevel",cmbBoxCompress->currentIndex());
    settings->setValue("Wizard/compressType",rBtnSaveSuffix->isChecked());
    settings->setValue("Wizard/preset",cmbBoxPreset->currentIndex());
    settings->setValue("Wizard/fullLog",chBoxFullLog->isChecked());
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
