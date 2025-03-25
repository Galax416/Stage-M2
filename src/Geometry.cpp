#include "Geometry.h"

QVector3D GetMin(const AABB& aabb)
{
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fminf(p1.x(), p2.x()), fminf(p1.y(), p2.y()), fminf(p1.z(), p2.z()));
}

QVector3D GetMax(const AABB& aabb) 
{
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fmaxf(p1.x(), p2.x()), fmaxf(p1.y(), p2.y()), fmaxf(p1.z(), p2.z()));
}

void Render(const Circle& circle) 
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(circle.position[0], circle.position[1]); // position of circle
    for (int i = 0; i <= 360; i++) {
        float theta = i * PI / 180.0f;
        float x = circle.radius * cosf(theta);
        float y = circle.radius * sinf(theta);
        glVertex2f(x + circle.position[0], y + circle.position[1]);
    }
    glEnd();
}

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

void Render(const Sphere& sphere) 
{
	const int slices = 32; 
    const int stacks = 16;

    glPushMatrix();
    glTranslatef(sphere.position[0], sphere.position[1], sphere.position[2]);

    for (int i = 0; i < stacks; i++) {
        float lat0 = PI * (-0.5f + (float)(i) / stacks);
        float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);

        float y0 = sinf(lat0);
        float y1 = sinf(lat1);
        float r0 = cosf(lat0);
        float r1 = cosf(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2.0f * PI * (float)(j) / slices;
            float x = cosf(lng);
            float z = sinf(lng);

            glNormal3f(x * r0, y0, z * r0);
            glVertex3f(sphere.radius * x * r0, sphere.radius * y0, sphere.radius * z * r0);

            glNormal3f(x * r1, y1, z * r1);
            glVertex3f(sphere.radius * x * r1, sphere.radius * y1, sphere.radius * z * r1);
        }
        glEnd();
    }

    glPopMatrix();
}

void Render(const Plane& plane) 
{

}





