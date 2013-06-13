#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    lblTitle->setText("SVG Cleaner " + QApplication::applicationVersion());
    tabWidget->setCurrentIndex(0); // show always first tab
}
