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

    // framebuffer info
    GLuint framebufferID;
    GLuint fboColorTextureID;
    GLuint renderbufferID;
    GLuint screenVAOID;
    GLuint screenVertexBufferID;
    GLuint screenUVBufferID;
    GLint numScreenVertices;

    // constructor
    FramebufferObject();
    // render framebuffer color attachment to screen
    void render();
    // clean up VAOs, VBOs, etc.
    void clean();

    // getters
    GLuint getFramebufferID() { return framebufferID; };

};


#endif //TUBEWORLD_FRAMEBUFFEROBJECT_H
