#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    tabWidget->setCurrentIndex(0); // show always first tab
}
