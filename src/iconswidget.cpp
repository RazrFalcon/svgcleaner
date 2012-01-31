#include <QDesktopServices>
#include <QMouseEvent>
#include <QUrl>
#include <QBitmap>
#include <QTimer>
#include <QCursor>
#include <QPainter>
#include <QtDebug>

#include "iconswidget.h"

IconsWidget::IconsWidget(QWidget *parent) :
    QWidget(parent)
{
    toolTip = new QLabel();
    toolTip->setWindowFlags(Qt::SplashScreen);
    setMouseTracking(true);
}

void IconsWidget::setPaths(const QString &pathIn,const QString &pathOut,const bool compare)
{
    inpath = pathIn;
    outpath = pathOut;

    QPixmap pix1(pathIn);
    QPixmap pix2(pathOut);
    QImage image(pix1.width()+pix2.width()+20,pix2.height()+20,QImage::Format_ARGB32);
    image.fill(0);
    mainPix = QPixmap::fromImage(image,Qt::NoOpaqueDetection|Qt::AutoColor);
    QPainter painter(&mainPix);
    QPalette pal;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setPen(QPen(Qt::blue,2));
    painter.setBrush(QBrush(pal.color(QPalette::Background)));
    painter.drawRoundedRect(3,3,image.width()-7,image.height()-7,20,20);
    painter.drawPixmap((image.width()-pix1.width()-pix2.width())/2,
                       (image.height()-pix1.height())/2,pix1.width(),pix1.height(),pix1);
    painter.drawPixmap((image.width()-pix1.width()-pix2.width())/2+pix1.width(),
                       (image.height()-pix2.height())/2,pix2.width(),pix2.height(),pix2);
    painter.end();

    if (compare) {
        setFixedWidth(200);
    } else {
        outpath.clear();
        setFixedWidth(100);
    }
    if (mainPix.width() > 600)
        mainPix = mainPix.scaledToWidth(600,Qt::SmoothTransformation);
    toolTip->setPixmap(mainPix);
    toolTip->setMask(mainPix.mask());
    crashed = false;
    repaint();
}

void IconsWidget::setCrashed(bool flag)
{
    crashed = flag;
    repaint();
}

void IconsWidget::enterEvent(QEvent *)
{
    QTimer::singleShot(1000,this,SLOT(showToolTip()));
}

void IconsWidget::leaveEvent(QEvent *)
{
    toolTip->hide();
}

void IconsWidget::showToolTip()
{
    if (crashed)
        return;

    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (cursPos.x() > 0 && cursPos.y() > 0 && cursPos.y() < height() && cursPos.x() < width()) {
        toolTip->show();
    }
}

void IconsWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (crashed)
        return;

    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (cursPos.x() > 0 && cursPos.y() > 0 && cursPos.y() < height() && cursPos.x() < width()) {
        int border = 5;
        QPoint p = mapToGlobal(event->pos());
        QPoint p2 = mapToGlobal(QPoint(0,0));
        if (p.y()-mainPix.height()-12 > 0)
            toolTip->setGeometry(p.x()-mainPix.width()/2,p2.y()-mainPix.height()-12,
                              mainPix.width(),mainPix.height());
        else
            toolTip->setGeometry(p.x()-mainPix.width()/2,p2.y()+height()+border,
                              mainPix.width(),mainPix.height());
    }
}

void IconsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (crashed) {
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(),Qt::AlignCenter,tr("Crashed"));
    } else {
        QPixmap in = QPixmap(inpath).scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        int heightBorder = (height()-in.height())/2;
//        qDebug()<<in.width()<<in.height();
        painter.drawPixmap(0,heightBorder,in.width(),in.height(),in);
        if (!outpath.isEmpty()) {
            QPixmap out = QPixmap(outpath).scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            painter.drawPixmap(width()/2,heightBorder,out.width(),out.height(),out);
        }
    }
}

void IconsWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    if (crashed)
        return;

    if (event->x() < width()/2)
        QDesktopServices::openUrl(QUrl(inpath));
    else
        QDesktopServices::openUrl(QUrl(outpath));
}
