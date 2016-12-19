//
// Created by mattw on 12/18/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../TubeTraveller.h"
#include "TextureCylinderLight.h"
#include "../../core/loaders/loadObjIndexed.h"
#include "../texture-generators/StaticFiles.h"
#include "../texture-generators/Rainbow.h"
#include "../texture-generators/Binary.h"
#include "../texture-generators/Noise.h"

TextureCylinderLight::TextureCylinderLight(GLint numCenters_, TubeTraveller::TextureType textureType_)
        :
        io(IOHandler::getInstance()) {

    // tube properties
    tubeRadius = 10.f;
    tubeLength = 2.f;

    // create and compile our GLSL program from the shaders
    shader = new Shader("tube-traveller/shaders/Lights.vert",
                        "tube-traveller/shaders/Lights.frag");

    // -------------------------------------------------------------------------
    //                          Load Textures
    // -------------------------------------------------------------------------

    switch (textureType_) {
        case TubeTraveller::TEXTURE_FILES_STATIC:
            texture = new StaticFiles(shader->programID);
            break;
        case TubeTraveller::TEXTURE_RAINBOW:
            texture = new Rainbow(shader->programID);
            break;
        case TubeTraveller::TEXTURE_BINARY:
            texture = new Binary(shader->programID);
            break;
        case TubeTraveller::TEXTURE_NOISE:
            texture = new Noise(shader->programID);
            break;
        default:
            texture = new Rainbow(shader->programID);
    }

    // find number of centers per texture; this will be used to initialize
    // the buffers that hold tube center vectors and rotation matrices
    GLint remainder = numCenters_ % texture->numTextures;
    if (remainder == 0) {
        numCenters = numCenters_;
    } else {
        numCenters = numCenters_ - remainder;
    }
    numCentersPerTexture = numCenters/texture->numTextures;

    // -------------------------------------------------------------------------
    //                          Initialize buffers
    // -------------------------------------------------------------------------

    // load cylinder model
    bool res = loadObjIndexed("data/obj/cylinder_articulated.obj",
                              cylinderModelCoordinates, uvs, normals);
    numVerticesPerInstance = static_cast<GLuint>(cylinderModelCoordinates.size());

    // populate buffer_data
    g_center_buffer_data = new glm::vec3*[texture->numTextures];
    g_rotation_buffer_data = new glm::mat4*[texture->numTextures];
    g_rotation2_buffer_data = new glm::mat4*[texture->numTextures];
    for (int i = 0; i < texture->numTextures; ++i) {
        g_center_buffer_data[i] = new glm::vec3[numCentersPerTexture];
        g_rotation_buffer_data[i] = new glm::mat4[numCentersPerTexture];
        g_rotation2_buffer_data[i] = new glm::mat4[numCentersPerTexture];
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


    // CYLINDER NORMALS
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
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
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
            3,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(3, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL ROTATIONS 1
    // holds rotation matrix for first ring in current cylinder
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &rotationBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCentersPerTexture,
                 g_rotation_buffer_data[0], GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    GLsizei vec4size = sizeof(glm::vec4);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(vec4size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2 * vec4size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3 * vec4size));
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL ROTATIONS 2
    // holds rotation matrix for first ring in next cylinder
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &rotation2BufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, rotation2BufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCentersPerTexture,
                 g_rotation2_buffer_data[0], GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(vec4size));
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2 * vec4size));
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3 * vec4size));
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);
    glVertexAttribDivisor(11, 1);
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

    // define light properties
    cameraPosition = glm::vec3(1.f);
    lightPosition = glm::vec3(1.f);
    lightAmbient = glm::vec3(0.5);
    lightDiffuse = glm::vec3(0.8);
    lightSpecular = glm::vec3(1.0);
    lightAttLin = 0.022f;  // vals from https://learnopengl.com/#!Lighting/Materials
    lightAttQuad = 0.0019f;

}

/*
 * update buffers with newly created path data
 */
