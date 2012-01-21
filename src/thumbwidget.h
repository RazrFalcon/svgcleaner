#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "arguments.h"
#include "iconbutton.h"
#include "ui_thumbwidget.h"

class ThumbWidget : public QWidget, private Ui::ThumbWidget
{
    Q_OBJECT

public:
    ThumbWidget(const SVGInfo &info, bool compare = false, QWidget *parent = 0);
    void refill(const SVGInfo &info, bool compare = false);

private:
    SVGInfo fullInfo;
    IconButton *btnIn;
    IconButton *btnOut;
    bool flag;

private slots:
    void openSVG();

protected:
    void resizeEvent(QResizeEvent *);
};
#endif // THUMBWIDGET_H
