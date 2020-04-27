#include "ui/meshrendererwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include "ecs/scene.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "resources/resourcemanager.h"
#include "globals.h"
#include <QPushButton>
#include <QListView>
#include <QAction>
#include <QMenu>
#include <QVariant>


MeshRendererWidget::MeshRendererWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(QString::fromLatin1("Mesh Renderer"));
}

MeshRendererWidget::~MeshRendererWidget()
{
}

void MeshRendererWidget::setMeshRenderer(MeshRenderer *m)
{
    meshRenderer = m;
    if (meshRenderer == nullptr) return;
    updateLayout();
}

void MeshRendererWidget::onMeshChanged(int index)
{
    meshRenderer->mesh = (Mesh*) comboMesh->itemData(index).value<void*>();
    emit componentChanged(meshRenderer);
}

void MeshRendererWidget::onMaterialSelect()
{
    QPushButton *button = (QPushButton*)sender();
    Material *currentMaterial =
            (Material*)sender()->property("material").value<void*>();

    QMenu contextMenu(tr("Materials"), button);

    QVector<QAction*> actions;

    auto action = new QAction("None", this);
    action->setCheckable(true);
    action->setChecked(currentMaterial == nullptr);
    action->font().setBold(currentMaterial == nullptr);
    action->setProperty("button", QVariant::fromValue<void*>(button));
    action->setProperty("material", QVariant::fromValue<void*>(nullptr));
    connect(action, SIGNAL(triggered()), this, SLOT(onMaterialChanged()));
    contextMenu.addAction(action);
    contextMenu.addSeparator();
    actions.push_back(action);

    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Material * material = resourceManager->resources[i]->asMaterial();
        if (material != nullptr)
        {
            auto action = new QAction(material->name, this);
            action->setCheckable(true);
            action->setChecked(currentMaterial == material);
            action->font().setBold(currentMaterial == material);
            action->setProperty("button", QVariant::fromValue<void*>(button));
            action->setProperty("material", QVariant::fromValue<void*>(material));
            connect(action, SIGNAL(triggered()), this, SLOT(onMaterialChanged()));
            contextMenu.addAction(action);
            actions.push_back(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MeshRendererWidget::onMaterialChanged()
{
    QPushButton *button = (QPushButton*)sender()->property("button").value<void*>();
    Material *material = (Material*)sender()->property("material").value<void*>();
    bool validSlot;
    int slotIndex = (int)button->property("slotIndex").toInt(&validSlot);

    if (button != nullptr && validSlot)
    {
        button->setText(material?material->name:"None");
        button->setProperty("material", QVariant::fromValue<void*>(material));
        meshRenderer->materials[slotIndex] = material;
        emit componentChanged(meshRenderer);
    }
}

void MeshRendererWidget::onAddMaterial()
{
    if (meshRenderer == nullptr) {
        qDebug("No hay meshRenderer.");
        return;
    }
    meshRenderer->materials.push_back(nullptr);
    updateLayout();
    emit componentChanged(meshRenderer);
}

void MeshRendererWidget::destroyLayout()
{
    hide();
    QVector<QLayoutItem*> items;
    items.push_back(layout());

    while (!items.empty())
    {
        QLayoutItem *item = items.takeAt(0);
        if (item != nullptr)
        {
            if (item->layout())
            {
                QLayoutItem *subitem;
                while ((subitem = item->layout()->takeAt(0)) != nullptr)
                {
                    items.push_back(subitem);
                }
                delete item->layout();
            }
            else if (item->widget() != nullptr)
            {
                item->widget()->hide();
                delete item->widget();
            }
            else
            {
                delete item;
            }
        }
    }
}

void MeshRendererWidget::updateLayout()
{
    destroyLayout();

    auto vlayout = new QVBoxLayout;

    { // Mesh
        auto label = new QLabel;
        label->setText("Mesh");

        comboMesh = createMeshesCombo();

        auto hlayout = new QHBoxLayout;
        hlayout->addWidget(label);
        hlayout->addWidget(comboMesh);
        vlayout->addItem(hlayout);
    }

    { // List of materials
        auto label = new QLabel;
        label->setText("Materials");

        auto addButton = new QPushButton("Add");
        connect(addButton, SIGNAL(clicked()), this, SLOT(onAddMaterial()));

        auto hlayout = new QHBoxLayout;
        hlayout->addWidget(label);
        hlayout->addWidget(addButton);
        vlayout->addItem(hlayout);

        // List
        materialsList = createMaterialsLayout();
        vlayout->addItem(materialsList);
    }

    setLayout(vlayout);
    show();
}

QComboBox *MeshRendererWidget::createMeshesCombo()
{
    auto comboMesh = new QComboBox;

    comboMesh->clear();

    comboMesh->addItem("None", QVariant::fromValue<void*>(nullptr));

    for (auto resource : resourceManager->resources)
    {
        Mesh *mesh = resource->asMesh();

        if (mesh != nullptr)
        {
            comboMesh->addItem(mesh->name, QVariant::fromValue<void*>(mesh));

            if (meshRenderer != nullptr && meshRenderer->mesh == mesh)
            {
                comboMesh->setCurrentIndex(comboMesh->count() - 1);
            }
        }
    }

    connect(comboMesh, SIGNAL(currentIndexChanged(int)), this, SLOT(onMeshChanged(int)));

    return comboMesh;
}
QVBoxLayout *MeshRendererWidget::createMaterialsLayout()
{
    auto listLayout = new QVBoxLayout;
    listLayout->setSpacing(0);
    listLayout->setMargin(0);

    if (meshRenderer == nullptr) {
        return listLayout;
    }

    for (int i = 0; i < meshRenderer->materials.size(); ++i)
    {
        Material *material = meshRenderer->materials[i];
        auto layout = new QHBoxLayout;
        auto label = new QLabel(QString::fromLatin1("[%0]").arg(i));
        auto button = createButtonForMaterialSlot(material, i);
        layout->addWidget(label);
        layout->addWidget(button);

        listLayout->addItem(layout);
    }

    return listLayout;
}

QPushButton * MeshRendererWidget::createButtonForMaterialSlot(Material *material, int slotIndex)
{
    auto button = new QPushButton;
    button->setText(material?material->name:"None");
    button->setProperty("material", QVariant::fromValue<void*>(material));
    button->setProperty("slotIndex", QVariant::fromValue(slotIndex));
    connect(button, SIGNAL(clicked()), this, SLOT(onMaterialSelect()));
    return button;
}
