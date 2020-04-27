#include "ui/inspectorwidget.h"
#include "ui/entitywidget.h"
#include "ui/transformwidget.h"
#include "ui/meshrendererwidget.h"
#include "ui/lightsourcewidget.h"
#include "ui/componentwidget.h"
#include "ui/mainwindow.h"
#include "ui/meshwidget.h"
#include "ui/texturewidget.h"
#include "ui/materialwidget.h"
#include "ui/resourcewidget.h"
#include "ecs/scene.h"
#include "resources/resource.h"
#include <QLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizePolicy>
#include <QEvent>


InspectorWidget::InspectorWidget(QWidget *parent) :
    QWidget(parent)
{
    //currentSize = QSize(64, 64);

    // Create subwidgets independently
    transformWidget = new TransformWidget;
    meshRendererWidget = new MeshRendererWidget;
    lightSourceWidget = new LightSourceWidget;

    // Add all elements to the layout
    entityWidget = new EntityWidget;

    transformComponentWidget = new ComponentWidget;
    transformComponentWidget->setWidget(transformWidget);

    meshRendererComponentWidget = new ComponentWidget;
    meshRendererComponentWidget->setWidget(meshRendererWidget);

    lightSourceComponentWidget = new ComponentWidget;
    lightSourceComponentWidget->setWidget(lightSourceWidget);

    buttonAddMeshRenderer = new QPushButton("Add Mesh Renderer");
    buttonAddLightSource = new QPushButton("Add Light Source");

    resourceWidget = new ResourceWidget;

    meshWidget = new MeshWidget;
    textureWidget = new TextureWidget;
    materialWidget = new MaterialWidget;

    // Vertical layout with widgets
    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(entityWidget);
    layout->addWidget(transformComponentWidget);
    layout->addWidget(meshRendererComponentWidget);
    layout->addWidget(lightSourceComponentWidget);
    layout->addWidget(buttonAddMeshRenderer);
    layout->addWidget(buttonAddLightSource);
    layout->addWidget(resourceWidget);
    layout->addWidget(meshWidget);
    layout->addWidget(textureWidget);
    layout->addWidget(materialWidget);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding));

    // Widget with contents
    contentsWidget = new QWidget;
    contentsWidget->setLayout(layout);
    contentsWidget->installEventFilter(this);

    // Scroll area
    scrollArea = new QScrollArea;
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scrollArea->setWidget(contentsWidget);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->installEventFilter(this);

    // Scroll into InspectorWidget
    layout = new QVBoxLayout;
    layout->addWidget(scrollArea);
    setLayout(layout);

    showEntity(nullptr);

    connect(entityWidget, SIGNAL(entityChanged(Entity*)), this, SLOT(onEntityChanged(Entity *)));
    connect(transformWidget, SIGNAL(componentChanged(Component*)), this, SLOT(onComponentChanged(Component *)));
    connect(meshRendererWidget, SIGNAL(componentChanged(Component*)), this, SLOT(onComponentChanged(Component *)));
    connect(lightSourceWidget, SIGNAL(componentChanged(Component*)), this, SLOT(onComponentChanged(Component *)));
    connect(entityWidget, SIGNAL(entityChanged(Entity*)), this, SLOT(onEntityChanged(Entity *)));
    connect(buttonAddMeshRenderer, SIGNAL(clicked()), this, SLOT(onAddMeshRendererClicked()));
    connect(buttonAddLightSource, SIGNAL(clicked()), this, SLOT(onAddLightSourceClicked()));
    connect(meshRendererComponentWidget, SIGNAL(removeClicked(Component*)), this, SLOT(onRemoveComponent(Component *)));
    connect(lightSourceComponentWidget, SIGNAL(removeClicked(Component*)), this, SLOT(onRemoveComponent(Component *)));

    connect(transformComponentWidget, SIGNAL(collapsed()), this, SLOT(adjustSize()));
    connect(transformComponentWidget, SIGNAL(expanded()), this, SLOT(adjustSize()));
    connect(meshRendererComponentWidget, SIGNAL(collapsed()), this, SLOT(adjustSize()));
    connect(meshRendererComponentWidget, SIGNAL(expanded()), this, SLOT(adjustSize()));
    connect(lightSourceComponentWidget, SIGNAL(collapsed()), this, SLOT(adjustSize()));
    connect(lightSourceComponentWidget, SIGNAL(expanded()), this, SLOT(adjustSize()));

    connect(resourceWidget, SIGNAL(resourceChanged(Resource*)), this, SLOT(onResourceChanged(Resource *)));
    connect(meshWidget, SIGNAL(resourceChanged(Resource*)), this, SLOT(onResourceChanged(Resource*)));
    connect(textureWidget, SIGNAL(resourceChanged(Resource*)), this, SLOT(onResourceChanged(Resource*)));
    connect(materialWidget, SIGNAL(resourceChanged(Resource*)), this, SLOT(onResourceChanged(Resource*)));

    updateLayout();
}

