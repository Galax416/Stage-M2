#include "Collision.h"

QVector3D GetMin(const AABB& aabb) {
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fminf(p1.x(), p2.x()), fminf(p1.y(), p2.y()), fminf(p1.z(), p2.z()));
}
QVector3D GetMax(const AABB& aabb) {
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fmaxf(p1.x(), p2.x()), fmaxf(p1.y(), p2.y()), fmaxf(p1.z(), p2.z()));
}

AABB FromMinMax(const QVector3D& min, const QVector3D& max) {
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

OBB AABBToOBB(const AABB& aabb)
{
    OBB obb;
    obb.position = aabb.position;
    obb.size = aabb.size;
    return obb;
}

void ResetCollisionManifold(CollisionManifold* result) {
    result->colliding = false;
    result->normal = QVector3D(0, 0, 0);
    result->depth = FLT_MAX;
    result->contacts.clear();
}


void Render(const SphereCollider& sphere) 
{
    // Dessiner le cercle dans le plan XY
    glBegin(GL_LINE_LOOP);
    glLineWidth(10.0f);
    glVertex3f(0, 0, 0); // position of circle
    for (int i = 0; i <= 360; i++) {
        float theta = i * PI / 180.0f;
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
        float theta = i * PI / 180.0f;
        float x = cosf(theta);
        float z = sinf(theta);
        glVertex3f(x , 0, z);
    }
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
    aabb.position = obb.position;
    aabb.size = obb.size;
    Render(aabb);
}

