QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project3
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
CONFIG += console

SOURCES += \
    src/main.cpp \
    src/globals.cpp \
    src/ecs/camera.cpp \
    src/ecs/scene.cpp  \
    src/ecs/entity.cpp \
    src/ecs/components.cpp \
    src/input/input.cpp \
    src/input/interaction.cpp \
    src/input/selection.cpp \
    src/rendering/deferredrenderer.cpp \
    src/rendering/gl.cpp \
    src/rendering/forwardrenderer.cpp \
    src/rendering/framebufferobject.cpp \
    src/rendering/miscsettings.cpp \
    src/rendering/renderer.cpp \
    src/resources/mesh.cpp \
    src/resources/resource.cpp \
    src/resources/resourcemanager.cpp \
    src/resources/material.cpp \
    src/resources/texture.cpp \
    src/resources/shaderprogram.cpp \
    src/ui/resourceswidget.cpp \
    src/ui/mainwindow.cpp \
    src/ui/inspectorwidget.cpp \
    src/ui/hierarchywidget.cpp \
    src/ui/transformwidget.cpp \
    src/ui/componentwidget.cpp \
    src/ui/entitywidget.cpp \
    src/ui/meshrendererwidget.cpp \
    src/ui/openglwidget.cpp \
    src/ui/aboutopengldialog.cpp \
    src/ui/meshwidget.cpp \
    src/ui/resourcewidget.cpp \
    src/ui/openglwidget_texture.cpp \
    src/ui/texturewidget.cpp \
    src/ui/materialwidget.cpp \
    src/ui/lightsourcewidget.cpp \
    src/ui/miscsettingswidget.cpp \
    src/util/modelimporter.cpp

HEADERS += \
    src/globals.h \
    src/ecs/camera.h \
    src/ecs/scene.h \
    src/ecs/entity.h \
    src/ecs/components.h \
    src/input/input.h \
    src/input/interaction.h \
    src/input/selection.h \
    src/rendering/deferredrenderer.h \
    src/rendering/gl.h \
    src/rendering/miscsettings.h \
    src/rendering/renderer.h \
    src/rendering/forwardrenderer.h \
    src/rendering/framebufferobject.h \
    src/resources/mesh.h \
    src/resources/resource.h \
    src/resources/resourcemanager.h \
    src/resources/material.h \
    src/resources/texture.h \
    src/resources/shaderprogram.h \
    src/ui/mainwindow.h \
    src/ui/inspectorwidget.h \
    src/ui/hierarchywidget.h \
    src/ui/transformwidget.h \
    src/ui/componentwidget.h \
    src/ui/entitywidget.h \
    src/ui/meshrendererwidget.h \
    src/ui/openglwidget.h \
    src/ui/aboutopengldialog.h \
    src/ui/resourceswidget.h \
    src/ui/meshwidget.h \
    src/ui/resourcewidget.h \
    src/ui/openglwidget_texture.h \
    src/ui/texturewidget.h \
    src/ui/materialwidget.h \
    src/ui/lightsourcewidget.h \
    src/ui/miscsettingswidget.h \
    src/util/modelimporter.h \
    src/util/stb_image.h

FORMS += \
    ui/mainwindow.ui \
    ui/hierarchywidget.ui \
    ui/transformwidget.ui \
    ui/componentwidget.ui \
    ui/entitywidget.ui \
    ui/aboutopengldialog.ui \
    ui/resourceswidget.ui \
    ui/meshwidget.ui \
    ui/resourcewidget.ui \
    ui/texturewidget.ui \
    ui/materialwidget.ui \
    ui/miscsettingswidget.ui

INCLUDEPATH += src/

RESOURCES += \
    res/resources.qrc

# OpenGL
win32: LIBS += -lopengl32

# Assimp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ThirdParty/Assimp/lib/windows/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ThirdParty/Assimp/lib/windows/ -lassimpd
else:unix: LIBS += -L$$PWD/../ThirdParty/Assimp/lib/osx/ -lassimp
INCLUDEPATH += $$PWD/../ThirdParty/Assimp/include
DEPENDPATH += $$PWD/../ThirdParty/Assimp/include

DISTFILES += \
    res/shaders/blit/blit.frag \
    res/shaders/blit/blit.vert \
    res/shaders/dof/dof.frag \
    res/shaders/dof/dof.vert \
    res/shaders/final_mix/final_mix.frag \
    res/shaders/forward_shader/forward_shading.frag \
    res/shaders/forward_shader/standard_shading.vert \
    res/shaders/grid/grid.frag \
    res/shaders/grid/grid.vert \
    res/shaders/lighting/deferred_geometry.frag \
    res/shaders/lighting/deferred_lighting.frag \
    res/shaders/mouse_picking/mousePicking.frag \
    res/shaders/mouse_picking/mousePicking.vert \
    res/shaders/outline/outline.frag \
    res/shaders/outline/outline.vert \
    res/shaders/ssao/ssao.frag \
    res/shaders/ssao/ssao.vert \
    res/shaders/texture_view/texture_view.frag \
    res/shaders/texture_view/texture_view.vert
