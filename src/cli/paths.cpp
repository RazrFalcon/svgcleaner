#include <QtCore/QStringBuilder>

#include "tools.h"
#include "paths.h"
#include "keys.h"

// http://www.w3.org/TR/SVG/paths.html

SegmentList::SegmentList()
{
    m_position = -1;
    m_isRound = !Keys::get().flag(Key::SkipRoundingNumbers);
}

void SegmentList::append(Segment data)
{
    m_data << data;
}

bool SegmentList::next()
{
    if (m_data.count()-1 > m_position) {
        m_position++;
        return true;
    } else {
        m_position = -1;
        return false;
    }
}

void SegmentList::restart()
{
    m_position = -1;
}

void SegmentList::removeCurrent()
{
    m_data.removeAt(m_position);
    m_position--;
}

int SegmentList::position()
{
    return m_position;
}

QString SegmentList::string(qreal value)
{
    QString str;
    if (m_isRound)
        str = Tools::roundNumber(value);
    else
        str = QString::number(value);
    return str;
}

void SegmentList::appendLastPoint(qreal x, qreal y)
{
    m_lastPoint.setX(m_lastPoint.x() + x);
    m_lastPoint.setY(m_lastPoint.y() + y);
}

QPointF SegmentList::lastPoint()
{
    return m_lastPoint;
}

void SegmentList::setLastPoint(qreal x, qreal y)
{
    m_lastPoint = QPointF(x, y);
}

//void SegmentList::setTransform(const QString &text)
//{
//    Transform tr(text);
//    QString cmd = command();
//    // TODO: how to transform h/v
//    if (cmd != Command::HorizontalLineTo
//        && cmd != Command::VerticalLineTo && cmd != Command::ClosePath) {

//        if (m_data.at(m_position).contains(SegmentList::X)) {
//            tr.setXY(number(SegmentList::X), number(SegmentList::Y));
//            setValue(SegmentList::X, tr.newX());
//            setValue(SegmentList::Y, tr.newY());
//        }

//        if (m_data.at(m_position).contains(SegmentList::X1)) {
//            tr.setXY(number(SegmentList::X1), number(SegmentList::Y1));
//            setValue(SegmentList::X1, tr.newX());
//            setValue(SegmentList::Y1, tr.newY());
//        }

//        if (m_data.at(m_position).contains(SegmentList::X2)) {
//            tr.setXY(number(SegmentList::X2), number(SegmentList::Y2));
//            setValue(SegmentList::X2, tr.newX());
//            setValue(SegmentList::Y2, tr.newY());
//        }
//    }
//}

void SegmentList::toRelative()
{
    Segment currSegment = segment();
    if (!currSegment.abs)
        return;

    // TODO: refract
    QString cmd = currSegment.command;
    if (cmd == Command::MoveTo || cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
        currSegment.x = currSegment.x - m_lastPoint.x();
        currSegment.y = currSegment.y - m_lastPoint.y();
    } else if (cmd == Command::CurveTo) {
        currSegment.x = currSegment.x - m_lastPoint.x();
        currSegment.y = currSegment.y - m_lastPoint.y();

        currSegment.x1 = currSegment.x1 - m_lastPoint.x();
        currSegment.y1 = currSegment.y1 - m_lastPoint.y();

        currSegment.x2 = currSegment.x2 - m_lastPoint.x();
        currSegment.y2 = currSegment.y2 - m_lastPoint.y();
    } else if (cmd == Command::SmoothCurveTo) {
        currSegment.x = currSegment.x - m_lastPoint.x();
        currSegment.y = currSegment.y - m_lastPoint.y();

        currSegment.x2 = currSegment.x2 - m_lastPoint.x();
        currSegment.y2 = currSegment.y2 - m_lastPoint.y();
    } else if (cmd == Command::Quadratic) {
        currSegment.x = currSegment.x - m_lastPoint.x();
        currSegment.y = currSegment.y - m_lastPoint.y();

        currSegment.x1 = currSegment.x1 - m_lastPoint.x();
        currSegment.y1 = currSegment.y1 - m_lastPoint.y();
    } else if (cmd == Command::HorizontalLineTo) {
        currSegment.x = currSegment.x - m_lastPoint.x();
    } else if (cmd == Command::VerticalLineTo) {
        currSegment.y = currSegment.y - m_lastPoint.y();
    } else if (cmd == Command::EllipticalArc) {
        currSegment.x = currSegment.x - m_lastPoint.x();
        currSegment.y = currSegment.y - m_lastPoint.y();
    }

    currSegment.abs = false;
    updateSegment(currSegment);
}

