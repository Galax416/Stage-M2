#pragma once

#include <QVector3D>

struct Line
{
    QVector3D start;
    QVector3D end;

    inline Line() {}
    inline Line(const QVector3D& s, const QVector3D& e) : start(s), end(e) { }
};

struct Ray
{
    QVector3D origin;
    QVector3D direction;

    inline Ray() {}
    inline Ray(const QVector3D& o, const QVector3D& d) : origin(o), direction(d) { NormalizeDirection(); }
    inline void NormalizeDirection() { direction.normalize(); }
};
