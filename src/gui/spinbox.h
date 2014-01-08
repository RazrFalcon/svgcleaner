#ifndef SPINBOX_H
#define SPINBOX_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

class SpinBox : public QWidget
{
    Q_OBJECT

public:
    explicit SpinBox(bool isDouble, QWidget *parent = 0);
    void setText(const QString &text);
    void setValue(const qreal value);
    qreal value();
    void setRange(const qreal min, const qreal max);
    void setStep(const qreal value);

private:
    QHBoxLayout *m_lay;
    QLabel *m_lbl;
    QSpinBox *m_spin;
    QDoubleSpinBox *m_doubleSpin;
};

#endif // SPINBOX_H
