#ifndef BACKGROUNDRENDERERWIDGET_H
#define BACKGROUNDRENDERERWIDGET_H

#include <QWidget>
#include <QColor>

class BackgroundRenderer;
class Component;

class BackgroundRendererWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundRendererWidget(QWidget *parent = nullptr);

    void setBackgroundRenderer(BackgroundRenderer *bgRenderer);

signals:

    void componentChanged(Component *);

public slots:

    void onColorButtonPressed();

private:

    BackgroundRenderer *backgroundRenderer = nullptr;
    QWidget *button = nullptr;
};

#endif // BACKGROUNDRENDERERWIDGET_H
