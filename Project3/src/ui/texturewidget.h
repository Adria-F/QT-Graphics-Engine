#ifndef TEXTUREWIDGET_H
#define TEXTUREWIDGET_H

#include <QWidget>

namespace Ui {
class TextureWidget;
}

class Resource;
class Texture;

class TextureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TextureWidget(QWidget *parent = nullptr);
    ~TextureWidget();

    void setTexture(Texture *tex);

signals:

    void resourceChanged(Resource *);

public slots:

    void onButtonClicked();

private:

    Ui::TextureWidget *ui = nullptr;
    Texture *texture = nullptr;
};

#endif // TEXTUREWIDGET_H
