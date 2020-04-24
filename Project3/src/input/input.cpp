#include "input.h"
#include <QKeyEvent>
#include <QMouseEvent>


Input::Input()
{
    for (int i = 0; i < MAX_BUTTONS; ++i) {
        mouseButtons[i] = MouseButtonState::Idle;
    }
    for (int i = 0; i < MAX_KEYS; ++i) {
        keys[i] = KeyState::Idle;
    }
}

void Input::keyPressEvent(QKeyEvent *event)
{
    if (event->key() < MAX_KEYS && !event->isAutoRepeat()) {
        if (keys[event->key()] == KeyState::Idle) {
            keys[event->key()] = KeyState::Press;
        }
    }
}

void Input::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() < MAX_KEYS && !event->isAutoRepeat()) {
        keys[event->key()] = KeyState::Idle;
    }
}

void Input::mousePressEvent(QMouseEvent *event)
{
    if (event->button() < MAX_BUTTONS) {
        if (mouseButtons[event->button()] == MouseButtonState::Idle) {
            mousex = mousex_prev = event->x();
            mousey = mousey_prev = event->y();
            mouseButtons[event->button()] = MouseButtonState::Press;
        }
    }
}

void Input::mouseMoveEvent(QMouseEvent *event)
{
    mousex = event->x();
    mousey = event->y();
}

void Input::mouseReleaseEvent(QMouseEvent *event)
{
    mouseButtons[event->button()] = MouseButtonState::Idle;
}

void Input::focusOutEvent(QFocusEvent *event)
{
    for (int i = 0; i < MAX_KEYS; ++i) {
        keys[i] = KeyState::Idle;
    }
    for (int i = 0; i < MAX_BUTTONS; ++i) {
        mouseButtons[i] = MouseButtonState::Idle;
    }
}

void Input::postUpdate()
{
    for (int i = 0; i < MAX_BUTTONS; ++i) {
        if (mouseButtons[i] == MouseButtonState::Press) {
            mouseButtons[i] = MouseButtonState::Pressed;
        }
    }
    for (int i = 0; i < MAX_KEYS; ++i) {
        if (keys[i] == KeyState::Press) {
            keys[i] = KeyState::Pressed;
        }
    }

    mousex_prev = mousex;
    mousey_prev = mousey;
}
