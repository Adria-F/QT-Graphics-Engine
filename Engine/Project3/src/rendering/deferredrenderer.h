#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "renderer.h"
#include "gl.h"

class ShaderProgram;
class FramebufferObject;

class DeferredRenderer : public Renderer
{
public:
    DeferredRenderer();
    ~DeferredRenderer() override;


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

    GLuint fboNormalsColor = 0;
    GLuint fboDepthColor = 0;
    GLuint fboDepth = 0;
    FramebufferObject *fbo = nullptr;

};

#endif // DEFERREDRENDERER_H
