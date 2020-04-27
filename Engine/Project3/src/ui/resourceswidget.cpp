#include "ui/resourceswidget.h"
#include "ui_resourceswidget.h"
#include "resources/resource.h"
#include "resources/resourcemanager.h"
#include "resources/texture.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "globals.h"
#include <QFileDialog>


ResourcesWidget::ResourcesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResourcesWidget)
{
    ui->setupUi(this);

    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeResource()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
}

ResourcesWidget::~ResourcesWidget()
{
    delete ui;
}

void ResourcesWidget::updateLayout()
{
    ui->listWidget->clear();
    for (int i = 0; i < resourceManager->numResources(); ++i)
    {
        Resource *res = resourceManager->resourceAt(i);
        if (res != nullptr && !res->needsRemove)
        {
            ui->listWidget->addItem(QString::fromLatin1("%0 (%1)").arg(res->name, res->typeName()));
        }
    }
}

void ResourcesWidget::addResource()
{
    QString path = QFileDialog::getOpenFileName(this, "Import a resource", QString(), "PNG (*.png *.jpg);;JPEG (*.jpg *.jpeg)");
    if (!path.isEmpty())
    {
        Resource *res = nullptr;
        if (path.endsWith(".png") || path.endsWith(".jpg"))
        {
            Texture *tex = resourceManager->createTexture();
            tex->name = path;
            tex->loadTexture(path.toLatin1());
            res = tex;
        }
        if (path.endsWith(".obj") || path.endsWith(".fbx"))
        {
//            Mesh *mesh = resourceManager->createMesh();
//            mesh->name = path;
//            mesh->loadModel(path.toLatin1());
//            res = mesh;
        }
        updateLayout();
        emit resourceAdded(res);
    }
}

void ResourcesWidget::removeResource()
{
    int index = ui->listWidget->currentRow();
    if (index != -1)
    {
        ui->listWidget->takeItem(index);
        resourceManager->removeResourceAt(index);
    }
    emit resourceRemoved(nullptr);
}

void ResourcesWidget::onItemClicked(QListWidgetItem *)
{
    int index = ui->listWidget->currentRow();
    if (index != -1)
    {
        Resource *res = resourceManager->resourceAt(index);
        ui->removeButton->setEnabled(res->includeForSerialization);
        emit resourceSelected(res);
    }
}
