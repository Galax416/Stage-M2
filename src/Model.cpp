#include "Model.h"

Model::Model()
{
    Init();
}

Model::Model(const QString& path)
{
    Init();
    LoadModel(path);
    SetUpColliders();
}

Model::~Model()
{
    if (mesh) delete mesh;
    if (customOBJ) delete customOBJ;

}

void Model::Init()
{
    initializeOpenGLFunctions();
    type = RIGIDBODY_TYPE_BOX;
    bounds = AABB();
    oldPosition = transform.position;
    customOBJ = new CustomOBJLoader();
}

void Model::SynsCollisionVolumes()
{
    if (type == RIGIDBODY_TYPE_SPHERE) {
        sphereCollider.position = transform.position;
        sphereCollider.radius = transform.scale.x();
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        boxCollider.position = transform.position;
        boxCollider.size = bounds.size * transform.scale;
        boxCollider.orientation = transform.GetRotationMatrix();
    }
}

void Model::SetUpColliders()
{
    if (mesh == nullptr) return;
    BuildAABB();
    SynsCollisionVolumes();
}

void Model::LoadModel(const QString& path)
{
    // Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile(path.toStdString().c_str(),
    //     aiProcess_Triangulate |
    //     aiProcess_FlipUVs |
    //     aiProcess_CalcTangentSpace);
    
    // if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    //     std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    //     return;
    // }

    // m_directory = QFileInfo(path).absolutePath();
    // ProcessNode(scene->mRootNode, scene);


    bool ok = customOBJ->LoadOBJ(path.toStdString().c_str());
    if (!ok) {
        std::cerr << "Failed to load OBJ file: " << path.toStdString() << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    Material material;

    for (long unsigned int i = 0; i < customOBJ->faces.size(); i++) {
        Face face = customOBJ->faces[i];
        for (long unsigned int j = 0; j < face.vertexIndices.size(); j++) {
            int vertexIndex = face.vertexIndices[j];
            Vertex vertex;
            vertex.position = customOBJ->vertices[vertexIndex];
            vertex.normal = customOBJ->normals[face.normalIndices[j]];
            vertex.texCoords = customOBJ->texCoords[face.texCoordIndices[j]];
            vertices.push_back(vertex);
            indices.push_back(vertices.size() - 1); 
        }
    }

    mesh = new Mesh(vertices, indices, material);

}

/*
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh* Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    Material material;

    // Get vertices
    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = QVector3D(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = QVector3D(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            vertex.tangent = QVector3D(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.bitangent = QVector3D(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        } else {
            vertex.texCoords = QVector2D(0.0f, 0.0f); // Default texture coordinates
        }

        vertices.push_back(vertex);
    }

    // Get indices
    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Get materials
    // if (mesh->mMaterialIndex != UINT_MAX) {
    //     aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

    //     aiColor3D color;
    //     if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
    //         material.albedo = QVector3D(color.r, color.g, color.b);
    //     } else {
    //         material.albedo = QVector3D(1.0f, 1.0f, 1.0f); // Default albedo color
    //     }

    //     if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
    //         material.specular = QVector3D(color.r, color.g, color.b);
    //     } else {
    //         material.specular = QVector3D(0.5f, 0.5f, 0.5f); // Default specular color
    //     }

    //     if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_SHININESS, material.shininess)) {
    //         material.shininess = qMax(material.shininess, 32.0f); // Default shininess
    //     } else {
    //         material.shininess = 32.0f;
    //     }

    //     // Default values
    //     auto appendTextures = [&](aiTextureType type, const QString& name) {
    //         std::vector<Texture> textures = LoadMaterialTextures(aiMaterial, type, name);
    //         material.textures.push_back(textures);
    //     };

    //     appendTextures(aiTextureType_DIFFUSE, "texture_diffuse");
    //     appendTextures(aiTextureType_NORMALS, "texture_normal");
    //     appendTextures(aiTextureType_METALNESS, "texture_metallic");
    //     appendTextures(aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
    // }

    return new Mesh(vertices, indices, material);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName)
{
    std::vector<Texture> textures;

    for (uint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        QString path = QString(str.C_Str());
        bool skip = false;

        for (const auto& loadedTexture : m_textures_loaded) {
            if (loadedTexture.path == path) {
                textures.push_back(loadedTexture);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(path, m_directory);
            texture.type = typeName;
            texture.path = path;

            textures.push_back(texture);
            m_textures_loaded.push_back(texture);
        }
    }

    return textures;
}

unsigned int Model::TextureFromFile(QString &path, const QString &directory)
{
    QString filename = QString(path).section('/', -1);
    filename = directory + '/' + filename;

    #ifdef _WIN32
        filename = filename.replace("/", "\\");
    #else
        filename = filename.replace("\\", "/");
    #endif

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.toStdString().c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB; // Default initialization
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path.toStdString() << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
*/

void Model::Update(float deltaTime)
{
    if (!isMovable) return;

    QVector3D velocity = (transform.position - oldPosition);
    oldPosition = transform.position;
    float deltaSquare = deltaTime * deltaTime;
    // QVector3D acceleration = forces * InvMass();
    // velocity += acceleration * deltaTime;
    // velocity *= friction; // à voir !
    
    if (fabsf(velocity.x()) < 0.001f) {
        velocity.setX(0.0f);
    }
    if (fabsf(velocity.y()) < 0.001f) {
        velocity.setY(0.0f);
    }
    if (fabsf(velocity.z()) < 0.001f) {
        velocity.setZ(0.0f);
    }

    // if (type == RIGIDBODY_TYPE_BOX) {
    //     QVector3D angAccel = torques * InvTensor();
    //     angVel += angAccel * deltaTime;
    //     angVel *= friction; // à voir !

    //     if (fabsf(angVel.x()) < 0.001f) {
    //         angVel.setX(0.0f);
    //     }
    //     if (fabsf(angVel.y()) < 0.001f) {
    //         angVel.setY(0.0f);
    //     }
    //     if (fabsf(angVel.z()) < 0.001f) {
    //         angVel.setZ(0.0f);
    //     }
    // }

    transform.position += (velocity * friction + forces * deltaSquare);
    // transform.position += velocity * deltaTime;

    // if (type == RIGIDBODY_TYPE_BOX) {
    //     orientation += angVel * deltaTime;
    //     transform.SetRotationEuler(orientation); // à voir !
    //     boxCollider.orientation = transform.GetRotationMatrix();
    // }

    SynsCollisionVolumes();

}

void Model::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();
    
    shaderProgram->setUniformValue("model", transform.GetModelMatrix());
    shaderProgram->setUniformValue("color", color);
    mesh->Render(shaderProgram);
    
    shaderProgram->release();

    // Debug collider
    Rigidbody::Render(shaderProgram);
}

