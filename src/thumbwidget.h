#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "arguments.h"
//#include "iconswidget.h"
#include "ui_thumbwidget.h"

class ThumbWidget : public QWidget, private Ui::ThumbWidget
{
    Q_OBJECT

public:
    ThumbWidget(const SVGInfo &info, bool compare = false, QWidget *parent = 0);
    void refill(const SVGInfo &info, bool compare = false);

private:
    SVGInfo fullInfo;
//    IconsWidget *iconsWidget;

protected:
    void resizeEvent(QResizeEvent *);
};
#endif // THUMBWIDGET_H
