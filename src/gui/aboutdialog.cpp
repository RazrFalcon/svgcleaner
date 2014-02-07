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

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    lblTitle->setText("SVG Cleaner " + QApplication::applicationVersion());
    tabWidget->setCurrentIndex(0); // always show first tab
    fillAbout();
    fillAuthors();
}

void AboutDialog::fillAbout()
{
    textAbout->append(tr("Generally, SVG files produced by vector editors contain a lot of unused "
                      "elements and attributes that just blow up their size without providing "
                      "better visible quality.") + "<br />");

    textAbout->append(tr("SVG Cleaner could help you to clean up your SVG files from unnecessary data. "
                      "It has a lot of options for cleanup and optimization, works in batch mode, "
                      "provides threaded processing on the multicore processors and basically does "
                      "two things:<br />"
                      "- removing elements and attributes that don't contribute to "
                      "the final rendering;<br />"
                      "- making those elements and attributes in use more compact.") + "<br />");

    textAbout->append(tr("Images cleaned by SVG Cleaner are typically 40-60 percent smaller "
                      "than the original ones.") + "<br />");

    textAbout->append(tr("Important! The internal image viewer in SVG Cleaner uses the QtSvg module "
                      "for rendering SVG images. Qt supports only the static features of "
                      "SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of "
                      "advanced features. For instance, elements such as clipPath, mask, "
                      "filters etc. will not be rendered at all.") + "<br />");

    textAbout->append(tr("Please send bug reports to ")
                      + genLink("svgcleanerteam@gmail.com") + ".");
}

void AboutDialog::fillAuthors()
{
    textAuthors->append(tr("Developers:"));
    textAuthors->append("Evgeniy Reizner " + genLink("razrfalcon@gmail.com"));
    textAuthors->append("");
    textAuthors->append(tr("Previous developers:"));
    textAuthors->append("Andrey Bayrak " + genLink("andrey.bayrak@gmail.com"));
    textAuthors->append("");
    textAuthors->append(tr("Special thanks:"));
    textAuthors->append("Alexandre Prokoudine " + genLink("alexandre.prokoudine@gmail.com"));
    textAuthors->append("");
    textAuthors->append(tr("Logo design:"));
    textAuthors->append("Ivan Kacenkov " + genLink("kacenkov7@gmail.com"));
    textAuthors->append("");
    textAuthors->append(tr("Translators:"));
    textAuthors->append("Czech - Pavel Fric " + genLink("pavelfric@seznam.cz"));
    textAuthors->append("German - Seb Adler " + genLink("seb.adler.bln@gmail.com"));
    textAuthors->append("Italian - Daniele Pantaleo " + genLink("ozone89.dp@gmail.com"));
    textAuthors->append("French - Sêyivê F. PADONOU " + genLink("promelduis@gmail.com"));
}

QString AboutDialog::genLink(const QString &link)
{
    return "<a href=\"" + link + "\">"
           "<span style=\" text-decoration: underline; color:#0057ae;\">"
           + link + "</span></a>";
}
