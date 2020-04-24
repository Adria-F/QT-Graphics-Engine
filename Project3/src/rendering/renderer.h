#ifndef RENDERER_H
#define RENDERER_H

#include <QVector>
#include <QString>

class Camera;

class Renderer
{
public:
    Renderer() { }
    virtual ~Renderer() { }

    virtual void initialize() = 0;
    virtual void finalize() = 0;

    virtual void resize(int width, int height) = 0;
    virtual void render(Camera *camera) = 0;

    QVector<QString> getTextures() const;
    void showTexture(QString textureName);
    QString shownTexture() const;

protected:

    void addTexture(QString textureName);
    QVector<QString> textures;
    QString m_shownTexture;
};

#endif // RENDERER_H
