#include "Geometry3D.h"
#include "Mesh.h"

// PLANE
std::shared_ptr<Mesh> Plane::GetSharedPlaneMesh()
{
    static std::shared_ptr<Mesh> sharedPlaneMesh = nullptr;

    if (!sharedPlaneMesh)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        Vertex vertex1;
        vertex1.position = QVector3D(-10.0f, 0.0f, -10.0f);
        vertex1.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
        vertex1.texCoords = QVector2D(0.0f, 0.0f);

        Vertex vertex2;
        vertex2.position = QVector3D(10.0f, 0.0f, -10.0f);
        vertex2.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
        vertex2.texCoords = QVector2D(1.0f, 0.0f);

        Vertex vertex3;
        vertex3.position = QVector3D(10.0f, 0.0f, 10.0f);
        vertex3.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
        vertex3.texCoords = QVector2D(1.0f, 1.0f);

        Vertex vertex4;
        vertex4.position = QVector3D(-10.0f, 0.0f, 10.0f);
        vertex4.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
        vertex4.texCoords = QVector2D(0.0f, 1.0f);

        vertices.push_back(vertex1);
        vertices.push_back(vertex2);
        vertices.push_back(vertex3);
        vertices.push_back(vertex4);

        indices = {
            0, 3, 2,
            2, 1, 0
        };

        sharedPlaneMesh = std::make_shared<Mesh>(vertices, indices, material);
    }

    return sharedPlaneMesh;
}

// BOX
std::shared_ptr<Mesh> Box::GetSharedBoxMesh()
{
    static std::shared_ptr<Mesh> sharedBoxMesh = nullptr;

    if (!sharedBoxMesh)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        QVector3D positions[8] = {
            {-1.0f, -1.0f, -1.0f}, // 0
            { 1.0f, -1.0f, -1.0f}, // 1
            { 1.0f,  1.0f, -1.0f}, // 2
            {-1.0f,  1.0f, -1.0f}, // 3
            {-1.0f, -1.0f,  1.0f}, // 4
            { 1.0f, -1.0f,  1.0f}, // 5
            { 1.0f,  1.0f,  1.0f}, // 6
            {-1.0f,  1.0f,  1.0f}  // 7
        };

        auto addFace = [&](int i0, int i1, int i2, int i3, QVector3D normal) {
            unsigned int startIndex = vertices.size();
            //                       position, normal,    texCoords,     tangent,   bitangent
            vertices.push_back({positions[i0], normal, {0.0f, 0.0f}, QVector3D(), QVector3D()});
            vertices.push_back({positions[i1], normal, {1.0f, 0.0f}, QVector3D(), QVector3D()});
            vertices.push_back({positions[i2], normal, {1.0f, 1.0f}, QVector3D(), QVector3D()});
            vertices.push_back({positions[i3], normal, {0.0f, 1.0f}, QVector3D(), QVector3D()});

            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 0);
        };

        addFace(1, 0, 3, 2, {0.0f, 0.0f, -1.0f}); // back
        addFace(4, 5, 6, 7, {0.0f, 0.0f, 1.0f});  // front
        addFace(0, 4, 7, 3, {-1.0f, 0.0f, 0.0f}); // left
        addFace(5, 1, 2, 6, {1.0f, 0.0f, 0.0f});  // right
        addFace(2, 3, 7, 6, {0.0f, 1.0f, 0.0f});  // top
        addFace(5, 4, 0, 1, {0.0f, -1.0f, 0.0f}); // bottom

        sharedBoxMesh = std::make_shared<Mesh>(vertices, indices, material);
    }

    return sharedBoxMesh;
}

// SPHERE
std::shared_ptr<Mesh> Sphere::GetSharedSphereMesh()
{
    static std::shared_ptr<Mesh> sharedSphereMesh = nullptr;

    if (!sharedSphereMesh)
    {
        const int slices = 9;
        const int stacks = 5;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        for (int i = 0; i <= stacks; ++i)
        {
            float v = (float)i / stacks;
            float phi = M_PI * (-0.5f + v);

            for (int j = 0; j <= slices; ++j)
            {
                float u = (float)j / slices;
                float theta = 2.0f * M_PI * u;

                float x = cosf(theta) * cosf(phi);
                float y = sinf(phi);
                float z = sinf(theta) * cosf(phi);

                Vertex vertex;
                vertex.position = QVector3D(x, y, z);
                vertex.normal = QVector3D(x, y, z).normalized();
                vertex.texCoords = QVector2D(u, v);

                vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                int first = (i * (slices + 1)) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        sharedSphereMesh = std::make_shared<Mesh>(vertices, indices, material);
    }

    return sharedSphereMesh;
}



// PLANE
Plane::Plane() : Model()
{
    SetUpPlane();
}
Plane::Plane(const QVector3D& p, const QVector3D& n) : Model()
{
    SetPosition(p);
    SetUpPlane(n);
}
Plane::Plane(const QVector3D& p, const QVector3D& n, QColor c) : Model()
{
    SetPosition(p);
    color = c;
    SetUpPlane(n);
}
void Plane::SetUpPlane(const QVector3D& n)
{
    // Change type to box
    type = RIGIDBODY_TYPE_BOX;

    // Normalize the normal vector
    normal = n.normalized();
    // Set the distance from the plane to the origin
    distance = -QVector3D::dotProduct(normal, transform.position);

    mesh = GetSharedPlaneMesh();

    // Align the plane to the normal
    transform.rotation = QQuaternion::rotationTo(QVector3D(0.0f, 1.0f, 0.0f), normal);
    transform.rotationEuler = transform.rotation.toEulerAngles();

    SetUpColliders();
}

// BOX
Box::Box() : Model()
{
    SetUpBox();
}
Box::Box(const QVector3D& p, const QVector3D& s) : Model()
{
    transform.scale = s;
    SetPosition(p);
    SetUpBox();
}
Box::Box(const QVector3D& p, const QVector3D& s, QColor c) : Model()
{
    transform.scale = s;
    SetPosition(p);
    color = c;
    SetUpBox();
}
void Box::SetUpBox()
{
    // Change type to box
    type = RIGIDBODY_TYPE_BOX;

    mesh = GetSharedBoxMesh();

    SetUpColliders();
}

// SPHERE
Sphere::Sphere() : Model()
{
    radius = 1.0f;
    SetUpSphere();
}
Sphere::Sphere(const QVector3D& p, float r) : Model()
{
    radius = r;
    transform.scale = QVector3D(r, r, r);
    SetPosition(p);
    SetUpSphere();
}
Sphere::Sphere(const QVector3D& p, float r, QColor c) : Model()
{
    radius = r;
    transform.scale = QVector3D(r, r, r);
    SetPosition(p);
    color = c;
    SetUpSphere();
}
void Sphere::SetUpSphere()
{   
    // Change type to sphere
    type = RIGIDBODY_TYPE_SPHERE;

    mesh = GetSharedSphereMesh();

    SetUpColliders();
}