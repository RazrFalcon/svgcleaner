#ifndef PATHS_H
#define PATHS_H

#include <QStringList>
#include <QPointF>

#include "tools.h"

namespace Command {
    static const QChar MoveTo           = 'm';
    static const QChar LineTo           = 'l';
    static const QChar HorizontalLineTo = 'h';
    static const QChar VerticalLineTo   = 'v';
    static const QChar CurveTo          = 'c';
    static const QChar SmoothCurveTo    = 's';
    static const QChar Quadratic        = 'q';
    static const QChar SmoothQuadratic  = 't';
    static const QChar EllipticalArc    = 'a';
    static const QChar ClosePath        = 'z';
}

class Segment
{
public:
    Segment();
    QStringList toStringList() const;
    QString string(qreal value) const;
    void setTransform(const QString &text);
    void toRelative(qreal xLast, qreal yLast);
    void toAbsolute(qreal xLast, qreal yLast);
    QList<Segment> toCurve(qreal prevX, qreal prevY);

    QChar command;
    bool absolute;
    // is this command defined in source path
    bool srcCmd;
    qreal x;
    qreal y;
    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;
    qreal rx;
    qreal ry;
    int xAxisRotation;
    int largeArc;
    int sweep;

private:
    bool m_isApplyRound;
};

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem);
    void setSegments(const QList<Segment> &list);
    QString segmentsToPath();
    void segmentsToRelative();

private:
    QList<Segment> m_segmentList;
    void splitToSegments(const QString &path);
    void processSegments();
    void segmentsToAbsolute();
    QString trimPath(const QString &path);
};

#endif // PATHS_H
