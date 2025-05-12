#pragma once

#include <cmath>

const float E_scale = 1e-1f; // Scale for Young's modulus (Pa)
// const float Density_scale = 1e-1f; // Scale for density (kg/m^3)

enum MaterialType
{
    MaterialType_Air,
    MaterialType_Rubber,
    MaterialType_Dirt,
    MaterialType_Rock,
    MaterialType_Metal, // Aluminum
    MaterialType_Bone,
};

struct MaterialProperties
{
    float E; // Young's modulus (Pa)
    float nu; // Poisson's ratio 
    float density; // Density (kg/m^3)
};

inline float f(float x) { return E_scale * std::pow(x, 0.6); } // E compression
// inline float g(float x) { return Density_scale * std::pow(x, 1.0f); } // Density compression

inline MaterialProperties GetMaterialProperties(MaterialType type)
{
    switch (type)
    {
    case MaterialType_Rubber:
        // return { 1e7f, 0.49f, 1100.0f };
        return { f(1e7f), 0.49f, 1100.0f };
        // return { f(1e7f), 0.49f, g(1100.0f) };
    case MaterialType_Dirt:
        // return { 5e8f, 0.3f, 1600.0f };
        return { f(5e8f), 0.3f, 1600.0f };
        // return { f(5e8f), 0.3f, g(1600.0f) };
    case MaterialType_Rock:
        // return { 6e10f, 0.25f, 2700.0f };
        return { f(6e10f), 0.25f, 2700.0f };
        // return { f(6e10f), 0.25f, g(2700.0f) };
    case MaterialType_Metal:
        // return { 6.9e10f, 0.33f, 2700.0f };
        return { f(6.9e10f), 0.33f, 2700.0f };
        // return { f(6.9e10f), 0.33f, g(2700.0f) };
    case MaterialType_Bone:
        // return { 1.7e10f, 0.3f, 1900.0f };
        return { f(1.7e10f), 0.3f, 1900.0f };
        // return { f(1.7e10f), 0.3f, g(1900.0f) };
    default: // Air
        return { 1e-6f, 0.0f, 1.225f }; // Default properties
    }
}
