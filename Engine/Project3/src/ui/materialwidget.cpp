#include "materialwidget.h"
#include "ui_materialwidget.h"
#include "resources/material.h"
#include "resources/texture.h"
#include "resources/resourcemanager.h"
#include "globals.h"
#include <QColorDialog>
#include <QMenu>
#include <QAction>


static void setButtonColor(QPushButton* button, const QColor &color)
{
    button->setStyleSheet(
                QString::fromLatin1("background-color: rgb(%0, %1, %2);")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue()));
}

static void setButtonTexture(QPushButton *button, Texture *tex)
{
    if (tex == nullptr) {
        button->setText("None");
    } else {
        button->setText(tex->name);
    }
}

MaterialWidget::MaterialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialWidget)
{
    ui->setupUi(this);

    ui->buttonAlbedo->setText("");
    ui->buttonEmissive->setText("");
    ui->buttonSpecular->setText("");
    ui->buttonAlbedoTexture->setText("None");
    ui->buttonEmissiveTexture->setText("None");
    ui->buttonSpecularTexture->setText("None");
    ui->buttonNormalTexture->setText("None");
    ui->buttonBumpTexture->setText("None");

    connect(ui->comboShader, SIGNAL(currentIndexChanged(int)), this, SLOT(onShaderChanged(int)));
    connect(ui->buttonAlbedo, SIGNAL(clicked()), this, SLOT(onButtonAlbedoClicked()));
    connect(ui->buttonAlbedoTexture, SIGNAL(clicked()), this, SLOT(onButtonAlbedoTextureClicked()));
    connect(ui->buttonEmissive, SIGNAL(clicked()), this, SLOT(onButtonEmissiveClicked()));
    connect(ui->buttonEmissiveTexture, SIGNAL(clicked()), this, SLOT(onButtonEmissiveTextureClicked()));
    connect(ui->buttonSpecular, SIGNAL(clicked()), this, SLOT(onButtonSpecularClicked()));
    connect(ui->buttonSpecularTexture, SIGNAL(clicked()), this, SLOT(onButtonSpecularTextureClicked()));
    connect(ui->buttonNormalTexture, SIGNAL(clicked()), this, SLOT(onButtonNormalTextureClicked()));
    connect(ui->buttonBumpTexture, SIGNAL(clicked()), this, SLOT(onButtonBumpTextureClicked()));
    connect(ui->sliderSmoothness, SIGNAL(valueChanged(int)), this, SLOT(onSmoothnessChanged(int)));
    connect(ui->sliderMetalness, SIGNAL(valueChanged(int)), this, SLOT(onMetalnessChanged(int)));
    connect(ui->spinBumpiness, SIGNAL(valueChanged(double)), this, SLOT(onBumpinessChanged(double)));
    connect(ui->spinTilingX, SIGNAL(valueChanged(double)), this, SLOT(onTilingChanged(double)));
    connect(ui->spinTilingY, SIGNAL(valueChanged(double)), this, SLOT(onTilingChanged(double)));
}

MaterialWidget::~MaterialWidget()
{
    delete ui;
}


void MaterialWidget::setMaterial(Material *m)
{
    material = m;

    if (material != nullptr)
    {
        setButtonColor(ui->buttonAlbedo, material->albedo);
        setButtonTexture(ui->buttonAlbedoTexture, material->albedoTexture);
        setButtonColor(ui->buttonEmissive, material->emissive);
        setButtonTexture(ui->buttonEmissiveTexture, material->emissiveTexture);
        setButtonColor(ui->buttonSpecular, material->specular);
        setButtonTexture(ui->buttonSpecularTexture, material->specularTexture);
        setButtonTexture(ui->buttonNormalTexture, material->normalsTexture);
        setButtonTexture(ui->buttonBumpTexture, material->bumpTexture);
        QSignalBlocker b0(ui->comboShader);
        QSignalBlocker b1(ui->sliderSmoothness);
        QSignalBlocker b2(ui->spinBumpiness);
        QSignalBlocker b3(ui->spinTilingX);
        QSignalBlocker b4(ui->spinTilingY);
        ui->comboShader->setCurrentIndex((int)material->shaderType);
        ui->sliderSmoothness->setValue(material->smoothness * 255);
        ui->spinBumpiness->setValue(material->bumpiness);
        ui->spinTilingX->setValue(material->tiling.x());
        ui->spinTilingY->setValue(material->tiling.y());
    }
}

void MaterialWidget::onShaderChanged(int index)
{
    material->shaderType = (MaterialShaderType)index;
    emit resourceChanged(material);
}

void MaterialWidget::onButtonAlbedoClicked()
{
    QColor color = QColorDialog::getColor(material->albedo, this, "Albedo");
    if (color.isValid())
    {
        material->albedo = color;
        setButtonColor(ui->buttonAlbedo, material->albedo);
        emit resourceChanged(material);
    }
}

