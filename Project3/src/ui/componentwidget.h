#ifndef COMPONENTWIDGET_H
#define COMPONENTWIDGET_H

#include <QWidget>

namespace Ui {
class ComponentWidget;
}

class Component;

class ComponentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComponentWidget(QWidget *parent = nullptr);
    ~ComponentWidget();

    void setWidget(QWidget *widget);
    void setComponent(Component *component);

signals:

    void removeClicked(Component *c);
    void collapsed();
    void expanded();

public slots:

    void collapse(bool c);
    void remove();

private:
    Ui::ComponentWidget *ui;
    Component *component = nullptr;
};

#endif // COMPONENTWIDGET_H
