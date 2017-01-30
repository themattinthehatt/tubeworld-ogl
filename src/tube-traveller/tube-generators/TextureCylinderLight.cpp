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

TextureCylinderLight::TextureCylinderLight(GLint numCenters_,
        TextureType textureType,
        LightStyle lightStyle)
        :
        io(IOHandler::getInstance()) {

    // tube properties
    tubeRadius = 10.f;
    tubeLength = 2.f;

    // initialize lights
    switch (lightStyle) {
        case LIGHTSTYLE_NONE:
            // create and compile our GLSL program from the shaders
            shader = new Shader("src/tube-traveller/shaders/SingleTexture.vert",
                                "src/tube-traveller/shaders/SingleTexture.frag");
            // no light
            light = nullptr;
            break;
        case LIGHTSTYLE_POINT:
            // create and compile our GLSL program from the shaders
            shader = new Shader("src/tube-traveller/shaders/Lights.vert",
                                "src/tube-traveller/shaders/Lights.frag");
            // create light
            light = new Light(shader->programID);
            break;
        default: // LIGHTSTYLE_NONE
            // create and compile our GLSL program from the shaders
            shader = new Shader("src/tube-traveller/shaders/SingleTexture.vert",
                                "src/tube-traveller/shaders/SingleTexture.frag");
            // no light
            light = nullptr;
    }


    // -------------------------------------------------------------------------
    //                          Load Textures
    // -------------------------------------------------------------------------

    switch (textureType) {
        case TEXTURE_FILES_STATIC:
            texture = new StaticFiles(shader->programID);
            break;
        case TEXTURE_RAINBOW:
            texture = new Rainbow(shader->programID);
            break;
        case TEXTURE_BINARY: {
            Binary::TexProperties texProps;
            texProps.numBits = 12;              // width of texture
            texProps.height = 3;                // height of texture
            texProps.interpLinear = false;      // 1 for yes, 0 for nearest neighbor
            texProps.grayscale = false;         // render image in grayscale
            texture = new Binary(shader->programID, texProps);
            break;
        }
        case TEXTURE_NOISE: {
            Noise::TexProperties texProps;
            texProps.width = 128;               // width of noise texture
            texProps.height = 128;              // height of noise texture
            texProps.interpLinear = true;       // 1 for yes, 0 for nearest neighbor
            texProps.woodGrain = false;         // render "woodGrain" style
            texProps.mirrorTex = true;          // mirror tex around tube centerline
            texProps.grayscale = false;         // render grayscale noise
            texProps.threshVal = 0.0f;          // values below threshVal rendered black
            texture = new Noise(shader->programID, texProps);
            break;
        }
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
    if (light != nullptr) {
        light->update(path, cam);
    }

}

void TextureCylinderLight::draw() {

    // draw lamp
    if (light) {
        light->draw();
    }

    // use our shader (makes programID "currently bound" shader?)
    shader->use();

    // send light data to fragment shader
    if (light) {
        light->setUniforms();
    }

    // send data to vertex shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    for (int i = 0; i < texture->numTextures; ++i) {

        // bind texture to texture unit
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
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &rotationBufferID);
    glDeleteBuffers(1, &rotation2BufferID);

    for (int i = 0; i < texture->numTextures; ++i) {
        delete[] g_center_buffer_data[i];
        delete[] g_rotation_buffer_data[i];
        delete[] g_rotation2_buffer_data[i];
    }
    delete[] g_center_buffer_data;
    delete[] g_rotation_buffer_data;
    delete[] g_rotation2_buffer_data;

    texture->clean();
    delete texture;
    shader->clean();
    delete shader;
    if (light) {
        light->clean();
    }
    delete light;

}