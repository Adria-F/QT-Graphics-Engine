#include "resourcemanager.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "shaderprogram.h"
#include <QVector3D>
#include <cmath>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>


ResourceManager::ResourceManager()
{
    float quad[] = {
        -1.0, -1.0, 0.0,
         1.0, -1.0, 0.0,
         1.0,  1.0, 0.0,
        -1.0, -1.0, 0.0,
         1.0,  1.0, 0.0,
        -1.0,  1.0, 0.0
    };

    QVector3D tris[] = {
        // Triangle 1
        QVector3D(-0.5f, -0.5f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f), // Vertex 1
        QVector3D( 0.5f, -0.5f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f), // Vertex 2
        QVector3D( 0.0f,  0.5f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), // Vertex 3
        // Triangle 2
        QVector3D(-0.7f, -0.3f, 0.5f), QVector3D(0.5f, 0.0f, 0.0f), // Vertex 4
        QVector3D( 0.3f, -0.3f, 0.5f), QVector3D(0.0f, 0.5f, 0.0f), // Vertex 5
        QVector3D(-0.2f,  0.7f, 0.5f), QVector3D(0.0f, 0.0f, 0.5f)  // Vertex 6
    };

    QVector3D cube[] = {
        // +Z
        QVector3D(-0.5,-0.5, 0.5), QVector3D(0,0,1),
        QVector3D( 0.5,-0.5, 0.5), QVector3D(0,0,1),
        QVector3D( 0.5, 0.5, 0.5), QVector3D(0,0,1),
        QVector3D(-0.5,-0.5, 0.5), QVector3D(0,0,1),
        QVector3D( 0.5, 0.5, 0.5), QVector3D(0,0,1),
        QVector3D(-0.5, 0.5, 0.5), QVector3D(0,0,1),
        // -Z
        QVector3D(-0.5,-0.5,-0.5), QVector3D(0,0,-1),
        QVector3D( 0.5, 0.5,-0.5), QVector3D(0,0,-1),
        QVector3D( 0.5,-0.5,-0.5), QVector3D(0,0,-1),
        QVector3D(-0.5,-0.5,-0.5), QVector3D(0,0,-1),
        QVector3D(-0.5, 0.5,-0.5), QVector3D(0,0,-1),
        QVector3D( 0.5, 0.5,-0.5), QVector3D(0,0,-1),
        // +X
        QVector3D( 0.5,-0.5,-0.5), QVector3D( 1,0,0),
        QVector3D( 0.5, 0.5, 0.5), QVector3D( 1,0,0),
        QVector3D( 0.5,-0.5, 0.5), QVector3D( 1,0,0),
        QVector3D( 0.5,-0.5,-0.5), QVector3D( 1,0,0),
        QVector3D( 0.5, 0.5,-0.5), QVector3D( 1,0,0),
        QVector3D( 0.5, 0.5, 0.5), QVector3D( 1,0,0),
        // -X
        QVector3D(-0.5,-0.5,-0.5), QVector3D(-1,0,0),
        QVector3D(-0.5,-0.5, 0.5), QVector3D(-1,0,0),
        QVector3D(-0.5, 0.5, 0.5), QVector3D(-1,0,0),
        QVector3D(-0.5,-0.5,-0.5), QVector3D(-1,0,0),
        QVector3D(-0.5, 0.5, 0.5), QVector3D(-1,0,0),
        QVector3D(-0.5, 0.5,-0.5), QVector3D(-1,0,0),
        // +Y
        QVector3D(-0.5, 0.5, 0.5), QVector3D(0,1,0),
        QVector3D( 0.5, 0.5, 0.5), QVector3D(0,1,0),
        QVector3D( 0.5, 0.5,-0.5), QVector3D(0,1,0),
        QVector3D(-0.5, 0.5, 0.5), QVector3D(0,1,0),
        QVector3D( 0.5, 0.5,-0.5), QVector3D(0,1,0),
        QVector3D(-0.5, 0.5,-0.5), QVector3D(0,1,0),
        // +Y
        QVector3D(-0.5,-0.5, 0.5), QVector3D(0,-1,0),
        QVector3D( 0.5,-0.5,-0.5), QVector3D(0,-1,0),
        QVector3D( 0.5,-0.5, 0.5), QVector3D(0,-1,0),
        QVector3D(-0.5,-0.5, 0.5), QVector3D(0,-1,0),
        QVector3D(-0.5,-0.5,-0.5), QVector3D(0,-1,0),
        QVector3D( 0.5,-0.5,-0.5), QVector3D(0,-1,0)
    };

#define VEC3(a, b, c) a, b, c
#define VEC2(a, b) a, b

    float plane[] = {
        VEC3(-10.0, 0.0, 10.0), VEC3(0,1,0), VEC2(0.0, 0.0), VEC3(1,0,0), VEC3(0,0,-1),
        VEC3( 10.0, 0.0, 10.0), VEC3(0,1,0), VEC2(1.0, 0.0), VEC3(1,0,0), VEC3(0,0,-1),
        VEC3( 10.0, 0.0,-10.0), VEC3(0,1,0), VEC2(1.0, 1.0), VEC3(1,0,0), VEC3(0,0,-1),
        VEC3(-10.0, 0.0, 10.0), VEC3(0,1,0), VEC2(0.0, 0.0), VEC3(1,0,0), VEC3(0,0,-1),
        VEC3( 10.0, 0.0,-10.0), VEC3(0,1,0), VEC2(1.0, 1.0), VEC3(1,0,0), VEC3(0,0,-1),
        VEC3(-10.0, 0.0,-10.0), VEC3(0,1,0), VEC2(0.0, 1.0), VEC3(1,0,0), VEC3(0,0,-1)
    };

#define H 32
#define V 16

    static const float pi = 3.1416f;
    struct Vertex { QVector3D pos; QVector3D norm; };

    Vertex sphere[H][V + 1];
    for (int h = 0; h < H; ++h) {
        for (int v = 0; v < V + 1; ++v) {
            float nh = float(h) / H;
            float nv = float(v) / V - 0.5f;
            float angleh = 2 * pi * nh;
            float anglev = - pi * nv;
            sphere[h][v].pos.setX(sinf(angleh) * cosf(anglev));
            sphere[h][v].pos.setY(-sinf(anglev));
            sphere[h][v].pos.setZ(cosf(angleh) * cosf(anglev));
            sphere[h][v].norm = sphere[h][v].pos;
        }
    }

    unsigned int sphereIndices[H][V][6];
    for (unsigned int h = 0; h < H; ++h) {
        for (unsigned int v = 0; v < V; ++v) {
            sphereIndices[h][v][0] =  (h+0)    * (V+1) + v;
            sphereIndices[h][v][1] = ((h+1)%H) * (V+1) + v;
            sphereIndices[h][v][2] = ((h+1)%H) * (V+1) + v+1;
            sphereIndices[h][v][3] =  (h+0)    * (V+1) + v;
            sphereIndices[h][v][4] = ((h+1)%H) * (V+1) + v+1;
            sphereIndices[h][v][5] =  (h+0)    * (V+1) + v+1;
        }
    }

    VertexFormat vertexFormatPos;
    vertexFormatPos.setVertexAttribute(0, 0, 3);

    VertexFormat vertexFormat;
    vertexFormat.setVertexAttribute(0, 0, 3);
    vertexFormat.setVertexAttribute(1, sizeof(QVector3D), 3);

    Mesh *mesh = nullptr;

    mesh = createMesh();
    mesh->guid = "3ae8c18f-4537-4ea5-b54e-3015fbe96487";
    mesh->name = "Quad";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(vertexFormatPos, quad, sizeof(quad));
    this->quad = mesh;

    mesh = createMesh();
    mesh->guid = "18b6765b-0f78-44a7-927c-2f38e08cd0bb";
    mesh->name = "Triangles";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(vertexFormat, tris, sizeof(tris));
    this->tris = mesh;

    mesh = createMesh();
    mesh->guid = "e75f0355-0b7c-4075-93b6-1bebce8af65a";
    mesh->name = "Cube";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(vertexFormat, cube, sizeof(cube));
    this->cube = mesh;

    VertexFormat planeVertexFormat;
    int offset = 0;
    planeVertexFormat.setVertexAttribute(0, offset, 3); offset += 3 * sizeof(float);
    planeVertexFormat.setVertexAttribute(1, offset, 3); offset += 3 * sizeof(float);
    planeVertexFormat.setVertexAttribute(2, offset, 2); offset += 2 * sizeof(float);
    planeVertexFormat.setVertexAttribute(3, offset, 3); offset += 3 * sizeof(float);
    planeVertexFormat.setVertexAttribute(4, offset, 3); offset += 3 * sizeof(float);
    mesh = createMesh();
    mesh->guid = "b53c759b-d6d6-4210-83e6-556bcb28624f";
    mesh->name = "Plane";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(planeVertexFormat, plane, sizeof(plane));
    this->plane = mesh;

    mesh = createMesh();
    mesh->guid = "45f3bbd4-317b-442c-bc81-db5773bc5cc5";
    mesh->name = "Sphere";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(vertexFormat, sphere, sizeof(sphere), &sphereIndices[0][0][0], H*V*6);
    this->sphere = mesh;

    float unitCubeGridData[] = {
        VEC3(0, 0, 0), VEC3(1, 0, 0),
        VEC3(0, 0, 0), VEC3(0, 1, 0),
        VEC3(0, 1, 0), VEC3(1, 1, 0),
        VEC3(1, 0, 0), VEC3(1, 1, 0),
        VEC3(0, 0, 1), VEC3(1, 0, 1),
        VEC3(0, 0, 1), VEC3(0, 1, 1),
        VEC3(0, 1, 1), VEC3(1, 1, 1),
        VEC3(1, 0, 1), VEC3(1, 1, 1),
        VEC3(0, 0, 0), VEC3(0, 0, 1),
        VEC3(0, 1, 0), VEC3(0, 1, 1),
        VEC3(1, 0, 0), VEC3(1, 0, 1),
        VEC3(1, 1, 0), VEC3(1, 1, 1)
    };

    VertexFormat unitCubeGridVertexFormat;
    unitCubeGridVertexFormat.setVertexAttribute(0, 0, 3);

    mesh = createMesh();
    mesh->guid = "ab044246-37ac-47a0-8373-e20ee929f57b";
    mesh->name = "Cube grid";
    mesh->includeForSerialization = false;
    mesh->addSubMesh(unitCubeGridVertexFormat, unitCubeGridData, sizeof(unitCubeGridData));
    this->unitCubeGrid = mesh;



    // Pre made textures

    QImage whitePixel(1, 1, QImage::Format::Format_RGB888);
    whitePixel.setPixelColor(0, 0, QColor::fromRgb(255, 255, 255));

    QImage blackPixel(1, 1, QImage::Format::Format_RGB888);
    blackPixel.setPixelColor(0, 0, QColor::fromRgb(0, 0, 0));

    QImage normalPixel(1, 1, QImage::Format::Format_RGB888);
    normalPixel.setPixelColor(0, 0, QColor::fromRgb(128, 128, 255));

    texWhite = createTexture();
    texWhite->name = "White texture";
    texWhite->includeForSerialization = false;
    texWhite->setImage(whitePixel);

    texBlack = createTexture();
    texBlack->name = "Black texture";
    texBlack->includeForSerialization = false;
    texBlack->setImage(blackPixel);

    texNormal = createTexture();
    texNormal->name = "Normal texture";
    texNormal->includeForSerialization = false;
    texNormal->setImage(normalPixel);


    // Pre made materials

    materialWhite = createMaterial();
    materialWhite->name = "White material";
    materialWhite->includeForSerialization = false;

    materialLight = createMaterial();
    materialLight->name = "Material light";
    materialLight->emissive = QColor(255, 255, 255);
    materialLight->includeForSerialization = false;
}

