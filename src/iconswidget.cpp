#include <QBitmap>
#include <QCursor>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <QtDebug>
#include <QTimer>
#include <QUrl>

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
    compareView = compare;
    newToolTip = true;
    if (toolTip->isVisible())
        makeToolTip();

    if (compare)
        setFixedWidth(205);
    else
        setFixedWidth(100);

    crashed = false;
    repaint();
}

void IconsWidget::setError(const QString &text)
{
    crashed = true;
    errText = text;
    repaint();
}

void IconsWidget::makeToolTip()
{
    QImage image(620,310,QImage::Format_ARGB32);
    image.fill(0);
    mainPix = QPixmap::fromImage(image,Qt::NoOpaqueDetection|Qt::AutoColor);
    QPainter painter(&mainPix);
    QPalette pal;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setBrush(QBrush(pal.color(QPalette::Background)));
    painter.setPen(QPen(QColor("#636363"),2));
    painter.drawRoundedRect(2,2,image.width()-4,image.height()-4,5,5);

    renderSvg(inpath,&painter,QRect(5,5,300,300));
    renderSvg(outpath,&painter,QRect(310,5,300,300));

    toolTip->setPixmap(mainPix);
    toolTip->setMask(mainPix.mask());
    newToolTip = false;
}

void IconsWidget::enterEvent(QEvent *)
{
    QTimer::singleShot(500,this,SLOT(showToolTip()));
}

void IconsWidget::showToolTip()
{
    if (crashed)
        return;

    if (newToolTip)
        makeToolTip();

    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (rect().contains(cursPos)) {
        toolTip->show();
        this->setFocus();
    }
}

void IconsWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (crashed)
        return;

    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());

    if (rect().contains(cursPos)) {
        int border = 5;
        QPoint p = mapToGlobal(event->pos());
        QPoint p2 = mapToGlobal(QPoint(0,0));
        if (p2.y()-mainPix.height()-6 > 0) {
            toolTip->setGeometry(p.x()-mainPix.width()/2,p2.y()-mainPix.height()-6,
                                 mainPix.width(),mainPix.height());
        } else {
            toolTip->setGeometry(p.x()-mainPix.width()/2,p2.y()+height()+border,
                                  mainPix.width(),mainPix.height());
        }
    }
}

void IconsWidget::leaveEvent(QEvent *)
{
    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    if (!rect().contains(cursPos))
        toolTip->hide();
}

void IconsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (crashed) {
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(),Qt::AlignCenter|Qt::TextWordWrap,errText);
        return;
    }

    if (compareView) {
        renderSvg(inpath, &painter,QRect(0,0,height(),height()));
        renderSvg(outpath,&painter,QRect(105,0,height(),height()));
    } else {
        renderSvg(outpath,&painter,QRect(0,0,width(),height()));
    }
}

void IconsWidget::renderSvg(const QString path, QPainter *painter, QRect rect)
{
    QSvgRenderer renderer(path);
    QSize size = renderer.viewBox().size();
    size.scale(rect.width(),rect.height(),Qt::KeepAspectRatio);
    int border = 3;
    renderer.render(painter,QRect(rect.x()+border,(rect.height()-size.height())/2+rect.y()+border,
                                  size.width()-border,size.height()-border));
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

void IconsWidget::focusOutEvent(QFocusEvent *)
{
//    toolTip->hide();
}
