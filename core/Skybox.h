//
// Created by mattw on 8/14/16.
//

#ifndef PLAYGROUND_SKYBOX_H
#define PLAYGROUND_SKYBOX_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Camera.h"

class Skybox {

private:
    GLfloat multiplier;         // half-width of skybox
    GLuint numVertices = 36;    // number of vertices in a cube
    static GLfloat skyboxModelCoordinates[];

public:
    GLuint vertexArrayID;       // ID of VAO
    GLuint vertexBufferID;      // ID of VBO
    GLuint texture;             // ID of texture (for binding)
    GLuint textureID;           // ID of texture (for cube sampler)
    GLuint shaderID;            // ID of shader program
    glm::mat4 viewMatrix;       // view matrix
    GLuint viewMatrixID;        // ID of view matrix for shaders

    // constructor
    Skybox(std::vector<const GLchar*> files_, GLfloat multiplier_);
    // destructor
    ~Skybox();
    // update skybox position
    void update(const Camera &cam);
    // draw skybox
    void draw();
    // clear up VAOs, VBOs, etc.
    void clean();

    // external function to load BMPs as a cubemap
    static GLuint loadCubemap(std::vector<const GLchar*> files);
};


#endif //PLAYGROUND_SKYBOX_H