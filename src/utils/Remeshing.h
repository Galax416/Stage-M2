#pragma once

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <QVector3D>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "Particle.h"
#include "Spring.h"
#include "TriangleCollider.h"
#include "Utils.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3                Point;
typedef CGAL::Surface_mesh<Point>      SurfaceMesh;

namespace PMP = CGAL::Polygon_mesh_processing;


struct RemeshingConstraints {
    std::unordered_set<SurfaceMesh::Vertex_index> constrainedVertices;
    std::unordered_set<SurfaceMesh::Edge_index> constrainedEdges;
};

struct BorderSprings {
    std::vector<std::shared_ptr<Spring>> springs;
    std::vector<std::shared_ptr<Particle>> particles;
};

inline SurfaceMesh ConvertToCGALMeshWithConstraints(
    const std::vector<std::shared_ptr<Particle>>& particles,
    const std::vector<std::shared_ptr<TriangleCollider>>& triangles,
    std::unordered_map<SurfaceMesh::Vertex_index, std::shared_ptr<Particle>>& vertexToParticleMap,
    RemeshingConstraints& constraints)
{
    SurfaceMesh mesh;
    std::unordered_map<std::shared_ptr<Particle>, SurfaceMesh::Vertex_index> particleToVertexMap;

    for (const auto& p : particles)
    {
        QVector3D pos = p->GetPosition();
        SurfaceMesh::Vertex_index v = mesh.add_vertex(Point(pos.x(), pos.y(), pos.z()));
        particleToVertexMap[p] = v;
        vertexToParticleMap[v] = p;

        if (p->IsConstraint()) {
            constraints.constrainedVertices.insert(v);  // Marquer les sommets à contraindre
        }
    }

    for (const auto& tri : triangles)
    {
        auto a = tri->p0;
        auto b = tri->p1;
        auto c = tri->p2;
        mesh.add_face(particleToVertexMap[a], particleToVertexMap[b], particleToVertexMap[c]);
    }

    return mesh;
}

inline void MarkConstrainedVertices(
    SurfaceMesh& mesh,
    const RemeshingConstraints& constraints,
    SurfaceMesh::Property_map<SurfaceMesh::Vertex_index, bool>& vertexConstrained)
{
    for (auto v : mesh.vertices())
    {
        if (constraints.constrainedVertices.count(v)) vertexConstrained[v] = true;
        else vertexConstrained[v] = false;
    }
}


inline void MarkConstrainedEdges(
    SurfaceMesh& mesh,
    const RemeshingConstraints& constraints,
    SurfaceMesh::Property_map<SurfaceMesh::Edge_index, bool>& edgeConstrained)
{    
    // for (auto f : mesh.faces())
    // {
    //     std::vector<SurfaceMesh::Vertex_index> faceVertices;
    //     for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh))
    //         faceVertices.push_back(v);

    //     for (int i = 0; i < 3; ++i)
    //     {
    //         auto v1 = faceVertices[i];
    //         auto v2 = faceVertices[(i + 1) % 3];

    //         if (constraints.constrainedVertices.count(v1) && constraints.constrainedVertices.count(v2))
    //         {
    //             auto h = CGAL::halfedge(v1, v2, mesh);
    //             if (!h.second) {
    //                 h = CGAL::halfedge(v2, v1, mesh);
    //             }
    //             if (h.second) {
    //                 auto e = mesh.edge(h.first);
    //                 edgeConstrained[e] = true;
    //             }
    //         }
    //     }
    // }

    for (auto e : mesh.edges())
    {
        auto h = mesh.halfedge(e);
        auto v1 = mesh.source(h);
        auto v2 = mesh.target(h);

        if (constraints.constrainedVertices.count(v1) && constraints.constrainedVertices.count(v2))
        {
            edgeConstrained[e] = true;
        }
    }
}

inline std::vector<std::vector<std::shared_ptr<Particle>>> GetBorderParticles(
    const SurfaceMesh& mesh,
    const std::unordered_map<SurfaceMesh::Vertex_index, std::shared_ptr<Particle>>& vertexToParticleMap)
{
    std::unordered_set<SurfaceMesh::Halfedge_index> visited;
    std::vector<std::vector<std::shared_ptr<Particle>>> boundaryParticles;

    for (auto h : mesh.halfedges())
    {
        if (!mesh.is_border(h) || visited.count(h)) continue;

        std::vector<std::shared_ptr<Particle>> boundary;
        SurfaceMesh::Halfedge_index start = h;
        SurfaceMesh::Halfedge_index current = h;

        do {
            visited.insert(current);
            auto v = mesh.target(current);
            auto it = vertexToParticleMap.find(v);
            if (it != vertexToParticleMap.end()) boundary.push_back(it->second);
            current = mesh.next(current);
        } while (current != start && mesh.is_border(current));

        if (!boundary.empty()) boundaryParticles.push_back(boundary);
    }

    return boundaryParticles;
}

