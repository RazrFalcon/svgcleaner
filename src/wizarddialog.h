#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileInfoList>

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
    QFileInfoList fileList;
    QFileInfoList presets;
    QSettings *settings;
    QStringList pageList;

    bool checkFor7z();
    bool checkForPerl();
    bool checkForWarnings();
    bool isDefault(QWidget *w);
    QStringList argsLine();
    QStringList getInFiles();
    QStringList getOutFiles();
    void createWarning(const QString &text);
    void loadSettings();
    void loadPresets();
    void resetFields();
    void resetToDefault();
    QString compressValue();
    void saveSettings();
    void setupGUI();

private slots:
    void resetItems(const QString &preset);
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void createExample();
    void loadFiles();
    void on_btnOpenInDir_clicked();
    void on_btnOpenOutDir_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void radioSelected();
    void on_btnSavePreset_clicked();
    void on_linePresetName_textChanged(const QString &text);
    void on_btnRemovePreset_clicked();
    void on_cmbBoxPreset_currentIndexChanged(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WIZARDDIALOG_H
