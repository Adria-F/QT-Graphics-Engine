#include "deferredrenderer.h"
#include "miscsettings.h"
#include "ecs/scene.h"
#include "ecs/camera.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/texture.h"
#include "resources/shaderprogram.h"
#include "resources/resourcemanager.h"
#include "framebufferobject.h"
#include "gl.h"
#include "globals.h"
#include <QVector>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QRandomGenerator>
#include <time.h>


static void sendLightsToProgram(QOpenGLShaderProgram &program, const QMatrix4x4 &worldMatrix)
{
    QVector<int> lightType;
    QVector<QVector3D> lightPosition;
    QVector<QVector3D> lightDirection;
    QVector<QVector3D> lightColor;
    for (auto entity : scene->entities)
    {
        if (entity->active && entity->lightSource != nullptr)
        {
            auto light = entity->lightSource;
            auto transform = *entity->transform;

            lightType.push_back(int(light->type));
            lightPosition.push_back(QVector3D(transform.matrix() * QVector4D(0.0, 0.0, 0.0, 1.0)));
            lightDirection.push_back(QVector3D(transform.matrix() * QVector4D(0.0, 1.0, 0.0, 0.0)));
            QVector3D color(light->color.redF(), light->color.greenF(), light->color.blueF());
            lightColor.push_back(color * light->intensity);
        }
    }
    if (lightPosition.size() > 0)
    {
        program.setUniformValueArray("lightType", &lightType[0], lightType.size());
        program.setUniformValueArray("lightPosition", &lightPosition[0], lightPosition.size());
        program.setUniformValueArray("lightDirection", &lightDirection[0], lightDirection.size());
        program.setUniformValueArray("lightColor", &lightColor[0], lightColor.size());
    }
    program.setUniformValue("lightCount", lightPosition.size());
}

DeferredRenderer::DeferredRenderer() :
    fboPosition(QOpenGLTexture::Target2D),
    fboNormal(QOpenGLTexture::Target2D),
    fboColor(QOpenGLTexture::Target2D),
    fboAO(QOpenGLTexture::Target2D),
    fboNoise(QOpenGLTexture::Target2D),
    fboMask(QOpenGLTexture::Target2D),
    fboIdentifiers(QOpenGLTexture::Target2D),
    fboDepth(QOpenGLTexture::Target2D),
    fboGrid(QOpenGLTexture::Target2D),
    fboDOFV(QOpenGLTexture::Target2D),
    fboDOF(QOpenGLTexture::Target2D),
    fboFinalTexture(QOpenGLTexture::Target2D)

{
    fboInfo = nullptr;
    fboMousePick = nullptr;
    fboLight = nullptr;
    fboFinal = nullptr;
    fboPostProcess = nullptr;

    // List of textures
    addTexture("Final render");
    addTexture("Lighting");
    addTexture("Position");
    addTexture("Normals");
    addTexture("Color");
    addTexture("Ambient Occlusion");
    addTexture("Grid");
    addTexture("DOF");
    addTexture("Object Identifiers");
    addTexture("Light Circles");
    addTexture("Depth");
}

DeferredRenderer::~DeferredRenderer()
{
    delete fboInfo;
    delete fboMousePick;
    delete fboLight;
    delete fboPostProcess;
    delete fboFinal;
}

