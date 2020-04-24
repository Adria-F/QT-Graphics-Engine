#ifndef MESHRENDERERWIDGET_H
#define MESHRENDERERWIDGET_H

#include <QWidget>

class MeshRenderer;
class Material;
class Component;
class QComboBox;
class QPushButton;
class QVBoxLayout;

class MeshRendererWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshRendererWidget(QWidget *parent = nullptr);
    ~MeshRendererWidget();

    void setMeshRenderer(MeshRenderer *s);

signals:

    void componentChanged(Component *);

public slots:

    void updateLayout();
    void onMeshChanged(int index);
    void onMaterialSelect();
    void onMaterialChanged();
    void onAddMaterial();


private:

    void destroyLayout();
    QComboBox *createMeshesCombo();
    QVBoxLayout* createMaterialsLayout();
    QPushButton * createButtonForMaterialSlot(Material*, int slotIndex);

    QComboBox *comboMesh = nullptr;
    MeshRenderer *meshRenderer = nullptr;
    QVBoxLayout *materialsList = nullptr;
};

#endif // MESHRENDERERWIDGET_H
