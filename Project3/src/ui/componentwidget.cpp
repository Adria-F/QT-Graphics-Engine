#include "componentwidget.h"
#include "ui_componentwidget.h"

ComponentWidget::ComponentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComponentWidget)
{
    ui->setupUi(this);

    connect(ui->checkboxCollapsed, SIGNAL(toggled(bool)), this, SLOT(collapse(bool)));
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(remove()));
}

ComponentWidget::~ComponentWidget()
{
    delete ui;
}

void ComponentWidget::setWidget(QWidget *widget)
{
    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(widget);
    ui->widget->setLayout(layout);

    ui->labelName->setText(widget->windowTitle());
}

void ComponentWidget::setComponent(Component *c)
{
    component = c;

    setVisible(c != nullptr);
}

void ComponentWidget::collapse(bool c)
{
    ui->widget->setVisible(!c);

    if (c) {
        emit collapsed();
    } else {
        emit expanded();
    }
}

void ComponentWidget::remove()
{
    emit removeClicked(component);
}
