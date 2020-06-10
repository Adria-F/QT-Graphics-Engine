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

    void renderIdentifiers(Camera* camera);
    unsigned int getClickedIdentifier(int x, int y);

private:

    void passGrid(Camera* camera);
    void passMeshes(Camera *camera);
    void passLights(Camera *camera);
    void passIdentifiers(Camera *camera);
    void passOutline();
    void passDOF();
    void finalMix();
    void passBlit();

    // Shaders

    ShaderProgram *deferredGeometryProgram = nullptr;
    ShaderProgram *deferredLightingProgram = nullptr;
    ShaderProgram *gridProgram = nullptr;
    ShaderProgram *DOFProgram = nullptr;
    ShaderProgram *blitProgram = nullptr;
    ShaderProgram *mousePickProgram = nullptr;
    ShaderProgram *outlineProgram = nullptr;

    FramebufferObject *fboInfo = nullptr;
    GLuint fboPosition = 0;
    GLuint fboNormal = 0;
    GLuint fboColor = 0;
    GLuint fboGrid = 0;
    GLuint fboMask = 0;

    FramebufferObject *fboMousePick = nullptr;
    GLuint fboIdentifiers = 0;

    FramebufferObject *fboLight = nullptr;
    GLuint fboLighting = 0;
    GLuint fboLightCircles = 0;

    FramebufferObject *fboPostProcess = nullptr;
    GLuint fboDOFV = 0;
    GLuint fboDOF = 0;
    GLuint fboOutline = 0;

    FramebufferObject *fboFinal = nullptr;
    GLuint fboFinalTexture;

    // Unused
    GLuint fboSpecular = 0;
    GLuint fboDepth = 0;
};

#endif // DEFERREDRENDERER_H