void SegmentList::toAbsolute()
{
    Segment currSegment = segment();
    if (currSegment.abs)
        return;

    QString cmd = currSegment.command;
    if (cmd == Command::MoveTo || cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
        currSegment.x = currSegment.x + m_lastPoint.x();
        currSegment.y = currSegment.y + m_lastPoint.y();
    } else if (cmd == Command::CurveTo) {
        currSegment.x = currSegment.x + m_lastPoint.x();
        currSegment.y = currSegment.y + m_lastPoint.y();

        currSegment.x1 = currSegment.x1 + m_lastPoint.x();
        currSegment.y1 = currSegment.y1 + m_lastPoint.y();

        currSegment.x2 = currSegment.x2 + m_lastPoint.x();
        currSegment.y2 = currSegment.y2 + m_lastPoint.y();
    } else if (cmd == Command::SmoothCurveTo) {
        currSegment.x = currSegment.x + m_lastPoint.x();
        currSegment.y = currSegment.y + m_lastPoint.y();

        currSegment.x2 = currSegment.x2 + m_lastPoint.x();
        currSegment.y2 = currSegment.y2 + m_lastPoint.y();
    } else if (cmd == Command::Quadratic) {
        currSegment.x = currSegment.x + m_lastPoint.x();
        currSegment.y = currSegment.y + m_lastPoint.y();

        currSegment.x1 = currSegment.x1 + m_lastPoint.x();
        currSegment.y1 = currSegment.y1 + m_lastPoint.y();
    } else if (cmd == Command::HorizontalLineTo) {
        currSegment.x = currSegment.x + m_lastPoint.x();
    } else if (cmd == Command::VerticalLineTo) {
        currSegment.y = currSegment.y + m_lastPoint.y();
    } else if (cmd == Command::EllipticalArc) {
        currSegment.x = currSegment.x + m_lastPoint.x();
        currSegment.y = currSegment.y + m_lastPoint.y();
    }

    currSegment.abs = true;
    updateSegment(currSegment);
}

Segment SegmentList::segment()
{
    return m_data.at(m_position);
}

void SegmentList::updateSegment(const Segment &seg)
{
    m_data.replace(m_position, seg);
}

QString SegmentList::genPath()
{
    Segment seg = segment();
    QString cmd = seg.command;
    QString out;
    if (cmd == Command::MoveTo || cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
        out = Tools::roundNumber(seg.x) % "," % Tools::roundNumber(seg.y);
    } else if (cmd == Command::CurveTo) {
        out =   string(seg.x1) % "," % string(seg.y1) % " "
              % string(seg.x2) % "," % string(seg.y2) % " "
              % string(seg.x)  % "," % string(seg.y);
    } else if (cmd == Command::HorizontalLineTo) {
        out = string(seg.x);
    } else if (cmd == Command::VerticalLineTo) {
        out = string(seg.y);
    } else if (cmd == Command::SmoothCurveTo) {
        out =   string(seg.x2) % "," % string(seg.y2) % " "
              % string(seg.x)  % "," % string(seg.y);
    } else if (cmd == Command::Quadratic) {
        out =   string(seg.x1) % "," % string(seg.y1) % " "
              % string(seg.x)  % "," % string(seg.y);
    } else if (cmd == Command::EllipticalArc) {
        out =   string(seg.rx) % "," % string(seg.ry) % " "
                % QString::number(seg.X_AXIS_ROTATION) % " "
                % QString::number(seg.LARGE_ARC_FLAG) % ","
                % QString::number(seg.SWEEP_FLAG) % " "
                % string(seg.x)  % "," % string(seg.y);
    }
    return out;
}

