#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "resource.h"
#include <QOpenGLShaderProgram>


class ShaderProgram : public Resource
{
public:
    ShaderProgram();

    virtual const char *typeName() const override { return "ShaderProgram"; }

    void reload();

    virtual ShaderProgram * asShaderProgram() override { return this; }

    void update() override;
    void destroy() override;

    void read(const QJsonObject &) override { }
    void write(QJsonObject &) override { }

    QString vertexShaderFilename;
    QString fragmentShaderFilename;
    QOpenGLShaderProgram program;
};

#endif // SHADERPROGRAM_H
