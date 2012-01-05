#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QSvgWidget>
#include <QtSvg/QtSvg>
#include <QtDebug>

#include "thumbwidget.h"
#include "someutils.h"

ThumbWidget::ThumbWidget(const SVGInfo &info, bool compare, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    btnIn = new QPushButton(this);
    btnIn->setFixedSize(70,70);
    btnIn->setFlat(true);
    btnIn->setAccessibleName("input");
    btnOut = new QPushButton(this);
    btnOut->setFixedSize(70,70);
    btnOut->setFlat(true);
    btnOut->setAccessibleName("output");
    iconLayout->addWidget(btnIn);
    iconLayout->addWidget(btnOut);
    connect(btnIn,SIGNAL(clicked()),this,SLOT(openSVG()));
    connect(btnOut,SIGNAL(clicked()),this,SLOT(openSVG()));

    iconLayout->setSpacing(4);
    refill(info,compare);
}

void ThumbWidget::refill(const SVGInfo &info, bool compare)
{
    fullInfo = info;

    if (compare) {
        QPixmap pixIn(info.paths[SVGInfo::INPUT]);
        if (pixIn.width() > 64)
            pixIn = pixIn.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        btnIn->show();
        btnIn->setIcon(QIcon(pixIn));
        btnIn->setIconSize(pixIn.size());
        if (pixIn.isNull() || info.crashed) {
        } else {
            btnIn->setText("");
        }
    } else {
        btnIn->hide();
    }

    QPixmap pixOut(info.paths[SVGInfo::OUTPUT]);
    if (pixOut.width() > 64)
        pixOut = pixOut.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    btnOut->setIcon(QIcon(pixOut));
    btnOut->setIconSize(pixOut.size());
    if (pixOut.isNull() || info.crashed) {

    } else
        btnOut->setText("");

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
        btnIn->setText(tr("crashed"));
        btnIn->setIcon(QIcon());

        btnOut->setText(tr("crashed"));
        btnOut->setIcon(QIcon());

        lblElemP->setText("(0.00%)");
        lblAttrP->setText("(0.00%)");
        lblTime->setText("0");

        // size
        lblSizeB->setText("0");
        lblSizeA->setText("0");
        lblSizeP->setText("(0.00%)");
    } else {
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

void ThumbWidget::openSVG()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (btn->accessibleName() == "input")
        QDesktopServices::openUrl(QUrl(fullInfo.paths[SVGInfo::INPUT], QUrl::TolerantMode));
    else
        QDesktopServices::openUrl(QUrl(fullInfo.paths[SVGInfo::OUTPUT], QUrl::TolerantMode));
}

void ThumbWidget::resizeEvent(QResizeEvent *event)
{
    QFontMetrics fm(QFont().defaultFamily());
    int size = event->size().width()-lbl1->width()-iconLayout->sizeHint().width()-25;
    QString normalStr = fm.elidedText(QFileInfo(fullInfo.paths[SVGInfo::OUTPUT]).fileName(),
                                      Qt::ElideLeft,size);
    lblName->setText(normalStr);
}
