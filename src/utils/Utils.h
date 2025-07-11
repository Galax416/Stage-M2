#pragma once

#include <QWidget>
#include <QLayout>
#include <QColor>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QtMath>
#include <cmath>
#include <random>

inline uint qHash(const QVector3D& key, uint seed = 0) 
{
    return qHash(key.x(), seed) ^ qHash(key.y(), seed << 1) ^ qHash(key.z(), seed << 2);
}

inline QVector3D Lerp(const QVector3D& a, const QVector3D& b, float t) 
{
    return (1.0f - t) * a + t * b;
}

inline QColor floatToQColor(float value) 
{
    unsigned int seed = static_cast<unsigned int>(value); // Generate a seed based on the float value
    srand(seed);
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;

    return QColor(r, g, b, 255); // Alpha set to 255 (opaque)
}

inline float RandomFloat(float min, float max) 
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

inline void clearLayout(QLayout* layout) 
{
    if (!layout) return;
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater(); // Delete the widget
        }
        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout); // Recursively clear child layouts
        }
        delete item;
    }
}

// inline bool operator<=(const QVector3D& lhs, const QVector3D& rhs)
// {
//     return lhs.x() <= rhs.x() && lhs.y() <= rhs.y() && lhs.z() <= rhs.z();
// }

// inline bool operator<(const QVector3D& lhs, const QVector3D& rhs) {
//     if (lhs.x() != rhs.x()) return lhs.x() < rhs.x();
//     if (lhs.y() != rhs.y()) return lhs.y() < rhs.y();
//     return lhs.z() < rhs.z();
// }

// inline bool operator>=(const QVector3D& lhs, const QVector3D& rhs)
// {
//     return lhs.x() >= rhs.x() && lhs.y() >= rhs.y() && lhs.z() >= rhs.z();
// }

enum Quadrant { Top, Bottom };

inline Quadrant GetQuadrant(const QVector3D& pos, const QVector3D& center, float angleDegrees = 90.0f, float epsilon = 0.15f) 
{
    QVector2D dirrection = QVector2D(pos) - QVector2D(center);
    if (dirrection.lengthSquared() < epsilon * epsilon) return Top;
    dirrection.normalize();

    QVector2D up(0, 1);  

    float dot = QVector2D::dotProduct(dirrection, up);
    dot = qBound(-1.0f, dot, 1.0f);

    float angle = std::acos(dot);

    return (angle <= qDegreesToRadians(angleDegrees * 0.5f)) ? Top : Bottom;
}

inline float GetStiffnessByQuadrant(QVector3D posA, QVector3D posB, const QVector3D& center, float angleDegrees = 100.0f) 
{
    Quadrant quadA = GetQuadrant(posA, center, angleDegrees);
    Quadrant quadB = GetQuadrant(posB, center, angleDegrees);

    float stiffness = 1000.0f; // Default stiffness
    if (quadA == Top && quadB == Top) stiffness = 450.0f;
    else if (quadA == Top && quadB == Bottom) stiffness = 500.0f;
    else if (quadA == Bottom && quadB == Bottom) stiffness = 500.0f;
    else if (quadA == Bottom && quadB == Top) stiffness = 500.0f;
    return stiffness;
}

namespace std 
{
    template <>
    struct hash<QVector3D> {
        size_t operator()(const QVector3D& v) const {
            size_t h1 = std::hash<float>{}(v.x());
            size_t h2 = std::hash<float>{}(v.y());
            size_t h3 = std::hash<float>{}(v.z());
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

inline float fromMapped(float mappedValue, float A, float B, float C)
{
    C = C - mappedValue;

    float discriminant = B * B - 4 * A * C;
    if (discriminant < 0)
        return -1.0f; 

    float sqrtDisc = sqrt(discriminant);
    float denom = 2.0f * A;
    float x1 = (-B + sqrtDisc) / denom;
    float x2 = (-B - sqrtDisc) / denom;

    // Choose the root that is within the range [0, 100]
    return (x1 >= -1 && x1 <= 1) ? x1 : x2;
}
