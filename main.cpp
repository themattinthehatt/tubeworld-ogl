

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic
#include <GL/glew.h>

// Include GLFW - handles window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - 3D mathematics
#include <glm/glm.hpp>

#include "core/Debug.h"

// include tubeworld components
#include "core/Camera.h"
#include "core/Skybox.h"
#include "core/KeyHandler.h"
#include "cube-array-inf/CubeArrayInf.h"
#include "cube-array-ring/CubeArrayRing.h"
#include "cube-array-user/CubeArrayUser.h"
#include "tube-traveller/TubeTraveller.h"



int main() {

    // -------------------------------------------------------------------------
    // GLFW - Initialize window and an OpenGL context to render in
    // -------------------------------------------------------------------------
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // configure window
    // 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    // we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // to make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // we don't want old OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // open a window and create its OpenGL context
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    GLFWmonitor *useMonitor;
    int monitorID = 0;
    if (monitorID < count) {
        useMonitor = monitors[monitorID];
    } else useMonitor = monitors[0];
    GLFWwindow *window;

    const GLFWvidmode *mode = glfwGetVideoMode(useMonitor);
    int xResolution = mode->width;
    int yResolution = mode->height;

    GLuint screenWidth = xResolution; //1200;
    GLuint screenHeight = yResolution; //900;
    window = glfwCreateWindow(screenWidth, screenHeight, "tubeworld 2.0", nullptr, nullptr);
//    window = glfwCreateWindow(screenWidth, screenHeight, "tubeworld 2.0", useMonitor, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // set up callback function for when a physical key is pressed or released
    glfwSetKeyCallback(window, keyHandler);
    // ensure we can capture key presses
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // hide the mouse and enable unlimited movement
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // set the mouse at the center of the screen
    // glfwPollEvents();
    // glfwSetCursorPos(window, 1024/2, 768/2);

//    glfwSwapInterval(1);

    // -------------------------------------------------------------------------
    // GLEW - queries location of OpenGL functions at run-time and stores
    // -------------------------------------------------------------------------
    glewExperimental = GL_TRUE; // needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        return -1;
    }

    GL_CHECK(true);

    // tell OpenGL the size of the rendering window
//    int width, height;
//    glfwGetFramebufferSize(window, &width, &height);
    // OpenGL uses data specified here to transform the 2D coordinates it
    // processes to the coordinates on the screen
    glViewport(0, 0, screenWidth, screenHeight); // lower left corner, width, height


    // OpenGL options
    // enable depth test
    glEnable(GL_DEPTH_TEST);
    // accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // -------------------------------------------------------------------------
    // Set up scene
    // -------------------------------------------------------------------------
    // set up skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files ={"textures/box3/right.bmp",
                                     "textures/box3/right.bmp",
                                     "textures/box3/right.bmp",
                                     "textures/box3/right.bmp",
                                     "textures/box3/right.bmp",
                                     "textures/box3/right.bmp"};
    Skybox skybox = Skybox(files, 1000.0f, keysPressed, keysToggled);

    // set up cubes
//    int numCubesX = 5;
//    int numCubesY = 300;
//    int numCubesZ = 5;
//    bool isTextureRendered = false;
//    CubeArrayInf cubearray = CubeArrayInf(numCubesX, numCubesY, numCubesZ, isTextureRendered);
//    CubeArrayRing cubearray = CubeArrayRing(5, 5, 100, keysPressed, keysToggled);
//    CubeArrayUser cubearray = CubeArrayUser(5, 5, 300, keysPressed, keysToggled);
    TubeTraveller cubearray = TubeTraveller(5, 5, 1000, keysPressed, keysToggled);

    // initialize player
    Player player = Player(keysPressed, keysToggled);
    // initialize camera
    Camera cam = Camera();

    // determine render mode
    bool polygonTrigger = false;
    GLuint polygonMode = 0;
    enum PolygonMode {
        FILL,
        POINT,
        LINE,
        MAX_POLYGON_MODES
    };

    // -------------------------------------------------------------------------
    // Run draw loop
    // -------------------------------------------------------------------------
    int counter = 0;
    do {

        counter++;

        // check for mouse and keyboard events
        glfwPollEvents();

        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update render mode if tab key was just released
        if (keysToggled[GLFW_KEY_CAPS_LOCK] != polygonTrigger) {
            polygonTrigger = !polygonTrigger;
            polygonMode = (polygonMode + 1) % MAX_POLYGON_MODES;
        }
        switch (polygonMode) {
            case FILL:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case LINE:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case POINT:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // RENDER CUBES
        cubearray.update(cam, player);
        cubearray.draw();

        // RENDER SKYBOX
        skybox.update(cam);
        skybox.draw();

        // swap screen buffers - outputs the buffer we have been drawing to this
        // iteration to the screen
        glfwSwapBuffers(window);

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    // clean up VAO, VBO, shader program and textures
    skybox.clean();
    cubearray.clean();

    // close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}