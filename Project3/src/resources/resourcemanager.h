#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QVector>
#include <QUuid>

class Resource;
class Mesh;
class Material;
class Texture;
class ShaderProgram;
class QJsonObject;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    Mesh *createMesh();
    Mesh *getMesh(const QUuid &guid);

    Material *createMaterial();
    Material *getMaterial(const QUuid &guid);

    Texture *createTexture();
    Texture *loadTexture(const QString &filename);
    Texture *getTexture(const QUuid &guid);

    ShaderProgram *createShaderProgram();
    ShaderProgram *getShaderProgram(const QString &name);
    void reloadShaderPrograms();

    Resource *createResource(const QString &type);
    Resource *loadResource(const QString &path);
    Resource *getResource(const QUuid &guid);

    int numResources() const;
    Resource *resourceAt(int index);
    void removeResourceAt(int index);

    void destroyResource(Resource *res);
    void clear();

    // Perform OpenGL calls
    void updateResources();
    void destroyResources();

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json);

    QVector<Resource*> resources;

    // Pre-made meshes
    Mesh *quad = nullptr;
    Mesh *tris = nullptr;
    Mesh *cube = nullptr;
    Mesh *plane = nullptr;
    Mesh *sphere = nullptr;
    Mesh *unitCubeGrid = nullptr;

    // Pre-made textures
    Texture *texWhite = nullptr;
    Texture *texBlack = nullptr;
    Texture *texNormal = nullptr;
    Texture *texTerrain = nullptr;
    Texture *texWaterNormals = nullptr;
    Texture *texWaterDudv = nullptr;

    // Pre-made materials
    Material *materialWhite = nullptr;
    Material *materialLight = nullptr;

private:

    QVector<Resource*> resourcesToDestroy;
};

#endif // RESOURCEMANAGER_H
