#include "ui/hierarchywidget.h"
#include "ui_hierarchywidget.h"
#include "ecs/scene.h"
#include "globals.h"

HierarchyWidget::HierarchyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HierarchyWidget)
{
    ui->setupUi(this);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntity()));
    connect(ui->duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateEntity()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeEntity()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onItemClicked(QListWidgetItem *)));
}

HierarchyWidget::~HierarchyWidget()
{
    delete ui;
}

void HierarchyWidget::updateLayout()
{
    ui->listWidget->clear();
    for (int i = 0; i < scene->numEntities(); ++i)
    {
        if (scene->entityAt(i) != nullptr)
        {
            ui->listWidget->addItem(scene->entityAt(i)->name);
        }
    }
}

void HierarchyWidget::addEntity()
{
    Entity *entity = scene->addEntity();
    emit entityAdded(entity);
}

void HierarchyWidget::duplicateEntity()
{
    int index = ui->listWidget->currentRow();
    if (index != -1)
    {
        auto entity = scene->entityAt(index);
        auto duplicatedEntity = entity->clone();
        emit entityAdded(duplicatedEntity);
    }
}

void HierarchyWidget::removeEntity()
{
    int index = ui->listWidget->currentRow();
    if (index != -1)
    {
        auto entity = scene->entityAt(index);
        scene->removeEntityAt(index);
        emit entityRemoved(entity);
    }
}

void HierarchyWidget::onItemClicked(QListWidgetItem *)
{
    int index = ui->listWidget->currentRow();
    if (index != -1)
    {
        Entity *entity = scene->entityAt(index);
        emit entitySelected(entity);
    }
}
