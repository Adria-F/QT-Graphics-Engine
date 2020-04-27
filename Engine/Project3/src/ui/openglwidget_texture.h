#ifndef OPENGLWIDGETTEXTURE_H
#define OPENGLWIDGETTEXTURE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

class Texture;

class OpenGLWidgetTexture :
        public QOpenGLWidget,
        protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:

    explicit OpenGLWidgetTexture(QWidget *parent = nullptr);
    ~OpenGLWidgetTexture() override;

    void setTexture(Texture *t);

    // Virtual OpenGL methods
    void initializeGL() override;
    void paintGL() override;

private:

    Texture *texture = nullptr;
    QOpenGLShaderProgram program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
};

#endif // OPENGLWIDGETTEXTURE_H
