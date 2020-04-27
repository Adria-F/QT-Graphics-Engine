#include "resource.h"
#include "globals.h"
#include <QDir>


QString Resource::absolutePathInProject(QString filePath)
{
    QDir dir(projectDirectory);
    return dir.absoluteFilePath(filePath);
}

QString Resource::relativePathInProject(QString filePath)
{
    QDir dir(projectDirectory);
    return dir.relativeFilePath(filePath);
}