inline std::vector<BorderSprings> GetBorderSprings(
    const SurfaceMesh& mesh,
    const std::unordered_map<SurfaceMesh::Vertex_index, std::shared_ptr<Particle>>& vertexToParticleMap,
    const std::vector<std::shared_ptr<Spring>>& allSprings)
{
    auto borderParticleLoops = GetBorderParticles(mesh, vertexToParticleMap);

    std::vector<BorderSprings> result;

    for (const auto& borderParticles : borderParticleLoops)
    {
        BorderSprings borderData;
        borderData.particles = borderParticles;

        for (const auto& spring : allSprings)
        {
            auto p1 = spring->GetP1();
            auto p2 = spring->GetP2();

            bool p1OnBorder = std::find(borderData.particles.begin(), borderData.particles.end(), p1) != borderData.particles.end();
            bool p2OnBorder = std::find(borderData.particles.begin(), borderData.particles.end(), p2) != borderData.particles.end();

            if (p1OnBorder && p2OnBorder)
                borderData.springs.push_back(spring);
        }

        result.push_back(borderData);
    }

    return result;
}

inline std::unordered_map<SurfaceMesh::Vertex_index, int> AssignSegmentToVertices(
    const SurfaceMesh& mesh,
    const QVector3D& center,
    const std::vector<float>& angularWeights,
    const QVector3D& up = QVector3D(0, 0, 1),
    const QVector3D& ref = QVector3D(0, 1, 0))
{
    std::unordered_map<SurfaceMesh::Vertex_index, int> segmentMap;

    // Check if angularWeights is valid
    float total = std::accumulate(angularWeights.begin(), angularWeights.end(), 0.0f);
    if (std::abs(total - 1.0f) > 0.01f) {
        qWarning("Warning: AssignSegmentToVertices: angularWeights should sum to 1.0");
        return segmentMap;
    }

    std::vector<float> cumulativeAngles;
    cumulativeAngles.push_back(0.0f);
    for (float w : angularWeights) cumulativeAngles.push_back(cumulativeAngles.back() + w * 2.0f * M_PI);

    for (auto v : mesh.vertices())
    {
        auto pt = mesh.point(v);
        QVector3D dir(pt.x() - center.x(), pt.y() - center.y(), pt.z() - center.z());
        dir.normalize();

        QVector3D proj = dir - QVector3D::dotProduct(dir, up) * up;
        QVector3D ref2D = ref - QVector3D::dotProduct(ref, up) * up;
        ref2D.normalize();
        proj.normalize();

        float angle = std::atan2(QVector3D::crossProduct(ref2D, proj).length(), QVector3D::dotProduct(ref2D, proj));
        if (QVector3D::dotProduct(QVector3D::crossProduct(ref2D, proj), up) < 0) angle = 2 * M_PI - angle;

        int segment = 0;
        for (size_t i = 0; i < cumulativeAngles.size() - 1; ++i)
        {
            if (angle >= cumulativeAngles[i] && angle < cumulativeAngles[i + 1])
            {
                segment = static_cast<int>(i);
                break;
            }
        }
        segmentMap[v] = segment;
    }

    return segmentMap;
}


inline void RemeshWithConstraints(
    SurfaceMesh& mesh,
    const RemeshingConstraints& constraints,
    double targetEdgeLength,
    int iterations = 3)
{
    auto vmap = mesh.add_property_map<SurfaceMesh::Vertex_index, bool>("v:is_constrained", false).first;
    auto emap = mesh.add_property_map<SurfaceMesh::Edge_index, bool>("e:is_constrained", false).first;

    MarkConstrainedVertices(mesh, constraints, vmap);
    MarkConstrainedEdges(mesh, constraints, emap);

        
    PMP::isotropic_remeshing(
        faces(mesh),
        targetEdgeLength,
        mesh,
        PMP::parameters::number_of_iterations(iterations)
            .vertex_is_constrained_map(vmap)
            .edge_is_constrained_map(emap)
    );

}


inline void ReconstructFromCGALMesh(
    const SurfaceMesh& mesh,
    const std::unordered_map<SurfaceMesh::Vertex_index, int>& sectorMap,
    const std::unordered_map<int, float>& sectorStiffness,
    std::unordered_map<SurfaceMesh::Vertex_index, std::shared_ptr<Particle>>& vertexToParticleMap,
    std::vector<std::shared_ptr<Particle>>& outParticles,
    std::vector<std::shared_ptr<Spring>>& outSprings,
    std::vector<std::shared_ptr<TriangleCollider>>& outTriangles)
{
    vertexToParticleMap.clear();

    for (auto v : mesh.vertices())
    {
        const auto& pt = mesh.point(v);
        QVector3D pos(pt.x(), pt.y(), pt.z());
        auto particle = std::make_shared<Particle>(pos, 1, 1.0f);
        particle->SetSegmentID(sectorMap.at(v));
        vertexToParticleMap[v] = particle;
        outParticles.push_back(particle);
    }

    // Springs from edges
    for (auto e : mesh.edges())
    {
        auto he = mesh.halfedge(e);
        auto v1 = mesh.source(he);
        auto v2 = mesh.target(he);

        auto p1 = vertexToParticleMap[v1];
        auto p2 = vertexToParticleMap[v2];

        int s1 = p1->GetSegmentID();
        int s2 = p2->GetSegmentID();

        float stiffness = s1 == s2 ? sectorStiffness.at(s1) : sectorStiffness.at(s2);

        auto spring = std::make_shared<Spring>(p1, p2, stiffness);
        outSprings.push_back(spring);
    }

    // TriangleColliders from faces
    for (auto f : mesh.faces())
    {
        std::vector<std::shared_ptr<Particle>> faceParticles;
        for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh)) faceParticles.push_back(vertexToParticleMap[v]);
        
        if (faceParticles.size() == 3)
        {
            outTriangles.push_back(std::make_shared<TriangleCollider>(
                faceParticles[0], faceParticles[1], faceParticles[2]
            ));
        }
    }
}

