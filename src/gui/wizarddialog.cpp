/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QWheelEvent>
#include <QtCore/QThread>
#include <QtDebug>

#include "../cli/keys.h"

#include "spinbox.h"
#include "someutils.h"
#include "settings.h"
#include "wizarddialog.h"

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent)
{
    Keys::get().prepareDescription();
    setupUi(this);
    initGUI();
    loadSettings();

    QSize windowSize = Settings().value(SettingKey::GUI::WizardSize).toSize();
    if (windowSize.isEmpty())
        adjustSize();
    else
        resize(windowSize);
}

WizardDialog::~WizardDialog()
{
    Settings().setValue(SettingKey::GUI::WizardSize, size());
}

void WizardDialog::initGUI()
{
    initElementsPage();
    initAttributesPage();
    initPathsPage();
    initOptimizationPage();
#ifdef Q_OS_MAC
    this->setStyleSheet("QScrollArea { background-color:transparent }");
#endif

    // setup type radioButtons
    connect(radioBtn1, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    connect(radioBtn2, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    connect(radioBtn3, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    QString str = "radioBtn";
    Settings settings;
    str.append(settings.string(SettingKey::Wizard::SaveMode, "1"));
    QRadioButton *rbtn = findChild<QRadioButton *>(str);
    rbtn->click();

    connect(chBoxRecursive, SIGNAL(toggled(bool)), treeView, SLOT(setRecursive(bool)));

    // Placeholder text work only on qt > 4.7
#if QT_VERSION >= 0x040700
    lineEditPrefix->setPlaceholderText(tr("prefix"));
    lineEditSuffix->setPlaceholderText(tr("suffix"));
#endif
    connect(lineEditPrefix, SIGNAL(textChanged(QString)), this, SLOT(createExample()));
    connect(lineEditSuffix, SIGNAL(textChanged(QString)), this, SLOT(createExample()));
    createExample();

    // list of all pages in wizard
    QStringList pageList;
    pageList << tr("Files")
             << tr("Preferences")
             << tr("Elements")
             << tr("Attributes")
             << tr("Paths")
             << tr("Optimizations");
    QStringList pageListNotTr;
    pageListNotTr << "files" << "preferences" << "elements" << "attributes" << "paths"
                  << "optimizations";
    // create icons for pages in "tabbar", which is QListWidget
#ifdef Q_OS_MAC
    int baseIconSize = 70;
#else
    int baseIconSize = 64;
#endif
    for (int i = 0; i < pageList.count(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(listWidget);
        QLabel *lblIcon = new QLabel(this);
        lblIcon->setPixmap(QIcon(":/" + pageListNotTr.at(i) + ".svgz").pixmap(46, 46));
        lblIcon->setAlignment(Qt::AlignCenter);
        item->setToolTip(pageList.at(i));
        item->setSizeHint(QSize(baseIconSize, baseIconSize));
        listWidget->setItemWidget(item, lblIcon);
    }
    listWidget->setFixedWidth(baseIconSize * listWidget->count() + 5);
    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,       SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
    listWidget->setCurrentRow(0);
    listWidget->installEventFilter(this);
    listWidget->setFocus();
}

void WizardDialog::loadSettings()
{
    Settings settings;

    chBoxRecursive->setChecked(settings.flag(SettingKey::Wizard::RecursiveScan));

    lineEditOutDir->setText(settings.string(SettingKey::Wizard::LastOutDir, QDir::homePath()));
    lineEditPrefix->setText(settings.string(SettingKey::Wizard::Prefix));
    lineEditSuffix->setText(settings.string(SettingKey::Wizard::Suffix, "_cleaned"));

    foreach (const QString &path, settings.value(SettingKey::Wizard::LastInPaths).toStringList())
        treeView->addRootPath(path, false);

    gBoxCompress->setChecked(settings.flag(SettingKey::Wizard::Compress));
    cmbBoxCompress->setCurrentIndex(settings.integer(SettingKey::Wizard::CompressLevel,
                                                     cmbBoxCompress->count()-1));
    if (settings.flag(SettingKey::Wizard::CompressType))
        rBtnSaveSuffix->setChecked(true);
    else
        rBtnCompressAll->setChecked(true);

    int presetNum = settings.integer(SettingKey::Wizard::Preset,
                                     cmbBoxPreset->findText(tr("Complete")));
    cmbBoxPreset->setCurrentIndex(-1);
    if (presetNum == -1)
        presetNum = 1;
    cmbBoxPreset->setCurrentIndex(presetNum);

    QStringList keyList = settings.value(SettingKey::Wizard::LastKeys).toStringList();
    if (!keyList.isEmpty()) {
        foreach (QWidget *page, m_pageList) {
            foreach(QWidget *w, page->findChildren<QWidget *>()) {
                bool ok = true;
                int key = w->property("key").toInt(&ok);
                if (ok && !keyList.filter(Keys::get().keyName(key)).isEmpty()) {
                    if (w->inherits("QCheckBox")) {
                        qobject_cast<QCheckBox *>(w)->setChecked(true);
                        keyList.removeOne(Keys::get().keyName(key));
                    }
                    else if (w->inherits("SpinBox")) {
                        QString ckey = keyList.filter(Keys::get().keyName(key)).first();
                        ckey.remove(Keys::get().keyName(key)).remove("=");
                        qobject_cast<SpinBox *>(w)->setValue(ckey.toDouble());
                    }
                }
            }
        }
    }

    spinBoxThreads->setValue(settings.integer(SettingKey::Wizard::ThreadsCount,
                                              QThread::idealThreadCount()));
    spinBoxThreads->setMaximum(QThread::idealThreadCount());
    gBoxThreads->setChecked(settings.flag(SettingKey::Wizard::ThreadingEnabled));
}

QVBoxLayout* WizardDialog::addPage()
{
    QScrollArea *area = new QScrollArea(this);
    area->setWidgetResizable(true);
    area->setFrameShape(QFrame::NoFrame);

    QWidget *w = new QWidget(this);
#ifdef Q_OS_MAC
    w->setObjectName("pageWidget_" + QString::number(m_pageList.size()));
    w->setStyleSheet(QString("QWidget#%1 { background-color:transparent }").arg(w->objectName()));
#endif
    QVBoxLayout *lay = new QVBoxLayout(w);

    m_pageList << w;
    area->setWidget(w);
    stackedWidget->addWidget(area);
    return lay;
}

void WizardDialog::addUtilsLabel(QVBoxLayout *layout)
{
    QLabel *lblUtils = new QLabel(tr("Additional:"), this);
    lblUtils->setToolTip(tr("Options below do not increase cleaning value, but can be useful."));
    layout->addSpacing(40);
    layout->addWidget(lblUtils);
}

void WizardDialog::initElementsPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const int &key, Keys::get().elementsKeysId()) {
        if (key != Key::RemoveTinyGaussianBlur) {
            QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
            chBox->setProperty("key", key);
            lay->addWidget(chBox);
        } else {
            SpinBox *spinBox = new SpinBox(true, this);
            spinBox->setRange(0.0, 1.0);
            spinBox->setStep(0.01);
            spinBox->setText(Keys::get().description(key));
            spinBox->setProperty("key", key);
            lay->addWidget(spinBox);
        }
    }
    lay->addStretch();
}

void WizardDialog::initAttributesPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const int &key, Keys::get().attributesKeysId()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    addUtilsLabel(lay);
    foreach (const int &key, Keys::get().attributesUtilsKeysId()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::initPathsPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const int &key, Keys::get().pathsKeysId()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::initOptimizationPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const int &key, Keys::get().optimizationsKeysId()) {
        if (   key != Key::TransformPrecision
            && key != Key::CoordsPrecision
            && key != Key::AttributesPrecision) {
            QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
            chBox->setProperty("key", key);
            lay->addWidget(chBox);
        } else {
            SpinBox *spinBox = new SpinBox(false, this);
            spinBox->setRange(1, 8);
            spinBox->setText(Keys::get().description(key));
            spinBox->setProperty("key", key);
            lay->addWidget(spinBox);
        }
    }
    addUtilsLabel(lay);
    foreach (const int &key, Keys::get().optimizationsUtilsKeysId()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::createExample()
{
    lblExample->setText(tr("For example") + ": " + lineEditPrefix->text()
                        + tr("filename" ) + lineEditSuffix->text() + ".svg");
}

void WizardDialog::setPathList(const QStringList &list)
{
    treeView->clear(true);
    foreach (const QString &path, list)
        treeView->addRootPath(path);
}

void WizardDialog::onRadioSelected()
{
    frameOutDir->setVisible(radioBtn1->isChecked());
    frameRename->setVisible(radioBtn2->isChecked());
    QRadioButton *rBtn = qobject_cast<QRadioButton *>(sender());
    Settings settings;
    settings.setValue(SettingKey::Wizard::SaveMode, rBtn->accessibleName());
}

void WizardDialog::on_cmbBoxPreset_currentIndexChanged(const QString &presetName)
{
    if (presetName == tr("Basic"))
        Keys::get().setPreset(Preset::Basic);
    else if (presetName == tr("Complete"))
        Keys::get().setPreset(Preset::Complete);
    else if (presetName == tr("Extreme"))
        Keys::get().setPreset(Preset::Extreme);
    else if (presetName == tr("Custom"))
        Keys::get().setPreset(Preset::Custom);

    lblPresetInfo->setText(Keys::get().presetDescription(Keys::get().preset()));

    foreach (QWidget *page, m_pageList) {
        foreach(QWidget *w, page->findChildren<QWidget *>()) {
            bool ok = true;
            int key = w->property("key").toInt(&ok);
            if (ok) {
                if (w->inherits("QCheckBox"))
                    qobject_cast<QCheckBox *>(w)->setChecked(Keys::get().flag(key));
                else if (w->inherits("SpinBox"))
                    qobject_cast<SpinBox *>(w)->setValue(Keys::get().doubleNumber(key));
            }
        }
    }
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
    QDialogButtonBox::StandardButton btn = buttonBox->standardButton(button);
    if (btn == QDialogButtonBox::Ok) {
        if (checkForWarnings()) {
            saveSettings();
            accept();
        }
    } else if (btn == QDialogButtonBox::Cancel) {
        reject();
    } else if (btn == QDialogButtonBox::Reset) {
        resetFields();
    }
}

QList<ToThread> WizardDialog::threadData()
{
    QList<ToThread> list;
    QString compressLevel = compressValue();
    bool ok = false;
    QStringList args = argsList(&ok);

    foreach (const QString &rootPath, treeView->rootList()) {
        foreach (const QString &filePath, treeView->rootFiles(rootPath)) {
            ToThread tth;
            QFileInfo fileInfo(filePath);
            tth.inputFile = fileInfo.absoluteFilePath();

            if (radioBtn1->isChecked()) {
                QString path = lineEditOutDir->text() + "/";
                if (QFileInfo(rootPath).isDir())
                    path += QDir(rootPath).dirName() + "/" + QDir(rootPath).relativeFilePath(filePath);
                else
                    path += fileInfo.fileName();
                path = QFileInfo(path).absoluteFilePath();
                if (path.endsWith("svgz"))
                    path.chop(1);
                tth.outputFile = path;
            } else if (radioBtn2->isChecked()) {
                tth.outputFile = fileInfo.absolutePath()
                                 + "/" + lineEditPrefix->text()
                                 + fileInfo.baseName() + lineEditSuffix->text() + ".svg";
            } else if (radioBtn3->isChecked()) {
                tth.outputFile = tth.inputFile;
                if (tth.outputFile.endsWith("svgz"))
                    tth.outputFile.chop(1);
            }
            tth.compress = false;
            if (gBoxCompress->isChecked()) {
                if (rBtnSaveSuffix->isChecked()) {
                    if (fileInfo.suffix().toLower() == "svgz")
                        tth.compress = true;
                } else if (rBtnCompressAll->isChecked()) {
                    tth.compress = true;
                }
            }
            tth.decompress    = tth.inputFile.endsWith("svgz");
            tth.args          = args;
            tth.compressLevel = compressLevel;
            list << tth;
        }
    }
    return list;
}

QStringList WizardDialog::argsList(bool *isCustom)
{
    QStringList tmpList;

    QList<QWidget *> allWidgets;
    foreach (QWidget *page, m_pageList) {
        foreach(QWidget *w, page->findChildren<QWidget *>()) {
            bool ok = true;
            w->property("key").toInt(&ok);
            if (ok)
                allWidgets << w;
        }
    }

    if (isCustom) {
        *isCustom = false;
        foreach (QWidget *w, allWidgets) {
            bool ok = true;
            int key = w->property("key").toInt(&ok);
            if (ok) {
                if (w->inherits("QCheckBox")) {
                    bool isChecked = qobject_cast<QCheckBox *>(w)->isChecked();
                    if (Keys::get().flag(key) != isChecked)
                        *isCustom = true;
                } else if (w->inherits("SpinBox")) {
                    if (Keys::get().doubleNumber(key) != qobject_cast<SpinBox *>(w)->value())
                        *isCustom = true;
                }
            }
        }
    }
    if (*isCustom) {
        foreach (QWidget *w, allWidgets) {
            if (w->inherits("QCheckBox")) {
                if (qobject_cast<QCheckBox *>(w)->isChecked())
                    tmpList << Keys::get().keyName(w->property("key").toInt());
            } else if (w->inherits("SpinBox")) {
                qreal value = qobject_cast<SpinBox *>(w)->value();
                tmpList << Keys::get().keyName(w->property("key").toInt()) + "=" + QString::number(value);
            }
        }
    } else {
        if (cmbBoxPreset->currentText() == tr("Basic"))
            tmpList << "--preset=" + Preset::Basic;
        else if (cmbBoxPreset->currentText() == tr("Complete"))
            tmpList << "--preset=" + Preset::Complete;
        else if (cmbBoxPreset->currentText() == tr("Extreme"))
            tmpList << "--preset=" + Preset::Extreme;
        foreach (QWidget *w, allWidgets) {
            int key = w->property("key").toInt();
            if (w->inherits("QCheckBox")) {
                bool isChecked = qobject_cast<QCheckBox *>(w)->isChecked();
                if (Keys::get().flag(key) != isChecked && isChecked)
                    tmpList << Keys::get().keyName(key);
            } else if (w->inherits("SpinBox")) {
                qreal value = qobject_cast<SpinBox *>(w)->value();
                if (Keys::get().doubleNumber(key) != value)
                    tmpList << Keys::get().keyName(key) + "=" + QString::number(value);
            }
        }
    }
    return tmpList;
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
    cmbBoxPreset->setCurrentIndex(cmbBoxPreset->findText(tr("Complete")));
    gBoxCompress->setChecked(true);
    treeView->clear(true);
    lineEditOutDir->setText(QDir::homePath());
    lineEditPrefix->clear();
    lineEditSuffix->setText("_cleaned");
    radioBtn1->click();
    rBtnSaveSuffix->setChecked(true);
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
    if (!treeView->hasFiles()) {
        createWarning(tr("No files are selected."));
        check = false;
    } else if (lineEditOutDir->text().isEmpty()) {
        createWarning(tr("An output folder is not selected."));
        check = false;
    } else if (lineEditPrefix->text().isEmpty() && lineEditSuffix->text().isEmpty()) {
        createWarning(tr("You have to set a prefix or a suffix for this save method."));
        check = false;
#ifdef Q_OS_WIN
    } else if (!QFile(QApplication::applicationDirPath() + "/svgcleaner-cli.exe").exists()) {
#else
    } else if (!QFile(QApplication::applicationDirPath() + "/svgcleaner-cli").exists()) {
#endif
        createWarning(tr("The 'svgcleaner-cli' executable is not found."));
        check = false;
#ifdef Q_OS_WIN
    } else if (!QFile(QApplication::applicationDirPath() + "/7za.exe").exists()) {
#else
    } else if (!QFile(QApplication::applicationDirPath() + "/7za").exists()) {
#endif
        createWarning(tr("The '7za' executable is not found.\n\n"
                                  "You will not be able to clean the SVGZ files."));
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

QString WizardDialog::findLabel(const QString &accessibleName)
{
    for (int i = 1; i < stackedWidget->count(); ++i) {
        foreach(QLabel *lbl, stackedWidget->widget(i)->findChildren<QLabel *>()) {
            if (lbl->accessibleName() == accessibleName)
                return lbl->text();
        }
    }
    return QString();
}

void WizardDialog::saveSettings()
{
    Settings settings;
    settings.setValue(SettingKey::Wizard::LastInPaths,      treeView->rootList());
    settings.setValue(SettingKey::Wizard::LastOutDir,       lineEditOutDir->text());
    settings.setValue(SettingKey::Wizard::Prefix,           lineEditPrefix->text());
    settings.setValue(SettingKey::Wizard::Suffix,           lineEditSuffix->text());
    settings.setValue(SettingKey::Wizard::RecursiveScan,    chBoxRecursive->isChecked());
    settings.setValue(SettingKey::Wizard::Compress,         gBoxCompress->isChecked());
    settings.setValue(SettingKey::Wizard::ThreadingEnabled, gBoxThreads->isChecked());
    settings.setValue(SettingKey::Wizard::CompressType,     rBtnSaveSuffix->isChecked());
    settings.setValue(SettingKey::Wizard::Preset,           cmbBoxPreset->currentIndex());
    settings.setValue(SettingKey::Wizard::CompressLevel,    cmbBoxCompress->currentIndex());
    settings.setValue(SettingKey::Wizard::ThreadsCount,     spinBoxThreads->value());
    bool isCustom = false;
    QStringList list = argsList(&isCustom);
    if (isCustom)
        settings.setValue(SettingKey::Wizard::Preset, cmbBoxPreset->findText(tr("Custom")));
    else
        list.clear();
    settings.setValue(SettingKey::Wizard::LastKeys, list);
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
    return QDialog::eventFilter(obj, event);
}

void WizardDialog::on_btnAddFiles_clicked()
{
    QString currPath = treeView->lastPath();
    if (currPath.isEmpty())
        currPath = QDir::homePath();

    QStringList paths = QFileDialog::getOpenFileNames(this, tr("Add files"),
                                                      currPath,
                                                      "SVG files (*.svg *.svgz)");
    if (!paths.isEmpty()) {
        foreach (const QString &path, paths)
            treeView->addRootPath(path);
    }
}

void WizardDialog::on_btnAddFolder_clicked()
{
    QString currPath = treeView->lastPath();
    if (currPath.isEmpty())
        currPath = QDir::homePath();

    QString path = QFileDialog::getExistingDirectory(this, tr("Add folder"),
                                                     currPath,
                                                     QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        treeView->addRootPath(path);
}
