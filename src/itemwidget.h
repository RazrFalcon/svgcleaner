#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(const QString &icon, QWidget *parent = 0);
    ~ItemWidget();

private:
    QLabel *lblIcon;
    QVBoxLayout *layout;
};

#endif // ITEMWIDGET_H
