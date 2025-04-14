#include "Render.h"

void Render(const Line& line) 
{
	glBegin(GL_LINES);
    glLineWidth(10.0f);
	glVertex3f(line.start[0], line.start[1], line.start[2]);
	glVertex3f(line.end[0], line.end[1], line.end[2]);
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
    glLineWidth(10.0f);
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
    glLineWidth(10.0f);
    glVertex3f(0, 0, 0); // position of circle
    for (int i = 0; i <= 360; i++) {
        float theta = i * M_PI / 180.0f;
        float x = cosf(theta);
        float z = sinf(theta);
        glVertex3f(x , 0, z);
    }
    glEnd();
}

