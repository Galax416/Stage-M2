#include "Geometry3D.h"
#include "Mesh.h"

// PLANE
Plane::Plane() : Model()
{
    SetUpPlane();
    SetUpColliders();
}
Plane::Plane(const QVector3D& p, const QVector3D& n) : Model()
{
    SetPosition(p);
    SetUpPlane(n);
    SetUpColliders();
}
Plane::Plane(const QVector3D& p, const QVector3D& n, QColor c) : Model()
{
    SetPosition(p);
    color = c;
    SetUpPlane(n);
    SetUpColliders();
}
void Plane::SetUpPlane(const QVector3D& n)
{
    // Normalize the normal vector
    normal = n.normalized();
    // Set the distance from the plane to the origin
    distance = -QVector3D::dotProduct(normal, transform.position);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    material.albedo = QVector3D(color.redF(), color.greenF(), color.blueF());
    qDebug() << "Plane color:" << color.redF() << color.greenF() << color.blueF();
    qDebug() << material.albedo;
    qDebug() << color;

    Vertex vertex1;
    vertex1.position = QVector3D(-1000.0f, 0.0f, -1000.0f);
    vertex1.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
    vertex1.texCoords = QVector2D(0.0f, 0.0f);

    Vertex vertex2;
    vertex2.position = QVector3D(1000.0f, 0.0f, -1000.0f);
    vertex2.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
    vertex2.texCoords = QVector2D(1.0f, 0.0f);

    Vertex vertex3;
    vertex3.position = QVector3D(1000.0f, 0.0f, 1000.0f);
    vertex3.normal = QVector3D(0.0f, 1.0f, 0.0f); // World up vector
    vertex3.texCoords = QVector2D(1.0f, 1.0f);

    Vertex vertex4;
    vertex4.position = QVector3D(-1000.0f, 0.0f, 1000.0f);
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

    // Create a mesh
    mesh = new Mesh(vertices, indices, material);

    // Align the plane to the normal
    transform.rotation = QQuaternion::rotationTo(QVector3D(0.0f, 1.0f, 0.0f), normal);
    transform.rotationEuler = transform.rotation.toEulerAngles();

    SetUpColliders();
}

// SPHERE
Sphere::Sphere() : Model()
{
    radius = 1.0f;
    SetUpSphere();
    SetUpColliders();
}
Sphere::Sphere(const QVector3D& p, float r) : Model()
{
    radius = r;
    transform.scale = QVector3D(r, r, r);
    SetPosition(p);
    SetUpSphere();
    SetUpColliders();
}
Sphere::Sphere(const QVector3D& p, float r, QColor c) : Model()
{
    radius = r;
    transform.scale = QVector3D(r, r, r);
    SetPosition(p);
    color = c;
    SetUpSphere();
    SetUpColliders();
}
void Sphere::SetUpSphere()
{   
    // Change type to sphere
    type = RIGIDBODY_TYPE_SPHERE;

    // Define the number of slices and stacks for the sphere
    const int slices = 32; 
    const int stacks = 16;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    material.albedo = QVector3D(color.redF(), color.greenF(), color.blueF());

    // Vertices
    for (int i = 0; i <= stacks; ++i) 
    {
        float v = (float)i / stacks; 
        float phi = M_PI * (-0.5f + v);  // Latitude angle (-pi/2 to pi/2)

        for (int j = 0; j <= slices; ++j) 
        {
            float u = (float)j / slices;  
            float theta = 2.0f * M_PI * u;  // Longitude angle (0 to 2pi)

            // Compute cartesian coordinates
            float x = cosf(theta) * cosf(phi);
            float y = sinf(phi);
            float z = sinf(theta) * cosf(phi);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);             // Vertex position
            vertex.normal = QVector3D(x, y, z).normalized();  // Normal
            vertex.texCoords = QVector2D(u, v);               // UV Cordinate

            vertices.push_back(vertex);
        }
    }

    // Indices
    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // Create a mesh
    mesh = new Mesh(vertices, indices, material);

    SetUpColliders();
}