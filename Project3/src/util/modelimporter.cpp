#include "util/modelimporter.h"
#include "resources/resourcemanager.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "resources/texture.h"
#include "ecs/scene.h"
#include "globals.h"
#include <QFile>
#include <QFileInfo>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>


ModelImporter::ModelImporter()
{

}

ModelImporter::~ModelImporter()
{

}

Entity* ModelImporter::import(const QString &path)
{
    Assimp::Importer import;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Could not open file for read: " << path.toStdString() << std::endl;
        return nullptr;
    }

    QFileInfo fileInfo(file);

#if 0
    QByteArray data = file.readAll();

    const aiScene *scene = import.ReadFileFromMemory(
                data.data(), data.size(),
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_OptimizeMeshes |
                aiProcess_PreTransformVertices |
                aiProcess_ImproveCacheLocality |
                aiProcess_CalcTangentSpace,
                fileInfo.suffix().toLatin1());
#else
    const aiScene *scene = import.ReadFile(
                path.toStdString(),
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_OptimizeMeshes |
                aiProcess_PreTransformVertices |
                aiProcess_ImproveCacheLocality |
                aiProcess_CalcTangentSpace);
#endif

    // Other flags
    // - aiProcess_JoinIdenticalVertices
    // - aiProcess_SortByPType
    // - aiProcess_RemoveRedundantMaterials
    // - https://www.ics.com/blog/qt-and-opengl-loading-3d-model-open-asset-import-library-assimp

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return nullptr;
    }

    // Used to find material files
    directory = fileInfo.path();

    // Create a list of materials
    QVector<Material*> myMaterials(scene->mNumMaterials, nullptr);
    QVector<Material*> mySubmeshMaterials(1024, nullptr);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        myMaterials[i] = resourceManager->createMaterial();
        processMaterial(scene->mMaterials[i], myMaterials[i]);
    }

    // Create the mesh and process the submeshes read by Assimp
    Mesh *myMesh = resourceManager->createMesh();
    myMesh->name = fileInfo.baseName();
    myMesh->filePath = fileInfo.filePath();
    processNode(scene->mRootNode, scene, myMesh, &myMaterials[0], &mySubmeshMaterials[0]);

    // Create an entity showing the mesh
    Entity *entity = ::scene->addEntity();
    entity->name = fileInfo.baseName();
    entity->addComponent(ComponentType::MeshRenderer);
    entity->meshRenderer->mesh = myMesh;
    for (int i = 0; i < myMesh->submeshes.size(); ++i)
    {
        entity->meshRenderer->materials.push_back(mySubmeshMaterials[i]);
    }

    return entity;
}

void ModelImporter::loadMesh(Mesh *mesh, const QString &path)
{
    Assimp::Importer import;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Could not open file for read: " << path.toStdString() << std::endl;
        return;
    }

    QFileInfo fileInfo(file);


    const aiScene *scene = import.ReadFile(
                path.toStdString(),
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_OptimizeMeshes |
                aiProcess_PreTransformVertices |
                aiProcess_ImproveCacheLocality |
                aiProcess_CalcTangentSpace);

    // Other flags
    // - aiProcess_JoinIdenticalVertices
    // - aiProcess_SortByPType
    // - aiProcess_RemoveRedundantMaterials
    // - https://www.ics.com/blog/qt-and-opengl-loading-3d-model-open-asset-import-library-assimp

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    // Used to find material files
    directory = fileInfo.path();

    // Create the mesh and process the submeshes read by Assimp
    processNode(scene->mRootNode, scene, mesh, nullptr, nullptr);
}

