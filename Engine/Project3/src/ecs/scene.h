#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QJsonObject>

class Entity;
class Component;

#include "entity.h"

class Scene
{
public:
    Scene();
    ~Scene();

    int numEntities() const;
    Entity *addEntity();
    Entity *entityAt(int index);
    void removeEntityAt(int index);

    Component *findComponent(ComponentType ctype);

    void clear();

    void handleResourcesAboutToDie();

    void read(const QJsonObject &json);
    void write(QJsonObject &json);

    QVector<Entity*> entities;
};


#endif // SCENE_H
