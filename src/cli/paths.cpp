#include <QtCore/QStringBuilder>

#include <cmath>

#include "tools.h"
#include "paths.h"
#include "keys.h"

// http://www.w3.org/TR/SVG/paths.html

// TODO: add path approximation
// fjudy2001_02_cleaned
// warszawianka_Betel.svg

Segment::Segment()
{
    m_isApplyRound = !Keys::get().flag(Key::SkipRoundingNumbers);
}

QString Segment::string(qreal value) const
{
    return (m_isApplyRound) ? Tools::roundNumber(value) : QString::number(value);
}

void Segment::setTransform(const QString &text)
{
    Transform tr(text);
    if (command == Command::MoveTo || command == Command::LineTo || command == Command::SmoothQuadratic) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();
    } else if (command == Command::CurveTo) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x1, y1);
        x1 = tr.newX();
        y1 = tr.newY();

        tr.setOldXY(x2, y2);
        x2 = tr.newX();
        y2 = tr.newY();
    } else if (command == Command::SmoothCurveTo) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x2, y2);
        x2 = tr.newX();
        y2 = tr.newY();
    } else if (command == Command::Quadratic) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x1, y1);
        x1 = tr.newX();
        y1 = tr.newY();
    }
}

void Segment::toRelative(qreal xLast, qreal yLast)
{
    if (!absolute)
        return;

    if (command == Command::HorizontalLineTo) {
        x = x - xLast;
    } else if (command == Command::VerticalLineTo) {
        y = y - yLast;
    } else {
        x = x - xLast;
        y = y - yLast;
        if (command == Command::CurveTo) {
            x1 = x1 - xLast;
            y1 = y1 - yLast;
            x2 = x2 - xLast;
            y2 = y2 - yLast;
        } else if (command == Command::SmoothCurveTo) {
            x2 = x2 - xLast;
            y2 = y2 - yLast;
        } else if (command == Command::Quadratic) {
            x1 = x1 - xLast;
            y1 = y1 - yLast;
        }
    }
    absolute = false;
}

void Segment::toAbsolute(qreal xLast, qreal yLast)
{
    if (absolute)
        return;

    if (command == Command::HorizontalLineTo) {
        x = x + xLast;
    } else if (command == Command::VerticalLineTo) {
        y = y + yLast;
    } else {
        x = x + xLast;
        y = y + yLast;
        if (command == Command::CurveTo) {
            x1 = x1 + xLast;
            y1 = y1 + yLast;
            x2 = x2 + xLast;
            y2 = y2 + yLast;
        } else if (command == Command::SmoothCurveTo) {
            x2 = x2 + xLast;
            y2 = y2 + yLast;
        } else if (command == Command::Quadratic) {
            x1 = x1 + xLast;
            y1 = y1 + yLast;
        }
    }
    absolute = true;
}

QString Segment::toString() const
{
    QString out;
    if (command == Command::MoveTo || command == Command::LineTo || command == Command::SmoothQuadratic) {
        out = Tools::roundNumber(x) % "," % Tools::roundNumber(y);
    } else if (command == Command::CurveTo) {
        out =   string(x1) % "," % string(y1) % " "
              % string(x2) % "," % string(y2) % " "
              % string(x)  % "," % string(y);
    } else if (command == Command::HorizontalLineTo) {
        out = string(x);
    } else if (command == Command::VerticalLineTo) {
        out = string(y);
    } else if (command == Command::SmoothCurveTo) {
        out =   string(x2) % "," % string(y2) % " "
              % string(x)  % "," % string(y);
    } else if (command == Command::Quadratic) {
        out =   string(x1) % "," % string(y1) % " "
              % string(x)  % "," % string(y);
    } else if (command == Command::EllipticalArc) {
        out =   string(rx) % "," % string(ry) % " "
                % QString::number(xAxisRotation) % " "
                % QString::number(largeArc) % ","
                % QString::number(sweep) % " "
                % string(x)  % "," % string(y);
    }
    return out;
}

// New class

void Path::processPath(SvgElement elem)
{
    const QString inPath = elem.attribute("d");

    if (inPath.contains("nan")) {
        elem.removeAttribute("d");
        return;
    }

    // path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are identical
    // can't be converted to relative coordinates
    // some kind of bug
    if (inPath.contains("A")) {
        if (QString(inPath).remove(QRegExp("[^a-AZ-z]")) == "MA") {
            return;
        }
    }

    splitToSegments(inPath);

    segmentsToAbsolute();
    segmentsToRelative();

    processSegments();

    // merge segments to path
    QString outPath = segmentsToPath();

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        elem.setAttribute("d", inPath);
    else
        elem.setAttribute("d", outPath);
}

