#include "framebufferobject.h"
#include "rendering/gl.h"
#include <QOpenGLFramebufferObject>
#include <QDebug>


FramebufferObject::FramebufferObject()
{

}

void FramebufferObject::create()
{
    gl->glGenFramebuffers(1, &id);
}

void FramebufferObject::destroy()
{
    gl->glDeleteFramebuffers(1, &id);
}

void FramebufferObject::addColorAttachment(GLuint attachment, GLuint textureId, GLint level)
{
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, textureId, level);
}

void FramebufferObject::addDepthAttachment(GLuint textureId, GLint level)
{
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, level);
}

void FramebufferObject::checkStatus()
{
    GLenum status;
    status = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE: // Everything's OK
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        default:
            qDebug() << "FBO::checkStatus() ERROR: " << name << " "
                << "Unknown ERROR";
    }
}

void FramebufferObject::bind()
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FramebufferObject::release()
{
    QOpenGLFramebufferObject::bindDefault();
}
