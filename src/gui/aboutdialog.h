#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_aboutdialog.h"

class AboutDialog : public QDialog, public Ui::AboutDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);

private:
    void fillAbout();
    void fillAuthors();
    QString genLink(const QString &link);
};

#endif // ABOUTDIALOG_H
