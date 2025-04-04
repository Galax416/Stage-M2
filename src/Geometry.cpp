#include "Geometry.h"

#define CMP(x, y) \
	(fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

void Sphere::SetUpSphere()
{
	Init();
    type = RIGIDBODY_TYPE_SPHERE;

    const int slices = 32; 
    const int stacks = 16;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    // Génération des sommets
    for (int i = 0; i <= stacks; ++i) 
    {
        float v = (float)i / stacks; 
        float phi = M_PI * (-0.5f + v);  // Angle de latitude (-pi/2 à pi/2)

        for (int j = 0; j <= slices; ++j) 
        {
            float u = (float)j / slices;  
            float theta = 2.0f * M_PI * u;  // Angle de longitude (0 à 2pi)

            // Calcul des coordonnées cartésiennes
            float x = cosf(theta) * cosf(phi);
            float y = sinf(phi);
            float z = sinf(theta) * cosf(phi);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);  // Position du sommet
            vertex.normal = QVector3D(x, y, z).normalized();           // Normale
            vertex.texCoords = QVector2D(u, v);                        // Coordonnée UV

            vertices.push_back(vertex);
        }
    }

    // Génération des indices
    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            // Premier triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Deuxième triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // Create a mesh
    mesh = new Mesh(vertices, indices, material);

    SetUpColliders();

}

void Plane::SetUpPlane()
{
	Init();
    type = RIGIDBODY_TYPE_BOX;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    Vertex vertex1;
    vertex1.position = QVector3D(-1000.0f, 0.0f, -1000.0f);
    vertex1.normal = WORLD_UP;
    vertex1.texCoords = QVector2D(0.0f, 0.0f);

    Vertex vertex2;
    vertex2.position = QVector3D(1000.0f, 0.0f, -1000.0f);
    vertex2.normal = WORLD_UP;
    vertex2.texCoords = QVector2D(1.0f, 0.0f);

    Vertex vertex3;
    vertex3.position = QVector3D(1000.0f, 0.0f, 1000.0f);
    vertex3.normal = WORLD_UP;
    vertex3.texCoords = QVector2D(1.0f, 1.0f);

    Vertex vertex4;
    vertex4.position = QVector3D(-1000.0f, 0.0f, 1000.0f);
    vertex4.normal = WORLD_UP;
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
    transform.rotation = QQuaternion::rotationTo(WORLD_UP, normal);
    transform.rotationEuler = transform.rotation.toEulerAngles();

    SetUpColliders();

}

