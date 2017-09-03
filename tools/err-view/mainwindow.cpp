#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString lblStyle = "background-color:white";
    ui->lblOrig->setStyleSheet(lblStyle);
    ui->lblCleaned->setStyleSheet(lblStyle);
    ui->lblDiff->setStyleSheet(lblStyle);

    ui->mainToolBar->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSvgFile(const QString &path_suffix)
{
    m_svgName = path_suffix;
}

void MainWindow::setFiles(const QString &orig, const QString &cleaned, const QString &diff)
{
    ui->lblOrig->setPixmap(QPixmap(orig));
    ui->lblCleaned->setPixmap(QPixmap(cleaned));
    ui->lblDiff->setPixmap(QPixmap(diff));
}

void MainWindow::setAE(const QString &text)
{
    m_validAE = text;
    ui->statusBar->showMessage("AE: " + text);
}

void MainWindow::setJsonPath(const QString &path)
{
    m_jsonPath = path;
}

void MainWindow::on_actionMarkAsValid_triggered()
{
    QFile f(m_jsonPath);
    if (!f.open(QFile::ReadWrite)) {
        qApp->exit(1);
        return;
    }

    auto doc = QJsonDocument::fromJson(f.readAll());
    auto map = doc.object().toVariantMap();
    auto cae_arr = map.value("custom_ae").toList();

    bool exist = false;
    for (int i = 0; i < cae_arr.size(); ++i) {
        QVariantMap map = cae_arr.at(i).toMap();
        if (map["name"] == m_svgName) {
            map.insert("valid_ae", m_validAE.toUInt());

            if (!ui->lineComment->text().isEmpty()) {
                map.insert("info", ui->lineComment->text());
            }

            cae_arr.replace(i, map);
            exist = true;

            break;
        }
    }

    if (!exist) {
        QVariantMap newItem;
        newItem.insert("name", m_svgName);
        newItem.insert("valid_ae", m_validAE.toUInt());
        if (!ui->lineComment->text().isEmpty()) {
            newItem.insert("info", ui->lineComment->text());
        }

        cae_arr.append(newItem);
    }

    map.insert("custom_ae", cae_arr);

    f.seek(0);
    f.write(QJsonDocument::fromVariant(map).toJson());

    qApp->exit(0);
}

void MainWindow::on_actionStop_triggered()
{
    qApp->exit(1);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    qApp->exit(1);
}
