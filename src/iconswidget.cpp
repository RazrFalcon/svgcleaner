#include <QDesktopServices>
#include <QMouseEvent>
#include <QUrl>
#include <QTimer>
#include <QCursor>
#include <QPainter>
#include <QtDebug>

#include "iconswidget.h"

IconsWidget::IconsWidget(QWidget *parent) :
    QWidget(parent)
{
    diag = new QLabel();
    diag->setWindowFlags(Qt::ToolTip);
    setMouseTracking(true);
//    setFixedWidth(200); // didn't work
}

void IconsWidget::setPaths(const QString &pathIn,const QString &pathOut,const bool compare)
{
    inpath = pathIn;

    QPixmap pix1(pathIn);
    if (compare) {
        outpath = pathOut;
        QPixmap pix2(pathOut);
        QImage image(pix1.width()+pix2.width(),pix2.height(),QImage::Format_ARGB32_Premultiplied);
        image.fill(0);
        mainPix = QPixmap::fromImage(image, Qt::NoOpaqueDetection | Qt::AutoColor);
        QPainter painter(&mainPix);
        painter.drawPixmap(0,0,pix1.width(),pix1.height(),pix1);
        painter.drawPixmap(pix2.width(),0,pix2.width(),pix2.height(),pix2);
        painter.end();
        setFixedWidth(200);
    } else {
        outpath.clear();
        mainPix = pix1;
        setFixedWidth(100);
    }
    if (mainPix.width() > 500)
        mainPix = mainPix.scaledToWidth(500,Qt::SmoothTransformation);
    diag->setPixmap(mainPix);
    isCrashed = false;
    repaint();
}

void IconsWidget::crashed()
{
    isCrashed = true;
}

void IconsWidget::enterEvent(QEvent *)
{
    QTimer::singleShot(1000,this,SLOT(showToolTip()));
}

void IconsWidget::leaveEvent(QEvent *)
{
    diag->hide();
}

void IconsWidget::showToolTip()
{
    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (cursPos.x() > 0 && cursPos.y() > 0 && cursPos.y() < height() && cursPos.x() < width()) {
        diag->show();
    }
}

void IconsWidget::mouseMoveEvent(QMouseEvent *event)
{
    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (cursPos.x() > 0 && cursPos.y() > 0 && cursPos.y() < height() && cursPos.x() < width()) {
        int border = 5;
        QPoint p = mapToGlobal(event->pos());
        QPoint p2 = mapToGlobal(QPoint(0,0));
        if (p.y()-mainPix.height()-12 > 0)
            diag->setGeometry(p.x()-mainPix.width()/2,p2.y()-mainPix.height()-12,
                              mainPix.width(),mainPix.height()+border);
        else
            diag->setGeometry(p.x()-mainPix.width()/2,p2.y()+height()+border,
                              mainPix.width(),mainPix.height()+border);
    }
}

void IconsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (isCrashed) {
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(),Qt::AlignCenter,tr("Creahed"));
    } else {
        QPixmap in = QPixmap(inpath).scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        int heightBorder = (height()-in.height())/2;
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

    if (event->x() < width()/2)
        QDesktopServices::openUrl(QUrl(inpath));
    else
        QDesktopServices::openUrl(QUrl(outpath));
}
