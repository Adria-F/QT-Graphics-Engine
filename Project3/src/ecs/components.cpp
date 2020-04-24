#include "components.h"
#include "resources/mesh.h"
#include "resources/material.h"




Transform::Transform() :
    position(0.0f, 0.0f, 0.0f),
    rotation(),
    scale(1.0f, 1.0f, 1.0f)
{

}

QMatrix4x4 Transform::matrix() const
{
    QMatrix4x4 mat;
    mat.translate(position);
    mat.rotate(rotation);
    mat.scale(scale);
    return mat;
}

void Transform::read(const QJsonObject &json)
{
}

void Transform::write(QJsonObject &json)
{
}




MeshRenderer::MeshRenderer()
{
}

void MeshRenderer::handleResourcesAboutToDie()
{
    if (mesh->needsRemove)
    {
        mesh = nullptr;
    }

    for (int i = 0; i < materials.size(); ++i)
    {
        if (materials[i] && materials[i]->needsRemove)
        {
            materials[i] = nullptr;
        }
    }
}

void MeshRenderer::read(const QJsonObject &json)
{
}

void MeshRenderer::write(QJsonObject &json)
{
}




LightSource::LightSource() :
    color(255, 255, 255, 255)
{

}

void LightSource::read(const QJsonObject &json)
{
}

void LightSource::write(QJsonObject &json)
{
}

