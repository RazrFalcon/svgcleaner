#include "spinbox.h"

SpinBox::SpinBox(bool isDouble, QWidget *parent) :
    QWidget(parent)
{
    m_lay = new QHBoxLayout(this);
    m_lay->setContentsMargins(0,0,0,0);

    m_lbl = new QLabel(this);
    m_lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_lay->addWidget(m_lbl);
    if (isDouble) {
        m_spin = 0;
        m_doubleSpin = new QDoubleSpinBox(this);
        m_doubleSpin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_lay->addWidget(m_doubleSpin);
    } else {
        m_spin = new QSpinBox(this);
        m_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_doubleSpin = 0;
        m_lay->addWidget(m_spin);
    }
}

void SpinBox::setText(const QString &text)
{
    m_lbl->setText(text);
}

void SpinBox::setValue(const qreal value)
{
    if (m_spin)
        m_spin->setValue(value);
    else
        m_doubleSpin->setValue(value);
}

qreal SpinBox::value()
{
    if (m_spin)
        return m_spin->value();
    else
        return m_doubleSpin->value();
}

void SpinBox::setRange(const qreal min, const qreal max)
{
    if (m_spin) {
        m_spin->setMinimum(min);
        m_spin->setMaximum(max);
    } else {
        m_doubleSpin->setMinimum(min);
        m_doubleSpin->setMaximum(max);
    }
}

void SpinBox::setStep(const qreal value)
{
    if (m_spin)
        m_spin->setSingleStep(value);
    else
        m_doubleSpin->setSingleStep(value);
}
