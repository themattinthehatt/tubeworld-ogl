//
// Created by mattw on 1/11/17.
//

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>         // for monitor stuff
#include "FramebufferObject.h"

FramebufferObject::FramebufferObject() {

    // get screen info
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    GLFWmonitor *useMonitor;
    int monitorID = 0;
    if (monitorID < count) {
        useMonitor = monitors[monitorID];
    } else useMonitor = monitors[0];
    const GLFWvidmode *mode = glfwGetVideoMode(useMonitor);

    // create framebuffer for off-screen rendering
    glGenFramebuffers(1, &framebufferID);               // create framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);   // bind framebuffer

    // create a texture for the color buffer
    glGenTextures(1, &fboColorTextureID);
    glBindTexture(GL_TEXTURE_2D, fboColorTextureID);
    // send texture data to texture unit (just initializing here)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mode->width, mode->height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);            // unbind texture

    // attach the texture to the color attachment of the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fboColorTextureID, 0);

    // create a render buffer for depth and stencil testing
    glGenRenderbuffers(1, &renderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          mode->width, mode->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);     // unbind render buffer

    // attach the renderbuffer to the depth and stencil attachments of framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, renderbufferID);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!"
                  << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind frame buffer

    // -------------------------------------------------------------------------
    //                     Create post-processing VAO
    // -------------------------------------------------------------------------

    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &screenVAOID);
    // use a VAO by binding it
    glBindVertexArray(screenVAOID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // SCREEN QUAD COORDINATES
    glm::vec2 screenQuadCoords[] = {glm::vec2(-1.f, 1.f),
                                    glm::vec2(-1.f, -1.f),
                                    glm::vec2(1.f, -1.f),
                                    glm::vec2(-1.f, 1.f),
                                    glm::vec2(1.f, -1.f),
                                    glm::vec2(1.f, 1.f)};
    numScreenVertices = sizeof(screenQuadCoords);
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &screenVertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, screenVertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numScreenVertices,
                 &screenQuadCoords[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            2,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // SCREEN QUAD UV COORDINATES
    glm::vec2 screenQuadUVs[] = {glm::vec2(0.f, 1.f),
                                 glm::vec2(0.f, 0.f),
                                 glm::vec2(1.f, 0.f),
                                 glm::vec2(0.f, 1.f),
                                 glm::vec2(1.f, 0.f),
                                 glm::vec2(1.f, 1.f)};
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &screenUVBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, screenUVBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numScreenVertices,
                 &screenQuadUVs[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
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

};

void FramebufferObject::render() {

    /* calling function should:
     * bind default framebuffer
     * clear variouos buffer bits
     * bind post-processing shader program
     * send shaders any updated uniforms
     */

    // bind vertex array
    glBindVertexArray(screenVAOID);
    // bind texture
    glBindTexture(GL_TEXTURE_2D, fboColorTextureID);
    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numScreenVertices);
    // break vertex array object binding
    glBindVertexArray(0);

};


void FramebufferObject::clean() {

    glDeleteFramebuffers(1, &framebufferID);
    glDeleteTextures(1, &fboColorTextureID);
    glDeleteRenderbuffers(1, &renderbufferID);
    glDeleteVertexArrays(1, &screenVAOID);
    glDeleteBuffers(1, &screenVertexBufferID);
    glDeleteBuffers(1, &screenUVBufferID);

};