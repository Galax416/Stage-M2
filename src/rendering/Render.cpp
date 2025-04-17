#include "Render.h"

void Render(const Line& line) 
{
	// glBegin(GL_LINES);
	// glVertex3f(line.start[0], line.start[1], line.start[2]);
	// glVertex3f(line.end[0], line.end[1], line.end[2]);
	// glEnd();
    int segments = 16; // Number of segments for the cylinder
    float radius = 0.003f; // Radius of the cylinder

    QVector3D start = line.start;
    QVector3D end = line.end;

    QVector3D axis = (end - start).normalized();
    float height = (end - start).length();

    // Trouver un vecteur perpendiculaire à l'axe pour construire le cercle de base
    QVector3D up = std::fabs(QVector3D::dotProduct(axis, QVector3D(0, 1, 0))) < 0.99f ? QVector3D(0, 1, 0) : QVector3D(1, 0, 0);
    QVector3D side1 = QVector3D::crossProduct(axis, up).normalized();
    QVector3D side2 = QVector3D::crossProduct(axis, side1).normalized();

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; ++i)
    {
        float theta1 = (2.0f * M_PI * i) / segments;
        float theta2 = (2.0f * M_PI * (i + 1)) / segments;

        // Points sur le cercle de base
        QVector3D p1 = start + (side1 * cos(theta1) + side2 * sin(theta1)) * radius;
        QVector3D p2 = start + (side1 * cos(theta2) + side2 * sin(theta2)) * radius;
        QVector3D p3 = p1 + axis * height;
        QVector3D p4 = p2 + axis * height;

        // Deux triangles pour former le quad latéral
        glVertex3f(p1.x(), p1.y(), p1.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
        glVertex3f(p3.x(), p3.y(), p3.z());

        glVertex3f(p3.x(), p3.y(), p3.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
        glVertex3f(p4.x(), p4.y(), p4.z());
    }
    glEnd();
}

void Render(const Ray& ray) 
{
	Line line;
	line.start = ray.origin;
	line.end = ray.origin + ray.direction * 50000.0f;
	glBegin(GL_LINES);
	glVertex3f(line.start[0], line.start[1], line.start[2]);
	glVertex3f(line.end[0], line.end[1], line.end[2]);
	glEnd();
}

void Render(const AABB& aabb) 
{
    QVector3D center = aabb.position;
    QVector3D halfSize = aabb.size;

    // Vertex of the cube
    QVector3D vertices[8] = {
        center + QVector3D(-halfSize.x(), -halfSize.y(), -halfSize.z()),
        center + QVector3D( halfSize.x(), -halfSize.y(), -halfSize.z()),
        center + QVector3D( halfSize.x(),  halfSize.y(), -halfSize.z()),
        center + QVector3D(-halfSize.x(),  halfSize.y(), -halfSize.z()),
        center + QVector3D(-halfSize.x(), -halfSize.y(),  halfSize.z()),
        center + QVector3D( halfSize.x(), -halfSize.y(),  halfSize.z()),
        center + QVector3D( halfSize.x(),  halfSize.y(),  halfSize.z()),
        center + QVector3D(-halfSize.x(),  halfSize.y(),  halfSize.z())
    };

    unsigned int indices[24] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // Front face
        4, 5, 5, 6, 6, 7, 7, 4,  // Back face
        0, 4, 1, 5, 2, 6, 3, 7   // Faces connections
    };

    glBegin(GL_LINES);
    glLineWidth(10.0f);
    for (int i = 0; i < 24; i += 2) {
        glVertex3f(vertices[indices[i]].x(), vertices[indices[i]].y(), vertices[indices[i]].z());
        glVertex3f(vertices[indices[i + 1]].x(), vertices[indices[i + 1]].y(), vertices[indices[i + 1]].z());
    }
    glEnd();
}

void Render(const OBB& obb) 
{
    AABB aabb;
    aabb.size = obb.size;
    Render(aabb);
}

void Render(const SphereCollider& sphere) 
{
    // Dessiner le cercle dans le plan XY
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, 0); // position of circle
    for (int i = 0; i <= 360; i++) {
        float theta = i * M_PI / 180.0f;
        float x = cosf(theta);
        float y = sinf(theta);
        glVertex3f(x, y, 0);
    }
    glEnd();

    // Dessiner le cercle dans le plan XZ
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, 0); // position of circle
    for (int i = 0; i <= 360; i++) {
        float theta = i * M_PI / 180.0f;
        float x = cosf(theta);
        float z = sinf(theta);
        glVertex3f(x , 0, z);
    }
    glEnd();
}

