#include "gl.h"
#include <QDebug>
#include <stdarg.h>

#define GL_DEBUG

#define Isolated    0
#define Constructor 1
#define Destructor  2


// Retrieve an error string ////////////////////////////////////////////
#ifdef GL_DEBUG
static const char *getErrorName(GLenum error)
{
    switch (error) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
//		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
//		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        default: return "Unknown GL error";
    }
}
/*
static const char *getErrorString(GLenum error)
{
    switch (error) {
        case GL_NO_ERROR: return "GL_NO_ERROR: No error was produced";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. ";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
        default: return "Unknown GL error";
    }
}
*/
#endif
////////////////////////////////////////////////////////////////////////

static void check(const char *text, int time)
{
#ifdef GL_DEBUG
    static const char *timeString[] = {"ISOLATED", "BEGIN", "END"};
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        const char *errString = getErrorName(error);
        qDebug() << " - OpenGL ERROR - " << timeString[time] << " - " << text << " - " << errString << " - " << error;
    }
#endif // GL_DEBUG
}


OpenGLErrorGuard::OpenGLErrorGuard(const char *text, ...)
{
#ifdef GL_DEBUG
#define MAX_FORMAT_LEN 2048
    char buffer[MAX_FORMAT_LEN];
    va_list ap;
    va_start(ap, text);
    vsnprintf(buffer, MAX_FORMAT_LEN, text, ap);
    va_end(ap);
    message = buffer;
    check(message.toLatin1(), Constructor);
#endif // GL_DEBUG
}

OpenGLErrorGuard::~OpenGLErrorGuard()
{
#ifdef GL_DEBUG
    check(message.toLatin1(), Destructor);
#endif // GL_DEBUG
}

static void checkGLError(const char *around, const char *message);
void OpenGLErrorGuard::checkGLError(const char *text, ...)
{
#ifdef GL_DEBUG
#define MAX_FORMAT_LEN 2048
        char buffer[MAX_FORMAT_LEN];
        va_list ap;
        va_start(ap, text);
        vsnprintf(buffer, MAX_FORMAT_LEN, text, ap);
        va_end(ap);
        check(buffer, Isolated);
#endif // GL_DEBUG
}


// OpenGLState ////////////////////////////////////////////////////////

OpenGLState OpenGLState::currentState;

void OpenGLState::apply()
{
    if (depthTest != currentState.depthTest)
    {
        if (depthTest) {
            gl->glEnable(GL_DEPTH_TEST);
        } else {
            gl->glDisable(GL_DEPTH_TEST);
        }
    }

    if (depthWrite != currentState.depthWrite)
    {
        if (depthWrite) {
            gl->glDepthMask(GL_TRUE);
        } else {
            gl->glDepthMask(GL_FALSE);
        }
    }

    if (depthFunc != currentState.depthFunc)
    {
        gl->glDepthFunc(depthFunc);
    }

    if (blending != currentState.blending)
    {
        if (blending) {
            gl->glEnable(GL_BLEND);
        } else {
            gl->glDisable(GL_BLEND);
        }
    }

    if (blendFuncSrc != currentState.blendFuncSrc || blendFuncDst != currentState.blendFuncDst)
    {
        gl->glBlendFunc(blendFuncSrc, blendFuncDst);
    }

    if (faceCulling != currentState.faceCulling)
    {
        if (faceCulling) {
            gl->glEnable(GL_CULL_FACE);
        } else {
            gl->glDisable(GL_CULL_FACE);
        }
    }

    if (faceCullingMode != currentState.faceCullingMode)
    {
        gl->glCullFace(faceCullingMode);
    }

    for (int i = 0; i < NUM_CLIP_PLANES; ++i)
    {
        if (clipDistance[i] != currentState.clipDistance[i]) {
            if (clipDistance[i]) {
                gl->glEnable(GL_CLIP_DISTANCE0 + i);
            } else {
                gl->glDisable(GL_CLIP_DISTANCE0 + i);
            }
        }
    }

    currentState = *this;
}

void OpenGLState::initialize()
{
    gl->glDisable(GL_DEPTH_TEST);
    gl->glDepthMask(GL_TRUE);
    gl->glDepthFunc(GL_LESS);
    gl->glDisable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
    gl->glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    for (int i = 0; i < NUM_CLIP_PLANES; ++i) {
        gl->glDisable(GL_CLIP_DISTANCE0 + i);
    }
}

void OpenGLState::reset()
{
    OpenGLState gl;
    gl.apply();
}
