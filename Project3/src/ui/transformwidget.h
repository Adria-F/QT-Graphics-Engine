#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>

namespace Ui {
class TransformWidget;
}

class Transform;
class Component;

class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(QWidget *parent = nullptr);
    ~TransformWidget();

    void setTransform(Transform *t);

signals:

    void componentChanged(Component *);

public slots:

    void onValueChanged(double);

private:
    Ui::TransformWidget *ui;
    Transform *transform;
};

#endif // TRANSFORMWIDGET_H
