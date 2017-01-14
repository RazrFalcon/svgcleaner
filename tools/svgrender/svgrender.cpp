/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Graphics Dojo project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "svgrender.h"

#include <iostream>

#include <QtGui>

// large enough to enclose the SVG icons
#define MAGIC_SIZE 4095

class WebPage : public QWebPage
{
public:
    WebPage(QObject *parent = Q_NULLPTR) : QWebPage(parent) {}

protected:
    void javaScriptAlert(QWebFrame *, const QString &msg)
    {
        qDebug() << msg;
    }
};

WebKitSVGRenderer::WebKitSVGRenderer(QWidget *parent) : QWebView(parent)
{
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(saveResult(bool)));
    setFixedSize(MAGIC_SIZE, MAGIC_SIZE);

    WebPage *page = new WebPage(this);
    setPage(page);

    setStyleSheet("background:transparent");
    setAttribute(Qt::WA_TranslucentBackground);
}

#define EVAL_JS(x) page()->mainFrame()->evaluateJavaScript((x))

void WebKitSVGRenderer::saveResult(bool ok)
{
    // crude error-checking
    if (!ok) {
        std::cerr << "Failed loading " << qPrintable(url().toString()) << std::endl;
        QApplication::instance()->exit(1);
        return;
    }

    // ensure it is an SVG
    QString root = EVAL_JS("document.rootElement.nodeName").toString();
    if (root.isEmpty() || root.compare("svg", Qt::CaseInsensitive)) {
        std::cerr << "Not an SVG! " << qPrintable(url().toString()) << std::endl;
        close();
        return;
    }

    // get the dimension, i.e. the width and height attributes
    // Note: if an attribute is not defined WebKit would return the view's dimension
    // hence the hack of checking for the MAGIC_SIZE
    double ww = EVAL_JS("document.rootElement.width.baseVal.value").toDouble();
    double hh = EVAL_JS("document.rootElement.height.baseVal.value").toDouble();
    if (ww == 0.0 || hh == 0.0 || ww == MAGIC_SIZE || hh == MAGIC_SIZE) {
        ww = hh = MAGIC_SIZE;
    }

    // try to give the best output file
    if (fileName.isEmpty()) {
        fileName = QFileInfo(url().path()).completeBaseName();
        if (fileName.isEmpty())
            fileName = "result";
        fileName += ".png";
    }

    // create the target surface
    QImage img(targetSize, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    // prepare the painter
    QPainter p(&img);
    qreal xs = targetSize.width() / ww;
    // center image
    p.translate((targetSize.width() - ww * xs) / 2, (targetSize.height() - hh * xs) / 2);
    // scale image to target size
    p.scale(xs, xs);

    // the best quality
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    page()->mainFrame()->render(&p);
    p.end();

    if (img.save(fileName, "png"))
        std::cout << "Result saved to " << qPrintable(fileName) << std::endl;
    else
        std::cout << "Failed to save to " << qPrintable(fileName) << std::endl;

    close();
}

static void usage()
{
    std::cout << "Rasterize an SVG icon to a PNG image" << std::endl << std::endl;
    std::cout << "  svgrender input.svg output.png width" << std::endl << std::endl;
    std::cout << "Examples: " << std::endl;
    std::cout << "  svgrender icon.svg icon.png 256" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char * argv[])
{
    if (argc != 4) {
        usage();
        return 0;
    }

    QApplication a(argc, argv);

    WebKitSVGRenderer renderer;
    renderer.fileName = QString::fromLatin1(argv[2]);

    int w = QString::fromLatin1(argv[3]).toInt();
    renderer.targetSize = QSize(w, w);
    if (!renderer.targetSize.isValid() || w <= 0) {
        std::cerr << "Please specify a valid target size!" << std::endl;
        return 0;
    }

    QString str = QFileInfo(QString::fromLatin1(argv[1])).absoluteFilePath();
    renderer.load(QUrl::fromLocalFile(str));

    return a.exec();
}

