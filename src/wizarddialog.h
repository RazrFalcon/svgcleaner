#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileInfoList>

#include "guiinfo.h"
#include "arguments.h"
#include "ui_wizarddialog.h"

class WizardDialog : public QDialog, private Ui::WizardDialog
{
    Q_OBJECT

public:
    explicit WizardDialog(QWidget *parent = 0);
    ~WizardDialog();
    ToThread threadArguments();

private:
    GuiInfo *gInfo;
    QFileInfoList fileList;
    QSettings *settings;
    bool checkFor7z();
    bool checkForPerl();
    bool checkForWarnings();
    QString argsLine();
    QStringList getInFiles();
    QStringList getOutFiles();
    void createWarning(const QString &text);
    void loadSettings();
    void resetFields();
    void resetItems(const QString &preset);
    void saveSettings();
    void setupGUI();

private slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void createExample();
    void createPage(const QString &name, const QString &iconName, QWidget *widget);
    void loadFiles();
    void on_btnOpenInDir_clicked();
    void on_btnOpenOutDir_clicked();
    void on_btnRmPreset_clicked();
    void on_btnSavePreset_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_cmbBoxPreset_currentIndexChanged(const QString &name);
    void radioSelected();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WIZARDDIALOG_H
