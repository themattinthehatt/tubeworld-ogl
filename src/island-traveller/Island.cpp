//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "Island.h"
#include "../core/loaders/loadObj.h"

Island::Island() : io(IOHandler::getInstance()) {

    // create and compile our GLSL program from the shaders
    shader = new Shader("src/island-traveller/SolidShader.vert",
                        "src/island-traveller/SolidShader.frag");

    // -------------------------------------------------------------------------
    //                          Initialize buffers
    // -------------------------------------------------------------------------

    // load cylinder model
    bool res = loadObj("data/obj/cube.obj",
                       islandCoordinates, uvs, normals);
    numVertices = static_cast<GLuint>(islandCoordinates.size());

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


    // ISLAND COORDINATES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &islandCoordinates[0], GL_STATIC_DRAW);

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


    // ISLAND NORMALS
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &normals[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 2; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // ISLAND UV COORDINATES
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVertices,
                 &uvs[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,         // attribute 2; must match "layout" in shader
            2,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbind the VAO
    glBindVertexArray(0);

    // -------------------------------------------------------------------------
    //                          Initialize uniforms
    // -------------------------------------------------------------------------

    // vertex shader uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shader->programID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shader->programID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shader->programID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shader->programID, "time");

}

void Island::update(Camera &cam, Player &player) {

    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

void Island::draw() {

    // use our shader (makes programID currently bound shader)
    shader->use();

    // send data to vertex shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    // bind vertex array
    glBindVertexArray(vertexArrayID);

    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    // break vertex array object binding
    glBindVertexArray(0);

}

void Island::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);

    shader->clean();
    delete shader;

}