#ifndef MISCSETTINGS_H
#define MISCSETTINGS_H

#include <QColor>

class MiscSettings
{
public:
    MiscSettings();

    // TODO: Maybe not the best place for this stuff...
    QColor backgroundColor;
    bool renderLightSources = true;
    QColor outlineColor = QColor(0,255,0);
    float outlineThickness = 2.0f;
    bool ambientOcclusion = true;
    float ambientValue = 0.2f;
    bool grid = true;
};

#endif // MISCSETTINGS_H
