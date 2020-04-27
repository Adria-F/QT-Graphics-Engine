#include "mesh.h"
#include "rendering/gl.h"
#include <QVector2D>
#include <QVector3D>
#include <QFile>
#include <QJsonObject>


const char *Mesh::TypeName = "Mesh";


SubMesh::SubMesh(VertexFormat vf, void *in_data, int in_data_size) :
ibo(QOpenGLBuffer::Type::IndexBuffer)
{
    vertexFormat = vf;
    data_size = size_t(in_data_size);
    data = new unsigned char[data_size];
    memcpy(data, in_data, data_size);
	
    computeBounds();
}

SubMesh::SubMesh(VertexFormat vf, void *in_data, int in_data_size, unsigned int *in_indices, int in_indices_count) :
ibo(QOpenGLBuffer::Type::IndexBuffer)
{
    vertexFormat = vf;
	
    data_size = size_t(in_data_size);
    data = new unsigned char[data_size];
    memcpy(data, in_data, data_size);
	
    indices_count = size_t(in_indices_count);
    indices = new unsigned int[indices_count];
    memcpy(indices, in_indices, indices_count * sizeof(unsigned int));
	
    computeBounds();
}

SubMesh::~SubMesh()
{
    delete[] data;
    delete[] indices;
}

void SubMesh::enableAttributes()
{
    vbo.bind();
    if (ibo.isCreated()) { ibo.bind(); }
	
    for (int location = 0; location < MAX_VERTEX_ATTRIBUTES; ++location)
    {
        VertexAttribute &attr = vertexFormat.attribute[location];
		
        if (attr.enabled)
        {
            gl->glEnableVertexAttribArray(GLuint(location));
            gl->glVertexAttribPointer(GLuint(location), attr.ncomp, GL_FLOAT, GL_FALSE, vertexFormat.size, (void *) (attr.offset));
        }
    }
}

void SubMesh::update()
{
    if (vbo.isCreated()) vbo.destroy();
    if (ibo.isCreated()) ibo.destroy();
    if (vao.isCreated()) vao.destroy();
	
    // VBO: Buffer with vertex data
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    vbo.allocate(data, int(data_size));
    vbo.release();
    delete[] data;
    data = nullptr;
	
    // IBO: Buffer with indexes
    if (indices != nullptr)
    {
        ibo.create();
        ibo.bind();
        ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
        ibo.allocate(indices, int(indices_count * sizeof(unsigned int)));
        ibo.release();
        delete[] indices;
        indices = nullptr;
    }
	
    // VAO: Vertex format description and state of VBOs
    vao.create();
    vao.bind();
	
    enableAttributes();
	
    // Release
    vao.release();
    vbo.release();
    if (ibo.isCreated()) { ibo.release(); }
}

void SubMesh::draw(GLenum primitiveType)
{
    int num_vertices = data_size / vertexFormat.size;
    vao.bind();
    if (indices_count > 0) {
        gl->glDrawElements(primitiveType, indices_count, GL_UNSIGNED_INT, nullptr);
    } else {
        gl->glDrawArrays(primitiveType, 0, num_vertices);
    }
    vao.release();
}

void SubMesh::destroy()
{
    if (vbo.isCreated()) { vbo.destroy(); }
    if (ibo.isCreated()) { ibo.destroy(); }
    if (vao.isCreated()) { vao.destroy(); }
}

static QVector3D min(const QVector3D &a, const QVector3D &b)
{
    QVector3D res = b;
    if (a.x() < res.x()) res.setX(a.x());
    if (a.y() < res.y()) res.setY(a.y());
    if (a.z() < res.z()) res.setZ(a.z());
    return res;
}

static QVector3D max(const QVector3D &a, const QVector3D &b)
{
    QVector3D res = b;
    if (a.x() > res.x()) res.setX(a.x());
    if (a.y() > res.y()) res.setY(a.y());
    if (a.z() > res.z()) res.setZ(a.z());
    return res;
}

void SubMesh::computeBounds()
{
    const float *vertex = (const float *)data;
    const float *end = (const float *)(data + data_size);
    const int float_advance = vertexFormat.size / sizeof(float);
    while (vertex < end)
    {
        const QVector3D pos = QVector3D(vertex[0], vertex[1], vertex[2]);
        bounds.min = min(bounds.min, pos);
        bounds.max = max(bounds.max, pos);
        vertex += float_advance;
    }
}

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
    for (auto submesh : submeshes)
    {
        delete submesh;
    }
}

void Mesh::addSubMesh(VertexFormat vertexFormat, void *data, int bytes)
{
    submeshes.push_back(new SubMesh(vertexFormat, data, bytes));
    updateBounds(submeshes.back()->bounds);
    needsUpdate = true;
}

void Mesh::addSubMesh(VertexFormat vertexFormat, void *data, int data_size, unsigned int *indices, int indices_size)
{
    submeshes.push_back(new SubMesh(vertexFormat, data, data_size, indices, indices_size));
    updateBounds(submeshes.back()->bounds);
    needsUpdate = true;
}

void Mesh::updateBounds(const Bounds &b)
{
    bounds.min = min(bounds.min, b.min);
    bounds.max = max(bounds.max, b.max);
}

void Mesh::update()
{
    for (auto submesh : submeshes)
    {
        submesh->update();
    }
}

void Mesh::destroy()
{
    for (auto submesh : submeshes)
    {
        submesh->destroy();
    }
}

void Mesh::read(const QJsonObject &json)
{
}

void Mesh::write(QJsonObject &json)
{
}
