#ifndef CAMERA_H
#define CAMERA_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

static const float DEFAULT_CAMERA_SPEED = 0.5f;
static const float DEFAULT_CAMERA_FOVY = 60.0f;
static const float DEFAULT_CAMERA_ZNEAR = 0.01f;
static const float DEFAULT_CAMERA_ZFAR = 10000.0f;

class Camera
{
public:

    Camera();

    QVector4D getLeftRightBottomTop();

    QVector3D screenPointToWorldRay(int x, int y);
    QVector3D screenDisplacementToWorldVector(int x0, int y0, int x1, int y1, const QVector3D &worldPoint);
    QVector2D worldToScreenPoint(const QVector3D &p);

    // Create the matrices
    void prepareMatrices();


    // Viewport
    int viewportWidth = 128;
    int viewportHeight = 128;

    // Projection
    float fovy = DEFAULT_CAMERA_FOVY;
    float znear = DEFAULT_CAMERA_ZNEAR;
    float zfar = DEFAULT_CAMERA_ZFAR;

    // World navigation
    float yaw = 0.0f;
    float pitch = 0.0f;
    QVector3D position;

    // Speed
    float maxSpeed = DEFAULT_CAMERA_SPEED;

    // Depth of field
    float depthFocus = -1;
    float depthFallofStartMargin = 3;
    float depthFallofEndMargin = 10;

    // Derived matrices
    QMatrix4x4 worldMatrix; // From camera space to world space
    QMatrix4x4 viewMatrix; // From world space to camera space
    QMatrix4x4 projectionMatrix; // From view space to clip space
};

#endif // CAMERA_H
