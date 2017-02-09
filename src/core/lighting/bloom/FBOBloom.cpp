//
// Created by mattw on 2/8/17.
//

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>         // for monitor stuff
#include "FBOBloom.h"

FBOBloom::FBOBloom(GLint numSamples) {

    // get screen info
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    GLFWmonitor *useMonitor;
    int monitorID = 0;
    if (monitorID < count) {
        useMonitor = monitors[monitorID];
    } else useMonitor = monitors[0];
    const GLFWvidmode *mode = glfwGetVideoMode(useMonitor);
    screenWidth = mode->width;
    screenHeight = mode->height;

    // -------------------------------------------------------------------------
    //                     Create framebuffer
    // -------------------------------------------------------------------------

    // create framebuffer for off-screen rendering
    glGenFramebuffers(1, &framebufferID);               // create framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);   // bind framebuffer

    // decide whether or not to enable multi-sampling
    if (numSamples > 1) {
        multiSamplingEnabled = true;

        // create multiple textures for the color buffer
        glGenTextures(2, fboColorTextureID);
        for (int i = 0; i < 2; ++i) {
            // create multi-sampled texture
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboColorTextureID[i]);
            // send texture data to texture unit (just initializing here)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples,
                                    GL_RGBA16F, screenWidth, screenHeight,
                                    GL_TRUE);
            // Set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); // unbind texture
            // attach the texture to the color attachment of the framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D_MULTISAMPLE,
                                   fboColorTextureID[i], 0);

        }

        // create a render buffer for depth and stencil testing
        glGenRenderbuffers(1, &renderbufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples,
                                         GL_DEPTH24_STENCIL8,
                                         screenWidth, screenHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);     // unbind render buffer

        // attach the renderbuffer to the depth and stencil attachments of framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, renderbufferID);

    } else {
        multiSamplingEnabled = false;

        // create multiple textures for the color buffer
        glGenTextures(2, fboColorTextureID);
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, fboColorTextureID[i]);
            // send texture data to texture unit (just initializing here)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth,
                             screenHeight, 0,
                             GL_RGBA,
                             GL_FLOAT, NULL);

            // Set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);             // unbind texture
            // attach the texture to the color attachment of the framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D,
                                   fboColorTextureID[i], 0);
        }

        // create a render buffer for depth and stencil testing
        glGenRenderbuffers(1, &renderbufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                              screenWidth, screenHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);     // unbind render buffer

        // attach the renderbuffer to the depth and stencil attachments of framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, renderbufferID);
    }

    // specify which color attachments to use (of this framebuffer) for rendering
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!"
                  << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind frame buffer

    // -------------------------------------------------------------------------
    //                     Create intermediate framebuffer
    // -------------------------------------------------------------------------

    // create intermediate framebuffer for transferring multi-sampled off-screen
    // rendering to a downsampled texture
    if (multiSamplingEnabled) {
        glGenFramebuffers(1, &intFramebufferID);               // create framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, intFramebufferID);   // bind framebuffer

        // create multiple textures for the color buffer
        glGenTextures(2, intFBOColorTextureID);
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, intFBOColorTextureID[i]);
            // send texture data to texture unit (just initializing here)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            // Set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);             // unbind texture
            // attach the texture to the color attachment of the framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D,
                                   intFBOColorTextureID[i], 0);
        }

        // create a render buffer for depth and stencil testing
        glGenRenderbuffers(1, &intRenderbufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, intRenderbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                              screenWidth, screenHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);     // unbind render buffer

        // attach the renderbuffer to the depth and stencil attachments of framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, intRenderbufferID);


        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!"
                      << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind frame buffer
    }

    // -------------------------------------------------------------------------
    //                     Create ping-pong framebuffer
    // -------------------------------------------------------------------------

    // create ping-pong framebuffers for applying blur kernel
    for (int i = 0; i < 2; ++i) {
        glGenFramebuffers(1, &pingPongFBOID[i]);             // create framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBOID[i]); // bind framebuffer

        // create a texture for the ping-pong buffer
        glGenTextures(1, &pingPongFBOColorTexID[i]);
        glBindTexture(GL_TEXTURE_2D, pingPongFBOColorTexID[i]);
        // send texture data to texture unit (just initializing here)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0,
                     GL_RGB,
                     GL_FLOAT, NULL);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach the texture to the color attachment of the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               pingPongFBOColorTexID[i], 0);
        glBindTexture(GL_TEXTURE_2D, 0);             // unbind texture

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!"
                      << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind frame buffer
    }

    // create blur shader
    blurShader = new Shader("src/core/lighting/bloom/GaussianBlur.vert",
                            "src/core/lighting/bloom/GaussianBlur.frag");

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

}

