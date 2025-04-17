#include "BezierCurve.h"

BezierCurve::BezierCurve() {}

void BezierCurve::AddControlPoint(const QVector3D& point) 
{ 
    m_controlPoints.push_back(point); 
}

void BezierCurve::SetControlPoint(int index, const QVector3D& point) 
{
    int size = static_cast<int>(m_controlPoints.size());
    if (index >= 0 && index < size) {
        m_controlPoints[index] = point;
    }
}

const std::vector<QVector3D>& BezierCurve::GetControlPoints() const 
{ 
    return m_controlPoints; 
}

QVector3D BezierCurve::Evaluate(float t) const 
{
    if (m_controlPoints.empty()) return QVector3D(0, 0, 0);
    if (m_controlPoints.size() == 1) return m_controlPoints[0];

    // return Bernstein(t);
    return DeCasteljau(t);
}

std::vector<QVector3D> BezierCurve::Sample(int numSamples) const 
{
    std::vector<QVector3D> samples;
    for (int i = 0; i <= numSamples; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(numSamples);
        samples.push_back(Evaluate(t));
    }
    return samples;
}

QVector3D BezierCurve::Bernstein(float t) const 
{
    int n = static_cast<int>(m_controlPoints.size()) - 1;
    QVector3D result(0, 0, 0);

    for (int i = 0; i <= n; ++i) {
        float coeff = [](float n, float k)
        {
            int res = 1;
            if (k > n - k) k = n - k;
            for (int i = 0; i < k; ++i) {
                res *= (n - i);
                res /= (i + 1);
            }
            return res;
        } (n, i); // Binomial coefficient C(n, i)

        result += coeff * std::pow(t, i) * std::pow(1 - t, n - i) * m_controlPoints[i];
    }
    return result;
}

QVector3D BezierCurve::DeCasteljau(float t) const 
{
    std::vector<QVector3D> points = m_controlPoints;
    int n = points.size() - 1;

    for (int r = 1; r <= n; ++r) {
        for (int i = 0; i < n - r + 1; ++i) {
            points[i] = (1 - t) * points[i] + t * points[i + 1];
        }
    }

    return points[0];
}

