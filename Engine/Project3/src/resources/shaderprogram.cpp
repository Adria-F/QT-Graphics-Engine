#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{
    needsUpdate = true;
}

void ShaderProgram::reload()
{
    needsUpdate = true;
}

void ShaderProgram::update()
{
    program.removeAllShaders();
    if (!vertexShaderFilename.isEmpty())
        program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFilename);
    if (!fragmentShaderFilename.isEmpty())
        program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFilename);
    program.link();
}

void ShaderProgram::destroy()
{
}
