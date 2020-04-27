#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>
#include <QUuid>

class Mesh;
class Material;
class Texture;
class ShaderProgram;
class QJsonObject;

class Resource
{
public:
    Resource() : guid(QUuid::createUuid()), name("Resource") { }
    //Resource(QString n) : name(n) { }
    virtual ~Resource() { }

    virtual const char *typeName() const = 0;

    virtual Mesh * asMesh() { return nullptr; }
    virtual Texture * asTexture() { return nullptr; }
    virtual Material * asMaterial() { return nullptr; }
    virtual ShaderProgram * asShaderProgram() { return nullptr; }

    virtual void handleResourcesAboutToDie() { }
    virtual void update() { needsUpdate = false; }
    virtual void destroy() { }

    virtual void write(QJsonObject &) = 0;
    virtual void read(const QJsonObject &) = 0;
    virtual void link(const QJsonObject &) { }

    QUuid guid;
    QString name;
    bool needsUpdate = false;
    bool needsRemove = false;
    bool includeForSerialization = true;


    // Static methods

    static QString absolutePathInProject(QString filePath);
    static QString relativePathInProject(QString filePath);
};

#endif // RESOURCE_H
