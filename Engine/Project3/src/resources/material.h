#ifndef MATERIAL_H
#define MATERIAL_H

#include "resource.h"
#include <QColor>
#include <QVector2D>

class Texture;

enum class MaterialShaderType
{
    Surface,
    Water
};

class Material : public Resource
{
public:

    static const char *TypeName;

    Material();
    ~Material() override;

    const char *typeName() const override { return TypeName; }

    Material * asMaterial() override { return this; }

    void handleResourcesAboutToDie() override;

    void write(QJsonObject &json) override;
    void read(const QJsonObject &json) override;
    void link(const QJsonObject &json) override;

    void createNormalFromBump();

    MaterialShaderType shaderType = MaterialShaderType::Surface; // Required shader

    QColor albedo;           // RGB color
    QColor emissive;         // Emissive color
    QColor specular;         // Specular color
    float smoothness = 0.0f; // from 0.0 to 1.0
    float metalness = 0.0f;  // from 0.0 to 1.0
    float bumpiness = 0.0f;  // from 0.0 to X
    QVector2D tiling;        // tiling factor

    // Textures
    Texture *albedoTexture = nullptr;
    Texture *emissiveTexture = nullptr;
    Texture *specularTexture = nullptr;
    Texture *normalsTexture = nullptr;
    Texture *bumpTexture = nullptr;
};

#endif // MATERIAL_H