void ModelImporter::processMaterial(aiMaterial *material, Material *myMaterial)
{
    aiString name;
    aiColor3D diffuseColor;
    aiColor3D emissiveColor;
    aiColor3D specularColor;
    ai_real shininess;
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    material->Get(AI_MATKEY_SHININESS, shininess);

    myMaterial->name = QString::fromLatin1(name.C_Str());
    myMaterial->albedo = QColor::fromRgbF(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    myMaterial->emissive = QColor::fromRgbF(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    myMaterial->smoothness = shininess / 256.0f;

    aiString filename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
        QString filepath = QString::fromLatin1("%0/%1").arg(directory.toLatin1().data()).arg(filename.C_Str());
        myMaterial->albedoTexture = resourceManager->loadTexture(filepath);
    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        material->GetTexture(aiTextureType_EMISSIVE, 0, &filename);
        QString filepath = QString::fromLatin1("%0/%1").arg(directory.toLatin1().data()).arg(filename.C_Str());
        myMaterial->emissiveTexture = resourceManager->loadTexture(filepath);
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
        QString filepath = QString::fromLatin1("%0/%1").arg(directory.toLatin1().data()).arg(filename.C_Str());
        myMaterial->specularTexture = resourceManager->loadTexture(filepath);
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        material->GetTexture(aiTextureType_NORMALS, 0, &filename);
        QString filepath = QString::fromLatin1("%0/%1").arg(directory.toLatin1().data()).arg(filename.C_Str());
        myMaterial->normalsTexture = resourceManager->loadTexture(filepath);
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
        material->GetTexture(aiTextureType_HEIGHT, 0, &filename);
        QString filepath = QString::fromLatin1("%0/%1").arg(directory.toLatin1().data()).arg(filename.C_Str());
        myMaterial->bumpTexture = resourceManager->loadTexture(filepath);
    }

    myMaterial->createNormalFromBump();
}

void ModelImporter::processNode(aiNode *node, const aiScene *scene, Mesh *myMesh, Material **myMaterials, Material **mySubmeshMaterials)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, myMesh, myMaterials, mySubmeshMaterials);
    }

    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, myMesh, myMaterials, mySubmeshMaterials);
    }
}

void ModelImporter::processMesh(aiMesh *mesh, const aiScene *scene, Mesh *myMesh, Material **myMaterials, Material **mySubmeshMaterials)
{
    QVector<float> vertices;
    QVector<unsigned int> indices;

    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            hasTexCoords = true;
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }

        if(mesh->mTangents != nullptr && mesh->mBitangents)
        {
            hasTangentSpace = true;
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);

            // For some reason ASSIMP gives me the bitangents flipped.
            // Maybe it's my fault, but when I generate my own geometry
            // in other files (see the generation of standard assets)
            // and all the bitangents have the orientation I expect,
            // everything works ok.
            // I think that (even if the documentation says the opposite)
            // it returns a left-handed tangent space matrix.
            // SOLUTION: I invert the components of the bitangent here.
            vertices.push_back(-mesh->mBitangents[i].x);
            vertices.push_back(-mesh->mBitangents[i].y);
            vertices.push_back(-mesh->mBitangents[i].z);
        }
    }

    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // store the proper (previously proceessed) material for this mesh
    if(mesh->mMaterialIndex >= 0 && mySubmeshMaterials != nullptr && myMaterials != nullptr)
    {
        mySubmeshMaterials[myMesh->submeshes.size()] = myMaterials[mesh->mMaterialIndex];
    }

    // create the vertex format
    VertexFormat vertexFormat;
    vertexFormat.setVertexAttribute(0, 0, 3);
    vertexFormat.setVertexAttribute(1, 3 * sizeof(float), 3);
    if (hasTexCoords)
    {
        vertexFormat.setVertexAttribute(2, 6 * sizeof(float), 2);
    }
    if (hasTangentSpace)
    {
        vertexFormat.setVertexAttribute(3, 8 * sizeof(float), 3);
        vertexFormat.setVertexAttribute(4, 11 * sizeof(float), 3);
    }

    // add the submesh into the mesh
    myMesh->addSubMesh(
            vertexFormat,
            &vertices[0], vertices.size() * sizeof(float),
            &indices[0], indices.size());
}
