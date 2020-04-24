#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/hierarchywidget.h"
#include "ui/resourceswidget.h"
#include "ui/inspectorwidget.h"
#include "ui/openglwidget.h"
#include "ui/aboutopengldialog.h"
#include "ui/miscsettingswidget.h"
#include "ecs/scene.h"
#include "resources/resourcemanager.h"
#include "resources/mesh.h"
#include "resources/texture.h"
#include "resources/material.h"
#include "util/modelimporter.h"
#include "globals.h"
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QComboBox>
#include <QDockWidget>
#include <QVBoxLayout>


MainWindow *g_MainWindow = nullptr;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    uiMainWindow(new Ui::MainWindow)
{
    // In globals.h / globals.cpp
    resourceManager = new ResourceManager();
    scene = new Scene();

    g_MainWindow = this;
    uiMainWindow->setupUi(this);

    // Central widget
    openGLWidget = new OpenGLWidget;
    auto centralLayout = new QVBoxLayout;
    centralLayout->addWidget(openGLWidget);
    uiMainWindow->centralWidget->setLayout(centralLayout);

    // Combo-box with renderer intermediate outputs
    auto comboRenderer = new QComboBox;
    QVector<QString> textureNames = openGLWidget->getTextureNames();
    for (auto textureName : textureNames) { comboRenderer->addItem(textureName); }
    uiMainWindow->toolBar->addWidget(comboRenderer);

    // All tab positions on top of the docking area
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);

    // Create the hierarchy widget and add it to a docking widget
    hierarchyWidget = new HierarchyWidget();
    auto hierarchyDock = new QDockWidget("Hierarchy");
    hierarchyDock->setWidget(hierarchyWidget);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, hierarchyDock);

    // Create the resources widget and add it to a docking widget
    resourcesWidget = new ResourcesWidget();
    auto resourcesDock = new QDockWidget("Resources");
    resourcesDock->setWidget(resourcesWidget);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, resourcesDock);

    tabifyDockWidget(hierarchyDock, resourcesDock);
    hierarchyDock->raise();

    // Create the inspector widget and add it to a docking widget
    inspectorWidget = new InspectorWidget();
    auto inspectorDock = new QDockWidget("Inspector");
    inspectorDock->setWidget(inspectorWidget);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, inspectorDock);

    // Create the misc settings widget and add it to a docking widget
    auto miscSettingsWidget = new MiscSettingsWidget();
    auto miscSettingsDock = new QDockWidget("Misc settings");
    miscSettingsDock->setWidget(miscSettingsWidget);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, miscSettingsDock);

    tabifyDockWidget(inspectorDock, miscSettingsDock);
    inspectorDock->raise();

    // View menu actions
    uiMainWindow->menuView->addAction(hierarchyDock->toggleViewAction());
    uiMainWindow->menuView->addAction(resourcesDock->toggleViewAction());
    uiMainWindow->menuView->addAction(inspectorDock->toggleViewAction());
    uiMainWindow->menuView->addAction(miscSettingsDock->toggleViewAction());

    // Signals / slots connections
    connect(uiMainWindow->actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(uiMainWindow->actionSaveProject, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(uiMainWindow->actionCloseProject, SIGNAL(triggered()), this, SLOT(closeProject()));
    connect(uiMainWindow->actionSaveScreenshot, SIGNAL(triggered()), this, SLOT(saveScreenshot()));
    connect(uiMainWindow->actionAboutOpenGL, SIGNAL(triggered()), this, SLOT(showAboutOpenGL()));
    connect(uiMainWindow->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    connect(uiMainWindow->actionAddCube, SIGNAL(triggered()), this, SLOT(addCube()));
    connect(uiMainWindow->actionAddPlane, SIGNAL(triggered()), this, SLOT(addPlane()));
    connect(uiMainWindow->actionAddSphere, SIGNAL(triggered()), this, SLOT(addSphere()));
    connect(uiMainWindow->actionAddPointLight, SIGNAL(triggered()), this, SLOT(addPointLight()));
    connect(uiMainWindow->actionAddDirectionalLight, SIGNAL(triggered()), this, SLOT(addDirectionalLight()));
    connect(uiMainWindow->actionImportModel, SIGNAL(triggered()), this, SLOT(importModel()));
    connect(uiMainWindow->actionAddMesh, SIGNAL(triggered()), this, SLOT(addMesh()));
    connect(uiMainWindow->actionAddTexture, SIGNAL(triggered()), this, SLOT(addTexture()));
    connect(uiMainWindow->actionAddMaterial, SIGNAL(triggered()), this, SLOT(addMaterial()));
    connect(uiMainWindow->actionReloadShaderPrograms, SIGNAL(triggered()), this, SLOT(reloadShaderPrograms()));
    connect(comboRenderer, SIGNAL(currentIndexChanged(QString)), this, SLOT(onRenderOutputChanged(QString)));

    connect(hierarchyWidget, SIGNAL(entityAdded(Entity *)), this, SLOT(onEntityAdded(Entity *)));
    connect(hierarchyWidget, SIGNAL(entityRemoved(Entity *)), this, SLOT(onEntityRemoved(Entity *)));
    connect(hierarchyWidget, SIGNAL(entitySelected(Entity *)), this, SLOT(onEntitySelectedFromHierarchy(Entity *)));
    connect(resourcesWidget, SIGNAL(resourceAdded(Resource *)), this, SLOT(onResourceAdded(Resource *)));
    connect(resourcesWidget, SIGNAL(resourceRemoved(Resource *)), this, SLOT(onResourceRemoved(Resource *)));
    connect(resourcesWidget, SIGNAL(resourceSelected(Resource *)), this, SLOT(onResourceSelected(Resource *)));
    connect(inspectorWidget, SIGNAL(entityChanged(Entity*)), this, SLOT(onEntityChangedFromInspector(Entity*)));
    connect(inspectorWidget, SIGNAL(resourceChanged(Resource*)), this, SLOT(onResourceChangedFromInspector(Resource*)));
    connect(openGLWidget, SIGNAL(interacted()), this, SLOT(onEntityChangedInteractively()));
    connect(miscSettingsWidget, SIGNAL(settingsChanged()), this, SLOT(updateRender()));

    connect(selection, SIGNAL(entitySelected(Entity *)), this, SLOT(onEntitySelectedFromSceneView(Entity *)));

    hierarchyWidget->updateLayout();
    resourcesWidget->updateLayout();

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    // In globals.h / globals.cpp
    delete scene;

    openGLWidget->makeCurrent();
    delete resourceManager;

    delete uiMainWindow;

    g_MainWindow = nullptr;
}

void MainWindow::openProject()
{
    QString path = QFileDialog::getOpenFileName(this,"Open project", QString(), QString::fromLatin1("Json files (*.json)"));
    if (!path.isEmpty())
    {
        QFile openFile(path);

        if (!openFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

        QFileInfo fileInfo(path);
        projectDirectory = fileInfo.absolutePath();

        QJsonDocument openDoc = QJsonDocument::fromJson(openFile.readAll());
        resourceManager->read(openDoc.object());
        scene->read(openDoc.object());

        updateEverything();
    }
}

void MainWindow::saveProject()
{
    QString path = QFileDialog::getSaveFileName(this, "Save project", QString(), QString::fromLatin1("Json files (*.json)"));
    if (!path.isEmpty())
    {
        QFile saveFile(path);

        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

        QFileInfo fileInfo(path);
        projectDirectory = fileInfo.absolutePath();

        QJsonObject project;
        resourceManager->write(project);
        scene->write(project);
        QJsonDocument saveDoc(project);
        saveFile.write(saveDoc.toJson());
    }
}

void MainWindow::closeProject()
{
    projectDirectory.clear();
    scene->clear();
    selection->clear();
    resourceManager->clear();
    inspectorWidget->showEntity(nullptr);
    updateEverything();
}

void MainWindow::saveScreenshot()
{
    QString path = QFileDialog::getSaveFileName(this, "Save screenshot", QString(), "*.png");
    if (!path.isEmpty()) {
        QImage image = openGLWidget->getScreenshot();
        image.save(path);
    }
}

void MainWindow::showAboutOpenGL()
{
    AboutOpenGLDialog dialog;
    dialog.setContents(openGLWidget->getOpenGLInfo());
    dialog.exec();
}

void MainWindow::addCube()
{
    Entity *entity = scene->addEntity();
    entity->name = "Cube";
    entity->addComponent(ComponentType::MeshRenderer);
    entity->meshRenderer->mesh = resourceManager->cube;
    onEntityAdded(entity);
}

void MainWindow::addPlane()
{
    Entity *entity = scene->addEntity();
    entity->name = "Plane";
    entity->addComponent(ComponentType::MeshRenderer);
    entity->meshRenderer->mesh = resourceManager->plane;
    onEntityAdded(entity);
}

void MainWindow::addSphere()
{
    Entity *entity = scene->addEntity();
    entity->name = "Sphere";
    entity->addComponent(ComponentType::MeshRenderer);
    entity->meshRenderer->mesh = resourceManager->sphere;
    onEntityAdded(entity);
}

void MainWindow::addPointLight()
{
    Entity *entity = scene->addEntity();
    entity->transform->position = QVector3D(3.0f, 5.0f, 4.0f);
    entity->name = "Point light";
    entity->addComponent(ComponentType::LightSource);
    entity->lightSource->type = LightSource::Type::Point;
    entity->lightSource->intensity = 10.0f;
    entity->lightSource->range = 20.0f;
    onEntityAdded(entity);
}

void MainWindow::addDirectionalLight()
{
    Entity *entity = scene->addEntity();
    entity->transform->position = QVector3D(3.0f, 5.0f, 4.0f);
    entity->name = "Directional light";
    entity->addComponent(ComponentType::LightSource);
    entity->lightSource->type = LightSource::Type::Directional;
    onEntityAdded(entity);
}

void MainWindow::importModel()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose a 3D model file.",QString(), "3D Models (*.obj *.fbx)");
    if (path.isEmpty()) return;

    ModelImporter importer;
    Entity *entity = importer.import(path);
    onEntityAdded(entity);
}

void MainWindow::addMesh()
{
    Mesh *res = resourceManager->createMesh();
    res->name = "Mesh";
    onResourceAdded(res);
}

void MainWindow::addTexture()
{
    Texture *res = resourceManager->createTexture();
    res->name = "Texture";
    onResourceAdded(res);
}

void MainWindow::addMaterial()
{
    Material *res = resourceManager->createMaterial();
    res->name = "Material";
    onResourceAdded(res);
}

void MainWindow::exit()
{
    QMessageBox::StandardButton button = QMessageBox::question(
                this,
                "Exit application",
                "Are you sure you want to exit the application?");
    if (button == QMessageBox::Yes) {
        qApp->quit();
    }
}

void MainWindow::updateRender()
{
    openGLWidget->update();
}

void MainWindow::updateEverything()
{
    hierarchyWidget->updateLayout();
    resourcesWidget->updateLayout();
    inspectorWidget->updateLayout();
    openGLWidget->update();
}

void MainWindow::onSceneChanged()
{
    hierarchyWidget->updateLayout();
}

void MainWindow::onEntityAdded(Entity * entity)
{
    inspectorWidget->showEntity(entity);
    updateEverything();
}

void MainWindow::onEntityRemoved(Entity *entity)
{
    selection->onEntityRemovedFromEditor(entity);
    inspectorWidget->onEntityRemoved(entity);
    updateEverything();
}

void MainWindow::onEntitySelectedFromHierarchy(Entity *entity)
{
    selection->onEntitySelectedFromEditor(entity);
    inspectorWidget->showEntity(entity);
    openGLWidget->update();
}

void MainWindow::onEntitySelectedFromSceneView(Entity *entity)
{
    inspectorWidget->showEntity(entity);
    openGLWidget->update();
}

void MainWindow::onEntityChangedFromInspector(Entity * /*entity*/)
{
   hierarchyWidget->updateLayout();
   openGLWidget->update();
}

void MainWindow::onEntityChangedInteractively()
{
   //inspectorWidget->updateLayout();
}

void MainWindow::onResourceAdded(Resource *resource)
{
    resourcesWidget->updateLayout();
    inspectorWidget->showResource(resource);
}

void MainWindow::onResourceRemoved(Resource *resource)
{
    scene->handleResourcesAboutToDie();
    resourcesWidget->updateLayout();
    inspectorWidget->showResource(resource);
    openGLWidget->update();
}

void MainWindow::onResourceSelected(Resource *resource)
{
    inspectorWidget->showResource(resource);
}

void MainWindow::onResourceChangedFromInspector(Resource *)
{
    resourcesWidget->updateLayout();
    openGLWidget->update();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
  event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* mimeData = event->mimeData();

  // check for our needed mime type, here a file or a list of files
  if (mimeData->hasUrls())
  {
    Resource *res = nullptr;
    QStringList pathList;
    QList<QUrl> urlList = mimeData->urls();

    // extract the local paths of the files
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
      QString filePath = urlList.at(i).toLocalFile();
      if (filePath.endsWith("obj") || filePath.endsWith("fbx"))
      {
          ModelImporter importer;
          Entity *entity = importer.import(filePath);
          onEntityAdded(entity);
      }
      else
      {
          res = resourceManager->loadResource(filePath);
      }
    }

    onResourceAdded(res);

    event->acceptProposedAction();
  }
  else
  {
      qDebug("Drop action not accepted");
  }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button = QMessageBox::question(
                this,
                "Exit application",
                "Are you sure you want to exit the application?");
    if (button == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::reloadShaderPrograms()
{
    resourceManager->reloadShaderPrograms();
    openGLWidget->update();
}

void MainWindow::onRenderOutputChanged(QString name)
{
    openGLWidget->showTextureWithName(name);
    openGLWidget->update();
}
