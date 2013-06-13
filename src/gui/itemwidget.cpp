#include <QtGui/QIcon>

#include "itemwidget.h"

ItemWidget::ItemWidget(const QString &icon, QWidget *parent) :
    QWidget(parent)
{
    lblIcon = new QLabel(this);
    layout = new QVBoxLayout(this);
    lblIcon->setPixmap(QIcon(":/" + icon + ".svgz").pixmap(46, 46));
    lblIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblIcon);
    setLayout(layout);
}

ItemWidget::~ItemWidget()
{
    delete lblIcon;
    delete layout;
}
