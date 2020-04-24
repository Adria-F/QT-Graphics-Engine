#include "renderer.h"

QVector<QString> Renderer::getTextures() const
{
    return textures;
}

void Renderer::showTexture(QString textureName)
{
    m_shownTexture = textureName;
}

QString Renderer::shownTexture() const
{
    return m_shownTexture;
}

void Renderer::addTexture(QString textureName)
{
    if (textures.empty()) m_shownTexture = textureName;

    textures.push_back(textureName);
}
