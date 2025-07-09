#include "Curve.h"

int BinomialCoeff(int n, int k);
std::vector<float> GenerateKnotVector(int degree, std::vector<QVector3D> controlPoints, bool closed);
float BSplineBasis(int i, int k, float t, const std::vector<float>& knots, int degree);
std::vector<QVector3D> GetClosedControlPoints(std::vector<QVector3D> controlPoints, int degree);


void Curve::AddControlPoint(const QVector3D& point) 
{ 
    m_controlPoints.push_back(point); 
}

void Curve::SetControlPoint(int index, const QVector3D& point) 
{
    int size = static_cast<int>(m_controlPoints.size());
    if (index >= 0 && index < size) {
        m_controlPoints[index] = point;
    }
}

const std::vector<QVector3D>& Curve::GetControlPoints() const 
{ 
    return m_controlPoints; 
}

QVector3D Curve::Evaluate(float t) const 
{
    if (m_controlPoints.empty()) return QVector3D(0, 0, 0);
    if (m_controlPoints.size() == 1) return m_controlPoints[0];
    
    switch (m_curveType)
    {
    case CurveType::Hermite:
        return Hermite(t);
        break;
    case CurveType::Bernstein:
        return Bernstein(t);
        break;
    case CurveType::DeCasteljau:
        return DeCasteljau(t);
        break;
    case CurveType::BSpline:
    {
        std::vector<QVector3D> controlPoints = m_closed ? GetClosedControlPoints(m_controlPoints, m_degree) : m_controlPoints;
        return BSpline(t, controlPoints);
        break;
    }
    case CurveType::CatmullRom:
        return CatmullRom(t);
        break;

    default:
        return QVector3D(0, 0, 0); // Default case
        break;
    }
    
}

std::vector<QVector3D> Curve::Sample(int numSamples) const 
{
    if (m_controlPoints.empty()) return std::vector<QVector3D>();

    if (m_curveType == CurveType::BSpline) {
        // Generate knot vector for B-spline
        std::vector<QVector3D> controlPoints = m_closed ? GetClosedControlPoints(m_controlPoints, m_degree) : m_controlPoints;
        m_knots = GenerateKnotVector(m_degree, controlPoints, m_closed);
    }

    std::vector<QVector3D> samples;

    float start = m_curveType == CurveType::BSpline ? m_knots[m_degree] : 0.0f;
    float end = m_curveType == CurveType::BSpline ? m_knots[m_knots.size() - m_degree - 1] : 1.0f;

    for (int i = 0; i <= numSamples; ++i) {
        float t = start + (end - start) * static_cast<float>(i) / static_cast<float>(numSamples);
        samples.push_back(Evaluate(t));
    }
    
    return samples;
}

QVector3D Curve::GetCenter() const 
{
    if (m_controlPoints.empty()) return QVector3D(0, 0, 0);
    
    QVector3D center(0, 0, 0);
    for (const auto& point : m_controlPoints) center += point;
    center /= static_cast<float>(m_controlPoints.size());
    
    return center;
}

std::vector<QVector3D> GetClosedControlPoints(std::vector<QVector3D> controlPoints, int degree) 
{
    std::vector<QVector3D> closedControlPoints = controlPoints;
    for (int i = 0; i < degree; ++i) {
        closedControlPoints.push_back(controlPoints[i]);  // Duplicate the first points
    }
    return closedControlPoints;
}

// -------------------- HERMITE ------------------------

QVector3D Curve::Hermite(float t) const 
{
    int n = static_cast<int>(m_controlPoints.size()) - 1;
    QVector3D result(0, 0, 0);

    for (int i = 0; i <= n; ++i) {
        float h00 = (1 + 2 * t) * (1 - t) * (1 - t);
        float h10 = t * (1 - t) * (1 - t);
        float h01 = t * t * (3 - 2 * t);
        float h11 = t * t * (t - 1);

        result += h00 * m_controlPoints[i] + h10 * m_controlPoints[(i + 1) % n] +
                  h01 * m_controlPoints[(i + 2) % n] + h11 * m_controlPoints[(i + 3) % n];
    }
    return result;
}

// -------------------- BERNSTEIN ------------------------

int BinomialCoeff(int n, int k) 
{
    int res = 1;
    if (k > n - k) k = n - k;
    for (int i = 0; i < k; ++i) {
        res *= (n - i);
        res /= (i + 1);
    }
    return res;
}