void DeferredRenderer::initialize()
{
    OpenGLErrorGuard guard("DeferredRenderer::initialize()");

    // Create programs
    ///Deferred Geometry
    deferredGeometryProgram = resourceManager->createShaderProgram();
    deferredGeometryProgram->name = "Forward shading";
    deferredGeometryProgram->vertexShaderFilename = "res/shaders/forward_shader/standard_shading.vert";
    deferredGeometryProgram->fragmentShaderFilename = "res/shaders/lighting/deferred_geometry.frag";
    deferredGeometryProgram->includeForSerialization = false;

    ///Ambient Occlusion
    ssaoProgram = resourceManager->createShaderProgram();
    ssaoProgram->name = "Ambient Occlusion";
    ssaoProgram->vertexShaderFilename = "res/shaders/ssao/ssao.vert";
    ssaoProgram->fragmentShaderFilename = "res/shaders/ssao/ssao.frag";
    ssaoProgram->includeForSerialization = false;

    ///Mouse Picking
    mousePickProgram = resourceManager->createShaderProgram();
    mousePickProgram->name = "Mouse Picking";
    mousePickProgram->vertexShaderFilename = "res/shaders/mouse_picking/mousePicking.vert";
    mousePickProgram->fragmentShaderFilename = "res/shaders/mouse_picking/mousePicking.frag";
    mousePickProgram->includeForSerialization = false;

    ///Selection Mask
    maskProgram = resourceManager->createShaderProgram();
    maskProgram->name = "Mask";
    maskProgram->vertexShaderFilename = "res/shaders/forward_shader/standard_shading.vert";
    maskProgram->fragmentShaderFilename = "res/shaders/outline/mask.frag";
    maskProgram->includeForSerialization = false;

    ///Outline
    outlineProgram = resourceManager->createShaderProgram();
    outlineProgram->name = "Outline";
    outlineProgram->vertexShaderFilename = "res/shaders/outline/outline.vert";
    outlineProgram->fragmentShaderFilename = "res/shaders/outline/outline.frag";
    outlineProgram->includeForSerialization = false;

    ///Grid
    gridProgram = resourceManager->createShaderProgram();
    gridProgram->name = "Grid";
    gridProgram->vertexShaderFilename = "res/shaders/grid/grid.vert";
    gridProgram->fragmentShaderFilename = "res/shaders/grid/grid.frag";
    gridProgram->includeForSerialization = false;

    ///Deferred Lighting
    deferredLightingProgram = resourceManager->createShaderProgram();
    deferredLightingProgram->name = "Deferred Lighting";
    deferredLightingProgram->vertexShaderFilename = "res/shaders/forward_shader/standard_shading.vert";
    deferredLightingProgram->fragmentShaderFilename = "res/shaders/lighting/deferred_lighting.frag";
    deferredLightingProgram->includeForSerialization = false;

    ///Ambient Lighting
    ambientLightingProgram = resourceManager->createShaderProgram();
    ambientLightingProgram->name = "Ambient Lighting";
    ambientLightingProgram->vertexShaderFilename = "res/shaders/lighting/ambientLighting.vert";
    ambientLightingProgram->fragmentShaderFilename = "res/shaders/lighting/ambientLighting.frag";
    ambientLightingProgram->includeForSerialization = false;

    /// DOF
    DOFProgram = resourceManager->createShaderProgram();
    DOFProgram->name = "DOF";
    DOFProgram->vertexShaderFilename = "res/shaders/dof/dof.vert";
    DOFProgram->fragmentShaderFilename = "res/shaders/dof/dof.frag";
    DOFProgram->includeForSerialization = false;

    ///Blit to screen
    blitProgram = resourceManager->createShaderProgram();
    blitProgram->name = "Blit";
    blitProgram->vertexShaderFilename = "res/shaders/blit/blit.vert";
    blitProgram->fragmentShaderFilename = "res/shaders/blit/blit.frag";
    blitProgram->includeForSerialization = false;


    // Create FBO
    fboInfo = new FramebufferObject;
    fboInfo->create();

    fboSSAO = new FramebufferObject;
    fboSSAO->create();

    fboMousePick = new FramebufferObject;
    fboMousePick->create();

    fboLight = new FramebufferObject;
    fboLight->create();

    fboPostProcess = new FramebufferObject;
    fboPostProcess->create();

    fboFinal = new FramebufferObject;
    fboFinal->create();

    //Create SSAO Kernel
    QRandomGenerator generator;
    generator.seed((time(NULL)));
    for (unsigned int i = 0; i< 64; ++i)
    {
        QVector3D sample(
                    generator.generateDouble() * 2.0 - 1.0,
                    generator.generateDouble() * 2.0 - 1.0,
                    generator.generateDouble()
                    );
        sample.normalize();
        sample *= generator.generateDouble();
        float scale = (float)i / 64.0;
        scale = 0.1f+ scale*scale * (1.0f-0.1f);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    //Create 4x4 texture of random vectors
    QVector<QVector3D> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        QVector3D noise(
                    generator.generateDouble() * 2.0 - 1.0,
                    generator.generateDouble() * 2.0 - 1.0,
                    0.0f
                    );
        ssaoNoise.push_back(noise);
    }
    gl->glGenTextures(1, &fboNoise);
    gl->glBindTexture(GL_TEXTURE_2D, fboNoise);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
}

