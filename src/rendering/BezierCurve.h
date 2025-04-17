#pragma once

#include <QVector3D>
#include <vector>
#include <cmath>

class BezierCurve
{
public:
    BezierCurve();
    void AddControlPoint(const QVector3D& point);
    void SetControlPoint(int index, const QVector3D& point);
    const std::vector<QVector3D>& GetControlPoints() const;

    QVector3D Evaluate(float t) const;
    std::vector<QVector3D> Sample(int numSamples) const;

    void Clear() { m_controlPoints.clear(); }

private:
    std::vector<QVector3D> m_controlPoints;
    QVector3D Bernstein(float t) const;
    QVector3D DeCasteljau(float t) const;
};