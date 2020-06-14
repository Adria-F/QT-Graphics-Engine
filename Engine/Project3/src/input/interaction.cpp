#include "interaction.h"
#include "globals.h"
#include "resources/mesh.h"
#include <QtMath>
#include <QVector2D>


bool Interaction::update()
{
    bool changed = false;

    switch (state)
    {
    case State::Idle:
        changed = idle();
        break;

    case State::Navigating:
        changed = navigate();
        break;

    case State::Focusing:
        changed = focus();
        break;
    }

    return changed;
}

bool Interaction::idle()
{
    if (input->mouseButtons[Qt::RightButton] == MouseButtonState::Pressed)
    {
        nextState = State::Navigating;
    }
    else if (input->mouseButtons[Qt::LeftButton] == MouseButtonState::Press)
    {
        // TODO: Left click
        emit selection->leftClick();
        renderIdentifiers = true;
    }
    else if(selection->count > 0)
    {
        if (input->keys[Qt::Key_F] == KeyState::Press)
        {
            nextState = State::Focusing;
        }
    }

    return false;
}

bool Interaction::navigate()
{
    static float v = 0.0f; // Instant speed
    static const float a = 5.0f; // Constant acceleration
    static const float t = 1.0/60.0f; // Delta time

    bool pollEvents = input->mouseButtons[Qt::RightButton] == MouseButtonState::Pressed;

    // Mouse delta smoothing
    static float mousex_delta_prev[3] = {};
    static float mousey_delta_prev[3] = {};
    static int curr_mousex_delta_prev = 0;
    static int curr_mousey_delta_prev = 0;
    float mousey_delta = 0.0f;
    float mousex_delta = 0.0f;
    if (pollEvents) {
        mousex_delta_prev[curr_mousex_delta_prev] = (input->mousex - input->mousex_prev);
        mousey_delta_prev[curr_mousey_delta_prev] = (input->mousey - input->mousey_prev);
        curr_mousex_delta_prev = curr_mousex_delta_prev % 3;
        curr_mousey_delta_prev = curr_mousey_delta_prev % 3;
        mousex_delta += mousex_delta_prev[0] * 0.33;
        mousex_delta += mousex_delta_prev[1] * 0.33;
        mousex_delta += mousex_delta_prev[2] * 0.33;
        mousey_delta += mousey_delta_prev[0] * 0.33;
        mousey_delta += mousey_delta_prev[1] * 0.33;
        mousey_delta += mousey_delta_prev[2] * 0.33;
    }

    float &yaw = camera->yaw;
    float &pitch = camera->pitch;

    // Camera navigation
    if (mousex_delta != 0 || mousey_delta != 0)
    {
        // Regular rotation
        if(input->keys[Qt::Key_O] != KeyState::Pressed){
            yaw -= 0.5f * mousex_delta;
            pitch -= 0.5f * mousey_delta;
            while (yaw < 0.0f) yaw += 360.0f;
            while (yaw > 360.0f) yaw -= 360.0f;
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        // Orbital rotation
        else{
            QVector3D target(0,0,0);
            if(selection->count != 0)
                target = selection->entities[0]->transform->position;

            // In radiants (entretainment)
            float rotationAngleX = 0.01 * mousex_delta;
            float rotationAngleY = 0.01 * mousey_delta;
            QVector2D cameraPos2D = QVector2D(camera->position.x(), camera->position.z());
            QVector2D target2D = QVector2D(target.x(), target.z());


            QVector2D diffVector = (cameraPos2D - target2D);
            QVector2D zeroVector = diffVector.normalized();
            QVector2D newVector = QVector2D(zeroVector.x() * cos(rotationAngleX) - zeroVector.y() * sin(rotationAngleX),
                                            zeroVector.x() * sin(rotationAngleX) + zeroVector.y() * cos(rotationAngleX));

            QVector2D newCameraPos2D = target2D + newVector.normalized() * diffVector.length();

            camera->position.setX(newCameraPos2D.x());
            camera->position.setZ(newCameraPos2D.y());


            QVector2D diffVectorNorm = (cameraPos2D - target2D).normalized();

            yaw = atan2(diffVectorNorm.x(), diffVectorNorm.y()) * 180.0/3.14159265359;


            pitch -= rotationAngleY * 180.0/3.14159265359;
            camera->position.setY(camera->position.y() + mousey_delta * 0.1);

           /* yaw -= rotationAngle * 180/3.1416;
            */

            /*while (yaw < 0.0f) yaw += 360.0f;
            while (yaw > 360.0f) yaw -= 360.0f;*/

            //QVector3D




        }
    }


    static QVector3D speedVector;
    speedVector *= 0.99;

    bool accelerating = false;
    if (input->keys[Qt::Key_W] == KeyState::Pressed) // Front
    {
        accelerating = true;
        speedVector += QVector3D(-sinf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch)),
                                        sinf(qDegreesToRadians(pitch)),
                                        -cosf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch))) * a * t;
    }
    if (input->keys[Qt::Key_A] == KeyState::Pressed) // Left
    {
        accelerating = true;
        speedVector -= QVector3D(cosf(qDegreesToRadians(yaw)),
                                        0.0f,
                                        -sinf(qDegreesToRadians(yaw))) * a * t;
    }
    if (input->keys[Qt::Key_S] == KeyState::Pressed) // Back
    {
        accelerating = true;
        speedVector -= QVector3D(-sinf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch)),
                                        sinf(qDegreesToRadians(pitch)),
                                        -cosf(qDegreesToRadians(yaw)) * cosf(qDegreesToRadians(pitch))) * a * t;
    }
    if (input->keys[Qt::Key_D] == KeyState::Pressed) // Right
    {
        accelerating = true;
        speedVector += QVector3D(cosf(qDegreesToRadians(yaw)),
                                        0.0f,
                                        -sinf(qDegreesToRadians(yaw))) * a * t;
    }



    if (!accelerating) {
        speedVector *= 0.9;
    }

    // Cap maximum speed
    if(speedVector.length() > camera->maxSpeed){
        speedVector = speedVector.normalized() * camera->maxSpeed;
    }

    camera->position += speedVector * t;

    if (!(pollEvents ||
        speedVector.length() > 0.01f||
        qAbs(mousex_delta) > 0.1f ||
        qAbs(mousey_delta) > 0.1f))
    {
        nextState = State::Idle;
    }

    return true;
}

