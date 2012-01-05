#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent),
    lbl(new QLabel(this))
{
    this->setObjectName("path");

    lbl->setObjectName("files");
    lbl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    lbl->setStyleSheet("QLabel#files { border: none; padding: 4px; padding-left: 1px;"
                       "background-color: rgba(255, 255, 255, 0); color: #808080; }");
}

LineEdit::~LineEdit()
{
    delete lbl;
}

void LineEdit::setValue(const int &value)
{
    lbl->setText(QString("%1 files in").arg(value));
    setStyleSheet(QString("padding-left: %1px;").arg(lbl->fontMetrics().width(lbl->text())+5));
}
