#pragma once

#include <QVector3D>
#include <cmath>
#include "Constants.h" 

inline QVector3D ScreenCordinateToUV(QVector3D screenCoord) 
{
    return QVector3D(screenCoord.x() / SCREEN_WIDTH, screenCoord.y() / SCREEN_HEIGHT, 0.0f); 
}
inline QVector3D UVToScreenCordinate(QVector3D uv) 
{
    return QVector3D(uv.x() * SCREEN_WIDTH, uv.y() * SCREEN_HEIGHT, 0.0f);
}