bool Interaction::focus()
{
    static bool idle = true;
    static float time = 0.0;
    static QVector3D initialCameraPosition;
    static QVector3D finalCameraPosition;
    if (idle) {
        idle = false;
        time = 0.0f;
        initialCameraPosition = camera->position;

        Entity *entity = selection->entities[0];

        float entityRadius = 0.5;
        if (entity->meshRenderer != nullptr && entity->meshRenderer->mesh != nullptr)
        {
            auto mesh = entity->meshRenderer->mesh;
            const QVector3D minBounds = entity->transform->matrix() * mesh->bounds.min;
            const QVector3D maxBounds = entity->transform->matrix() * mesh->bounds.max;
            entityRadius = (maxBounds - minBounds).length();
        }

        QVector3D entityPosition = entity->transform->position;
        QVector3D viewingDirection = QVector3D(camera->worldMatrix * QVector4D(0.0, 0.0, -1.0, 0.0));
        QVector3D displacement = - 1.5 * entityRadius * viewingDirection.normalized();
        finalCameraPosition = entityPosition + displacement;
    }

    const float focusDuration = 0.5f;
    time = qMin(focusDuration, time + 1.0f/60.0f); // TODO: Use frame delta time
    const float t = qPow(qSin(3.14159f * 0.5f * time / focusDuration), 0.5);

    camera->position = (1.0f - t) * initialCameraPosition + t * finalCameraPosition;

    if (t == 1.0f) {
        nextState = State::Idle;
        idle = true;;
    }

    return true;
}

void Interaction::postUpdate()
{
    state = nextState;
}
