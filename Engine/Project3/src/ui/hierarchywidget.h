#ifndef HIERARCHYWIDGET_H
#define HIERARCHYWIDGET_H

#include <QWidget>

namespace Ui {
class HierarchyWidget;
}

class Entity;
class QListWidgetItem;

class HierarchyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HierarchyWidget(QWidget *parent = nullptr);
    ~HierarchyWidget();

signals:

    void entityAdded(Entity *entity);
    void entityRemoved(Entity *entity); // entity is null here
    void entitySelected(Entity *entity);

public slots:

    void updateLayout();
    void addEntity();
    void duplicateEntity();
    void removeEntity();
    void onItemClicked(QListWidgetItem *);

private:
    Ui::HierarchyWidget *ui;
};

#endif // HIERARCHYWIDGET_H
