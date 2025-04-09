#pragma once

#include <QVector3D>
#include <unordered_map>
#include <vector>

#include "BoundingBox.h"

class Particle; // Forward declaration

// Sphere Collider
struct SphereCollider 
{
	QVector3D position;
	float radius;

	inline SphereCollider() : radius(1.0f) { }
	inline SphereCollider(const QVector3D& p, float r) : position(p), radius(r) { }
};

struct TriangleCollider 
{
	int a, b, c; // indices of the vertices
	QVector3D p1, p2, p3; // vertices of the triangle
	AABB bounds; // bounding box of the triangle

	inline TriangleCollider(int a, int b, int c) : a(a), b(b), c(c) { }

	void Update(const std::vector<std::shared_ptr<Particle>>& particles);
	/*{
		p1 = particles[a]->transform.position;
		p2 = particles[b]->transform.position;
		p3 = particles[c]->transform.position;

		QVector3D minPos = QVector3D(qMin(qMin(p1.x(), p2.x()), p3.x()),
									 qMin(qMin(p1.y(), p2.y()), p3.y()),
									 qMin(qMin(p1.z(), p2.z()), p3.z()));
		QVector3D maxPos = QVector3D(qMax(qMax(p1.x(), p2.x()), p3.x()),
									 qMax(qMax(p1.y(), p2.y()), p3.y()),
									 qMax(qMax(p1.z(), p2.z()), p3.z()));

		bounds.position = (minPos + maxPos) * 0.5f;
		bounds.size = (maxPos - minPos) * 0.5f;
	}*/

};

using GridKey = std::tuple<int, int, int>;

namespace std {
    template <>
    struct hash<GridKey> {
        std::size_t operator()(const GridKey& key) const {
            std::size_t h1 = std::hash<int>{}(std::get<0>(key));
            std::size_t h2 = std::hash<int>{}(std::get<1>(key));
            std::size_t h3 = std::hash<int>{}(std::get<2>(key));

            std::size_t seed = h1;
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

struct GridHash {
    std::unordered_map<GridKey, std::vector<TriangleCollider*>> grid;
    float cellSize = 1.0f;

    inline GridKey key(const QVector3D& pos) const {
        return {
            static_cast<int>(std::floor(pos.x() / cellSize)),
            static_cast<int>(std::floor(pos.y() / cellSize)),
            static_cast<int>(std::floor(pos.z() / cellSize))
        };
    }

    inline void Insert(TriangleCollider* tri) { grid[key(tri->bounds.position)].push_back(tri); }

    inline std::vector<TriangleCollider*> Query(const QVector3D& pos) const {
        auto it = grid.find(key(pos));
        if (it != grid.end()) return it->second;
        return {};
    }

    inline void Clear() { grid.clear(); }
};

/*
bool IntersectParticleTriangle(const QVector3D& p, float radius,
	const QVector3D& a, const QVector3D& b, const QVector3D& c,
	QVector3D& correctionNormal, float& penetrationDepth);

void ResolveSoftBodyCollisions(const std::vector<std::shared_ptr<Particle>>& particles,
	const std::vector<TriangleCollider>& triangles,
	const GridHash& grid);
*/