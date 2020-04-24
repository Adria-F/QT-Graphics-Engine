#include "meshwidget.h"
#include "ui_meshwidget.h"
#include "resources/mesh.h"
#include "resources/resourcemanager.h"
#include "globals.h"
#include <QFileDialog>


MeshWidget::MeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeshWidget)
{
    ui->setupUi(this);
    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

MeshWidget::~MeshWidget()
{
    delete ui;
}

void MeshWidget::setMesh(Mesh *m)
{
    mesh = m;
    if (mesh == nullptr) return;

    // Bounds
    QString info = QString(
                "Bounds:\n"
                " - min: (%1, %2, %3)\n"
                " - max: (%4, %5, %6)\n")
            .arg(m->bounds.min.x()).arg(m->bounds.min.y()).arg(m->bounds.min.z())
            .arg(m->bounds.max.x()).arg(m->bounds.max.y()).arg(m->bounds.max.z());

    // Submeshes
    for (unsigned int i = 0; i < m->submeshes.size(); ++i)
    {
        auto submesh = m->submeshes[i];
        info += QString("Submesh %0:\n").arg(i);
        info += QString(" - v. count: %0\n").arg(submesh->vertexCount());
    }

    QFileInfo fileInfo(mesh->getFilePath());
    QString filename = fileInfo.baseName();

    ui->textFile->setText(filename);
    ui->textMeshInfo->setText(info);
}

void MeshWidget::onButtonClicked()
{
    if (mesh == nullptr) return;

//    QString path = QFileDialog::getOpenFileName(this,"Load model file", QString(), QString::fromLatin1("3D meshes (*.obj *.fbx)"));
//    if (!path.isEmpty())
//    {
//        mesh->loadModel(path.toLatin1());
//        emit resourceChanged(mesh);
//    }
}
