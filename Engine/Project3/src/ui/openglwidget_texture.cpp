#include "ui/openglwidget_texture.h"
#include <QVector3D>
#include <iostream>
#include "resources/texture.h"
#include "resources/resourcemanager.h"
#include "globals.h"
#include <QVector3D>


OpenGLWidgetTexture::OpenGLWidgetTexture(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(QSize(128, 128));
}

OpenGLWidgetTexture::~OpenGLWidgetTexture()
{
}

void OpenGLWidgetTexture::setTexture(Texture *t)
{
    texture = t;
    update();
}

void OpenGLWidgetTexture::initializeGL()
{
    if (!initializeOpenGLFunctions())
    {
        qDebug("Error in initializeOpenGLFunctions()");
        return;
    }

    // Program
    program.create();
    program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture_view.vert");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture_view.frag");
    if (!program.link()) {
        qDebug("Error linking texture_view shaders.");
        return;
    }

    // VAO: Vertex format description and state of VBOs
    vao.create();
    vao.bind();

    // VBO: Buffer with vertex data
    QVector3D data[6] = {
        QVector3D(-0.5f,-0.5f, 0.0f),
        QVector3D( 0.5f,-0.5f, 0.0f),
        QVector3D( 0.5f, 0.5f, 0.0f),
        QVector3D(-0.5f,-0.5f, 0.0f),
        QVector3D( 0.5f, 0.5f, 0.0f),
        QVector3D(-0.5f, 0.5f, 0.0f)
    };
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    vbo.allocate(data, sizeof(data));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);

    // Release
    vao.release();
    vbo.release();
}

void OpenGLWidgetTexture::paintGL()
{
    resourceManager->updateResources();

    glClearDepth(1.0);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    Texture *tex = texture;
    if (tex == nullptr) {
        tex = resourceManager->texWhite;
    }

    const float widgetAR = static_cast<float>(width()) / height();
    const float textureAR = static_cast<float>(tex->width()) / tex->height();

    if (program.bind())
    {
        vao.bind();

        QMatrix4x4 projectionMatrix;
        projectionMatrix.ortho(-width()/2, width()/2, -height()/2, height()/2, -1.0, 1.0);
        program.setUniformValue("projectionMatrix", projectionMatrix);

        QVector3D scale(textureAR, 1.0, 1.0);
        if (widgetAR < textureAR) {
            scale = scale * QVector3D(width()*0.9, width()*0.9, 1.0);
        } else {
            scale = scale * QVector3D(height()*0.9, height()*0.9, 1.0);
        }
        program.setUniformValue("scale", scale);

        const int textureUnit = 0;
        tex->bind(textureUnit);
        program.setUniformValue("colorMap", textureUnit);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        vao.release();
        program.release();
    }
}
