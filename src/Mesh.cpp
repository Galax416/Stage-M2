#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) :
    m_vertices(vertices), m_indices(indices), m_material(material),
    VAO(new QOpenGLVertexArrayObject), VBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)), IBO(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
    initializeOpenGLFunctions();

    VAO->create();
    VBO->create();
    IBO->create();

	// accelerator = nullptr;

	UnifySharedVertices();
    SetUpMesh();
    // ComputeTriangleList();
    // BuildBVH();

}

Mesh::~Mesh()
{
    if (VAO) VAO->destroy();
    if (VBO) VBO->destroy();
    if (IBO) IBO->destroy();

	// if (accelerator) {
	// 	delete accelerator;
	// 	accelerator = nullptr;
	// }
}

void Mesh::SetUpMesh()
{    
    VAO->bind();
    
    VBO->bind();
    VBO->allocate(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    IBO->bind();
    IBO->allocate(m_indices.data(), m_indices.size() * sizeof(unsigned int));
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // Normal
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)); // TexCoords
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)); // Tangent
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent)); // Bitangent
    
    VBO->release();
    IBO->release();
    VAO->release();
    
}

/*void Mesh::ComputeTriangleList()
{
    for (long unsigned int i = 0; i < m_indices.size(); i += 3) {
        Triangle tri;
        tri.a = m_vertices[m_indices[i]].position;
        tri.b = m_vertices[m_indices[i + 1]].position;
        tri.c = m_vertices[m_indices[i + 2]].position;
        m_triangles.push_back(tri);
    }
}*/

void Mesh::Render(QOpenGLShaderProgram* shaderProgram)
{
    #if 0 // For debugging
    if (!shaderProgram || !shaderProgram->isLinked()) {
        qDebug() << "Shader program is null or not linked!";
        return;
    }
    int notCreatedCount = 0;
    if (!VAO || !VAO->isCreated()) {
        qDebug() << "VAO is not properly created!";
        notCreatedCount++;
    }
    if (!VBO || !VBO->isCreated()) {
        qDebug() << "VBO is not properly created!";
        notCreatedCount++;
    }
    if (!IBO || !IBO->isCreated()) {
        qDebug() << "IBO is not properly created!";
        notCreatedCount++;
    }
    if (notCreatedCount == 3) {
        qDebug() << "All VAO, VBO, and IBO are not properly created!";
        return;
    }
    if (m_vertices.empty()) {
        qDebug() << "Error: m_vertices is empty before VBO allocation!";
        return;
    }
    if (m_indices.empty()) {
        qDebug() << "Error: m_indices is empty before rendering!";
        return;
    }
    for (int index : m_indices) {
        if (index >= m_vertices.size()) {
            qDebug() << "Error: Index out of bounds! index=" << index << " max=" << m_vertices.size();
            return;
        }
    }
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qDebug() << "Error: No active OpenGL context!";
        return;
    }
    #endif
    shaderProgram->bind();

    VAO->bind();
    VBO->bind();
    IBO->bind();

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

    VBO->release();
    IBO->release();
    VAO->release();

    shaderProgram->release();
}

void Mesh::ComputeNormals()
{

	// Reset normals to zero
	for (auto& vertex : m_vertices) {
		vertex.normal = QVector3D(0.0f, 0.0f, 0.0f);
	}

	// Compute normals for each triangle and accumulate them in the vertices	
	for (long unsigned int i = 0; i < m_indices.size(); i += 3) {
		unsigned int index1 = m_indices[i];
		unsigned int index2 = m_indices[i + 1];
		unsigned int index3 = m_indices[i + 2];

		QVector3D edge1 = m_vertices[index2].position - m_vertices[index1].position;
		QVector3D edge2 = m_vertices[index3].position - m_vertices[index1].position;
		QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();

		m_vertices[index1].normal += normal;
		m_vertices[index2].normal += normal;
		m_vertices[index3].normal += normal;
	}

	for (auto& vertex : m_vertices) {
		vertex.normal.normalize();
	}
}