void DeferredRenderer::finalize()
{
    fboInfo->destroy();
    delete fboInfo;

    fboSSAO->destroy();
    delete fboSSAO;

    fboMousePick->destroy();
    delete fboMousePick;

    fboLight->destroy();
    delete fboLight;

    fboPostProcess->destroy();
    delete fboPostProcess;

    fboFinal->destroy();
    delete fboFinal;


}

void DeferredRenderer::resize(int w, int h)
{
    OpenGLErrorGuard guard("DeferredRenderer::resize()");

    // Regenerate render targets

    if (fboLighting == 0) gl->glDeleteTextures(1, &fboLighting);
    gl->glGenTextures(1, &fboLighting);
    gl->glBindTexture(GL_TEXTURE_2D, fboLighting);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (fboPosition == 0) gl->glDeleteTextures(1, &fboPosition);
    gl->glGenTextures(1, &fboPosition);
    gl->glBindTexture(GL_TEXTURE_2D, fboPosition);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

    if (fboNormal == 0) gl->glDeleteTextures(1, &fboNormal);
    gl->glGenTextures(1, &fboNormal);
    gl->glBindTexture(GL_TEXTURE_2D, fboNormal);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

    if (fboColor == 0) gl->glDeleteTextures(1, &fboColor);
    gl->glGenTextures(1, &fboColor);
    gl->glBindTexture(GL_TEXTURE_2D, fboColor);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (fboAO == 0) gl->glDeleteTextures(1, &fboAO);
    gl->glGenTextures(1, &fboAO);
    gl->glBindTexture(GL_TEXTURE_2D, fboAO);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);

    if (fboIdentifiers == 0) gl->glDeleteTextures(1, &fboIdentifiers);
    gl->glGenTextures(1, &fboIdentifiers);
    gl->glBindTexture(GL_TEXTURE_2D, fboIdentifiers);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, nullptr);

    if (fboMask == 0) gl->glDeleteTextures(1, &fboMask);
    gl->glGenTextures(1, &fboMask);
    gl->glBindTexture(GL_TEXTURE_2D, fboMask);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    if (fboOutline == 0) gl->glDeleteTextures(1, &fboOutline);
    gl->glGenTextures(1, &fboOutline);
    gl->glBindTexture(GL_TEXTURE_2D, fboOutline);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (fboGrid == 0) gl->glDeleteTextures(1, &fboGrid);
    gl->glGenTextures(1, &fboGrid);
    gl->glBindTexture(GL_TEXTURE_2D, fboGrid);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (fboLightCircles == 0) gl->glDeleteTextures(1, &fboLightCircles);
    gl->glGenTextures(1, &fboLightCircles);
    gl->glBindTexture(GL_TEXTURE_2D, fboLightCircles);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);


    if (fboDepth == 0) gl->glDeleteTextures(1, &fboDepth);
    gl->glGenTextures(1, &fboDepth);
    gl->glBindTexture(GL_TEXTURE_2D, fboDepth);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    if(fboDOFV == 0) gl->glDeleteTextures(1, &fboDOFV);
    gl->glGenTextures(1, &fboDOFV);
    gl->glBindTexture(GL_TEXTURE_2D, fboDOFV);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if(fboDOF == 0) gl->glDeleteTextures(1, &fboDOF);
    gl->glGenTextures(1, &fboDOF);
    gl->glBindTexture(GL_TEXTURE_2D, fboDOF);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);


    if (fboFinalTexture == 0) gl->glDeleteTextures(1, &fboFinalTexture);
    gl->glGenTextures(1, &fboFinalTexture);
    gl->glBindTexture(GL_TEXTURE_2D, fboFinalTexture);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    //Set color attachments
    fboInfo->bind();
    fboInfo->addColorAttachment(0, fboPosition);
    fboInfo->addColorAttachment(1, fboNormal);
    fboInfo->addColorAttachment(2, fboColor);
    fboInfo->addColorAttachment(3, fboGrid);    
    fboInfo->addDepthAttachment(fboDepth);
    fboInfo->release();

    fboSSAO->bind();
    fboSSAO->addColorAttachment(0, fboAO);
    fboSSAO->release();

    fboMousePick->bind();
    fboMousePick->addColorAttachment(0, fboIdentifiers);
    fboMousePick->addDepthAttachment(fboDepth);
    fboMousePick->release();

    fboLight->bind();
    fboLight->addColorAttachment(0, fboLighting);
    fboLight->addColorAttachment(1, fboLightCircles);
    fboLight->release();

    fboPostProcess->bind();
    fboPostProcess->addColorAttachment(0, fboDOFV);
    fboPostProcess->addColorAttachment(1, fboDOF);
    fboPostProcess->addColorAttachment(2, fboMask);
    fboPostProcess->addColorAttachment(3, fboOutline);
    fboPostProcess->release();

    fboFinal->bind();
    fboFinal->addColorAttachment(0, fboFinalTexture);
    fboFinal->release();
}

