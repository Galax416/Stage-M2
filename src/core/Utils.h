#pragma once

#include <QWidget>
#include <QLayout>
#include <QColor>
#include <QVector3D>
#include <cmath>
#include <random>

inline QColor floatToQColor(float value) 
{
    // Clamp
    value = std::fmax(0.0f, std::fmin(value, 10000.0f));

    // Normalize to 0-1 range
    float norm = (value - 1.0f) / (10000.0f - 1.0f);

    int r = static_cast<int>(255 * std::min(2.0f * norm, 1.0f));
    int g = static_cast<int>(255 * (1.0f - std::abs(2.0f * norm - 1.0f)));
    int b = static_cast<int>(255 * std::max(1.0f - 2.0f * norm, 0.0f));

    return QColor(r, g, b, 255); 
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