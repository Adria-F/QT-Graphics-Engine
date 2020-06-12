#ifndef MISCSETTINGSWIDGET_H
#define MISCSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class MiscSettingsWidget;
}

class MiscSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MiscSettingsWidget(QWidget *parent = nullptr);
    ~MiscSettingsWidget();

signals:

    void settingsChanged();

public slots:

    void onCameraSpeedChanged(double speed);
    void onCameraFovYChanged(double speed);
    void onMaxSubmeshesChanged(int n);
    void onBackgroundColorClicked();
    void onVisualHintChanged();
    void onOutlineColorClicked();
    void onOutlineThicknessChanged(double newOutlineThickness);
    void onDepthFocusChanged(double newDepthFocus);
    void onFallofStartMarginChanged(double newFallofStartMargin);
    void onFallofEndMarginChanged(double newFallofEndMargin);
    void onAmbientLightToggled();
    void onAmbientLightChanged(double newAmbientLight);

private:
    Ui::MiscSettingsWidget *ui;
};

#endif // MISCSETTINGSWIDGET_H
