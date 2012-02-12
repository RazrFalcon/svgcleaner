#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent),
    lbl(new QLabel(this))
{
    this->setObjectName("path");

    lbl->setObjectName("files");
    lbl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    lbl->setStyleSheet("QLabel#files { border: none; padding: 3px; padding-left: 1px;"
                       "background-color: rgba(255, 255, 255, 0); color: #808080; }");
}

LineEdit::~LineEdit()
{
    delete lbl;
}

void LineEdit::setValue(const int &value)
{
    lbl->setText(QString(tr("%1 files")).arg(value));
    QFontMetrics fm(QFont().defaultFamily());
    setStyleSheet(QString("padding-left: %1px;").arg(fm.width(lbl->text())+6));
    lbl->setFixedWidth(fm.width(lbl->text())+8);
}