void Model::BuildAABB()
{
    if (mesh == nullptr) return;

    QVector3D minBounds = mesh->m_vertices[0].position;
    QVector3D maxBounds = mesh->m_vertices[0].position;

    for (const Vertex& vertex : mesh->m_vertices)
    {
        minBounds.setX(qMin(minBounds.x(), vertex.position.x()));
        minBounds.setY(qMin(minBounds.y(), vertex.position.y()));
        minBounds.setZ(qMin(minBounds.z(), vertex.position.z()));

        maxBounds.setX(qMax(maxBounds.x(), vertex.position.x()));
        maxBounds.setY(qMax(maxBounds.y(), vertex.position.y()));
        maxBounds.setZ(qMax(maxBounds.z(), vertex.position.z()));
    }
    

    bounds.position = (minBounds + maxBounds) * 0.5f;
    bounds.size = (maxBounds - minBounds) * 0.5f;

}

void Model::SolveConstraints(const std::vector<Rigidbody*>& constraints)
{
    for (Rigidbody* rb : constraints) {
        if (rb == this || !rb->HasVolume()) continue;

        if (type == RIGIDBODY_TYPE_SPHERE && rb->type == RIGIDBODY_TYPE_SPHERE) {
            SolveSphereSphereCollision(sphereCollider, rb->sphereCollider, rb);
        }
        else if (type == RIGIDBODY_TYPE_SPHERE && rb->type == RIGIDBODY_TYPE_BOX) {
            SolveSphereOBBCollision(sphereCollider, rb->boxCollider, rb);
        }
        else if (type == RIGIDBODY_TYPE_BOX && rb->type == RIGIDBODY_TYPE_SPHERE) {
            SolveSphereOBBCollision(rb->sphereCollider, boxCollider, this);
        }
        else if (type == RIGIDBODY_TYPE_BOX && rb->type == RIGIDBODY_TYPE_BOX) {
            SolveOBBOBBCollision(boxCollider, rb->boxCollider, rb);
        }
    }
}