ResourceManager::~ResourceManager()
{
    qDebug("ResourceManager deletion");
    for (auto res : resources) {
        delete res;
    }
}

Mesh *ResourceManager::createMesh()
{
    Mesh *m = new Mesh;
    resources.push_back(m);
    return m;
}

Mesh *ResourceManager::getMesh(const QUuid &guid)
{
    for (auto res : resources)
    {
        if (res->guid == guid)
        {
            return res->asMesh();
        }
    }
    return nullptr;
}


Material *ResourceManager::createMaterial()
{
    Material *m = new Material;
    resources.push_back(m);
    return m;
}

Material *ResourceManager::getMaterial(const QUuid &guid)
{
    for (auto res : resources)
    {
        if (res->guid == guid)
        {
            return res->asMaterial();
        }
    }
    return nullptr;
}

Texture *ResourceManager::createTexture()
{
    Texture *t = new Texture;
    resources.push_back(t);
    return t;
}

Texture *ResourceManager::loadTexture(const QString &filePath)
{
    Texture *tex = nullptr;
    for (auto res : resources)
    {
        tex = res->asTexture();
        if (tex != nullptr && tex->getFilePath() == filePath)
        {
            return tex;
        }
    }
    QFileInfo fileInfo(filePath);
    tex = createTexture();
    tex->loadTexture(filePath.toLatin1());
    tex->name = fileInfo.fileName();
    return tex;
}

