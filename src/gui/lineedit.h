#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QStyleOptionFrameV2>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = 0);
    void showLoading(bool value);
    ~LineEdit();

private:
    QLabel *lbl;
    int angle;
    int timerId;
    QRect progressRect;
    void initStyleOption(QStyleOptionFrameV2 *option) const;
    QColor textColor;

public slots:
    void setValue(const int &value);

protected:
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);

};

#endif // LINEEDIT_H
