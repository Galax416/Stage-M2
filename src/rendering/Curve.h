#pragma once

#include <QVector3D>
#include <QDebug>
#include <vector>
#include <cmath>

enum CurveType
{
    Hermite,
    Bernstein,
    DeCasteljau,
    BSpline,
    CatmullRom
};

class Curve
{
public:
    Curve(int degree = 3, bool closed = true) : m_degree(degree), m_closed(closed) {}

    void AddControlPoint(const QVector3D& point);
    void SetControlPoint(int index, const QVector3D& point);
    const std::vector<QVector3D>& GetControlPoints() const;

    QVector3D Evaluate(float t) const;
    std::vector<QVector3D> Sample(int numSamples) const;

    void Clear() { m_controlPoints.clear(); }

    QVector3D GetCenter() const;

    void SetCurveType(CurveType type) { m_curveType = type; }
    void SetDegree(int degree) { m_degree = degree; }
    void SetClosed(bool closed) { m_closed = closed; }
    bool IsClosed() const { return m_closed; }

private:
    CurveType m_curveType { CurveType::BSpline }; // Type of curve to use
    int m_degree { 3 }; // Degree of the curve (Cubic)}
    bool m_closed { true }; // Whether the curve is closed or not

    std::vector<QVector3D> m_controlPoints; // Control points of the curve
    mutable std::vector<float> m_knots; // Knot vector for B-spline

    QVector3D Hermite(float t) const;
    QVector3D Bernstein(float t) const;
    QVector3D DeCasteljau(float t) const;
    QVector3D BSpline(float t, const std::vector<QVector3D>& controlPoints) const ;
    QVector3D CatmullRom(float t) const;

};