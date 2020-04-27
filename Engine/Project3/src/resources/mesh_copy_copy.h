#ifndef MESH_H
#define MESH_H

#include "resource.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>

static const int MAX_VERTEX_ATTRIBUTES = 8;

struct VertexAttribute
{
    bool enabled = false;
    int offset = 0;
    int ncomp = 0;
};

class VertexFormat
{
public:

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
    void draw();
    void destroy();

private:

    unsigned char *data = nullptr;
    size_t data_size = 0;

    unsigned int *indices = nullptr;
    size_t indices_count = 0;

    VertexFormat vertexFormat;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLVertexArrayObject vao;
};

// Assimp stuff
struct aiScene;
struct aiNode;
struct aiMesh;

class Mesh : public Resource
{
public:
    Mesh();
    ~Mesh() override;

    Mesh * asMesh() override { return this; }

    void update() override;
    void destroy() override;

    void addSubMesh(VertexFormat vertexFormat, void *data, int bytes);
    void addSubMesh(VertexFormat vertexFormat, void *data, int bytes, unsigned int *indexes, int bytes_indexes);
    void loadModel(const char *filename);

    QVector<SubMesh*> submeshes;

private:

    // Assimp stuff
    void processNode(aiNode *node, const aiScene *scene);
    SubMesh * processMesh(aiMesh *mesh, const aiScene *scene);
};

#endif // MESH_H
