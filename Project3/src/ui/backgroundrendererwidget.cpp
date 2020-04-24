#include "backgroundrendererwidget.h"
#include "scene.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

BackgroundRendererWidget::BackgroundRendererWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Background Renderer");

    auto label = new QLabel;
    label->setText("Background color");

    auto colorButton = new QPushButton;
    colorButton->setText("");
    colorButton->setStyleSheet("background-color: #ffffff;");
    button = colorButton;

    auto layout = new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(colorButton);
    setLayout(layout);

    connect(colorButton, SIGNAL(clicked()), this, SLOT(onColorButtonPressed()));
}

void BackgroundRendererWidget::setBackgroundRenderer(BackgroundRenderer *bgRenderer)
{
    backgroundRenderer = bgRenderer;
    if (backgroundRenderer == nullptr) return;

    QColor col = bgRenderer->color;
    button->setStyleSheet(QString("background-color: %1;").arg(col.name()));
}

void BackgroundRendererWidget::onColorButtonPressed()
{
    QColor color = backgroundRenderer->color;
    color = QColorDialog::getColor(color);
    if (color.isValid())
    {
        button->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        backgroundRenderer->color = color;
        emit componentChanged(backgroundRenderer);
    }
}
