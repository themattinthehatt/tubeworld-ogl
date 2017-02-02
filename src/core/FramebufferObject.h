//
// Created by mattw on 1/11/17.
//

#ifndef TUBEWORLD_FRAMEBUFFEROBJECT_H
#define TUBEWORLD_FRAMEBUFFEROBJECT_H


#include <GL/glew.h>
#include <glm/glm.hpp>

class FramebufferObject {

private:
public:

    // (multi-sample) framebuffer info
    GLuint framebufferID;
    GLuint fboColorTextureID;
    GLuint renderbufferID;

    // (intermediate) framebuffer info
    GLuint intFramebufferID;
    GLuint intFBOColorTextureID;
    GLuint intRenderbufferID;

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
    FramebufferObject(GLint numSamples);
    // transfer multi-sampled framebuffer to standard color buffer
    void transferMSFBO();
    // render framebuffer color attachment to screen
    void render();
    // clean up VAOs, VBOs, etc.
    void clean();

    // getters
    GLuint getFramebufferID() { return framebufferID; };

};


#endif //TUBEWORLD_FRAMEBUFFEROBJECT_H
