#ifndef MESH_H
#define MESH_H

#include "resource.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>
#include <QVector3D>
#include <cfloat>

static const int MAX_VERTEX_ATTRIBUTES = 8;

struct Bounds {
    QVector3D min = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
    QVector3D max = QVector3D(-FLT_MAX, -FLT_MAX, -FLT_MAX);
};

struct VertexAttribute
{
    bool enabled = false;
    int offset = 0;
    int ncomp = 0;
};

class VertexFormat
{
public:

    VertexFormat()
    {
        size = 0;
        for (int i = 0; i < MAX_VERTEX_ATTRIBUTES; ++i)
        {
            attribute[i].enabled = false;
        }
    }

    void setVertexAttribute(int location, int offset, int ncomp)
    {
        attribute[location].enabled = true;
        attribute[location].offset = offset;
        attribute[location].ncomp = ncomp;
        size += ncomp * sizeof(float);
    }

    VertexAttribute attribute[MAX_VERTEX_ATTRIBUTES];
    int size = 0;
};

class SubMesh
{
public:
    SubMesh(VertexFormat vertexFormat, void *data, int size);
    SubMesh(VertexFormat vertexFormat, void *data, int size, unsigned int *indices, int indices_count);
    ~SubMesh();

    void update();
    void draw(GLenum primitiveType = GL_TRIANGLES);
    void destroy();

    unsigned int vertexCount() const { return data_size/vertexFormat.size; }

    void enableAttributes();

private:

    friend class Mesh;
    Bounds bounds;

    void computeBounds();

    unsigned char *data = nullptr;
    size_t data_size = 0;

    unsigned int *indices = nullptr;
    size_t indices_count = 0;

    VertexFormat vertexFormat;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLVertexArrayObject vao;
};

class Mesh : public Resource
{
public:

    static const char *TypeName;

    Mesh();
    ~Mesh() override;

    const char *typeName() const override { return TypeName; }

    Mesh * asMesh() override { return this; }

    void update() override;
    void destroy() override;

    void addSubMesh(VertexFormat vertexFormat, void *data, int bytes);
    void addSubMesh(VertexFormat vertexFormat, void *data, int bytes, unsigned int *indexes, int bytes_indexes);

    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

    const QString &getFilePath() const { return filePath; }

    QVector<SubMesh*> submeshes;

    Bounds bounds;

private:

    void updateBounds(const Bounds &b);

    QString filePath;
    friend class ModelImporter;
};

#endif // MESH_H