void DeferredRenderer::render(Camera *camera)
{
    OpenGLErrorGuard guard("DeferredRenderer::render()");

    // Passes
    fboInfo->bind();
    passMeshes(camera);
    passGrid(camera);
    fboInfo->release();
    fboSSAO->bind();
    passSSAO(camera);
    fboSSAO->release();
    fboLight->bind();
    passAmbient();
    passLights(camera);
    fboLight->release();
    fboPostProcess->bind();
    passMask(camera);
    passOutline();
    passDOF();
    fboPostProcess->release();

    fboFinal->bind();
    finalMix();
    fboFinal->release();


    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    passBlit();
}

void DeferredRenderer::renderIdentifiers(Camera* camera)
{
    fboMousePick->bind();
    passIdentifiers(camera);
    fboMousePick->release();
}

unsigned int DeferredRenderer::getClickedIdentifier(int x, int y)
{
    fboMousePick->bind();

    float data[3];
    gl->glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, data);

    fboMousePick->release();

    int r = data[0]*255;
    int g = data[1]*255;
    int b = data[2]*255;

    return (r+g*256+b*256*256);
}

void DeferredRenderer::passMeshes(Camera *camera)
{
    QOpenGLShaderProgram &program = deferredGeometryProgram->program;

    if (program.bind())
    {
        // Set FBO buffers
        unsigned int attachments_info[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        gl->glDrawBuffers(3, attachments_info);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Set uniforms
        program.setUniformValue("viewMatrix", camera->viewMatrix);
        program.setUniformValue("projectionMatrix", camera->projectionMatrix);

        sendLightsToProgram(program, camera->worldMatrix);

        QVector<MeshRenderer*> meshRenderers;
        QVector<LightSource*> lightSources;

        // Get components
        for (auto entity : scene->entities)
        {
            if (entity->active)
            {
                if (entity->meshRenderer != nullptr) { meshRenderers.push_back(entity->meshRenderer); }
                if (entity->lightSource != nullptr) { lightSources.push_back(entity->lightSource); }
            }
        }

        // Meshes
        for (auto meshRenderer : meshRenderers)
        {
            auto mesh = meshRenderer->mesh;

            if (mesh != nullptr)
            {
                QMatrix4x4 worldMatrix = meshRenderer->entity->transform->matrix();
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix;
                QMatrix3x3 normalMatrix = worldViewMatrix.normalMatrix();

                program.setUniformValue("worldMatrix", worldMatrix);
                program.setUniformValue("worldViewMatrix", worldViewMatrix);
                program.setUniformValue("normalMatrix", normalMatrix);

                int materialIndex = 0;
                for (auto submesh : mesh->submeshes)
                {
                    // Get material from the component
                    Material *material = nullptr;
                    if (materialIndex < meshRenderer->materials.size()) {
                        material = meshRenderer->materials[materialIndex];
                    }
                    if (material == nullptr) {
                        material = resourceManager->materialWhite;
                    }
                    materialIndex++;

#define SEND_TEXTURE(uniformName, tex1, tex2, texUnit) \
    program.setUniformValue(uniformName, texUnit); \
    if (tex1 != nullptr) { \
    tex1->bind(texUnit); \
                } else { \
    tex2->bind(texUnit); \
                }

                    // Send the material to the shader
                    program.setUniformValue("albedo", material->albedo);
                    program.setUniformValue("emissive", material->emissive);
                    program.setUniformValue("specular", material->specular);
                    program.setUniformValue("smoothness", material->smoothness);
                    program.setUniformValue("bumpiness", material->bumpiness);
                    program.setUniformValue("tiling", material->tiling);
                    SEND_TEXTURE("albedoTexture", material->albedoTexture, resourceManager->texWhite, 0);
                    SEND_TEXTURE("emissiveTexture", material->emissiveTexture, resourceManager->texBlack, 1);
                    SEND_TEXTURE("specularTexture", material->specularTexture, resourceManager->texBlack, 2);
                    SEND_TEXTURE("normalTexture", material->normalsTexture, resourceManager->texNormal, 3);
                    SEND_TEXTURE("bumpTexture", material->bumpTexture, resourceManager->texWhite, 4);

                    submesh->draw();
                }
            }
        }

        // Light spheres
        if (miscSettings->renderLightSources)
        {
            for (auto lightSource : lightSources)
            {
                QMatrix4x4 worldMatrix = lightSource->entity->transform->matrix();
                QMatrix4x4 scaleMatrix; scaleMatrix.scale(0.1f, 0.1f, 0.1f);
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix * scaleMatrix;
                QMatrix3x3 normalMatrix = worldViewMatrix.normalMatrix();
                program.setUniformValue("worldMatrix", worldMatrix);
                program.setUniformValue("worldViewMatrix", worldViewMatrix);
                program.setUniformValue("normalMatrix", normalMatrix);

                for (auto submesh : resourceManager->sphere->submeshes)
                {
                    // Send the material to the shader
                    Material *material = resourceManager->materialLight;
                    program.setUniformValue("albedo", material->albedo);
                    program.setUniformValue("emissive", material->emissive);
                    program.setUniformValue("smoothness", material->smoothness);

                    submesh->draw();
                }
            }
        }

        program.release();
    }
}

void DeferredRenderer::passGrid(Camera *camera){
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl->glDepthMask(GL_FALSE);

    QOpenGLShaderProgram &program = gridProgram->program;

    if(program.bind()){

        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT3);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        QVector4D cameraParameters = camera->getLeftRightBottomTop();

        // Grid parameters
        program.setUniformValue("left", cameraParameters.x());
        program.setUniformValue("right", cameraParameters.y());
        program.setUniformValue("bottom", cameraParameters.z());
        program.setUniformValue("top", cameraParameters.w());
        program.setUniformValue("znear", camera->znear);
        program.setUniformValue("worldMatrix", camera->worldMatrix);
        program.setUniformValue("viewMatrix", camera->viewMatrix);
        program.setUniformValue("projectionMatrix", camera->projectionMatrix);

        // Models depth
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboDepth);

        // Background parameters
        program.setUniformValue("backgroundColor", miscSettings->backgroundColor);
        program.setUniformValue("drawGrid", miscSettings->grid);

        resourceManager->quad->submeshes[0]->draw();

        program.release();
    }


    gl->glDepthMask(GL_TRUE);
    gl->glDisable(GL_BLEND);
}

