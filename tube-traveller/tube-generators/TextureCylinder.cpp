//
// Created by mattw on 8/21/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "TubeGenerator.h"
#include "TextureCylinder.h"
#include "../../core/loaders/loadShaders.h"
#include "../../core/loaders/loadTextures.h"
#include "../../core/loaders/loadObjIndexed.h"

TextureCylinder::TextureCylinder(GLint numCenters_) : io(IOHandler::getInstance()) {

    // location of textures
    std::vector<const char*> file_loc = {"data/textures/temp2.bmp",
                                         "data/textures/temp3.bmp"};

    // create and compile our GLSL program from the shaders
    shaderID = loadShaders("tube-traveller/shaders/SingleTexture.vert",
                           "tube-traveller/shaders/SingleTexture.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    // -------------------------------------------------------------------------
    //                          Load Textures
    // -------------------------------------------------------------------------

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "loadedTexture");

    numTextures = static_cast<GLint>(file_loc.size());

    textureIDs = new GLuint[numTextures];

    for (int i = 0; i < numTextures; ++i) {

        int width, height;
        unsigned char *image = loadBMP(file_loc[i], &width, &height);

        // generate 1 texture ID, put the resutling identifier in textureID
        glGenTextures(1, &textureIDs[i]);
        glActiveTexture(GL_TEXTURE0 + i);

        // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
        // commands will configure the currently bound texture
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // generate the texture by using the previously loaded image data
        glTexImage2D(
                GL_TEXTURE_2D,     // texture target; will gen texture on textureIDs[i]
                0,                 // mipmap level; use base of 0
                GL_RGB,            // type of format we want to store the texture
                width,
                height,
                0,                 // legacy bs
                GL_RGB,            // format of source image
                GL_UNSIGNED_BYTE,  // format of source image
                image              // source image
        );
        // textureIDs[i] now has texture image attached to it
        // glGenerateMipmap here if desired

        // free image memory
        delete[] image;

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // unbind the texture object
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    // find number of centers per texture; this will be used to initialize
    // the buffers that hold tube center vectors and rotation matrices
    GLint remainder = numCenters_ % numTextures;
    if (remainder == 0) {
        numCenters = numCenters_;
    } else {
        numCenters = numCenters_ - remainder;
    }
    numCentersPerTexture = numCenters/numTextures;

    // -------------------------------------------------------------------------
    //                          Initialize buffers
    // -------------------------------------------------------------------------

    // load cylinder model
    bool res = loadObjIndexed("data/obj/cylinder_long.obj", cylinderModelCoordinates,
                       uvs, normals);
    numVerticesPerInstance = static_cast<GLuint>(cylinderModelCoordinates.size());

    // populate buffer_data
    g_center_buffer_data = new glm::vec3*[numTextures];
    g_rotation_buffer_data = new glm::mat4*[numTextures];
    for (int i = 0; i < numTextures; ++i) {
        g_center_buffer_data[i] = new glm::vec3[numCentersPerTexture];
        g_rotation_buffer_data[i] = new glm::mat4[numCentersPerTexture];
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


    // CYLINDER MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &cylinderModelCoordinates[0], GL_STATIC_DRAW);

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


    // CYLINDER TEXTURE COORDINATES
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVerticesPerInstance,
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


    // CENTER COORDINATES
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCentersPerTexture,
                 g_center_buffer_data[0], GL_DYNAMIC_DRAW);

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


    // MODEL ROTATIONS
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &rotationBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCentersPerTexture,
                 g_rotation_buffer_data[0], GL_DYNAMIC_DRAW);

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
void TextureCylinder::update(const PathGenerator *path, Camera &cam ) {

    // update all vertices
    int count = 0;
    for (int i = 0; i < numCentersPerTexture; ++i) {
        for (int j = 0; j < numTextures; ++j) {
            g_center_buffer_data[j][i] = path->positions[count];
            g_rotation_buffer_data[j][i] = glm::rotate(glm::rotate(
                    glm::mat4(1.0f),-path->verticalAngles[count]+PI/2,
                    path->rights[count]),
                    path->horizontalAngles[count]-PI/2,
                    glm::vec3(0.f, 0.f, 1.f));
            ++count;
        }
    }

    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

void TextureCylinder::draw() {

    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(shaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    for (int i = 0; i < numTextures; ++i) {

        // Activate the texture unit first before binding texture
        glActiveTexture(GL_TEXTURE0);
        // bind texture to the currently active texture unit
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
        // puts the texture in texture unit 0
        glUniform1i(samplerID, 0);

        // bind vertex array
        glBindVertexArray(vertexArrayID);

        // send g_center_buffer_data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCentersPerTexture,
                     g_center_buffer_data[i], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // send rotation matrices to GPU
        glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCentersPerTexture,
                     g_rotation_buffer_data[i], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // draw arrays using currently active shaders
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance,
                              numCentersPerTexture);
        // break vertex array object binding
        glBindVertexArray(0);
    }

}

void TextureCylinder::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &rotationBufferID);
    glDeleteProgram(shaderID);
//    glDeleteSamplers(1, samplerID);

    for (int i = 0; i < numTextures; ++i) {
        delete[] g_center_buffer_data[i];
        delete[] g_rotation_buffer_data[i];
    }
    delete[] g_center_buffer_data;
    delete[] g_rotation_buffer_data;

}