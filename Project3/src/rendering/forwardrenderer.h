#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "renderer.h"
#include "gl.h"

class ShaderProgram;
class FramebufferObject;

class ForwardRenderer : public Renderer
{
public:

    ForwardRenderer();
    ~ForwardRenderer() override;

    void initialize() override;
    void finalize() override;

    void resize(int width, int height) override;
    void render(Camera *camera) override;

private:

    void passMeshes(Camera *camera);
    void passBlit();

    // Shaders
    ShaderProgram *forwardProgram = nullptr;
    ShaderProgram *blitProgram;

    GLuint fboColor = 0;
    GLuint fboDepth = 0;
    FramebufferObject *fbo = nullptr;
};

#endif // FORWARDRENDERER_H