void DeferredRenderer::passLights(Camera *camera)
{
    QOpenGLShaderProgram &program = deferredLightingProgram->program;

    if (program.bind())
    {
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT1); //Clear only attachments 1 (light circles) as attachment 0 has been cleared in passAmbient()

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned int attachments_final[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        gl->glDrawBuffers(2, attachments_final);

        //Set uniforms
        program.setUniformValue("viewMatrix", camera->viewMatrix);

        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboPosition);
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, fboNormal);
        gl->glActiveTexture(GL_TEXTURE2);
        gl->glBindTexture(GL_TEXTURE_2D, fboColor);
        program.setUniformValue("gPosition", 0);
        program.setUniformValue("gNormal", 1);
        program.setUniformValue("gColor", 2);

        program.setUniformValue("cameraPos", camera->position);
        program.setUniformValue("viewportSize", QVector2D(camera->viewportWidth, camera->viewportHeight));

        gl->glEnable(GL_BLEND);
        gl->glBlendFunc(GL_ONE, GL_ONE);

        //Render spheres on lights
        for (auto entity : scene->entities)
        {
            if (entity->active && entity->lightSource != nullptr)
            {
                auto light = entity->lightSource;
                auto transform = *entity->transform;

                if (light->type == LightSource::Type::Point)
                    transform.scale = QVector3D(light->radius,light->radius, light->radius);
                else
                {
                    transform.position = QVector3D(0.0f,0.0f,0.0f);
                    transform.scale = QVector3D(1.0f,1.0f,1.0f);
                }
                QMatrix4x4 worldMatrix = transform.matrix();
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix;
                QMatrix3x3 normalMatrix = worldViewMatrix.normalMatrix();
                QMatrix4x4 projectionMatrix = camera->projectionMatrix;

                if(light->type ==  LightSource::Type::Directional){
                    projectionMatrix = QMatrix4x4();
                    worldViewMatrix = QMatrix4x4();
                }

                program.setUniformValue("worldMatrix", worldMatrix);
                program.setUniformValue("worldViewMatrix", worldViewMatrix);
                program.setUniformValue("normalMatrix", normalMatrix);
                program.setUniformValue("projectionMatrix", projectionMatrix);

                program.setUniformValue("lightType", (int)light->type);
                program.setUniformValue("lightPosition", transform.position);
                program.setUniformValue("lightDirection", QVector3D(transform.matrix() * QVector4D(0.0, 1.0, 0.0, 0.0)));
                QVector3D color = {light->color.red()/255.0f, light->color.green()/255.0f, light->color.blue()/255.0f};
                program.setUniformValue("lightColor", color);
                program.setUniformValue("lightIntensity", light->intensity);
                program.setUniformValue("lightRange", light->radius);

                if (light->type == LightSource::Type::Point)
                {
                    gl->glEnable(GL_CULL_FACE);
                    gl->glCullFace(GL_FRONT);
                    resourceManager->sphere->submeshes[0]->draw();
                }
                else
                {
                    gl->glDisable(GL_CULL_FACE);
                    resourceManager->quad->submeshes[0]->draw();
                }
            }
        }

        gl->glDisable(GL_BLEND);
        gl->glDisable(GL_CULL_FACE);

        program.release();
    }
}

