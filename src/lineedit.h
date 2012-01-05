#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QLabel>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = 0);
    ~LineEdit();

private:
    QLabel *lbl;

public slots:
    void setValue(const int &value);
};

#endif // LINEEDIT_H
