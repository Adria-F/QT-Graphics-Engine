#ifndef ENTITY_H
#define ENTITY_H

#include "components.h"

#define MAX_COMPONENTS 8

class Entity
{
public:

    Entity();
    ~Entity();

    Component *addComponent(ComponentType ctype);
    Component *findComponent(ComponentType ctype);
    void removeComponent(Component *component);

    Entity *clone() const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json);

    QVector3D getIDColor() const;

    QString name;

    union
    {
        struct
        {
            Transform *transform;
            MeshRenderer *meshRenderer;
            LightSource *lightSource;
        };
        Component *components[MAX_COMPONENTS];
    };

    bool active = true;

    unsigned int id = 0;
};

#endif // ENTITY_H