void DeferredRenderer::passAmbient()
{
    QOpenGLShaderProgram &program = ambientLightingProgram->program;
    if(program.bind()){
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.setUniformValue("ambientValue", miscSettings->ambientValue);
        program.setUniformValue("applyOcclusion", miscSettings->ambientOcclusion);

        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboColor);
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, fboAO);
        program.setUniformValue("gColor", 0);
        program.setUniformValue("ssao", 1);

        resourceManager->quad->submeshes[0]->draw();

        program.release();
    }
}

void DeferredRenderer::passSSAO(Camera *camera)
{
    QOpenGLShaderProgram &program = ssaoProgram->program;
    if(program.bind()){
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Clear color
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT);

        program.setUniformValue("projectionMatrix", camera->projectionMatrix);
        program.setUniformValue("viewMatrix", camera->viewMatrix);
        program.setUniformValueArray("samples", &ssaoKernel[0], 64);

        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboPosition);
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, fboNormal);
        gl->glActiveTexture(GL_TEXTURE2);
        gl->glBindTexture(GL_TEXTURE_2D, fboDepth);
        gl->glActiveTexture(GL_TEXTURE3);
        gl->glBindTexture(GL_TEXTURE_2D, fboNoise);
        program.setUniformValue("gPosition", 0);
        program.setUniformValue("gNormal", 1);
        program.setUniformValue("gDepth", 2);
        program.setUniformValue("noiseMap", 3);

        resourceManager->quad->submeshes[0]->draw();

        program.release();
    }
}

