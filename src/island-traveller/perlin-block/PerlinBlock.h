//
// Created by mattw on 1/8/17.
//

#ifndef TUBEWORLD_PERLINBLOCK_H
#define TUBEWORLD_PERLINBLOCK_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../../core/Skybox.h"
#include "../../core/Shader.h"
#include "../Island.h"

class PerlinBlock : public Island {

private:


public:

    // Island.h contains parameters for fading

    // framebuffer info
    GLuint framebufferID;
    GLuint fboColorTextureID;
    GLuint renderbufferID;
    GLuint screenVAOID;
    GLuint screenVertexBufferID;
    GLuint screenUVBufferID;
    GLint numScreenVertices;

    Skybox *skybox;

    std::vector<glm::vec3> islandCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    Shader *shader;
    Shader *postShader;
//    Light *light;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLfloat time;
    GLint timeParamID;

    IOHandler &io;

    // constructor
    PerlinBlock();
    // update dynamics of island
    GLint update(Camera &cam, Player &player);
    // draw island to screen
    void draw();
    // draw island to framebuffer
    void renderOffscreen();
    // play with post-processing effects
    void postProcess();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_PERLINBLOCK_H
