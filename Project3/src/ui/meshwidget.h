#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include <QWidget>

namespace Ui {
class MeshWidget;
}

class Resource;
class Mesh;

class MeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshWidget(QWidget *parent = nullptr);
    ~MeshWidget();

    void setMesh(Mesh *mesh);

signals:

    void resourceChanged(Resource *);

public slots:

    void onButtonClicked();

private:

    Ui::MeshWidget *ui = nullptr;
    Mesh *mesh = nullptr;
};

#endif // MESHWIDGET_H
