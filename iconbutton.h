#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>
#include <QLabel>

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    explicit IconButton(QWidget *parent = 0);
    void setPath(const QString &path);

private:
    QLabel *diag;
    bool flag;
    
private slots:
    void showToolTip();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    
};

#endif // ICONBUTTON_H
