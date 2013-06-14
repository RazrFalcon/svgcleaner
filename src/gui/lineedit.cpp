#include <QtGui/QPainter>

#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    this->setObjectName("path");

    angle = 0;
    timerId = -1;
    textColor = QPalette().color(QPalette::WindowText).darker();

    lbl = new QLabel(this);
    lbl->setObjectName("files");
    lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    int padding = 4;
#ifdef Q_OS_WIN
    padding = 3;
#endif
    lbl->setStyleSheet(QString("QLabel#files {border: none; padding-top: %1px; padding-left: 1px;"
                               "background-color: rgba(0, 0, 0, 0); color: %2;}")
                               .arg(padding).arg(textColor.name()));
}

void LineEdit::initStyleOption(QStyleOptionFrameV2 *option) const
{
    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;
    option->features = QStyleOptionFrameV2::None;
}

LineEdit::~LineEdit()
{
    delete lbl;
}

void LineEdit::setValue(const int &value)
{
    lbl->setText(QString(tr("%1 files")).arg(value));
    QFont font;
    QFontMetrics fm(font);
    // FIXME: make padding only for text
    int w = fm.width(lbl->text() + "0");
    setStyleSheet(QString("padding-left: %1px;").arg(w));
    lbl->setFixedWidth(w);
}

void LineEdit::showLoading(bool value)
{
    if (value) {
        angle = 0;
        if (timerId == -1)
            timerId = startTimer(100);
        timerId = 0;
        setStyleSheet(QString("padding-left: %1px;").arg(progressRect.height()));
    } else {
        if (timerId != -1)
            killTimer(timerId);
        timerId = -1;
        update();
    }

    lbl->setVisible(!value);
}

void LineEdit::resizeEvent(QResizeEvent *)
{
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    progressRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    if (timerId != -1)
        setStyleSheet(QString("padding-left: %1px;").arg(progressRect.height()));
}

void LineEdit::timerEvent(QTimerEvent *)
{
    angle = (angle + 30) % 360;
    update();
}

// code from QProgressIndicator
void LineEdit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);

    if (timerId == -1)
        return;

    int width = progressRect.height();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int outerRadius = (width-1)*0.5;
    int innerRadius = (width-1)*0.5*0.38;

    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    int capsuleRadius = capsuleWidth/2;

    for (int i = 0; i < 12; ++i) {
        QColor color(textColor);
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().left()+width/2+3,rect().center().y());
        p.rotate(angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight),  capsuleWidth,
                          capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}
