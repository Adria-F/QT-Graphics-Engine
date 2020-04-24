#include "ui/transformwidget.h"
#include "ui_transformwidget.h"
#include "ecs/scene.h"
#include <QSignalBlocker>

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);
    ui->spinSx->setRange(0.001, 1000);
    ui->spinSy->setRange(0.001, 1000);
    ui->spinSz->setRange(0.001, 1000);

    connect(ui->spinTx, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinTy, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinTz, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinRx, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinRy, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinRz, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinSx, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinSy, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(ui->spinSz, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
}

TransformWidget::~TransformWidget()
{
    delete ui;
}

void TransformWidget::setTransform(Transform *t)
{
    transform = t;
    if (transform == nullptr) return;

    QSignalBlocker txb(ui->spinTx);
    QSignalBlocker tyb(ui->spinTy);
    QSignalBlocker tzb(ui->spinTz);
    QSignalBlocker rxb(ui->spinRx);
    QSignalBlocker ryb(ui->spinRy);
    QSignalBlocker rzb(ui->spinRz);
    QSignalBlocker sxb(ui->spinSx);
    QSignalBlocker syb(ui->spinSy);
    QSignalBlocker szb(ui->spinSz);

    ui->spinTx->setValue(t->position.x());
    ui->spinTy->setValue(t->position.y());
    ui->spinTz->setValue(t->position.z());
    ui->spinRx->setValue(t->rotation.toEulerAngles().x());
    ui->spinRy->setValue(t->rotation.toEulerAngles().y());
    ui->spinRz->setValue(t->rotation.toEulerAngles().z());
    ui->spinSx->setValue(t->scale.x());
    ui->spinSy->setValue(t->scale.y());
    ui->spinSz->setValue(t->scale.z());
}

void TransformWidget::onValueChanged(double)
{
    float tx = ui->spinTx->value();
    float ty = ui->spinTy->value();
    float tz = ui->spinTz->value();
    transform->position = QVector3D(tx, ty, tz);

    float rx = ui->spinRx->value(); // pitch
    float ry = ui->spinRy->value(); // yaw
    float rz = ui->spinRz->value(); // roll
    transform->rotation = QQuaternion::fromEulerAngles(rx, ry, rz);

    float sx = ui->spinSx->value();
    float sy = ui->spinSy->value();
    float sz = ui->spinSz->value();
    transform->scale = QVector3D(sx, sy, sz);

    emit componentChanged(transform);
}
