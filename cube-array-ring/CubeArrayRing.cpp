//
// Created by mattw on 8/16/16.
//
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CubeArrayRing.h"
#include "../core/loaders/loadShaders.h"

/*
 * CubeArray()
 * Constructor method for CubeArray class
 */
CubeArrayRing::CubeArrayRing(int numCubesHorizontal_, int numCubesVertical_,
                             int numCubeRings_) : io(IOHandler::getInstance()){

    numCubesHorizontal = numCubesHorizontal_;
    numCubesVertical = numCubesVertical_;
    numCubeRings = numCubeRings_;

    // create and compile our GLSL program from the shaders
    shaderID = loadShaders("cube-array-ring/SolidShader.vert",
                           "cube-array-ring/SolidShader.frag");

    // give the MVP matrix to GLSL; get a handle on our "MVP" uniform
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    sideLength = 2.0f;
    spacing = 5.0f;

    numModelsPerRing = 2 * numCubesHorizontal + 2 * (numCubesVertical - 2);
    numVerticesPerInstance = numVertices;

    modelOffsets = new glm::vec3[numModelsPerRing];
    ringOffsets = new glm::vec3[numCubeRings];
    normalVec = new glm::vec3[numCubeRings];
    rotationMatrix = new glm::mat4[numCubeRings];

    // get radial offsets
    int counter = 0;
    for (int i = 0; i < numCubesHorizontal; ++i) {
        for (int j = 0; j < numCubesVertical; ++j) {
            if (i == 0 || i == numCubesHorizontal-1 || j == 0 || j == numCubesVertical-1) {
                modelOffsets[counter] = glm::vec3(
                        spacing * static_cast<float>(-numCubesHorizontal/2 + i),
                        0,
                        spacing * static_cast<float>(-numCubesVertical/2 + j));
                counter++;
            }
        }
    }

    // follow a circle
    float *angles;
    angles = new float[numCubeRings];

    // get centers
    for (int i = 0; i < numCubeRings; ++i) {
        angles[i] = static_cast<float>(i) / (2 * PI);

        ringOffsets[i] = glm::vec3(100.f * cos(angles[i]),
                                   100.f * sin(angles[i]),
                                   0.f);
    }

    // orientation information
    for (int i = 0; i < numCubeRings; ++i) {
        normalVec[i] = glm::vec3(0.f, 1.0f, 0.f);
        rotationMatrix[i] = glm::rotate(glm::mat4(1.0f),
                                        angles[i],
                                        glm::vec3(0.0f, 0.0f, 1.f));
    }


    // populate buffer_data
    g_center_buffer_data = new glm::vec3[numCubeRings * numModelsPerRing];
    g_radial_buffer_data = new glm::vec3[numCubeRings * numModelsPerRing];
    g_rotation_buffer_data = new glm::mat4[numCubeRings * numModelsPerRing];

    // get all vertices
    counter = 0;
    for (int i = 0; i < numCubeRings; ++i) {
        for (int j = 0; j < numModelsPerRing; ++j) {
            g_center_buffer_data[counter] = ringOffsets[i];
            g_radial_buffer_data[counter] = modelOffsets[j];
            g_rotation_buffer_data[counter] = rotationMatrix[i];
            counter++;
        }
    }

    // create VAO for CubeArray to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // CUBE MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in VertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 cubeModelCoordinates, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // RING CENTER COORDINATES
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCubeRings * numModelsPerRing,
                 g_center_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(1, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // RING OFFSET COORDINATES
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &radialBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, radialBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCubeRings * numModelsPerRing,
                 g_radial_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(2, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL ROTATIONS
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &rotationBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCubeRings * numModelsPerRing,
                 g_rotation_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    GLsizei vec4size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(vec4size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2 * vec4size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3 * vec4size));
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // unbind the VAO
    glBindVertexArray(0);

    delete[] angles;

}

/*
 * ~CubeArray()
 * Destructor method for CubeArray class
 */
CubeArrayRing::~CubeArrayRing() {}

/*
 * update()
 * Update position of cube array
 */
void CubeArrayRing::update(Camera &cam, Player &player) {

    // update render mode if tab key was just released
    if (io.keysToggled[GLFW_KEY_SPACE] != playerModeTrigger) {
        playerModeTrigger = !playerModeTrigger;
        playerMode = (playerMode + 1) % MAX_PLAYER_MODES;
    }
    switch (playerMode) {
        case PLAYER_FREE:
            std::cout << "player free" << std::endl;
            // let player update like normal
            player.update();
            // compute view and projection matrices from player info
            cam.update(player);
            break;
        case PLAYER_BOUND:
            std::cout << "player bound" << std::endl;
            // change player controls
            // update time variables.
            player.setLastTime(player.getCurrentTime());
            player.setCurrentTime(static_cast<float>(glfwGetTime()));
            player.setDeltaTime(player.getCurrentTime() - player.getLastTime());
            player.moveTheta(player.getDeltaTime() * player.getRotationSpeed(),
                             glm::vec3(0.f, 0.f, 0.f));
            // move forward or up
            if (io.keysPressed[GLFW_KEY_UP]) {
                if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
                    player.moveUp(player.getDeltaTime() * player.getSpeed());
                else
                    player.moveTheta(player.getDeltaTime() * player.getRotationSpeed(),
                                     glm::vec3(0.f, 0.f, 0.f));
            }
            // move backward or down
            if (io.keysPressed[GLFW_KEY_DOWN]) {
                if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
                    player.moveDown(player.getDeltaTime() * player.getSpeed());
                else
                    player.moveTheta(-player.getDeltaTime() * player.getRotationSpeed(),
                                     glm::vec3(0.f, 0.f, 0.f));
            }
            // strafe right
            if (io.keysPressed[GLFW_KEY_RIGHT]) {
                player.moveRight(player.getDeltaTime() * player.getSpeed());
            }
            // strafe left
            if (io.keysPressed[GLFW_KEY_LEFT]) {
                player.moveLeft(player.getDeltaTime() * player.getSpeed());
            }
            // rotate up
            if (io.keysPressed[GLFW_KEY_W]) {
                player.rotateUp(player.getDeltaTime() * player.getRotationSpeed());
            }
            // rotate down
            if (io.keysPressed[GLFW_KEY_S]) {
                player.rotateDown(player.getDeltaTime() * player.getRotationSpeed());
            }
            // rotate right
            if (io.keysPressed[GLFW_KEY_D]) {
                player.rotateRight(player.getDeltaTime() * player.getRotationSpeed());
            }
            // rotate left
            if (io.keysPressed[GLFW_KEY_A]) {
                player.rotateLeft(player.getDeltaTime() * player.getRotationSpeed());
            }
            // decrease speed
            if (io.keysPressed[GLFW_KEY_E]) {
                player.incrementRotationSpeed(-0.001f);
            }
            // increase speed
            if (io.keysPressed[GLFW_KEY_R]) {
                player.incrementRotationSpeed(0.001f);
            }
            // reset
            if (io.keysPressed[GLFW_KEY_Q]) {
                player.reset();
            }

            // clamp vertical angle b/t 0 and PI
            if (player.getVerticalAngle() < 0)
                player.setVerticalAngle(0);
            else if (player.getVerticalAngle() > PI)
                player.setVerticalAngle(PI);

            // mod out horizontal angle by 2*PI
            player.setHorizontalAngle(static_cast<float>(
                                 fmod(player.getHorizontalAngle(), 2.0f * PI)));

            // restrict speed
            if (player.getSpeed() > player.getMaxSpeed())
                player.setSpeed(player.getMaxSpeed());
            else if (player.getSpeed() < 0)
                player.setSpeed(0);

            // update heading information.
            player.setHeading(glm::vec3(
                    cos(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
                    sin(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
                    cos(player.getVerticalAngle()))
            );
            // right vector
            player.setRight(glm::vec3(
                    cos(player.getHorizontalAngle() - PI/2),
                    sin(player.getHorizontalAngle() - PI/2),
                    0)
            );
            // up vector
            player.setUp(glm::cross(player.getRight(), player.getHeading()));



            // update camera
            cam.update(player);

            break;

        default:
            player.update();
            cam.update(player);
    }

    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

/*
 * draw()
 * Draw cube array to screen
 */
void CubeArrayRing::draw() {
    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(shaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    // bind vertex array
    glBindVertexArray(vertexArrayID);
    // draw arrays using currently active shaders
    glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance,
                          numCubeRings * numModelsPerRing);
    // break vertex array object binding
    glBindVertexArray(0);

}

/*
 * clean()
 * Clean up VAOs, VBOs, etc.
 */
void CubeArrayRing::clean() {
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteProgram(shaderID);


    delete[] ringOffsets;
    delete[] g_center_buffer_data;
    delete[] modelOffsets;
    delete[] g_radial_buffer_data;
    delete[] normalVec;
    delete[] rotationMatrix;
    delete[] g_rotation_buffer_data;
}

glm::vec3 CubeArrayRing::cubeModelCoordinates[] = {
        glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f), // triangle 1 : end
        glm::vec3(1.0f, 1.0f,-1.0f), // triangle 2 : begin
        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f), // triangle 2 : end

        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),

        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),

        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),

        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),

        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f)
};