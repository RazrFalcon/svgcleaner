#ifndef ICONSWIDGET_H
#define ICONSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>

class IconsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconsWidget(QWidget *parent = 0);
    void setError(const QString &text);
    void setPaths(const QString &pathIn, const QString &pathOut, const bool compare);

private:
    bool compareView;
    bool crashed;
    bool newToolTip;
    QLabel *toolTip;
    QPixmap mainPix;
    QString inpath;
    QString outpath;
    QString errText;
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
    void focusOutEvent(QFocusEvent *);
};

#endif // ICONSWIDGET_H
