#include <QFileInfo>
#include <QtDebug>

#include "thumbwidget.h"
#include "someutils.h"

ThumbWidget::ThumbWidget(const SVGInfo &info, bool compare, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    // fix Windows no-frame to ugly frame...
#ifdef Q_OS_WIN
    frame->setFrameShadow(QFrame::Plain);
#endif
    refill(info,compare);
}

void ThumbWidget::refill(const SVGInfo &info, bool compare)
{
    iconsWidget->setMinimumHeight(height()-20);
    iconsWidget->setPaths(info.paths[SVGInfo::INPUT],info.paths[SVGInfo::OUTPUT],compare);

    lblValues->setAccessibleName(QFileInfo(info.paths[SVGInfo::OUTPUT]).fileName());
    QString name = QFileInfo(info.paths[SVGInfo::OUTPUT]).fileName();
    if (name.count() > 40)
        name = "..."+name.rightRef(40).toString();

    if (info.crashed) {
        lblValues->setText(name);
        iconsWidget->setCrashed(true);
    } else {
        float elemPerc = ((float)info.elemFinal/info.elemInitial)*100;
        float attrPerc = ((float)info.attrFinal/info.attrInitial)*100;
        lblValues->setText(QString("%1<table align=right>"
            "<tr><td></td><td style=line-height:110% align=center>%2 </td><td>-&gt; </td>"
                                   "<td align=center>%3 </td><td align=right>(%4)</td></tr>"
            "<tr><td></td><td style=line-height:110% align=center>%5 </td><td>-&gt; </td>"
                                   "<td align=center>%6 </td><td align=right>(%7)</td></tr>"
            "<tr><td></td><td style=line-height:110% align=center>%8 </td><td>-&gt; </td>"
                                   "<td align=center>%9 </td><td align=right>(%10)</td></tr>"
            "<tr><td></td><td style=line-height:110% colspan=5 align=right>%11</td></tr>"
            "</table>")
             .arg(name)
             .arg(SomeUtils::prepareSize(info.sizes[SVGInfo::INPUT]))
             .arg(SomeUtils::prepareSize(info.sizes[SVGInfo::OUTPUT]))
             .arg(QString::number(info.compress,'f',2))
             .arg(info.elemInitial)
             .arg(info.elemFinal)
             .arg(QString::number(elemPerc,'f',2))
             .arg(info.attrInitial)
             .arg(info.attrFinal)
             .arg(QString::number(attrPerc,'f',2))
             .arg(SomeUtils::prepareTime(info.time))
             );
    }
}
