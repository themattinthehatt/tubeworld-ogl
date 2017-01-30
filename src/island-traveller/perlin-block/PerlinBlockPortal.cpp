//
// Created by mattw on 1/14/17.
//

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "PerlinBlockPortal.h"
#include "../../core/loaders/loadTextures.h"

PerlinBlockPortal::PerlinBlockPortal(glm::vec3 center_, glm::vec3 heading_,
                                     const char* fileLoc) {

    center = center_;
    heading = heading_;

    // create and compile our GLSL program from the shaders
    shader = new Shader("src/island-traveller/perlin-block/PortalShader.vert",
                        "src/island-traveller/perlin-block/PortalShader.frag");

    // -------------------------------------------------------------------------
    //                     Load portal image
    // -------------------------------------------------------------------------

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shader->programID, "loadedTexture");

    // load data
    int width, height;
    unsigned char *image = loadBMP(fileLoc, &width, &height);

    for (int i = 0; i < height*width; ++i) {
        image[i] = 0;
    }
    // generate 1 texture ID, put the resutling identifier in textureID
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
    // commands will configure the currently bound texture
    glBindTexture(GL_TEXTURE_2D, textureID);

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

    // -------------------------------------------------------------------------
    //                      Initialize portal uniforms
    // -------------------------------------------------------------------------

    // vertex shader uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shader->programID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shader->programID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shader->programID, "mvpMatrix");

    // scale portal
    glm::mat4 scaleMat = glm::scale(glm::vec3(1.f, 1.f, 1.f));

    // translate portal
    glm::mat4 translateMat = glm::translate(center);


    // get desired angle of portal
    GLfloat horzAngle;
    GLfloat vertAngle;

    if (heading.x == 0.f && heading.y == 0.f) {
        // heading is pointing up; no need to calculate horizontal angle
        horzAngle = 0.f;
    } else {
        horzAngle = glm::orientedAngle(glm::vec2(0.f, 1.f), glm::normalize(
                                       glm::vec2(heading.x, heading.y)));
    }
    // rotate portal horizontally around +z axis
    glm::mat4 rotateHorzMat = glm::rotate(horzAngle, glm::vec3(0.f, 0.f, 1.f));
    glm::vec3 tempHeading = glm::vec3(rotateHorzMat * glm::vec4(heading, 1.f));

    if (tempHeading.z == 0) {
        // heading is pointing sideways; no need to calculate vertical angle
        vertAngle = 0.f;
    } else {
        GLfloat tempXY = glm::length(glm::vec2(tempHeading.x, tempHeading.y));
        vertAngle = glm::orientedAngle(glm::vec2(1.f, 0.f), glm::normalize(
                                       glm::vec2(tempXY, tempHeading.z)));
    }
    // rotate portal vertically around +x that has been rotated horizontally
    // according to previous transformation
    glm::mat4 rotateVertMat = glm::rotate(vertAngle,
                     glm::vec3(rotateHorzMat * glm::vec4(1.f, 0.f, 0.f, 0.f)));

    // combine all transformations
    mMatrix = translateMat * rotateVertMat * rotateHorzMat * scaleMat * mMatrix;

    // -------------------------------------------------------------------------
    //                     Create portal VAO
    // -------------------------------------------------------------------------

    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers

    glm::vec3 portalCoords[] = {glm::vec3(1.f, 0.f, 1.f),
                                glm::vec3(1.f, 0.f, -1.f),
                                glm::vec3(-1.f, 0.f, 1.f),
                                glm::vec3(1.f, 0.f, -1.f),
                                glm::vec3(-1.f, 0.f, -1.f),
                                glm::vec3(-1.f, 0.f, 1.f)};
    numVertices = 6; //sizeof(portalCoords);
    glm::vec3 portalNormals[] = {glm::vec3(0.f, 1.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f)};
    glm::vec2 portalUVs[] = {glm::vec2(1.f, 1.f),
                             glm::vec2(1.f, 0.f),
                             glm::vec2(0.f, 1.f),
                             glm::vec2(1.f, 0.f),
                             glm::vec2(0.f, 0.f),
                             glm::vec2(0.f, 1.f)};

    // PORTAL COORDINATES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &portalCoords[0], GL_STATIC_DRAW);

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


    // PORTAL NORMALS
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &portalNormals[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // PORTAL UV COORDINATES
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVertices,
                 &portalUVs[0], GL_STATIC_DRAW);

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

}

void PerlinBlockPortal::update(Camera cam, Player player) {

    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

}

void PerlinBlockPortal::draw() {

    // use our shader
    shader->use();

    // send data to vertex shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureID);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, 0);

    // bind vertex array
    glBindVertexArray(vertexArrayID);
    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    // break vertex array object binding
    glBindVertexArray(0);

    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, 0);

}

void PerlinBlockPortal::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteTextures(1, &textureID);

    shader->clean();
    delete shader;

}