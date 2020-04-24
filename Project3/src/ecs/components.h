#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
#include <QColor>
#include <QJsonObject>

class Entity;
class Mesh;
class Material;

enum class ComponentType {
    Transform,
    MeshRenderer,
    LightSource
};

// Components //////////////////////////////////////////////////////////

class Component
{
public:
    Component() { }
    virtual ~Component() { }

    virtual ComponentType componentType() const = 0;

    virtual void read(const QJsonObject &json) = 0;
    virtual void write(QJsonObject &json) = 0;

    Entity *entity = nullptr;
};

class Transform : public Component
{
public:
    Transform();

    QMatrix4x4 matrix() const;

    ComponentType componentType() const override { return ComponentType::Transform; }

    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

    QVector3D position;
    QQuaternion rotation;
    QVector3D scale;
};

class MeshRenderer : public Component
{
public:

    MeshRenderer();

    void handleResourcesAboutToDie();

    ComponentType componentType() const override { return ComponentType::MeshRenderer; }

    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

    Mesh *mesh = nullptr;
    QVector<Material*> materials;
};

class LightSource : public Component
{
public:

    enum class Type { Point, Directional };

    LightSource();

    ComponentType componentType() const override { return ComponentType::LightSource; }

    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

    Type type = Type::Point;
    QColor color;
    float intensity = 1.0f;
    float range = 10.0f;
};

#endif // COMPONENTS_H
