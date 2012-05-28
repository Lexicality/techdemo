#include "InputManager.h"
#include "gl.h"
#include <GL/glfw.h>

InputManager::InputManager() {
    GLint data[4];
    glGetIntegerv(GL_VIEWPORT, data);
    const GLuint width  = data[2];
    const GLuint height = data[3];
    centerx = static_cast<int>(width  / 2);
    centery = static_cast<int>(height / 2);
}

InputManager::~InputManager() {
    //
}

bool keypress(int key);
bool mpress  (int btn);

const InputData InputManager::GetInput() {
    InputData data = {
        false, // w
        false, // s
        false, // a
        false, // d
        /*
        false, // ctrl / mouse1
        false, // e    / mouse2
        false, // [    / mwheeldn
        false, // ]    / mwheelup
        false, // 1
        false, // 2
        false, // 3
        false, // 4
         */
        false, // esc
        0,     // mousey
        0      // mousex
    };
    glfwPollEvents();
    // Movement
    if (keypress('W') || keypress(GLFW_KEY_UP) || keypress(GLFW_KEY_KP_8))
        data.Forwards = true;
    if (keypress('S') || keypress(GLFW_KEY_DOWN) || keypress(GLFW_KEY_KP_5) || keypress(GLFW_KEY_KP_2))
        data.Backwards = true;
    if (keypress('A') || keypress(GLFW_KEY_LEFT) || keypress(GLFW_KEY_KP_4))
        data.Left = true;
    if (keypress('D') || keypress(GLFW_KEY_RIGHT) || keypress(GLFW_KEY_KP_6))
        data.Right = true;
    /*
    if (keypress(GLFW_KEY_LCTRL) || keypress(GLFW_KEY_RCTRL))
        data.Shoot = true;
    if (keypress('E') || keypress(GLFW_KEY_SPACE))
        data.Use = true;
    if (keypress('[') || keypress(GLFW_KEY_KP_7))
        data.InventoryPrev = true;
    if (keypress(']') || keypress(GLFW_KEY_KP_9))
        data.InventoryNext = true;
    if (keypress('1'))
        data.Slot1 = true;
    if (keypress('2'))
        data.Slot2 = true;
    if (keypress('3'))
        data.Slot3 = true;
    if (keypress('4'))
        data.Slot4 = true;
    */
    if (keypress(GLFW_KEY_ESC) || keypress('Q'))
        data.Exit = true;
    // Mouse events
    if (glfwGetWindowParam(GLFW_ACTIVE) == GL_TRUE) {
        if (lostFocus) {
            lostFocus = false;
            // Reset the mouse for the next call
        } else {
            /*
            if (mpress(GLFW_MOUSE_BUTTON_LEFT))
                data.Shoot = true;
            if (mpress(GLFW_MOUSE_BUTTON_RIGHT))
                data.Use = true;
            */
            int x = 0, y = 0;
            glfwGetMousePos(&x, &y);
            data.DeltaViewX = static_cast<float>(x - centerx);
            data.DeltaViewY = static_cast<float>(y - centery);
            /*
            int mwheel = glfwGetMouseWheel();
            if (mwheel != 0) {
                if (mwheel > 0)
                    data.InventoryNext = true;
                else
                    data.InventoryPrev = true;
            }
            */
        }
        glfwSetMousePos(centerx, centery);
        glfwSetMouseWheel(0);
    } else {
        // Ignore the mouse if the window's not in focus.
        lostFocus = true;
    }
    return data;
}

bool keypress(int key) {
    return glfwGetKey(key) == GLFW_PRESS;
}
bool mpress(int mbutton) {
    return glfwGetMouseButton(mbutton) == GLFW_PRESS;
}
