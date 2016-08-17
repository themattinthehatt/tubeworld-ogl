//
// Created by mattw on 8/15/16.
//

#ifndef TUBEWORLD_KEYHANDLER_H
#define TUBEWORLD_KEYHANDLER_H

// array for pressed keys
bool keysPressed[GLFW_KEY_LAST] = {};

// array for toggled keys
bool keysToggled[GLFW_KEY_LAST] = {};

// Is called whenever a key is pressed/released via GLFW
void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) {
        keysPressed[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keysPressed[key] = false;
        keysToggled[key] = !keysToggled[key];
    }
}

#endif //TUBEWORLD_KEYHANDLER_H
