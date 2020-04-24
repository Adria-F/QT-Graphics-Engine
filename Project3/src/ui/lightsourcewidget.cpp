#include "lightsourcewidget.h"
#include "ecs/scene.h"
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QColorDialog>
#include <QSignalBlocker>


LightSourceWidget::LightSourceWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(QString::fromLatin1("Light Source"));

    auto vlayout = new QFormLayout;
    vlayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    auto labelType = new QLabel("Type");
    labelType->setMinimumSize(QSize(70, 10));
    comboType = new QComboBox;
    comboType->addItem("Point", QVariant::fromValue<int>((int)LightSource::Type::Point));
    comboType->addItem("Directional", QVariant::fromValue<int>((int)LightSource::Type::Directional));
    vlayout->addRow(labelType, comboType);

    auto labelColor = new QLabel("Color");
    labelColor->setMinimumSize(QSize(70, 10));
    buttonColor = new QPushButton("");
    vlayout->addRow(labelColor, buttonColor);

    auto labelIntensity = new QLabel("Intensity");
    labelIntensity->setMinimumSize(QSize(70, 10));
    spinIntensity = new QDoubleSpinBox();
    spinIntensity->setMinimum(0.0);
    spinIntensity->setMaximum(10000.0);
    spinIntensity->setValue(1.0);
    vlayout->addRow(labelIntensity, spinIntensity);

    auto labelRange = new QLabel("Range");
    labelRange->setMinimumSize(QSize(70, 10));
    spinRange = new QDoubleSpinBox();
    spinRange->setMinimum(0.0);
    spinRange->setMaximum(10000.0);
    spinRange->setValue(1.0);
    vlayout->addRow(labelRange, spinRange);

    setLayout(vlayout);

    connect(comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
    connect(spinIntensity, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));
    connect(spinRange, SIGNAL(valueChanged(double)), this, SLOT(onRangeChanged(double)));
    connect(buttonColor, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
}

void LightSourceWidget::setLightSource(LightSource *light)
{
    lightSource = light;
    if (lightSource == nullptr) return;

    QSignalBlocker b1(comboType);
    QSignalBlocker b2(spinIntensity);
    QSignalBlocker b3(spinRange);
    QSignalBlocker b4(buttonColor);

    comboType->setCurrentIndex((int)lightSource->type);

    spinIntensity->setValue(lightSource->intensity);
    spinRange->setValue(lightSource->range);

    QString colorName = lightSource->color.name();
    buttonColor->setStyleSheet(QString::fromLatin1("background-color: %0").arg(colorName));
}

void LightSourceWidget::onTypeChanged(int index)
{
    lightSource->type = (LightSource::Type)comboType->itemData(index).value<int>();
    emit componentChanged(lightSource);
}

void LightSourceWidget::onIntensityChanged(double val)
{
    lightSource->intensity = val;
    emit componentChanged(lightSource);
}

void LightSourceWidget::onRangeChanged(double val)
{
    lightSource->range = val;
    emit componentChanged(lightSource);
}

void LightSourceWidget::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(lightSource->color, this, "Light color");
    if (color.isValid())
    {
        QString colorName = color.name();
        buttonColor->setStyleSheet(QString::fromLatin1("background-color: %0").arg(colorName));
        lightSource->color = color;
        emit componentChanged(lightSource);
    }
}
