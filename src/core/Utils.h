#pragma once

#include <QWidget>
#include <QLayout>
#include <QColor>
#include <cmath>

inline QColor floatToQColor(float value) {
    if (value < 0.0f) value = 0.0f;

    float scaledValue = std::log1p(value); 
    
    float hue = fmod(scaledValue * 60.0f, 360.0f); 

    int h = static_cast<int>(hue); 
    int s = 255;
    int v = 255; 
    int a = 255;

    return QColor::fromHsv(h, s, v, a);
}

inline void clearLayout(QLayout* layout) {
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