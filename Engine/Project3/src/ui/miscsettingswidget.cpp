#include "miscsettingswidget.h"
#include "ui_miscsettingswidget.h"
#include "globals.h"
#include <QColorDialog>


MiscSettingsWidget::MiscSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiscSettingsWidget)
{
    ui->setupUi(this);

    ui->spinCameraSpeed->setValue(DEFAULT_CAMERA_SPEED);
    ui->spinFovY->setValue(DEFAULT_CAMERA_FOVY);

    ui->outlineColor->setStyleSheet(QString::fromLatin1("background-color: %0").arg(miscSettings->outlineColor.name()));

    connect(ui->spinCameraSpeed, SIGNAL(valueChanged(double)), this, SLOT(onCameraSpeedChanged(double)));
    connect(ui->spinFovY, SIGNAL(valueChanged(double)), this, SLOT(onCameraFovYChanged(double)));
    connect(ui->buttonBackgroundColor, SIGNAL(clicked()), this, SLOT(onBackgroundColorClicked()));
    connect(ui->checkBoxGrid, SIGNAL(clicked()), this, SLOT(onVisualHintChanged()));
    connect(ui->checkBoxLightSources, SIGNAL(clicked()), this, SLOT(onVisualHintChanged()));
    connect(ui->outlineColor, SIGNAL(clicked()), this, SLOT(onOutlineColorClicked()));
    connect(ui->outlineThickness, SIGNAL(valueChanged(double)), this, SLOT(onOutlineThicknessChanged(double)));
    connect(ui->depthFocus, SIGNAL(valueChanged(double)), this, SLOT(onDepthFocusChanged(double)));
    connect(ui->ambientOcclusion, SIGNAL(clicked()), this, SLOT(onAmbientLightToggled()));
    connect(ui->ambientValue, SIGNAL(valueChanged(double)), this, SLOT(onAmbientLightChanged(double)));
}

MiscSettingsWidget::~MiscSettingsWidget()
{
    delete ui;
}

void MiscSettingsWidget::onCameraSpeedChanged(double speed)
{
    camera->maxSpeed = speed;
}

void MiscSettingsWidget::onCameraFovYChanged(double fovy)
{
    camera->fovy = fovy;
    emit settingsChanged();
}

int g_MaxSubmeshes = 100;

void MiscSettingsWidget::onMaxSubmeshesChanged(int n)
{
    g_MaxSubmeshes = n;
    emit settingsChanged();
}

void MiscSettingsWidget::onBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(miscSettings->backgroundColor, this, "Background color");
    if (color.isValid())
    {
        QString colorName = color.name();
        ui->buttonBackgroundColor->setStyleSheet(QString::fromLatin1("background-color: %0").arg(colorName));
        miscSettings->backgroundColor = color;
        emit settingsChanged();
    }
}

void MiscSettingsWidget::onVisualHintChanged()
{
    miscSettings->renderLightSources = ui->checkBoxLightSources->isChecked();
    miscSettings->grid = ui->checkBoxGrid->isChecked();
    emit settingsChanged();
}

void MiscSettingsWidget::onOutlineColorClicked()
{
    QColor color = QColorDialog::getColor(miscSettings->outlineColor, this, "Outline color");
    if (color.isValid())
    {
        QString colorName = color.name();
        ui->outlineColor->setStyleSheet(QString::fromLatin1("background-color: %0").arg(colorName));
        miscSettings->outlineColor = color;
        emit settingsChanged();
    }
}

void MiscSettingsWidget::onOutlineThicknessChanged(double newOutlineThickness)
{
    miscSettings->outlineThickness = newOutlineThickness;
    emit settingsChanged();
}

void MiscSettingsWidget::onDepthFocusChanged(double newDepthFocus){
    camera->depthFocus = newDepthFocus;
    emit settingsChanged();
}

void MiscSettingsWidget::onAmbientLightToggled()
{
    miscSettings->ambientOcclusion = ui->ambientOcclusion->isChecked();
    emit settingsChanged();
}

void MiscSettingsWidget::onAmbientLightChanged(double newAmbientLight)
{
    miscSettings->ambientValue = newAmbientLight;
    emit settingsChanged();
}
