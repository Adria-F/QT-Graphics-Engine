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
    fboDepth(QOpenGLTexture::Target2D),
    fboGrid(QOpenGLTexture::Target2D),
    fboDOFV(QOpenGLTexture::Target2D),
    fboDOF(QOpenGLTexture::Target2D),
    fboFinalTexture(QOpenGLTexture::Target2D)

{
    fboInfo = nullptr;
    fboLight = nullptr;
    fboFinal = nullptr;
    fboPostProcess = nullptr;

    // List of textures
    addTexture("Final render");
    addTexture("Lighting");
    addTexture("Position");
    addTexture("Normals");
    addTexture("Color");
    addTexture("Grid");
    addTexture("DOF");
    addTexture("Light Circles");
    addTexture("Depth");
}

DeferredRenderer::~DeferredRenderer()
{
    delete fboInfo;
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

    ///Grid
    gridProgram = resourceManager->createShaderProgram();
    gridProgram->name = "Grid";
    gridProgram->vertexShaderFilename = "res/shaders/grid/grid.vert";
    gridProgram->fragmentShaderFilename = "res/shaders/grid/grid.frag";
    gridProgram->includeForSerialization = false;


    ///Deferred Lighting
    deferredLightingProgram = resourceManager->createShaderProgram();
    deferredLightingProgram->name = "Forward shading";
    deferredLightingProgram->vertexShaderFilename = "res/shaders/forward_shader/standard_shading.vert";
    deferredLightingProgram->fragmentShaderFilename = "res/shaders/lighting/deferred_lighting.frag";
    deferredLightingProgram->includeForSerialization = false;

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

    fboLight = new FramebufferObject;
    fboLight->create();

    fboPostProcess = new FramebufferObject;
    fboPostProcess->create();

    fboFinal = new FramebufferObject;
    fboFinal->create();


}

void DeferredRenderer::finalize()
{
    fboInfo->destroy();
    delete fboInfo;

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

    fboLight->bind();
    fboLight->addColorAttachment(0, fboLighting);
    fboLight->addColorAttachment(1, fboLightCircles);
    fboLight->release();

    fboPostProcess->bind();
    fboPostProcess->addColorAttachment(0, fboDOFV);
    fboPostProcess->addColorAttachment(1, fboDOF);
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
    fboLight->bind();
    passLights(camera);
    fboLight->release();
    fboPostProcess->bind();
    passDOF();
    fboPostProcess->release();

    fboFinal->bind();
    finalMix();
    fboFinal->release();


    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    passBlit();
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
        // Clear color
        gl->glClearDepth(1.0);
        gl->glClearColor(0.0f,0.0f,0.0f,1.0);
        gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set FBO buffers
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

void DeferredRenderer::passDOF(){

    QOpenGLShaderProgram &program = DOFProgram->program;

    if(program.bind()){

        // General uniforms
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, fboDepth);
        program.setUniformValue("depth", 0);
        program.setUniformValue("color", 1);
        program.setUniformValue("depthFocus", camera->depthFocus);

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
        } else if(shownTexture() == "Grid"){
            gl->glBindTexture(GL_TEXTURE_2D, fboGrid);
        } else if(shownTexture() == "Light Circles") {
            gl->glBindTexture(GL_TEXTURE_2D, fboLightCircles);
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
