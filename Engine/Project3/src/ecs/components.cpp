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
    calculateRadius();
}

void LightSource::read(const QJsonObject &json)
{
}

void LightSource::write(QJsonObject &json)
{
}

void LightSource::calculateRadius()
{
    float constant  = 1.0;
    float linear    = 1.0f/intensity;
    float quadratic = 1.0f/(range*0.01);
    float lightMax  = std::fmaxf(std::fmaxf(color.red(), color.green()), color.blue());
    radius = (-linear +  std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))/ (2 * quadratic)*0.1;
}