Texture *ResourceManager::getTexture(const QUuid &guid)
{
    for (auto res : resources)
    {
        if (res->guid == guid)
        {
            return res->asTexture();
        }
    }
    return nullptr;
}

ShaderProgram *ResourceManager::createShaderProgram()
{
    ShaderProgram *res = new ShaderProgram;
    resources.push_back(res);
    return res;
}

ShaderProgram *ResourceManager::getShaderProgram(const QString &name)
{
    for (auto res : resources)
    {
        if (res->name == name)
        {
            return res->asShaderProgram();
        }
    }
    return nullptr;
}

void ResourceManager::reloadShaderPrograms()
{
    for (auto res : resources)
    {
        if (res->asShaderProgram())
        {
            res->asShaderProgram()->reload();
        }
    }
}

Resource *ResourceManager::createResource(const QString &type)
{
    if (type == QString::fromLatin1(Mesh::TypeName))
    {
        return createMesh();
    }
    if (type == QString::fromLatin1(Texture::TypeName))
    {
        return createTexture();
    }
    if (type == QString::fromLatin1(Material::TypeName))
    {
        return createMaterial();
    }

    qDebug("Could not create the resource of type %s", type.toStdString().c_str());
    return nullptr;
}

Resource *ResourceManager::loadResource(const QString &path)
{
    Resource *res = nullptr;
    QFileInfo fileInfo(path);
    if (fileInfo.suffix().endsWith("jpg") ||
            fileInfo.suffix().endsWith("png") ||
            fileInfo.suffix().endsWith("hdr"))
    {
        res = loadTexture(path);
    }
    if (fileInfo.suffix().endsWith("obj") ||
            fileInfo.suffix().endsWith("fbx"))
    {
        // TODO
    }
    return res;
}

