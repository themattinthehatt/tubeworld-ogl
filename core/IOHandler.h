//
// Created by mattw on 8/26/16.
//

#ifndef TUBEWORLD_IOHANDLER_H
#define TUBEWORLD_IOHANDLER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class IOHandler {
private:

    // private constructor, only allow 1 instance
    IOHandler() {};

    // prevent copies
    IOHandler(IOHandler const&) {};

    // prevent assignments
    void operator=(IOHandler const&) {};



public:

    static IOHandler& getInstance() {
        static IOHandler instance;
        return instance;
    }

    // array for pressed keys
    bool keysPressed[GLFW_KEY_LAST] = {};

    // array for toggled keys
    bool keysToggled[GLFW_KEY_LAST] = {};

    // fucky shit
    bool first_release = true;

    static void baseKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        getInstance().keyCallback(window, key, scancode, action, mode);
    }

    // Is called whenever a key is pressed/released via GLFW
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (action == GLFW_PRESS) {
            keysPressed[key] = true;
            first_release = true;
        }
        else if (action == GLFW_RELEASE) {
            if (first_release) {
                keysPressed[key] = false;
                keysToggled[key] = !keysToggled[key];
                first_release = false;
            } else {
                keysPressed[key] = false;
            }
        }
    }
};


#endif //TUBEWORLD_IOHANDLER_H
