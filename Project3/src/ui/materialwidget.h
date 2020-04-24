#ifndef MATERIALWIDGET_H
#define MATERIALWIDGET_H

#include <QWidget>

namespace Ui {
class MaterialWidget;
}

class Material;
class Resource;

class MaterialWidget : public QWidget
{
    Q_OBJECT

public:

    explicit MaterialWidget(QWidget *parent = nullptr);
    ~MaterialWidget();

    void setMaterial(Material *);

signals:

    void resourceChanged(Resource *);

public slots:

    void onShaderChanged(int index);
    void onButtonAlbedoClicked();
    void onButtonAlbedoTextureClicked();
    void onAlbedoTextureChanged();
    void onButtonEmissiveClicked();
    void onButtonEmissiveTextureClicked();
    void onEmissiveTextureChanged();
    void onButtonSpecularClicked();
    void onButtonSpecularTextureClicked();
    void onSpecularTextureChanged();
    void onButtonNormalTextureClicked();
    void onNormalTextureChanged();
    void onButtonBumpTextureClicked();
    void onBumpTextureChanged();
    void onSmoothnessChanged(int);
    void onMetalnessChanged(int);
    void onBumpinessChanged(double);
    void onTilingChanged(double);

private:
    Ui::MaterialWidget *ui;

    Material *material = nullptr;
};

#endif // MATERIALWIDGET_H
