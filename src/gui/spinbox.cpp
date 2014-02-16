/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include <QHBoxLayout>

#include "spinbox.h"

SpinBox::SpinBox(bool isDouble, QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    m_lbl = new QLabel(this);
    m_lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay->addWidget(m_lbl);
    if (isDouble) {
        m_spin = 0;
        m_doubleSpin = new QDoubleSpinBox(this);
        m_doubleSpin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        lay->addWidget(m_doubleSpin);
    } else {
        m_spin = new QSpinBox(this);
        m_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_doubleSpin = 0;
        lay->addWidget(m_spin);
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
