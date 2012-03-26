#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    tabWidget->setCurrentIndex(0); // show always first tab
    // FIXME: move all text from .ui to .cpp for easier translation
}
