/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H

#include <QtGui/QDialog>

#include <QtCore/QFileInfoList>
#include <QtCore/QSettings>
#include <QtCore/QThread>

#include "arguments.h"
#include "filefinder.h"
#include "ui_wizarddialog.h"

class WizardDialog : public QDialog, private Ui::WizardDialog
{
    Q_OBJECT

public:
    explicit WizardDialog(QWidget *parent = 0);
    ~WizardDialog();
    ToThread threadArguments();

signals:
    void start(QString, bool);

private:
    QFileInfoList fileList;
    QFileInfoList presets;
    QSettings *settings;
    FileFinder *fileSearch;
    QThread *searchThread;

    bool checkForWarnings();
    bool isDefault(QWidget *w);
    QString compressValue();
    QString findLabel(const QString &accessibleName);
    QString settingPath();
    QStringList argsLine();
    QStringList getInFiles();
    QStringList genOutFiles();
    void createWarning(const QString &text);
    void loadPresets();
    void loadSettings();
    void resetFields();
    void resetToDefault();
    void saveSettings();
    void setupGUI();
    void setupToolTips();
    void deleteThreads();

private slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void createExample();
    void loadFiles();
    void loadFinished(QFileInfoList list);
    void on_btnOpenInDir_clicked();
    void on_btnOpenOutDir_clicked();
    void on_btnRemovePreset_clicked();
    void on_btnSavePreset_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_cmbBoxPreset_currentIndexChanged(const QString &text);
    void on_linePresetName_textChanged(const QString &text);
    void radioSelected();
    void setPreset(const QString &preset);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WIZARDDIALOG_H