QVector3D Curve::Bernstein(float t) const 
{
    int n = static_cast<int>(m_controlPoints.size()) - 1;
    QVector3D result(0, 0, 0);

    for (int i = 0; i <= n; ++i) {
        float coeff = BinomialCoeff(n, i);
        float b = coeff * std::pow(t, i) * std::pow(1 - t, n - i);
        result += b * m_controlPoints[i];
    }
    return result;
}

// -------------------- DE CASTELJAU --------------------

QVector3D Curve::DeCasteljau(float t) const 
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

// -------------------- B-SPLINE ------------------------

std::vector<float> GenerateKnotVector(int degree, std::vector<QVector3D> controlPoints, bool closed) 
{
    int n = static_cast<int>(controlPoints.size()) - 1;
    int m = n + degree + 1;
    std::vector<float> knots(m + 1);

    int numInteriorKnots = m - 2 * (degree + 1) + 1;

    if (closed) {
        // Uniform periodic (closed)
        for (int i = 0; i <= m; ++i) {
            knots[i] = static_cast<float>(i) / (m + 1);
        }
    } else {
        // Uniform open (non-periodic)
        for (int i = 0; i <= degree; ++i) knots[i] = 0.0f;
        for (int i = degree + 1; i <= n; ++i) knots[i] = static_cast<float>(i - degree) / (numInteriorKnots + 1);
        for (int i = n + 1; i <= m; ++i) knots[i] = 1.0f;
    }
    return knots;
}

float BSplineBasis(int i, int k, float t, const std::vector<float>& knots, int degree)
{
    if (k == 0) {
        if (t == knots.back() && i == static_cast<int>(knots.size()) - 2 - degree) return 1.0f;
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
    } else {
        float denom1 = knots[i + k] - knots[i];
        float denom2 = knots[i + k + 1] - knots[i + 1];
        float term1 = denom1 > 0 ? (t - knots[i]) / denom1 * BSplineBasis(i, k - 1, t, knots, degree) : 0.0f;
        float term2 = denom2 > 0 ? (knots[i + k + 1] - t) / denom2 * BSplineBasis(i + 1, k - 1, t, knots, degree) : 0.0f;
        return term1 + term2;
    }
}

QVector3D Curve::BSpline(float t, const std::vector<QVector3D>& controlPoints) const 
{
    int n = static_cast<int>(controlPoints.size()) - 1;
    QVector3D result(0, 0, 0);
    
    for (int i = 0; i <= n; ++i) {
        float b = BSplineBasis(i, m_degree, t, m_knots, m_degree);
        result += b * controlPoints[i];
    }
    return result;
}

// -------------------- CATMULL-ROM ---------------------

QVector3D Curve::CatmullRom(float t) const 
{
    int n = static_cast<int>(m_controlPoints.size());
    if (n < 4) return QVector3D(0, 0, 0);

    float ft = t * (m_closed ? n : n - 3);
    int i = static_cast<int>(ft);
    float localT = ft - i;
    float t2 = localT * localT;
    float t3 = t2 * localT;

    if (!m_closed) {
        // Open curve: clamp the indices
        int idx0 = std::max(0, i - 1);
        int idx1 = i;
        int idx2 = std::min(i + 1, n - 1);
        int idx3 = std::min(i + 2, n - 1);
        return 0.5f * (
            (2.0f * m_controlPoints[idx1]) +
            (-m_controlPoints[idx0] + m_controlPoints[idx2]) * localT +
            (2.0f * m_controlPoints[idx0] - 5.0f * m_controlPoints[idx1] + 4.0f * m_controlPoints[idx2] - m_controlPoints[idx3]) * t2 +
            (-m_controlPoints[idx0] + 3.0f * m_controlPoints[idx1] - 3.0f * m_controlPoints[idx2] + m_controlPoints[idx3]) * t3
        );
    } else {
        // Closed curve: wrap around the indices
        const QVector3D& p0 = m_controlPoints[(i - 1 + n) % n];
        const QVector3D& p1 = m_controlPoints[i % n];
        const QVector3D& p2 = m_controlPoints[(i + 1) % n];
        const QVector3D& p3 = m_controlPoints[(i + 2) % n];

        return 0.5f * (
            (2.0f * p1) +
            (-p0 + p2) * localT +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
        );
    }
}


