#include "entity.h"
#include "globals.h"

#include <QRandomGenerator>
#include <time.h>

Entity::Entity() :
    name("Entity")
{
    for (int i = 0; i < MAX_COMPONENTS; ++i)
        components[i] = nullptr;
    transform = new Transform;

    QRandomGenerator generator;
    generator.seed((time(NULL)));
    double range01 = generator.generateDouble();
    id = range01*256*256*256;
}

Entity::~Entity()
{
    delete transform;
    delete meshRenderer;
    delete lightSource;
}

Component *Entity::addComponent(ComponentType componentType)
{
    Component *component = nullptr;

    switch (componentType)
    {
    case ComponentType::Transform:
        Q_ASSERT(transform == nullptr);
        component = transform = new Transform;
        break;
    case ComponentType::LightSource:
        Q_ASSERT(lightSource == nullptr);
        component = lightSource = new LightSource;
        break;;
    case ComponentType::MeshRenderer:
        Q_ASSERT(meshRenderer == nullptr);
        component = meshRenderer = new MeshRenderer;
        break;
    default:
        Q_ASSERT(false && "Invalid code path");
    }

    component->entity = this;
    return component;
}

void Entity::removeComponent(Component *component)
{
    if (transform == component)
    {
        delete transform;
        transform = nullptr;
    }
    else if (component == meshRenderer)
    {
        delete meshRenderer;
        meshRenderer = nullptr;
    }
    else if (component == lightSource)
    {
        delete lightSource;
        lightSource = nullptr;
    }
}

Component *Entity::findComponent(ComponentType ctype)
{
    for (int i = 0; i < MAX_COMPONENTS; ++i)
    {
        if (components[i] != nullptr && components[i]->componentType() == ctype)
        {
            return components[i];
        }
    }
    return nullptr;
}

Entity *Entity::clone() const
{
    Entity *entity = scene->addEntity(); // Global scene
    entity->name = name;
    entity->active = active;
    if (transform != nullptr) {
        //entity->addComponent(ComponentType::Transform); // transforms are created by default
        *entity->transform = *transform;
        entity->transform->entity = entity;
    }
    if (meshRenderer != nullptr) {
        entity->addComponent(ComponentType::MeshRenderer);
        *entity->meshRenderer = *meshRenderer;
        entity->meshRenderer->entity = entity;
    }
    if (lightSource != nullptr) {
        entity->addComponent(ComponentType::LightSource);
        *entity->lightSource = *lightSource;
        entity->lightSource->entity = entity;
    }
    return entity;
}

void Entity::read(const QJsonObject &json)
{
}

void Entity::write(QJsonObject &json)
{
}

QVector3D Entity::getIDColor() const
{
    int r = (id & 0x000000FF) >>  0;
    int g = (id & 0x0000FF00) >>  8;
    int b = (id & 0x00FF0000) >> 16;

    return QVector3D(r,g,b)/255.0;
}
