#ifndef ICONSWIDGET_H
#define ICONSWIDGET_H

#include <QWidget>
#include <QLabel>

class IconsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconsWidget(QWidget *parent = 0);
    void setCrashed(bool flag);
    void setPaths(const QString &pathIn, const QString &pathOut, const bool compare);

private:
    bool compareView;
    bool crashed;
    bool newToolTip;
    QLabel *toolTip;
    QPixmap mainPix;
    QString inpath;
    QString outpath;
    void makeToolTip();
    void renderSvg(const QString path, QPainter *painter, QRect rect);
    
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