void DeferredRenderer::passIdentifiers(Camera *camera)
{
    QOpenGLShaderProgram &program = mousePickProgram->program;

    if (program.bind())
    {
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);
        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Set uniforms
        program.setUniformValue("projectionMatrix", camera->projectionMatrix);

        QVector<MeshRenderer*> meshRenderers;
        QVector<LightSource*> lightSources;

        // Get components
        for (auto entity : scene->entities)
        {
            if (entity->active)
            {
                if (entity->meshRenderer != nullptr) { meshRenderers.push_back(entity->meshRenderer); }
                if (entity->lightSource != nullptr) { lightSources.push_back(entity->lightSource); }
            }
        }

        // Meshes
        for (auto meshRenderer : meshRenderers)
        {
            auto mesh = meshRenderer->mesh;

            if (mesh != nullptr)
            {
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * meshRenderer->entity->transform->matrix();

                program.setUniformValue("worldViewMatrix", worldViewMatrix);
                program.setUniformValue("colorId", meshRenderer->entity->getIDColor());

                for (auto submesh : mesh->submeshes)
                {
                    submesh->draw();
                }
            }
        }

        // Light spheres
        if (miscSettings->renderLightSources)
        {
            for (auto lightSource : lightSources)
            {
                QMatrix4x4 worldMatrix = lightSource->entity->transform->matrix();
                QMatrix4x4 scaleMatrix; scaleMatrix.scale(0.1f, 0.1f, 0.1f);
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix * scaleMatrix;

                program.setUniformValue("worldViewMatrix", worldViewMatrix);
                program.setUniformValue("colorId", lightSource->entity->getIDColor());

                for (auto submesh : resourceManager->sphere->submeshes)
                {
                    submesh->draw();
                }
            }
        }

        program.release();
    }
}

void DeferredRenderer::passMask(Camera *camera)
{
    QOpenGLShaderProgram &program = maskProgram->program;

    if (program.bind())
    {
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT2);

        // Clear color
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT);

        //Set uniforms
        program.setUniformValue("projectionMatrix", camera->projectionMatrix);

        QVector<MeshRenderer*> meshRenderers;
        QVector<LightSource*> lightSources;

        // Get components
        for (int i = 0; i < selection->count; ++i)
        {
            Entity* entity = selection->entities[i];
            if (entity->active)
            {
                if (entity->meshRenderer != nullptr) { meshRenderers.push_back(entity->meshRenderer); }
                if (entity->lightSource != nullptr) { lightSources.push_back(entity->lightSource); }
            }
        }

        // Meshes
        for (auto meshRenderer : meshRenderers)
        {
            auto mesh = meshRenderer->mesh;

            if (mesh != nullptr)
            {
                QMatrix4x4 worldMatrix = meshRenderer->entity->transform->matrix();
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix;

                program.setUniformValue("worldMatrix", worldMatrix);
                program.setUniformValue("worldViewMatrix", worldViewMatrix);

                for (auto submesh : mesh->submeshes)
                {
                    submesh->draw();
                }
            }
        }

        // Light spheres
        if (miscSettings->renderLightSources)
        {
            for (auto lightSource : lightSources)
            {
                QMatrix4x4 worldMatrix = lightSource->entity->transform->matrix();
                QMatrix4x4 scaleMatrix; scaleMatrix.scale(0.1f, 0.1f, 0.1f);
                QMatrix4x4 worldViewMatrix = camera->viewMatrix * worldMatrix * scaleMatrix;
                program.setUniformValue("worldMatrix", worldMatrix);
                program.setUniformValue("worldViewMatrix", worldViewMatrix);

                for (auto submesh : resourceManager->sphere->submeshes)
                {
                    submesh->draw();
                }
            }
        }

        program.release();
    }
}

void DeferredRenderer::passOutline()
{
    QOpenGLShaderProgram &program = outlineProgram->program;
    if(program.bind()){
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT3);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,0.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.setUniformValue("outlineColor", miscSettings->outlineColor);
        program.setUniformValue("outlineThickness", miscSettings->outlineThickness);

        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboMask);
        program.setUniformValue("mask", 0);

        resourceManager->quad->submeshes[0]->draw();

        program.release();
    }
}

