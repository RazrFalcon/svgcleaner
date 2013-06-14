#include <QtCore/QFileInfo>

#include "someutils.h"
#include "thumbwidget.h"

ThumbWidget::ThumbWidget(const SVGInfo &info, bool compare, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    // fix Windows/Gnome no-frame to ugly frame...
#ifdef Q_OS_WIN
    frame->setFrameShadow(QFrame::Plain);
#endif

    lbl1->setText("<b>" + tr("Name:") + "</b>");
    refill(info, compare);
}

void ThumbWidget::refill(const SVGInfo &info, bool compare)
{
    iconsWidget->setPaths(info.inPath, info.outPath,compare);

    lblName->setMinimumWidth(300);
    QFont font = QFont().defaultFamily();
    font.setBold(true);
    QFontMetrics fm(font);
    QString name = fm.elidedText(QFileInfo(info.outPath).fileName(),
                                 Qt::ElideLeft, lblName->width());
    lblName->setText("<b>" + name + "</b>");

    if (!info.errString.isEmpty()) {
        lblSizes->setText("0 -> 0 (0.00%)");
        lblElem->setText( "0 -> 0 (0.00%)");
        lblAttr->setText( "0 -> 0 (0.00%)");
        lblTime->setText( "0");
        iconsWidget->setError(info.errString);
    } else {
        lblSizes->setText(QString("%1 -> %2 (%3%)")
                          .arg(SomeUtils::prepareSize(info.inSize))
                          .arg(SomeUtils::prepareSize(info.outSize))
                          .arg(QString::number(info.compress, 'f', 2)));

        float elemPerc = ((float)info.elemFinal / info.elemInitial) * 100;
        lblElem->setText(QString("%1 -> %2 (%3%)")
                         .arg(info.elemInitial)
                         .arg(info.elemFinal)
                         .arg(QString::number(elemPerc, 'f', 2)));

        float attrPerc = ((float)info.attrFinal / info.attrInitial) * 100;
        lblAttr->setText(QString("%1 -> %2 (%3%)")
                         .arg(info.attrInitial)
                         .arg(info.attrFinal)
                         .arg(QString::number(attrPerc, 'f', 2)));
        lblTime->setText(SomeUtils::prepareTime(info.time));
    }
}
