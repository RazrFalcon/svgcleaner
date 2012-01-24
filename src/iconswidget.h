#ifndef ICONSWIDGET_H
#define ICONSWIDGET_H

#include <QWidget>
#include <QLabel>

class IconsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconsWidget(QWidget *parent = 0);
    void setPaths(const QString &pathIn, const QString &pathOut, const bool compare);
    void crashed();

private:
    QString inpath;
    QString outpath;
    QLabel *diag;
    bool isCrashed;
    QPixmap mainPix;
    
private slots:
    void showToolTip();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
};

#endif // ICONSWIDGET_H
