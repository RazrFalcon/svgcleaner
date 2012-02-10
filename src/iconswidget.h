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
    void setCrashed(bool flag);

private:
    QString inpath;
    QString outpath;
    QLabel *toolTip;
    bool crashed;
    bool compareView;
    bool newToolTip;
    QPixmap mainPix;
    void makeToolTip();
    void renderSvg(const QString path, QPainter *painter, QRect rect);
    
private slots:
    void showToolTip();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void paintEvent(QPaintEvent *);
};

#endif // ICONSWIDGET_H
