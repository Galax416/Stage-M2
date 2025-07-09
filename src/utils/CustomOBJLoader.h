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
    bool movable;
};

struct SpringLink
{
    int nodeA;
    int nodeB;
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
    bool isCustomOBJ { false }; // Flag to indicate if the file is a custom OBJ format
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<QVector2D> texCoords;
    std::vector<Face> faces;         // triangulated
    std::vector<Face> originalFaces; // original faces (not triangulated)
    std::vector<Node> nodes;
    std::vector<SpringLink> springLinks;
    std::unordered_map<std::string, float> variables;

public:
    inline CustomOBJLoader() {}

    inline void Clear()
    {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        faces.clear();
        originalFaces.clear();
        nodes.clear();
        springLinks.clear();
        variables.clear();
        isCustomOBJ = false;
    }

    // à refaire
    inline std::string GetVariableKey(float value)
    {
        for (const auto& var : variables) {
            if (fabs(var.second - value) < 1e-6) {
                return var.first;
            }
        }
        return std::to_string(value); 
    }


    // Load OBJ file
    inline bool LoadOBJ(const QString& filename)
    {
        // Initialize vectors
        vertices.clear();
        normals.clear();
        texCoords.clear();
        faces.clear();
        nodes.clear();
        springLinks.clear();
        variables.clear();
        isCustomOBJ = false;      
        
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
                
                // Store original face data
                if (vIndices.size() >= 3) {
                    Face original;
                    original.vertexIndices = vIndices;
                    original.texCoordIndices = vtIndices;
                    original.normalIndices = vnIndices;
                    originalFaces.push_back(original);
                }
            
                // Triangulate using fan method if more than 3 vertices
                for (size_t i = 1; i + 1 < vIndices.size(); ++i) {
                    Face tri;
                    tri.vertexIndices = { vIndices[0], vIndices[i], vIndices[i + 1] };
                    if (vtIndices.size() >= i + 2) {
                        tri.texCoordIndices = { vtIndices[0], vtIndices[i], vtIndices[i + 1] };
                    }
                    faces.push_back(tri);
                }
            }

            else if (prefix == "l") {
                int p1, p2;
                ss >> p1 >> p2;
                Face line;
                line.vertexIndices = { p1 - 1, p2 - 1 };
                originalFaces.push_back(line);
            }

            // Metadata (ex: node 0 r1 r2 or spring 0 1 k1)
            else if (prefix == "node") {
                isCustomOBJ = true;
                size_t index;
                std::string radiusVar, massVar, movableVar;
                ss >> index >> radiusVar >> massVar >> movableVar;
                if (index < vertices.size()) {
                    float radius = variables.count(radiusVar) ? variables[radiusVar] : std::stof(radiusVar);
                    float mass = variables.count(massVar) ? variables[massVar] : std::stof(massVar);
                    bool movable = movableVar == "1";
                    nodes.push_back({ static_cast<int>(index), radius, mass, movable });
                }
            }
            else if (prefix == "spring") {
                isCustomOBJ = true;
                int n1, n2;
                std::string stiffnessVar;
                ss >> n1 >> n2 >> stiffnessVar;
                float stiffness = variables.count(stiffnessVar) ? variables[stiffnessVar] : std::stof(stiffnessVar);
                springLinks.push_back({ n1, n2, stiffness });
            }
        }

        file.close();
        return true;
    }


    // Save OBJ file
    inline void SaveOBJ(const QString& filename)
    {
        std::ofstream file(filename.toStdString(), std::ios::trunc); // Use std::ios::trunc to overwrite the file
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
            file << "node " << node.vertexIndex << " " << GetVariableKey(node.radius) << " " << GetVariableKey(node.mass) << " " << node.movable << std::endl;
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