void Mesh::UnifySharedVertices()
{
	QHash<QVector3D, int> uniqueVertices;
    std::vector<Vertex> newVertices;
    std::vector<unsigned int> newIndices;

	for (long unsigned int i = 0; i < m_indices.size(); ++i) {
        const QVector3D& position = m_vertices[m_indices[i]].position;

        if (uniqueVertices.contains(position)) {
            newIndices.push_back(uniqueVertices[position]);
        } else {
            int newIndex = newVertices.size();
            uniqueVertices[position] = newIndex;

            newVertices.push_back(m_vertices[m_indices[i]]);
            newIndices.push_back(newIndex);
        }
    }

	m_vertices = newVertices;
    m_indices = newIndices;

	ComputeNormals();

}























/*
void Mesh::BuildBVH()
{
    // AccelerateMesh(*this);
}

void AccelerateMesh(Mesh& mesh) 
{
	if (mesh.accelerator != 0) {
		return;
	}

	QVector3D min = mesh.m_vertices[0].position;
	QVector3D max = mesh.m_vertices[0].position;

    int numTriangles = mesh.m_triangles.size();

	for (int i = 1; i < numTriangles * 3; ++i) {
		min[0] = fminf(mesh.m_vertices[i].position.x(), min.x());
		min[1] = fminf(mesh.m_vertices[i].position.y(), min.y());
		min[2] = fminf(mesh.m_vertices[i].position.z(), min.z());
	
		max[0] = fmaxf(mesh.m_vertices[i].position.x(), max.x());
		max[1] = fmaxf(mesh.m_vertices[i].position.y(), max.y());
		max[2] = fmaxf(mesh.m_vertices[i].position.z(), max.z());
	}

	mesh.accelerator = new BVHNode();
	mesh.accelerator->bounds = FromMinMax(min, max);
	mesh.accelerator->children = 0;
	mesh.accelerator->numTriangles = numTriangles;
	mesh.accelerator->triangles = new int[numTriangles];
	for (int i = 0; i < numTriangles; ++i) {
		mesh.accelerator->triangles[i] = i;
	}

	SplitBVHNode(mesh.accelerator, mesh, 3);
}

void SplitBVHNode(BVHNode* node, const Mesh& model, int depth) {
	if (depth-- <= 0) { // Decrements depth
		return;
	}

	if (node->children == 0) {
		// Only split if this node contains triangles
		if (node->numTriangles > 0) {
			node->children = new BVHNode[8];

			QVector3D c = node->bounds.position;
			QVector3D e = node->bounds.size *0.5f;

			node->children[0].bounds = AABB(c + QVector3D(-e.x(), +e.y(), -e.z()), e);
			node->children[1].bounds = AABB(c + QVector3D(+e.x(), +e.y(), -e.z()), e);
			node->children[2].bounds = AABB(c + QVector3D(-e.x(), +e.y(), +e.z()), e);
			node->children[3].bounds = AABB(c + QVector3D(+e.x(), +e.y(), +e.z()), e);
			node->children[4].bounds = AABB(c + QVector3D(-e.x(), -e.y(), -e.z()), e);
			node->children[5].bounds = AABB(c + QVector3D(+e.x(), -e.y(), -e.z()), e);
			node->children[6].bounds = AABB(c + QVector3D(-e.x(), -e.y(), +e.z()), e);
			node->children[7].bounds = AABB(c + QVector3D(+e.x(), -e.y(), +e.z()), e);

		}
	}

	// If this node was just split
	if (node->children != 0 && node->numTriangles > 0) {
		for (int i = 0; i < 8; ++i) { // For each child
			// Count how many triangles each child will contain
			node->children[i].numTriangles = 0;
			for (int j = 0; j < node->numTriangles; ++j) {
				Triangle t = model.m_triangles[node->triangles[j]];
				if (TriangleAABB(t, node->children[i].bounds)) {
					node->children[i].numTriangles += 1;
				}
			}
			if (node->children[i].numTriangles == 0) {
				continue;
			}
			node->children[i].triangles = new int[node->children[i].numTriangles];
			int index = 0; // Add the triangles in the new child arrau
			for (int j = 0; j < node->numTriangles; ++j) {
				Triangle t = model.m_triangles[node->triangles[j]];
				if (TriangleAABB(t, node->children[i].bounds)) {
					node->children[i].triangles[index++] = node->triangles[j];
				}
			}
		}

		node->numTriangles = 0;
		delete[] node->triangles;
		node->triangles = 0;

		// Recurse
		for (int i = 0; i < 8; ++i) {
			SplitBVHNode(&node->children[i], model, depth);
		}
	}
}

void FreeBVHNode(BVHNode* node) 
{
	if (node->children != 0) {
		for (int i = 0; i < 8; ++i) {
			FreeBVHNode(&node->children[i]);
		}
		delete[] node->children;
		node->children = 0;
	}

	if (node->numTriangles != 0 || node->triangles != 0) {
		delete[] node->triangles;
		node->triangles = 0;
		node->numTriangles = 0;
	}
}

bool TriangleAABB(const Triangle& t, const AABB& a) 
{
	// Compute the edge vectors of the triangle  (ABC)
	QVector3D f0 = t.b - t.a; 
	QVector3D f1 = t.c - t.b; 
	QVector3D f2 = t.a - t.c; 

	// Compute the face normals of the AABB
	QVector3D u0(1.0f, 0.0f, 0.0f);
	QVector3D u1(0.0f, 1.0f, 0.0f);
	QVector3D u2(0.0f, 0.0f, 1.0f);

	QVector3D test[13] = {
		// 3 Normals of AABB
		u0, // AABB Axis 1
		u1, // AABB Axis 2
		u2, // AABB Axis 3
		// 1 Normal of the Triangle
		QVector3D::crossProduct(f0, f1),
		// 9 Axis, QVector3D::crossProduct products of all edges
		QVector3D::crossProduct(u0, f0),
		QVector3D::crossProduct(u0, f1),
		QVector3D::crossProduct(u0, f2),
		QVector3D::crossProduct(u1, f0),
		QVector3D::crossProduct(u1, f1),
		QVector3D::crossProduct(u1, f2),
		QVector3D::crossProduct(u2, f0),
		QVector3D::crossProduct(u2, f1),
		QVector3D::crossProduct(u2, f2)
	};

	for (int i = 0; i < 13; ++i) {
		if (!OverlapOnAxis(a, t, test[i])) {
			return false; // Seperating axis found
		}
	}

	return true; // Seperating axis not found
}

bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const QVector3D& axis) 
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(triangle, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

Interval GetInterval(const Triangle& triangle, const QVector3D& axis) 
{
	Interval result;
    std::vector<QVector3D> points = { triangle.a, triangle.b, triangle.c };

	result.min = QVector3D::dotProduct(axis, points[0]);
	result.max = result.min;
	for (int i = 1; i < 3; ++i) {
		float value = QVector3D::dotProduct(axis, points[i]);
		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}

Interval GetInterval(const OBB& obb, const QVector3D& axis) 
{
	QVector3D vertex[8];

	QVector3D C = obb.position;	// OBB Center
	QVector3D E = obb.size;		// OBB Extents
	const float* o = obb.orientation.data();
	QVector3D A[] = {			// OBB Axis
		QVector3D(o[0], o[1], o[2]),
		QVector3D(o[3], o[4], o[5]),
		QVector3D(o[6], o[7], o[8]),
	};

	vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	Interval result;
	result.min = result.max = QVector3D::dotProduct(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = QVector3D::dotProduct(axis, vertex[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}

Interval GetInterval(const AABB& aabb, const QVector3D& axis) 
{
	QVector3D i = GetMin(aabb);
	QVector3D a = GetMax(aabb);

	QVector3D vertex[8] = {
		QVector3D(i.x(), a.y(), a.z()),
		QVector3D(i.x(), a.y(), i.z()),
		QVector3D(i.x(), i.y(), a.z()),
		QVector3D(i.x(), i.y(), i.z()),
		QVector3D(a.x(), a.y(), a.z()),
		QVector3D(a.x(), a.y(), i.z()),
		QVector3D(a.x(), i.y(), a.z()),
		QVector3D(a.x(), i.y(), i.z())
	};

	Interval result;
	result.min = result.max = QVector3D::dotProduct(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = QVector3D::dotProduct(axis, vertex[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}
*/