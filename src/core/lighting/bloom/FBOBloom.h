//
// Created by mattw on 2/8/17.
//

#ifndef TUBEWORLD_FBOBLOOM_H
#define TUBEWORLD_FBOBLOOM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../../Shader.h"

class FBOBloom {

private:
public:

    // (multi-sample) framebuffer info; one color attach for color, one for blur
    GLuint framebufferID;
    GLuint fboColorTextureID[2];
    GLuint renderbufferID;

    // (intermediate) framebuffer info
    GLuint intFramebufferID;
    GLuint intFBOColorTextureID[2];
    GLuint intRenderbufferID;

    // ping-pong framebuffer info
    GLuint pingPongFBOID[2];
    GLuint pingPongFBOColorTexID[2];
    Shader *blurShader;
    bool horizontal;
    int iters; // number of iterations in ping-pong fbo

    // screen quad info
    GLuint screenVAOID;
    GLuint screenVertexBufferID;
    GLuint screenUVBufferID;
    GLint numScreenVertices;

    // screen info
    GLint screenWidth;
    GLint screenHeight;

    bool multiSamplingEnabled;

    // constructor
    FBOBloom(GLint numSamples);
    // transfer multi-sampled framebuffer to standard color buffer
    void transferMSFBO();
    // render framebuffer color attachment to screen
    void render();
    // blur output
    void blur();
    // clean up VAOs, VBOs, etc.
    void clean();

    // getters
    GLuint getFramebufferID() { return framebufferID; };

};


#endif //TUBEWORLD_FBOBLOOM_H
