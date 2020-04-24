#ifndef RESOURCESWIDGET_H
#define RESOURCESWIDGET_H

#include <QWidget>

namespace Ui {
class ResourcesWidget;
}

class Resource;
class QListWidgetItem;

class ResourcesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResourcesWidget(QWidget *parent = nullptr);
    ~ResourcesWidget();

signals:

    void resourceAdded(Resource *entity);
    void resourceRemoved(Resource *entity); // entity is null here
    void resourceSelected(Resource *entity);

public slots:

    void updateLayout();
    void addResource();
    void removeResource();
    void onItemClicked(QListWidgetItem *);

private:
    Ui::ResourcesWidget *ui;
};

#endif // RESOURCESWIDGET_H
