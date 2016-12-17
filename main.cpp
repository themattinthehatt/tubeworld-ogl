// TODO
// - use SOIL rather than custom image loader so that I can store jpgs instead
//   of bitmaps - update this next time I start using textures
// - get serious about "models", and move away from custom code? Should be soon,
//   as ideally I'll update all old code to reflect this (ASSIMP?)

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

// include tubeworld components
#include "core/Debug.h"
#include "core/Camera.h"
#include "core/Skybox.h"
#include "tube-traveller/TubeTraveller.h"
#include "physic-spheres/PhysicSpheres.h"

// forward declarations
GLFWwindow* openGLInit();

int main() {

    GLFWwindow *window = openGLInit();
    if (window == nullptr) { return -1; }

    IOHandler &io(IOHandler::getInstance());

    // -------------------------------------------------------------------------
    // Set up scene
    // -------------------------------------------------------------------------
    // set up skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/box3/front.bmp",
                                      "data/textures/box3/back.bmp",
                                      "data/textures/box3/left.bmp",
                                      "data/textures/box3/right.bmp",
                                      "data/textures/box3/up.bmp",
                                      "data/textures/box3/down.bmp"};
    Skybox skybox = Skybox(files, 1000.0f);

    // set up tube
    GLint numCenters = 100;
    TubeTraveller cubearray = TubeTraveller(numCenters);

//    PhysicSpheres cubearray = PhysicSpheres2();

    // initialize player
    Player player = Player();
    // initialize camera
    Camera cam = Camera();

    // determine render mode
    bool polygonTrigger = false;
    GLuint polygonMode = 0;
    enum PolygonMode {
        POLY_MODE_FILL,
        POLY_MODE_POINT,
        POLY_MODE_LINE,
        MAX_NUM_POLY_MODES
    };

    // -------------------------------------------------------------------------
    // Run draw loop
    // -------------------------------------------------------------------------

    // for printing FPS
    bool printFPS = true;
    int counter = 0;
    GLfloat currentTime = glfwGetTime();
    GLfloat lastTime;

    do {

        if (printFPS) {
            counter++;
            // output FPS
            lastTime = currentTime;
            currentTime = glfwGetTime();
            if (counter % 100 == 0){
                std::cout << 1.f / (currentTime - lastTime) << std::endl;
            }
        }

        // check for mouse and keyboard events
        glfwPollEvents();

        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update render mode if tab key was just released
        if (io.keysToggled[GLFW_KEY_CAPS_LOCK] != polygonTrigger) {
            polygonTrigger = !polygonTrigger;
            polygonMode = (polygonMode + 1) % MAX_NUM_POLY_MODES;
        }
        switch (polygonMode) {
            case POLY_MODE_FILL:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case POLY_MODE_LINE:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case POLY_MODE_POINT:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // RENDER SKYBOX
        skybox.update(cam);
        skybox.draw();

        // RENDER CUBES
        cubearray.update(cam, player);
        cubearray.draw();


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



GLFWwindow* openGLInit() {

    // -------------------------------------------------------------------------
    // GLFW - Initialize window and an OpenGL context to render in
    // -------------------------------------------------------------------------
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return nullptr;
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

    const GLFWvidmode *mode = glfwGetVideoMode(useMonitor);
    int xResolution = mode->width;
    int yResolution = mode->height;

    GLuint screenWidth = xResolution; //1200;
    GLuint screenHeight = yResolution; //900;
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight,
                                          "tubeworld 2.0", nullptr, nullptr);
//  GLFWwindow * window = glfwCreateWindow(screenWidth, screenHeight,
//                                         "tubeworld 2.0", useMonitor, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // set up callback function for when a physical key is pressed or released
    glfwSetKeyCallback(window, IOHandler::getInstance().baseKeyCallback);
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
        return nullptr;
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
    // size of points in POINT mode
    glPointSize(3.f);

    return window;
}