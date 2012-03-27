#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QtGui/QWidget>

#include "arguments.h"
#include "ui_thumbwidget.h"

class ThumbWidget : public QWidget, private Ui::ThumbWidget
{
    Q_OBJECT

public:
    ThumbWidget(const SVGInfo &info, bool compare = false, QWidget *parent = 0);
    void refill(const SVGInfo &info, bool compare = false);
};
#endif // THUMBWIDGET_H
