#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setSvgFile(const QString &path_suffix);
    void setFiles(const QString &orig, const QString &cleaned, const QString &diff);
    void setAE(const QString &text);
    void setJsonPath(const QString &path);

private slots:
    void on_actionMarkAsValid_triggered();
    void on_actionStop_triggered();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    QString m_jsonPath;
    QString m_validAE;
    QString m_svgName;
};

#endif // MAINWINDOW_H
