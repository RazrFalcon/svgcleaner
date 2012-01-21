#include <QTimer>
#include <QVBoxLayout>
#include <QCursor>
#include <QPainter>
#include <QtDebug>

#include "iconbutton.h"

IconButton::IconButton(QWidget *parent) :
    QPushButton(parent)
{
    diag = new QLabel();
    diag->setWindowFlags(Qt::ToolTip);
}

void IconButton::setPath(const QString &path)
{
    QPixmap p(path);
    QPixmap pixmap(path);
    QPainter painter(&pixmap);
    painter.drawPixmap(pixmap.width(),0,p.width(),p.height(),p);
//    pixmap.save("test.png");

    diag->setPixmap(pixmap.scaled(190,190,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void IconButton::enterEvent(QEvent *)
{
    QTimer::singleShot(1000,this,SLOT(showToolTip()));
//    flag = true;
    qDebug()<<"enter";
}

void IconButton::leaveEvent(QEvent *)
{
    diag->hide();
//    flag = false;
    qDebug()<<"leave";
}

void IconButton::showToolTip()
{
//    if (diag->isVisible())
//        return;

    QCursor curs;
    QPoint cursPos = mapFromGlobal(curs.pos());
    qDebug()<<cursPos;
    if (cursPos.x() > 0 && cursPos.y() > 0 && cursPos.y() < height() && cursPos.x() < width()) {
        QPoint p = mapToGlobal(QPoint(0,0));
        diag->setGeometry(p.x(),p.y()-200,200,200);
        diag->show();
    }
}
