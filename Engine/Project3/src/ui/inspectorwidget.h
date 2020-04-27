#ifndef INSPECTORWIDGET_H
#define INSPECTORWIDGET_H

#include <QWidget>

class QPushButton;
class QScrollArea;
class Entity;
class Component;
class EntityWidget;
class TransformWidget;
class MeshRendererWidget;
class LightSourceWidget;
class ComponentWidget;
class MeshWidget;
class TextureWidget;
class MaterialWidget;
class Resource;
class ResourceWidget;

class InspectorWidget : public QWidget
{
    Q_OBJECT

public:

    explicit InspectorWidget(QWidget *parent = nullptr);
    ~InspectorWidget();

    QSize sizeHint() const override;

    void showEntity(Entity *e);
    void showResource(Resource *r);

public slots:

    void updateLayout();
    void onEntityChanged(Entity *);
    void onEntityRemoved(Entity *);
    void onComponentChanged(Component *);
    void onAddMeshRendererClicked();
    void onAddLightSourceClicked();
    void onRemoveComponent(Component *);
    void onResourceChanged(Resource *);
    void adjustSize();

signals:

    void entityChanged(Entity *e);
    void resourceChanged(Resource *r);

private:

    bool eventFilter(QObject *o, QEvent *e) override;

    QSize currentSize;

    QWidget *contentsWidget = nullptr;
    QScrollArea *scrollArea = nullptr;

    Entity *entity = nullptr;
    QLayout *layout = nullptr;
    EntityWidget *entityWidget = nullptr;
    TransformWidget *transformWidget = nullptr;
    MeshRendererWidget *meshRendererWidget = nullptr;
    LightSourceWidget *lightSourceWidget = nullptr;
    ComponentWidget *transformComponentWidget = nullptr;
    ComponentWidget *meshRendererComponentWidget = nullptr;
    ComponentWidget *lightSourceComponentWidget = nullptr;
    QPushButton *buttonAddMeshRenderer = nullptr;
    QPushButton *buttonAddLightSource = nullptr;

    Resource *resource = nullptr;
    ResourceWidget *resourceWidget = nullptr;
    MeshWidget *meshWidget = nullptr;
    TextureWidget *textureWidget = nullptr;
    MaterialWidget *materialWidget = nullptr;
};

#endif // INSPECTORWIDGET_H
