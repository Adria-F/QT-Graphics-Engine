#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include <QString>

class Entity;
class Mesh;
class Material;
struct aiMesh;
struct aiNode;
struct aiScene;
struct aiMaterial;

class ModelImporter
{
public:

    ModelImporter();
    ~ModelImporter();

    // It loads a model and creates an entity with it
    Entity *import(const QString &path);

    // It only loads the mesh geometry into a mesh
    void loadMesh(Mesh *mesh, const QString &path);

private:

    // Assimp stuff
    void processMaterial(aiMaterial *material, Material *myMaterial);
    void processNode(aiNode *node, const aiScene *scene, Mesh *myMesh, Material **myMaterials, Material **mySubmeshMaterials);
    void processMesh(aiMesh *mesh, const aiScene *scene, Mesh *myMesh, Material **myMaterials, Material **mySubmeshMaterials);

    QString directory; /**< Directory in which the file to import is located. */
};

#endif // MODELIMPORTER_H
