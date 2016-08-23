//
// Created by mattw on 8/21/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "CubeTube.h"
#include "../core/loaders/loadShaders.h"
#include "../core/loaders/loadObj.h"

CubeTube::CubeTube(GLuint numCubesHorizontal_, GLuint numCubesVertical_,
                   GLuint numCenters_,
                   const bool *keysPressed_, const bool *keysToggled_) {

    numCubesHorizontal = numCubesHorizontal_;
    numCubesVertical = numCubesVertical_;
    numCenters = numCenters_;

    keysPressed = keysPressed_;
    keysToggled = keysToggled_;

    sideLength = 2.0f;
    spacing = 8.0f;     // distance between ring centers

    // create and compile our GLSL program from the shaders
    shaderID = loadShaders("tube-traveller/SolidShader.vert",
                           "tube-traveller/SolidShader.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    // load cube model
    bool res = loadObj("tube-traveller/cube.obj", cubeModelCoordinates, uvs, normals);
    numVertices = static_cast<GLuint>(cubeModelCoordinates.size());
    numModelsPerRing = 2 * numCubesHorizontal + 2 * (numCubesVertical - 2);
    numVerticesPerInstance = numVertices;

    // -------------------------------------------------------------------------
    //                          Fill buffers
    // -------------------------------------------------------------------------

    modelOffsets = new glm::vec3[numModelsPerRing];
    ringOffsets = new glm::vec3[numCenters];
    rotationMatrix = new glm::mat4[numCenters];

    // get radial offsets
    int counter = 0;
    for (int i = 0; i < numCubesHorizontal; ++i) {
        for (int j = 0; j < numCubesVertical; ++j) {
            if (i == 0 || i == numCubesHorizontal-1 ||
                j == 0 || j == numCubesVertical-1) {
                modelOffsets[counter] = glm::vec3(
                        spacing * static_cast<float>(-numCubesHorizontal/2 + i),
                        0,
                        spacing * static_cast<float>(-numCubesVertical/2 + j));
                counter++;
            }
        }
    }

    // get centers
    for (int i = 0; i < numCenters; ++i) {
        ringOffsets[i] = glm::vec3(0.f, 0.f, 0.f);
        rotationMatrix[i] = glm::rotate(glm::mat4(1.0f),
                                        0.f,
                                        glm::vec3(0.0f, 0.0f, 1.f));
    }

    // populate buffer_data
    g_center_buffer_data = new glm::vec3[numCenters * numModelsPerRing];
    g_radial_buffer_data = new glm::vec3[numCenters * numModelsPerRing];
    g_rotation_buffer_data = new glm::mat4[numCenters * numModelsPerRing];

    // get all vertices
    counter = 0;
    for (int i = 0; i < numCenters; ++i) {
        for (int j = 0; j < numModelsPerRing; ++j) {
            g_center_buffer_data[counter] = ringOffsets[i];
            g_radial_buffer_data[counter] = modelOffsets[j];
            g_rotation_buffer_data[counter] = rotationMatrix[i];
            counter++;
        }
    }

    // -------------------------------------------------------------------------
    //                          Create VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
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
                 &cubeModelCoordinates[0], GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters * numModelsPerRing,
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters * numModelsPerRing,
                 g_radial_buffer_data, GL_DYNAMIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters * numModelsPerRing,
                 g_rotation_buffer_data, GL_DYNAMIC_DRAW);

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

}

/*
 * update buffers with newly created path data
 */
void CubeTube::update(const PathUserInput &path, Player &player,
                      Camera &cam ) {

    // update render mode if tab key was just released
    if (keysToggled[GLFW_KEY_SPACE] != playerModeTrigger) {
        playerModeTrigger = !playerModeTrigger;
        playerMode = (playerMode + 1) % MAX_PLAYER_MODES;
    }
    switch (playerMode) {
        case PLAYER_FREE:
            // let player update like normal
            //player.update();
            break;
        case PLAYER_BOUND:
        {
            // get all vertices
            int counter = 0;
            for (int i = 0; i < numCenters; ++i) {
                for (int j = 0; j < numModelsPerRing; ++j) {
                    g_center_buffer_data[counter] = path.positions[i];
                    g_rotation_buffer_data[counter] = glm::rotate(glm::rotate(
                            glm::mat4(1.0f),-path.verticalAngles[i]+PI/2, path.rights[i]),
                            path.horizontalAngles[i]-PI/2, glm::vec3(0.f, 0.f, 1.f));
                    counter++;
                }
            }
            break;
        }
        default:
            player.update();
    }
    // compute view and projection matrices from player info
    cam.update(player);

    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

void CubeTube::draw() {

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

    // send g_center_buffer_data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters * numModelsPerRing,
                 g_center_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // send rotation matrices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters * numModelsPerRing,
                 g_rotation_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw arrays using currently active shaders
    glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance,
                          numCenters * numModelsPerRing);
    // break vertex array object binding
    glBindVertexArray(0);

}

void CubeTube::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &radialBufferID);
    glDeleteBuffers(1, &rotationBufferID);
    glDeleteProgram(shaderID);

    delete[] ringOffsets;
    delete[] g_center_buffer_data;
    delete[] modelOffsets;
    delete[] g_radial_buffer_data;
    delete[] rotationMatrix;
    delete[] g_rotation_buffer_data;

}