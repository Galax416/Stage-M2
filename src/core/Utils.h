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

inline bool operator<=(const QVector3D& lhs, const QVector3D& rhs)
{
    return lhs.x() <= rhs.x() && lhs.y() <= rhs.y() && lhs.z() <= rhs.z();
}

inline bool operator>=(const QVector3D& lhs, const QVector3D& rhs)
{
    return lhs.x() >= rhs.x() && lhs.y() >= rhs.y() && lhs.z() >= rhs.z();
}

enum Quadrant { TopLeft, TopRight, BottomLeft, BottomRight };

inline Quadrant GetQuadrant(const QVector3D& pos, const QVector3D& center) {
    if (pos.x() < center.x()) {
        return (pos.y() > center.y()) ? TopLeft : BottomLeft;
    } else {
        return (pos.y() > center.y()) ? TopRight : BottomRight;
    }
}

inline float GetStiffnessByQuadrant(QVector3D posA, QVector3D posB, const QVector3D& center) {
    Quadrant quadA = GetQuadrant(posA, center);
    Quadrant quadB = GetQuadrant(posB, center);

    int stiffness = 1; // Default stiffness
    if (quadA == TopLeft || quadB == TopLeft) stiffness = 500;
    else if (quadA == TopRight || quadB == TopRight) stiffness = 700;
    else if (quadA == BottomLeft || quadB == BottomLeft) stiffness = 300;
    else if (quadA == BottomRight || quadB == BottomRight) stiffness = 900;
    return stiffness;
}