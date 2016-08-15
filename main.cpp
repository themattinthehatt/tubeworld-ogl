// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic
#include <GL/glew.h>

// Include GLFW - handles window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - 3D mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "loadShader.h"
#include "loadTextures.h"
#include "Camera.h"
#include "Skybox.h"
#include "CubeModelCoordinates.h"
#include "CubeColorCoordinates.h"
#include "CubeUVCoordinates.h"
#include "createCubeArray.h"

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
    GLFWwindow *window;
    window = glfwCreateWindow(1024, 768, "tubeworld 2.0", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // -------------------------------------------------------------------------
    // GLEW - queries location of OpenGL functions at run-time and stores
    // -------------------------------------------------------------------------
    glewExperimental = GL_TRUE; // needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        return -1;
    }

    // tell OpenGL the size of the rendering window
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // OpenGL uses data specified here to transform the 2D coordinates it
    // processes to the coordinates on the screen
    glViewport(0, 0, width, height); // lower left corner, width, height

    // ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//    // hide the mouse and enable unlimited movement
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    // set the mouse at the center of the screen
//    glfwPollEvents();
//    glfwSetCursorPos(window, 1024/2, 768/2);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    // accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // set up skybox
    // +x, -x, +y, -y, +z, -z
    std::vector<const char*> files ={"box3/right.bmp", "box3/right.bmp",
                                     "box3/right.bmp", "box3/right.bmp",
                                     "box3/right.bmp", "box3/right.bmp"};
    Skybox skybox = Skybox(files, 100.0f);

    /*
     * -------------------------------------------------------------------------
     *                              CUBES
     * -------------------------------------------------------------------------
     */

    bool isTextureRendered = true;
    GLuint programID;
    if (isTextureRendered)
        // create and compile our GLSL program from the shaders
        programID = loadShader("TextureShader.vert",
                               "TextureShader.frag");
    else
        // create and compile our GLSL program from the shaders
        programID = loadShader("SolidShader.vert",
                               "SolidShader.frag");

    // give the MVP matrix to GLSL; get a handle on our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // -------------------------- CUBE VERTICES --------------------------------
    // vertex array objects; stores
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(VertexArrayID);
    // from here we should bind/config corresponding VBO(s) and attribute pointers

    // COPY VERTEX ARRAY INTO A BUFFER
    // specify model coordinates
    int numCubesX = 5;
    int numCubesY = 5;
    int numCubesZ = 1;
    int numCubes = numCubesX * numCubesY * numCubesZ;
    GLfloat g_vertex_buffer_data[numCubes * numModelVertices *
                                 3]; // numModelVertices in CubeModelCoordinates.h
    // populate g_vertex_buffer_data
    createCubeArray(numCubesX, numCubesY, numCubesZ, g_vertex_buffer_data);

    // identify our vertex buffer object
    GLuint VertexBufferID;
    // generate 1 buffer, put the resulting identifier in VertexBufferID
    glGenBuffers(1, &VertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                 g_vertex_buffer_data, GL_STATIC_DRAW);
    // set vertex attribute pointers
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    glEnableVertexAttribArray(0);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // --------------------- CUBE TEXTURES / COLORS ----------------------------
    GLuint Texture;         // store texture
    GLuint TextureID;       // ids the texture
    GLuint UVBufferID;      // id our UV coordinates buffer
    GLuint ColorBufferID;   // id our color buffer
    if (isTextureRendered) {
        Texture = loadBMP_custom("uvtemplate.bmp");
        // get a handle for our "myTextureSampler" uniform
        TextureID = glGetUniformLocation(programID, "myTextureSampler");

        // create data to put in UV buffer
        GLfloat g_uv_buffer_data[numCubes*numModelVertices*2];
        createUVArray(g_uv_buffer_data, numCubes);
        // generate 1 buffer, put the resulting identifier in UVBufferID
        glGenBuffers(1, &UVBufferID);
        // bind newly created buffer to GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, UVBufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
                1,
                2,
                GL_FLOAT,
                GL_FALSE,
                0,
                (GLvoid *) 0
        );
    }
    else {
        // create data to put in color buffer
        GLfloat g_color_buffer_data[numCubes*numModelVertices*3];
        createColorArray(g_color_buffer_data, numCubes);
        // generate 1 buffer, put the resulting identifier in VertexBufferID
        glGenBuffers(1, &ColorBufferID);
        // bind newly created buffer to GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data),
                     g_color_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (GLvoid *) 0
        );
    }
    glEnableVertexAttribArray(1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind the VAO
    glBindVertexArray(0);


    // initialize camera (and MVP model components)
    Camera cam = Camera(window);
    glm::mat4 modelMatrix;
    glm::mat4 MVP;


    int numVertices;
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // run draw loop
    do {

        // check for mouse and keyboard events
        glfwPollEvents();

        // set black background on screen clear
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // compute view and projection matrices from keyboard and mouse input
        cam.update();

        modelMatrix = glm::mat4(1.0);
        MVP = cam.Projection * cam.View * modelMatrix;

        // ------------------- RENDER SKYBOX -----------------------------------
        skybox.update(cam);
        skybox.draw();

        // ------------------- RENDER CUBES ------------------------------------
//        // play with colors
        GLfloat timeValue = glfwGetTime();
        GLfloat hueMultiplier = (sin(timeValue) / 2) + 0.5;
        GLuint timeMultiplier = glGetUniformLocation(programID, "time");

        // use our shader (makes programID "currently bound" shader?)
        glUseProgram(programID);

        // send our transformation to the currently bound shader, in the "MVP" uniform
        // This is done in the main loop since each model will have a different MVP matrix
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform1f(timeMultiplier, hueMultiplier);

        if (isTextureRendered) {
            // Activate the texture unit first before binding texture
            glActiveTexture(GL_TEXTURE0);
            // binds Texture to the currently active texture unit
            glBindTexture(GL_TEXTURE_2D, Texture);
            // puts the texture in texture unit 0
            glUniform1i(TextureID, 0);
//            glActiveTexture(GL_TEXTURE1);
//            glBindTexture(GL_TEXTURE_2D, Texture2);
//            glUniform1i(TextureID, 1);
        }

        // bind vertex array
        glBindVertexArray(VertexArrayID);
        // draw the squares!
        numVertices = sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]) / 3;
        // draw arrays using currently active shaders
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        // break vertex array object binding
        glBindVertexArray(0);
        // ---------------------------------------------------------------------


        // swap screen buffers - outputs the buffer we have been drawing to this
        // iteration to the screen
        glfwSwapBuffers(window);

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    // clean up VAO, VBO, shader program and textures
    skybox.clean();
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &VertexBufferID);
    glDeleteProgram(programID);
    if (isTextureRendered) {
        glDeleteTextures(1, &TextureID);
        glDeleteBuffers(1, &UVBufferID);
    }
    else
        glDeleteBuffers(1, &ColorBufferID);

    // close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}