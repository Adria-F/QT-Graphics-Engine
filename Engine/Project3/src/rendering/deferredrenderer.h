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

    void passGrid(Camera* camera);
    void passMeshes(Camera *camera);
    void passLights(Camera *camera);
    void passBlit();

    // Shaders

    ShaderProgram *deferredGeometryProgram = nullptr;
    ShaderProgram *deferredLightingProgram = nullptr;
    ShaderProgram *gridProgram = nullptr;
    ShaderProgram *blitProgram = nullptr;

    FramebufferObject *fboInfo = nullptr;
    GLuint fboPosition = 0;
    GLuint fboNormal = 0;
    GLuint fboColor = 0;
    GLuint fboGrid = 0;


    FramebufferObject *fboFinal = nullptr;
    GLuint fboFinalRender = 0;
    GLuint fboLightCircles = 0;

    // Unused
    GLuint fboSpecular = 0;
    GLuint fboDepth = 0;
    GLuint grid = 0; // Maybe create another shader




};

#endif // DEFERREDRENDERER_H
