#include <QResizeEvent>
#include <QFileInfo>

#include "thumbwidget.h"
#include "someutils.h"

ThumbWidget::ThumbWidget(const SVGInfo &info, bool compare, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
#ifdef Q_OS_WIN
    frame->setFrameShadow(QFrame::Plain);
#endif

//    iconsWidget = new IconsWidget();
//    gridLayout->addWidget(iconsWidget,0,0,5,1);

    refill(info,compare);
}

void ThumbWidget::refill(const SVGInfo &info, bool compare)
{
    fullInfo = info;

    lblName->setText(QFileInfo(info.paths[SVGInfo::OUTPUT]).fileName());
    lblName->setToolTip(tr("Input file: ")+info.paths[SVGInfo::INPUT]);

    SomeUtils utils;

    // elements
    lblElemB->setText(QString::number(info.elemInitial));
    lblElemA->setText(QString::number(info.elemFinal));

    // attributes
    lblAttrB->setText(QString::number(info.attrInitial));
    lblAttrA->setText(QString::number(info.attrFinal));

    // time
    lblTime->setText(utils.prepareTime(info.time));

    if (info.crashed) {
        lblElemP->setText("(0.00%)");
        lblAttrP->setText("(0.00%)");
        lblTime->setText("0");

        // size
        lblSizeB->setText("0");
        lblSizeA->setText("0");
        lblSizeP->setText("(0.00%)");
    } else {
        iconsWidget->setPaths(info.paths[SVGInfo::INPUT],info.paths[SVGInfo::OUTPUT],compare);
        // size
        lblSizeB->setText(utils.prepareSize(info.sizes[SVGInfo::INPUT]));
        lblSizeA->setText(utils.prepareSize(info.sizes[SVGInfo::OUTPUT]));
        lblSizeP->setText("("+QString::number(info.compress,'f',2)+"%)");

        lblElemP->setText("("+QString::number(((float)info.elemFinal/
                                               info.elemInitial)*100,'f',2)+"%)");
        lblAttrP->setText("("+QString::number(((float)info.attrFinal/
                                               info.attrInitial)*100,'f',2)+"%)");
    }
}

void ThumbWidget::resizeEvent(QResizeEvent *event)
{
    QFontMetrics fm(QFont().defaultFamily());
    int size = event->size().width()-lbl1->width()-iconsWidget->width()-25;
    QString normalStr = fm.elidedText(QFileInfo(fullInfo.paths[SVGInfo::OUTPUT]).fileName(),
                                      Qt::ElideLeft,size);
    lblName->setText(normalStr);
}