void FBOBloom::transferMSFBO() {

    if (multiSamplingEnabled) {

        // blit first color att
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intFramebufferID);
        // now all subsequent rendering operations will render from the
        // multisampled framebuffer to the intermediate framebuffer

        // blit color attachments in FBO to color attachment in int FBO
        for (int i = 0; i < 2; ++i) {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            glBlitFramebuffer(0, 0, screenWidth, screenHeight,
                              0, 0, screenWidth, screenHeight,
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        // now scene is stored in 2D texture image, so use that image for
        // post-processing
    }

}

void FBOBloom::blur() {

    // begin blur in horizontal direction
    horizontal = true;
    bool firstIter = true;
    iters = 6;
    blurShader->use();

    // make blur passes
    for (int i = 0; i < iters; ++i) {

        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBOID[horizontal]);
        // send over horizontal uniform
        glUniform1i(glGetUniformLocation(blurShader->programID, "horizontal"),
                    horizontal);
        // bind texture
        if (multiSamplingEnabled) {
            glBindTexture(GL_TEXTURE_2D, firstIter ? intFBOColorTextureID[1] :
                                         pingPongFBOColorTexID[!horizontal]);
        } else {
            glBindTexture(GL_TEXTURE_2D, firstIter ? fboColorTextureID[1] :
                                         pingPongFBOColorTexID[!horizontal]);
        }

        // render quad
        glBindVertexArray(screenVAOID);
        glDrawArrays(GL_TRIANGLES, 0, numScreenVertices);
        glBindVertexArray(0);

        // switch blur direction for next pass
        horizontal = !horizontal;

        if (firstIter) {
            firstIter = false;
        }

    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FBOBloom::render() {

    /* calling function should:
     * bind default framebuffer
     * clear various buffer bits
     * bind post-processing shader program
     * send shaders any updated uniforms
     */

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // now all subsequent rendering operations will render to the attachments of
    // the default framebuffer, and as such will be rendered to the scene
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // set polygon mode so that the whole framebuffer is rendered
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // bind vertex array
    glBindVertexArray(screenVAOID);

    // bind the HDR texture
    if (multiSamplingEnabled) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, intFBOColorTextureID[0]);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboColorTextureID[0]);
    }

    // bind the blur texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongFBOColorTexID[!horizontal]);

    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numScreenVertices);
    // break vertex array object binding
    glBindVertexArray(0);

}


void FBOBloom::clean() {

    glDeleteFramebuffers(1, &framebufferID);
    glDeleteTextures(2, fboColorTextureID);
    glDeleteRenderbuffers(1, &renderbufferID);

    glDeleteFramebuffers(1, &intFramebufferID);
    glDeleteTextures(2, intFBOColorTextureID);
    glDeleteRenderbuffers(1, &intRenderbufferID);

    glDeleteFramebuffers(2, pingPongFBOID);
    glDeleteTextures(2, pingPongFBOColorTexID);
    blurShader->clean();
    delete blurShader;

    glDeleteVertexArrays(1, &screenVAOID);
    glDeleteBuffers(1, &screenVertexBufferID);
    glDeleteBuffers(1, &screenUVBufferID);

}