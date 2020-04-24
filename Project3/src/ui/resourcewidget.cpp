#include "ui/resourcewidget.h"
#include "ui_resourcewidget.h"
#include "resources/resource.h"

ResourceWidget::ResourceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResourceWidget)
{
    ui->setupUi(this);

    connect(ui->nameText, SIGNAL(editingFinished()), this, SLOT(clearFocus()));
    connect(ui->nameText, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

ResourceWidget::~ResourceWidget()
{
    delete ui;
}

void ResourceWidget::setResource(Resource *r)
{
    resource = r;
    if (resource == nullptr) return;

    if (r->name != ui->nameText->text()) {
        ui->nameText->setText(r->name);
    }
}

void ResourceWidget::onReturnPressed()
{
    resource->name = ui->nameText->text();
    ui->nameText->clearFocus();
    emit resourceChanged(resource);
}

void ResourceWidget::clearFocus()
{
    ui->nameText->clearFocus();
}
