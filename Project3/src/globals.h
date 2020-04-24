#ifndef GLOBALS_H
#define GLOBALS_H

#include "resources/resourcemanager.h"
#include "ecs/scene.h"
#include "ecs/camera.h"
#include "input/input.h"
#include "input/interaction.h"
#include "input/selection.h"
#include "rendering/miscsettings.h"
#include <QString>

extern ResourceManager *resourceManager;
extern Camera *camera;
extern Scene *scene;
extern Input *input;
extern Interaction *interaction;
extern Selection *selection;
extern MiscSettings *miscSettings;

extern QString projectDirectory;

#endif // GLOBALS_H
