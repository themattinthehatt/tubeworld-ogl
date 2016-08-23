//
// Created by mattw on 8/15/16.
//

#ifndef TUBEWORLD_KEYHANDLER_H
#define TUBEWORLD_KEYHANDLER_H

// array for pressed keys
bool keysPressed[GLFW_KEY_LAST] = {};

// array for toggled keys
bool keysToggled[GLFW_KEY_LAST] = {};

// fucky shit
bool first_release = true;

// Is called whenever a key is pressed/released via GLFW
void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) {
        keysPressed[key] = true;
//        std::cout << key << std::endl;
        first_release = true;
//        std::cout << keysPressed[key] << std::endl;
    }
    else if (action == GLFW_RELEASE) {
        if (first_release) {
//            std::cout << keysToggled[key] << " ";
            keysPressed[key] = false;
            keysToggled[key] = !keysToggled[key];
            std::cout << keysToggled[key] << std::endl;
            first_release = false;
        } else {
            keysPressed[key] = false;
        }
    }
}

#endif //TUBEWORLD_KEYHANDLER_H
