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

    bool checkFor(const QString &name);
    bool checkForWarnings();
    bool isDefault(QWidget *w);
    QString compressValue();
    QStringList argsLine();
    QStringList getInFiles();
    QStringList getOutFiles();
    void createWarning(const QString &text);
    void loadPresets();
    void loadSettings();
    void resetFields();
    void resetToDefault();
    void saveSettings();
    void setupGUI();

private slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void createExample();
    void loadFiles();
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
