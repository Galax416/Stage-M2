#include "Geometry.h"

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

}

void Render(const AABB& aabb) 
{

}

void Render(const OBB& obb) 
{

}


