#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugMessage>
#include <QTimer>

class Input;
class Camera;
class Interaction;
class Selection;
class Renderer;

class OpenGLWidget :
        public QOpenGLWidget,
        protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget() override;

    // Virtual OpenGL methods
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // Virtual event methods
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    // Public methods
    QString getOpenGLInfo();
    QImage getScreenshot();

    // Texture created by renderers
    QVector<QString> getTextureNames();
    void showTextureWithName(QString textureName);

signals:

    void interacted();

public slots:

    // Not virtual
    void frame();
    void finalizeGL();

    void handleLoggedMessage(const QOpenGLDebugMessage &debugMessage);

private:

    QTimer timer;

    Input *input = nullptr;
    Camera *camera = nullptr;
    Interaction *interaction = nullptr;
    Selection *selection = nullptr;
    Renderer *renderer = nullptr;
};

#endif // OPENGLWIDGET_H
