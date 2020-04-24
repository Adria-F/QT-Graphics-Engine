#ifndef RESOURCEWIDGET_H
#define RESOURCEWIDGET_H

#include <QWidget>

namespace Ui {
class ResourceWidget;
}

class Resource;

class ResourceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResourceWidget(QWidget *parent = nullptr);
    ~ResourceWidget();

    void setResource(Resource *r);

signals:

    void resourceChanged(Resource *);

public slots:

    void onReturnPressed();
    void clearFocus();

private:
    Ui::ResourceWidget *ui;
    Resource *resource = nullptr;
};

#endif // RESOURCEWIDGET_H