void MaterialWidget::onButtonAlbedoTextureClicked()
{
    QMenu contextMenu(tr("Context menu"), (QPushButton*)sender());

    QVector<QAction*> actions;
    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Texture * texture = resourceManager->resources[i]->asTexture();
        if (texture != nullptr)
        {
            auto action = new QAction(texture->name, this);
            action->setProperty("texture", QVariant::fromValue<void*>(texture));
            actions.push_back(action);
            connect(action, SIGNAL(triggered()), this, SLOT(onAlbedoTextureChanged()));
            contextMenu.addAction(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MaterialWidget::onAlbedoTextureChanged()
{
    QAction *action = (QAction*)sender();
    Texture *texture = (Texture*)action->property("texture").value<void*>();
    material->albedoTexture = texture;
    ui->buttonAlbedoTexture->setText(material->albedoTexture->name);
    emit resourceChanged(material);
}

void MaterialWidget::onButtonEmissiveClicked()
{
    QColor color = QColorDialog::getColor(material->emissive, this, "Emissive");
    if (color.isValid())
    {
        material->emissive = color;
        setButtonColor(ui->buttonEmissive, material->emissive);
        emit resourceChanged(material);
    }
}

void MaterialWidget::onButtonEmissiveTextureClicked()
{
    QMenu contextMenu(tr("Context menu"), (QPushButton*)sender());

    QVector<QAction*> actions;
    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Texture * texture = resourceManager->resources[i]->asTexture();
        if (texture != nullptr)
        {
            auto action = new QAction(texture->name, this);
            action->setProperty("texture", QVariant::fromValue<void*>(texture));
            actions.push_back(action);
            connect(action, SIGNAL(triggered()), this, SLOT(onEmissiveTextureChanged()));
            contextMenu.addAction(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MaterialWidget::onEmissiveTextureChanged()
{
    QAction *action = (QAction*)sender();
    Texture *texture = (Texture*)action->property("texture").value<void*>();
    material->emissiveTexture = texture;
    ui->buttonEmissiveTexture->setText(material->emissiveTexture->name);
    emit resourceChanged(material);
}

void MaterialWidget::onButtonSpecularClicked()
{
    QColor color = QColorDialog::getColor(material->specular, this, "Specular");
    if (color.isValid())
    {
        material->specular = color;
        setButtonColor(ui->buttonSpecular, material->specular);
        emit resourceChanged(material);
    }
}

void MaterialWidget::onButtonSpecularTextureClicked()
{
    QMenu contextMenu(tr("Context menu"), (QPushButton*)sender());

    QVector<QAction*> actions;
    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Texture * texture = resourceManager->resources[i]->asTexture();
        if (texture != nullptr)
        {
            auto action = new QAction(texture->name, this);
            action->setProperty("texture", QVariant::fromValue<void*>(texture));
            actions.push_back(action);
            connect(action, SIGNAL(triggered()), this, SLOT(onSpecularTextureChanged()));
            contextMenu.addAction(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MaterialWidget::onSpecularTextureChanged()
{
    QAction *action = (QAction*)sender();
    Texture *texture = (Texture*)action->property("texture").value<void*>();
    material->specularTexture = texture;
    ui->buttonSpecularTexture->setText(material->specularTexture->name);
    emit resourceChanged(material);
}

void MaterialWidget::onButtonNormalTextureClicked()
{
    QMenu contextMenu(tr("Context menu"), (QPushButton*)sender());

    QVector<QAction*> actions;
    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Texture * texture = resourceManager->resources[i]->asTexture();
        if (texture != nullptr)
        {
            auto action = new QAction(texture->name, this);
            action->setProperty("texture", QVariant::fromValue<void*>(texture));
            actions.push_back(action);
            connect(action, SIGNAL(triggered()), this, SLOT(onNormalTextureChanged()));
            contextMenu.addAction(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MaterialWidget::onNormalTextureChanged()
{
    QAction *action = (QAction*)sender();
    Texture *texture = (Texture*)action->property("texture").value<void*>();
    material->normalsTexture = texture;
    ui->buttonNormalTexture->setText(material->normalsTexture->name);
    emit resourceChanged(material);
}

void MaterialWidget::onButtonBumpTextureClicked()
{
    QMenu contextMenu(tr("Context menu"), (QPushButton*)sender());

    QVector<QAction*> actions;
    for (int i = 0; i < resourceManager->resources.size(); ++i)
    {
        Texture * texture = resourceManager->resources[i]->asTexture();
        if (texture != nullptr)
        {
            auto action = new QAction(texture->name, this);
            action->setProperty("texture", QVariant::fromValue<void*>(texture));
            actions.push_back(action);
            connect(action, SIGNAL(triggered()), this, SLOT(onBumpTextureChanged()));
            contextMenu.addAction(action);
        }
    }

    contextMenu.exec(mapToGlobal( ((QPushButton*)sender())->pos() ) );
    for (auto action : actions) { delete action; }
}

void MaterialWidget::onBumpTextureChanged()
{
    QAction *action = (QAction*)sender();
    Texture *texture = (Texture*)action->property("texture").value<void*>();
    material->bumpTexture = texture;
    ui->buttonBumpTexture->setText(material->bumpTexture->name);
    emit resourceChanged(material);
}

void MaterialWidget::onSmoothnessChanged(int value)
{
    material->smoothness = value / 255.0f;
    emit resourceChanged(material);
}

void MaterialWidget::onMetalnessChanged(int value)
{
    material->metalness = value / 255.0f;
    emit resourceChanged(material);
}

void MaterialWidget::onBumpinessChanged(double value)
{
    material->bumpiness = value;
    emit resourceChanged(material);
}

void MaterialWidget::onTilingChanged(double)
{
    material->tiling = QVector2D(ui->spinTilingX->value(), ui->spinTilingY->value());
    emit resourceChanged(material);
}
