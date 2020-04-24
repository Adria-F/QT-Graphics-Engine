#ifndef INPUT_H
#define INPUT_H

class QKeyEvent;
class QMouseEvent;
class QFocusEvent;

enum class KeyState { Idle, Press, Pressed };

enum class MouseButtonState { Idle, Press, Pressed };

class Input
{
public:
    Input();

    // Event procesing functions
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void focusOutEvent(QFocusEvent *event);

    void postUpdate();

    enum {
        MAX_BUTTONS = 10,
        MAX_KEYS = 300
    };

    // Keyboard state
    KeyState keys[MAX_KEYS];

    // Mouse state
    MouseButtonState mouseButtons[MAX_BUTTONS];
    int mousex = 0;
    int mousey = 0;
    int mousex_prev = 0;
    int mousey_prev = 0;
};

#endif // INPUT_H
