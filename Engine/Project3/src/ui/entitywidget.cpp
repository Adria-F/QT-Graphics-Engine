#include "ui/entitywidget.h"
#include "ui_entitywidget.h"
#include "ecs/scene.h"
#include <QSignalBlocker>


EntityWidget::EntityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntityWidget)
{
    ui->setupUi(this);

    connect(ui->checkActive, SIGNAL(clicked()), this, SLOT(onActiveChanged()));
    connect(ui->nameText, SIGNAL(editingFinished()), this, SLOT(clearFocus()));
    connect(ui->nameText, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

EntityWidget::~EntityWidget()
{
    delete ui;
}

void EntityWidget::setEntity(Entity *ent)
{
    entity = ent;
    if (entity == nullptr) return;

    if (ent->name != ui->nameText->text()) {
        ui->nameText->setText(ent->name);
    }

    QSignalBlocker b(ui->checkActive);
    ui->checkActive->setChecked(ent->active);

    show();
}

void EntityWidget::onActiveChanged()
{
    entity->active = ui->checkActive->isChecked();
    emit entityChanged(entity);
}

void EntityWidget::onReturnPressed()
{
    entity->name = ui->nameText->text();
    ui->nameText->clearFocus();
    emit entityChanged(entity);
}

void EntityWidget::clearFocus()
{
    ui->nameText->clearFocus();
}
