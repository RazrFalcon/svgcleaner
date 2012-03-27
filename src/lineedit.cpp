#include <QtGui/QPainter>

#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    this->setObjectName("path");

    angle = 0;
    timerId = -1;

    lbl = new QLabel(this);
    lbl->setObjectName("files");
    lbl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    lbl->setStyleSheet("QLabel#files {border: none; padding: 4px; padding-left: 1px;"
                       "background-color: rgba(255, 255, 255, 0); color: #808080;}");
}

void LineEdit::initStyleOption(QStyleOptionFrameV2 *option) const
{
    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;
    if (this->isReadOnly())
        option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
    if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
    option->features = QStyleOptionFrameV2::None;
}

LineEdit::~LineEdit()
{
    delete lbl;
}

void LineEdit::setValue(const int &value)
{
    lbl->setText(QString(tr("%1 files")).arg(value));
    QFontMetrics fm(QFont().defaultFamily());
    // FIXME: make padding only for text
    setStyleSheet(QString("padding-left: %1px;").arg(fm.boundingRect(lbl->text()).width()));
    lbl->setFixedWidth(fm.boundingRect(lbl->text()).width()+8);
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
    angle = (angle+30)%360;
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
        QColor color(Qt::black);
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().left()+width/2+3,rect().center().y());
        p.rotate(angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth,
                          capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}