// New class

void Path::processPath(SvgElement elem)
{
    QString inPath = elem.attribute("d");

    if (inPath.contains("nan")) {
        elem.removeAttribute("d");
        return;
    }

    // path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are identical
    // can't be converted to relative coordinates
    // some kind of bug
    if (inPath.contains("A")) {
        if (QString(inPath).remove(QRegExp("[^a-AZ-z]")) == "MA")
            return;
    }

    splitToSegments(inPath);

    // TODO: apply transform to filter
//    if (!Keys::get().flag(Key::ApplyTransforms)) {
//        if (elem.hasAttribute("transform")) {
//            segmentsToAbsolute();
//            m_segment.restart();
//            while (m_segment.next())
//                m_segment.setTransform(elem.attribute("transform"));
//            elem.removeAttribute("transform");
//        }

    segmentsToAbsolute();
    segmentsToRelative();

    processSegments();

    // merge segments to path
    QString outPath = segmentsToPath();

    elem.setAttribute("d", outPath);
}

void Path::setSegments(const SegmentList &list)
{
    m_segmentList = list;
}

void Path::splitToSegments(const QString &path)
{
    QStringList data;
    QString buf;
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
            if (c.isNumber() || c == '.')
                buf += c;
            else if (c == '-') {
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
    }

    QString currCmd;
    // convert to segments
    while (!data.isEmpty()) {
        if (data.first().at(0).isLetter()) {
            currCmd = data.takeFirst();
        }

        QString lowerCmd = currCmd.toLower();
        Segment segment;

        segment.command = lowerCmd;
        segment.abs = currCmd.at(0).isUpper();

        // TODO: add count check to prevent crash
        if (   lowerCmd == Command::MoveTo
            || lowerCmd == Command::LineTo
            || lowerCmd == Command::SmoothQuadratic) {
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::HorizontalLineTo) {
            segment.x  = data.takeFirst().toDouble();
            segment.y  = 0;
        } else if (lowerCmd == Command::VerticalLineTo) {
            segment.x  = 0;
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::CurveTo) {
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::SmoothCurveTo) {
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::Quadratic) {
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::EllipticalArc) {
            segment.rx = data.takeFirst().toDouble();
            segment.ry = data.takeFirst().toDouble();
            segment.X_AXIS_ROTATION = data.takeFirst().toDouble();
            segment.LARGE_ARC_FLAG  = data.takeFirst().toDouble();
            segment.SWEEP_FLAG      = data.takeFirst().toDouble();
            segment.x = data.takeFirst().toDouble();
            segment.y = data.takeFirst().toDouble();
        }

        m_segmentList.append(segment);
    }
}

