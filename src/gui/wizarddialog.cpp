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

#include "../cli/keys.h"

#include "spinbox.h"
#include "someutils.h"
#include "wizarddialog.h"

// TODO: rewrite presets
// TODO: save last options automatically
// TODO: save as svg when src is svgz
// TODO: add one file processing support
// TODO: add Advanced chbox

bool WizardDialog::m_isRecursive = true;
bool WizardDialog::m_isStopScan  = false;

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent)
{
    Keys::get().prepareDescription();
    setupUi(this);
    initGUI();
    loadSettings();
    adjustSize();
}

WizardDialog::~WizardDialog()
{
    m_isStopScan = true;
    m_folderWatcher->cancel();
    delete m_folderWatcher;
}

void WizardDialog::loadSettings()
{
    Settings settings;

    chBoxRecursive->setChecked(settings.flag(SettingKey::Wizard::RecursiveScan));
    lineEditInDir->setText(settings.string(SettingKey::Wizard::LastInDir));
    lineEditOutDir->setText(settings.string(SettingKey::Wizard::LastOutDir, QDir::homePath()));
    lineEditPrefix->setText(settings.string(SettingKey::Wizard::Prefix));
    lineEditSuffix->setText(settings.string(SettingKey::Wizard::Suffix, "_cleaned"));

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
                QString key = w->property("key").toString();
                if (!key.isEmpty() && !keyList.filter(key).isEmpty()) {
                    if (w->inherits("QCheckBox")) {
                        qobject_cast<QCheckBox *>(w)->setChecked(true);
                        keyList.removeOne(key);
                    }
                    else if (w->inherits("SpinBox")) {
                        QString ckey = keyList.filter(key).first();
                        ckey.remove(key).remove("=");
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

void WizardDialog::initGUI()
{
    initElementsPage();
    initAttributesPage();
    initPathsPage();
    initOptimizationPage();

    // setup type radioButtons
    connect(radioBtn1, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    connect(radioBtn2, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    connect(radioBtn3, SIGNAL(clicked()), this, SLOT(onRadioSelected()));
    QString str = "radioBtn";
    Settings settings;
    str.append(settings.string(SettingKey::Wizard::SaveMode, "1"));
    QRadioButton *rbtn = findChild<QRadioButton *>(str);
    rbtn->click();

    connect(lineEditInDir,  SIGNAL(textChanged(QString)), this, SLOT(loadFiles()));
    connect(chBoxRecursive, SIGNAL(clicked()),            this, SLOT(loadFiles()));

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
    pageList << tr("Main")
             << tr("Elements")
             << tr("Attributes")
             << tr("Paths")
             << tr("Optimizations");
    QStringList pageListNotTr;
    pageListNotTr << "main" << "elements" << "attributes" << "paths"
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
    listWidget->setFixedWidth(baseIconSize*listWidget->count() + 5);
    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,       SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
    listWidget->setCurrentRow(0);
    listWidget->installEventFilter(this);

    // setup icons
    btnOpenInDir->setIcon( QIcon(":/open.svgz"));
    btnOpenOutDir->setIcon(QIcon(":/open.svgz"));
    setWindowIcon(QIcon(":/svgcleaner.svgz"));
    listWidget->setFocus();

    m_folderWatcher = new QFutureWatcher<QFileInfoList>(this);
    connect(m_folderWatcher, SIGNAL(resultReadyAt(int)), SLOT(onFolderScaned(int)));
    connect(m_folderWatcher, SIGNAL(finished()), SLOT(onFolderScanFinished()));
    loadFiles();
}

QVBoxLayout* WizardDialog::addPage()
{
    QScrollArea *area = new QScrollArea(this);
    area->setWidgetResizable(true);
    area->setFrameShape(QFrame::NoFrame);

    QWidget *w = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(w);

    m_pageList << w;
    area->setWidget(w);
    stackedWidget->addWidget(area);
    return lay;
}

void WizardDialog::addUtilsLabel(QVBoxLayout *layout)
{
    // TODO: add info button
    QLabel *lblUtils = new QLabel(tr("Additional:"), this);
    layout->addSpacing(40);
    layout->addWidget(lblUtils);
}

void WizardDialog::initElementsPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const QString &key, Keys::get().elementsKeys()) {
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
    foreach (const QString &key, Keys::get().attributesKeys()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    addUtilsLabel(lay);
    foreach (const QString &key, Keys::get().attributesUtilsKeys()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::initPathsPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const QString &key, Keys::get().pathsKeys()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::initOptimizationPage()
{
    QVBoxLayout* lay = addPage();
    foreach (const QString &key, Keys::get().optimizationsKeys()) {
        if (key != Key::TransformPrecision && key != Key::CoordsPrecision && key != Key::AttributesPrecision) {
            QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
            chBox->setProperty("key", key);
            lay->addWidget(chBox);
        } else {
            SpinBox *spinBox = new SpinBox(false, this);
            spinBox->setRange(0, 8);
            spinBox->setText(Keys::get().description(key));
            spinBox->setProperty("key", key);
            lay->addWidget(spinBox);
        }
    }
    addUtilsLabel(lay);
    foreach (const QString &key, Keys::get().optimizationsUtilsKeys()) {
        QCheckBox *chBox = new QCheckBox(Keys::get().description(key), this);
        chBox->setProperty("key", key);
        lay->addWidget(chBox);
    }
    lay->addStretch();
}

void WizardDialog::onRadioSelected()
{
    frameOutDir->setVisible(radioBtn1->isChecked());
    frameRename->setVisible(radioBtn2->isChecked());
    QRadioButton *rBtn = static_cast<QRadioButton *>(sender());
    Settings settings;
    settings.setValue(SettingKey::Wizard::SaveMode, rBtn->accessibleName());
}

void WizardDialog::createExample()
{
    lblExample->setText(tr("For example") + ": " + lineEditPrefix->text()
                        + tr("filename" ) + lineEditSuffix->text() + ".svg");
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

    foreach (QWidget *page, m_pageList) {
        foreach(QWidget *w, page->findChildren<QWidget *>()) {
            QString key = w->property("key").toString();
            if (!key.isEmpty()) {
                if (w->inherits("QCheckBox"))
                    qobject_cast<QCheckBox *>(w)->setChecked(Keys::get().flag(key));
                else if (w->inherits("SpinBox"))
                    qobject_cast<SpinBox *>(w)->setValue(Keys::get().doubleNumber(key));
            }
        }
    }
}

void WizardDialog::loadFiles()
{
    if (!QFile(lineEditInDir->text()).exists()) {
        lineEditInDir->setValue(0);
        return;
    }
    lineEditInDir->showLoading(true);
    m_isStopScan = true;
    m_folderWatcher->cancel();
    m_folderWatcher->waitForFinished();
    m_isStopScan = false;
    m_folderWatcher->setFuture(QtConcurrent::mapped(QStringList(lineEditInDir->text()),
                                                    &WizardDialog::scanFolder));
}

QFileInfoList WizardDialog::searchForFiles(const QString &startDir, bool recursive)
{
    QDir dir(startDir);
    QFileInfoList list;
    foreach (QString file, dir.entryList(QStringList() << "*.svg" << "*.svgz",  QDir::Files))
        list += QFileInfo(startDir + "/" + file);
    if (recursive && !m_isStopScan) {
        foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
            list += searchForFiles(startDir + "/" + subdir, recursive);
    }
    return list;
}

QFileInfoList WizardDialog::scanFolder(const QString &dirPath)
{
    return searchForFiles(dirPath, m_isRecursive);
}

void WizardDialog::onFolderScaned(int value)
{
    m_fileList = m_folderWatcher->resultAt(value);
}

void WizardDialog::onFolderScanFinished()
{
    lineEditInDir->setValue(m_fileList.size());
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
    QStringList args = argsList();

    foreach (QFileInfo file, m_fileList) {
        ToThread tth;
        tth.inputFile = file.absoluteFilePath();
        tth.decompress = tth.inputFile.endsWith("svgz");
        if (radioBtn1->isChecked()) {
            QString path =   QDir(lineEditOutDir->text()).absolutePath()
                           + QString(file.absoluteFilePath())
                             .remove(QDir(lineEditInDir->text()).absolutePath());
            if (path.endsWith("svgz"))
                path.chop(1);
            tth.outputFile = path;
        } else if (radioBtn2->isChecked()) {
            tth.outputFile =   file.absolutePath() + QDir::separator() + lineEditPrefix->text()
                             + file.baseName() + lineEditSuffix->text() + ".svg";
        } else if (radioBtn3->isChecked()) {
            tth.outputFile = tth.inputFile;
            if (tth.outputFile.endsWith("svgz"))
                tth.outputFile.chop(1);
        }
        tth.compress = false;
        if (rBtnSaveSuffix->isChecked()) {
            if (file.suffix().toLower() == "svgz")
                tth.compress = true;
        } else if (rBtnCompressAll->isChecked()) {
            tth.compress = true;
        }
        tth.args          = args;
        tth.compressLevel = compressLevel;
        list << tth;
    }
    return list;
}

QStringList WizardDialog::argsList()
{
    QStringList tmpList;
    if (cmbBoxPreset->currentText() == tr("Basic"))
        tmpList << "--preset=" + Preset::Basic;
    else if (cmbBoxPreset->currentText() == tr("Complete"))
        tmpList << "--preset=" + Preset::Complete;
    else if (cmbBoxPreset->currentText() == tr("Extreme"))
        tmpList << "--preset=" + Preset::Extreme;
    else if (cmbBoxPreset->currentText() == tr("Custom"))
        tmpList << "--preset=" + Preset::Custom;

    bool isCustom = false;
    QList<QWidget *> allWidgets;
    foreach (QWidget *page, m_pageList) {
        foreach(QWidget *w, page->findChildren<QWidget *>()) {
            QString key = w->property("key").toString();
            if (!key.isEmpty()) {
                allWidgets << w;
            }
        }
    }
    foreach (QWidget *w, allWidgets) {
        if (w->inherits("QCheckBox")) {
            bool isChecked = qobject_cast<QCheckBox *>(w)->isChecked();
            QString key = w->property("key").toString();
            if (Keys::get().flag(key) != isChecked && !isChecked)
                isCustom = true;
        }
    }
    if (isCustom) {
        tmpList.replace(0, "--preset=" + Preset::Custom);
        foreach (QWidget *w, allWidgets) {
            if (w->inherits("QCheckBox")) {
                if (qobject_cast<QCheckBox *>(w)->isChecked())
                    tmpList << w->property("key").toString();
            } else if (w->inherits("SpinBox")) {
                qreal value = qobject_cast<SpinBox *>(w)->value();
                tmpList << w->property("key").toString() + "=" + QString::number(value);
            }
        }
    } else {
        foreach (QWidget *w, allWidgets) {
            QString key = w->property("key").toString();
            if (w->inherits("QCheckBox")) {
                bool isChecked = qobject_cast<QCheckBox *>(w)->isChecked();
                if (Keys::get().flag(key) != isChecked && isChecked)
                    tmpList << key;
            } else if (w->inherits("SpinBox")) {
                qreal value = qobject_cast<SpinBox *>(w)->value();
                if (Keys::get().doubleNumber(key) != value)
                    tmpList << key + "=" + QString::number(value);
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
    } else if (m_fileList.isEmpty()) {
        createWarning(tr("An input folder did not contain any svg, svgz files."));
        check = false;
#ifdef Q_OS_WIN
    } else if (!QFile("./svgcleaner-cli.exe").exists()) {
#else
    } else if (!QFile("./svgcleaner-cli").exists()) {
#endif
        createWarning(tr("The 'svgcleaner-cli' executable is not found."));
        check = false;
    } else if (!QFile(SomeUtils::zipPath()).exists()) {
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
    settings.setValue(SettingKey::Wizard::LastInDir,        lineEditInDir->text());
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
    QStringList list = argsList();
    if (list.first().contains(Preset::Custom))
        settings.setValue(SettingKey::Wizard::Preset, cmbBoxPreset->findText(tr("Custom")));
    list.removeFirst();
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

void WizardDialog::on_chBoxRecursive_toggled(bool checked)
{
    m_isRecursive = checked;
}
