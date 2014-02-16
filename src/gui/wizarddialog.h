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

#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H

#include <QtGui/QDialog>

#include "arguments.h"
#include "ui_wizarddialog.h"

class WizardDialog : public QDialog, private Ui::WizardDialog
{
    Q_OBJECT

public:
    explicit WizardDialog(QWidget *parent = 0);
    ~WizardDialog();
    void setPathList(const QStringList &list);
    QList<ToThread> threadData();

private:
    QList<QWidget *> m_pageList;

private:
    bool checkForWarnings();
    QString compressValue();
    QString findLabel(const QString &accessibleName);
    QStringList argsList(bool *isCustom);
    void createWarning(const QString &text);
    void loadSettings();
    void resetFields();
    void resetToDefault();
    void saveSettings();
    void initGUI();
    void initElementsPage();
    void initAttributesPage();
    void initPathsPage();
    void initOptimizationPage();
    void deleteThreads();
    QVBoxLayout* addPage();
    void addUtilsLabel(QVBoxLayout *layout);

private slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void createExample();
    void on_btnOpenOutDir_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_cmbBoxPreset_currentIndexChanged(const QString &presetName);
    void onRadioSelected();
    void on_btnAddFiles_clicked();
    void on_btnAddFolder_clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WIZARDDIALOG_H