void Path::processSegments()
{
    // TODO: add other commands conv

    // convert "lineto" to "horizontal/vertical lineto" equivalent
    if (!Keys::get().flag(Key::KeepLinesTo)) {
        m_segmentList.restart();
        while (m_segmentList.next()) {
            Segment seg = m_segmentList.segment();
            QString currCmd = seg.command;
            if (currCmd == Command::LineTo) {
                if (seg.x == 0 && seg.y != 0) {
                    seg.command = Command::VerticalLineTo;
                    m_segmentList.updateSegment(seg);
                } else if (seg.x != 0 && seg.y == 0) {
                    seg.command = Command::HorizontalLineTo;
                    m_segmentList.updateSegment(seg);
                }
            }
        }
    }

    if (!Keys::get().flag(Key::KeepEmptySegments)) {
        m_segmentList.restart();
        while (m_segmentList.next()) {
            Segment seg = m_segmentList.segment();
            const QString cmd = seg.command;
            // remove empty segments 'm0,0' (except first)
            if (cmd == Command::MoveTo && m_segmentList.position() > 0) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::CurveTo) {
                if (   (seg.x1 == 0 && seg.y1 == 0)
                    && (seg.x2 == 0 && seg.y2 == 0)
                    && (seg.x  == 0 && seg.y  == 0))
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::HorizontalLineTo) {
                if (seg.x == 0)
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::VerticalLineTo) {
                if (seg.y == 0)
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::SmoothCurveTo) {
                if (   (seg.x2 == 0 && seg.y2 == 0)
                    && (seg.x  == 0 && seg.y  == 0))
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::Quadratic) {
                if (   seg.x1 == 0 && seg.y1 == 0
                    && seg.x  == 0 && seg.y  == 0)
                    m_segmentList.removeCurrent();
            }
            else if (cmd == Command::EllipticalArc) {
                if (seg.x == 0 && seg.y == 0)
                    m_segmentList.removeCurrent();
            }
        }
    }
}

// NOTE: didn't work without relative conv
void Path::segmentsToAbsolute()
{
    m_segmentList.setLastPoint(0, 0);
    m_segmentList.restart();
    while (m_segmentList.next()) {
        Segment segment = m_segmentList.segment();
        if (segment.command != Command::ClosePath) {
            if (segment.abs) {
                qreal x = segment.x;
                qreal y = segment.y;
                if (x == 0 && segment.command == Command::VerticalLineTo)
                    x = m_segmentList.lastPoint().x();
                else if (y == 0 && segment.command == Command::HorizontalLineTo)
                    y = m_segmentList.lastPoint().y();
                m_segmentList.setLastPoint(x, y);
            } else {
                m_segmentList.toAbsolute();
                m_segmentList.appendLastPoint(segment.x, segment.y);
            }
        }
    }
}

void Path::segmentsToRelative()
{
    m_segmentList.setLastPoint(0, 0);
    m_segmentList.restart();
    while (m_segmentList.next()) {
        Segment segment = m_segmentList.segment();
        if (segment.command != Command::ClosePath) {
            if (segment.abs) {
                m_segmentList.toRelative();
                segment = m_segmentList.segment();
                qreal x = segment.x;
                qreal y = segment.y;
                if (segment.command == Command::HorizontalLineTo)
                    y = 0;
                else if (segment.command == Command::VerticalLineTo)
                    x = 0;
                m_segmentList.appendLastPoint(x, y);
            } else {
                m_segmentList.setLastPoint(segment.x, segment.y);
            }
        }
    }
}

QString Path::segmentsToPath()
{
    QString outPath;
    QString prevCom;
    bool isPrevComAbs;
    m_segmentList.restart();
    while (m_segmentList.next()) {
        Segment segment = m_segmentList.segment();
        QString currCmd = segment.command;
        // check is previous command is the same as next
        bool writeCmd = true;
        if (currCmd == prevCom && !prevCom.isEmpty()) {
            if (segment.abs == isPrevComAbs)
                writeCmd = false;
        }
        prevCom = currCmd;
        isPrevComAbs = segment.abs;

        if (writeCmd) {
            if (segment.abs)
                outPath += segment.command.toUpper() + " ";
            else
                outPath += segment.command + " ";
        }
        outPath += m_segmentList.genPath() + " ";
    }
    outPath.remove(QRegExp("\\ +$"));

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

                bool flag = true;
                // example: m 100 -> m100
                if (prev.isLetter() && curr == ' ')
                    flag = false;
                // example: 100 c -> 100c
                if (curr == ' ' && next.isLetter())
                    flag = false;
                // example: 42,-42 -> 42-42
                if (curr == ',' && next == '-')
                    flag = false;
                // example: 42 -42 -> 42-42
                if (prev.isNumber() && curr == ' ' && next == '-' && !isArc)
                    flag = false;

                if (flag)
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
