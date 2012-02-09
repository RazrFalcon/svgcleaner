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
    toolTip->setWindowFlags(Qt::ToolTip);
    setMouseTracking(true);
}

void IconsWidget::setPaths(const QString &pathIn,const QString &pathOut,const bool compare)
{
    inpath = pathIn;
    outpath = pathOut;
    compareView = compare;

    if (compare) {
        setFixedWidth(height()*2+20);
    } else {
        setFixedWidth(height()+20);
    }
    crashed = false;
    repaint();

    QPixmap pix1(pathIn);
    QPixmap pix2(pathOut);
    toolTipEnabled = true;
    if (pix1.height() < height() || pix2.height() < height()) {
        toolTipEnabled = false;
        return;
    }

    if (pix1.width() > 300)
        pix1 = pix1.scaledToWidth(300,Qt::SmoothTransformation);
    if (pix2.width() > 300)
        pix2 = pix2.scaledToWidth(300,Qt::SmoothTransformation);

    QImage image(pix1.width()+pix2.width()+20,pix2.height()+20,QImage::Format_ARGB32);
    image.fill(0);
    mainPix = QPixmap::fromImage(image,Qt::NoOpaqueDetection|Qt::AutoColor);
    QPainter painter(&mainPix);
    QPalette pal;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setBrush(QBrush(pal.color(QPalette::Background)));
    painter.setPen(QPen(QColor("#636363"),2));
    painter.drawRoundedRect(2,2,image.width()-4,image.height()-4,5,5);
    painter.drawPixmap((image.width()-pix1.width()-pix2.width())/2,
                       (image.height()-pix1.height())/2,pix1.width(),pix1.height(),pix1);
    painter.drawPixmap((image.width()-pix1.width()-pix2.width())/2+pix1.width(),
                       (image.height()-pix2.height())/2,pix2.width(),pix2.height(),pix2);
    painter.end();

    toolTip->setPixmap(mainPix);
    toolTip->setMask(mainPix.mask());
}

void IconsWidget::setCrashed(bool flag)
{
    crashed = flag;
    repaint();
}

void IconsWidget::enterEvent(QEvent *)
{
    if (toolTipEnabled)
        QTimer::singleShot(1000,this,SLOT(showToolTip()));
}

void IconsWidget::leaveEvent(QEvent *)
{
    if (toolTipEnabled)
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
        return;
    }

    if (compareView) {
        QPixmap in(inpath);
        if (in.height() > height() || in.width() > width()/2)
            in = in.scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        int heightBorder = (height()-in.height())/2;
        painter.drawPixmap(width()/4-in.width()/2,heightBorder,in.width(),in.height(),in);
        QPixmap out(outpath);
        if (out.height() > height() || out.width() > width()/2)
            out = out.scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        painter.drawPixmap(width()/2+width()/4-in.width()/2,heightBorder,out.width(),out.height(),out);
    } else {
        QPixmap out(outpath);
        if (out.height() > height() || out.width() > width())
            out = out.scaled(height(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        int heightBorder = (height()-out.height())/2;
        painter.drawPixmap(width()/2-out.width()/2,heightBorder,out.width(),out.height(),out);
    }
}

void IconsWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || crashed)
        return;

    if (event->x() < width()/2)
        QDesktopServices::openUrl(QUrl(inpath));
    else
        QDesktopServices::openUrl(QUrl(outpath));
}

// temp fix
void IconsWidget::wheelEvent(QWheelEvent *e)
{
    if (toolTipEnabled)
        toolTip->hide();
    QWidget::wheelEvent(e);
}
