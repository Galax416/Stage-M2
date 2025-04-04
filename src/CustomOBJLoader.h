#pragma once

#include <QVector3D>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>

struct Node
{
    int vertexIndex;
    float radius;
    float mass;
};

struct SpringLink
{
    int nodeA;
    int nodeB;
    // float restLength;
    float stiffness;
};

struct Face
{
    std::vector<int> vertexIndices;
    std::vector<int> texCoordIndices;
    std::vector<int> normalIndices;
};

class CustomOBJLoader
{
public:
    bool isCustomOBJ = false; // Flag to indicate if the file is a custom OBJ format
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<QVector2D> texCoords;
    std::vector<Face> faces;
    std::vector<Node> nodes;
    std::vector<SpringLink> springLinks;
    std::unordered_map<std::string, float> variables;

public:
    CustomOBJLoader() = default;
    ~CustomOBJLoader() = default;

    std::string GetVariableKey(float value)
    {
        for (const auto& var : variables) {
            if (fabs(var.second - value) < 1e-6) {
                return var.first;
            }
        }
        return std::to_string(value); 
    }

    // Load OBJ file
    bool LoadOBJ(const QString& filename)
    {
        std::ifstream file(filename.toStdString());
        if (!file.is_open()) {
            std::cerr << "Impossible to open " << filename.toStdString() << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "#") continue; // Ignore commentaries

            // Global variables (ex: r1=23)
            else if (prefix.find('=') != std::string::npos) {
                size_t eqPos = prefix.find('=');
                std::string varName = prefix.substr(0, eqPos);
                float value = std::stof(prefix.substr(eqPos + 1));
                variables[varName] = value;
            }

            // Vertices, normals, and texture coordinates
            else if (prefix == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                vertices.push_back(QVector3D(x, y, z));
            }
            else if (prefix == "vn") {
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back(QVector3D(x, y, z));
            }
            else if (prefix == "vt") {
                float u, v;
                ss >> u >> v;
                texCoords.push_back(QVector2D(u, v));
            }
            else if (prefix == "f") {
                Face originalFace;
                std::vector<std::string> vertDefs;
                std::string vertDef;
            
                while (ss >> vertDef) {
                    vertDefs.push_back(vertDef);
                }
            
                // Parse all vertex definitions into indices
                std::vector<int> vIndices, vtIndices, vnIndices;
                for (const std::string& vd : vertDefs) {
                    std::istringstream vs(vd);
                    std::string idx;
                    std::vector<int> indices;
                    while (std::getline(vs, idx, '/')) {
                        indices.push_back(idx.empty() ? 0 : std::stoi(idx) - 1);
                    }
                    vIndices.push_back(indices[0]);
                    if (indices.size() > 1) vtIndices.push_back(indices[1]);
                    if (indices.size() > 2) vnIndices.push_back(indices[2]);
                }
            
                // Triangulate using fan method if more than 3 vertices
                for (size_t i = 1; i + 1 < vIndices.size(); ++i) {
                    Face tri;
                    tri.vertexIndices = { vIndices[0], vIndices[i], vIndices[i + 1] };
                    if (vtIndices.size() >= i + 2) {
                        tri.texCoordIndices = { vtIndices[0], vtIndices[i], vtIndices[i + 1] };
                    }
                    if (vnIndices.size() >= i + 2) {
                        tri.normalIndices = { vnIndices[0], vnIndices[i], vnIndices[i + 1] };
                    }
                    faces.push_back(tri);
                }
            }

            // Metadata (ex: node 0 r1 r2 or spring 0 1 k1)
            else if (prefix == "node") {
                isCustomOBJ = true;
                long unsigned int index;
                std::string radiusVar, massVar;
                ss >> index >> radiusVar >> massVar;
                if (index < vertices.size()) {
                    nodes.push_back({ (int)index, variables[radiusVar], variables[massVar] });
                }
            }
            else if (prefix == "spring") {
                isCustomOBJ = true;
                int n1, n2;
                std::string stiffnessVar;
                ss >> n1 >> n2 >> stiffnessVar;
                springLinks.push_back({ n1, n2, variables[stiffnessVar] });
            }
        }

        file.close();
        return true;
    }

    // Save OBJ file
    void SaveOBJ(const QString& filename)
    {
        std::ofstream file(filename.toStdString());
        if (!file.is_open()) {
            std::cerr << "Impossible to save " << filename.toStdString() << std::endl;
            return;
        }

        // Global variables
        file << "# Global variables" << std::endl;
        for (const auto& var : variables) {
            file << var.first << "=" << var.second << std::endl;
        }
        file << std::endl;

        // Vertices
        file << "# Vertices" << std::endl;
        for (const auto& vertex : vertices) {
            file << "v " << vertex.x() << " " << vertex.y() << " " << vertex.z() << std::endl;
        }
        file << std::endl;

        // Normals
        file << "# Normals" << std::endl;
        for (const auto& normal : normals) {
            file << "vn " << normal.x() << " " << normal.y() << " " << normal.z() << std::endl;
        }
        file << std::endl;

        // Texture coordinates
        file << "# Texture coordinates" << std::endl;
        for (const auto& texCoord : texCoords) {
            file << "vt " << texCoord.x() << " " << texCoord.y() << std::endl;
        }
        file << std::endl;

        // Faces
        file << "# Faces" << std::endl;
        for (const auto& face : faces) {
            file << "f ";
            for (size_t i = 0; i < face.vertexIndices.size(); ++i) {
                file << face.vertexIndices[i] + 1;
                if (i < face.texCoordIndices.size()) {
                    file << "/" << face.texCoordIndices[i] + 1;
                }
                if (i < face.normalIndices.size()) {
                    file << "/" << face.normalIndices[i] + 1;
                }
                file << " ";
            }
            file << std::endl;
        }
        file << std::endl;

        // Nodes
        file << "# Nodes" << std::endl;
        for (const auto& node : nodes) {
            file << "node " << node.vertexIndex << " " << GetVariableKey(node.radius) << " " << GetVariableKey(node.mass) << std::endl;
        }
        file << std::endl;

        // Spring links
        file << "# Spring links" << std::endl;
        for (const auto& spring : springLinks) {
            file << "spring " << spring.nodeA << " " << spring.nodeB << " " << GetVariableKey(spring.stiffness) << std::endl;
        }
        file << std::endl;

        file.close();
    }   
};