InspectorWidget::~InspectorWidget()
{
}

QSize InspectorWidget::sizeHint() const
{
    return QSize(270, 300);
}

void InspectorWidget::showEntity(Entity *e)
{
    entity = e;
    resource = nullptr;
    updateLayout();
}

void InspectorWidget::showResource(Resource *r)
{
    entity = nullptr;
    resource = r;
    updateLayout();
}

void InspectorWidget::onEntityChanged(Entity *entity)
{
    emit entityChanged(entity);
}

void InspectorWidget::onEntityRemoved(Entity *e)
{
    if (entity == e)
    {
        entity = nullptr;
        updateLayout();
    }
}

void InspectorWidget::onComponentChanged(Component *)
{
    emit entityChanged(entity);
    adjustSize();
}

void InspectorWidget::onAddMeshRendererClicked()
{
    if (entity == nullptr) return;
    entity->addComponent(ComponentType::MeshRenderer);
    updateLayout();
    emit entityChanged(entity);
}

void InspectorWidget::onAddLightSourceClicked()
{
    if (entity == nullptr) return;
    entity->addComponent(ComponentType::LightSource);
    updateLayout();
    emit entityChanged(entity);
}

void InspectorWidget::onRemoveComponent(Component *c)
{
    if (entity == nullptr) return;
    entity->removeComponent(c);
    updateLayout();
    emit entityChanged(entity);
}

void InspectorWidget::onResourceChanged(Resource *res)
{
    updateLayout();
    emit resourceChanged(res);
}

void InspectorWidget::updateLayout()
{
    entityWidget->setVisible(false);
    transformComponentWidget->setVisible(false);
    meshRendererComponentWidget->setVisible(false);
    lightSourceComponentWidget->setVisible(false);
    buttonAddMeshRenderer->setVisible(false);
    buttonAddLightSource->setVisible(false);
    resourceWidget->setVisible(false);
    meshWidget->setVisible(false);
    textureWidget->setVisible(false);
    materialWidget->setVisible(false);

    // Entity related
    if (entity != nullptr)
    {
        entityWidget->setEntity(entity);

        transformComponentWidget->setComponent(entity->transform);
        meshRendererComponentWidget->setComponent(entity->meshRenderer);
        lightSourceComponentWidget->setComponent(entity->lightSource);

        transformWidget->setTransform(entity->transform);
        meshRendererWidget->setMeshRenderer(entity->meshRenderer);
        lightSourceWidget->setLightSource(entity->lightSource);

        buttonAddMeshRenderer->setVisible(entity->meshRenderer == nullptr);
        buttonAddLightSource->setVisible(entity->lightSource == nullptr);
    }

    // Resource related
    if (resource != nullptr)
    {
        Mesh *mesh = resource->asMesh();
        Texture *texture = resource->asTexture();
        Material *material = resource->asMaterial();

        resourceWidget->setVisible(true);
        meshWidget->setVisible(mesh != nullptr);
        textureWidget->setVisible(texture != nullptr);
        materialWidget->setVisible(material != nullptr);

        resourceWidget->setResource(resource);
        meshWidget->setMesh(mesh);
        textureWidget->setTexture(texture);
        materialWidget->setMaterial(material);
    }

    adjustSize();
}

void InspectorWidget::adjustSize()
{
    QSignalBlocker blocker(contentsWidget);
    int scrollWidth = scrollArea->width() - scrollArea->verticalScrollBar()->width();
    contentsWidget->adjustSize();
    contentsWidget->resize(scrollWidth, contentsWidget->height());
}

bool InspectorWidget::eventFilter(QObject *o, QEvent *e)
{
//    if (o == contentsWidget && e->type() == QEvent::Resize)
//    {
//        QSignalBlocker blocker(scrollArea);
//        //contentsWidget->adjustSize();
//        int contentWidth = contentsWidget->minimumSizeHint().width();
//        //scrollArea->setMinimumWidth(contentWidth + scrollArea->verticalScrollBar()->width());
//    }
//    else
    if (o == scrollArea && e->type() == QEvent::Resize)
    {
        adjustSize();
    }
    return false;
}
