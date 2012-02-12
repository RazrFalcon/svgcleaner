#include <QIcon>
#include "itemwidget.h"

ItemWidget::ItemWidget(int icon, QWidget *parent) :
    QWidget(parent),
    lblIcon(new QLabel()),
    layout(new QVBoxLayout())
{
    lblIcon->setPixmap(QIcon(":/"+iconName(icon)+".svgz").pixmap(46,46));
    lblIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblIcon);
    setLayout(layout);
}

QString ItemWidget::iconName(int pos)
{
    switch (pos) {
        case 0 : return "files";
        case 1 : return "presets";
        case 2 : return "elements";
        case 3 : return "attributes";
        case 4 : return "paths";
        case 5 : return "optimization";
        case 6 : return "output";
    }
    return "files";
}

ItemWidget::~ItemWidget()
{
    delete lblIcon;
    delete layout;
}