Resource *ResourceManager::getResource(const QUuid &guid)
{
    for (auto res : resources)
    {
        if (res->guid == guid)
        {
            return res;
        }
    }
    return nullptr;
}

int ResourceManager::numResources() const
{
    return resources.size();
}

Resource *ResourceManager::resourceAt(int index)
{
    return resources[index];
}

void ResourceManager::read(const QJsonObject &json)
{
}

void ResourceManager::write(QJsonObject &json)
{
}

void ResourceManager::removeResourceAt(int index)
{
    resources[index]->needsRemove = true;
}

void ResourceManager::destroyResource(Resource *res)
{
    if (res != nullptr) {
        res->needsRemove = true;
    }
}

void ResourceManager::clear()
{
    for (auto resource : resources)
    {
        if (resource->includeForSerialization) {
            resource->needsRemove = true;
        }
    }
}

void ResourceManager::updateResources()
{
    int i = 0;
    while (i < resources.size())
    {
        Resource *resource = resources[i];

        resource->handleResourcesAboutToDie();

        if (resource->needsUpdate)
        {
            resource->update();
            resource->needsUpdate = false;
        }

        if (resource->needsRemove)
        {
            resourcesToDestroy.push_back(resource);
            resources.removeAt(i);
        }
        else
        {
            ++i;
        }
    }

    for (auto resource : resourcesToDestroy)
    {
        resource->destroy();
        delete resource;
    }
    resourcesToDestroy.clear();
}

void ResourceManager::destroyResources()
{
    for (auto resource : resources)
    {
        resource->destroy();
    }
}
