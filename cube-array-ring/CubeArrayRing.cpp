//
// Created by mattw on 8/16/16.
//
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "CubeArrayRing.h"
#include "../core/loaders/loadShaders.h"
#include "CubeRing.h"

/*
 * CubeArray()
 * Constructor method for CubeArray class
 */
CubeArrayRing::CubeArrayRing(int numCubesHorizontal_, int numCubesVertical_,
                             int numCubeRings_) {

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
    playerPos = glm::vec3(1.0f);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    tubeOrigin = glm::vec3(0.f, 0.f, 0.f);

    // create VAO for CubeArray to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // CUBE MODEL COORDINATES
    numModelsPerRing = 2 * numCubesHorizontal + 2 * (numCubesVertical - 2);
    numVerticesPerInstance = numVertices;

    modelOffsets = new glm::vec3[numModelsPerRing];
//    glm::vec3 *normalVec[numRing];
    ringOffsets = new glm::vec3[numCubeRings];

    sideLength = 2.0f;
    spacing = 5.0f;

    // get offsets
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

    // get centers
    for (int i = 0; i < numCubeRings; ++i) {
        ringOffsets[i] = glm::vec3(0.0, spacing* static_cast<float>(i), 0.f);
    }

    // populate buffer_data
    glm::vec3 *g_center_buffer_data;
    g_center_buffer_data = new glm::vec3[numCubeRings * numModelsPerRing];
    glm::vec3 *g_radial_offset_buffer_data;
    g_radial_offset_buffer_data = new glm::vec3[numCubeRings * numModelsPerRing];

    // get all vertices
    counter = 0;
    for (int i = 0; i < numCubeRings; ++i) {
        for (int j = 0; j < numModelsPerRing; ++j) {
            g_center_buffer_data[counter] = ringOffsets[i];
            g_radial_offset_buffer_data[counter] = modelOffsets[j];
            counter++;
        }
    }

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
                 g_radial_offset_buffer_data, GL_STATIC_DRAW);

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


    // unbind the VAO
    glBindVertexArray(0);

    delete[] modelOffsets;
    delete[] ringOffsets;
    delete[] g_center_buffer_data;
    delete[] g_radial_offset_buffer_data;
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
void CubeArrayRing::update(const Camera &cam) {
    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;
    playerPos = cam.getPosition();
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