void TextureCylinderLight::update(const PathGenerator *path, Camera &cam ) {

    // update all vertices
    int count = 0;
    for (int i = 0; i < numCentersPerTexture; ++i) {
        for (int j = 0; j < texture->numTextures; ++j) {
            g_center_buffer_data[j][i] = path->positions[count];
            g_rotation_buffer_data[j][i] = glm::rotate(glm::rotate(
                    glm::mat4(1.0f),-path->verticalAngles[count]+PI/2,
                    path->rights[count]),
                    path->horizontalAngles[count]-PI/2,
                    glm::vec3(0.f, 0.f, 1.f));
            if (count == path->firstElement) {
                g_rotation2_buffer_data[j][i] = glm::rotate(glm::rotate(
                        glm::mat4(1.0f),-path->verticalAngles[count]+PI/2,
                        path->rights[count]),
                        path->horizontalAngles[count]-PI/2,
                        glm::vec3(0.f, 0.f, 1.f));
            } else if (count == 0) {
                g_rotation2_buffer_data[j][i] = glm::rotate(glm::rotate(
                        glm::mat4(1.0f),-path->verticalAngles[numCenters-1]+PI/2,
                        path->rights[numCenters-1]),
                        path->horizontalAngles[numCenters-1]-PI/2,
                        glm::vec3(0.f, 0.f, 1.f));
            } else {
                g_rotation2_buffer_data[j][i] = glm::rotate(glm::rotate(
                        glm::mat4(1.0f),-path->verticalAngles[count-1]+PI/2,
                        path->rights[count-1]),
                        path->horizontalAngles[count-1]-PI/2,
                        glm::vec3(0.f, 0.f, 1.f));
            }
            ++count;
        }
    }

    // update uniforms
    time = glfwGetTime();
    mMatrix[0] = glm::vec4(tubeRadius, 0.0, 0.0, 0.0);
    mMatrix[1] = glm::vec4(0.0, tubeLength, 0.0, 0.0);
    mMatrix[2] = glm::vec4(0.0, 0.0, tubeRadius, 0.0);
    mMatrix[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

    // update textures depending on path variables
    texture->update(path);

    // update light position
    lightPosition = path->positions[(path->firstElement+50) % 100];
    cameraPosition = cam.getPosition();

}

void TextureCylinderLight::draw() {

    // use our shader (makes programID "currently bound" shader?)
    shader->use();

    // send data to vertex shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    // send data to fragment shader
    // material properties loaded by TextureGenerator object
    // light properites
    glUniform3fv(glGetUniformLocation(shader->programID, "cameraPosition"),
                 1, &cameraPosition[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "pointLight.position"),
                 1, &lightPosition[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "pointLight.ambient"),
                 1, &lightAmbient[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "pointLight.diffuse"),
                 1, &lightDiffuse[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "pointLight.specular"),
                 1, &lightSpecular[0]);
    glUniform1f(glGetUniformLocation(shader->programID, "pointLight.lin"),
                 lightAttLin);
    glUniform1f(glGetUniformLocation(shader->programID, "pointLight.quad"),
                 lightAttQuad);

    for (int i = 0; i < texture->numTextures; ++i) {

        texture->draw(i);

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

        // send rotation matrices to GPU
        glBindBuffer(GL_ARRAY_BUFFER, rotation2BufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCentersPerTexture,
                     g_rotation2_buffer_data[i], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // draw arrays using currently active shaders
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance,
                              numCentersPerTexture);
        // break vertex array object binding
        glBindVertexArray(0);
    }

}

void TextureCylinderLight::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &rotationBufferID);

    for (int i = 0; i < texture->numTextures; ++i) {
        delete[] g_center_buffer_data[i];
        delete[] g_rotation_buffer_data[i];
    }
    delete[] g_center_buffer_data;
    delete[] g_rotation_buffer_data;

    texture->clean();
    delete texture;
    shader->clean();
    delete shader;

}