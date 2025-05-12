#pragma once

#include <QWidget>
#include <QLayout>
#include <QColor>
#include <QVector3D>
#include <QtMath>
#include <cmath>
#include <random>

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

inline Quadrant GetQuadrant(const QVector3D& pos, const QVector3D& center, float epsilon = 0.02f) 
{
    return (pos.y() > center.y() - epsilon) ? Top : Bottom;
}

inline float GetStiffnessByQuadrant(QVector3D posA, QVector3D posB, const QVector3D& center) 
{
    Quadrant quadA = GetQuadrant(posA, center);
    Quadrant quadB = GetQuadrant(posB, center);

    int stiffness = 1.0f; // Default stiffness
    if (quadA == Top || quadB == Top) stiffness = 650.0f;
    else if (quadA == Bottom || quadB == Bottom) stiffness = 250.0f;
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

