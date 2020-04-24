#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <QString>
#include "gl.h"


class FramebufferObject
{
public:

    FramebufferObject();

    void create();
    void destroy();

    void addColorAttachment(GLuint attachment, GLuint textureId, GLint level = 0);
    void addDepthAttachment(GLuint textureId, GLint level = 0);

    void checkStatus();

    void bind();
    void release();

    GLuint id = 0;

    QString name;
};

#endif // FRAMEBUFFEROBJECT_H