void Path::setSegments(const QList<Segment> &list)
{
    m_segmentList = list;
}

void Path::splitToSegments(const QString &path)
{
    QStringList data;
    QString buf;
    QChar prevNonDigit;
    // split path to commands and points
    for (int i = 0; i < path.size(); ++i) {
        QChar c = path.at(i);
        if (c.isLetter() && c.toLower() != 'e') {
            if (!buf.isEmpty())
                data << buf;
            data << c;
            buf.clear();
        } else if (c == ',' || c == ' ') {
            if (!buf.isEmpty())
                data << buf;
            buf.clear();
        } else if (i == path.size()-1) {
            buf += c;
            if (!buf.isEmpty())
                data << buf;
            buf.clear();
        } else {
            if (c.isNumber() || (c == '.' && prevNonDigit != '.') || c == 'e') {
                buf += c;
            } else if (c == '-' || c == '.') {
                if (path.at(i-1).isNumber()) {
                    if (!buf.isEmpty())
                        data << buf;
                    buf.clear();
                    buf += c;
                } else {
                    buf += c;
                }
            }
        }
        if (!c.isDigit())
            prevNonDigit = c;
    }

    // convert to segments
    QString currCmd;
    while (!data.isEmpty()) {
        bool isNewCmd = false;
        if (data.first().at(0).isLetter()) {
            currCmd = data.takeFirst();
            isNewCmd = true;
        }

        QString lowerCmd = currCmd.toLower();
        Segment segment;

        segment.command = lowerCmd;
        segment.absolute = currCmd.at(0).isUpper();
        segment.srcCmd = isNewCmd;

        if (   lowerCmd == Command::MoveTo
            || lowerCmd == Command::LineTo
            || lowerCmd == Command::SmoothQuadratic) {
            while (data.size() < 2)
                data << "0";
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::HorizontalLineTo) {
            while (data.size() < 1)
                data << "0";
            segment.x  = data.takeFirst().toDouble();
            segment.y  = 0;
        } else if (lowerCmd == Command::VerticalLineTo) {
            while (data.size() < 1)
                data << "0";
            segment.x  = 0;
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::CurveTo) {
            while (data.size() < 6)
                data << "0";
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::SmoothCurveTo) {
            while (data.size() < 4)
                data << "0";
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::Quadratic) {
            while (data.size() < 4)
                data << "0";
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::EllipticalArc) {
            while (data.size() < 7)
                data << "0";
            segment.rx = data.takeFirst().toDouble();
            segment.ry = data.takeFirst().toDouble();
            segment.xAxisRotation = data.takeFirst().toDouble();
            segment.largeArc      = data.takeFirst().toDouble();
            segment.sweep         = data.takeFirst().toDouble();
            segment.x = data.takeFirst().toDouble();
            segment.y = data.takeFirst().toDouble();
        }

        m_segmentList.append(segment);
    }
}

// TODO: if last point equal to first - convert segment into z
void Path::processSegments()
{
    // TODO: add other commands conv

    // convert "lineto" to "horizontal/vertical lineto" equivalent
    if (!Keys::get().flag(Key::KeepLinesTo)) {
        for (int i = 0; i < m_segmentList.count(); ++i) {
            Segment seg = m_segmentList.at(i);
            QString currCmd = seg.command;
            if (currCmd == Command::LineTo) {
                if (seg.x == 0 && seg.y != 0) {
                    seg.command = Command::VerticalLineTo;
                    m_segmentList.replace(i, seg);
                } else if (seg.x != 0 && seg.y == 0) {
                    seg.command = Command::HorizontalLineTo;
                    m_segmentList.replace(i, seg);
                }
            }
        }
    }

    if (!Keys::get().flag(Key::KeepEmptySegments)) {
        for (int i = 0; i < m_segmentList.count(); ++i) {
            Segment seg = m_segmentList.at(i);
            const QString cmd = seg.command;
            // remove empty segments 'm0,0' (except first)
            if (cmd == Command::MoveTo && i > 0) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::CurveTo) {
                if (   (seg.x1 == 0 && seg.y1 == 0)
                    && (seg.x2 == 0 && seg.y2 == 0)
                    && (seg.x  == 0 && seg.y  == 0))
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::HorizontalLineTo) {
                if (seg.x == 0)
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::VerticalLineTo) {
                if (seg.y == 0)
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::SmoothCurveTo) {
                if (   (seg.x2 == 0 && seg.y2 == 0)
                    && (seg.x  == 0 && seg.y  == 0))
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::Quadratic) {
                if (   seg.x1 == 0 && seg.y1 == 0
                    && seg.x  == 0 && seg.y  == 0)
                    m_segmentList.removeAt(i);
            }
            else if (cmd == Command::EllipticalArc) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeAt(i);
            }
        }
    }
}

