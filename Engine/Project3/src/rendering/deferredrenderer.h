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

    GLuint fboPosition = 0;
    GLuint fboNormals = 0;
    GLuint fboColor = 0;
    // Unused
    GLuint fboSpecular = 0;
    GLuint fboDepth = 0;
    GLuint grid = 0; // Maybe create another shader

    FramebufferObject *fbo = nullptr;

};

#endif // DEFERREDRENDERER_H
