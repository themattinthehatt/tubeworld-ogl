//
// Created by mattw on 9/4/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "PhysicSpheres.h"
#include "../core/loaders/loadShaders.h"
#include "../core/loaders/loadObj.h"
#include "../core/loaders/loadObjIndexed.h"

PhysicSpheres::PhysicSpheres()
        :
        io(IOHandler::getInstance()) {

    numCenters = 1000; // 10000@~30 fps, 1000@~225 fps

    // create and compile our GLSL program from the shaders
    shaderID = loadShaders("physic-spheres/SolidShader.vert",
                           "physic-spheres/SolidShader.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    // load sphere model
//    bool res = loadObj("physic-spheres/sphere.obj", sphereModelCoordinates, uvs, normals);
    bool res = loadObjIndexed("physic-spheres/ico_sphere_4.obj", sphereModelCoordinates, uvs, normals);
    numVertices = static_cast<GLuint>(sphereModelCoordinates.size());
    numVerticesPerInstance = numVertices;

    // populate buffer_data
    g_center_buffer_data = new glm::vec3[numCenters];
    g_radial_buffer_data = new glm::vec3[numCenters];
    g_rotation_buffer_data = new glm::mat4[numCenters];

    // get all vertices
    int counter = 0;
    for (int i = 0; i < numCenters; ++i) {
        g_center_buffer_data[counter] = glm::vec3(5.f, static_cast<GLfloat>(i)*5.f, 0.f);
        g_radial_buffer_data[counter] = glm::vec3(0.f);
        g_rotation_buffer_data[counter] = glm::mat4(1.f);
        counter++;
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


    // SPHERE MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in VertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &sphereModelCoordinates[0], GL_STATIC_DRAW);

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


    //
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
                 g_center_buffer_data, GL_DYNAMIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters,
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
void PhysicSpheres::update(Camera &cam, Player &player) {

    player.update();
    cam.update(player);

    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

void PhysicSpheres::draw() {

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
                 g_center_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // send rotation matrices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters,
                 g_rotation_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw arrays using currently active shaders
    glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance,
                          numCenters);
    // break vertex array object binding
    glBindVertexArray(0);

}

void PhysicSpheres::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &radialBufferID);
    glDeleteBuffers(1, &rotationBufferID);
    glDeleteProgram(shaderID);

    delete[] ringCenters;
    delete[] g_center_buffer_data;
    delete[] modelOffsets;
    delete[] g_radial_buffer_data;
    delete[] rotationMatrix;
    delete[] g_rotation_buffer_data;

}