// NOTE: didn't work without relative conv
void Path::segmentsToAbsolute()
{
    qreal lastX = 0;
    qreal lastY = 0;
    for (int i = 0; i < m_segmentList.count(); ++i) {
        Segment segment = m_segmentList.at(i);
        if (segment.command != Command::ClosePath) {
            if (segment.absolute) {
                qreal x = segment.x;
                qreal y = segment.y;
                if (x == 0 && segment.command == Command::VerticalLineTo)
                    x = lastX;
                else if (y == 0 && segment.command == Command::HorizontalLineTo)
                    y = lastY;
                lastX = x;
                lastY = y;
            } else {
                m_segmentList[i].toAbsolute(lastX, lastY);
                lastX += segment.x;
                lastY += segment.y;
            }
        }
    }
}

void Path::segmentsToRelative()
{
    qreal lastX = 0;
    qreal lastY = 0;
    for (int i = 0; i < m_segmentList.count(); ++i) {
        Segment segment = m_segmentList.at(i);
        if (segment.command != Command::ClosePath) {
            if (segment.absolute) {
                m_segmentList[i].toRelative(lastX, lastY);
                segment = m_segmentList.at(i);
                lastX += segment.x;
                lastY += segment.y;
            } else {
                lastX = segment.x;
                lastY = segment.y;
            }
        }
    }
}

QString Path::segmentsToPath()
{
    QString outPath;
    QString prevCom;
    bool isPrevComAbs = false;
    for (int i = 0; i < m_segmentList.count(); ++i) {
        Segment segment = m_segmentList.at(i);
        QString currCmd = segment.command;
        // check is previous command is the same as next
        bool writeCmd = true;
        if (currCmd == prevCom && !prevCom.isEmpty()) {
            if (segment.absolute == isPrevComAbs
                && !(segment.srcCmd && segment.command == Command::MoveTo))
                writeCmd = false;
        }
        prevCom = currCmd;
        isPrevComAbs = segment.absolute;

        if (writeCmd) {
            if (segment.absolute)
                outPath += segment.command.toUpper() % " ";
            else
                outPath += segment.command % " ";
        }
        outPath += m_segmentList.at(i).toString() % " ";
    }
    outPath.chop(1);

    if (!Keys::get().flag(Key::KeepUnusedSymbolsFromPath))
        outPath = trimPath(outPath);

    return outPath;
}

// remove unneeded spaces and comas
// m 30    to  m30
// 10,-30  to  10-30
// 15,.5   to  15.5
QString Path::trimPath(const QString &path)
{
    QString outPath;
    bool isArc = false;
    for (int i = 0; i < path.size(); ++i) {
        QChar curr = path.at(i);
        if (curr == ',' || curr == ' ') {
            if (i < path.size()-1 && i > 0) {
                QChar prev = path.at(i-1);
                QChar next = path.at(i+1);

                bool keep = true;
                // example: m 100 -> m100
                if (prev.isLetter() && curr == ' ')
                    keep = false;
                // example: 100 c -> 100c
                if (curr == ' ' && next.isLetter())
                    keep = false;
                // example: 42,-42 -> 42-42
                if (curr == ',' && next == '-')
                    keep = false;
                // example: 42 -42 -> 42-42
                if (prev.isNumber() && curr == ' ' && next == '-' && !isArc)
                    keep = false;

                if (keep)
                    outPath += curr;                 
            }
        } else {
            if (curr.toLower() == 'a')
                isArc = true;
            else if (curr.isLetter())
                isArc = false;
            outPath += curr;
        }
    }
    return outPath;
}