void DeferredRenderer::passDOF(){

    QOpenGLShaderProgram &program = DOFProgram->program;

    if(program.bind()){

        // General uniforms
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboDepth);
        program.setUniformValue("depth", 0);
        program.setUniformValue("color", 1);
        program.setUniformValue("depthFocus", camera->depthFocus);
        program.setUniformValue("viewportSize", QVector2D(camera->viewportWidth, camera->viewportHeight));

        // Vertical pass
        // Draw on fboDOFV
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Color texture
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, fboLighting);
        // Vertical increment
        program.setUniformValue("texCoordInc", QVector2D(0.0, 1.0));

        resourceManager->quad->submeshes[0]->draw();

        // Horizontal pass
        // Draw on fboDOF
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT1);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // fboDOFV texture
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, fboDOFV);
        // Horizontal increment
        program.setUniformValue("texCoordInc", QVector2D(1.0, 0.0));


        resourceManager->quad->submeshes[0]->draw();

        program.release();
    }
}

void DeferredRenderer::finalMix(){
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE);

    QOpenGLShaderProgram &program = blitProgram->program;
    if(program.bind()){
        // Set FBO buffers
        gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.setUniformValue("blitSimple", true);
        gl->glActiveTexture(GL_TEXTURE0);

        // Background
        gl->glBindTexture(GL_TEXTURE_2D, fboGrid);
        resourceManager->quad->submeshes[0]->draw();

        // Lighting
        gl->glBindTexture(GL_TEXTURE_2D, fboLighting);
        resourceManager->quad->submeshes[0]->draw();

        gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Disable additive blend to avoid blending with background colors
        // Outline
        gl->glBindTexture(GL_TEXTURE_2D, fboOutline);
        resourceManager->quad->submeshes[0]->draw();

        gl->glDisable(GL_BLEND);
        program.release();
    }
}


void DeferredRenderer::passBlit()
{
    gl->glDisable(GL_DEPTH_TEST);

    QOpenGLShaderProgram &program = blitProgram->program;

    if (program.bind())
    {
        program.setUniformValue("blitSimple", false);
        program.setUniformValue("blitDepth", false);
        program.setUniformValue("blitAlpha", false);

        program.setUniformValue("colorTexture", 0);
        gl->glActiveTexture(GL_TEXTURE0);

        if (shownTexture() == "Final render") {
            gl->glBindTexture(GL_TEXTURE_2D, fboFinalTexture);
        } else if(shownTexture() == "Lighting"){
            gl->glBindTexture(GL_TEXTURE_2D, fboLighting);
        } else if (shownTexture() == "Position") {
            gl->glBindTexture(GL_TEXTURE_2D, fboPosition);
        } else if(shownTexture() == "Normals") {
            gl->glBindTexture(GL_TEXTURE_2D, fboNormal);
        } else if(shownTexture() == "Color"){
            gl->glBindTexture(GL_TEXTURE_2D, fboColor);
        } else if(shownTexture() == "Ambient Occlusion"){
            gl->glBindTexture(GL_TEXTURE_2D, fboAO);
        } else if(shownTexture() == "Grid"){
            gl->glBindTexture(GL_TEXTURE_2D, fboGrid);
        } else if(shownTexture() == "Light Circles") {
            gl->glBindTexture(GL_TEXTURE_2D, fboLightCircles);
        } else if(shownTexture() == "Object Identifiers") {
            gl->glBindTexture(GL_TEXTURE_2D, fboIdentifiers);
        } else if(shownTexture() == "DOF"){
            gl->glBindTexture(GL_TEXTURE_2D, fboDOF);
        } else if(shownTexture() == "Depth"){
            program.setUniformValue("blitDepth", true);
            gl->glBindTexture(GL_TEXTURE_2D, fboDepth);
        }

        resourceManager->quad->submeshes[0]->draw();
    }

    gl->glEnable(GL_DEPTH_TEST);
}
