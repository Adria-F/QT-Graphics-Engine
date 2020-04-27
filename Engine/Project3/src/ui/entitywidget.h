#ifndef ENTITYWIDGET_H
#define ENTITYWIDGET_H

#include <QWidget>

namespace Ui {
class EntityWidget;
}

class Entity;

class EntityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EntityWidget(QWidget *parent = nullptr);
    ~EntityWidget();

    void setEntity(Entity *ent);

signals:

    void entityChanged(Entity *);

public slots:

    void onActiveChanged();
    void onReturnPressed();
    void clearFocus();

private:
    Ui::EntityWidget *ui;
    Entity *entity = nullptr;
};

#endif // ENTITYWIDGET_H