/*
// Functions
Plane FromTriangle(const Triangle& t) 
{
	Plane result;
	result.normal = (QVector3D::crossProduct(t.b - t.a, t.c - t.a)).normalized();
	result.distance = QVector3D::dotProduct(result.normal, t.a);
	return result;
}

QVector3D Project(const QVector3D& length, const QVector3D& direction) 
{
	float dot = QVector3D::dotProduct(length, direction);
	float magSq = QVector3D::dotProduct(direction, direction);
	return direction * (dot / magSq);
}

QVector3D Barycentric(const Point& p, const Triangle& t) 
{
	QVector3D ap = p - t.a;
	QVector3D bp = p - t.b;
	QVector3D cp = p - t.c;

	QVector3D ab = t.b - t.a;
	QVector3D ac = t.c - t.a;
	QVector3D bc = t.c - t.b;
	QVector3D cb = t.b - t.c;
	QVector3D ca = t.a - t.c;

	QVector3D v = ab - Project(ab, cb);
	float a = 1.0f - (QVector3D::dotProduct(v, ap) / QVector3D::dotProduct(v, ab));

	v = bc - Project(bc, ac);
	float b = 1.0f - (QVector3D::dotProduct(v, bp) / QVector3D::dotProduct(v, bc));

	v = ca - Project(ca, ab);
	float c = 1.0f - (QVector3D::dotProduct(v, cp) / QVector3D::dotProduct(v, ca));

	return QVector3D(a, b, c);
}


// Collision Detection
bool SphereSphere(const Sphere& s1, const Sphere& s2) 
{
	float radiiSum = s1.transform.scale.x() + s2.transform.scale.x();
	float sqDistance = (s1.transform.position - s2.transform.position).lengthSquared();
	return sqDistance < radiiSum * radiiSum;
}

bool PointInOBB(const Point& point, const OBB& obb) {
	QVector3D dir = point - obb.position;

	for (int i = 0; i < 3; ++i) {
		const float* orientation = &obb.orientation.data()[i * 3];
		QVector3D axis(orientation[0], orientation[1], orientation[2]);

		float distance = QVector3D::dotProduct(dir, axis);

		if (distance > obb.size[i]) {
			return false;
		}
		if (distance < -obb.size[i]) {
			return false;
		}
	}

	return true;
}

std::vector<Point> GetVertices(const OBB& obb) {
	std::vector<QVector3D> v;
	v.resize(8);

	QVector3D C = obb.position;	// OBB Center
	QVector3D E = obb.size;		// OBB Extents
	const float* o = obb.orientation.data();
	QVector3D A[] = {			// OBB Axis
		QVector3D(o[0], o[1], o[2]),
		QVector3D(o[3], o[4], o[5]),
		QVector3D(o[6], o[7], o[8]),
	};

	v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	return v;
}

std::vector<Line> GetEdges(const OBB& obb) {
	std::vector<Line> result;
	result.reserve(12);
	std::vector<Point> v = GetVertices(obb);

	int index[][2] = { // Indices of edges
		{ 6, 1 },{ 6, 3 },{ 6, 4 },{ 2, 7 },{ 2, 5 },{ 2, 0 },
		{ 0, 1 },{ 0, 3 },{ 7, 1 },{ 7, 4 },{ 4, 5 },{ 5, 3 }
	};

	for (int j = 0; j < 12; ++j) {
		result.push_back(Line(
			v[index[j][0]], v[index[j][1]]
		));
	}

	return result;
}

std::vector<Plane> GetPlanes(const OBB& obb) 
{
	QVector3D c = obb.position;	// OBB Center
	QVector3D e = obb.size;		// OBB Extents
	const float* o = obb.orientation.data();
	QVector3D a[] = {			// OBB Axis
		QVector3D(o[0], o[1], o[2]),
		QVector3D(o[3], o[4], o[5]),
		QVector3D(o[6], o[7], o[8]),
	};

	std::vector<Plane> result;
	result.resize(6);

	result[0] = Plane(a[0]        ,  QVector3D::dotProduct(a[0], (c + a[0] * e.x())));
	result[1] = Plane(a[0] * -1.0f, -QVector3D::dotProduct(a[0], (c - a[0] * e.x())));
	result[2] = Plane(a[1]        ,  QVector3D::dotProduct(a[1], (c + a[1] * e.y())));
	result[3] = Plane(a[1] * -1.0f, -QVector3D::dotProduct(a[1], (c - a[1] * e.y())));
	result[4] = Plane(a[2]        ,  QVector3D::dotProduct(a[2], (c + a[2] * e.z())));
	result[5] = Plane(a[2] * -1.0f, -QVector3D::dotProduct(a[2], (c - a[2] * e.z())));

	return result;
}

bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint) {
    QVector3D ab = line.end - line.start;
    float nA = QVector3D::dotProduct(plane.normal, line.start) - plane.distance;
    float nAB = QVector3D::dotProduct(plane.normal, ab);

    if (qFuzzyIsNull(nAB)) {
        return false; // Line is parallel to the plane
    }

    float t = -nA / nAB;
    if (t >= 0.0f && t <= 1.0f) {
        *outPoint = line.start + t * ab;
        return true;
    }

    return false;
}

std::vector<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb) {
	std::vector<Point> result;
	result.reserve(edges.size() * 3);
	Point intersection;

	const std::vector<Plane> planes = GetPlanes(obb);

	for (long unsigned int i = 0; i < planes.size(); ++i) {
		for (long unsigned int j = 0; j < edges.size(); ++j) {
			if (ClipToPlane(planes[i], edges[j], &intersection)) {
				if (PointInOBB(intersection, obb)) {
					result.push_back(intersection);
				}
			}
		}
	}

	return result;
}

float PenetrationDepth(const OBB& o1, const OBB& o2, const QVector3D& axis, bool* outShouldFlip) 
{
	Interval i1 = GetInterval(o1, axis.normalized());
	Interval i2 = GetInterval(o2, axis.normalized());

	if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
		return 0.0f; // No penerattion
	}

	float len1 = i1.max - i1.min;
	float len2 = i2.max - i2.min;
	float min = fminf(i1.min, i2.min);
	float max = fmaxf(i1.max, i2.max);
	float length = max - min;

	if (outShouldFlip != 0) {
		*outShouldFlip = (i2.min < i1.min);
	}

	return (len1 + len2) - length;
}

Point ClosestPoint(const OBB& obb, const Point& point) 
{
	Point result = obb.position;
	QVector3D dir = point - obb.position;

	for (int i = 0; i < 3; ++i) {
		const float* orientation = &obb.orientation.data()[i * 3];
		QVector3D axis(orientation[0], orientation[1], orientation[2]);

		float distance = QVector3D::dotProduct(dir, axis);

		if (distance > obb.size[i]) {
			distance = obb.size[i];
		}
		if (distance < -obb.size[i]) {
			distance = -obb.size[i];
		}

		result = result + (axis * distance);
	}

	return result;
}

CollisionManifold FindCollisionFeatures(const OBB& A, const OBB& B) 
{
	CollisionManifold result; // Will return result of intersection!
	ResetCollisionManifold(&result);

	Sphere s1(A.position, (A.size).length());
	Sphere s2(B.position, (B.size).length());

	if (!SphereSphere(s1, s2)) {
		return result;
	}

	const float* o1 = A.orientation.data();
	const float* o2 = B.orientation.data();

	QVector3D test[15] = {
		QVector3D(o1[0], o1[1], o1[2]),
		QVector3D(o1[3], o1[4], o1[5]),
		QVector3D(o1[6], o1[7], o1[8]),
		QVector3D(o2[0], o2[1], o2[2]),
		QVector3D(o2[3], o2[4], o2[5]),
		QVector3D(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
		test[6 + i * 3 + 0] = QVector3D::crossProduct(test[i], test[0]);
		test[6 + i * 3 + 1] = QVector3D::crossProduct(test[i], test[1]);
		test[6 + i * 3 + 2] = QVector3D::crossProduct(test[i], test[2]);
	}

	QVector3D* hitNormal = 0;
	bool shouldFlip;

	for (int i = 0; i < 15; ++i) {
		if (test[i].x() < 0.000001f) test[i][0] = 0.0f;
		if (test[i].y() < 0.000001f) test[i][1] = 0.0f;
		if (test[i].z() < 0.000001f) test[i][2] = 0.0f;
		if ((test[i]).lengthSquared() < 0.001f) {
			continue;
		}

		float depth = PenetrationDepth(A, B, test[i], &shouldFlip);
		if (depth <= 0.0f) {
			return result;
		}
		else if (depth < result.depth) {
			if (shouldFlip) {
				test[i] = test[i] * -1.0f;
			}
			result.depth = depth;
			hitNormal = &test[i];
		}
	}

	if (hitNormal == 0) {
		return result;
	}
	QVector3D axis = (*hitNormal).normalized();

	std::vector<Point> c1 = ClipEdgesToOBB(GetEdges(B), A);
	std::vector<Point> c2 = ClipEdgesToOBB(GetEdges(A), B);
	result.contacts.reserve(c1.size() + c2.size());
	result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
	result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

	Interval i = GetInterval(A, axis);
	float distance = (i.max - i.min)* 0.5f - result.depth * 0.5f;
	QVector3D pointOnPlane = A.position + axis * distance;
	
	for (int i = result.contacts.size() - 1; i >= 0; --i) {
		QVector3D contact = result.contacts[i];
		result.contacts[i] = contact + (axis * QVector3D::dotProduct(axis, pointOnPlane - contact));
		
		// This bit is in the "There is more" section of the book
		for (int j = result.contacts.size() - 1; j > i; --j) {
			if ((result.contacts[j] - result.contacts[i]).lengthSquared() < 0.0001f) {
				result.contacts.erase(result.contacts.begin() + j);
				break;
			}
		}
	}

	result.colliding = true;
	result.normal = axis;

	return result;
}

CollisionManifold FindCollisionFeatures(const Sphere& A, const Sphere& B) {
	CollisionManifold result; // Will return result of intersection!
	ResetCollisionManifold(&result);

	float r = A.transform.scale.x() + B.transform.scale.x();
	QVector3D d = B.transform.position - A.transform.position;

	if ((d).lengthSquared() - r * r > 0 || (d).lengthSquared() == 0.0f) {
		return result;
	}
	d.normalize();

	result.colliding = true;
	result.normal = d;
	result.depth = fabsf((d).length() - r) * 0.5f;
	
	// dtp - Distance to intersection point
	float dtp = A.transform.scale.x() - result.depth;
	Point contact = A.transform.position + d * dtp;
	
	result.contacts.push_back(contact);

	return result;
}

CollisionManifold FindCollisionFeatures(const OBB& A, const Sphere& B) {
	CollisionManifold result; // Will return result of intersection!
	ResetCollisionManifold(&result);

	Point closestPoint = ClosestPoint(A, B.transform.position);

	float distanceSq = (closestPoint - B.transform.position).lengthSquared();
	if (distanceSq > B.transform.scale.x() * B.transform.scale.x()) {
		return result;
	}

	QVector3D normal; 
	if (CMP(distanceSq, 0.0f)) {
		if (CMP((closestPoint - A.position).lengthSquared(), 0.0f)) {
			return result;

		}
		// Closest point is at the center of the sphere
		normal = (closestPoint - A.position).normalized();
	}
	else {
		normal = (B.transform.position - closestPoint).normalized();
	}

	Point outsidePoint = B.transform.position - normal * B.transform.scale.x();

	float distance = (closestPoint - outsidePoint).length();

	result.colliding = true;
	result.contacts.push_back(closestPoint + (outsidePoint - closestPoint) * 0.5f);
	result.normal = normal;
	result.depth = distance * 0.5f;

	return result;
}


// Raycasting
void ResetRaycastResult(RaycastResult* result) 
{
    result->point = QVector3D();
    result->normal = QVector3D();
    result->t = 0.0f;
    result->hit = false;
}

bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult) 
{
	ResetRaycastResult(outResult);

	float nd = QVector3D::dotProduct(ray.direction, plane.normal);
	float pn = QVector3D::dotProduct(ray.origin, plane.normal);

	// nd must be negative, and not 0
	// if nd is positive, the ray and plane normals
	// point in the same direction. No intersection.
	if (nd >= 0.0f) {
		return false;
	}

	float t = (plane.distance - pn) / nd;

	// t must be positive
	if (t >= 0.0f) {
		if (outResult != 0) {
			outResult->t = t;
			outResult->hit = true;
			outResult->point = ray.origin + ray.direction * t;
			outResult->normal = (plane.normal).normalized();
		}
		return true;
	}

	return false;
}

bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult) 
{
	ResetRaycastResult(outResult);
	Plane plane = FromTriangle(triangle);

	RaycastResult planeResult;
	if (!Raycast(plane, ray, &planeResult)) {
		return false;
	}
	float t = planeResult.t;

	Point result = ray.origin + ray.direction * t;
	
	QVector3D barycentric = Barycentric(result, triangle);
	if (barycentric.x() >= 0.0f && barycentric.x() <= 1.0f &&
		barycentric.y() >= 0.0f && barycentric.y() <= 1.0f &&
		barycentric.z() >= 0.0f && barycentric.z() <= 1.0f) {

		if (outResult != 0) {
			outResult->t = t;
			outResult->hit = true;
			outResult->point = ray.origin + ray.direction * t;
			outResult->normal = plane.normal;
		}

		return true;
	}

	return false;
}

// Rendering
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
*/

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







