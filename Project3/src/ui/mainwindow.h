#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class OpenGLWidget;
class HierarchyWidget;
class ResourcesWidget;
class InspectorWidget;
class ResourceManager;
class Scene;
class Entity;
class Resource;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void openProject();
    void saveProject();
    void closeProject();
    void saveScreenshot();
    void showAboutOpenGL();
    void addCube();
    void addPlane();
    void addSphere();
    void addPointLight();
    void addDirectionalLight();
    void importModel();
    void addMesh();
    void addTexture();
    void addMaterial();
    void exit();
    void onSceneChanged();
    void onEntityAdded(Entity *entity);
    void onEntityRemoved(Entity *entity);
    void onEntitySelectedFromHierarchy(Entity *entity);
    void onEntitySelectedFromSceneView(Entity *entity);
    void onEntityChangedFromInspector(Entity *entity);
    void onEntityChangedInteractively();
    void onResourceAdded(Resource *resource);
    void onResourceRemoved(Resource *resource);
    void onResourceSelected(Resource *resource);
    void onResourceChangedFromInspector(Resource *resource);
    void updateRender();
    void updateEverything();
    void reloadShaderPrograms();
    void onRenderOutputChanged(QString);

private:

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

private:

    Ui::MainWindow *uiMainWindow;

public:
    OpenGLWidget    *openGLWidget;
    HierarchyWidget *hierarchyWidget;
    ResourcesWidget *resourcesWidget;
    InspectorWidget *inspectorWidget;
};

extern MainWindow *g_MainWindow;

#endif // MAINWINDOW_H
